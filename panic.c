#include "base.h"

#define LOGFILE_NAME "log.txt"
static FILE *s_Logfile = NULL;

/*
 * OpenLog
 *	Opens the log file. If it fails we don't care, really.
 */
static void OpenLog()
{
	if (s_Logfile != NULL)
		return;

	s_Logfile = fopen(LOGFILE_NAME, "w");
	if (!s_Logfile) {
		fprintf(stderr, "WARNING: Failed to open log file at %s\n",
				LOGFILE_NAME);
		return;
	}
}

/*
 * CloseLog
 *	Just wraps the fclose() call so we can do it atexit().
 */
static void CloseLog()
{
	if (s_Logfile != NULL) {
		fflush(s_Logfile);
		fclose(s_Logfile);
		s_Logfile = NULL;
	}
}

/*
 * Trace
 *	Write the given message to stdout and the log file.
 */
void _Trace(const char *file, long line, const char *func, const char *message)
{
	assert(message != NULL);

	if (!s_Logfile) {
		OpenLog();
		atexit(CloseLog);
	}

	printf("[T] (%s:%ld) %s: %s\n", file, line, func, message);
	fprintf(s_Logfile, "[T] %s: %s\n", func, message);
	//fflush(s_Logfile);
}

/*
 * Panic
 * 	Prints the given message to stderr and the log file, then exit()'s.
 *	We like simple error handling. Crash and burn.
 */
void _Panic(const char *file, long line, const char *func, const char *message)
{
	assert(message != NULL);

	if (!s_Logfile) {
		OpenLog();
		atexit(CloseLog);
	}

	fprintf(stderr, "\nPANIC:\n\tin %s (%s:%ld)\n\t%s\n\n",
			func, file, line, message);
	fprintf(s_Logfile, "\nPANIC:\n\tin %s (%s:%ld)\n\t%s\n\n",
			func, file, line, message);
	//fflush(s_Logfile);
	exit(EXIT_FAILURE);
}
