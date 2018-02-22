#ifndef __HASH_MAP_H__
#define __HASH_MAP_H__
struct entry_s
{
	FrogObject *key, *value;
	long hash;
	struct entry_s *next;
};

typedef struct
{
	size_t size;
	size_t capacity;
	struct entry_s **entries;	
} hashmap;

hashmap *create_hashmap();

int put_hashmap(hashmap *map, FrogObject *key, FrogObject *value);

FrogObject *get_hashmap(hashmap *map, FrogObject *key);

FrogObject *remove_hashmap(hashmap *map, FrogObject *key);

void clear_hashmap(hashmap *map);

void free_hashmap(hashmap *map);
#endif
