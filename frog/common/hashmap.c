#include "../frog.h"

#define DEFAULT_SIZE 32

struct entry_s *create_entry(struct entry_s *prev, long hash, FrogObject *key, void *value)
{
	struct entry_s *v = malloc(sizeof(struct entry_s));

	if(!v)
		return NULL;

	v->key = key;
	v->value = value;
	v->hash = hash;

	Ref(key);
	//Ref(value);

	v->next = prev ? prev->next : NULL;

	if(prev)
		prev->next = v;

	return v;
}

void replace(struct entry_s *v, FrogObject *val)
{
	Ref(val);
	Unref(v->value);
	v->value = val;
}

void free_entry(struct entry_s *e)
{
	if(!e) return;

	Unref(e->key);

	free_entry(e->next);
	free(e);
}

hashmap *create_hashmap(void)
{
	hashmap *map = malloc(sizeof(hashmap));

	if(!map) return NULL;

	map->size = 0;
	map->capacity = DEFAULT_SIZE;
	map->entries = calloc(map->capacity, sizeof(struct entry_s **));

	if(!map->entries)
	{
		free(map);
		return NULL;
	}

	return map;
}

static struct entry_s *copy_entry(struct entry_s *e)
{
	if(!e) return NULL;

	struct entry_s *res = create_entry(NULL, e->hash, e->key, e->value);
	if(!res) return NULL;

	res->next = copy_entry(e->next);

	return res;
}

hashmap *copy_hashmap(hashmap *map)
{
	hashmap *new = malloc(sizeof(hashmap));

	if(!new) return NULL;

	new->size = map->size;
	new->capacity = map->capacity;
	new->entries = calloc(new->capacity, sizeof(struct entry_s **));

	if(!new->entries)
	{
		free(new);
		return NULL;
	}

	for(size_t i = 0; i < new->capacity; i++)
	{
		new->entries[i] = copy_entry(map->entries[i]);
	}

	return new;
}

int put_hashmap(hashmap *map, FrogObject *key, void *value)
{
	long hash = Frog_Hash(key);
	long pos = hash % map->capacity;

	struct entry_s *val = map->entries[pos];

	if(!val)
	{
		map->size += 1;
		map->entries[pos] = create_entry(NULL, hash, key, value);
		return map->entries[pos] != NULL;
	}

	while(1)
	{
		if(val->hash == hash && ObType(key) == ObType(val->key)
			&& IsTrue(FrogCall_EQ(key, val->key))) // FIXME verify equals
		{
			replace(val, value);
			return 1;
		}
		else if(!val->next)
		{
			map->size += 1;
			return create_entry(val, hash, key, value) != NULL;
		}

		val = val->next;
	}
}

void *get_hashmap(hashmap *map, FrogObject *key)
{
	long hash = Frog_Hash(key);
	long pos = hash % map->capacity;

	struct entry_s *cur = map->entries[pos];

	for( ; cur ; cur = cur->next )
		if(cur->hash == hash && ObType(key) == ObType(cur->key)
				&& IsTrue(FrogCall_EQ(key, cur->key)))
			return cur->value;

	return NULL;
}

void *remove_hashmap(hashmap *map, FrogObject *key)
{
	long hash = Frog_Hash(key);
	long pos = hash % map->capacity;

	struct entry_s *cur = map->entries[pos];
	FrogObject *res = NULL;

	if(cur && cur->hash == hash)
	{
		map->entries[pos] = cur->next;
	}
	else
	{
		for( ; cur ; cur = cur->next )
			if(cur->hash == hash)
				break;
	}

	if(cur)
	{
		map->size -= 1;
		cur->next = NULL;
		res = cur->value;
		free_entry(cur);
	}

	return res;
}

void clear_hashmap(hashmap *map)
{
	for(size_t i = 0; i < map->capacity; i++)
	{
		free_entry(map->entries[i]);
		map->entries[i] = NULL;
	}

	map->size = 0;
}

void free_hashmap(hashmap *map)
{
	clear_hashmap(map);
	free(map->entries);
	free(map);
}
