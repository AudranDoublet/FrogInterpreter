#include "frog.h"

int init_frog(void)
{
	init_builtin_functions();
	init_str_functions();
	init_lst_functions();
	init_map_functions();
	init_set_functions();

	init_native_modules();

	return 1;
}

int main(int argc, char **argv)
{
	if(!init_frog())
		errx(-1, "fatal error: initialization");

	if(argc >= 2)
	{
		char *name = argv[1];
		FILE *file = fopen(name, "r");

		if(!file)
		{
			printf("Unknow file '%s'\n", name);
			return 1;
		}

		FrogObject *obj = LoadModule(name, file);

		if(!obj) return 1;
		if(!ModuleExec(obj, create_stack()))
		{
			FrogErr_DebugPrint();
		}
	}
	else
	{
		parse_terminal();
	}

	return 0;
}
