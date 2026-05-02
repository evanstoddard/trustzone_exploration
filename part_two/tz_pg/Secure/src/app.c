/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file app.c
 * @author Evan Stoddard
 * @brief
 */

#include "main.h"
#include "stm32h5xx_hal_uart.h"
#include "usart.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "secure_nsc.h"

#include "log.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Variables
 *****************************************************************************/

/**
 * @brief Pointer to NS IRQ callback
 */
static gpio_irq_cb_t prv_gpio_irq_cb = NULL;

static uart_rx_irq_cb_t prv_uart_irq_cb = NULL;

static uint8_t prv_rx_byte = 0;

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
  // Purposely avoid HAL so we can also print from fault handlers...

  while (!(USART3->ISR & USART_ISR_TXE_TXFNF)) {
    __NOP();
  }

  USART3->TDR = (uint8_t)ch;

  return 0;
}

/**
 * @brief GPIO Callback
 *
 * @param GPIO_Pin GPIO Pin that triggered callback
 */
void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin) {
  if (prv_gpio_irq_cb != NULL) {
    ((CMSE_NS_CALL gpio_irq_cb_t)prv_gpio_irq_cb)(GPIO_Pin);
  }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

  if (prv_uart_irq_cb != NULL) {
    ((CMSE_NS_CALL uart_rx_irq_cb_t)prv_uart_irq_cb)((char)prv_rx_byte);
  }

  HAL_UART_Receive_IT(huart, &prv_rx_byte, 1);
}
/*****************************************************************************
 * Functions
 *****************************************************************************/

CMSE_NS_ENTRY void SECURE_register_gpio_cb(gpio_irq_cb_t callback) {
  prv_gpio_irq_cb = callback;
}

CMSE_NS_ENTRY void SECURE_register_uart_rx_cb(uart_rx_irq_cb_t callback) {
  prv_uart_irq_cb = callback;
}

void app_entry(void) {
  LOG_INF("Application started...");
  HAL_UART_Receive_IT(&huart3, &prv_rx_byte, 1);
}
