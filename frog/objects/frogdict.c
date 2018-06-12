#include "../frog.h"

#define MIN_CAPACITY 10

FrogType map_type;
hashmap *map_functions;

static void add_bifunc(char *name, builtinfunc fc, ssize_t param)
{
	FrogObject *nm = utf8tostr(name);

	if(!nm)
	{
		errx(-1, "fatal error: memory");
	}

	FrogObject *func = CreateBIFunction(nm, NULL, fc, param);	

	if(!func)
	{
		errx(-1, "fatal error: memory");
	}

	put_hashmap(map_functions, nm, func);
}

void init_map_functions(void)
{
	map_functions = create_hashmap();

	if(!map_functions) errx(-1, "fatal error: memory");

	add_bifunc("put", dict_put, 3);
	add_bifunc("get", dict_get, 2);
	add_bifunc("contains", dict_contains, 2);
	add_bifunc("remove", dict_remove, 2);
	add_bifunc("clear", dict_clear, 1);
}

void free_map(FrogObject *obj)
{
	FrogDict *o = (FrogDict *) obj;

	free_hashmap(o->map);
	free(o);
}

int map_scompare(FrogObject *a, FrogObject *b)
{
	if(ObType(b) != &map_type)
		return 0;

	FrogDict *va = ((FrogDict *) a);
	FrogDict *vb = ((FrogDict *) b);

	if(va->map->size != vb->map->size)
		return 0;

	if(va->map->capacity != vb->map->capacity)
		return 0;

	for(size_t i = 0; i < va->map->capacity; i++)
	{
		struct entry_s *s1 = va->map->entries[i];
		struct entry_s *s2 = vb->map->entries[i];

		while(s1 && s2)
		{
			FrogObject *cmp = FrogCall_EQ(s1->key, s2->key);
			if(!cmp) return -2;
			if(IsFalse(cmp)) return 0;
		
			cmp = FrogCall_EQ(s1->value, s2->value);
			if(!cmp) return -2;
			if(IsFalse(cmp)) return 0;
		}

		if(s1 != s2) return 0;
	}

	return 1;
}

void map_print(FrogObject *obj, FILE *file)
{
	FrogDict *map = (FrogDict *) obj;
	fprintf(file, "{");

	int first = 1;

	for(size_t i = 0; i < map->map->capacity; i++)
	{
		struct entry_s *s = map->map->entries[i];

		while(s)
		{
			if(first) first = 0; else fprintf(file, ", ");

			Frog_Print(s->key, stdout);
			fprintf(file, ": ");
			Frog_Print(s->value, stdout);
			s = s->next;
		}
	}

	fprintf(file, "}");
}

FrogObject *map_to_str(FrogObject *obj)
{
	FrogDict *map = (FrogDict *) obj;
	struct strbuilder *bd = create_strbuilder();

	if(!bd)
	{
		FrogErr_Memory();
		return NULL;
	}

	append_strbuilder(bd, (fchar *) L"{");

	int first = 1;

	for(size_t i = 0; i < map->map->capacity; i++)
	{
		struct entry_s *s = map->map->entries[i];

		while(s)
		{
			if(first)
			{
				first = 0;
			}
			else
			{
				append_strbuilder(bd, (fchar *) L", ");
			}

			FrogObject *str = Frog_ToString(s->key);

			if(!str)
			{
				free_strbuilder(bd);
				FrogErr_Memory();
				return NULL;
			}

			FrogObject *str2 = Frog_ToString(s->value);

			if(!str2)
			{
				free_strbuilder(bd);
				FrogErr_Memory();
				return NULL;
			}
			
			append_strbuilder(bd, ((FrogString *) str)->str);
			append_strbuilder(bd, (fchar *) L": ");
			append_strbuilder(bd, ((FrogString *) str2)->str);
			s = s->next;
		}

	}

	append_strbuilder(bd, (fchar *) L"}");
	fchar *res = tostr_strbuilder(bd);
	free_strbuilder(bd);	

	if(!res)
	{
		FrogErr_Memory();
		return NULL;
	}

	FrogObject *r = utf32tostr(res);
	free(res);

	return r;
}

FrogObject *map_get_at(FrogObject *o, FrogObject *b)
{
	if(!ObType(b)->hash)
	{
		FrogErr_Attribute(b, "hash");
		return NULL;
	}

	FrogDict *s = (FrogDict *) o;
	FrogObject *res = get_hashmap(s->map, b);

	if(!res)
	{
		FrogErr_Key(b);
	}

	return res;
}

FrogObject *map_contains(FrogObject *o, FrogObject *b)
{
	if(!ObType(b)->hash)
	{
		FrogErr_Attribute(b, "hash");
		return NULL;
	}

	FrogDict *s = (FrogDict *) o;
	FrogObject *res = get_hashmap(s->map, b);

	if(!res)
	{
		return FrogFalse();
	}

	return FrogTrue();
}

FrogObject *map_remove(FrogObject *o, FrogObject *b)
{
	if(!ObType(b)->hash)
	{
		FrogErr_Attribute(b, "hash");
		return NULL;
	}

	FrogDict *s = (FrogDict *) o;
	FrogObject *res = remove_hashmap(s->map, b);

	if(!res)
	{
		FrogErr_Key(b);
	}

	return res;
}

void map_clear(FrogObject *o)
{
	FrogDict *s = (FrogDict *) o;
	clear_hashmap(s->map);
}

FrogObject *map_set_at(FrogObject *o, FrogObject *b, FrogObject *c, binaryfunction f)
{
	if(!ObType(b)->hash)
	{
		FrogErr_Attribute(b, "hash");
		return NULL;
	}

	FrogDict *s = (FrogDict *) o;

	if(f)
	{
		FrogObject *tmp = get_hashmap(s->map, b);

		if(!tmp)
		{
			FrogErr_Key(tmp);
			return NULL;
		}

		c = f(tmp, c);
		if(!c) return NULL;
	}

	put_hashmap(s->map, b, c);
	return c;
}

FrogObject *map_size(FrogObject *o)
{
	return FromNativeInteger(((FrogDict *) o)->map->size);
}

FrogObject *map_get(FrogObject *map, FrogObject *value)
{
	FrogObject *func = get_hashmap(map_functions, value);

	if(!func)
	{
		FrogErr_SubName(map, value);
		return NULL;
	}

	return CreateObjFunction(map, func);
}

FrogObject *CreateMap(FrogObject **array, long len)
{
	if(len & 1)
	{
		FrogErr_Post("CallError", "Need an even number of arguments");
		return NULL;
	}

	FrogDict *list = malloc(sizeof(FrogDict));

	if(!list)
	{
		FrogErr_Memory();
		return NULL;
	}

	ObType(list) = &map_type;
	list->map = create_hashmap();

	if(!list->map)
	{
		free(list);
		FrogErr_Memory();
		return NULL;
	}

	FrogObject *res = (FrogObject *) list;

	for(long i = 0; i < len; i += 2)
	{
		if(!map_set_at(res, array[i], array[i + 1], NULL))
		{
			free_hashmap(list->map);
			free(list);
			return NULL;
		}
	}

	return res;
}

FrogAsSequence map_as_sequence =
{
	map_set_at,
	map_get_at,
	map_contains
};

FrogType map_type = {
	{
		-1,		// FIXME type
		NULL		// refcnt
	}, 
	"dict",			// type name
	map_get,		// getter
	NULL,			// setter
	NULL,			// hash
	map_size,		// size
	map_print,		// print
	map_to_str,		// tomap
	NULL,			// toint
	NULL,			// exec
	map_scompare,		// compare
	NULL,			// compare
	NULL,			// as number
	&map_as_sequence,	// as sequence
	NULL,
	NULL,			// call
	free_map		// free
};
