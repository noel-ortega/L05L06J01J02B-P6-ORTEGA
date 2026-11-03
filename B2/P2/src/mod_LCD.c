 /* ** Fichero mod_LCD .c ** */

 // Creelo , se añadira aquí todo el código que maneja el LCD

 // Debe contener dos threads , uno de gestión y otro de test
 // Tambi én se irán añadiendo las funciones de inicialización del LCD
// Thread de gestion

#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"
#include <stdlib.h> 
#include "Driver_SPI.h"


extern ARM_DRIVER_SPI Driver_SPI0;
ARM_DRIVER_SPI* SPIdrv = &Driver_SPI0;

osThreadId_t tid_Thread;                        // thread id
int Init_Thread (void);  
void TH_LCD (void *argument); 
void TH_LCD_TEST (void *argument);
void cb_spi(uint32_t);
void LCD_reset();

uint32_t spi_errors = 0;


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
	LCD_reset();
  while (1) {
  }
}

void TH_LCD_TEST(void *argument){
	while(1){
	}
}

void LCD_reset(){

	//LCD RESET
	
	osDelay(1);
	
	//CONFIGURACION DE SPI
SPIdrv->Initialize(cb_spi);
SPIdrv->PowerControl(ARM_POWER_FULL);
SPIdrv->Control(ARM_SPI_MODE_MASTER | ARM_SPI_CPOL1_CPHA1 | ARM_SPI_MSB_LSB |
ARM_SPI_SS_MASTER_SW | ARM_SPI_DATA_BITS(8), 20000000);
SPIdrv->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE);
	
	//CONFIGURACION DE LOS PINES GPIO
GPIO_InitTypeDef pin_reset = { 
        .Pin = GPIO_PIN_12,
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = GPIO_PULLUP,
        .Speed = GPIO_SPEED_FREQ_LOW
    };
GPIO_InitTypeDef pin_A0 = { 
        .Pin = GPIO_PIN_7,
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = GPIO_PULLUP,
        .Speed = GPIO_SPEED_FREQ_LOW
    };
GPIO_InitTypeDef pin_CS = { 
        .Pin = GPIO_PIN_10,
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = GPIO_PULLUP,
        .Speed = GPIO_SPEED_FREQ_LOW
    };

		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
		osDelay(1);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
		osDelay(1);

	
		
}


void cb_spi (uint32_t event) {
	
if (event & ARM_SPI_EVENT_TRANSFER_COMPLETE) {
osThreadFlagsSet(tid_Thread, 0x0001);
}
if (event & ARM_SPI_EVENT_DATA_LOST) {
spi_errors++; // pérdida de datos
}
if (event & ARM_SPI_EVENT_MODE_FAULT) {
spi_errors++; // conflicto de bus
}
}

void LCD_wr_data(unsigned char data){
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_RESET); //CS
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET); //A0
		  SPIdrv->Send(&data, sizeof(data)); //enviar dato
	    osThreadFlagsWait(ARM_SPI_EVENT_TRANSFER_COMPLETE, osFlagsWaitAny, osWaitForever);
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_SET);//CS
}

void LCD_wr_cmd(unsigned char cmd){
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_RESET); //CS
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET); //A0
		  SPIdrv->Send(&cmd, sizeof(cmd)); //enviar dato
	    osThreadFlagsWait(ARM_SPI_EVENT_TRANSFER_COMPLETE, osFlagsWaitAny, osWaitForever);
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_SET);//CS
}


