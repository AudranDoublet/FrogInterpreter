#include "../frog.h"

FrogType int_type;

FrogObject *FromNativeInteger(long i)
{
	FrogInt *result = malloc(sizeof(FrogInt));

	if(!result)
	{
		FrogErr_Memory();
		return NULL;
	}

	result->value = i;
	result->ob_base.type = &int_type;
	result->ob_base.refcnt = 0;

	return (FrogObject *) result;
}

void int_print(FrogObject *obj, FILE *f)
{
	fprintf(f, "%li", FIValue(obj));
}

FrogObject *int_to_str(FrogObject *obj)
{
	struct strbuilder *builder = create_strbuilder();
	long value = FIValue(obj);

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

FrogObject *int_to_int(FrogObject *obj)
{
	return obj;
}

long int_hash(FrogObject *obj)
{
	return FIValue(obj);
}

int int_scompare(FrogObject *a, FrogObject *b)
{
	if(!ObType(b)->toint)
	{
		return 0;
	}

	b = (*ObType(b)->toint)(b);
	return FIValue(a) == FIValue(b);
}

int int_ccompare(FrogObject *a, FrogObject *b)
{
	long vala = FIValue(a);

	if( !ObType(b)->toint )
	{
		return -2;
	}
	else
	{
		b = (*ObType(b)->toint)(b);
	}

	long valb = FIValue(b);
	return (vala < valb) ? -1 : (vala > valb);
}

FrogObject *int_add(FrogObject *a, FrogObject *b)
{
	if(ObType(b)->toint)
	{
		b = (*ObType(b)->toint)(b);
	}
	else
	{
		return NULL;
	}

	return FromNativeInteger(FIValue(a) + FIValue(b));
}

FrogObject *int_sub(FrogObject *a, FrogObject *b)
{
	if(ObType(b)->toint)
	{
		b = (*ObType(b)->toint)(b);
	}
	else
	{
		return NULL;
	}

	return FromNativeInteger(FIValue(a) - FIValue(b));
}

FrogObject *int_mul(FrogObject *a, FrogObject *b)
{
	if(ObType(b)->toint)
	{
		b = (*ObType(b)->toint)(b);
	}
	else
	{
		return NULL;
	}

	//fixme 

	return FromNativeInteger(FIValue(a) * FIValue(b));
}

FrogObject *int_div(FrogObject *a, FrogObject *b)
{
	return b ? b : a; //FIXME return a floating number :D
}

FrogObject *int_divfloor(FrogObject *a, FrogObject *b)
{
	if(ObType(b)->toint)
	{
		b = (*ObType(b)->toint)(b);
	}
	else
	{
		FrogErr_Operator(a, b, "/");
		return NULL;
	}

	if(FIValue(b) == 0)
	{
		FrogErr_Div0();
		return NULL;
	}

	return FromNativeInteger(FIValue(a) / FIValue(b));
}

FrogObject *int_mod(FrogObject *a, FrogObject *b)
{
	if(ObType(b)->toint)
	{
		b = (*ObType(b)->toint)(b);
	}
	else
	{
		FrogErr_Operator(a, b, "%");
		return NULL;
	}

	if(FIValue(b) == 0)
	{
		FrogErr_Div0();
		return NULL;
	}

	return FromNativeInteger(FIValue(a) % FIValue(b));
}

FrogObject *int_pow(FrogObject *a, FrogObject *b)
{
	if(ObType(b)->toint)
	{
		b = (*ObType(b)->toint)(b);
	}
	else
	{
		return NULL;
	}

	long va = FIValue(a), vb = FIValue(b);

	if(vb < 0)
		return NULL; //FIXME float
	if(vb == 0)
		return FromNativeInteger(1);

	long res = va;

	while(vb > 1)
	{
		res *= res;
		if((vb & 1) == 1)
			res *= va;
		vb >>= 1;
	}

	return FromNativeInteger(res);
}

static inline long int_abs(long v)
{
	return v < 0 ? -v : v;
}

FrogObject *int_neg(FrogObject *a)
{
	return FromNativeInteger(-FIValue(a));
}

FrogObject *int_pos(FrogObject *a)
{
	return FromNativeInteger(FIValue(a));
}

FrogObject *int_inv(FrogObject *a)
{
	return FromNativeInteger(~FIValue(a));
}

FrogObject *int_lshift(FrogObject *a, FrogObject *b)
{
	if(ObType(b)->toint)
	{
		b = (*ObType(b)->toint)(b);
	}
	else
	{
		FrogErr_Operator(a, b, "<<");
		return NULL;
	}

	if(FIValue(b) < 0)
	{
		FrogErr_Value("negative shift count");
	}

	return FromNativeInteger(FIValue(a) << FIValue(b));
}

FrogObject *int_rshift(FrogObject *a, FrogObject *b)
{
	if(ObType(b)->toint)
	{
		b = (*ObType(b)->toint)(b);
	}
	else
	{
		FrogErr_Operator(a, b, ">>");
		return NULL;
	}

	if(FIValue(b) < 0)
	{
		FrogErr_Value("negative shift count");
	}

	return FromNativeInteger(FIValue(a) >> FIValue(b));
}

FrogObject *int_and(FrogObject *a, FrogObject *b)
{
	if(ObType(b)->toint)
	{
		b = (*ObType(b)->toint)(b);
	}
	else
	{
		FrogErr_Operator(a, b, "&");
		return NULL;
	}

	return FromNativeInteger(FIValue(a) & FIValue(b));
}

FrogObject *int_or(FrogObject *a, FrogObject *b)
{
	if(ObType(b)->toint)
	{
		b = (*ObType(b)->toint)(b);
	}
	else
	{
		FrogErr_Operator(a, b, "|");
		return NULL;
	}

	return FromNativeInteger(FIValue(a) | FIValue(b));
}

FrogObject *int_xor(FrogObject *a, FrogObject *b)
{
	if(ObType(b)->toint)
	{
		b = (*ObType(b)->toint)(b);
	}
	else
	{
		FrogErr_Operator(a, b, "^");
		return NULL;
	}

	return FromNativeInteger(FIValue(a) ^ FIValue(b));
}

int FrogIsInt(FrogObject *o)
{
	return o && o->type == &int_type;
}

FrogAsNumber int_as_number = {
	int_add,
	int_sub,
	int_mul,
	int_div,
	int_divfloor,
	int_mod,
	int_pow,
	int_neg,
	int_pos,
	int_pos,
	int_inv,
	int_lshift,
	int_rshift,
	int_and,
	int_or,
	int_xor,
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

FrogType int_type = {
	{
		-1,		// FIXME type
		NULL		// refcnt
	}, 
	"int",			// type name
	NULL,			// FIXME getter
	NULL,			// setter
	int_hash,		// hash
	NULL,			// size
	int_print,		// print
	int_to_str,		// tostr
	int_to_int,		// toint
	NULL,			// exec
	int_scompare,		// compare
	int_ccompare,		// complexe compare
	&int_as_number,		// as number
	NULL,			// as sequence
	NULL,			// call	
	NULL			// free
};
