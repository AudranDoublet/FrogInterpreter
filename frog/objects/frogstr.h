#ifndef __FROGOBJECTS_STR_H__
#define __FROGOBJECTS_STR_H__

#define CHAR_AT(s, i) //FIXME
#define LENGTH(s) 


typedef struct
{
	FrogObjHead
	size_t length;
	long hash;

	unsigned int *str;
} FrogString;
i

typedef struct
{
	void *array;
	size_t length;
	size_t mode;
} fstring;

void init_str_functions(void);

FrogObject *fstring_toobj(fstring *str);

fstring *fstring_fromUTF8(uint8_t *str);

fstring *fstring_fromUTF16(uint16_t *str);

fstring *fstring_fromUTF32(uint32_t *str);

char *fstring_toUTF8(uint8_t *str);

long fstring_hash(fstring *s);

fstring *fstring_sub(fstring *s, size_t start, size_t end);

fstring *fstring_concat(fstring *a, fstring *b);

fstring *fstring_mult(fstring *s, size_t count);

int fstring_indexof(fstring *s, fstring *sub);


fstring **fstring_split(fstring *s, fstring *splitter, size_t *count, ssize_t max);

fstring *fstring_join(fstring *joiner, FrogObject **values, size_t count);

fstring *fstring_replace(fstring *s, fstring *search, fstring *replace, ssize_t max);

fstring *fstring_tolower(fstring *s);

fstring *fstring_toupper(fstring *s);

fstring *fstring_trim(fstring *s);

fstring *fstring_isalnum(fstring *s);

fstring *fstring_isalpha(fstring *s);

fstring *fstring_islower(fstring *s);

fstring *fstring_isupper(fstring *s);

fstring *fstring_isnumeric(fstring *s);

fstring *fstring_isspace(fstring *s);
#endif
