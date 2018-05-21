#include "../frog.h"

FrogObject *list_append(FrogObject **args, size_t count, stack *p)
{
	UNUSED(count);
	UNUSED(p);

	if(ListAppend(args[0], args[1]))
		return FrogNone();
	return NULL;
}

FrogObject *list_pop(FrogObject **args, size_t count, stack *p)
{
	UNUSED(count);
	UNUSED(p);

	return ListPop(args[0]);
}

FrogObject *list_insert(FrogObject **args, size_t count, stack *p)
{
	UNUSED(count);
	UNUSED(p);

	if(!FrogIsInt(args[1]))
	{
		FrogErr_TypeS("list indices must be integers");
		return NULL;
	}

	if(ListInsert(args[0], args[2], FIValue(args[1])))
		return FrogNone();
	return NULL;
}

FrogObject *list_remove(FrogObject **args, size_t count, stack *p)
{
	UNUSED(count);
	UNUSED(p);

	if(!FrogIsInt(args[1]))
	{
		FrogErr_TypeS("list indices must be integers");
		return NULL;
	}

	return ListRemove(args[0], FIValue(args[1]));
}
