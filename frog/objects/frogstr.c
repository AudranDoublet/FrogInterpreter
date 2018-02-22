#include "../frog.h"

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
			printf("a");
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
		return NULL;

	ObType(res) = &str_type;
	ObRefcnt(res) = 0;

	res->length = 0;
	res->str = str;
	res->hash = 0;

	for( ; *str ; str++ )
		res->length++;

	return (FrogObject *) res;
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
	fprintf(file, "'%ls'", FrogSValue(str));
}

FrogObject *str_to_str(FrogObject *obj)
{
	return obj;
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

size_t str_size(FrogObject *o)
{
	return ((FrogString *) o)->length;
}

FrogAsSequence str_as_sequence =
{
	NULL,
	str_get_charat
};

FrogType str_type = {
	{
		-1,		// FIXME type
		NULL		// refcnt
	}, 
	"str",			// type name
	NULL,			// FIXME getter
	NULL,			// setter
	str_hash,		// hash
	str_size,		// size
	str_print,		// print
	str_to_str,		// tostr
	NULL,			// toint
	NULL,			// exec
	str_scompare,		// compare
	str_ccompare,		// compare
	NULL,			// as number
	&str_as_sequence,	// as sequence
	NULL,			// call
	free_str		// free
};
