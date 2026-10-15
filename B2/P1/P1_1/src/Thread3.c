#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"
#include <stdlib.h> 
/*----------------------------------------------------------------------------
 *      Thread 3 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/

osThreadId_t tid_Thread3;                        // thread id
int Init_Thread3 (void);  
void Thread3 (void *argument);                   // thread function
GPIO_InitTypeDef led_ldd = { 
        .Pin = GPIO_PIN_14,
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = GPIO_NOPULL,
        .Speed = GPIO_SPEED_FREQ_LOW
    };

typedef struct  {
	  GPIO_InitTypeDef pin;
		GPIO_TypeDef *port;
} mygpio_pin3;

mygpio_pin3 pinB2;
int Init_Thread3 (void) {
 
	// Initialize LEDS
    __HAL_RCC_GPIOB_CLK_ENABLE();
	pinB2.pin= led_ldd;
	pinB2.port=GPIOB;
  tid_Thread3 = osThreadNew(Thread3, (void *)&pinB2, NULL);
  if (tid_Thread3 == NULL) {
    return(-1);
  }
  return(0);
}
 
void Thread3 (void *argument) {
  mygpio_pin3 *gpio = (mygpio_pin3 *)argument;
	HAL_GPIO_Init(gpio->port, &(gpio->pin));	
  while (1) {
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14  , GPIO_PIN_SET); 
		osDelay(287);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14 , GPIO_PIN_RESET); 
		osDelay(287);
  }
}
