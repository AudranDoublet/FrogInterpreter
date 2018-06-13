#ifndef __FROGMODULE_GLOBAL_H__
#define __FROGMODULE_GLOBAL_H__
FrogObject *global_print(FrogObject **args, size_t count, stack *p);

FrogObject *global_max(FrogObject **args, size_t count, stack *p);

FrogObject *global_min(FrogObject **args, size_t count, stack *p);

FrogObject *global_len(FrogObject **args, size_t count, stack *p);

FrogObject *global_str(FrogObject **args, size_t count, stack *p);

FrogObject *global_id(FrogObject **args, size_t count, stack *p);

FrogObject *global_include(FrogObject **args, size_t count, stack *p);

FrogObject *global_list(FrogObject **args, size_t count, stack *p);

FrogObject *global_dict(FrogObject **args, size_t count, stack *p);

FrogObject *global_tuple(FrogObject **args, size_t count, stack *p);

FrogObject *global_set(FrogObject **args, size_t count, stack *p);

FrogObject *global_native(FrogObject **args, size_t count, stack *p);

FrogObject *global_import(FrogObject **args, size_t count, stack *p);

FrogObject *global_range(FrogObject **args, size_t count, stack *p);
#endif
