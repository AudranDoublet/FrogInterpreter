#include "../frog.h"

FrogObject *str_toLowerCase(FrogObject **args, size_t count, stack *p)
{
	UNUSED(count);
	UNUSED(p);

	FrogString *str = (FrogString *) (args[0]);

	size_t len = str->length;
	fchar *o = str->str;
	fchar *n = malloc(sizeof(fchar) * (len + 1));

	if(!n)
	{
		FrogErr_Memory();
		return NULL;
	}

	for(size_t i = 0; i <= len; i++)
	{
		if(o[i] >= 'A' && o[i] <= 'Z')
		{
			n[i] = o[i] - 'A' + 'a';
		}
		else n[i] = o[i];
	}

	return str2obj(n, len);
}

FrogObject *str_toUpperCase(FrogObject **args, size_t count, stack *p)
{
	UNUSED(count);
	UNUSED(p);

	FrogString *str = (FrogString *) (args[0]);

	size_t len = str->length;
	fchar *o = str->str;
	fchar *n = malloc(sizeof(fchar) * (len + 1));

	if(!n)
	{
		FrogErr_Memory();
		return NULL;
	}

	for(size_t i = 0; i <= len; i++)
	{
		if(o[i] >= 'a' && o[i] <= 'z')
		{
			n[i] = o[i] - 'a' + 'A';
		}
		else n[i] = o[i];
	}

	return str2obj(n, len);
}
