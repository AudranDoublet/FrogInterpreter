#include "../frog.h"

FrogType flt_type;

static int get_value(FrogObject *a, FrogObject *b, double *va, double *vb)
{
	*va = FFValue(a);

	if(FrogIsFloat(b))
	{
		*vb = FFValue(b);
		return 1;
	}
	else if(ObType(b)->toint)
	{
		b = (ObType(b)->toint)(b);
		if(!b) return 0;
		*vb = (double) FIValue(b);
	
		return 1;
	}

	return 0;
}

FrogObject *FromNativeFloat(double i)
{
	FrogFloat *result = malloc(sizeof(FrogFloat));

	if(!result)
	{
		FrogErr_Memory();
		return NULL;
	}

	result->value = i;
	ObType(result) = &flt_type;
	ObRefcnt(result) = 0;

	return (FrogObject *) result;
}

void flt_print(FrogObject *obj, FILE *f)
{
	fprintf(f, "%lf", FFValue(obj));
}

FrogObject *flt_to_str(FrogObject *obj)
{
	struct strbuilder *builder = create_strbuilder();
	long value = FFValue(obj);

	if(!builder || !addint_strbuilder(builder, value, 10))
		goto error;

	fchar *res = tostr_strbuilder(builder);

	if(!res)
		goto error;

	free_strbuilder(builder);
	return utf32tostr(res);
error:
	FrogErr_Memory();
	free_strbuilder(builder);
	return NULL;
}

FrogObject *flt_to_int(FrogObject *obj)
{
	return FromNativeInteger((long) FFValue(obj));
}

long flt_hash(FrogObject *obj)
{
	return FIValue(obj);
}

int flt_scompare(FrogObject *a, FrogObject *b)
{
	double va, vb;

	if(get_value(a, b, &va, &vb))
	{
		return va == vb;
	}

	return 0;
}

int flt_ccompare(FrogObject *a, FrogObject *b)
{
	double va, vb;

	if(get_value(a, b, &va, &vb))
	{
		return va < vb ? -1 : (va > vb);
	}

	return -2;
}

FrogObject *flt_add(FrogObject *a, FrogObject *b)
{
	double va, vb;

	if(get_value(a, b, &va, &vb))
	{
		return FromNativeFloat(va + vb);
	}

	return NULL;
}

FrogObject *flt_sub(FrogObject *a, FrogObject *b)
{
	double va, vb;

	if(get_value(a, b, &va, &vb))
	{
		return FromNativeFloat(va - vb);
	}

	return NULL;
}

FrogObject *flt_mul(FrogObject *a, FrogObject *b)
{
	double va, vb;

	if(get_value(a, b, &va, &vb))
	{
		return FromNativeFloat(va * vb);
	}

	return NULL;
}

FrogObject *flt_div(FrogObject *a, FrogObject *b)
{
	double va, vb;

	if(get_value(a, b, &va, &vb))
	{
		return FromNativeFloat(va / vb);
	}

	return NULL;
}

FrogObject *flt_divfloor(FrogObject *a, FrogObject *b)
{
	double va, vb;

	if(get_value(a, b, &va, &vb))
	{
		return FromNativeFloat(floor(va / vb));
	}

	return NULL;
}

FrogObject *flt_mod(FrogObject *a, FrogObject *b)
{
	double va, vb;

	if(get_value(a, b, &va, &vb))
	{
		if(vb == 0)
		{
			FrogErr_Div0();
			return NULL;
		}

		return FromNativeFloat(fmod(va, vb));
	}

	return NULL;
}

FrogObject *flt_pow(FrogObject *a, FrogObject *b)
{
	double va, vb;

	if(get_value(a, b, &va, &vb))
	{
		return FromNativeFloat(pow(va, vb));
	}

	return NULL;
}

FrogObject *flt_neg(FrogObject *a)
{
	return FromNativeFloat(-FFValue(a));
}

FrogObject *flt_pos(FrogObject *a)
{
	return FromNativeFloat(FFValue(a));
}

int FrogIsFloat(FrogObject *o)
{
	return o && o->type == &flt_type;
}

FrogAsNumber flt_as_number = {
	flt_add,
	flt_sub,
	flt_mul,
	flt_div,
	flt_divfloor,
	flt_mod,
	flt_pow,
	flt_neg,
	flt_pos,
	flt_pos,
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

FrogType flt_type = {
	{
		-1,		// FIXME type
		NULL		// refcnt
	}, 
	"float",		// type name
	NULL,			// FIXME getter
	NULL,			// setter
	flt_hash,		// hash
	NULL,			// size
	flt_print,		// prflt
	flt_to_str,		// tostr
	flt_to_int,		// toflt
	NULL,			// exec
	flt_scompare,		// compare
	flt_ccompare,		// complexe compare
	&flt_as_number,		// as number
	NULL,			// as sequence
	NULL,
	NULL,			// call	
	NULL			// free
};
