#include "../frog.h"

hashmap *native_modules;

static FrogObject *create_module(char *name)
{
	FrogObject *module = EmptyModule(name);
	put_hashmap(native_modules, utf8tostr(name), module);

	return module;
}

static void add_bifunc(char *name, builtinfunc fc, ssize_t param, FrogObject *module)
{
	FrogObject *nm = utf8tostr(name);

	if(!nm)
	{
		errx(-1, "fatal error: memory");
	}

	FrogObject *func = CreateBIFunction(nm, NULL, fc, param);	

	if(!func)
	{
		errx(-1, "fatal error: memory");
	}

	long *p = malloc(sizeof(p));
	if(!p) errx(-1, "fatal error: memory");

	FrogModule *mod = (FrogModule *) module;
	*p = mod->name2var->size;

	put_hashmap(mod->name2var, nm, p);
	mod->memory = realloc(mod->memory, sizeof(FrogObject *) * (*p + 1));
	mod->memory[*p] = func;
}

void init_module_pthread(void)
{
	FrogObject *p = create_module("pthread");

	add_bifunc("start", frogpthread_start, 2, p);
	add_bifunc("join", frogpthread_join, 1, p);
	add_bifunc("detach", frogpthread_detach, 1, p);
	add_bifunc("create_mutex", frogpthread_mutex, 1, p);
	add_bifunc("lock_mutex", frogpthread_lock, 1, p);
	add_bifunc("try_lock_mutex", frogpthread_try_lock, 1, p);
	add_bifunc("unlock_mutex", frogpthread_unlock, 1, p);
}

void init_native_modules(void)
{
	native_modules = create_hashmap();

	if(!native_modules) errx(-1, "fatal error: memory");
	init_module_pthread();
}

FrogObject *get_native_module(FrogObject *obj)
{
	return get_hashmap(native_modules, obj);
}
