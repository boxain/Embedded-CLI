#include "cmsis_os2.h"
#include <stdio.h>
#include "app_uart.h"
#include "app_cmd_handler.h"
#include "cmsis_os.h"
#include "lib_uart.h"
#include "lib_pot.h"
#include "lib_aht20.h"

#define UART_READ_BUFFER_SIZE   6
#define UART_PACKET_NOTIFY_FLAG 0x01

static uint8_t uart_read_buffer[UART_READ_BUFFER_SIZE];
static uart_packet_t uart_packet = {0};
static uint8_t uart_packet_len = sizeof(uart_packet_t);

// ================ Function Declaration ================
osThreadId_t GetUartTaskHandleID(void);
osMessageQueueId_t GetCmdQueueHandleID(void);
osSemaphoreId_t GetUartSemaphoreHandleID(void);
static int8_t _uart_packet_parse(command_t* cmd, action_t* act, uint8_t* value);

// ================ STM32 CallBack Function ================
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

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart == &huart2) {
        osSemaphoreId_t sid = GetUartSemaphoreHandleID();
        osSemaphoreRelease(sid);
    }
}

// ================ Public Function ================
void app_uart_init() {
    lib_uart_init();
    HAL_UART_Receive_DMA(&huart2, uart_read_buffer, UART_READ_BUFFER_SIZE);  // consider moving it to lib
}

void app_uart_transmit(uint8_t* data, uint8_t len) {
    osSemaphoreId_t sid = GetUartSemaphoreHandleID();
    osSemaphoreAcquire(sid, osWaitForever);
    HAL_UART_Transmit_DMA(&huart2, data, len);
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
            osMessageQueuePut(cmd_queue_id, (void*)&executed_cmd, 0, osWaitForever);
            // osSemaphoreId_t sid = GetUartSemaphoreHandleID();
            // osSemaphoreAcquire(sid, osWaitForever);
            // HAL_UART_Transmit_DMA(&huart2, uart_read_buffer, UART_READ_BUFFER_SIZE);
        }
        osDelay(50);
    }
}

void app_uart_cmd_execute_task() {
    osMessageQueueId_t cmd_queue_id = GetCmdQueueHandleID();
    while (1) {
        executed_command_t executed_cmd = {0};
        osMessageQueueGet(cmd_queue_id, &executed_cmd, NULL, osWaitForever);
        app_cmd_handler_execute(&executed_cmd);
    }
}


// ================ Private Function ================
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