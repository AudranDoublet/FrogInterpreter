#ifndef __FROGOBJECTS_DICT_H__
#define __FROGOBJECTS_DICT_H__

typedef struct
{
	FrogObjHead
	hashmap *map;	
} FrogDict;

void init_map_functions(void);

FrogObject *CreateMap(FrogObject **keyValues, long len);

FrogObject *map_get_at(FrogObject *o, FrogObject *b);

FrogObject *map_set_at(FrogObject *o, FrogObject *b, FrogObject *c, binaryfunction f);

void map_clear(FrogObject *o);

FrogObject *map_remove(FrogObject *o, FrogObject *b);

FrogObject *map_contains(FrogObject *o, FrogObject *b);
#endif
