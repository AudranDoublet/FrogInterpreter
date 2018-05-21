#include "../frog.h"

FrogBool trueObj, falseObj;
FrogType bool_type;

FrogAsNumber int_as_number;
int int_scompare(FrogObject *a, FrogObject *b);
int int_ccompare(FrogObject *a, FrogObject *b);


long bool_hash(FrogObject *o)
{
	return IsTrue(o) ? 1 : 0;
}

FrogObject *FrogTrue(void)
{
	return (FrogObject *) &trueObj;
}

FrogObject *FrogFalse(void)
{
	return (FrogObject *) &falseObj;
}

FrogObject *bool_as_int(FrogObject *o)
{
	return FromNativeInteger(IsTrue(o));
}

int IsTrue(FrogObject *o)
{
	return o == FrogTrue();
}

int IsFalse(FrogObject *o)
{
	return o == FrogFalse();
}

void bool_print(FrogObject *o, FILE *f)
{
	fprintf(f, IsTrue(o) ? "True" : "False");
}

FrogObject *bool_tostr(FrogObject *o)
{
	return utf8tostr(IsTrue(o) ? "True" : "False");
}

FrogType bool_type =
{
	{
		-1,
		NULL//FIXME
	},
	"bool",
	NULL,
	NULL,
	bool_hash,
	NULL,
	bool_print,
	bool_tostr,
	bool_as_int,
	NULL,
	int_scompare,
	int_ccompare,
	&int_as_number,
	NULL,
	NULL,
	NULL,
	NULL
};

FrogBool trueObj = {
	{
		-1,
		&bool_type
	},
	1
};

FrogBool falseObj = {
	{
		-1,
		&bool_type
	},
	0
};

