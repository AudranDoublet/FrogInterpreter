#include "../frog.h"

#define MIN_CAPACITY 10

FrogType set_type;
hashmap *set_functions;

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

	put_hashmap(set_functions, nm, func);
}

void init_set_functions(void)
{
	set_functions = create_hashmap();

	if(!set_functions) errx(-1, "fatal error: memory");

	add_bifunc("add", setlib_add, 2);
	add_bifunc("contains", setlib_contains, 2);
	add_bifunc("remove", setlib_remove, 2);
	add_bifunc("clear", setlib_clear, 1);
}

void free_set(FrogObject *obj)
{
	FrogSet *o = (FrogSet *) obj;

	free_hashmap(o->map);
	free(o);
}

int set_scompare(FrogObject *a, FrogObject *b)
{
	if(ObType(b) != &set_type)
		return 0;

	FrogSet *va = ((FrogSet *) a);
	FrogSet *vb = ((FrogSet *) b);

	if(va->map->size != vb->map->size)
		return 0;


	for(size_t i = 0; i < va->map->capacity; i++)
	{
		struct entry_s *s1 = va->map->entries[i];

		while(s1)
		{
			FrogObject *c = set_contains(b, s1->key);

			if(!c) return -2;
			if(IsFalse(c)) return 0;
		}
	}

	return 1;
}

void set_print(FrogObject *obj, FILE *file)
{
	FrogSet *set = (FrogSet *) obj;
	fprintf(file, "{");

	int first = 1;

	for(size_t i = 0; i < set->map->capacity; i++)
	{
		struct entry_s *s = set->map->entries[i];

		while(s)
		{
			if(first) first = 0; else fprintf(file, ", ");

			Frog_Print(s->key, stdout);
			s = s->next;
		}
	}

	fprintf(file, "}");
}

FrogObject *set_to_str(FrogObject *obj)
{
	FrogSet *set = (FrogSet *) obj;
	struct strbuilder *bd = create_strbuilder();

	if(!bd)
	{
		FrogErr_Memory();
		return NULL;
	}

	append_strbuilder(bd, (fchar *) L"{");

	int first = 1;

	for(size_t i = 0; i < set->map->capacity; i++)
	{
		struct entry_s *s = set->map->entries[i];

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

			append_strbuilder(bd, ((FrogString *) str)->str);
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

FrogObject *set_contains(FrogObject *o, FrogObject *b)
{
	if(!ObType(b)->hash)
	{
		FrogErr_Attribute(b, "hash");
		return NULL;
	}

	FrogSet *s = (FrogSet *) o;
	FrogObject *res = get_hashmap(s->map, b);

	if(!res)
	{
		return FrogFalse();
	}

	return FrogTrue();
}

FrogObject *set_remove(FrogObject *o, FrogObject *b)
{
	if(!ObType(b)->hash)
	{
		FrogErr_Attribute(b, "hash");
		return NULL;
	}

	FrogSet *s = (FrogSet *) o;
	FrogObject *res = remove_hashmap(s->map, b);

	if(!res)
	{
		return FrogFalse();
	}

	return FrogTrue();
}

void set_clear(FrogObject *o)
{
	FrogSet *s = (FrogSet *) o;
	clear_hashmap(s->map);
}

FrogObject *set_inplaceunion(FrogObject *a, FrogObject *b)
{
	if(ObType(b) != &set_type)
		return NULL;

	FrogSet *vb = (FrogSet *) b;

	for(size_t i = 0; i < vb->map->capacity; i++)
	{
		struct entry_s *s = vb->map->entries[i];

		while(s)
		{
			set_add(a, s->key);
			s = s->next;
		}
	}

	return a;
}

FrogObject *set_union(FrogObject *a, FrogObject *b)
{
	if(ObType(b) != &set_type)
		return NULL;

	FrogObject *res = CreateSet(NULL, 0);

	if(!res) return NULL;

	set_inplaceunion(res, a);
	set_inplaceunion(res, b);

	return res;
}

FrogObject *set_union(FrogObject *a, FrogObject *b)
{
	if(ObType(b) != &set_type)
		return NULL;

	FrogObject *res = CreateSet(NULL, 0);

	if(!res) return NULL;

	FrogSet *vb = (FrogSet *) b;

	for(size_t i = 0; i < vb->map->capacity; i++)
	{
		struct entry_s *s = vb->map->entries[i];

		while(s)
		{
			if(IsTrue(set_contains(a, s->key)))
				set_add(a, s->key);
			s = s->next;
		}
	}

	return res;
}

FrogObject *set_add(FrogObject *o, FrogObject *b)
{
	if(!ObType(b)->hash)
	{
		FrogErr_Attribute(b, "hash");
		return NULL;
	}

	FrogSet *s = (FrogSet *) o;
	put_hashmap(s->map, b, b);

	return FrogNone();
}

FrogObject *set_size(FrogObject *o)
{
	return FromNativeInteger(((FrogSet *) o)->map->size);
}

FrogObject *set_get(FrogObject *set, FrogObject *value)
{
	FrogObject *func = get_hashmap(set_functions, value);

	if(!func)
	{
		FrogErr_SubName(set, value);
		return NULL;
	}

	return CreateObjFunction(set, func);
}

FrogObject *CreateSet(FrogObject **array, long len)
{
	if(len & 1)
	{
		FrogErr_Post("CallError", "Need an even number of arguments");
		return NULL;
	}

	FrogSet *list = malloc(sizeof(FrogSet));

	if(!list)
	{
		FrogErr_Memory();
		return NULL;
	}

	ObType(list) = &set_type;
	list->map = create_hashmap();

	if(!list->map)
	{
		free(list);
		FrogErr_Memory();
		return NULL;
	}

	FrogObject *res = (FrogObject *) list;

	for(long i = 0; i < len; i++)
	{
		if(!set_add(res, array[i]))
		{
			free_hashmap(list->map);
			free(list);
			return NULL;
		}
	}

	return res;
}

FrogAsSequence set_as_sequence =
{
	NULL,
	NULL,
	set_contains
};

FrogType set_type = {
	{
		-1,		// FIXME type
		NULL		// refcnt
	}, 
	"set",			// type name
	set_get,		// getter
	NULL,			// setter
	NULL,			// hash
	set_size,		// size
	set_print,		// print
	set_to_str,		// toset
	NULL,			// toint
	NULL,			// exec
	set_scompare,		// compare
	NULL,			// compare
	NULL,			// as number
	&set_as_sequence,	// as sequence
	NULL,
	NULL,			// call
	free_set		// free
};
