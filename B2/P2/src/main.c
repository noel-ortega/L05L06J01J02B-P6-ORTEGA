
/**
  ******************************************************************************
  * @file    main.c
  * @brief   B2 - Practica 1 - Ejercicio 4
  *
  * Sistema con tres hilos:
  *   - Thread_Verde: controla el LED verde (PB0).
  *   - Thread_Rojo: controla el LED rojo (PB14).
  *   - Thread_Contador: gestiona un timer virtual de 3 segundos.
  *
  * El pulsador B1 (PC13) reinicia la cuenta mediante una interrupcion.
  * Si pasan 3 segundos sin pulsaciones, el sistema se bloquea.
  ******************************************************************************
  */

/* ====================== LIBRERIAS ====================== */

#include "main.h"

#ifdef _RTE_
#include "RTE_Components.h"
#endif

#ifdef RTE_CMSIS_RTOS2
#include "cmsis_os2.h"
#endif

#ifdef RTE_CMSIS_RTOS2_RTX5

uint32_t HAL_GetTick(void)
{
    static uint32_t ticks = 0U;
    uint32_t i;

    if (osKernelGetState() == osKernelRunning)
    {
        return ((uint32_t)osKernelGetTickCount());
    }

    for (i = (SystemCoreClock >> 14U); i > 0U; i--)
    {
        __NOP(); __NOP(); __NOP(); __NOP();
        __NOP(); __NOP(); __NOP(); __NOP();
        __NOP(); __NOP(); __NOP(); __NOP();
    }

    return ++ticks;
}

HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
    UNUSED(TickPriority);
    return HAL_OK;
}

#endif

/* ====================== ARCHIVOS DE LOS HILOS ====================== */

#include "Thread_Contador.h"
#include "Thread_led_verde.h"
#include "Thread_led_rojo.h"

/* ====================== PROTOTIPOS ====================== */

static void SystemClock_Config(void);
static void Error_Handler(void);

/* ====================== PROGRAMA PRINCIPAL ====================== */

int main(void)
{
    // Estructura utilizada para configurar el pulsador
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* ====================== INICIALIZACION HAL ====================== */

    // Inicializamos la biblioteca HAL
    HAL_Init();

#ifndef SIMULATOR
    // Configuramos el reloj cuando utilizamos la placa real
    SystemClock_Config();
#endif

    // Actualizamos la frecuencia del nucleo
    SystemCoreClockUpdate();

    /* ====================== CONFIGURACION DEL PULSADOR ====================== */

    // Habilitamos el reloj del puerto GPIOC
    __HAL_RCC_GPIOC_CLK_ENABLE();

    // Configuramos PC13 para detectar flancos ascendentes
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;

    // Aplicamos la configuracion
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    // Configuramos la prioridad de EXTI13
    // Utilizamos una prioridad baja para las llamadas al RTOS
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 15, 0);

    /* ====================== INICIALIZACION RTOS ====================== */

#ifdef RTE_CMSIS_RTOS2

    // Inicializamos el nucleo del sistema operativo
    if (osKernelInitialize() != osOK)
    {
        Error_Handler();
    }

    /* ====================== CREACION DE HILOS ====================== */

    // Creamos el hilo encargado del LED verde
    if (Init_Thread_Verde() != 0)
    {
        Error_Handler();
    }

    // Creamos el hilo encargado del LED rojo
    if (Init_Thread_Rojo() != 0)
    {
        Error_Handler();
    }

    // Creamos el hilo contador y su timer virtual
    if (Init_Thread() != 0)
    {
        Error_Handler();
    }

    /* ====================== HABILITACION DE INTERRUPCIONES ====================== */

    // Limpiamos posibles interrupciones pendientes
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_13);
    HAL_NVIC_ClearPendingIRQ(EXTI15_10_IRQn);

    // Permitimos atender las interrupciones del pulsador
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

    /* ====================== INICIO DEL SISTEMA OPERATIVO ====================== */

    // Iniciamos el planificador del RTOS
    if (osKernelStart() != osOK)
    {
        Error_Handler();
    }

#endif

    /* ====================== BUCLE PRINCIPAL ====================== */

    // En funcionamiento normal con RTOS no llegaremos aqui
    while (1)
    {
    }
}

/* ====================== INTERRUPCION DEL PULSADOR ====================== */

void EXTI15_10_IRQHandler(void)
{
    // HAL comprueba y limpia la interrupcion de EXTI13
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
}

/* ====================== CALLBACK DEL PULSADOR ====================== */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
#ifdef RTE_CMSIS_RTOS2

    // Comprobamos que la interrupcion pertenece a PC13
    if (GPIO_Pin == GPIO_PIN_13)
    {
        // Comprobamos que el sistema operativo esta funcionando
        // y que el hilo contador tiene un identificador valido
        if (osKernelGetState() == osKernelRunning &&
            tid_Contador != NULL)
        {
            // Avisamos al contador para reiniciar los 3 segundos
            osThreadFlagsSet(tid_Contador, FLAG_REINICIAR);
        }
    }

#else
    UNUSED(GPIO_Pin);
#endif
}

/* ====================== CONFIGURACION DEL RELOJ ====================== */

static void SystemClock_Config(void)
{
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};

    // Habilitamos el reloj del controlador de alimentacion
    __HAL_RCC_PWR_CLK_ENABLE();

    // Configuramos la escala de tension
    __HAL_PWR_VOLTAGESCALING_CONFIG(
        PWR_REGULATOR_VOLTAGE_SCALE1
    );

    /* ====================== CONFIGURACION DEL OSCILADOR ====================== */

    // Utilizamos HSE con bypass y PLL
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;

    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;

    // Configuracion para SYSCLK de 168 MHz con HSE de 8 MHz
    RCC_OscInitStruct.PLL.PLLM = 4;
    RCC_OscInitStruct.PLL.PLLN = 168;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 7;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /* ====================== CONFIGURACION DE BUSES ====================== */

    RCC_ClkInitStruct.ClockType =
        RCC_CLOCKTYPE_SYSCLK |
        RCC_CLOCKTYPE_HCLK |
        RCC_CLOCKTYPE_PCLK1 |
        RCC_CLOCKTYPE_PCLK2;

    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(
        &RCC_ClkInitStruct,
        FLASH_LATENCY_5
    ) != HAL_OK)
    {
        Error_Handler();
    }

    // Activamos prefetch si corresponde a la revision del chip
    if (HAL_GetREVID() == 0x1001)
    {
        __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
    }
}

/* ====================== MANEJADOR DE ERRORES ====================== */

static void Error_Handler(void)
{
    // Detenemos el programa si ocurre un error de inicializacion
    while (1)
    {
    }
}

#ifdef USE_FULL_ASSERT

void assert_failed(uint8_t* file, uint32_t line)
{
    UNUSED(file);
    UNUSED(line);

    while (1)
    {
    }
}

#endif