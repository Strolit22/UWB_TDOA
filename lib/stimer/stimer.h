#ifndef __STIMER_H__
#define __STIMER_H__

#include <stdint.h>
#include <stdbool.h>

#define MAX_TIMER_COUNT		    32

typedef void(*TimerCallback)(void*);

void startTimer(uint32_t millis, TimerCallback callback, void* data);
void cancelTimer(TimerCallback callback);
bool isTimerRunning(TimerCallback callback);
void processTimerEvents(void);

#endif //__STIMER_H__