/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file log.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef log_h
#define log_h

#include <stdio.h>
#include "shell/shell.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Definitions
 *****************************************************************************/

#define LOG_LEVEL_ERROR_COLOR "\e[31m"
#define LOG_LEVEL_WARNING_COLOR "\e[33m"
#define LOG_LEVEL_INFO_COLOR "\e[37m"
#define LOG_LEVEL_DEBUG_COLOR "\e[34m"

#define LOG_RESET_COLOR "\e[0m"

#define LOG_IMPL(color, level, fmt_str, ...)                                   \
  do {                                                                         \
    shell_printf(color "[%s] " level ": " fmt_str LOG_RESET_COLOR "\r\n",      \
                 FW_ENV, ##__VA_ARGS__);                                       \
  } while (0)

#define LOG_ERR(fmt_str, ...)                                                  \
  LOG_IMPL(LOG_LEVEL_ERROR_COLOR, "ERR", fmt_str, ##__VA_ARGS__)
#define LOG_WRN(fmt_str, ...)                                                  \
  LOG_IMPL(LOG_LEVEL_WARNING_COLOR, "WRN", fmt_str, ##__VA_ARGS__)
#define LOG_INF(fmt_str, ...)                                                  \
  LOG_IMPL(LOG_LEVEL_INFO_COLOR, "INF", fmt_str, ##__VA_ARGS__)
#define LOG_DBG(fmt_str, ...)                                                  \
  LOG_IMPL(LOG_LEVEL_DEBUG_COLOR, "DBG", fmt_str, ##__VA_ARGS__)

#define LOG_PRINTF_IMPL(color, fmt_str, ...)                                   \
  do {                                                                         \
    shell_printf(color fmt_str LOG_RESET_COLOR, ##__VA_ARGS__);                \
  } while (0)

#define LOG_ERR_PRINTF(fmt_str, ...)                                           \
  LOG_PRINTF_IMPL(LOG_LEVEL_ERROR_COLOR, fmt_str, ##__VA_ARGS__)
#define LOG_WRN_PRINTF(fmt_str, ...)                                           \
  LOG_PRINTF_IMPL(LOG_LEVEL_WARNING_COLOR, fmt_str, ##__VA_ARGS__)
#define LOG_INF_PRINTF(fmt_str, ...)                                           \
  LOG_PRINTF_IMPL(LOG_LEVEL_INFO_COLOR, fmt_str, ##__VA_ARGS__)
#define LOG_DBG_PRINTF(fmt_str, ...)                                           \
  LOG_PRINTF_IMPL(LOG_LEVEL_DEBUG_COLOR, fmt_str, ##__VA_ARGS__)

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
}
#endif
#endif /* log_h */
