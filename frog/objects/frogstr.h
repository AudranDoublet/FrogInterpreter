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
#endif
