/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    Secure_nsclib/secure_nsc.h
 * @author  MCD Application Team
 * @brief   Header for secure non-secure callable APIs list
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* USER CODE BEGIN Non_Secure_CallLib_h */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SECURE_NSC_H
#define SECURE_NSC_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported types ------------------------------------------------------------*/
/**
 * @brief  non-secure callback ID enumeration definition
 */
typedef enum {
  SECURE_FAULT_CB_ID = 0x00U, /*!< System secure fault callback ID */
  GTZC_ERROR_CB_ID = 0x01U    /*!< GTZC secure error callback ID */
} SECURE_CallbackIDTypeDef;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void SECURE_RegisterCallback(SECURE_CallbackIDTypeDef CallbackId, void *func);

/**
 * @brief Veneer to call write character to secure UART3 from NS firmware
 *
 * @param ch Character to write to UART
 * @return Returns 0 on success
 */
int SECURE_put_char_uart(int ch);

/**
 * @brief Typedef for GPIO IRQ Callback
 *
 * @param pin Pin that triggered IRQ
 */
typedef void (*gpio_irq_cb_t)(uint16_t pin);

typedef int (*uart_rx_irq_cb_t)(char ch);

/**
 * @brief Function to register NS GPIO callback
 */
void SECURE_register_gpio_cb(gpio_irq_cb_t callback);

/**
 * @brief Runction to register UART RX callback
 *
 * @param callback Pointer to RX callback
 */
void SECURE_register_uart_rx_cb(uart_rx_irq_cb_t callback);

#endif /* SECURE_NSC_H */
/* USER CODE END Non_Secure_CallLib_h */
