#ifndef __HASH_MAP_H__
#define __HASH_MAP_H__
struct entry_s
{
	FrogObject *key;
	void *value;
	long hash;
	struct entry_s *next;
};

typedef struct
{
	size_t size;
	size_t capacity;
	struct entry_s **entries;	
} hashmap;

hashmap *create_hashmap(void);

hashmap *copy_hashmap(hashmap *map);

int put_hashmap(hashmap *map, FrogObject *key, void *value);

void *get_hashmap(hashmap *map, FrogObject *key);

void *remove_hashmap(hashmap *map, FrogObject *key);

void clear_hashmap(hashmap *map);

void free_hashmap(hashmap *map);
#endif
