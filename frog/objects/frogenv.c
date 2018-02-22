#include "../frog.h"

FrogType env_type;

FrogObject *CreateEnvironment(FrogObject *parent)
{
	hashmap *map = create_hashmap();

	if(!map)
	{
		FrogErr_Memory();
		return NULL;
	}

	FrogEnv *env = malloc(sizeof(FrogEnv));

	if(!env)
	{
		FrogErr_Memory();
		free(map);
		return NULL;
	}

	ObType(env) = &env_type;
	env->parent = (FrogEnv *) parent;
	env->map = map;
	env->breaktype = BREAK_NONE;

	return (FrogObject *) env;
}

/** Verify if an object is an FrogEnv object */
int IsEnv(FrogObject *o)
{
	return o && ObType(o) == &env_type;
}

void free_env(FrogObject *o)
{
	FrogEnv *env = (FrogEnv *) o;
	free_hashmap(env->map);
}

FrogObject *env_get(FrogObject *o, FrogObject *key)
{
	FrogEnv *env = (FrogEnv *) o;
	FrogObject *res;

	while(env)
	{
		res = get_hashmap(env->map, key);

		if(res)
			return res;

		env = env->parent;
	}

	FrogErr_Name(key);
	return NULL;
}

FrogObject *env_set(FrogObject *o, FrogObject *key, FrogObject *value)
{
	FrogEnv *env = (FrogEnv *) o;

	if(!put_hashmap(env->map, key, value))
	{
		FrogErr_Memory();
		return NULL;
	}

	return value;
}

FrogType env_type = {
	{
		-1,		// FIXME type
		NULL		// refcnt
	}, 
	"env",			// type name
	env_get,		// getter
	env_set,		// setter
	NULL,			// hash
	NULL,			// size
	NULL,			// print
	NULL,			// tostr
	NULL,			// toint
	NULL,			// exec
	NULL,			// compare
	NULL,			// compare 2
	NULL,			// as number
	NULL,			// as sequence
	NULL,			// call	
	free_env		// free
};
