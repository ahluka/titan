#pragma once

struct Timer {
	uint32_t startTicks;
	uint32_t pausedTicks;
	bool paused;
	bool started;
};

void Timer_Init(struct Timer *timer);
void Timer_Start(struct Timer *timer);
void Timer_Stop(struct Timer *timer);
void Timer_Pause(struct Timer *timer);
void Timer_Unpause(struct Timer *timer);
uint32_t Timer_GetTicks(struct Timer *timer);
bool Timer_IsStarted(struct Timer *timer);
bool Timer_IsPaused(struct Timer *timer);
