#ifndef __FROGOBJECTS_CLASS_H__
#define __FROGOBJECTS_CLASS_H__

typedef struct {
	FrogObjHead

	FrogObject *module;
	FrogObject *name;
	FrogObject *parent;
	hashmap *functions;

	long hash;
} FrogClass;

FrogObject *CreateClass(FrogObject *module, FrogObject *name,
		FrogObject *parent, hashmap *functions);

int IsClass(FrogObject *o);
#endif
