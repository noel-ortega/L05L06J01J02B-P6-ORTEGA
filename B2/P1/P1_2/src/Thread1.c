#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"
#include <stdlib.h> 
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/

osThreadId_t tid_Thread1;                        // thread id
int Init_Thread (void);  
void Thread1 (void *argument);  // thread function
static int ciclosLed=0;
extern osThreadId_t tid_Thread2;


GPIO_InitTypeDef led_ld1 = {
        .Pin = GPIO_PIN_0,
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = GPIO_NOPULL,
        .Speed = GPIO_SPEED_FREQ_LOW
    };

typedef struct  {
	  GPIO_InitTypeDef pin;
		GPIO_TypeDef *port;
} mygpio_pin;

mygpio_pin pinB0;
int Init_Thread (void) {
	// Initialize LEDS
    __HAL_RCC_GPIOB_CLK_ENABLE();
	pinB0.pin= led_ld1;
	pinB0.port=GPIOB;
  tid_Thread1 = osThreadNew(Thread1, (void *)&pinB0, NULL);
  if (tid_Thread1 == NULL) {
    return(-1);
  }
  return(0);
}
 
void Thread1 (void *argument) {
  mygpio_pin *gpio = (mygpio_pin *)argument;
	HAL_GPIO_Init(gpio->port, &(gpio->pin));	
  while (1) {
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0  , GPIO_PIN_SET); 
		osDelay(200);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 , GPIO_PIN_RESET); 
		osDelay(800);
		ciclosLed++;
		if(ciclosLed == 5){
			osThreadFlagsSet(tid_Thread2, 0x0001);
		}
	
  }
}
