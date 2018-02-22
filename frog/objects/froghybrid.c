#include "../frog.h"

FrogType hybrid_type;

FrogObject *CreateHybrid(FrogObject *left, FrogObject *value, int type)
{
	FrogHybrid *h = malloc(sizeof(FrogHybrid));

	if(!h)
		return NULL;

	ObType(h) = &hybrid_type;

	Ref(left);
	Ref(value);

	h->left = left;
	h->value = value;
	h->type = type;

	return (FrogObject *) h;
}

/** Verify if an object is an FrogHybrid object */
int IsHybrid(FrogObject *o)
{
	return ObType(o) == &hybrid_type;
}

/** Return the pointed object (left[value] or left.value) if o is a FrogHybrid, otherwise returns o **/
FrogObject *GetHybrid(FrogObject *o)
{
	if(!o || !IsHybrid(o))
		return o;

	FrogHybrid *h = (FrogHybrid *) o;

	if(h->type == 1) // identifier
	{
		return FrogCall_Get(h->left, h->value);
	}
	else // sequence
	{
		return FrogCall_SeqGet(h->left, h->value);
	}
}

/** Set the pointed object (left[value] off left.value) and returns the value if o is a FrogHybrid, otherwise returns NULL **/
FrogObject *SetHybrid(FrogObject *hybrid, FrogObject *value)
{
	if(!hybrid || !IsHybrid(hybrid))
	{
		FrogErr_Value("Can't assign to literal");
		return NULL;
	}

	FrogHybrid *h = (FrogHybrid *) hybrid;

	if(h->type == 1) // identifier
	{
		return FrogCall_Set(h->left, h->value, value);
	}
	else // sequence
	{
		return FrogCall_SeqSet(h->left, h->value, value);
	}
}

FrogObject *hybrid_exec(FrogObject *env, FrogObject *o)
{
	FrogHybrid *h = (FrogHybrid *) o;
	FrogObject *left = h->left ? GetHybrid(FrogCall_Exec(env, h->left)) : env;

	if(!left)
		return NULL;

	Ref(left);
	FrogObject *value = GetHybrid(FrogCall_Exec(env, h->value));

	if(!value)
	{
		Unref(left);
		return NULL;
	}

	Ref(value);

	return CreateHybrid(left, value, h->type);
}

void free_hybrid(FrogObject *o)
{
	FrogHybrid *h = (FrogHybrid *) o;

	Unref(h->left);
	Unref(h->value);

	free(h);
}

FrogType hybrid_type = {
	{
		-1,		// FIXME type
		NULL		// refcnt
	}, 
	"hybrid",		// type name
	NULL,			// getter
	NULL,			// setter
	NULL,			// hash
	NULL,			// size
	NULL,			// print
	NULL,			// tostr
	NULL,			// toint
	hybrid_exec,		// exec
	NULL,			// compare
	NULL,			// compare 2
	NULL,			// as number
	NULL,			// as sequence
	NULL,			// call	
	free_hybrid		// free
};
