#include "stimer.h"
#include "Arduino.h"
#define TIMER_EXPIRED(timer) ((uint32_t)(millis() - timer.start) >= timer.time)

typedef struct sTimer {
	uint32_t start;
	uint32_t time;
	TimerCallback callback;
	void* params;
} sTimer;

static sTimer softTimer[MAX_TIMER_COUNT] = {0};

void startTimer(uint32_t ms, TimerCallback callback, void* data) {
    int index;
	for (index = 0; index < MAX_TIMER_COUNT; index++) {
		if ((softTimer[index].callback == NULL) || (softTimer[index].callback == callback)) {
			softTimer[index].start = millis();
			softTimer[index].callback = callback;
			softTimer[index].time = ms;
			softTimer[index].params = data;

			for (index = index + 1; index < MAX_TIMER_COUNT; index++) {
				if (softTimer[index].callback == callback)
					softTimer[index].callback = NULL;
			}
			return;
		}
	}
}

void cancelTimer(TimerCallback callback) {
    int index;
	for (index = 0; index < MAX_TIMER_COUNT; index++) {
		if (softTimer[index].callback == callback) {
			softTimer[index].callback = NULL;
			break;
		}
	}
}

bool isTimerRunning(TimerCallback callback) {
    int index;
	for (index = 0; index < MAX_TIMER_COUNT; index++) {
		if (softTimer[index].callback == callback)
			return true;
	}
	return false;
}

void processTimerEvents(void) {
    int index;
	static void* param = NULL;
	static TimerCallback callback = NULL;

    for (index = 0; index < MAX_TIMER_COUNT; index++)
    {
        if ((softTimer[index].callback != NULL) && TIMER_EXPIRED(softTimer[index]))
        {
            callback = softTimer[index].callback;
            param = softTimer[index].params;
            softTimer[index].callback = NULL;
            callback(param);
            break;
        }
    }
}