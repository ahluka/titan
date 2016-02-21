#include "base.h"
#include "event.h"
#include "memory.h"
#include "panic.h"
#include "globals.h"

/* There are two event queues, one for events to be processed next frame
 * ("queued events"), and one for events that are timestamped ("future events").
 */
#define MAX_QUEUED_EVENTS 64
UNUSED static event_t *s_QueuedEvents[MAX_QUEUED_EVENTS] = {NULL};
UNUSED static event_t *s_FutureEvents[MAX_QUEUED_EVENTS] = {NULL};
static mem_pool_t *s_EventPool = NULL;

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
 * init_events
 */
ecode_t init_events()
{
	if (s_EventPool != NULL) {
		trace(CHAN_INFO, "event system already initialised");
		return EFAIL;
	}

	s_EventPool = create_pool(MAX_QUEUED_EVENTS, sizeof(event_t),
		POOL_DYNGROW, "EventPool");

	return EOK;
}

/*
 * shutdown_events
 */
ecode_t shutdown_events()
{
	if (!s_EventPool) {
		trace(CHAN_INFO, "event system doesn't need shutting down");
		return EFAIL;
	}

	destroy_pool(s_EventPool);
	s_EventPool = NULL;

	return EOK;
}

/*
 * create_event
 */
event_t *create_event(const char *name, uint32_t flags)
{
        assert(name != NULL);

        if (!s_EventPool) {
                panic("event_t system not initialised");
        }

        if (!ValidFlags(flags)) {
                panic("Invalid flags");
        }

        event_t *evt = PAlloc(s_EventPool);
        evt->name = name;
        evt->flags = flags;

        return evt;
}

/*
 * queue_event
 */
ecode_t queue_event(event_t *event)
{
	assert(event != NULL);

	return EOK;
}

/*
 * process_events
 */
ecode_t process_events()
{
	// TODO: PFree() events after they've been processed.
	return EOK;
}
