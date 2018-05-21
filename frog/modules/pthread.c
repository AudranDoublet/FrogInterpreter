#include <semaphore.h>
#include <errno.h>
#include "../frog.h"

static void *thread_function(void *arg)
{
	FrogObject **args = arg;
	stack *s = create_stack();

	FrogObject *obj = FrogCall_Call(args[0], NULL, 0, s);
	
	if(!obj)
	{
		FrogErr_DebugPrint();
		obj = FrogNone();
	}

	FrogObject *ret = FrogCall_Call(args[1], NULL, 0, s);

	if(!ret)
	{
		FrogErr_DebugPrint();
	}

	return obj;
}

FrogObject *frogpthread_start(FrogObject **args, size_t count, stack *p)
{
	UNUSED(count);
	UNUSED(p);

	pthread_t *thread = malloc(sizeof(pthread_t));

	if(!thread)
	{
		FrogErr_Memory();
		return NULL;
	}

	if(pthread_create(thread, NULL, thread_function, args) != 0)
	{
		FrogErr_Post("ThreadError", "Can't create thread.");
	}

	return FromNativeInteger((long) thread);
}

FrogObject *frogpthread_detach(FrogObject **args,  size_t count, stack *p)
{
	UNUSED(count);
	UNUSED(p);

	if(!FrogIsInt(args[0]))
	{
		FrogErr_TypeS("argument must be an integer");
		return NULL;
	}

	pthread_t *thread = (pthread_t *) FIValue(args[0]);

	if(pthread_detach(*thread) != 0)
	{
		FrogErr_Post("ThreadError", "Can't detach thread.");
	}

	return FrogNone();
}

FrogObject *frogpthread_join(FrogObject **args, size_t count, stack *p)
{
	UNUSED(count);
	UNUSED(p);

	if(!FrogIsInt(args[0]))
	{
		FrogErr_TypeS("argument must be an integer");
		return NULL;
	}

	pthread_t *thread = (pthread_t *) FIValue(args[0]);
	void *ret = NULL;

	if(pthread_join(*thread, &ret) != 0)
	{
		FrogErr_Post("ThreadError", "Can't join thread");
	}

	return ret;
}

FrogObject *frogpthread_mutex(FrogObject **args,  size_t count, stack *p)
{
	UNUSED(count);
	UNUSED(p);

	if(!FrogIsInt(args[0]))
	{
		FrogErr_TypeS("argument must be an integer");
		return NULL;
	}

	sem_t *s = malloc(sizeof(sem_t));

	if(!s)
	{
		FrogErr_Memory();
		return NULL;
	}

	if(sem_init(s, 0, FIValue(args[0])) != 0)
	{
		FrogErr_Post("ThreadError", "Can't lock mutex");
	}

	return FromNativeInteger((long) s);
}

FrogObject *frogpthread_lock(FrogObject **args,  size_t count, stack *p)
{
	UNUSED(count);
	UNUSED(p);

	if(!FrogIsInt(args[0]))
	{
		FrogErr_TypeS("argument must be an integer");
		return NULL;
	}

	sem_t *s = (sem_t *) FIValue(args[0]);

	if(sem_wait(s) != 0)
	{
		FrogErr_Post("ThreadError", "Can't lock mutex");
	}

	return FrogNone();
}

FrogObject *frogpthread_try_lock(FrogObject **args,  size_t count, stack *p)
{
	UNUSED(count);
	UNUSED(p);

	if(!FrogIsInt(args[0]))
	{
		FrogErr_TypeS("argument must be an integer");
		return NULL;
	}

	sem_t *s = (sem_t *) FIValue(args[0]);

	int ret = sem_trywait(s);

	if(ret != 0 && ret != EAGAIN)
	{
		FrogErr_Post("ThreadError", "Can't lock mutex");
	}

	return ret == 0 ? FrogTrue() : FrogFalse();
}

FrogObject *frogpthread_unlock(FrogObject **args, size_t count, stack *p)
{
	UNUSED(count);
	UNUSED(p);

	if(!FrogIsInt(args[0]))
	{
		FrogErr_TypeS("argument must be an integer");
		return NULL;
	}

	sem_t *s = (sem_t *) FIValue(args[0]);

	if(sem_post(s) != 0)
	{
		FrogErr_Post("ThreadError", "Can't unlock mutex");
	}

	return FrogNone();

}
