#ifndef __FROGOBJECTS_SET_H__
#define __FROGOBJECTS_SET_H__

typedef struct
{
	FrogObjHead
	hashmap *map;
} FrogSet;

void init_set_functions(void);

FrogObject *CreateSet(FrogObject **keyValues, long len);

void set_clear(FrogObject *o);

FrogObject *set_union(FrogObject *a, FrogObject *b);

FrogObject *set_add(FrogObject *o, FrogObject *b);

FrogObject *set_remove(FrogObject *o, FrogObject *b);

FrogObject *set_contains(FrogObject *o, FrogObject *b);
#endif
