#include "../frog.h"

FrogType caller_type;

FrogObject *CreateCaller(int assign, void *func, FrogObject *a, FrogObject *b, FrogObject *c)
{
	FrogObjectCaller *caller = malloc(sizeof(FrogObjectCaller));

	if(!caller)
	{
		FrogErr_Memory();
		return NULL;
	}

	caller->assignation = assign;
	caller->func = func;
	caller->a = a;
	caller->b = b;
	caller->c = c;

	ObType(caller) = &caller_type;
	return (FrogObject *) caller;
}

void free_caller(FrogObject *caller)
{
	FrogObjectCaller *c = (FrogObjectCaller *) caller;

	Frog_Free(c->a);
	Frog_Free(c->b);
	Frog_Free(c->c);

	free(caller);
}

FrogObject *caller_exec(FrogObject *env, FrogObject *caller)
{
	FrogObjectCaller *ca = (FrogObjectCaller *) caller;

	FrogObject *a = NULL, *b = NULL, *c = NULL, *assignto;
	void *f = ca->func;

	if(ca->a && !(assignto = FrogCall_Exec(env, ca->a)))
		goto error;

	if(f) // otherwise, can be simple assignation (=), so we don't want to gethybrid
	{
		a = GetHybrid(assignto);

		if(!a) goto error;
		Ref(a);
	}

	Ref(assignto);

	if(ca->b && !(b = FrogCall_Exec(env, ca->b)))
	{
		Unref(a);
		goto error;
	}

	b = GetHybrid(b);
	Ref(b);

	if(ca->c && !(c = FrogCall_Exec(env, ca->c)))
	{
		Unref(a);
		Unref(b);
		goto error;
	}

	c = GetHybrid(c);
	Ref(c);

	FrogObject *res;

	if(f)
	{
		if(c == NULL)
		{
			if(b == NULL)
			{
				res = (*(unaryfunction)f)(a);
			} else 	res = (*(binaryfunction)f)(a, b);
		}
		else res = (*(ternaryfunction)f)(a, b, c);
	}
	else res = b;

	if(ca->assignation)
	{
		res = SetHybrid(assignto, res);
	}

	Unref(assignto);
	Unref(a);
	Unref(b);
	Unref(c);

	return res;
error:	return NULL;
}

FrogType caller_type = {
	{
		-1,		// FIXME type
		NULL		// refcnt
	}, 
	"caller",		// type name
	NULL,			// FIXME getter
	NULL,			// setter
	NULL,			// hash
	NULL,			// size
	NULL,			// print
	NULL,			// tostr
	NULL,			// toint
	caller_exec,		// exec
	NULL,			// compare
	NULL,			// compare 2
	NULL,			// as number
	NULL,			// as sequence
	NULL,			// call	
	free_caller		// free
};
