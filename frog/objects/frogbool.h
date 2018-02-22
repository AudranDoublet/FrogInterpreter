#ifndef __FROGOBJECTS_BOOL_H__
#define __FROGOBJECTS_BOOL_H__
typedef struct {
	FrogObjHead
	long value;
} FrogBool;

FrogObject *FrogTrue(void);

FrogObject *FrogFalse(void);

int IsTrue(FrogObject *o);

int IsFalse(FrogObject *o);
#endif
