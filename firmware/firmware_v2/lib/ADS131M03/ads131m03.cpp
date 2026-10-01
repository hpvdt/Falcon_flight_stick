#include "ads131m03.hpp"
#include "ads131m03_reg.hpp"
#include <Arduino.h>
#include <SPI.h>

const char TAG_ADS[] = "ADS131";

/**
 * \brief SPI settings for the ADC
 * 
 * \note ADC can handle 25 MHz SPI, MSB first, CPOL = 0, CPHA = 1
 */
static const SPISettings SPI_SETTINGS = SPISettings(10000000, MSBFIRST, SPI_MODE1);

static const int ADS_TIMEOUT_MS = 10;

void ADS131M03::begin(SPIClass* ads_bus, int8_t cs_pin, int8_t data_ready_pin, int8_t reset_sync_pin) {
    CS_PIN = cs_pin;
    RESET_SYNC_PIN = reset_sync_pin;
    DATA_READY_PIN = data_ready_pin;
    bus = ads_bus;

    if (bus == NULL) {
        ESP_LOGE(TAG_ADS, "Provided null pointer for bus. Expect issues.");
    }

    // Setup SPI interface pins to drive and into default states
    pinMode(CS_PIN, OUTPUT); // Enabling this seems to cause LED issues, leaving it as is is ok
    digitalWrite(CS_PIN, HIGH);
    
    if (RESET_SYNC_PIN != -1) {
        pinMode(RESET_SYNC_PIN, OUTPUT);
        digitalWrite(RESET_SYNC_PIN, HIGH);
    }
    if (DATA_READY_PIN != -1) {
        pinMode(DATA_READY_PIN, INPUT);
    }

    // Let chip start and reset
    delay(10); 
    reset();
    delay(10);
}

void ADS131M03::reset() {
    if (RESET_SYNC_PIN != -1) {
        digitalWrite(RESET_SYNC_PIN, LOW);
        delay(5); // Needs to be longer than 2048 pulses of MCLK
        digitalWrite(RESET_SYNC_PIN, HIGH);
    }
    else {
        issue_command(ADS_CMD_RESET);
        delay(5);
    }
}

uint16_t ADS131M03::issue_command(uint16_t command) {
    uint16_t response = 0;
    uint16_t calculatedCRC = calculate_crc(command, nullptr, 0);

    bus->beginTransaction(SPI_SETTINGS);
    digitalWrite(CS_PIN, LOW);
    transfer_single_word(command, false);
    // Update the channel data with the most recent data available on each channel
    // Not applicable when issuing a reset command
    if ((command != ADS_CMD_RESET)) {
        current_readings[0] = transfer_single_word(calculatedCRC, true);
        current_readings[1] = transfer_single_word(0, true);
        current_readings[2] = transfer_single_word(0, true);
    }
    else {
        transfer_single_word(calculatedCRC, false);
        transfer_single_word(0, false);
        transfer_single_word(0, false);
    }
    transfer_single_word(0, false); // Not bothering with CRC for returns from ADS131 (yet)

    digitalWrite(CS_PIN, HIGH);
    bus->endTransaction();

    return response;
    }

void ADS131M03::update_readings() {
    // Channels are sent in response to any short command, so using NULL to get them
    issue_command(ADS_CMD_NULL);
}

uint16_t ADS131M03::calculate_crc(uint16_t command, uint16_t payload[], size_t length) {
	if (!chip_mode_reg.spi_crc_enable) return 0;

	(void) command;
	(void) payload;
	(void) length;

	if (chip_mode_reg.crc_type == ADS_CRC_TYPE_16_BIT_CCITT) {
		// x^16 + x^12 + x^5 + 1
	}
	else {
		// 16-bit ANSI calculation
		// x^16 + x^15 + x^2 + 1
	}

	return 0;
}

void ADS131M03::write_to_reg(ADSRegisterAddress reg, uint8_t num_regs, uint16_t *source) {
    uint16_t writeCommandToIssue = ADSCommands::ADS_CMD_WREG + (reg << 7) + (num_regs - 1);

    /* Writting to registers is a bit messy

    Writting is kind of like a normal operation where the ADC values are returned with a CRC, 
    so at least 4 words are exchanged in addition to the command.

    However depending on the amount of writing done there may not be enough registers to write
    to complete the reads so some transfers need to be done with no data to complete.

    Conversely if there are for than four registers write, it will extend beyond the returned
    data so that needs to be handled as well.

    Appended to any transfer into the ADC is a CRC word, which is currently not implemented
    nor expected by the ADC by default.
    */

    uint8_t regSent = 0; // Used to keep track of registers sent between stages
    uint16_t calculatedCRC = calculate_crc(reg, source, num_regs);

    bus->beginTransaction(SPI_SETTINGS);
    digitalWrite(CS_PIN, LOW);

    transfer_single_word(writeCommandToIssue, false); 

    // Read ADCs
    for (; regSent < 3; regSent++) {
        // Send register values if available, CRC on trailing if needed
        if (regSent < num_regs) current_readings[regSent] = transfer_single_word(source[regSent], true);
        else if (regSent == num_regs) current_readings[regSent] = transfer_single_word(calculatedCRC, true);
        else current_readings[regSent] = transfer_single_word(0, true);
    }

    // Read in CRC from ADC
    uint16_t receivedCRC = 0;
    if (regSent < num_regs) receivedCRC = transfer_single_word(source[regSent], false);
    else if (regSent == num_regs) receivedCRC = transfer_single_word(calculatedCRC, false);
    else receivedCRC = transfer_single_word(0, false);
    regSent++;

    // Send remaining registers. Data from ADS131M03 is invalid at this point
    for (; regSent < num_regs; regSent++) transfer_single_word(source[regSent], false);
    
    // Append CRC if needed to exchanges longer than the standard message
    if ((chip_mode_reg.spi_crc_enable == true) && (num_regs > 4)) transfer_single_word(calculatedCRC, false);

    digitalWrite(CS_PIN, HIGH);
    bus->endTransaction();
}

void ADS131M03::read_from_reg(ADSRegisterAddress reg, uint8_t num_regs, uint16_t *destination) {
    
    uint16_t readCommandToIssue = ADS_CMD_RREG + (reg << 7) + (num_regs - 1);

    issue_command(readCommandToIssue);

    // Read in the register(s)
    if (num_regs == 1) *destination = issue_command(ADS_CMD_NULL); // For a single register the register is returned as a response to the subsequent command
    else {
        // For multi-register reads, they are read in as a series appended with CRC

        bus->beginTransaction(SPI_SETTINGS);
        digitalWrite(CS_PIN, LOW);
        uint16_t calculatedCRC = calculate_crc(ADS_CMD_NULL, NULL, 0);

        transfer_single_word(ADS_CMD_NULL, false); // First response is an acknowledgement, discarded

        // Transfer CRC in exchange for first register
        destination[0] = transfer_single_word(calculatedCRC, false);
        for (uint8_t i = 1; i < num_regs; i++) destination[i] = transfer_single_word(0, false);
        
        transfer_single_word(0, false); // Discard CRC from ADS131M03

        digitalWrite(CS_PIN, HIGH);
        bus->endTransaction();
    }
}

int32_t ADS131M03::transfer_single_word(uint16_t value, bool is_adc_reading) {
    int32_t response = 0;

    if (is_adc_reading == false) { // Non-ADC reading exchange
        // If not an ADC reading then the first two bytes exchanged are the value and response
        response = bus->transfer16(value);

        // Pad with trailing zeros as needed by word size
        if (wordStyle == ADS_WORD_NORMAL) bus->transfer(0);
        else if ((wordStyle == ADS_WORD_ZERO_PAD) || (wordStyle == ADS_WORD_SIGN_PAD)) bus->transfer16(0);

        // Do not extend sign for response if not an ADC reading! 
        // That would mess up casting to unsigned integers often used for registers.
    }
    else { // Read in ADC reading, extend sign using Two's compliment logic
        
        if (wordStyle == ADS_WORD_NORMAL) {
        response = bus->transfer16(value);
        response = (response << 8) + bus->transfer(0);
        
        // Extend sign if needed (1 at 24th bit)
        if (response & 0x00800000) response |= 0xFF000000;
        }
        else if (wordStyle == ADS_WORD_SHORT) {
        response = bus->transfer16(value);
        
        // Extend sign if needed (1 at 16th bit)
        if (response & 0x00008000) response |= 0xFFFF0000;
        }
        else if (wordStyle == ADS_WORD_ZERO_PAD) {
        response = bus->transfer16(value);
        response = (response << 8) + (bus->transfer16(0) >>  8);
        
        // Extend sign if needed (1 at 24th bit)
        if (response & 0x00800000) response |= 0xFF000000;
        }
        else {
        response = bus->transfer16(value);
        response = (response << 16) | bus->transfer16(0);
    
        // Sign already extended in SIGN_PAD
        }
    }

    return response;
}

void ADS131M03::record_readings(int32_t* destination) {
    noInterrupts(); // Atomic read of ADC values
    for (uint8_t i = 0; i < 3; i++) destination[i] = current_readings[i];
    interrupts();
}

void ADS131M03::update_config_regs() {
	// Since the main registers are sequential we can send them in a single burst
	uint16_t buffer[3];
	buffer[0] = *(uint16_t*) &chip_mode_reg;
	buffer[1] = *(uint16_t*) &chip_clock_reg;
	buffer[2] = *(uint16_t*) &chip_gain_reg;
	write_to_reg(ADS_REG_MODE, 3, buffer);

	// Clear the two most recent measurements since they won't reflect these settings
	issue_command(ADS_CMD_NULL);
	issue_command(ADS_CMD_NULL);
}

void ADS131M03::configure_channel(enum ADSChannel channel, enum ADSPGAGain gain, enum ADSOSRRatio osr) {
    switch (channel) {
	case ADS_CHANNEL_0:
		chip_gain_reg.channel_0_gain = gain;
		chip_clock_reg.osr = osr;
		break;
	case ADS_CHANNEL_1:
		chip_gain_reg.channel_1_gain = gain;
		chip_clock_reg.osr = osr;
		break;
	case ADS_CHANNEL_2:
		chip_gain_reg.channel_2_gain = gain;
		chip_clock_reg.osr = osr;
		break;
	default:
		return;
	}

	update_config_regs();
}
