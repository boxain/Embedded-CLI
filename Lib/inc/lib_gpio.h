#ifndef MCU_SHELL_LIB_GPIO_H
#define MCU_SHELL_LIB_GPIO_H

#include "gpio.h"

void lib_gpio_set(GPIO_TypeDef *GPIOx, uint16_t GPIO_PIN);
void lib_gpio_reset(GPIO_TypeDef *GPIOx, uint16_t GPIO_PIN);
void lib_gpio_toggle(GPIO_TypeDef *GPIOx, uint16_t GPIO_PIN);

#endif //MCU_SHELL_LIB_GPIO_H