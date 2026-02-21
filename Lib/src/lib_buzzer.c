#include "lib_buzzer.h"

void lib_buzzer_init() {
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
}

void lib_buzzer_play() {
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, 125);
}

void lib_buzzer_stop() {
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, 0);
}