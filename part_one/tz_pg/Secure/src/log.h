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
    printf(color "[%s] " level ": " fmt_str LOG_RESET_COLOR "\r\n", FW_ENV,    \
           ##__VA_ARGS__);                                                     \
  } while (0)

#define LOG_ERR(fmt_str, ...)                                                  \
  LOG_IMPL(LOG_LEVEL_ERROR_COLOR, "ERR", fmt_str, ##__VA_ARGS__)
#define LOG_WRN(fmt_str, ...)                                                  \
  LOG_IMPL(LOG_LEVEL_WARNING_COLOR, "WRN", fmt_str, ##__VA_ARGS__)
#define LOG_INF(fmt_str, ...)                                                  \
  LOG_IMPL(LOG_LEVEL_INFO_COLOR, "INF", fmt_str, ##__VA_ARGS__)
#define LOG_DBG(fmt_str, ...)                                                  \
  LOG_IMPL(LOG_LEVEL_DEBUG_COLOR, "DBG", fmt_str, ##__VA_ARGS__)

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
