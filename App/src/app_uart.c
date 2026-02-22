#include "cmsis_os2.h"
#include <stdio.h>
#include "app_uart.h"
#include "app_cmd_handler.h"
#include "cmsis_os.h"
#include "lib_uart.h"
#include "lib_pot.h"
#include "lib_aht20.h"

#define UART_PACKET_HEADER_LEN    2
#define UART_PACKET_CHECKSUM_LEN  1
#define UART_READ_BUFFER_SIZE     48
#define UART_PACKET_NOTIFY_FLAG   0x01

static uint8_t uart_read_buffer[UART_READ_BUFFER_SIZE];
static uart_packet_t uart_packet = {0};
static uint8_t uart_packet_len = sizeof(uart_packet_t);

// ================ Function Declaration ================
osThreadId_t GetUartTaskHandleID(void);
osMessageQueueId_t GetCmdQueueHandleID(void);
osSemaphoreId_t GetUartSemaphoreHandleID(void);
static void _uart_packet_send(uint8_t* data, uint8_t offset);

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

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
    if (huart == &huart2) {
        int8_t res = lib_uart_write(uart_read_buffer, Size);
        if (res != -1) {
            osThreadId_t tid = GetUartTaskHandleID();
            osThreadFlagsSet(tid, UART_PACKET_NOTIFY_FLAG);
        }
        HAL_UARTEx_ReceiveToIdle_DMA(&huart2, uart_read_buffer, UART_READ_BUFFER_SIZE); // consider moving it to lib
        __HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT);
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
    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, uart_read_buffer, UART_READ_BUFFER_SIZE);
    __HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT);
    // HAL_UART_Receive_DMA(&huart2, uart_read_buffer, UART_READ_BUFFER_SIZE);  // consider moving it to lib
}

void app_uart_transmit(uint8_t* data, uint8_t len) {
    osSemaphoreId_t sid = GetUartSemaphoreHandleID();
    osSemaphoreAcquire(sid, osWaitForever);
    HAL_UART_Transmit_DMA(&huart2, data, len);
}

void app_uart_process_task() {
    uint8_t read_buffer[UART_READ_BUFFER_SIZE];
    while (1) {
        osThreadFlagsWait(UART_PACKET_NOTIFY_FLAG, osFlagsWaitAny, osWaitForever);
        while (1) {
            // =========== Header ( 0xAA ) Check ===========
            int8_t ret = lib_uart_peak(read_buffer, 1);
            if (ret == -1) break;
            if (read_buffer[0] != 0xAA) {
                lib_uart_read_index_update(1);
                continue;
            }

            // =========== Packet Length Check ===========
            ret = lib_uart_peak(read_buffer, 2);
            if (ret == -1) break;

            // =========== Calculate Checksum ===========
            ret = lib_uart_peak(read_buffer, 2+read_buffer[1]+1);  // header + len + payload + checksum
            if (ret == -1) break;
            uint8_t checksum = 0;
            for (uint8_t i = 0; i < read_buffer[1]+2; i++) {
                checksum += read_buffer[i];
            }
            if (checksum != read_buffer[2+read_buffer[1]]) {
                lib_uart_read_index_update(1);
            }else {
                lib_uart_read_index_update(2+read_buffer[1]+1);
                _uart_packet_send(read_buffer, 2); // skip header
            }
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
static void _uart_packet_send(uint8_t* data, uint8_t offset) {
    executed_command_t executed_cmd = {0};
    executed_cmd.cmd_type = data[offset];
    executed_cmd.act_type = data[offset+1];
    executed_cmd.value = data[offset+2];

    osMessageQueueId_t cmd_queue_id = GetCmdQueueHandleID();
    osMessageQueuePut(cmd_queue_id, (void*)&executed_cmd, 0, osWaitForever);

    osSemaphoreId_t sid = GetUartSemaphoreHandleID();
    osSemaphoreAcquire(sid, osWaitForever);
    HAL_UART_Transmit_DMA(&huart2, uart_read_buffer, UART_READ_BUFFER_SIZE);
}