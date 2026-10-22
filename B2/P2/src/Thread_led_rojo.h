#ifndef THREAD_LED_ROJO_H
#define THREAD_LED_ROJO_H

#include "cmsis_os2.h"

/* ====================== FLAGS DEL LED ROJO ====================== */

// Señal que indica que debemos encender el LED rojo
#define FLAG_ENCENDER 0x01U

/* ====================== IDENTIFICADOR DEL HILO ====================== */

// Identificador definido en Thread_led_rojo.c
extern osThreadId_t tid_Rojo;

/* ====================== PROTOTIPOS DE FUNCIONES ====================== */

// Función encargada de crear el hilo rojo
int Init_Thread_Rojo(void);

#endif