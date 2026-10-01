#include <Arduino.h>
#include <SPI.h>

#include "ads131m03.hpp"

const int ADS_RESET_PIN = 4;
const int ADS_DRDY_PIN  = 6;
const int ADS_CS_PIN    = 5;
const int ADS_SCK_PIN   = 46;
const int ADS_MOSI_PIN  = 45;
const int ADS_MISO_PIN  = 37;

SPIClass adc_bus(HSPI);

ADS131M03 ads;

int32_t cells_zeros[3] = {225896, 118401, 154683};
float cells_scales[3] = {365, -365, 4050};

void setup() {
    adc_bus.begin(ADS_SCK_PIN, ADS_MISO_PIN, ADS_MOSI_PIN);

    printf("\n\n\nInitializing ADS131M03\n");
    ads.begin(&adc_bus, ADS_CS_PIN, ADS_DRDY_PIN, ADS_RESET_PIN);

    printf("Configuring channels of interest\n");
    ads.configure_channel(ADS_CHANNEL_0, ADS_PGA_GAIN_128, ADS_OSR_RATIO_1024);
    ads.configure_channel(ADS_CHANNEL_1, ADS_PGA_GAIN_128, ADS_OSR_RATIO_1024);
    ads.configure_channel(ADS_CHANNEL_2, ADS_PGA_GAIN_128, ADS_OSR_RATIO_1024);
    
    printf("Completed setup(), starting loop()\n\n");
    delay(1000);
}


void loop() {
    ads.update_readings();

    int32_t raw_data[3];
    ads.record_readings(raw_data);
    printf("Channel raw readings 0:%8ld 1:%8ld 2:%8ld\n", raw_data[0], raw_data[1], raw_data[2]);

    float load_data[3]; // in kg
    for (int i = 0; i < 3; i++){
        load_data[i] = (float)(raw_data[i] - cells_zeros[i]) / cells_scales[i]; 
    }
    printf("Channel load readings (kg)\n 0:  %.1f\n 1:  %.1f\n 2:  %.1f\n", load_data[0], load_data[1], load_data[2]);

    delay(2500);
}