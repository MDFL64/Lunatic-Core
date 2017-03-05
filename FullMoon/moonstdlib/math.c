#include "../src/screen.h"

double floor(double x) {
	write_str("floor\n");
	__halt();
	return -1;

	// implementation: [TEST ME!]
	__int64 trun = x;
	if (x<0 && trun != x)
		return (__int64)(x - 1);
	return (__int64)(x);
}

double sinh(double x) {
	write_str("sinh\n");
	__halt();
	return -1;
}

double cosh(double x) {
	write_str("cosh\n");
	__halt();
	return -1;
}

double tanh(double x) {
	write_str("tanh\n");
	__halt();
	return -1;
}

double ldexp(double x, int exp) {
	write_str("ldexp\n");
	__halt();
	return -1;
}