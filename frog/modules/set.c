#include "../frog.h"

FrogObject *setlib_add(FrogObject **args, size_t count, stack *p)
{
	UNUSED(count);
	UNUSED(p);

	return set_add(args[0], args[1]);
}

FrogObject *setlib_remove(FrogObject **args, size_t count, stack *p)
{
	UNUSED(count);
	UNUSED(p);

	return set_remove(args[0], args[1]);
}

FrogObject *setlib_contains(FrogObject **args, size_t count, stack *p)
{
	UNUSED(count);
	UNUSED(p);

	return set_contains(args[0], args[1]);
}

FrogObject *setlib_clear(FrogObject **args, size_t count, stack *p)
{
	UNUSED(count);
	UNUSED(p);

	set_clear(args[0]);
	return FrogNone();
}
