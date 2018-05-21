#ifndef __FROGOBJECTS_FLOAT_H__
#define __FROGOBJECTS_FLOAT_H__

#define FFValue(v) (((FrogFloat *)v)->value)
typedef struct {
	FrogObjHead
	double value;
} FrogFloat;

FrogObject *FromNativeFloat(double v);

int FrogIsFloat(FrogObject *o);
#endif
