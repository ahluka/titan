#include "base.h"
#include "timer.h"
#include <SDL2/SDL.h>


/*
 * Timer_Init
 */
void Timer_Init(struct Timer *timer)
{
	assert(timer != NULL);

	timer->startTicks = 0;
	timer->pausedTicks = 0;
	timer->paused = false;
	timer->started = false;
}

/*
 * Timer_Start
 */
void Timer_Start(struct Timer *timer)
{
	assert(timer != NULL);

	timer->started = true;
	timer->paused = false;

	timer->startTicks = SDL_GetTicks();
	timer->pausedTicks = 0;
}

/*
 * Timer_Stop
 */
void Timer_Stop(struct Timer *timer)
{
	assert(timer != NULL);

	timer->started = false;
	timer->paused = false;
	timer->startTicks = timer->pausedTicks = 0;
}

/*
 * Timer_Pause
 */
void Timer_Pause(struct Timer *timer)
{
	assert(timer != NULL);

	if (timer->started && !timer->paused) {
		timer->paused = true;
		timer->pausedTicks = SDL_GetTicks() - timer->startTicks;
		timer->startTicks = 0;
	}
}

/*
 * Timer_Unpause
 */
void Timer_Unpause(struct Timer *timer)
{
	assert(timer != NULL);

	if (timer->started && timer->paused) {
		timer->paused = false;
		timer->startTicks = SDL_GetTicks() - timer->pausedTicks;
		timer->pausedTicks = 0;
	}
}

/*
 * Timer_GetTicks
 */
uint32_t Timer_GetTicks(struct Timer *timer)
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
 * Timer_IsStarted
 */
bool Timer_IsStarted(struct Timer *timer)
{
	assert(timer != NULL);
	return timer->started;
}

/*
 * Timer_IsPaused
 */
bool Timer_IsPaused(struct Timer *timer)
{
	assert(timer != NULL);
	return timer->paused && timer->started;
}
