#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"
#include <stdlib.h> 
/*----------------------------------------------------------------------------
 *      Thread 2 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/

osThreadId_t tid_Thread2;                        // thread id
int Init_Thread2(void);  
void Thread2 (void *argument);                   // thread function
GPIO_InitTypeDef led_ldm = { 
        .Pin = GPIO_PIN_7,
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = GPIO_NOPULL,
        .Speed = GPIO_SPEED_FREQ_LOW
    };

typedef struct  {
	  GPIO_InitTypeDef pin;
		GPIO_TypeDef *port;
} mygpio_pin2;

mygpio_pin2 pinB1;
int Init_Thread2 (void) {
 
	// Initialize LEDS
    __HAL_RCC_GPIOB_CLK_ENABLE();
	pinB1.pin= led_ldm;
	pinB1.port=GPIOB;
  tid_Thread2 = osThreadNew(Thread2, (void *)&pinB1, NULL);
  if (tid_Thread2 == NULL) {
    return(-1);
  }
  return(0);
}
 
void Thread2 (void *argument) {
  mygpio_pin2 *gpio = (mygpio_pin2 *)argument;
	HAL_GPIO_Init(gpio->port, &(gpio->pin));	
  while (1) {
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7  , GPIO_PIN_SET); 
		osDelay(137);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7 , GPIO_PIN_RESET); 
		osDelay(137);
  }
}
