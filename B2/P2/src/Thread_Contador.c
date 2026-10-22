#include "cmsis_os2.h"
#include "stm32f4xx_hal.h"

#include "Thread_Contador.h"
#include "Thread_led_verde.h"
#include "Thread_led_rojo.h"

/*----------------------------------------------------------------------------
 * Thread Contador: Encargado de gestionar la cuenta del sistema
 *---------------------------------------------------------------------------*/

/* ========================= IDENTIFICADORES RTOS ========================= */

// Identificador del hilo contador
osThreadId_t tid_Contador;

// Identificador del timer virtual
osTimerId_t timer_id;

/* ====================== PROTOTIPOS DE FUNCIONES ====================== */

// Función encargada de crear el hilo y el temporizador
int Init_Thread(void);

// Función que ejecutará el hilo contador
void Thread_Contador(void *argument);

// Función ejecutada cuando vence el timer
void Timer_Callback(void *argument);

/* ====================== INICIALIZACIÓN DEL HILO ====================== */

int Init_Thread(void)
{
    // Creamos el timer virtual de un solo disparo
    timer_id = osTimerNew(Timer_Callback, osTimerOnce, NULL, NULL);

    // Comprobamos que se ha creado correctamente
    if (timer_id == NULL)
    {
        return -1;
    }

    // Creamos el hilo contador
    tid_Contador = osThreadNew(Thread_Contador, NULL, NULL);

    // Comprobamos que se ha creado correctamente
    if (tid_Contador == NULL)
    {
        osTimerDelete(timer_id);
        timer_id = NULL;
        return -1;
    }

    return 0;
}

/* ====================== HILO CONTADOR ====================== */

void Thread_Contador(void *argument)
{
    (void)argument;

    // Variable que almacena las flags recibidas
    uint32_t flag;

    // Calculamos los ticks equivalentes a 3 segundos
    uint32_t tiempo = 3U * osKernelGetTickFreq();

    // Comprobamos que el periodo es válido
    if (tiempo == 0U)
    {
        osThreadExit();
    }

    // Iniciamos la cuenta de 3 segundos
    if (osTimerStart(timer_id, tiempo) != osOK)
    {
        osThreadExit();
    }

    while (1)
    {
        // Esperamos una pulsación o el fin del temporizador
        flag = osThreadFlagsWait(
            FLAG_REINICIAR | FLAG_BLOQUEAR,
            osFlagsWaitAny,
            osWaitForever
        );

        // Comprobamos si se ha producido un error
        if (flag & osFlagsError)
        {
            break;
        }

        // Si ha terminado la cuenta, bloqueamos el sistema
        if (flag & FLAG_BLOQUEAR)
        {
            // Deshabilitamos la interrupción del pulsador
            HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);

            // Avisamos al hilo verde para que se apague
            osThreadFlagsSet(tid_Verde, FLAG_APAGAR);

            // Avisamos al hilo rojo para que se encienda
            osThreadFlagsSet(tid_Rojo, FLAG_ENCENDER);

            // Salimos del bucle y finalizamos el hilo
            break;
        }
        else if (flag & FLAG_REINICIAR)
        {
            // Reiniciamos la cuenta de 3 segundos
            if (osTimerStart(timer_id, tiempo) != osOK)
            {
                break;
            }
        }
    }

    // Finalizamos el hilo contador
    osThreadExit();
}

/* ====================== CALLBACK DEL TIMER ====================== */

void Timer_Callback(void *argument)
{
    (void)argument;

    // Avisamos al hilo contador de que han terminado los 3 segundos
    osThreadFlagsSet(tid_Contador, FLAG_BLOQUEAR);
}