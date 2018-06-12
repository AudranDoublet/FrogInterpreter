#include "../frog.h"
#include <string.h>

#define UTF8_MASK1 0x1F
#define UTF8_MASK2 0x0F
#define UTF8_MASK3 0x07
#define UTF8_MASKN 0x3F

#define OFFSET		16777619
#define FNV_PRIME	1099511628211

#define UTF8_NEXT(c1, c2, n)	\
	if(!(c1 = *(++utf8)))	\
		goto utf8error;	\
	else			\
		c2 |= (c1 & UTF8_MASKN) << n;

#define FrogSValue(s) (((FrogString *) s)->str)
#define FrogSLength(s) (((FrogString *)s)->length)

FrogType str_type;
hashmap *str_functions;

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

	put_hashmap(str_functions, nm, func);
}

void init_str_functions(void)
{
	str_functions = create_hashmap();

	if(!str_functions) errx(-1, "fatal error: memory");

	add_bifunc("toLowerCase", str_toLowerCase, 1);
	add_bifunc("toUpperCase", str_toUpperCase, 1);
}

FrogObject *utf8tostr(char *str)
{
	if(!str)
		return NULL;

	struct strbuilder *builder = NULL;
	uint8_t *utf8 = NULL;
	FrogString *res = NULL;

	uint8_t c1 = 0;
	fchar c2 = 0;

	utf8 = (uint8_t *) str;
	builder = create_strbuilder();
	if(!builder) goto error;

	res = malloc(sizeof(FrogString));
	if(!res)
	{
		goto error;
	}

	ObType(res) = &str_type;
	ObRefcnt(res) = 0;

	res->length = 0;
	res->hash = 0;

	for(; *utf8; utf8++)
	{
		c1 = *utf8;

		if(c1 >> 7 == 0)
		{
			c2 = c1;
		}
		else if( ((c1 >> 5) & 1) == 0)
		{
			c2 = (c1 & UTF8_MASK1) << 6;
			UTF8_NEXT(c1, c2, 0);
		}
		else if( ((c1 >> 4) & 1) == 0)
		{
			c2 = (c1 & UTF8_MASK2) << 12;
			UTF8_NEXT(c1, c2, 6);
			UTF8_NEXT(c1, c2, 0);
			printf(" %i\n", c2);
		}
		else if( ((c1 >> 3) & 1) == 0)
		{
			c2 = (c1 & UTF8_MASK3) << 18;
			UTF8_NEXT(c1, c2, 12);
			UTF8_NEXT(c1, c2, 6);
			UTF8_NEXT(c1, c2, 0);
		}
		else
		{
			goto utf8error;
		}

		add_strbuilder(builder, c2);
	}

	res->str = tostr_strbuilder(builder);

	if(!res->str)
		goto error;

	res->length = builder->length;
	free_strbuilder(builder);

	return (FrogObject *) res;
utf8error:
	//FrogErr_Post("TypeError", "invalid utf8 string");
error:
	free_strbuilder(builder);
	free(res);
	return NULL;
}

FrogObject *utf32tostr(fchar *str)
{
	if(str == NULL)
		return NULL;

	FrogString *res = malloc(sizeof(FrogString));

	if(!str)
	{
		FrogErr_Memory();
		return NULL;
	}

	ObType(res) = &str_type;
	ObRefcnt(res) = 0;

	res->length = 0;
	res->str = str;
	res->hash = 0;

	for( ; *str ; str++ )
		res->length++;

	return (FrogObject *) res;
}

FrogObject *str2obj(fchar *str, size_t len)
{
	FrogString *res = malloc(sizeof(FrogString));

	if(!res)
	{
		FrogErr_Memory();
		return NULL;
	}

	ObType(res) = &str_type;
	ObRefcnt(res) = 0;

	res->length = len;
	res->str = str;
	res->hash = 0;

	return (FrogObject *) res;
}

char *strtoutf8(FrogObject *obj)
{
	FrogString *o = (FrogString *) obj;
	char *result = malloc(sizeof(fchar) * o->length);

	if(!result)
	{
		FrogErr_Memory();
		return NULL;
	}

	sprintf(result, "%ls", o->str);
	return result;
}

void free_str(FrogObject *obj)
{
	FrogString *o = (FrogString *) obj;
	free(o->str);
}

long str_hash(FrogObject *obj)
{
	FrogString *s = (FrogString *) obj;

	if(s->hash)
		return s->hash;

	s->hash = OFFSET;
	uint8_t *str = (uint8_t *) s->str;
	size_t max = s->length * 4;

	for(size_t i = 0; i < max; i++)
		s->hash = (s->hash ^ *str) * FNV_PRIME;

	return s->hash;
}

int str_scompare(FrogObject *a, FrogObject *b)
{
	if(ObType(b) != &str_type)
		return 0;

	fchar *va = ((FrogString *) a)->str;
	fchar *vb = ((FrogString *) b)->str;

	for( ; *va && *vb; va++, vb++)
	{
		if(*va != *vb)
			return 0;
	}

	return *va == *vb;
}

int str_ccompare(FrogObject *a, FrogObject *b)
{
	if(ObType(b) != &str_type)
		return -2;

	fchar *va = ((FrogString *) a)->str;
	fchar *vb = ((FrogString *) b)->str;

	for( ; *va && *vb; va++, vb++)
	{
		if(*va != *vb)
			return *va < *vb ? -1 : 1;
	}

	if(!*va)
		return !*vb ? 0 : -1;
	return 0;
}

void str_print(FrogObject *str, FILE *file)
{
	fprintf(file, "%ls", FrogSValue(str));
}

FrogObject *str_to_str(FrogObject *obj)
{
	return obj;
}

FrogObject *str_nget_charat(FrogObject *o, size_t pos)
{
	FrogString *s = (FrogString *) o;
	fchar *v = malloc(sizeof(fchar) * 2);

	if(!v)
	{
		FrogErr_Memory();
		return NULL;
	}

	*v = s->str[pos];
	*(v + 1) = L'\0';
	return utf32tostr(v);
}

FrogObject *str_get_charat(FrogObject *o, FrogObject *b)
{
	if(!FrogIsInt(b))
	{
		FrogErr_TypeS("string indices must be integers");
		return NULL;
	}

	FrogString *s = (FrogString *) o;
	long pos = FIValue(b);

	if(pos < 0) pos = s->length + pos;

	if(pos < 0 || pos >= (long) s->length)
	{
		FrogErr_Index();
		return NULL;
	}

	return str_nget_charat(o, pos);
}

FrogObject *str_contains(FrogObject *o, FrogObject *v)
{
	return FrogFalse();
}

FrogObject *str_add(FrogObject *a, FrogObject *b)
{
	if(ObType(b) == &str_type)
	{
		FrogString *s1 = (FrogString *) a;
		FrogString *s2 = (FrogString *) b;

		size_t final = s1->length + s2->length;
		unsigned int *res = malloc(sizeof(int) * (final + 1));

		if(!res)
			return NULL;

		memcpy(res, s1->str, sizeof(int) * s1->length);
		memcpy(res + s1->length, s2->str, sizeof(int) * s2->length);

		res[final] = '\0';

		FrogString *fin = malloc(sizeof(FrogString));

		if(!fin)
			return NULL;

		ObType(fin) = &str_type;
		fin->length = final;
		fin->str = res;
		fin->hash = 0;

		return (FrogObject *) fin;
	}
	else
	{
		return NULL;
	}
}

FrogObject *str_mul(FrogObject *a, FrogObject *b)
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
	FrogString *s = (FrogString *) a;

	size_t len = s->length * v;
	unsigned int *res = malloc(sizeof(int) * (len + 1));

	if(!res)
		return NULL;

	for(long i = 0; i < v; i++)
		memcpy(res + i * s->length, s->str, sizeof(int) * s->length);

	res[len] = '\0';
	FrogString *fin = malloc(sizeof(FrogString));

	if(!fin)
		return NULL;

	ObType(fin) = &str_type;
	fin->length = len;
	fin->str = res;
	fin->hash = 0;

	return (FrogObject *) fin;
}

FrogObject *str_size(FrogObject *o)
{
	return FromNativeInteger(((FrogString *) o)->length);
}

FrogObject *str_get(FrogObject *str, FrogObject *value)
{
	FrogObject *func = get_hashmap(str_functions, value);

	if(!func)
	{
		FrogErr_SubName(str, value);
		return NULL;
	}

	return CreateObjFunction(str, func);
}

FrogObject *str_i_init(FrogObject *o)
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

FrogObject *str_i_next(FrogObject *o)
{
	FrogIter *iter = (FrogIter *) o;

	size_t pos = (size_t) iter->values[0];
	iter->values[0] = (void *) (pos + 1);

	return str_nget_charat(iter->base, pos);
}

FrogObject *str_i_hasnext(FrogObject *o)
{
	FrogIter *iter = (FrogIter *) o;

	size_t pos = (size_t) iter->values[0];
	size_t len = ((FrogString *) iter->base)->length;

	return pos < len ? FrogTrue() : FrogFalse();
}

FrogAsSequence str_as_sequence =
{
	NULL,
	str_get_charat,
	str_contains
};

FrogAsIterable str_as_iterable = {
	str_i_init,
	str_i_next,
	str_i_hasnext
};

FrogAsNumber str_as_number = {
	str_add,
	NULL,
	str_mul,
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

FrogType str_type = {
	{
		-1,		// FIXME type
		NULL		// refcnt
	}, 
	"str",			// type name
	str_get,		// getter
	NULL,			// setter
	str_hash,		// hash
	str_size,		// size
	str_print,		// print
	str_to_str,		// tostr
	NULL,			// toint
	NULL,			// exec
	str_scompare,		// compare
	str_ccompare,		// compare
	&str_as_number,		// as number
	&str_as_sequence,	// as sequence
	&str_as_iterable,
	NULL,			// call
	free_str		// free
};
