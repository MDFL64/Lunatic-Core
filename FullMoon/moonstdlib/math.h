#pragma once

#define HUGE_VAL ((double)0x7ff0000000000000)

double abs(double x);

double pow(double x, double y);

double fmod(double x, double y);

double frexp(double x, int* exp);

double floor(double x);