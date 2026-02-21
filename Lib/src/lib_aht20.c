#include "lib_aht20.h"

#define AHT20_I2C_ADDRESS 0x70
#define AHT20_STATUS_CALIBRATION_BIT 3
#define AHT20_STATUS_MEASUREMENT_BIT 7

void lib_aht20_init() {
    uint8_t state;
    HAL_I2C_Master_Receive(&hi2c1, AHT20_I2C_ADDRESS, &state, 1, HAL_MAX_DELAY);
    if ((state >> AHT20_STATUS_CALIBRATION_BIT & 0x01) == 0x00) {
        uint8_t init_cmd[] = {0xbe, 0x08, 0x00};
        HAL_I2C_Master_Transmit(&hi2c1, AHT20_I2C_ADDRESS, init_cmd, sizeof(init_cmd), HAL_MAX_DELAY);
    }
}

void lib_aht20_trigger() {
    uint8_t trigger_cmd[] = {0xac, 0x33, 0x00};
    HAL_I2C_Master_Transmit(&hi2c1, AHT20_I2C_ADDRESS, trigger_cmd, sizeof(trigger_cmd), HAL_MAX_DELAY);
}

uint8_t lib_aht20_read(float* humidity, float* temperature) {
    uint8_t read_buffer[6];
    HAL_I2C_Master_Receive(&hi2c1, AHT20_I2C_ADDRESS, read_buffer, sizeof(read_buffer), HAL_MAX_DELAY);
    if (read_buffer[0] >> AHT20_STATUS_MEASUREMENT_BIT & 0x01) {
        *humidity = 0;
        *temperature = 0;
        return 0;
    }else {
        uint32_t humidity_data = ((uint32_t)read_buffer[3] >> 4) + ((uint32_t)read_buffer[2] << 4) + ((uint32_t)read_buffer[1] << 12);
        *humidity = (float)humidity_data / (1 << 20) * 100.0;

        uint32_t temperature_data = ((uint32_t)read_buffer[5]) + ((uint32_t)read_buffer[4] << 8) + ((uint32_t)(read_buffer[3] & 0x0f) << 16);
        *temperature = (float)temperature_data / (1 << 20) * 200-50;
        return 1;
    }
}
