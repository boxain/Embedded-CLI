#ifndef MCU_SHELL_APP_UART_H
#define MCU_SHELL_APP_UART_H

#include <stdint.h>

typedef enum {
    CMD_TYPE_GPIO,
    CMD_TYPE_PWM,
    CMD_TYPE_I2C,
    CMD_TYPE_SPI,
    CMD_TYPE_ADC
} command_t;

typedef enum {
    ACT_TYPE_LED_RED,
    ACT_TYPE_LED_GREEN,
    ACT_TYPE_LED_BLUE,
    ACT_TYPE_SERVO,
    ACT_TYPE_BUZZER,
    ACT_TYPE_AHT20,
    ACT_TYPE_POT,
    ACT_TYPE_NTC,
} action_t;

typedef struct {
    uint8_t header;
    uint8_t length;
    uint8_t cmd_type;
    uint8_t action_type;
    uint8_t value;
    uint8_t checksum;
} uart_packet_t;

typedef struct {
    command_t cmd_type;
    action_t act_type;
    uint8_t value;
} executed_command_t;

void app_uart_init();
void app_uart_transmit(uint8_t* data, uint8_t len);
void app_uart_process_task();
void app_uart_cmd_execute_task();


#endif //MCU_SHELL_APP_UART_H