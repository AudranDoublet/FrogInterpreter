#include "../frog.h"

#define OFFSET		14695981039346656037U
#define FNV_PRIME 	1099511628211

#define HASH(x)		(hash = (hash ^ (x)) * FNV_PRIME)

FrogType bifunc_type;

long bifunc_hash(FrogObject *o)
{
	FrogBIFunc *f = (FrogBIFunc *) o;

	if(f->hash != -1)
		return f->hash;

	long hash = (long) OFFSET;

	if(f->name)
		HASH( str_hash(f->name) );

	HASH((long) f->native_function);

	f->hash = hash;
	return hash;
}

void bifunc_print(FrogObject *o, FILE *f)
{
	FrogBIFunc *fu = (FrogBIFunc *) o;

	fprintf(f, "<built-in function ");

	if(fu->file)
	{
		Frog_Print(fu->file, f);
		fprintf(f, "/");
	}

	if(fu->name)
		Frog_Print(fu->name, f);
	else
		fprintf(f, "<anonymous>");


	if(fu->param_count != -1)
		fprintf(f, "(x%zu)>", (size_t) fu->param_count);
	else
		fprintf(f, "(...)>");
}

FrogObject *bifunc_tostr(FrogObject *o)
{
	UNUSED(o);
	return utf8tostr("False"); //FIXME
}

FrogObject *bifunc_call(FrogObject *o, FrogObject **args, size_t arg, stack *stack)
{
	UNUSED(stack);
	FrogBIFunc *func = (FrogBIFunc *) o;

	if(func->param_count != -1 && arg != (size_t) func->param_count)
	{
		FrogErr_BadArgCount(o, func->param_count, arg);
		return NULL;
	}

	return (func->native_function)(args, arg, stack);
}

void bifunc_free(FrogObject *o)
{
	FrogBIFunc *func = (FrogBIFunc *) o;

	Frog_Free(func->name);
	free(func);
}

FrogObject *CreateBIFunction(FrogObject *name, FrogObject *file,
				builtinfunc native, ssize_t param_count)
{
	FrogBIFunc *func = malloc(sizeof(FrogBIFunc));

	if(!func)
		return NULL;

	func->param_count = param_count;
	func->name = name;
	func->file = file;
	func->native_function = native;
	func->hash = -1;

	ObType(func) = &bifunc_type;

	return (FrogObject *) func;
}

FrogType bifunc_type =
{
	{
		-1,
		NULL//FIXME
	},
	"built-in function",
	NULL,
	NULL,
	bifunc_hash,
	NULL,
	bifunc_print,
	bifunc_tostr,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	bifunc_call,
	bifunc_free
};
