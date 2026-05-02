/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file app.c
 * @author Evan Stoddard
 * @brief
 */

#include "app.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "log.h"

#include "secure_nsc.h"

#include "shell/shell.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Variables
 *****************************************************************************/

/*****************************************************************************
 * Bindings
 *****************************************************************************/

/**
 * @brief Binding for stdio
 *
 * @param ch Character to write
 * @return Returns 0 on success
 */
int __io_putchar(int ch) {
  SECURE_put_char_uart(ch);
  return 0;
}

/*****************************************************************************
 * Private Functions
 *****************************************************************************/

/*****************************************************************************
 * Shell Commands
 *****************************************************************************/

static int prv_cmd_func(int argc, char **argv) {
  LOG_INF("Command called with %d args:", argc);

  for (int i = 0; i < argc; i++) {
    LOG_INF("Arg: %s", argv[i]);
  }

  return 0;
}

/**
 * @brief Array of shell commands
 */
static shell_cmd_t prv_shell_commands[] = {
    {.cmd = "b", .usage = "B command.", .func = prv_cmd_func},
    {.cmd = "aa", .usage = "Aa command.", .func = prv_cmd_func},
    {.cmd = "a", .usage = "A command.", .func = prv_cmd_func},
};

/*****************************************************************************
 * Functions
 *****************************************************************************/

void app_entry(void) {

  shell_init();

  for (size_t i = 0; i < (sizeof(prv_shell_commands) / sizeof(shell_cmd_t));
       i++) {
    shell_register_command(&prv_shell_commands[i]);
  }

  SECURE_register_uart_rx_cb(shell_handle_rx_char);

  while (1) {
    shell_tick();

    if ((HAL_GetTick() % 1000) == 0) {
      LOG_INF("Tick!");
    }
  }
}
