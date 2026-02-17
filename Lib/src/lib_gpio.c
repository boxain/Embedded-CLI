#include "lib_gpio.h"

void lib_gpio_set(GPIO_TypeDef *GPIOx, uint16_t GPIO_PIN) {
    HAL_GPIO_WritePin(GPIOx, GPIO_PIN, GPIO_PIN_SET);
}

void lib_gpio_reset(GPIO_TypeDef *GPIOx, uint16_t GPIO_PIN) {
    HAL_GPIO_WritePin(GPIOx, GPIO_PIN, GPIO_PIN_RESET);
}

void lib_gpio_toggle(GPIO_TypeDef *GPIOx, uint16_t GPIO_PIN) {
    HAL_GPIO_TogglePin(GPIOx, GPIO_PIN);
}