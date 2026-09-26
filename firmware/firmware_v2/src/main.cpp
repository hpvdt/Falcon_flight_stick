/**
 * Falcon Flight Stick firmware_v2
 * STM32F103CBU6: UART hello + peripheral status, then loop: poll ADC -> 16-bit -> serial + RGB.
 *
 * Pinout:
 *   UART:    PB6 (TX), PB7 (RX)  - USART1 remapped
 *   ADC SPI: PB13 (SCLK), PB14 (MISO), PB15 (MOSI), CS=PA9, RESET=PA10, DRDY=PB12, MCO=PA7
 *   EEPROM:  PB10 (SCL), PB11 (SDA) - I2C2
 *   RGB:     PB3 (Blue), PB4 (Red), PB5 (Green)
 */

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

// ---- UART (COM port via ST-Link) ----
#define UART_RX  PB7
#define UART_TX  PB6
HardwareSerial SerialUart(UART_RX, UART_TX);

// ---- ADC (ADS131M03) ----
#define ADC_CS    PA9
#define ADC_RST   PA10
#define ADC_DRDY  PB12
#define ADC_MCO   PA7
#define ADC_SCK   PB13
#define ADC_MISO  PB14
#define ADC_MOSI  PB15

SPIClass adc_spi(ADC_MOSI, ADC_MISO, ADC_SCK);

// ---- RGB LED ----
#define RGB_BLUE  PB3
#define RGB_RED   PB4
#define RGB_GREEN PB5

// ---- EEPROM I2C ----
#define EEPROM_SCL PB10
#define EEPROM_SDA PB11
#define EEPROM_ADDR 0x50

// ---- Timing ----
#define LOOP_DELAY_MS  2500

static void initUart(void);
static void initRgb(void);
static void initAdc(void);
static bool probeEeprom(void);
static void printStatus(bool adc_ok, bool eeprom_ok);
static void readAdcChannels(uint16_t* ch0, uint16_t* ch1, uint16_t* ch2);
static void setRgbFromAdc(uint16_t ch0, uint16_t ch1, uint16_t ch2);

void setup() {
  initUart();
  SerialUart.println("Hello");

  SerialUart.println("initRgb...");
  initRgb();
  SerialUart.println("initAdc...");
  initAdc();

  // Probe ADC: try one read (can block if ADC not connected / wrong wiring)
  SerialUart.println("readAdc...");
  uint16_t c0, c1, c2;
  readAdcChannels(&c0, &c1, &c2);
  bool adc_ok = (c0 != 0xFFFF && c1 != 0xFFFF && c2 != 0xFFFF) ||
                (c0 != 0x0000 && c1 != 0x0000 && c2 != 0x0000);  // avoid all 0 or all 1 as "no response"
  SerialUart.println("eeprom...");
  bool eeprom_ok = probeEeprom();

  printStatus(adc_ok, eeprom_ok);
}

void loop() {
  uint16_t ch0, ch1, ch2;
  readAdcChannels(&ch0, &ch1, &ch2);

  SerialUart.print("CH0=");
  SerialUart.print(ch0);
  SerialUart.print(" CH1=");
  SerialUart.print(ch1);
  SerialUart.print(" CH2=");
  SerialUart.println(ch2);

  setRgbFromAdc(ch0, ch1, ch2);
  delay(LOOP_DELAY_MS);
}

static void initUart(void) {
  SerialUart.begin(115200);
  while (!SerialUart)
    ;
}

static void initRgb(void) {
  pinMode(RGB_BLUE, OUTPUT);
  pinMode(RGB_RED, OUTPUT);
  pinMode(RGB_GREEN, OUTPUT);
  digitalWrite(RGB_BLUE, LOW);
  digitalWrite(RGB_RED, LOW);
  digitalWrite(RGB_GREEN, LOW);
}

static void initAdc(void) {
  pinMode(ADC_MCO, OUTPUT);
  RCC->CFGR &= ~RCC_CFGR_MCO;
  RCC->CFGR |= RCC_CFGR_MCO_SYSCLK;

  pinMode(ADC_CS, OUTPUT);
  pinMode(ADC_RST, OUTPUT);
  pinMode(ADC_DRDY, INPUT);
  digitalWrite(ADC_CS, HIGH);
  digitalWrite(ADC_RST, HIGH);

  digitalWrite(ADC_RST, LOW);
  delay(10);
  digitalWrite(ADC_RST, HIGH);
  delay(10);

  adc_spi.begin();
  delay(50);  // let ADC settle after reset before first SPI read
}

static bool probeEeprom(void) {
  Wire.begin((int)EEPROM_SDA, (int)EEPROM_SCL);
#if defined(WIRE_HAS_TIMEOUT)
  Wire.setWireTimeout(500, true);  // 500 us, reset on timeout so we don't hang
#endif
  Wire.beginTransmission((uint8_t)EEPROM_ADDR);
  uint8_t err = Wire.endTransmission();
  return (err == 0);
}

static void printStatus(bool adc_ok, bool eeprom_ok) {
  SerialUart.print("ADC: ");
  SerialUart.println(adc_ok ? "OK" : "FAIL");
  SerialUart.print("EEPROM: ");
  SerialUart.println(eeprom_ok ? "OK" : "FAIL/skip");
  SerialUart.println("RGB: OK");
  SerialUart.println("---");
}

/**
 * ADS131M03: 3 channels, 24-bit each. SPI frame: send null cmd (0x2000), read status + 3×24-bit.
 * We use 16-bit transfers: 1 status + 6 words for CH0/CH1/CH2 (each channel = 2×16-bit, 24-bit data).
 * 24-bit value = (high_word << 8) | (low_word >> 8). Normalize to 16-bit: take top 16 bits (>> 8).
 */
static void readAdcChannels(uint16_t* ch0, uint16_t* ch1, uint16_t* ch2) {
  digitalWrite(ADC_CS, LOW);
  delayMicroseconds(1);

  const uint16_t null_cmd = 0x2000;
  uint16_t w0 = adc_spi.transfer16(null_cmd);  // status
  uint16_t w1 = adc_spi.transfer16(null_cmd);  // CH0 high
  uint16_t w2 = adc_spi.transfer16(null_cmd);  // CH0 low
  uint16_t w3 = adc_spi.transfer16(null_cmd);  // CH1 high
  uint16_t w4 = adc_spi.transfer16(null_cmd);  // CH1 low
  uint16_t w5 = adc_spi.transfer16(null_cmd);  // CH2 high
  uint16_t w6 = adc_spi.transfer16(null_cmd);  // CH2 low

  digitalWrite(ADC_CS, HIGH);

  (void)w0;
  uint32_t v0_24 = ((uint32_t)w1 << 8) | (w2 >> 8);
  uint32_t v1_24 = ((uint32_t)w3 << 8) | (w4 >> 8);
  uint32_t v2_24 = ((uint32_t)w5 << 8) | (w6 >> 8);

  *ch0 = (uint16_t)(v0_24 >> 8);
  *ch1 = (uint16_t)(v1_24 >> 8);
  *ch2 = (uint16_t)(v2_24 >> 8);
}

/** CH0->R, CH1->G, CH2->B. Simple: above threshold (e.g. mid-scale) turn on, else off. */
static void setRgbFromAdc(uint16_t ch0, uint16_t ch1, uint16_t ch2) {
  const uint16_t thresh = 32768;
  digitalWrite(RGB_RED,   ch0 > thresh ? HIGH : LOW);
  digitalWrite(RGB_GREEN, ch1 > thresh ? HIGH : LOW);
  digitalWrite(RGB_BLUE,  ch2 > thresh ? HIGH : LOW);
}
