#include "app_uart.h"
#include "cmsis_os2.h"
#include "lib_gpio.h"

#define UART_READ_BUFFER_SIZE   6
#define UART_PACKET_NOTIFY_FLAG 0x01

static uint8_t uart_read_buffer[UART_READ_BUFFER_SIZE];
static uart_packet_t uart_packet = {0};
static uint8_t uart_packet_len = sizeof(uart_packet_t);


osThreadId_t GetUartTaskHandleID(void);
osMessageQueueId_t GetCmdQueueHandleID(void);

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart == &huart2) {
        int8_t res = lib_uart_write(uart_read_buffer, UART_READ_BUFFER_SIZE);
        if (res != -1) {
            osThreadId_t tid = GetUartTaskHandleID();
            osThreadFlagsSet(tid, UART_PACKET_NOTIFY_FLAG);
        }
        HAL_UART_Receive_DMA(&huart2, uart_read_buffer, UART_READ_BUFFER_SIZE); // consider moving it to lib
    }
}

void app_uart_init() {
    lib_uart_init();
    HAL_UART_Receive_DMA(&huart2, uart_read_buffer, UART_READ_BUFFER_SIZE);  // consider moving it to lib
}

static int8_t _uart_packet_parse(command_t* cmd, action_t* act, uint8_t* value) {
    if (uart_packet.header != 0xAA || uart_packet.length != uart_packet_len-3) {
        return -1;
    }

    uint8_t checksum = uart_packet.header + uart_packet.length + uart_packet.cmd_type + uart_packet.action_type + uart_packet.value;
    if (checksum != uart_packet.checksum) return -1;

    *cmd = uart_packet.cmd_type;
    *act = uart_packet.action_type;
    *value = uart_packet.value;
    return 0;
}

void app_uart_process_task() {
    osMessageQueueId_t cmd_queue_id = GetCmdQueueHandleID();
    while (1) {
        osThreadFlagsWait(UART_PACKET_NOTIFY_FLAG, osFlagsWaitAny, osWaitForever);
        int8_t ret = lib_uart_read((uint8_t*)&uart_packet, uart_packet_len);
        executed_command_t executed_cmd = {0};
        if (ret != -1) {
            // ========== 1. Parsed packet format ==========
            ret = _uart_packet_parse(&executed_cmd.cmd_type, &executed_cmd.act_type, &executed_cmd.value);
            if (ret == -1) continue;

            // ========== 2. Add to queue for later processing ==========
            HAL_UART_Transmit_DMA(&huart2, uart_read_buffer, UART_READ_BUFFER_SIZE);
            osMessageQueuePut(cmd_queue_id, (void*)&executed_cmd, 0, osWaitForever);
        }
        osDelay(50);
    }
}

void app_uart_cmd_execute_task() {
    osMessageQueueId_t cmd_queue_id = GetCmdQueueHandleID();
    while (1) {
        executed_command_t executed_cmd = {0};
        osMessageQueueGet(cmd_queue_id, &executed_cmd, NULL, osWaitForever);
        switch (executed_cmd.cmd_type) {
            case CMD_TYPE_GPIO: {
                switch (executed_cmd.act_type) {
                    case ACT_TYPE_LED_BLUE:
                    case ACT_TYPE_LED_GREEN:
                    case ACT_TYPE_LED_RED:
                    {
                        if (executed_cmd.value == 1) {
                            lib_gpio_set(LED_BLUE_GPIO_Port, LED_BLUE_Pin);
                        }else if (executed_cmd.value == 0) {
                            lib_gpio_reset(LED_BLUE_GPIO_Port, LED_BLUE_Pin);
                        }else {
                            lib_gpio_toggle(LED_BLUE_GPIO_Port, LED_BLUE_Pin);
                        }
                    }
                    default:break;
                }
            }
            default:break;
        }
    }
}


/**
 * 1. Why can use GetUartTaskHandleID() ?
 *
 *
 *
 */

