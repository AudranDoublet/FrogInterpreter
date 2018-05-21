#include "../frog.h"

FrogObject *dict_put(FrogObject **args, size_t count, stack *p)
{
	UNUSED(count);
	UNUSED(p);

	return map_set_at(args[0], args[1], args[2], NULL);
}

FrogObject *dict_get(FrogObject **args, size_t count, stack *p)
{
	UNUSED(count);
	UNUSED(p);

	return map_get_at(args[0], args[1]);
}

FrogObject *dict_remove(FrogObject **args, size_t count, stack *p)
{
	UNUSED(count);
	UNUSED(p);

	return map_remove(args[0], args[1]);
}

FrogObject *dict_contains(FrogObject **args, size_t count, stack *p)
{
	UNUSED(count);
	UNUSED(p);

	return map_contains(args[0], args[1]);
}

FrogObject *dict_clear(FrogObject **args, size_t count, stack *p)
{
	UNUSED(count);
	UNUSED(p);

	map_clear(args[0]);
	return FrogNone();
}
