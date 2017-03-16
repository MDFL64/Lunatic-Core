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

//

double pow(double x, double y) {
	write_str_halt("pow\n");
	return -1;
}

double log(double x) {
	write_str_halt("log\n");
	return -1;
}
double log2(double x) {
	write_str_halt("log2\n");
	return -1;
}
double log10(double x) {
	write_str_halt("log10\n");
	return -1;
}

double exp(double x) {
	write_str_halt("exp\n");
	return -1;
}

double sin(double x) {
	write_str_halt("sin\n");
	return -1;
}
double cos(double x) {
	write_str_halt("cos\n");
	return -1;
}	
double tan(double x) {
	write_str_halt("tan\n");
	return -1;
}

double asin(double x) {
	write_str_halt("asin\n");
	return -1;
}
double acos(double x) {
	write_str_halt("acos\n");
	return -1;
}
double atan(double x) {
	write_str_halt("atan\n");
	return -1;
}

double atan2(double x, double y) {
	write_str_halt("atan2\n");
	return -1;
}

double fmod(double n, double d) {
	write_str_halt("fmod\n");
	return -1;
}

double frexp(double arg, int* exp) {
	write_str_halt("frexp\n");
	return -1;
}

double modf(double x, double* iptr) {
	write_str_halt("modf\n");
	return -1;
}