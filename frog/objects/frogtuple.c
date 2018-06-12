#include "../frog.h"

#define MIN_CAPACITY 10

FrogType tuple_type;

void free_tuple(FrogObject *obj)
{
	FrogTuple *o = (FrogTuple *) obj;

	for(size_t i = 0; i < o->length; i++)
	{
		Unref(o->array[i]);
	}

	free(o->array);
	free(o);
}

int tuple_scompare(FrogObject *a, FrogObject *b)
{
	if(ObType(b) != &tuple_type)
		return 0;

	FrogTuple *va = ((FrogTuple *) a);
	FrogTuple *vb = ((FrogTuple *) b);

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

int tuple_ccompare(FrogObject *a, FrogObject *b)
{
	if(ObType(b) != &tuple_type)
		return -2;

	FrogTuple *va = ((FrogTuple *) a);
	FrogTuple *vb = ((FrogTuple *) b);

	for(size_t i = 0; i < va->length && i < vb->length; i++)
	{
		int cmp = FrogCall_Compare(va->array[i], vb->array[i]);
		if(cmp == -2) return -2;

		if(cmp != 0) return cmp;
	}

	if(va->length == vb->length) return 0;
	return va->length < vb->length ? -1 : 1;
}

void tuple_print(FrogObject *obj, FILE *file)
{
	FrogTuple *tuple = (FrogTuple *) obj;
	fprintf(file, "(");

	for(size_t i = 0; i < tuple->length; i++)
	{
		if(i != 0)
			fprintf(file, ", ");

		Frog_Print(tuple->array[i], stdout);
	}

	fprintf(file, ")");
}

FrogObject *tuple_to_str(FrogObject *obj)
{
	FrogTuple *tuple = (FrogTuple *) obj;
	struct strbuilder *bd = create_strbuilder();

	if(!bd)
	{
		FrogErr_Memory();
		return NULL;
	}

	append_strbuilder(bd, (fchar *) L"(");

	for(size_t i = 0; i < tuple->length; i++)
	{
		if(i != 0)
			append_strbuilder(bd, (fchar *) L", ");

		FrogObject *str = Frog_ToString(tuple->array[i]);

		if(!str)
		{
			free_strbuilder(bd);
			FrogErr_Memory();
			return NULL;
		}

		append_strbuilder(bd, ((FrogString *) str)->str);
	}

	append_strbuilder(bd, (fchar *) L")");
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
	FrogTuple *s = (FrogTuple *) o;

	if(pos < 0) pos = s->length + pos;

	if(pos < 0 || pos >= (long) s->length)
	{
		FrogErr_Index();
		return -1;
	}

	return pos;
}

FrogObject *tuple_get_at(FrogObject *o, FrogObject *b)
{
	if(!FrogIsInt(b))
	{
		FrogErr_TypeS("list indices must be integers");
		return NULL;
	}

	FrogTuple *s = (FrogTuple *) o;
	long pos = fix_pos(o, FIValue(b));
	if(pos < 0) return NULL;

	return s->array[pos];
}

FrogObject *tuple_contains(FrogObject *o, FrogObject *v)
{
	FrogList *l = (FrogList *) o;

	for(size_t i = 0; i < l->length; i++)
		if(IsTrue( FrogCall_EQ(l->array[i], v) ))
			return FrogTrue();

	return FrogFalse();
}

FrogObject *tuple_add(FrogObject *a, FrogObject *b)
{
	if(ObType(b) == &tuple_type)
	{
		FrogTuple *s1 = (FrogTuple *) a;
		FrogTuple *s2 = (FrogTuple *) b;

		size_t final = s1->length + s2->length;
		FrogObject **res = malloc(sizeof(FrogObject *) * (final));

		if(!res)
		{
			FrogErr_Memory();
			return NULL;
		}

		memcpy(res, s1->array, sizeof(FrogObject *) * s1->length);
		memcpy(res + s1->length, s2->array, sizeof(FrogObject *) * s2->length);

		FrogTuple *fin = malloc(sizeof(FrogTuple));

		if(!fin)
		{
			FrogErr_Memory();
			return NULL;
		}

		ObType(fin) = &tuple_type;
		fin->length = final;
		fin->array = res;

		return (FrogObject *) fin;
	}
	else
	{
		return NULL;
	}
}

FrogObject *tuple_mul(FrogObject *a, FrogObject *b)
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
	FrogTuple *s = (FrogTuple *) a;

	size_t len = s->length * v;
	FrogObject **res = malloc(sizeof(FrogObject *) * len);

	if(!res)
	{
		FrogErr_Memory();
		return NULL;
	}

	for(long i = 0; i < v; i++)
		memcpy(res + i * s->length, s->array, sizeof(FrogObject *) * s->length);

	FrogTuple *fin = malloc(sizeof(FrogTuple));

	if(!fin)
	{
		FrogErr_Memory();
		return NULL;
	}

	ObType(fin) = &tuple_type;
	fin->length = len;
	fin->array = res;

	return (FrogObject *) fin;
}

FrogObject *tuple_size(FrogObject *o)
{
	return FromNativeInteger(((FrogTuple *) o)->length);
}

FrogObject *CreateTuple(FrogObject **array, long len)
{
	FrogTuple *list = malloc(sizeof(FrogTuple));

	if(!list)
	{
		FrogErr_Memory();
		return NULL;
	}

	ObType(list) = &tuple_type;

	list->length = len;
	list->array = malloc(sizeof(FrogObject *) * list->length);

	if(!list->array)
	{
		free(list);
		FrogErr_Memory();
		return NULL;
	}

	memcpy(list->array, array, len * sizeof(FrogObject *));
	return (FrogObject *) list;
}

FrogObject *tuple_i_init(FrogObject *o)
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

FrogObject *tuple_i_next(FrogObject *o)
{
	FrogIter *iter = (FrogIter *) o;

	size_t pos = (size_t) iter->values[0];

	FrogObject *res = ((FrogTuple *) iter->base)->array[pos++];
	iter->values[0] = (void *) pos;

	return res;
}

FrogObject *tuple_i_hasnext(FrogObject *o)
{
	FrogIter *iter = (FrogIter *) o;

	size_t pos = (size_t) iter->values[0];
	size_t len = ((FrogTuple *) iter->base)->length;

	return pos < len ? FrogTrue() : FrogFalse();
}

int FrogIsTuple(FrogObject *o)
{
	return ObType(o) == &tuple_type;
}

FrogAsSequence tuple_as_sequence =
{
	NULL,
	tuple_get_at,
	tuple_contains
};

FrogAsIterable tuple_as_iterable = {
	tuple_i_init,
	tuple_i_next,
	tuple_i_hasnext
};

FrogAsNumber tuple_as_number = {
	tuple_add,
	NULL,
	tuple_mul,
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

FrogType tuple_type = {
	{
		-1,		// FIXME type
		NULL		// refcnt
	}, 
	"tuple",			// type name
	NULL,			// getter
	NULL,			// setter
	NULL,			// hash
	tuple_size,		// size
	tuple_print,		// print
	tuple_to_str,		// totuple
	NULL,			// toint
	NULL,			// exec
	tuple_scompare,		// compare
	tuple_ccompare,		// compare
	NULL,			// as number
	&tuple_as_sequence,	// as sequence
	&tuple_as_iterable,	// as iterable
	NULL,			// call
	free_tuple		// free
};
