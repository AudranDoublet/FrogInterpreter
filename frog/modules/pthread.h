#ifndef __FROGMODULES_PTHREAD__
#define __FROGMODULES_PTHREAD__
FrogObject *frogpthread_start(FrogObject **args, size_t count, stack *p);

FrogObject *frogpthread_detach(FrogObject **args,  size_t count, stack *p);

FrogObject *frogpthread_join(FrogObject **args, size_t count,  stack *p);

FrogObject *frogpthread_mutex(FrogObject **args, size_t count, stack *p);

FrogObject *frogpthread_lock(FrogObject **args,  size_t count, stack *p);

FrogObject *frogpthread_try_lock(FrogObject **args,  size_t count, stack *p);

FrogObject *frogpthread_unlock(FrogObject **args, size_t count, stack *p);
#endif
