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
#include <stdint.h>

#include <stdio.h>

#include "usart.h"

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
  HAL_UART_Transmit(&huart3, (const uint8_t *)&ch, sizeof(uint8_t), 1000);

  return 0;
}

/*****************************************************************************
 * Functions
 *****************************************************************************/

void app_entry(void) {
  while (1) {
    printf("Hello World!\r\n");
    HAL_Delay(1000);
  }
}
