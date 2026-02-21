#include "app_cmd_handler.h"
#include "app_uart.h"
#include "lib_gpio.h"
#include "lib_pot.h"
#include "lib_buzzer.h"
#include "lib_aht20.h"
#include <stdio.h>
#include <string.h>

typedef void (*cmd_handler_fn)(uint8_t value);
typedef struct {
    command_t cmd;
    action_t  act;
    cmd_handler_fn handler;
} cmd_entry_t;


// ================ Handler Declaration ================
static void handle_gpio_led_red(uint8_t value);
static void handle_gpio_led_green(uint8_t value);
static void handle_gpio_led_blue(uint8_t value);
static void handle_adc_pot(uint8_t value);
static void handle_pwm_buzzer(uint8_t value);
static void handle_i2c_aht20(uint8_t value);


// ================ Handler Register ================
static const cmd_entry_t cmd_registry[] = {
{CMD_TYPE_GPIO, ACT_TYPE_LED_RED, handle_gpio_led_red},
{CMD_TYPE_GPIO, ACT_TYPE_LED_GREEN, handle_gpio_led_green},
{CMD_TYPE_GPIO, ACT_TYPE_LED_BLUE, handle_gpio_led_blue},
{CMD_TYPE_ADC, ACT_TYPE_POT, handle_adc_pot},
{CMD_TYPE_PWM, ACT_TYPE_BUZZER, handle_pwm_buzzer},
{CMD_TYPE_I2C, ACT_TYPE_AHT20, handle_i2c_aht20},
};
static const uint8_t cmd_registry_len = sizeof(cmd_registry) / sizeof(cmd_entry_t);


// ================ Public API ================
cmd_result_t app_cmd_handler_execute(executed_command_t* cmd) {
    for (uint8_t i = 0; i < cmd_registry_len; i++) {
        if (cmd->cmd_type == cmd_registry[i].cmd && cmd->act_type == cmd_registry[i].act) {
            cmd_registry[i].handler(cmd->value);
            return CMD_RESULT_OK;
        }
    }
    return CMD_RESULT_NOT_FOUND;
}


// ================ Private API ================
static void handle_gpio_led_red(uint8_t value) {
    if (value == 1) {
        lib_gpio_set(LED_RED_GPIO_Port, LED_RED_Pin);
    }else if (value == 0) {
        lib_gpio_reset(LED_RED_GPIO_Port, LED_RED_Pin);
    }else {
        lib_gpio_toggle(LED_RED_GPIO_Port, LED_RED_Pin);
    }
}

static void handle_gpio_led_green(uint8_t value) {
    if (value == 1) {
        lib_gpio_set(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
    }else if (value == 0) {
        lib_gpio_reset(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
    }else {
        lib_gpio_toggle(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
    }
}

static void handle_gpio_led_blue(uint8_t value) {
    if (value == 1) {
        lib_gpio_set(LED_BLUE_GPIO_Port, LED_BLUE_Pin);
    }else if (value == 0) {
        lib_gpio_reset(LED_BLUE_GPIO_Port, LED_BLUE_Pin);
    }else {
        lib_gpio_toggle(LED_BLUE_GPIO_Port, LED_BLUE_Pin);
    }
}

static void handle_adc_pot(uint8_t value) {
    float voltage = lib_pot_read();
    static char mesg[32] = {0};
    sprintf(mesg, "Potentiometer voltage: %.2f.", voltage);
    app_uart_transmit((uint8_t*)mesg, strlen(mesg));
}

static void handle_pwm_buzzer(uint8_t value) {
    if (value == 0) {
        lib_buzzer_stop();
    } else {
        lib_buzzer_play();
    }
}

static void handle_i2c_aht20(uint8_t value) {
    lib_aht20_init();
    osDelay(10);
    float humidity, temperature;
    lib_aht20_trigger();
    osDelay(80);
    uint8_t res = lib_aht20_read(&humidity, &temperature);
    if (res) {
        static char mesg[64] = {0};
        sprintf(mesg, "Temperature: %.2f C, Humidity: %.2f", temperature, humidity);
        app_uart_transmit((uint8_t*)mesg, strlen(mesg));
    }
}
