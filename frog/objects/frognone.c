#include "../frog.h"

FrogObject none;
FrogType none_type;

long none_hash(FrogObject *o)
{
	UNUSED(o);
	return 0;
}

FrogObject *FrogNone(void)
{
	return &none;
}

int IsNone(FrogObject *o)
{
	return o == &none;
}

int none_scompare(FrogObject *a, FrogObject *b)
{
	UNUSED(a);
	return IsNone(b);
}

int none_ccompare(FrogObject *a, FrogObject *b)
{
	UNUSED(a);
	UNUSED(b);
	return -2;
}

void none_print(FrogObject *o, FILE *f)
{
	UNUSED(o);
	fprintf(f, "None");
}

FrogObject *none_tostr(FrogObject *o)
{
	UNUSED(o);
	return utf8tostr("None");
}

FrogType none_type =
{
	{
		-1,
		NULL//FIIXME
	},
	"NoneType",
	NULL,
	NULL,
	none_hash,
	NULL,
	none_print,
	none_tostr,
	NULL,
	NULL,
	none_scompare,
	none_ccompare,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};

FrogObject none = {
		-1,
		&none_type
};

