#ifndef THREAD_CONTADOR_H
#define THREAD_CONTADOR_H

#include "cmsis_os2.h"

/* ====================== FLAGS DEL CONTADOR ====================== */

#define FLAG_REINICIAR 0x01U
#define FLAG_BLOQUEAR  0x02U

/* ====================== IDENTIFICADOR DEL HILO ====================== */

// Identificador definido en Thread_Contador.c
extern osThreadId_t tid_Contador;

/* ====================== PROTOTIPOS DE FUNCIONES ====================== */

int Init_Thread(void);

#endif