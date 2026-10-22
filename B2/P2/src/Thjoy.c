#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"
#include <stdlib.h> 
/*----------------------------------------------------------------------------
 *      Thjoy manejará toda la funcionalidad del joystick
 *---------------------------------------------------------------------------*/

/* =========================== FLAGS =========================== */

#define FLAG_JOYSTICK 0x01
#define FLAG_TIMER	  0x02

/* =================== IDENTIFICADORES RTOS ==================== */

osThreadId_t tid_Thjoy;
osTimerId_t  timer_id;

/* ================= VARIABLES DEL JOYSTICK ================= */

volatile GPIO_PinState arriba;
volatile GPIO_PinState derecha;
volatile GPIO_PinState abajo;
volatile GPIO_PinState izquierda;
volatile GPIO_PinState centro;

volatile uint32_t contador_arriba = 0;
volatile uint32_t contador_derecha = 0;
volatile uint32_t contador_abajo = 0;
volatile uint32_t contador_izquierda = 0;
volatile uint32_t contador_centro = 0;

/* ================== PROTOTIPOS DE FUNCIONES ================== */

int Init_Thjoy(void);

void Thread_Joystick(void *argument);
void Timer_Callback(void *argument);
void Joystick_GPIO_Init(void);

/* ================== INICIALIZACIÓN DEL HILO ================== */

int Init_Thjoy(void){
	timer_id = osTimerNew(Timer_Callback, osTimerOnce, NULL, NULL);
	if(timer_id == NULL){
		return -1;
	}
	tid_Thjoy = osThreadNew(Thread_Joystick, NULL, NULL);
	if(tid_Thjoy == NULL){
		osTimerDelete(timer_id);
		timer_id = NULL;
		return -1;
	}
	Joystick_GPIO_Init();
	return 0;
}

/* =================== INICIALIZACIÓN GPIO ==================== */

void Joystick_GPIO_Init(void){
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	
	GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_11;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_14 | GPIO_PIN_15;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	
	//INICIAR LOS LEDS
  GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_7 | GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* ======================== HILO THJOY ======================== */

void Thread_Joystick(void *argument){
	(void)argument;
	uint32_t flag;
	uint32_t tiempo = osKernelGetTickFreq()/20U;
	if(tiempo == 0U){
		osThreadExit();
	}
	while(1){
		flag = osThreadFlagsWait(FLAG_JOYSTICK | FLAG_TIMER, osFlagsWaitAny, osWaitForever);
		if(flag & osFlagsError){
			osThreadExit();
		}
		if(flag & FLAG_JOYSTICK){
			if(osTimerStart(timer_id, tiempo) != osOK){
				osThreadExit();
			}
		}
		else if (flag & FLAG_TIMER){
			//Aquí ya tenemos nuestro pulsador sin rebotes
			arriba = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10);
      derecha = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11);
			abajo = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_12);
			izquierda = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_14);
			centro = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_15);
			if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_10)){ //ARRIBA
		    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET); //0
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET); //0
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET); //1
				contador_arriba++;
			
			}else if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_11)){ //DERECHA
		    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET); //0
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET); //1
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET); //0
				contador_derecha++;
				
			}else if(HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_12)){ //ABAJO
		    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET); //0
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET); //1
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET); //1
				contador_abajo++;
				
			}else if(HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_14)){ //IZQUIERDA
		    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET); //1
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET); //0
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET); //0
				contador_izquierda++;
				
			}else if(HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_15)){ //CENTER
		    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET); //1
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET); //0
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET); //1
				contador_centro++;
			}
			else if(arriba   == GPIO_PIN_RESET &&
						 derecha   == GPIO_PIN_RESET &&
						 abajo     == GPIO_PIN_RESET &&
					   izquierda == GPIO_PIN_RESET &&
						 centro    == GPIO_PIN_RESET){

				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 | GPIO_PIN_7 | GPIO_PIN_14, GPIO_PIN_RESET);
			}
		} // 		else if (flag & FLAG_TIMER){
	} // 	while(1){
} // void Thread_Joystick(void *argument){




/* =================== CALLBACK TIMER ==================== */

void Timer_Callback(void *argument){
	(void)argument;
	osThreadFlagsSet(tid_Thjoy, FLAG_TIMER);
}

/* =================== CALLBACK EXTI ==================== */

void EXTI15_10_IRQHandler(void){

    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_14);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if(GPIO_Pin == GPIO_PIN_10 ||
		 GPIO_Pin == GPIO_PIN_11 ||
		 GPIO_Pin == GPIO_PIN_12 ||
		 GPIO_Pin == GPIO_PIN_14 ||
		 GPIO_Pin == GPIO_PIN_15){
			osThreadFlagsSet(tid_Thjoy, FLAG_JOYSTICK);
		 }
}