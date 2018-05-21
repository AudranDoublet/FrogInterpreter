#ifndef __FROGMODULE_DICT_H
#define __FROGMODULE_DICT_H
FrogObject *dict_put(FrogObject **args, size_t count, stack *p);

FrogObject *dict_get(FrogObject **args, size_t count, stack *p);

FrogObject *dict_remove(FrogObject **args, size_t count, stack *p);

FrogObject *dict_contains(FrogObject **args, size_t count, stack *p);

FrogObject *dict_clear(FrogObject **args, size_t count, stack *p);
#endif
