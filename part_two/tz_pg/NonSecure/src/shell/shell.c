/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file shell.c
 * @author Evan Stoddard
 * @brief
 */

#include "shell.h"

#include <errno.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <string.h>

#include <stdio.h>
#include <stdarg.h>

#include "log.h"

#include "cmsis_compiler.h"

#include <ctype.h>

/*****************************************************************************
 * Definitions
 *****************************************************************************/

#ifndef SHELL_RX_BUF_SIZE_BYTES
#define SHELL_RX_BUF_SIZE_BYTES 256
#endif

#define SHELL_MAX_ARGS 8

#ifndef SHELL_PROMPT
#define SHELL_PROMPT "shell:~$ "
#endif

/*****************************************************************************
 * Variables
 *****************************************************************************/

typedef struct {
  char buf[SHELL_RX_BUF_SIZE_BYTES];
  volatile uint32_t idx;
  volatile uint8_t avail_flag;
} shell_rx_buf_t;

/**
 * @brief Private instance
 */
static struct {
  bool initialized;

  shell_rx_buf_t rx_buf;

  shell_cmd_t *cmds;
} prv_inst;

/*****************************************************************************
 * Private Functions
 *****************************************************************************/

/**
 * @brief Handle reception of backspace
 */
static void prv_handle_backspace_char(void) {
  if (prv_inst.rx_buf.idx == 0) {
    return;
  }

  prv_inst.rx_buf.idx--;
  printf("\b \b");
  fflush(NULL);
}

/**
 * @brief Handle terminator character(s)
 */
static void prv_handle_terminator_char(void) {
  prv_inst.rx_buf.buf[prv_inst.rx_buf.idx] = '\0';
  __DMB();
  prv_inst.rx_buf.avail_flag = true;
  printf("\r\n");
  fflush(NULL);
}

/**
 * @brief Handle non-printable characters
 *
 * @param ch Received character
 */
static void prv_handle_non_printable_char(char ch) {
  switch (ch) {
  case 0x7f:
    prv_handle_backspace_char();
    break;
  case '\r':
  case '\n':
    prv_handle_terminator_char();
    break;
  default:
    break;
  }
}

static void prv_dispatch(void) {
  char *buf = prv_inst.rx_buf.buf;
  char *argv[SHELL_MAX_ARGS];
  int argc = 0;

  // Null-terminate every whitespace run and collect token pointers
  char *p = buf;
  while (*p != '\0' && argc < SHELL_MAX_ARGS) {
    // Skip whitespace between tokens
    while (*p != '\0' && isspace((unsigned char)*p)) {
      *p++ = '\0';
    }

    if (*p == '\0') {
      break;
    }

    argv[argc++] = p;

    // Advance past the token
    while (*p != '\0' && !isspace((unsigned char)*p)) {
      p++;
    }
  }

  if (argc == 0) {
    return;
  }

  shell_cmd_t *node = prv_inst.cmds;
  while (node) {
    if (strcmp(argv[0], node->cmd) == 0) {
      node->func(argc, argv);
      return;
    }
    node = node->next;
  }

  LOG_ERR_PRINTF("Unknown command: %s\r\n", argv[0]);
}

/*****************************************************************************
 * Default shell commands
 *****************************************************************************/

/**
 * @brief Handler for help command
 *
 * @param argc Unused
 * @param argv Unused
 * @return Always returns 0
 */
static int prv_help_cmd_handler(int argc, char **argv) {
  shell_print_help();

  return 0;
}

/**
 * @brief Handler to clear command
 *
 * @param argc Unused
 * @param argv Unused
 * @return Always returns 0
 */
static int prv_clear_cmd_handler(int argc, char **argv) {
  printf("\033[2J\033[H");
  fflush(NULL);

  return 0;
}

static shell_cmd_t prv_default_commands[] = {
    {
        .cmd = "help",
        .usage = "Prints list of commands with their usage.",
        .func = prv_help_cmd_handler,
    },
    {
        .cmd = "clear",
        .usage = "Clears screen.",
        .func = prv_clear_cmd_handler,
    },
};

/*****************************************************************************
 * Functions
 *****************************************************************************/

int shell_init(void) {

  if (prv_inst.initialized == true) {
    return -EALREADY;
  }

  for (size_t i = 0; i < (sizeof(prv_default_commands) / sizeof(shell_cmd_t));
       i++) {
    shell_register_command(&prv_default_commands[i]);
  }

  prv_inst.initialized = true;

  printf("%s ", SHELL_PROMPT);
  fflush(NULL);

  return 0;
}

int shell_handle_rx_char(char ch) {

  if (prv_inst.rx_buf.avail_flag) {
    return 0;
  }

  if (!isprint(ch)) {
    prv_handle_non_printable_char(ch);
    return 0;
  }

  if (prv_inst.rx_buf.idx < SHELL_RX_BUF_SIZE_BYTES - 1) {
    prv_inst.rx_buf.buf[prv_inst.rx_buf.idx++] = ch;
    printf("%c", ch);
    fflush(NULL);
  }

  return 0;
}

int shell_register_command(shell_cmd_t *cmd) {
  if (cmd == NULL || cmd->cmd == NULL || cmd->usage == NULL ||
      cmd->func == NULL) {
    return -EINVAL;
  }

  if (prv_inst.cmds == NULL) {
    prv_inst.cmds = cmd;
    cmd->prev = NULL;
    cmd->next = NULL;
    return 0;
  }

  shell_cmd_t *node = prv_inst.cmds;

  while (node) {
    int cmp = strcmp(cmd->cmd, node->cmd);

    if (cmp == 0) {
      return -EALREADY;
    }

    if (cmp < 0) {
      cmd->next = node;
      cmd->prev = node->prev;

      if (node->prev == NULL) {
        prv_inst.cmds = cmd;
      } else {
        node->prev->next = cmd;
      }

      return 0;
    }

    if (node->next == NULL) {
      node->next = cmd;
      cmd->prev = node;
      cmd->next = NULL;
      return 0;
    }

    node = node->next;
  }

  return 0;
}

void shell_print_help(void) {
  printf("Command list:\r\n\r\n");

  shell_cmd_t *node = prv_inst.cmds;
  while (node) {
    printf("%s\r\n\t%s\r\n", node->cmd, node->usage);
    node = node->next;
  }
}

void shell_tick(void) {
  if (!prv_inst.rx_buf.avail_flag) {
    return;
  }

  prv_dispatch();

  prv_inst.rx_buf.idx = 0;
  __DMB();
  prv_inst.rx_buf.avail_flag = false;

  printf("%s ", SHELL_PROMPT);
  fflush(NULL);
}

void shell_printf(const char *fmt, ...) {
  if (prv_inst.initialized) {
    // Erase current line (prompt + any typed characters)
    printf("\r\033[K");
  }

  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);

  if (prv_inst.initialized) {
    // Reprint prompt and whatever the user has typed so far
    printf("%s %.*s", SHELL_PROMPT, (int)prv_inst.rx_buf.idx,
           prv_inst.rx_buf.buf);
    fflush(NULL);
  }
}
