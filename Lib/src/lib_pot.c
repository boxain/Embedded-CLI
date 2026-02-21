#include "lib_pot.h"

#define WORK_VOLTAGE 3.3
#define ADC_MAX_VALUE 4095.0

void lib_pot_calibrate() {
    HAL_ADCEx_Calibration_Start(&hadc1);
}

float lib_pot_read() {
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
    uint32_t value = HAL_ADC_GetValue(&hadc1);
    float voltage = (value/ADC_MAX_VALUE) * WORK_VOLTAGE;
    return voltage;
}

