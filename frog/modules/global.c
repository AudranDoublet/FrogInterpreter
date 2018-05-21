#include "../frog.h"

FrogObject *global_print(FrogObject **obj, size_t count, stack *p)
{
	UNUSED(p);

	for(size_t i = 0; i < count; i++)
	{
		if(i > 0)
			printf(" ");

		Frog_Print(obj[i], stdout);
	}

	printf("\n");
	return FrogNone();
}

FrogObject *global_max(FrogObject **args, size_t count, stack *p)
{
	UNUSED(p);

	if(count == 0)
		return FrogNone();

	FrogObject *bigger = NULL;

	if(count == 1)
	{
		FrogObject *iter = FrogCall_IterInit(args[0]);
		if(!iter) return NULL;

		while(IsTrue(FrogCall_IterHasNext(iter)))
		{
			FrogObject *res = FrogCall_IterNext(iter);
			if(!res) return NULL;

			if(!bigger)
			{
				bigger = res;
				continue;
			}

			FrogObject *cmp = FrogCall_GE(bigger, res);
			if(!cmp) return NULL;

			if(IsFalse(cmp)) bigger = res;
		}

		return bigger;
	}

	bigger = args[0];

	for(size_t i = 1; i < count; i++)
	{
		FrogObject *res = FrogCall_GE(bigger, args[i]);

		if(!res)
			return NULL;
		if(IsFalse(res))
			bigger = args[i];
	}

	return bigger;
}

FrogObject *global_min(FrogObject **args, size_t count, stack *p)
{
	UNUSED(p);

	if(count == 0)
		return FrogNone();

	FrogObject *smaller = NULL;

	if(count == 1)
	{
		FrogObject *iter = FrogCall_IterInit(args[0]);
		if(!iter) return NULL;

		while(IsTrue(FrogCall_IterHasNext(iter)))
		{
			FrogObject *res = FrogCall_IterNext(iter);
			if(!res) return NULL;

			if(!smaller)
			{
				smaller = res;
				continue;
			}

			FrogObject *cmp = FrogCall_LE(smaller, res);
			if(!cmp) return NULL;

			if(IsFalse(cmp)) smaller = res;
		}

		return smaller;
	}

	smaller = args[0];

	for(size_t i = 1; i < count; i++)
	{
		FrogObject *res = FrogCall_LE(smaller, args[i]);

		if(!res)
			return NULL;
		if(IsFalse(res))
			smaller = args[i];

	}

	return smaller;
}

FrogObject *global_len(FrogObject **args, size_t count, stack *p)
{
	UNUSED(count);
	UNUSED(p);
	return Frog_Len(args[0]);
}

FrogObject *global_str(FrogObject **args, size_t count, stack *p)
{
	UNUSED(count);
	UNUSED(p);
	return Frog_ToString(args[0]);
}

FrogObject *global_id(FrogObject **args, size_t count, stack *p)
{
	UNUSED(count);
	UNUSED(p);
	return FromNativeInteger((long) args[0]);
}

FrogObject *global_include(FrogObject **args, size_t count, stack *p)
{
	UNUSED(count);
	FrogString *str = (FrogString *) Frog_ToString(args[0]);

	if(!str) return NULL;

	char *name = strtoutf8((FrogObject *) str);

	if(!name) return NULL;
	FILE *file = fopen(name, "r");

	if(!file)
	{
		FrogErr_Module((FrogObject *) str);
		return NULL;
	}

	FrogObject *obj = LoadModule(name, file);

	if(!obj) return NULL;
	if(!ModuleExec(obj, p)) return NULL;

	return obj;
}

FrogObject *global_native(FrogObject **args, size_t count, stack *p)
{
	UNUSED(p);
	UNUSED(count);

	FrogObject *str = Frog_ToString(args[0]);

	if(!str) return NULL;

	FrogObject *obj = get_native_module(str);

	if(!obj)
	{
		FrogErr_Module(str);
		return NULL;
	}

	return obj;
}

FrogObject *global_import(FrogObject **args, size_t count, stack *p)
{
	UNUSED(p);
	UNUSED(count);

	FrogObject *str = Frog_ToString(args[0]);
	char *name = strtoutf8((FrogObject *) str);

	char result[1024];

	sprintf(result, "fgmodules/%s.fg", name);

	if(!name) return NULL;
	FILE *file = fopen(result, "r");

	if(!file)
	{
		FrogErr_Module((FrogObject *) str);
		return NULL;
	}

	FrogObject *obj = LoadModule(name, file);

	if(!obj) return NULL;
	if(!ModuleExec(obj, p)) return NULL;

	return obj;
}

FrogObject *global_list(FrogObject **args, size_t count, stack *p)
{
	UNUSED(p);
	return CreateList(args, count);
}

FrogObject *global_tuple(FrogObject **args, size_t count, stack *p)
{
	UNUSED(p);
	return CreateTuple(args, count);
}

FrogObject *global_dict(FrogObject **args, size_t count, stack *p)
{
	UNUSED(p);
	return CreateMap(args, count);
}

FrogObject *range_next(FrogObject *o)
{
	FrogIter *i = (FrogIter *) o;

	long v = (long) i->values[0];
	long v2 = v + (long) i->values[1];

	i->values[0] = (void *) v2;
	return FromNativeInteger(v);
}

FrogObject *range_hasnext(FrogObject *o)
{
	FrogIter *i = (FrogIter *) o;

	long v1 = (long) i->values[0];
	long v2 = (long) i->values[1];
	long v3 = (long) i->values[2];

	int r = 0;

	if(v2 < 0 && v1 > v3) r  = 1;
	else if(v2 > 0 && v1 < v3) r = 1;

	return r ? FrogTrue() : FrogFalse();
}

FrogObject *range_init(FrogObject *o)
{
	UNUSED(o);
	return o;
}

FrogAsIterable range_iter = { range_init, range_next, range_hasnext };

FrogObject *global_range(FrogObject **args, size_t count, stack *p)
{
	UNUSED(p);
	if(count == 0 || count > 3)
	{
		FrogErr_BadArgCountS("range", 3, count);
		return NULL;
	}

	for(size_t i = 0; i < count; i++)
	{
		if(!FrogIsInt(args[i]))
		{
			FrogErr_TypeS("argument must be an integer");
			return NULL;
		}
	}

	long max, base, diff;

	max = (count == 1) ? FIValue(args[0]) : FIValue(args[1]);
	base = (count > 1) ? FIValue(args[0]) : 0;
	diff = (count == 3) ? FIValue(args[2]) : 1;

	FrogIter *iter = (FrogIter *) CreateIterable0(NULL);
	if(!iter) return NULL;

	iter->values = malloc(sizeof(void *) * 3);
	iter->iterable = &range_iter;

	if(!iter->values)
	{
		FrogErr_Memory();
		return NULL;
	}

	iter->values[0] = (void *) base;
	iter->values[1] = (void *) diff;
	iter->values[2] = (void *) max;

	return (FrogObject *) iter;
}
