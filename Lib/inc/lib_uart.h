#ifndef MCU_SHELL_LIB_UART_H
#define MCU_SHELL_LIB_UART_H

#include <stdint.h>

void lib_uart_init();
int8_t lib_uart_write(uint8_t* data, uint8_t len);
int8_t lib_uart_read(uint8_t* buffer, uint8_t len);
void lib_uart_read_index_update(uint8_t len);
int8_t lib_uart_peak(uint8_t* buffer, uint8_t len);

#endif //MCU_SHELL_LIB_UART_H