#include <time.h>

clock_t clock() {
	write_str_halt("clock\n");
	return 0;
}

time_t time(time_t *tloc) {
	write_str_halt("time\n");
	return 0;
}

struct tm* gmtime(const time_t *timer) {
	write_str_halt("gmtime\n");
	return 0;
}

struct tm* localtime(const time_t *timer) {
	write_str_halt("localtime\n");
	return 0;
}

size_t strftime(char *s, size_t maxsize, const char *format, const struct tm *timptr) {
	write_str_halt("strftime\n");
	return 0;
}

time_t mktime(struct tm *timeptr) {
	write_str_halt("mktime\n");
	return 0;
}

double difftime(time_t time1, time_t time0) {
	write_str_halt("difftime\n");
	return 0;
}