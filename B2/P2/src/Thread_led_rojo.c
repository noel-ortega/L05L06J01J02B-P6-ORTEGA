#include "cmsis_os2.h"
#include "stm32f4xx_hal.h"
#include "Thread_led_rojo.h"

/*----------------------------------------------------------------------------
 * Thread LED Rojo: Controla el encendido del LED LD3 (PB14)
 *---------------------------------------------------------------------------*/

/* ====================== IDENTIFICADOR RTOS ====================== */

// Identificador del hilo encargado del LED rojo
osThreadId_t tid_Rojo;

/* ====================== PROTOTIPOS DE FUNCIONES ====================== */

// Función que ejecutará el hilo del LED rojo
void Thread_Rojo(void *argument);

/* ====================== INICIALIZACIÓN DEL HILO ====================== */

int Init_Thread_Rojo(void)
{
    // Habilitamos el reloj del puerto GPIOB
    __HAL_RCC_GPIOB_CLK_ENABLE();

    // Creamos el hilo rojo y guardamos su identificador
    tid_Rojo = osThreadNew(Thread_Rojo, NULL, NULL);

    // Comprobamos que se ha creado correctamente
    if (tid_Rojo == NULL)
    {
        return -1;
    }

    return 0;
}

/* ====================== HILO LED ROJO ====================== */

void Thread_Rojo(void *argument)
{
	(void)argument;
    // Estructura para configurar el LED rojo
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = GPIO_PIN_14;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    // Aplicamos la configuración al pin PB14
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // Mantenemos el LED rojo apagado inicialmente
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);

    // Esperamos hasta recibir la señal del contador
    osThreadFlagsWait(FLAG_ENCENDER, osFlagsWaitAny, osWaitForever);

    // Encendemos definitivamente el LED rojo
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);

    // Finalizamos el hilo
    osThreadExit();
}