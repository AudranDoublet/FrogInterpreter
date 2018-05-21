#ifndef __FROGOBJECTS_STR_H__
#define __FROGOBJECTS_STR_H__

typedef struct
{
	FrogObjHead
	size_t length;
	long hash;

	unsigned int *str;
} FrogString;

FrogObject *utf8tostr(char *utf8);

FrogObject *utf32tostr(fchar *str);

FrogObject *str2obj(fchar *str, size_t len);

char *strtoutf8(FrogObject *obj);

long str_hash(FrogObject *obj);

void init_str_functions(void);
#endif
