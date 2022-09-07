#ifndef _MYTIMER_H_
#define _MYTIMER_H_

unsigned int ST_CreateAppTimer(const char* pcName, void (*ExpirationRoutine)(int),  unsigned long ulInitialTime, unsigned long ulRescheduleTime, int uiEnable, int* phOSHandle);
unsigned int ST_DeleteAppTimer(int* phOSHandle);
unsigned int ST_ControlAppTimer(int hOSHandle, unsigned int uiEnable);
unsigned int ST_ResetAppTimer(int hOSHandle, void (*ExpirationRoutine)(int), unsigned long ulInitialTime, unsigned long ulRescheduleTime, unsigned int uiEnable);
extern int timer_item;

#endif
