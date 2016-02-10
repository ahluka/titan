#include "base.h"
#include "event.h"
#include "memory.h"
#include "panic.h"
#include "globals.h"

/* There are two event queues, one for events to be processed next frame
 * ("queued events"), and one for events that are timestamped ("future events").
 */
#define MAX_QUEUED_EVENTS 64
UNUSED static Event *s_QueuedEvents[MAX_QUEUED_EVENTS] = {NULL};
UNUSED static Event *s_FutureEvents[MAX_QUEUED_EVENTS] = {NULL};
static MemPool *s_EventPool = NULL;

/*
 * ValidFlags
 *	Return true if the given event flags are sane.
 */
static bool ValidFlags(uint32_t flags)
{
	if ((flags & EVENT_QUEUED) && (flags & EVENT_FUTURE)) {
		return false;
	}

	return true;
}

/*
 * Evt_Init
 */
ecode_t Evt_Init()
{
	if (s_EventPool != NULL) {
		Trace(CHAN_INFO, "Event system already initialised");
		return EFAIL;
	}

	s_EventPool = Pool_Create(MAX_QUEUED_EVENTS, sizeof(Event),
		POOL_DYNGROW, "EventPool");

	return EOK;
}

/*
 * Evt_Shutdown
 */
ecode_t Evt_Shutdown()
{
	if (!s_EventPool) {
		Trace(CHAN_INFO, "Event system doesn't need shutting down");
		return EFAIL;
	}

	Pool_Destroy(s_EventPool);
	s_EventPool = NULL;

	return EOK;
}

/*
 * Evt_Create
 */
Event *Evt_Create(const char *name, uint32_t flags)
{
        assert(name != NULL);

        if (!s_EventPool) {
                Panic("Event system not initialised");
        }

        if (!ValidFlags(flags)) {
                Panic("Invalid flags");
        }

        Event *evt = PAlloc(s_EventPool);
        evt->name = name;
        evt->flags = flags;

        return evt;
}

/*
 * Evt_Enqueue
 */
ecode_t Evt_Enqueue(Event *event)
{
	assert(event != NULL);

	return EOK;
}

/*
 * Evt_Process
 */
ecode_t Evt_Process()
{
	// TODO: PFree() events after they've been processed.
	return EOK;
}
