#ifndef ADS131M03_REG_HEADER
#define ADS131M03_REG_HEADER

#include <stdint.h>

enum ADSRegisterAddress : uint8_t {
    ADS_REG_ID            = 0x00,
    ADS_REG_STATUS        = 0x01,
    ADS_REG_MODE          = 0x02,
    ADS_REG_CLOCK         = 0x03,
    ADS_REG_GAIN          = 0x04,
    ADS_REG_CFG           = 0x06,
    ADS_REG_THRSHLD_MSB   = 0x07,
    ADS_REG_THRSHLD_LSB   = 0x08,
    ADS_REG_CH0_CFG       = 0x09,
    ADS_REG_CH0_OCAL_MSB  = 0x0A,
    ADS_REG_CH0_OCAL_LSB  = 0x0B,
    ADS_REG_CH0_GCAL_MSB  = 0x0C,
    ADS_REG_CH0_GCAL_LSB  = 0x0D,
    ADS_REG_CH1_CFG       = 0x0E,
    ADS_REG_CH1_OCAL_MSB  = 0x0F,
    ADS_REG_CH1_OCAL_LSB  = 0x10,
    ADS_REG_CH1_GCAL_MSB  = 0x11,
    ADS_REG_CH1_GCAL_LSB  = 0x12,
    ADS_REG_CH2_CFG       = 0x13,
    ADS_REG_CH2_OCAL_MSB  = 0x14,
    ADS_REG_CH2_OCAL_LSB  = 0x15,
    ADS_REG_CH2_GCAL_MSB  = 0x16,
    ADS_REG_CH2_GCAL_LSB  = 0x17,
    ADS_REG_REGMAP_CRC    = 0x3E
};

enum ADSCommands : uint16_t {
    ADS_CMD_NULL      = 0x0000,
    ADS_CMD_RESET     = 0x0011,
    ADS_CMD_STANDBY   = 0x0022,
    ADS_CMD_WAKEUP    = 0x0033,
    ADS_CMD_LOCK      = 0x0555,
    ADS_CMD_UNLOCK    = 0x0666,
    ADS_CMD_RREG      = 0xA000,
    ADS_CMD_WREG      = 0x6000
};

#endif
