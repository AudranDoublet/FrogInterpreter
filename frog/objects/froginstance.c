#include "../frog.h"

#define OFFSET		14695981039346656037U
#define FNV_PRIME 	1099511628211

#define HASH(x)		(hash = (hash ^ (x)) * FNV_PRIME)

FrogType instance_type;

void free_instance(FrogObject *obj)
{
	FrogInstance *o = (FrogInstance *) obj;

	free_hashmap(o->children);
	Unref(o->parent);

	free(o);
}

long instance_hash(FrogObject *obj)
{
	long hash = (long) OFFSET;
	HASH((long) obj);

	return hash;
}

int instance_scompare(FrogObject *a, FrogObject *b)
{
	return a == b;
}

FrogObject *instance_to_str(FrogObject *str)
{
	FrogInstance *o = (FrogInstance *) str;
	FrogObject *func = get_hashmap(o->children, utf8tostr("toString"));
	FrogObject *result = NULL;

	if(!func)
	{
		char *buffer = malloc(sizeof(char) * 1024);

		if(!buffer)
		{
			FrogErr_Memory();
			return NULL;
		}

		FrogString *par = (FrogString *) Frog_ToString(o->parent);
		if(!par) return NULL;

		sprintf(buffer, "<instance %p of %ls>", o, par->str);
		result = utf8tostr(buffer);

		free(buffer);

		if(!result) return NULL;
	}
	else
	{
		stack *st = create_stack();

		if(!st) return NULL;
		result = FrogCall_Call(func, NULL, 0, st);
		free(st);

		if(!result) return NULL;
		result = Frog_ToString(result);
	}

	return result;
}

void instance_print(FrogObject *str, FILE *file)
{
	Frog_Print(instance_to_str(str), file);
}

FrogObject *instance_getseq(FrogObject *str, FrogObject *b)
{
	FrogInstance *o = (FrogInstance *) str;
	FrogObject *func = get_hashmap(o->children, utf8tostr("__seq_get"));
	FrogObject *result = NULL;

	if(!func)
	{
		FrogErr_InstanceName("__seq_get");
		return NULL;
	}

	stack *st = create_stack();
	if(!st) return NULL;

	FrogObject **args = malloc(sizeof(FrogObject *));
	args[0] = b;

	result = FrogCall_Call(func, args, 1, st);
	free(st);

	return result;
}

FrogObject *instance_setseq(FrogObject *str, FrogObject *b, FrogObject *v, binaryfunction f)
{
	FrogInstance *o = (FrogInstance *) str;
	FrogObject *func = get_hashmap(o->children, utf8tostr("__seq_set"));
	FrogObject *result = NULL;

	if(!func)
	{
		FrogErr_InstanceName("__seq_set");
		return NULL;
	}

	if(f)
	{
		FrogObject *tmp = instance_getseq(str, b);

		if(!tmp) return NULL;
		v = f(tmp, v);

		if(!v) return NULL;
	}

	stack *st = create_stack();
	if(!st) return NULL;

	FrogObject **args = malloc(sizeof(FrogObject *) * 2);
	args[0] = b;
	args[1] = v;

	result = FrogCall_Call(func, args, 2, st);
	free(st);

	if(!result) return NULL;
	result = Frog_ToString(result);

	return result;
}

FrogObject *instance_size(FrogObject *str)
{
	FrogInstance *o = (FrogInstance *) str;
	FrogObject *func = get_hashmap(o->children, utf8tostr("__seq_len"));
	FrogObject *result = NULL;

	if(!func)
	{
		FrogErr_InstanceName("__seq_len");
		return NULL;
	}

	stack *st = create_stack();
	if(!st) return NULL;

	result = FrogCall_Call(func, NULL, 0, st);
	free(st);

	if(!result) return NULL;
	result = Frog_ToString(result);

	return result;
}

FrogObject *instance_get(FrogObject *str, FrogObject *value)
{
	FrogInstance *o = (FrogInstance *) str;
	FrogObject *func = get_hashmap(o->children, value);

	if(!func)
	{
		FrogErr_SubName(str, value);
		return NULL;
	}

	return func;
}

FrogObject *instance_set(FrogObject *str, FrogObject *key, FrogObject *value,
		binaryfunction operator)
{
	FrogInstance *o = (FrogInstance *) str;

	if(operator)
	{
		FrogObject *tmp = instance_get(str, key);
		if(!tmp) return NULL;

		value = operator(tmp, value);
		if(!value) return NULL;
	}

	put_hashmap(o->children, key, value);

	return value;
}

FrogObject *InstanceIterator(FrogObject *str)
{
	FrogInstance *o = (FrogInstance *) str;
	FrogObject *func = get_hashmap(o->children, utf8tostr("__iter"));

	if(!func)
	{
		FrogErr_InstanceName("__iter");
		return NULL;
	}

	return func;
}

int InstanceInit(FrogObject *ins, FrogObject **args, size_t count, stack *p)
{
	FrogInstance *o = (FrogInstance *) ins;
	FrogObject *func = get_hashmap(o->children, utf8tostr("__init"));

	if(!func)
	{
		if(count > 0)
		{
			FrogErr_BadArgCountS("__init", 1, count + 1);
			return 0;
		}

		return 1;
	}

	return FrogCall_Call(func, args, count, p) != NULL;
}

FrogObject *CreateInstance(FrogObject *parent, hashmap *children)
{
	FrogInstance *ins = malloc(sizeof(FrogInstance));

	if(!ins)
	{
		FrogErr_Memory();
		return NULL;
	}

	ins->parent = parent;
	ins->children = children;

	ObType(ins) = &instance_type;
	return (FrogObject *) ins;
}

int IsInstance(FrogObject *o)
{
	return ObType(o) == &instance_type;
}


FrogAsSequence ins_as_sequence =
{
	instance_setseq,
	instance_getseq
};

FrogType instance_type = {
	{
		-1,		// FIXME type
		NULL		// refcnt
	}, 
	"instance",		// type name
	instance_get,		// getter
	instance_set,		// setter
	instance_hash,		// hash
	instance_size,		// size
	instance_print,		// print
	instance_to_str,	// tostr
	NULL,			// toint
	NULL,			// exec
	instance_scompare,	// compare
	NULL,			// compare
	NULL,			// as number
	&ins_as_sequence,	// as sequence
	NULL,			// as iterable
	NULL,			// call
	free_instance		// free
};
