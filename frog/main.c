#include "frog.h"

int init_frog(void)
{
	init_builtin_functions();
	init_str_functions();
	init_lst_functions();
	init_map_functions();

	init_native_modules();

	return 1;
}

int main(void)
{
	if(!init_frog())
		errx(-1, "fatal error: initialization");

	parse_terminal();
	return 0;
}
