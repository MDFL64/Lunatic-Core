typedef __int64 time_t;
typedef __int64 clock_t;

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

#define CLOCKS_PER_SEC 1000

time_t time(time_t* timer);