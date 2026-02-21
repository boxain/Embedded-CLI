//
// Created by aaron on 2026/2/20.
//

#ifndef MCU_SHELL_LIB_AHT20_H
#define MCU_SHELL_LIB_AHT20_H

#include "i2c.h"
#include "usart.h"
#include <string.h>
#include "cmsis_os2.h"

void lib_aht20_init();
void lib_aht20_trigger();
uint8_t lib_aht20_read(float* humidity, float* temperature);

#endif //MCU_SHELL_LIB_AHT20_H