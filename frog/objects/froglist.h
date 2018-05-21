#ifndef __FROGOBJECTS_LIST_H__
#define __FROGOBJECTS_LIST_H__

typedef struct
{
	FrogObjHead

	FrogObject **array;

	size_t length;
	size_t capacity;	
} FrogList;

void init_lst_functions(void);

FrogObject *CreateList(FrogObject **array, long len);

int ListAppend(FrogObject *list, FrogObject *value);

int ListInsert(FrogObject *list, FrogObject *value, long pos);

FrogObject *ListPop(FrogObject *list);

FrogObject *ListRemove(FrogObject *list, long pos);
#endif
