#ifndef __PANIC_H__
#define __PANIC_H__

#define Panic(m) do { _Panic(__FILE__, __LINE__, __func__, m); } while (0)
#define Trace(chan,m) do { _Trace(__FILE__, __LINE__, __func__, m, chan); } while (0)

/* Trace channels. Trace messages will only be printed if their channel is
 * currently active.
 */
#define CHAN_GENERAL	0x1
#define CHAN_GAME	0x2
#define CHAN_REND	0x4
#define CHAN_MEM	0x8
#define CHAN_DBG	0x10

#define CHAN_ALL	(CHAN_GENERAL | CHAN_GAME | CHAN_REND | CHAN_MEM | \
			CHAN_DBG)

void SetTraceChannels(int mask);

void _Trace(const char *file, long line, const char *func, const char *message,
	int channel);
void _Panic(const char *file, long line, const char *func, const char *message)
	__attribute__((noreturn));

#endif /* __PANIC_H__ */
