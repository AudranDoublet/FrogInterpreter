#ifndef __FROGOBJECTS_ITER_H__
#define __FROGOBJECTS_ITER_H__

typedef struct {
	FrogObjHead
	FrogObject *base;
	FrogAsIterable *iterable;
	void **values;
} FrogIter;

FrogObject *CreateIterable(FrogObject *iter);

FrogObject *CreateIterable0(FrogObject *base);

int FrogIsIter(FrogObject *o);
#endif
