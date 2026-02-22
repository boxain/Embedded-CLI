#include <string.h>
#include "usart.h"
#include "lib_uart.h"

#define UART_BUFFER_SIZE 64

typedef struct {
    uint8_t rx_buffer[UART_BUFFER_SIZE];
    uint8_t write_index;
    uint8_t read_index;
} uart_buffer_t;

static uart_buffer_t uart_buffer;

void lib_uart_init() {
    uart_buffer.write_index = 0;
    uart_buffer.read_index = 0;
}

uint8_t lib_uart_buffer_length() {
    return ( uart_buffer.write_index - uart_buffer.read_index + UART_BUFFER_SIZE ) % UART_BUFFER_SIZE;
}

static uint8_t lib_uart_buffer_free_length() {
    return UART_BUFFER_SIZE - lib_uart_buffer_length() - 1;
}

int8_t lib_uart_write(uint8_t* data, uint8_t len) {
    if (lib_uart_buffer_free_length() < len) {
        return -1;
    }

    uint8_t write_idx = uart_buffer.write_index;
    for (uint8_t i = 0; i < len; i++) {
        uart_buffer.rx_buffer[(write_idx+i) % UART_BUFFER_SIZE] = data[i];
    }

    uart_buffer.write_index = (write_idx + len) % UART_BUFFER_SIZE;
    return 0;
}

int8_t lib_uart_read(uint8_t* buffer, uint8_t len) {
    if (len > lib_uart_buffer_length()) {
        return -1;
    }

    for (uint8_t i = 0; i < len; i++) {
        buffer[i] = uart_buffer.rx_buffer[(uart_buffer.read_index+i) % UART_BUFFER_SIZE];
    }
    uart_buffer.read_index = (uart_buffer.read_index + len) % UART_BUFFER_SIZE;
    return 0;
}

void lib_uart_read_index_update(uint8_t len) {
    uart_buffer.read_index = (uart_buffer.read_index + len) % UART_BUFFER_SIZE;
}

int8_t lib_uart_peak(uint8_t* buffer, uint8_t len) {
    if (len > lib_uart_buffer_length()) {
        return -1;
    }

    for (uint8_t i = 0; i < len; i++) {
        buffer[i] = uart_buffer.rx_buffer[(uart_buffer.read_index+i) % UART_BUFFER_SIZE];
    }
    return 0;
}