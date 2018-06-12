#include "../frog.h"

FrogType iter_type;

void free_iter(FrogObject *obj)
{
	FrogIter *o = (FrogIter *) obj;

	if(o->values)
		free(o->values);
	free(o);
}

void iter_print(FrogObject *obj, FILE *file)
{
	UNUSED(obj);
	fprintf(file, "<iterator>");
}

FrogObject *iter_to_str(FrogObject *obj)
{
	UNUSED(obj);
	return utf8tostr("<iterator>");;
}

FrogObject *CreateIterable0(FrogObject *base)
{
	FrogIter *iter = malloc(sizeof(FrogIter));

	if(!iter)
	{
		FrogErr_Memory();
		return NULL;
	}

	iter->base = base;
	iter->iterable = NULL;
	iter->values = NULL;

	ObType(iter) = &iter_type;

	return (FrogObject *) iter;
}

FrogObject *CreateIterable(FrogObject *base)
{
	FrogIter *iter = malloc(sizeof(FrogIter));

	if(!iter)
	{
		FrogErr_Memory();
		return NULL;
	}

	if(IsInstance(base))
	{
		base = InstanceIterator(base);

		if(!base)
		{
			Frog_Free((FrogObject *) iter);
			return NULL;
		}
	}

	iter->base = base;
	iter->iterable = ObType(base)->as_iterable;
	iter->values = NULL;

	ObType(iter) = &iter_type;

	return (FrogObject *) iter;
}

int FrogIsIter(FrogObject *o)
{
	return ObType(o) == &iter_type;
}

FrogType iter_type = {
	{
		-1,		// FIXME type
		NULL		// refcnt
	}, 
	"iter",			// type name
	NULL,			// getter
	NULL,			// setter
	NULL,			// hash
	NULL,			// size
	iter_print,		// print
	iter_to_str,		// toiter
	NULL,			// toint
	NULL,			// exec
	NULL,			// compare
	NULL,			// compare
	NULL,			// as number
	NULL,			// as sequence
	NULL,
	NULL,			// call
	free_iter		// free
};
