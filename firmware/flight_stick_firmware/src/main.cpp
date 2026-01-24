// This code has been built into submodules testing each peripheral of the microcontroller.
// This has been done so that each peripheral can be operated independently and tested in isolation.
// At any point, one submodule can simply be commented out in void loop() to disable it.

#include <Arduino.h>
#include <string.h>
#include <SPI.h>

#define GREEN_LED   PA7
#define RGB_GREEN   PB0
#define RGB_RED     PB1
#define RGB_BLUE    PB2

#define PIN_CS      PB6   
#define PIN_SCLK    PB3   
#define PIN_MISO    PB4   
#define PIN_MOSI    PB5  
#define PIN_DRDY    PB7   // TODO: implement interrupt-based data fetching
#define PIN_RESET   PA9   // (NOT ACTUALLY used)
#define PIN_CLKOUT  PA8   // MCO

static void initLed(void);        // Initialize the LED pins
void ledTest(uint32_t delayTime);
void ledCycle(); //sanity check for 
void checkAdcConnection();
void setupAdc();

uint64_t counter = 0;

void setup() {
  Serial2.begin(115200); // PA2 = TX, PA3 = RX
  Serial2.println("Serial monitor initalized!");
  initLed(); 
  ledCycle();
  setupAdc();
}

void loop() {
  ledTest(1000);
  //Serial2.println(counter);
  //counter++;
  //checkAdcConnection();
  delay(1000);
}

static void initLed(void) {
  pinMode(RGB_GREEN, OUTPUT); 
  digitalWrite(RGB_GREEN, LOW); 
  pinMode(RGB_RED, OUTPUT);
  digitalWrite(RGB_RED, LOW);
  pinMode(RGB_BLUE, OUTPUT);
  digitalWrite(RGB_BLUE, LOW);
  pinMode(GREEN_LED, OUTPUT);
  digitalWrite(GREEN_LED, LOW);
}

void ledTest(uint32_t delayTime) {
  for (int i = 0; i < 5; i++) {
    digitalWrite(RGB_BLUE, HIGH);
    delay(delayTime);
    digitalWrite(RGB_RED, LOW);
    delay(delayTime);
  }
}

void ledCycle(){
  digitalWrite(GREEN_LED, HIGH);
  delay(1000);
  digitalWrite(GREEN_LED, LOW);
  delay(1000);

  digitalWrite(RGB_BLUE, HIGH);
  delay(1000);
  digitalWrite(RGB_BLUE, LOW);
  delay(1000);
  
  digitalWrite(RGB_GREEN, HIGH);
  delay(1000);
  digitalWrite(RGB_GREEN, LOW);
  delay(1000);
  
  digitalWrite(RGB_RED, HIGH);
  delay(1000);
  digitalWrite(RGB_RED, LOW);
  delay(1000);
}

void setupAdc() {
  pinMode(PIN_CLKOUT, OUTPUT);
  RCC->CFGR &= ~RCC_CFGR_MCO;
  RCC->CFGR |= RCC_CFGR_MCO_SYSCLK; // Set MCO to SYSCLK

  pinMode(PIN_CS, OUTPUT);
  pinMode(PIN_RESET, OUTPUT);
  pinMode(PIN_DRDY, INPUT);
  digitalWrite(PIN_CS, HIGH);
  digitalWrite(PIN_RESET, HIGH);

  digitalWrite(PIN_RESET, LOW);
  delay(10);
  digitalWrite(PIN_RESET, HIGH);
  delay(10);

  SPI.begin();
}

void checkAdcConnection() {
  digitalWrite(PIN_CS, LOW);
  delayMicroseconds(1);

  uint16_t response = SPI.transfer16(0x0000); 

  digitalWrite(PIN_CS, HIGH);

  if (response != 0xFFFF && response != 0x0000) {
    digitalWrite(RGB_BLUE, HIGH);
    delay(3000);
    digitalWrite(RGB_BLUE, LOW);
  } else {
    digitalWrite(RGB_RED, HIGH);
    delay(3000);
    digitalWrite(RGB_RED, LOW);
  }

  delay(500);
}