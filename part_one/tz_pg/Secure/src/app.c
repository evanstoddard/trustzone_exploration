/*
 * Copyright (C) Evan Stoddard
 */

/**
 * @file app.c
 * @author Evan Stoddard
 * @brief
 */

#include "main.h"
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
  LOG_INF("GPIO IRQ fired!");

  char *ns_aliased = (char *)0x20050000;
  char *s_aliased = (char *)(ns_aliased + 0x10000000);

  LOG_INF("String value at %p - %s", ns_aliased, ns_aliased);
  LOG_INF("String value at %p - %s", s_aliased, s_aliased);

  if (prv_gpio_irq_cb != NULL) {
    ((CMSE_NS_CALL gpio_irq_cb_t)prv_gpio_irq_cb)(GPIO_Pin);
  }
}

/*****************************************************************************
 * Functions
 *****************************************************************************/

__attribute__((section(".secure_target_func"), noinline)) void
secure_target(void) {
  LOG_INF("secure_target reached!");
}

CMSE_NS_ENTRY void SECURE_register_gpio_cb(gpio_irq_cb_t callback) {
  prv_gpio_irq_cb = callback;
}

void app_entry(void) {
  LOG_INF("Application started...");

  extern char _secure_label_start[];
  extern char _secure_label_end[];
  size_t secure_ram_label_size_bytes =
      (size_t)(_secure_label_end - _secure_label_start);

  memset(_secure_label_start, 0, secure_ram_label_size_bytes);
  char str[] = "Secure RAM.";
  strcpy(_secure_label_start, str);

  // Create pointers to secure alias and non-secure alias.  Subtract 0x10000000
  // because NS RAM starts at 0x20000000 and S is aliased to 0x30000000
  char *s_aliased = (char *)_secure_label_start;
  char *ns_aliased = (char *)(s_aliased - 0x10000000);

  LOG_DBG("String value at %p - %s", s_aliased, s_aliased);
  LOG_DBG("String value at %p - %s", ns_aliased, ns_aliased);
}
