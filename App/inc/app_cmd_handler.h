#ifndef MCU_SHELL_APP_CMD_HANDLER_H
#define MCU_SHELL_APP_CMD_HANDLER_H

#include "app_uart.h"

typedef enum {
    CMD_RESULT_FAILED,
    CMD_RESULT_OK,
    CMD_RESULT_NOT_FOUND
} cmd_result_t;

cmd_result_t app_cmd_handler_execute(executed_command_t* cmd);

#endif //MCU_SHELL_APP_CMD_HANDLER_H