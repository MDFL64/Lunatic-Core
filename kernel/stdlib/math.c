#include <screen.h>

double floor(double x) {
	write_str_halt("floor\n");
	return -1;

	// implementation: [TEST ME!]
	long trun = x;
	if (x<0 && trun != x)
		return (long)(x - 1);
	return (long)(x);
}

double sinh(double x) {
	write_str_halt("sinh\n");
	return -1;
}

double cosh(double x) {
	write_str_halt("cosh\n");
	return -1;
}

double tanh(double x) {
	write_str_halt("tanh\n");
	return -1;
}

double ldexp(double x, int exp) {
	write_str_halt("ldexp\n");
	return -1;
}