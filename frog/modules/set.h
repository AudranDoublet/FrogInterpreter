#ifndef __FROGMODULE_SET_H
#define __FROGMODULE_SET_H
FrogObject *setlib_add(FrogObject **args, size_t count, stack *p);

FrogObject *setlib_remove(FrogObject **args, size_t count, stack *p);

FrogObject *setlib_contains(FrogObject **args, size_t count, stack *p);

FrogObject *setlib_clear(FrogObject **args, size_t count, stack *p);
#endif
