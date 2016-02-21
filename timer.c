#include "base.h"
#include "timer.h"
#include <SDL2/SDL.h>


/*
 * init_timer
 */
void init_timer(struct timer *timer)
{
	assert(timer != NULL);

	timer->startTicks = 0;
	timer->pausedTicks = 0;
	timer->paused = false;
	timer->started = false;
}

/*
 * start_timer
 */
void start_timer(struct timer *timer)
{
	assert(timer != NULL);

	timer->started = true;
	timer->paused = false;

	timer->startTicks = SDL_GetTicks();
	timer->pausedTicks = 0;
}

/*
 * stop_timer
 */
void stop_timer(struct timer *timer)
{
	assert(timer != NULL);

	timer->started = false;
	timer->paused = false;
	timer->startTicks = timer->pausedTicks = 0;
}

/*
 * pause_timer
 */
void pause_timer(struct timer *timer)
{
	assert(timer != NULL);

	if (timer->started && !timer->paused) {
		timer->paused = true;
		timer->pausedTicks = SDL_GetTicks() - timer->startTicks;
		timer->startTicks = 0;
	}
}

/*
 * unpause_timer
 */
void unpause_timer(struct timer *timer)
{
	assert(timer != NULL);

	if (timer->started && timer->paused) {
		timer->paused = false;
		timer->startTicks = SDL_GetTicks() - timer->pausedTicks;
		timer->pausedTicks = 0;
	}
}

/*
 * timer_get_ticks
 */
uint32_t timer_get_ticks(struct timer *timer)
{
	assert(timer != NULL);

	uint32_t time = 0;

	if (timer->started) {
		if (timer->paused)
			time = timer->pausedTicks;
		else
			time = SDL_GetTicks() - timer->startTicks;
	}

	return time;
}

/*
 * timer_is_started
 */
bool timer_is_started(struct timer *timer)
{
	assert(timer != NULL);
	return timer->started;
}

/*
 * timer_is_paused
 */
bool timer_is_paused(struct timer *timer)
{
	assert(timer != NULL);
	return timer->paused && timer->started;
}
