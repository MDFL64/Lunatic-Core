#pragma once // DONE

double floor(double x);

double sinh(double x);
double cosh(double x);
double tanh(double x);

double ldexp(double x, int exp);

// New [THESE WERE -ACTUALLY- NEEDED FOR THE UPGRADE!]
double fabs(double x);

double pow(double x, double y);

double log(double x);
double log2(double x);
double log10(double x);

double exp(double x);

double sin(double x);
double cos(double x);
double tan(double x);

double asin(double x);
double acos(double x);
double atan(double x);

double atan2(double x, double y);

double fmod(double n, double d);

double frexp(double arg, int* exp);
double modf(double x, double* iptr);