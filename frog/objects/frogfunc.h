#ifndef __FROGOBJECTS_FUNC_H__
#define __FROGOBJECTS_FUNC_H__

typedef struct {
	FrogObjHead
	long *ins;
	size_t len, param_count, memlen;
	FrogObject *name, *file;
	long hash;
} FrogFunc;

typedef struct {
	FrogObjHead
	FrogObject *function;
	FrogObject *obj;
} FrogObjFunc;

typedef struct {
	FrogObjHead
	FrogObject *function;
} FrogIterFunc;

FrogObject *CreateFunction(FrogObject *name, FrogObject *file, size_t memlen,
				long *ins, size_t len, size_t param_count);

FrogObject *CreateIFunction(FrogObject *name, FrogObject *file, size_t memlen,
				long *ins, size_t len, size_t param_count);

FrogObject *CreateObjFunction(FrogObject *obj, FrogObject *function);
#endif
