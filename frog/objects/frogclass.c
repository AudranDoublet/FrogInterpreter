#include <string.h>
#include "../frog.h"

#define OFFSET		14695981039346656037U
#define FNV_PRIME 	1099511628211

#define HASH(x)		(hash = (hash ^ (x)) * FNV_PRIME)

FrogType class_type;

long class_hash(FrogObject *o)
{
	FrogClass *f = (FrogClass *) o;

	if(f->hash != -1)
		return f->hash;

	long hash = (long) OFFSET;

	if(f->name)
		HASH( str_hash(f->name) );

	HASH(Frog_Hash(f->module));

	//FIXME use functions

	f->hash = hash;
	return hash;
}

void class_print(FrogObject *o, FILE *f)
{
	FrogClass *fc = (FrogClass *) o;
	fprintf(f, "<class ");
	Frog_Print(fc->module, f);
	fprintf(f, ":");

	if(fc->name)
		Frog_Print(fc->name, f);
	else fprintf(f, "<anonymous>");

	fprintf(f, ">");
}

FrogObject *class_tostr(FrogObject *o)
{
	UNUSED(o);
	return utf8tostr("<class>"); //FIXME
}

void class_free(FrogObject *o)
{
	FrogClass *func = (FrogClass *) o;

	Frog_Free(func->name);
	free_hashmap(func->functions);

	Unref(func->parent);

	free(func);
}

int build_methods(FrogClass *class, FrogObject *o, hashmap *map)
{
	FrogClass *parent = NULL;

	if(class->parent)
	{
		parent = (FrogClass *) module_getter(class->module, class->parent);

		if(parent && !IsClass((FrogObject *) parent))
		{
			FrogErr_ClassName(class->parent);
			return 0;
		}
	}

	if(parent && !build_methods(parent, o, map))
	{
		return 0;
	}

	hashmap *m = class->functions;

	for(size_t i = 0; i < m->capacity; i++)
	{
		struct entry_s *e = m->entries[i];

		while(e)
		{
			FrogObject *res = CreateObjFunction(o, e->value);
			if(!res) return 0;

			put_hashmap(map, e->key, res);
			e = e->next;
		}
	}

	return 1;
}

FrogObject *class_call(FrogObject *o, FrogObject **args, size_t count, stack *p)
{
	FrogClass *class = (FrogClass *) o;
	hashmap *methods = create_hashmap();

	if(!methods) return NULL;
	FrogObject *ins = CreateInstance(o, methods);

	if(!ins)
	{
		free_hashmap(methods);
		FrogErr_Memory();

		return NULL;
	}

	if(!build_methods(class, ins, methods)
		|| !InstanceInit(ins, args, count, p))
	{
		free_hashmap(methods);
		free(ins);

		return NULL;
	}

	return ins;
}

FrogObject *CreateClass(FrogObject *module, FrogObject *name,
		FrogObject *parent, hashmap *functions)
{
	FrogClass *class = malloc(sizeof(FrogClass));

	class->module = module;
	class->name = name;
	class->parent = parent;
	class->functions = functions;

	ObType(class) = &class_type;

	return (FrogObject *) class;
}

int IsClass(FrogObject *o)
{
	return ObType(o) == &class_type;
}

FrogType class_type =
{
	{
		-1,
		NULL//FIXME
	},
	"class",
	NULL,
	NULL,
	class_hash,
	NULL,
	class_print,
	class_tostr,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	class_call,
	class_free
};
