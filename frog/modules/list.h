#ifndef __FROGMODULE_LIST_H
#define __FROGMODULE_LIST_H
FrogObject *list_append(FrogObject **args, size_t count, stack *p);

FrogObject *list_pop(FrogObject **args, size_t count, stack *p);

FrogObject *list_insert(FrogObject **args, size_t count, stack *p);

FrogObject *list_remove(FrogObject **args, size_t count, stack *p);
#endif
