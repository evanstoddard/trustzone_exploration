/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file app.h
 * @author Evan Stoddard
 * @brief
 */

#ifndef app_h
#define app_h

#include "stm32h5xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Structs, Unions, Enums, & Typedefs
 *****************************************************************************/

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * @brief Entrypoint into application
 */
void app_entry(void);

#ifdef __cplusplus
}
#endif
#endif /* app_h */
