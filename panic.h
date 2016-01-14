#ifndef __PANIC_H__
#define __PANIC_H__

#define Panic(m) do { _Panic(__FILE__, __LINE__, __func__, m); } while (0)
#define Trace(m) do { _Trace(__FILE__, __LINE__, __func__, m); } while (0)

void _Trace(const char *file, long line, const char *func, const char *message);
void _Panic(const char *file, long line, const char *func, const char *message);

#endif /* __PANIC_H__ */
