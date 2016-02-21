#ifndef __PANIC_H__
#define __PANIC_H__

#define panic(m) do { _panic(__FILE__, __LINE__, __func__, m); } while (0)
#define trace(chan,m) do { _trace(__FILE__, __LINE__, __func__, m, chan); } while (0)

/* trace channels. trace messages will only be printed if their channel is
 * currently active.
 */
#define CHAN_INFO	0x1
#define CHAN_GAME	0x2
#define CHAN_REND	0x4
#define CHAN_MEM	0x8
#define CHAN_DBG	0x10
#define CHAN_SCRIPT	0x20

#define CHAN_ALL	(CHAN_INFO | CHAN_GAME | CHAN_REND | CHAN_MEM | \
			CHAN_DBG | CHAN_SCRIPT)

void set_trace_channels(int mask);

void _trace(const char *file, long line, const char *func, const char *message,
	int channel);
void _panic(const char *file, long line, const char *func, const char *message)
	__attribute__((noreturn));

#endif /* __PANIC_H__ */
