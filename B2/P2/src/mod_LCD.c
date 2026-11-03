 /* ** Fichero mod_LCD .c ** */

 // Creelo , se añadira aquí todo el código que maneja el LCD

 // Debe contener dos threads , uno de gestión y otro de test
 // Tambi én se irán añadiendo las funciones de inicialización del LCD
// Thread de gestion

#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"
#include <stdlib.h> 


osThreadId_t tid_Thread;                        // thread id
int Init_Thread (void);  
void TH_LCD (void *argument); 
void TH_LCD_TEST (void *argument);


typedef struct  {
	  GPIO_InitTypeDef pin;
		GPIO_TypeDef *port;
		int delay;
		uint8_t counter;
} mygpio_pin;


int Init_Thread_th (void) {
	
	return NULL;
}
 
void TH_LCD (void *argument) {
  while (1) {
  }
}

void TH_LCD_TEST(void *argument){
	while(1){
	}
}

