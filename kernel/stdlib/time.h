#pragma once

#include <stddef.h>

#define CLOCKS_PER_SEC 1

typedef long time_t;
typedef long clock_t;

struct tm {
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	int tm_isdst;
};

clock_t clock();

time_t time(time_t *tloc);

struct tm* gmtime(const time_t *timer);

struct tm* localtime(const time_t *timer);

size_t strftime(char *s, size_t maxsize, const char *format, const struct tm *timptr);

time_t mktime(struct tm *timeptr);

double difftime(time_t time1, time_t time0);