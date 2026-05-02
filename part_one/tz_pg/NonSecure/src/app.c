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
 * @brief Rudamentary flag indicating GPIO IRQ occurred
 */
static volatile bool prv_gpio_irq_flag = false;

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

/**
 * @brief Callback for GPIO IRQs
 *
 * @param pin Pin number
 */
static void prv_gpio_irq_callback(uint16_t pin) {
  (void)pin;
  prv_gpio_irq_flag = true;

  LOG_INF("GPIO IRQ fired.");
}

/*****************************************************************************
 * Functions
 *****************************************************************************/

void app_entry(void) {
  extern char _ns_label_start[];
  extern char _ns_label_end[];
  size_t ns_label_size = (size_t)(_ns_label_end - _ns_label_start);

  memset(_ns_label_start, 0, ns_label_size);
  char str[] = "NonSecure RAM.";
  strcpy(_ns_label_start, str);

  LOG_DBG("String value at %p - %s", (void *)_ns_label_start, _ns_label_start);

  // Register GPIO IRQ with secure firmware
  SECURE_register_gpio_cb(prv_gpio_irq_callback);

  while (1) {
    LOG_INF("Hello from NS World!");

    if (prv_gpio_irq_flag) {

      prv_gpio_irq_flag = false;
    }

    HAL_Delay(1000);
  }
}
