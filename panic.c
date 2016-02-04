#include "base.h"
#include "panic.h"
#include <time.h>

#define LOGFILE_NAME "log.txt"
static FILE *s_Logfile = NULL;

/* Defaults to 1 so CHAN_INFO is already active. */
static uint64_t s_Channels = 1;

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
 * SetTraceChannels
 */
void SetTraceChannels(int mask)
{
	s_Channels = mask;
}

/*
 * ChannelToStr
 */
static const char *ChannelToStr(int chan)
{
	switch (chan) {
	case CHAN_INFO:
		return "INFO";
	case CHAN_MEM:
		return "MEM";
	case CHAN_DBG:
		return "DEBUG";
	case CHAN_GAME:
		return "GAME";
	case CHAN_REND:
		return "REND";
	case CHAN_SCRIPT:
		return "SCRIPT";
	}

	return "<INVALID CHANNEL>";
}

/*
 * Trace
 *	Write the given message to stdout and the log file.
 */
void _Trace(
	const char *file,
	long line,
	const char *func,
	const char *message,
	int channel)
{
	assert(message != NULL);

	if (!(s_Channels & channel)) {
		return;
	}

	if (!s_Logfile) {
		OpenLog();
		atexit(CloseLog);
	}

	// struct tm {
        //        int tm_sec;    /* Seconds (0-60) */
        //        int tm_min;    /* Minutes (0-59) */
        //        int tm_hour;   /* Hours (0-23) */
        //        int tm_mday;   /* Day of the month (1-31) */
        //        int tm_mon;    /* Month (0-11) */
        //        int tm_year;   /* Year - 1900 */
        //        int tm_wday;   /* Day of the week (0-6, Sunday = 0) */
        //        int tm_yday;   /* Day in the year (0-365, 1 Jan = 0) */
        //        int tm_isdst;  /* Daylight saving time */
	// };
	time_t t = time(NULL);
	struct tm *curTime = localtime(&t);
	// TODO: Don't output file and line in release, maybe func too
	const char *fmt = "[%d:%d:%d %s] (%s:%ld) %s: %s\n";

	printf(fmt, curTime->tm_hour, curTime->tm_min, curTime->tm_sec,
		ChannelToStr(channel), file, line, func, message);

	fprintf(s_Logfile, fmt, curTime->tm_hour, curTime->tm_min,
		curTime->tm_sec, ChannelToStr(channel), file, line,
		func, message);
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
