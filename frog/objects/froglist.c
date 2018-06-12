#include "../frog.h"

#define MIN_CAPACITY 10

FrogType lst_type;
hashmap *lst_functions;

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

	put_hashmap(lst_functions, nm, func);
}

void init_lst_functions(void)
{
	lst_functions = create_hashmap();

	if(!lst_functions) errx(-1, "fatal error: memory");

	add_bifunc("append", list_append, 2);
	add_bifunc("pop", list_pop, 1);
	add_bifunc("insert", list_insert, 3);
	add_bifunc("remove", list_remove, 2);
}

void free_lst(FrogObject *obj)
{
	FrogList *o = (FrogList *) obj;

	for(size_t i = 0; i < o->length; i++)
	{
		Unref(o->array[i]);
	}

	free(o->array);
	free(o);
}

int lst_scompare(FrogObject *a, FrogObject *b)
{
	if(ObType(b) != &lst_type)
		return 0;

	FrogList *va = ((FrogList *) a);
	FrogList *vb = ((FrogList *) b);

	if(va->length != vb->length)
		return 0;

	for(size_t i = 0; i < va->length; i++)
	{
		FrogObject *cmp = FrogCall_EQ(va->array[i], vb->array[i]);
		if(!cmp) return -2;

		if(IsFalse(cmp)) return 0;
	}

	return 1;
}

int lst_ccompare(FrogObject *a, FrogObject *b)
{
	if(ObType(b) != &lst_type)
		return -2;

	FrogList *va = ((FrogList *) a);
	FrogList *vb = ((FrogList *) b);

	for(size_t i = 0; i < va->length && i < vb->length; i++)
	{
		int cmp = FrogCall_Compare(va->array[i], vb->array[i]);
		if(cmp == -2) return -2;

		if(cmp != 0) return cmp;
	}

	if(va->length == vb->length) return 0;
	return va->length < vb->length ? -1 : 1;
}

void lst_print(FrogObject *obj, FILE *file)
{
	FrogList *lst = (FrogList *) obj;
	fprintf(file, "[");

	for(size_t i = 0; i < lst->length; i++)
	{
		if(i != 0)
			fprintf(file, ", ");

		Frog_Print(lst->array[i], stdout);
	}

	fprintf(file, "]");
}

FrogObject *lst_to_str(FrogObject *obj)
{
	FrogList *lst = (FrogList *) obj;
	struct strbuilder *bd = create_strbuilder();

	if(!bd)
	{
		FrogErr_Memory();
		return NULL;
	}

	append_strbuilder(bd, (fchar *) L"[");

	for(size_t i = 0; i < lst->length; i++)
	{
		if(i != 0)
			append_strbuilder(bd, (fchar *) L", ");

		FrogObject *str = Frog_ToString(lst->array[i]);

		if(!str)
		{
			free_strbuilder(bd);
			FrogErr_Memory();
			return NULL;
		}

		append_strbuilder(bd, ((FrogString *) str)->str);
	}

	append_strbuilder(bd, (fchar *) L"]");
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

static long fix_pos(FrogObject *o, long pos)
{
	FrogList *s = (FrogList *) o;

	if(pos < 0) pos = s->length + pos;

	if(pos < 0 || pos >= (long) s->length)
	{
		FrogErr_Index();
		return -1;
	}

	return pos;
}

FrogObject *lst_get_at(FrogObject *o, FrogObject *b)
{
	if(!FrogIsInt(b))
	{
		FrogErr_TypeS("list indices must be integers");
		return NULL;
	}

	FrogList *s = (FrogList *) o;
	long pos = fix_pos(o, FIValue(b));
	if(pos < 0) return NULL;

	return s->array[pos];
}

FrogObject *lst_set_at(FrogObject *o, FrogObject *b, FrogObject *c, binaryfunction f)
{
	if(!FrogIsInt(b))
	{
		FrogErr_TypeS("list indices must be integers");
		return NULL;
	}

	FrogList *s = (FrogList *) o;
	long pos = fix_pos(o, FIValue(b));

	if(pos < 0) return NULL;

	if(f)
	{
		c = f(s->array[pos], c);
		if(!c) return NULL;
	}

	s->array[pos] = c;
	return c;
}

FrogObject *lst_add(FrogObject *a, FrogObject *b)
{
	if(ObType(b) == &lst_type)
	{
		FrogList *s1 = (FrogList *) a;
		FrogList *s2 = (FrogList *) b;

		size_t final = s1->length + s2->length;
		FrogObject **res = malloc(sizeof(FrogObject *) * (final));

		if(!res)
		{
			FrogErr_Memory();
			return NULL;
		}

		memcpy(res, s1->array, sizeof(FrogObject *) * s1->length);
		memcpy(res + s1->length, s2->array, sizeof(FrogObject *) * s2->length);

		FrogList *fin = malloc(sizeof(FrogList));

		if(!fin)
		{
			FrogErr_Memory();
			return NULL;
		}

		ObType(fin) = &lst_type;
		fin->length = final;
		fin->capacity = final;
		fin->array = res;

		return (FrogObject *) fin;
	}
	else
	{
		return NULL;
	}
}

FrogObject *lst_mul(FrogObject *a, FrogObject *b)
{
	if(ObType(b)->toint)
	{
		b = (*ObType(b)->toint)(b);
	}
	else
	{
		return NULL;
	}

	long v = FIValue(b);

	if(v < 0) v = 0;
	FrogList *s = (FrogList *) a;

	size_t len = s->length * v;
	FrogObject **res = malloc(sizeof(FrogObject *) * len);

	if(!res)
	{
		FrogErr_Memory();
		return NULL;
	}

	for(long i = 0; i < v; i++)
		memcpy(res + i * s->length, s->array, sizeof(FrogObject *) * s->length);

	FrogList *fin = malloc(sizeof(FrogList));

	if(!fin)
	{
		FrogErr_Memory();
		return NULL;
	}

	ObType(fin) = &lst_type;
	fin->length = len;
	fin->capacity = len;
	fin->array = res;

	return (FrogObject *) fin;
}

FrogObject *lst_size(FrogObject *o)
{
	return FromNativeInteger(((FrogList *) o)->length);
}

FrogObject *lst_contains(FrogObject *o, FrogObject *v)
{
	FrogList *l = (FrogList *) o;

	for(size_t i = 0; i < l->length; i++)
	{
		FrogObject *o = FrogCall_EQ(l->array[i], v);

		if(!o) return NULL;

		if(IsTrue(o))
			return FrogTrue();
	}

	return FrogFalse();
}

FrogObject *lst_get(FrogObject *lst, FrogObject *value)
{
	FrogObject *func = get_hashmap(lst_functions, value);

	if(!func)
	{
		FrogErr_SubName(lst, value);
		return NULL;
	}

	return CreateObjFunction(lst, func);
}

int lst_min_capacity(FrogObject *list, size_t size)
{
	FrogList *l = (FrogList *) list;
	if(l->capacity >= size) return 1;

	size_t cap = l->capacity + (l->capacity >> 1);
	if(cap < size) cap = size;

	FrogObject **tmp = realloc(l->array, cap * sizeof(FrogObject *));

	if(!tmp)
	{
		FrogErr_Memory();
		return 0;
	}

	l->array = tmp;
	l->capacity = cap;
	return 1;
}

int ListAppend(FrogObject *list, FrogObject *value)
{
	FrogList *l = (FrogList *) list;
	
	if(!lst_min_capacity(list, l->length + 1))
	{
		return 0;
	}

	l->array[l->length++] = value;
	return 1;
}

int ListInsert(FrogObject *list, FrogObject *value, long pos)
{
	FrogList *l = (FrogList *) list;
	pos = fix_pos(list, pos);

	if(pos < 0) return 0;
	if(!lst_min_capacity(list, l->length + 1)) return 0;

	memmove(l->array + pos + 1, l->array + pos,
		(l->length - (size_t) pos) * sizeof(FrogObject *));

	l->array[pos] = value;
	l->length++;

	return 1;
}

FrogObject *ListPop(FrogObject *list)
{
	FrogList *l = (FrogList *) list;
	return l->array[--l->length];
}

FrogObject *ListRemove(FrogObject *list, long pos)
{
	FrogList *l = (FrogList *) list;
	pos = fix_pos(list, pos);

	if(pos < 0) return 0;

	FrogObject *res = l->array[pos];
	l->length--;

	memmove(l->array + pos, l->array + pos + 1,
		(l->length - (size_t) pos) * sizeof(FrogObject *));

	return res;
}

FrogObject *CreateList(FrogObject **array, long len)
{
	FrogList *list = malloc(sizeof(FrogList));

	if(!list)
	{
		FrogErr_Memory();
		return NULL;
	}

	ObType(list) = &lst_type;

	list->length = len;
	list->capacity = (len < MIN_CAPACITY) ? MIN_CAPACITY : len;

	list->array = malloc(sizeof(FrogObject *) * list->capacity);

	if(!list->array)
	{
		free(list);
		FrogErr_Memory();
		return NULL;
	}

	memcpy(list->array, array, len * sizeof(FrogObject *));
	return (FrogObject *) list;
}

FrogObject *lst_i_init(FrogObject *o)
{
	FrogIter *iter = (FrogIter *) o;
	iter->values = calloc(sizeof(void *), 2);

	if(!iter->values)
	{
		FrogErr_Memory();
		return NULL;
	}

	iter->values[0] = (void *) 0;
	return o;
}

FrogObject *lst_i_next(FrogObject *o)
{
	FrogIter *iter = (FrogIter *) o;

	size_t pos = (size_t) iter->values[0];

	FrogObject *res = ((FrogList *) iter->base)->array[pos++];
	iter->values[0] = (void *) pos;

	return res;
}

FrogObject *lst_i_hasnext(FrogObject *o)
{
	FrogIter *iter = (FrogIter *) o;

	size_t pos = (size_t) iter->values[0];
	size_t len = ((FrogList *) iter->base)->length;

	return pos < len ? FrogTrue() : FrogFalse();
}

FrogAsSequence lst_as_sequence =
{
	lst_set_at,
	lst_get_at,
	lst_contains
};

FrogAsIterable lst_as_iterable = {
	lst_i_init,
	lst_i_next,
	lst_i_hasnext
};

FrogAsNumber lst_as_number = {
	lst_add,
	NULL,
	lst_mul,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};

FrogType lst_type = {
	{
		-1,		// FIXME type
		NULL		// refcnt
	}, 
	"lst",			// type name
	lst_get,		// getter
	NULL,			// setter
	NULL,			// hash
	lst_size,		// size
	lst_print,		// print
	lst_to_str,		// tolst
	NULL,			// toint
	NULL,			// exec
	lst_scompare,		// compare
	lst_ccompare,		// compare
	&lst_as_number,		// as number
	&lst_as_sequence,	// as sequence
	&lst_as_iterable,	// as iterable
	NULL,			// call
	free_lst		// free
};
