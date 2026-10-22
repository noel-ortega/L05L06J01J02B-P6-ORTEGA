#ifndef THREAD_LED_VERDE_H
#define THREAD_LED_VERDE_H

#include "cmsis_os2.h"

#define FLAG_APAGAR 0x01U

extern osThreadId_t tid_Verde;

int Init_Thread_Verde(void);

#endif