#ifndef __FROGOBJECTS_INSTANCE_H__
#define __FROGOBJECTS_INSTANCE_H__

typedef struct {
	FrogObjHead

	FrogObject *parent;
	hashmap *children;
} FrogInstance;

FrogObject *CreateInstance(FrogObject *parent, hashmap *children);

int IsInstance(FrogObject *o);

int InstanceInit(FrogObject *ins, FrogObject **args, size_t count, stack *p);

FrogObject *InstanceIterator(FrogObject *ins);
#endif
