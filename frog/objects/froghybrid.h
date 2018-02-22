#ifndef __FROG_OBJECTS_HYBRID_H__
#define __FROG_OBJECTS_HYBRID_H__

/** Represent an hybrid-used object, which is useful for getting and setting vars */
typedef struct {
	FrogObjHead
	FrogObject *left;
	FrogObject *value;
	/**
	*	1	=> identifier
	*	2	=> sequence
	*/
	int type;
} FrogHybrid;

FrogObject *CreateHybrid(FrogObject *left, FrogObject *right, int type);

/** Verify if an object is an FrogHybrid object */
int IsHybrid(FrogObject *o);

/** Return the pointed object (left[value] or left.value) if o is a FrogHybrid, otherwise returns o **/
FrogObject *GetHybrid(FrogObject *o);

/** Set the pointed object (left[value] off left.value) and returns the value if o is a FrogHybrid, otherwise returns NULL **/
FrogObject *SetHybrid(FrogObject *hybrid, FrogObject *value);
#endif
