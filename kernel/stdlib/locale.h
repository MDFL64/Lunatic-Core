#pragma once

#define LC_ALL 1
#define LC_CTYPE 2
#define LC_NUMERIC 3
#define LC_TIME 4
#define LC_COLLATE 5
#define LC_MONETARY 6

char* setlocale(int category, const char* locale);