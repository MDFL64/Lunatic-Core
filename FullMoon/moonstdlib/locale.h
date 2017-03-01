
typedef struct {
	const char* decimal_point;
} lconv;

lconv locale = {
	.decimal_point = "."
};

lconv* localeconv() {
	return &locale;
}

#define LC_ALL 1
#define LC_COLLATE 2
#define LC_CTYPE 3
#define LC_MONETARY 4
#define LC_NUMERIC 5
#define LC_TIME 6