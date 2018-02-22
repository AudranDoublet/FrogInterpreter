#ifndef __FROG_OBJECTS_ENV_H__
#define __FROG_OBJECTS_ENV_H__

#define BREAK_NONE 0
#define BREAK_RETURN 1
#define BREAK_BREAK 2
#define BREAK_CONTINUE 3

/** Represent an environement */
typedef struct FrogEnv FrogEnv;

struct FrogEnv {
	FrogObjHead
	FrogEnv *parent;
	hashmap *map;

	// breaks: returns (1), break (2) and continue (3)
	int breaktype;
	FrogObject *breakvalue;
};

FrogObject *CreateEnvironment(FrogObject *parent);

/** Verify if an object is an FrogEnv object */
int IsEnv(FrogObject *o);
#endif
