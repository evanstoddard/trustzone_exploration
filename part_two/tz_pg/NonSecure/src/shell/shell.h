/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file shell.h
 * @author Evan Stoddard
 * @brief Stupid simple and most likely problematic shell
 */

#ifndef shell_h
#define shell_h

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

/**
 * @brief Shell command function
 *
 * @param argc Number of arguments, including the command
 * @param argv Array of arguments, with index of 0 being the command
 * @return Return 0 on success
 */
typedef int (*shell_cmd_func_t)(int argc, char **argv);

/**
 * @typedef shell_cmd_t
 * @brief Typedef of shell command structure
 *
 */
typedef struct shell_cmd_t {
  struct shell_cmd_t *prev;
  struct shell_cmd_t *next;
  const char *cmd;
  const char *usage;
  shell_cmd_func_t func;
} shell_cmd_t;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * @brief Initialize shell
 *
 * @return Returns 0 on success
 */
int shell_init(void);

/**
 * @brief Called when incoming character received
 *
 * @param ch Character received
 * @return Returns 0 on success
 */
int shell_handle_rx_char(char ch);

/**
 * @brief Register command with shell
 *
 * @param cmd Pointer to command struct
 * @return Returns 0 on success
 */
int shell_register_command(shell_cmd_t *cmd);

/**
 * @brief Print all commands with their usage string
 *
 */
void shell_print_help(void);

/**
 * @brief Poll shell — call from main loop to process a pending command
 *
 */
void shell_tick(void);

/**
 * @brief Printf routed through the shell — erases the current input line,
 *        prints the message, then reprints the prompt and in-progress input
 *
 */
void shell_printf(const char *fmt, ...);

#ifdef __cplusplus
}
#endif
#endif /* shell_h */
