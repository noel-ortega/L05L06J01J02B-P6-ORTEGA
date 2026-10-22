#include "cmsis_os2.h"
#include "stm32f4xx_hal.h"
#include "Thread_led_verde.h"

/*----------------------------------------------------------------------------
 * Thread LED Verde: Controla el encendido y apagado del LED LD1 (PB0)
 *---------------------------------------------------------------------------*/

/* ====================== IDENTIFICADOR RTOS ====================== */

// Identificador del hilo encargado del LED verde
osThreadId_t tid_Verde;

/* ====================== PROTOTIPOS DE FUNCIONES ====================== */

// Función que ejecutará el hilo del LED verde
void Thread_Verde(void *argument);

/* ====================== INICIALIZACIÓN DEL HILO ====================== */

int Init_Thread_Verde(void)
{
    // Habilitamos el reloj del puerto GPIOB
    __HAL_RCC_GPIOB_CLK_ENABLE();

    // Creamos el hilo verde y guardamos su identificador
    tid_Verde = osThreadNew(Thread_Verde, NULL, NULL);

    // Comprobamos que se haya creado correctamente
    if (tid_Verde == NULL)
    {
        return -1;
    }

    return 0;
}

/* ====================== HILO LED VERDE ====================== */

void Thread_Verde(void *argument)
{
	(void)argument;
    // Estructura para configurar el pin del LED verde
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    // Aplicamos la configuración al pin PB0
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // Encendemos el LED verde al arrancar
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);

    // Esperamos a recibir la orden de apagar el LED
    osThreadFlagsWait(FLAG_APAGAR, osFlagsWaitAny, osWaitForever);

    // Apagamos definitivamente el LED verde
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);

    // Finalizamos el hilo
    osThreadExit();
}