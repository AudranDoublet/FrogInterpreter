#ifndef __FROGOBJECTS_BIFUNC_H__
#define __FROGOBJECTS_BIFUNC_H__

typedef FrogObject *(*builtinfunc)(FrogObject **, size_t count, stack *s);

typedef struct {
	FrogObjHead

	builtinfunc native_function;
	ssize_t param_count;
	FrogObject *name, *file;
	long hash;
} FrogBIFunc;

FrogObject *CreateBIFunction(FrogObject *name, FrogObject *file,
			builtinfunc native, ssize_t param_count);
#endif
