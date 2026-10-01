#ifndef ADS131M03_HEADER
#define ADS131M03_HEADER

#include <Arduino.h>
#include <SPI.h>
#include "ads131m03_reg.hpp"

enum ADSWordFormat : uint8_t {
    ADS_WORD_SHORT    = 0b00,
    ADS_WORD_NORMAL   = 0b01,
    ADS_WORD_ZERO_PAD = 0b10,
    ADS_WORD_SIGN_PAD = 0b11
};

enum ADSDataReadyActiveFormat {
	ADS_DRDY_ACTIVE_FRMT_LOW 	= 0,
	ADS_DRDY_ACTIVE_FRMT_PULSE 	= 1,
};

enum ADSDataReadyIdleFormat {
	ADS_DRDY_IDLE_FRMT_HIGH 	= 0,
	ADS_DRDY_IDLE_FRMT_HIZ	 	= 1,
};

enum ADSDataReadySource {
	ADS_DRDY_SRC_LAGGING = 0,
	ADS_DRDY_SRC_ANY = 1,
	ADS_DRDY_SRC_LEADING = 2,
};

enum ADSCRCType {
	ADS_CRC_TYPE_16_BIT_CCITT 	= 0, // x^16 + x^12 + x^5 + 1
	ADS_CRC_TYPE_16_BIT_ANSI	= 1, // x^16 + x^15 + x^2 + 1
};

struct ADSModeRegister {
	enum ADSDataReadyActiveFormat data_ready_available_format : 1;
	enum ADSDataReadyIdleFormat data_ready_unavailable_format : 1;
	enum ADSDataReadySource data_ready_source : 2;
	bool spi_timeout_enabled : 1;
	uint8_t RESERVE_0_A : 3; // Always write 0
	enum ADSWordFormat word_format : 2;
	bool reset : 1; // Write a zero to clear bit in status register
	enum ADSCRCType crc_type : 1;
	bool spi_crc_enable : 1;
	bool register_map_crc_enable : 1;
	uint8_t RESERVE_0_B : 2; // Write zero
};

enum ADSPowerMode {
	ADS_POWER_VERY_LOW = 0,
	ADS_POWER_LOW = 1,
	ADS_POWER_HIGH_RES = 2,
};

enum ADSOSRRatio {
	ADS_OSR_RATIO_128 = 0,
	ADS_OSR_RATIO_256 = 1,
	ADS_OSR_RATIO_512 = 2,
	ADS_OSR_RATIO_1024 = 3,
	ADS_OSR_RATIO_2048 = 4,
	ADS_OSR_RATIO_4096 = 5,
	ADS_OSR_RATIO_8192 = 6,
	ADS_OSR_RATIO_16256 = 7
};

struct ADSClockRegister {
	enum ADSPowerMode power_mode : 2;
	enum ADSOSRRatio osr : 3;
	bool turbo_mode_osr_64 : 1; // Engage OSR of 64, overrides OSR if enabled
	uint8_t RESERVED_0_A : 2; // Write 0
	bool enable_channel_0 : 1;
	bool enable_channel_1 : 1;
	bool enable_channel_2 : 1;
	uint8_t RESERVED_0_B : 5; // Write 0
};

enum ADSPGAGain {
	ADS_PGA_GAIN_1 = 0,
	ADS_PGA_GAIN_2 = 1,
	ADS_PGA_GAIN_4 = 2,
	ADS_PGA_GAIN_8 = 3,
	ADS_PGA_GAIN_16 = 4,
	ADS_PGA_GAIN_32 = 5,
	ADS_PGA_GAIN_64 = 6,
	ADS_PGA_GAIN_128 = 7,
};

struct ADSGainRegister {
	enum ADSPGAGain channel_0_gain : 3;
	uint8_t RESERVE_0_A : 1; // Write 0
	enum ADSPGAGain channel_1_gain : 3;
	uint8_t RESERVE_0_B : 1; // Write 0
	enum ADSPGAGain channel_2_gain : 3;
	uint8_t RESERVE_0_C : 5; // Write 0
};

enum ADSChannel {
	ADS_CHANNEL_0 = 0,
	ADS_CHANNEL_1 = 1,
	ADS_CHANNEL_2 = 2,
};

class ADS131M03 {
public:

    /**
     * \brief Initializes communication with the ADS131M03 chip
     * 
     * \param ads_bus Reference to SPI bus object
     * \param cs_pin ADS's chip select pin
     * \param data_ready_pin ADS's data ready pin (-1 if unused)
     * \param reset_sync_pin ADS's reset/sync pin (-1 if unused)
     * 
     * \warning Assumes that the SPI bus has already been properly started before this is called
     */
    void begin(SPIClass* ads_bus, int8_t cs_pin, int8_t data_ready_pin, int8_t reset_sync_pin);

    /**
     * \brief Resets the ADS131M03 chip
     *
     * \note Uses the SYNC/RESET pin if available, otherwise the RESET command
     */
    void reset();

    /**
     * \brief Copies most recent values read in from the ADC into this object to an array
     * 
     * \param destination Destination array for readings
     */
    void record_readings(int32_t* destination);

    /**
     * \brief Triggers a reading of the ADS131M03 to the object
     * 
     */
    void update_readings();
    
    /**
     * \brief Configure a given input channel
     * 
     * \param channel Targetted channel
     * \param gain Desired gain factor to use
     * \param osr Desired oversampling rate (note this is shared accross all channels)
     */
    void configure_channel(enum ADSChannel channel, enum ADSPGAGain gain, enum ADSOSRRatio osr);

private:
    SPIClass* bus;
    int8_t CS_PIN;
    int8_t RESET_SYNC_PIN;
    int8_t DATA_READY_PIN;

    volatile int32_t current_readings[3] = {0,0,0};

    struct ADSModeRegister chip_mode_reg = {
        .data_ready_available_format = ADS_DRDY_ACTIVE_FRMT_LOW,
        .data_ready_unavailable_format = ADS_DRDY_IDLE_FRMT_HIGH,
        .data_ready_source = ADS_DRDY_SRC_LAGGING,
        .spi_timeout_enabled = true,
        .RESERVE_0_A = 0,
        .word_format = ADS_WORD_NORMAL,
        .reset = 0,
        .crc_type = ADS_CRC_TYPE_16_BIT_CCITT,
        .spi_crc_enable = false,
        .register_map_crc_enable = false,
        .RESERVE_0_B = 0,
    };

    struct ADSClockRegister chip_clock_reg = {
        .power_mode = ADS_POWER_HIGH_RES,
        .osr = ADS_OSR_RATIO_1024,
        .turbo_mode_osr_64 = 0,
        .RESERVED_0_A = 0,
        .enable_channel_0 = true,
        .enable_channel_1 = true,
        .enable_channel_2 = true,
        .RESERVED_0_B = 0,
    };

    struct ADSGainRegister chip_gain_reg = {
        .channel_0_gain = ADS_PGA_GAIN_1,
        .RESERVE_0_A = 0,
        .channel_1_gain = ADS_PGA_GAIN_1,
        .RESERVE_0_B = 0,
        .channel_2_gain = ADS_PGA_GAIN_1,
        .RESERVE_0_C = 0,
    };

    /**
     * \brief Word style used for transactions 
     * 
     * \note Defaults to 24-bits
     * \warning SHOULD NOT BE CHANGED OTHER THAN BY CORRECT FUNCTION!
     */
    ADSWordFormat wordStyle = ADS_WORD_NORMAL;

    /**
     * \brief Issues a single (short) command and returns response of previous command
     * \note Note to be used for Writing to any number of registers or reading multiple registers (single reads are ok)
     *
     * \param command Command type to perform
     * \return Response to previous command
     */
    uint16_t issue_command(uint16_t command);

    /**
     * \brief Calculate CRC for messages
     * 
     * \param command The command being used
     * \param payload Payload buffer to process
     * \param length Length of payload in registers
     * 
     * \return Appropriate CRC value to use
     * 
     * \warning CURRENTLY NOT PROPERLY IMPLEMENTED!
     */
    uint16_t calculate_crc(uint16_t command, uint16_t payload[], size_t length);

    /**
     * \brief Writes a given number of registers from the ADS131M03 chip
     * 
     * \param reg Start register for write operation
     * \param num_reg Number of individual registers to write to
     * \param source Source buffer for new register data
     */
    void write_to_reg(ADSRegisterAddress reg, uint8_t num_reg, uint16_t *source);

    /**
     * \brief Reads a given number of registers from the ADS131M03 chip
     * 
     * \param reg Start register for read operation
     * \param num_reg Number of individual registers to read
     * \param destination Destination buffer for received data
     */
    void read_from_reg(ADSRegisterAddress reg, uint8_t num_reg, uint16_t *destination);

    /**
     * \brief Transfers a word with the ADS131M03
     * 
     * \param value Value to pass into ADS131M03
     * \param is_adc_reading Are we reading an ADC reading (defaults to false)
     * \return Response from ADS131M03 as 32 bit integer
     */
    int32_t transfer_single_word(uint16_t value, bool is_adc_reading  = false);

    /**
     * \brief Update the primary configuration registers on the ADS chip
     */
    void update_config_regs();
};

#endif