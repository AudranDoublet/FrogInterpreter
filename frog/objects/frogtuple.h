#ifndef __FROGOBJECTS_TUPLE_H__
#define __FROGOBJECTS_TUPLE_H__

typedef struct
{
	FrogObjHead

	FrogObject **array;
	size_t length;
} FrogTuple;

FrogObject *CreateTuple(FrogObject **array, long len);

int FrogIsTuple(FrogObject *o);
#endif
