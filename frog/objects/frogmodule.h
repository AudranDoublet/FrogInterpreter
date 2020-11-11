#ifndef __FROGOBJECTS_MODULE_H__
#define __FROGOBJECTS_MODULE_H__

typedef struct {
	FrogObjHead
	hashmap *name2var;
	FrogObject **memory;

	// 'main' function
	long *ins;
	size_t len;

	FrogObject *name;
	long hash;
} FrogModule;

extern FrogObject *function_create_list, *function_create_map, *function_create_tuple;

FrogObject *EmptyModule(char *name);

int ModuleExec(FrogObject *module, stack *st);

FrogObject *LoadModule(char *name, FILE *input);

void init_builtin_functions(void);

FrogObject *module_getter(FrogObject *o, FrogObject *var);
#endif
