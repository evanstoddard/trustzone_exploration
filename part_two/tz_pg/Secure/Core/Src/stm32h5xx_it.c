/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    stm32h5xx_it.c
 * @brief   Interrupt Service Routines.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32h5xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "log.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* Naked prevents compiler prologue from corrupting SP before we read it */
void HardFault_Handler(void)   __attribute__((naked));
void MemManage_Handler(void)   __attribute__((naked));
void BusFault_Handler(void)    __attribute__((naked));
void UsageFault_Handler(void)  __attribute__((naked));
void SecureFault_Handler(void) __attribute__((naked));

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

typedef struct {
    uint32_t r0, r1, r2, r3;
    uint32_t r12;
    uint32_t lr;
    uint32_t pc;
    uint32_t xpsr;
} fault_frame_t;

static void fault_dump(fault_frame_t *frame, uint32_t exc_return) {
    uint32_t cfsr = SCB->CFSR;
    uint32_t hfsr = SCB->HFSR;

    LOG_ERR("  r0:   0x%08x  r1:   0x%08x", (unsigned)frame->r0,  (unsigned)frame->r1);
    LOG_ERR("  r2:   0x%08x  r3:   0x%08x", (unsigned)frame->r2,  (unsigned)frame->r3);
    LOG_ERR("  r12:  0x%08x  lr:   0x%08x", (unsigned)frame->r12, (unsigned)frame->lr);
    LOG_ERR("  pc:   0x%08x  xpsr: 0x%08x", (unsigned)frame->pc,  (unsigned)frame->xpsr);
    LOG_ERR("  sp:   0x%08x", (unsigned)(frame + 1));

    /* EXC_RETURN: encodes the security/stack/mode context of the interrupted code */
    LOG_ERR("  EXC_RETURN: 0x%08x  mode=%-7s  stack=%s  security=%s",
        (unsigned)exc_return,
        (exc_return & (1u << 3)) ? "Thread" : "Handler",
        (exc_return & (1u << 2)) ? "PSP" : "MSP",
        (exc_return & (1u << 0)) ? "NS" : "S");

    /* HFSR */
    LOG_ERR("  HFSR: 0x%08x%s%s", (unsigned)hfsr,
        (hfsr & SCB_HFSR_FORCED_Msk)  ? "  FORCED"  : "",
        (hfsr & SCB_HFSR_VECTTBL_Msk) ? "  VECTTBL" : "");

    /* CFSR — MemManage, Bus, and Usage fault sub-registers */
    LOG_ERR("  CFSR: 0x%08x", (unsigned)cfsr);

    if (cfsr & 0x000000FFu) {
        LOG_ERR("  MMFSR: 0x%02x%s%s%s%s",
            (unsigned)(cfsr & 0xFFu),
            (cfsr & SCB_CFSR_IACCVIOL_Msk)  ? "  IACCVIOL"  : "",
            (cfsr & SCB_CFSR_DACCVIOL_Msk)  ? "  DACCVIOL"  : "",
            (cfsr & SCB_CFSR_MUNSTKERR_Msk) ? "  MUNSTKERR" : "",
            (cfsr & SCB_CFSR_MSTKERR_Msk)   ? "  MSTKERR"   : "");
        if (cfsr & SCB_CFSR_MMARVALID_Msk)
            LOG_ERR("    MMFAR: 0x%08x", (unsigned)SCB->MMFAR);
    }

    if (cfsr & 0x0000FF00u) {
        LOG_ERR("  BFSR:  0x%02x%s%s%s%s%s",
            (unsigned)((cfsr >> 8) & 0xFFu),
            (cfsr & SCB_CFSR_IBUSERR_Msk)     ? "  IBUSERR"     : "",
            (cfsr & SCB_CFSR_PRECISERR_Msk)   ? "  PRECISERR"   : "",
            (cfsr & SCB_CFSR_IMPRECISERR_Msk) ? "  IMPRECISERR" : "",
            (cfsr & SCB_CFSR_UNSTKERR_Msk)    ? "  UNSTKERR"    : "",
            (cfsr & SCB_CFSR_STKERR_Msk)      ? "  STKERR"      : "");
        if (cfsr & SCB_CFSR_BFARVALID_Msk)
            LOG_ERR("    BFAR:  0x%08x", (unsigned)SCB->BFAR);
    }

    if (cfsr & 0xFFFF0000u) {
        LOG_ERR("  UFSR:  0x%04x%s%s%s%s%s%s%s",
            (unsigned)((cfsr >> 16) & 0xFFFFu),
            (cfsr & SCB_CFSR_UNDEFINSTR_Msk) ? "  UNDEFINSTR" : "",
            (cfsr & SCB_CFSR_INVSTATE_Msk)   ? "  INVSTATE"   : "",
            (cfsr & SCB_CFSR_INVPC_Msk)      ? "  INVPC"      : "",
            (cfsr & SCB_CFSR_NOCP_Msk)       ? "  NOCP"       : "",
            (cfsr & SCB_CFSR_STKOF_Msk)      ? "  STKOF"      : "",
            (cfsr & SCB_CFSR_UNALIGNED_Msk)  ? "  UNALIGNED"  : "",
            (cfsr & SCB_CFSR_DIVBYZERO_Msk)  ? "  DIVBYZERO"  : "");
    }

    /* SFSR: set even when SecureFault escalates to HardFault (SECUREFAULTENA=0
     * or fault masked), so always check it here to catch that case. */
    uint32_t sfsr = SAU->SFSR;
    uint32_t sfar = SAU->SFAR;
    if (sfsr) {
        LOG_ERR("  SFSR: 0x%08x%s%s%s%s%s", (unsigned)sfsr,
            (sfsr & SAU_SFSR_INVEP_Msk)   ? "  INVEP"   : "",
            (sfsr & SAU_SFSR_INVIS_Msk)   ? "  INVIS"   : "",
            (sfsr & SAU_SFSR_INVER_Msk)   ? "  INVER"   : "",
            (sfsr & SAU_SFSR_AUVIOL_Msk)  ? "  AUVIOL"  : "",
            (sfsr & SAU_SFSR_INVTRAN_Msk) ? "  INVTRAN" : "");
        if (sfsr & SAU_SFSR_SFARVALID_Msk)
            LOG_ERR("  SFAR: 0x%08x", (unsigned)sfar);
    }

    /* SHCSR: fault enables and which handlers are currently active */
    uint32_t shcsr = SCB->SHCSR;
    LOG_ERR("  SHCSR: 0x%08x", (unsigned)shcsr);
    LOG_ERR("    ena:%s%s%s%s",
        (shcsr & SCB_SHCSR_SECUREFAULTENA_Msk) ? "  SFAULTENA" : "",
        (shcsr & SCB_SHCSR_BUSFAULTENA_Msk)    ? "  BFAULTENA" : "",
        (shcsr & SCB_SHCSR_USGFAULTENA_Msk)    ? "  UFAULTENA" : "",
        (shcsr & SCB_SHCSR_MEMFAULTENA_Msk)    ? "  MFAULTENA" : "");
    LOG_ERR("    act:%s%s%s%s%s",
        (shcsr & SCB_SHCSR_HARDFAULTACT_Msk)   ? "  HARDFAULT" : "",
        (shcsr & SCB_SHCSR_SECUREFAULTACT_Msk) ? "  SFAULT"    : "",
        (shcsr & SCB_SHCSR_USGFAULTACT_Msk)    ? "  UFAULT"    : "",
        (shcsr & SCB_SHCSR_BUSFAULTACT_Msk)    ? "  BFAULT"    : "",
        (shcsr & SCB_SHCSR_MEMFAULTACT_Msk)    ? "  MEMFAULT"  : "");

    /* When the fault came from NS context, fault status bits land in the NS
     * SCB registers, not the Secure ones we read above. */
    if (exc_return & 0x1u) {
        uint32_t cfsr_ns = SCB_NS->CFSR;
        uint32_t hfsr_ns = SCB_NS->HFSR;
        LOG_ERR("  [NS] HFSR: 0x%08x%s%s", (unsigned)hfsr_ns,
            (hfsr_ns & SCB_HFSR_FORCED_Msk)  ? "  FORCED"  : "",
            (hfsr_ns & SCB_HFSR_VECTTBL_Msk) ? "  VECTTBL" : "");
        LOG_ERR("  [NS] CFSR: 0x%08x", (unsigned)cfsr_ns);
        if (cfsr_ns & 0x000000FFu) {
            LOG_ERR("  [NS] MMFSR: 0x%02x%s%s%s%s",
                (unsigned)(cfsr_ns & 0xFFu),
                (cfsr_ns & SCB_CFSR_IACCVIOL_Msk)  ? "  IACCVIOL"  : "",
                (cfsr_ns & SCB_CFSR_DACCVIOL_Msk)  ? "  DACCVIOL"  : "",
                (cfsr_ns & SCB_CFSR_MUNSTKERR_Msk) ? "  MUNSTKERR" : "",
                (cfsr_ns & SCB_CFSR_MSTKERR_Msk)   ? "  MSTKERR"   : "");
            if (cfsr_ns & SCB_CFSR_MMARVALID_Msk)
                LOG_ERR("    [NS] MMFAR: 0x%08x", (unsigned)SCB_NS->MMFAR);
        }
        if (cfsr_ns & 0x0000FF00u) {
            LOG_ERR("  [NS] BFSR:  0x%02x%s%s%s%s%s",
                (unsigned)((cfsr_ns >> 8) & 0xFFu),
                (cfsr_ns & SCB_CFSR_IBUSERR_Msk)     ? "  IBUSERR"     : "",
                (cfsr_ns & SCB_CFSR_PRECISERR_Msk)   ? "  PRECISERR"   : "",
                (cfsr_ns & SCB_CFSR_IMPRECISERR_Msk) ? "  IMPRECISERR" : "",
                (cfsr_ns & SCB_CFSR_UNSTKERR_Msk)    ? "  UNSTKERR"    : "",
                (cfsr_ns & SCB_CFSR_STKERR_Msk)      ? "  STKERR"      : "");
            if (cfsr_ns & SCB_CFSR_BFARVALID_Msk)
                LOG_ERR("    [NS] BFAR:  0x%08x", (unsigned)SCB_NS->BFAR);
        }
        if (cfsr_ns & 0xFFFF0000u) {
            LOG_ERR("  [NS] UFSR:  0x%04x%s%s%s%s%s%s%s",
                (unsigned)((cfsr_ns >> 16) & 0xFFFFu),
                (cfsr_ns & SCB_CFSR_UNDEFINSTR_Msk) ? "  UNDEFINSTR" : "",
                (cfsr_ns & SCB_CFSR_INVSTATE_Msk)   ? "  INVSTATE"   : "",
                (cfsr_ns & SCB_CFSR_INVPC_Msk)      ? "  INVPC"      : "",
                (cfsr_ns & SCB_CFSR_NOCP_Msk)       ? "  NOCP"       : "",
                (cfsr_ns & SCB_CFSR_STKOF_Msk)      ? "  STKOF"      : "",
                (cfsr_ns & SCB_CFSR_UNALIGNED_Msk)  ? "  UNALIGNED"  : "",
                (cfsr_ns & SCB_CFSR_DIVBYZERO_Msk)  ? "  DIVBYZERO"  : "");
        }
    }
}

static __attribute__((noreturn)) void hard_fault_c(fault_frame_t *frame, uint32_t exc_return) {
    LOG_ERR("**** Hard Fault ****");
    fault_dump(frame, exc_return);
    while (1);
}

static __attribute__((noreturn)) void mem_manage_c(fault_frame_t *frame, uint32_t exc_return) {
    LOG_ERR("**** MemManage Fault ****");
    fault_dump(frame, exc_return);
    while (1);
}

static __attribute__((noreturn)) void bus_fault_c(fault_frame_t *frame, uint32_t exc_return) {
    LOG_ERR("**** Bus Fault ****");
    fault_dump(frame, exc_return);
    while (1);
}

static __attribute__((noreturn)) void usage_fault_c(fault_frame_t *frame, uint32_t exc_return) {
    LOG_ERR("**** Usage Fault ****");
    fault_dump(frame, exc_return);
    while (1);
}

static __attribute__((noreturn)) void secure_fault_c(fault_frame_t *frame, uint32_t exc_return) {
    /* Read SFSR/SFAR immediately — sticky bits clear on read */
    uint32_t sfsr = SAU->SFSR;
    uint32_t sfar = SAU->SFAR;
    LOG_ERR("**** Secure Fault ****");
    fault_dump(frame, exc_return);
    LOG_ERR("  SFSR: 0x%08x%s%s%s%s%s",
        (unsigned)sfsr,
        (sfsr & SAU_SFSR_INVEP_Msk)   ? "  INVEP"   : "",
        (sfsr & SAU_SFSR_INVIS_Msk)   ? "  INVIS"   : "",
        (sfsr & SAU_SFSR_INVER_Msk)   ? "  INVER"   : "",
        (sfsr & SAU_SFSR_AUVIOL_Msk)  ? "  AUVIOL"  : "",
        (sfsr & SAU_SFSR_INVTRAN_Msk) ? "  INVTRAN" : "");
    if (sfsr & SAU_SFSR_SFARVALID_Msk)
        LOG_ERR("  SFAR: 0x%08x", (unsigned)sfar);
    while (1);
}

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern UART_HandleTypeDef huart3;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1) {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */
  __asm volatile(
      "tst lr, #4      \n"
      "ite eq          \n"
      "mrseq r0, msp   \n"
      "mrsne r0, psp   \n"
      "mov r1, lr      \n"
      "b hard_fault_c  \n"
  );
  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */
  __asm volatile(
      "tst lr, #4      \n"
      "ite eq          \n"
      "mrseq r0, msp   \n"
      "mrsne r0, psp   \n"
      "mov r1, lr      \n"
      "b mem_manage_c  \n"
  );
  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */
  __asm volatile(
      "tst lr, #4      \n"
      "ite eq          \n"
      "mrseq r0, msp   \n"
      "mrsne r0, psp   \n"
      "mov r1, lr      \n"
      "b bus_fault_c   \n"
  );
  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */
  __asm volatile(
      "tst lr, #4       \n"
      "ite eq           \n"
      "mrseq r0, msp    \n"
      "mrsne r0, psp    \n"
      "mov r1, lr       \n"
      "b usage_fault_c  \n"
  );
  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Secure fault.
  */
void SecureFault_Handler(void)
{
  /* USER CODE BEGIN SecureFault_IRQn 0 */
  __asm volatile(
      "tst lr, #4         \n"
      "ite eq             \n"
      "mrseq r0, msp      \n"
      "mrsne r0, psp      \n"
      "mov r1, lr         \n"
      "b secure_fault_c   \n"
  );
  /* USER CODE END SecureFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_SecureFault_IRQn 0 */
    /* USER CODE END W1_SecureFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32H5xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32h5xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles EXTI Line13 interrupt.
  */
void EXTI13_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI13_IRQn 0 */

  /* USER CODE END EXTI13_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
  /* USER CODE BEGIN EXTI13_IRQn 1 */

  /* USER CODE END EXTI13_IRQn 1 */
}

/**
  * @brief This function handles USART3 global interrupt.
  */
void USART3_IRQHandler(void)
{
  /* USER CODE BEGIN USART3_IRQn 0 */

  /* USER CODE END USART3_IRQn 0 */
  HAL_UART_IRQHandler(&huart3);
  /* USER CODE BEGIN USART3_IRQn 1 */

  /* USER CODE END USART3_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
