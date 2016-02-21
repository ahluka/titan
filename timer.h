#pragma once

/* Multipliers for clarity */
#define MS_SECONDS	1000

struct timer {
	uint32_t startTicks;
	uint32_t pausedTicks;
	bool paused;
	bool started;
};

void init_timer(struct timer *timer);
void start_timer(struct timer *timer);
void stop_timer(struct timer *timer);
void pause_timer(struct timer *timer);
void unpause_timer(struct timer *timer);
uint32_t timer_get_ticks(struct timer *timer);
bool timer_is_started(struct timer *timer);
bool timer_is_paused(struct timer *timer);
