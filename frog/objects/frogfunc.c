#include "../frog.h"

#define OFFSET		14695981039346656037U
#define FNV_PRIME 	1099511628211

#define HASH(x)		(hash = (hash ^ (x)) * FNV_PRIME)

FrogType frog_type;
FrogType funcobj_type;
FrogType funciter_type;
FrogAsIterable funciter_as_iter;

long func_hash(FrogObject *o)
{
	FrogFunc *f = (FrogFunc *) o;

	if(f->hash != -1)
		return f->hash;

	long hash = (long) OFFSET;

	if(f->name)
		HASH( str_hash(f->name) );

	for(size_t i = 0; i < f->len; i++)
		HASH(f->ins[i]);

	f->hash = hash;
	return hash;
}

long funcobj_hash(FrogObject *o)
{
	FrogObjFunc *of = (FrogObjFunc *) o;
	return Frog_Hash(of->function);
}

void func_print(FrogObject *o, FILE *f)
{
	FrogFunc *fu = (FrogFunc *) o;

	if(fu->file)
		Frog_Print(fu->file, f);

	fprintf(f, ":");

	if(fu->name)
		Frog_Print(fu->name, f);
	else
		fprintf(f, "<anonymous>");


	fprintf(f, "(arg * %zu)", fu->param_count);
}

void funcobj_print(FrogObject *o, FILE *f)
{
	FrogObjFunc *of = (FrogObjFunc *) o;
	Frog_Print(of->function, f);
}

FrogObject *func_tostr(FrogObject *o)
{
	UNUSED(o);
	return utf8tostr("False"); //FIXME
}

FrogObject *funcobj_tostr(FrogObject *o)
{
	FrogObjFunc *of = (FrogObjFunc *) o;
	return Frog_ToString(of->function);
}

FrogObject *func_call(FrogObject *o, FrogObject **args, size_t arg, stack *stack)
{
	FrogFunc *func = (FrogFunc *) o;
	FrogModule *file = (FrogModule *) func->file;

	if(arg != func->param_count)
	{
		FrogErr_BadArgCount(o, func->param_count, arg);
		return NULL;
	}

	FrogObject **memory = realloc(args, sizeof(FrogObject *) * func->memlen);

	if(!memory)
	{
		FrogErr_Memory();
		return NULL;
	}

//	for(size_t i = 0; i < arg; i++)
//		memory[i] = args[i];

	return execute(stack, file->memory, memory, func->ins, func->len, NULL);
}

FrogObject *funcobj_call(FrogObject *o, FrogObject **args, size_t arg, stack *stack)
{
	FrogObjFunc *of = (FrogObjFunc *) o;
	FrogObject **args2 = malloc(sizeof(FrogObject *) * (arg + 1));

	for(size_t i = 1; i <= arg; i++)
	{
		args2[i] = args[i - 1];
	}

	args2[0] = of->obj;
	free(args);

	return FrogCall_Call(of->function, args2, arg + 1, stack);
}

FrogObject *funciter_i_init(FrogObject *o)
{
	FrogIter *i = (FrogIter *) o;
	FrogFunc *func = (FrogFunc *) i->base;

	FrogObject **args = i->values[0];
	size_t arg = (size_t) i->values[1];

	i->values = malloc(sizeof(void *) * 4);
	if(!i->values)
	{
		FrogErr_Memory();
		return NULL;
	}

	i->values[0] = create_stack();
	i->values[1] = calloc(sizeof(FrogObject *), func->memlen);
	i->values[2] = malloc(sizeof(yieldmg));
	i->values[3] = NULL;

	if(!i->values[0] || !i->values[1] || !i->values[2])
	{
		free(i->values[0]);
		free(i->values[1]);
		free(i->values[2]);
		FrogErr_Memory();
		return NULL;
	}

	yieldmg *tmp = i->values[2];
	FrogObject **memory = i->values[1];

	for(size_t j = 0; j < arg; j++)
	{
		memory[j] = args[j];
	}

	tmp->end = 0;
	tmp->pos = 0;

	return o;
}

FrogObject *funciter_i_next(FrogObject *o)
{
	FrogIter *i = (FrogIter *) o;
	return i->values[3];
}

FrogObject *funciter_i_hasnext(FrogObject *o)
{
	FrogIter *i = (FrogIter *) o;
	yieldmg *mg = i->values[2];
	FrogFunc *func = (FrogFunc *) i->base;
	FrogModule *file = (FrogModule *) func->file;

	if(mg->end) return FrogFalse();
	FrogObject *r = 
		execute(i->values[0], file->memory, i->values[1], func->ins, func->len, mg);

	i->values[3] = r;
	return mg->end ? FrogFalse() : FrogTrue();
}

FrogObject *funciter_call(FrogObject *o, FrogObject **args, size_t arg, stack *stack)
{
	UNUSED(stack);

	FrogIterFunc *iter = (FrogIterFunc *) o;
	FrogFunc *func = (FrogFunc *) iter->function;

	if(arg != func->param_count)
	{
		FrogErr_BadArgCount(o, func->param_count, arg);
		return NULL;
	}

	FrogIter *i = (FrogIter *) CreateIterable0((FrogObject *) func);
	if(!i) return NULL;

	i->iterable = &funciter_as_iter;
	i->values = malloc(sizeof(void *) * 2);

	if(!i->values)
	{
		free(i);
		FrogErr_Memory();
		return NULL;
	}

	i->values[0] = args;
	i->values[1] = (void *) arg;

	return (FrogObject *) i;
}

void func_free(FrogObject *o)
{
	FrogFunc *func = (FrogFunc *) o;

	free(func->ins);
	Frog_Free(func->name);
	free(func);
}

void funcobj_free(FrogObject *o)
{
	free(o);
}

FrogObject *CreateFunction(FrogObject *name, FrogObject *file, size_t memlen,
				long *ins, size_t len, size_t param_count)
{
	FrogFunc *func = malloc(sizeof(FrogFunc));

	if(!func)
	{
		FrogErr_Memory();
		return NULL;
	}

	func->ins =  ins;
	func->len = len;
	func->param_count = param_count;
	func->name = name;
	func->file = file;
	func->memlen = memlen;
	func->hash = -1;

	ObType(func) = &frog_type;

	return (FrogObject *) func;
}

FrogObject *CreateIFunction(FrogObject *name, FrogObject *file, size_t memlen,
				long *ins, size_t len, size_t param_count)
{
	FrogObject *f = CreateFunction(name, file, memlen, ins, len, param_count);
	if(!f) return NULL;

	FrogIterFunc *func = malloc(sizeof(FrogIterFunc));

	if(!func)
	{
		Frog_Free(f);
		FrogErr_Memory();
		return NULL;
	}

	ObType(func) = &funciter_type;
	func->function = f;
	return (FrogObject *) func;
}

FrogObject *CreateObjFunction(FrogObject *obj, FrogObject *function)
{
	FrogObjFunc *func =  malloc(sizeof(FrogObjFunc));

	if(!func)
	{
		FrogErr_Memory();
		return NULL;
	}

	func->obj = obj;
	func->function = function;

	ObType(func) = &funcobj_type;

	return (FrogObject *) func;
}



FrogType frog_type =
{
	{
		-1,
		NULL//FIXME
	},
	"function",
	NULL,
	NULL,
	func_hash,
	NULL,
	func_print,
	func_tostr,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	func_call,
	func_free
};

FrogType funcobj_type =
{
	{
		-1,
		NULL//FIXME
	},
	"function",
	NULL,
	NULL,
	funcobj_hash,
	NULL,
	funcobj_print,
	funcobj_tostr,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	funcobj_call,
	funcobj_free
};

FrogAsIterable funciter_as_iter =
{
	funciter_i_init,
	funciter_i_next,
	funciter_i_hasnext
};

FrogType funciter_type =
{
	{
		-1,
		NULL//FIXME
	},
	"function",
	NULL,
	NULL,
	funcobj_hash,
	NULL,
	funcobj_print,
	funcobj_tostr,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	funciter_call,
	funcobj_free
};
