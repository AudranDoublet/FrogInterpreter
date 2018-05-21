#include "../frog.h"

void Frog_Free(FrogObject *o)
{
	if(!o)
		return;

	if(ObType(o)->free)
	{
		(*ObType(o)->free)(o);
	}
	else return;

	free(o);
}

FrogObject *FrogCall_BAnd(FrogObject *a, FrogObject *b)
{
	return IsTrue(FrogCall_AsBool(a)) && IsTrue(FrogCall_AsBool(b)) ? FrogTrue() : FrogFalse();
}

FrogObject *FrogCall_BOr(FrogObject *a, FrogObject *b)
{
	return IsTrue(FrogCall_AsBool(a)) || IsTrue(FrogCall_AsBool(b)) ? FrogTrue() : FrogFalse();
}

FrogObject *Frog_ToString(FrogObject *a)
{
	if(ObType(a)->tostr)
	{
		return (*ObType(a)->tostr)(a);
	}

	FrogErr_Type("can't convert '%ls' to string", a);
	return NULL;
}

FrogObject *Frog_Len(FrogObject *a)
{
	if(ObType(a)->size)
	{
		return (*ObType(a)->size)(a);
	}

	FrogErr_Type("'%ls' has no len()", a);
	return NULL;
}

int Frog_Print(FrogObject *a, FILE *out)
{
	if(ObType(a)->print)
	{
		(*ObType(a)->print)(a, out);
	}
	else
	{
		a = Frog_ToString(a);

		if(a == NULL)
			return 0;
		else
			return Frog_Print(a, out);
	}

	return 1;
}

long Frog_Hash(FrogObject *a)
{
	if(ObType(a)->hash)
	{
		return (*ObType(a)->hash)(a);
	}

	return 0;
}

FrogObject *FrogCall_AsBool(FrogObject *o)
{
	int res = 1;

	if(ObType(o)->toint)
	{
		res = FIValue((*ObType(o)->toint)(o));
	}
	else if(IsNone(o))
	{
		res = 0;
	}
	else if(IsInstance(o))
	{
	}
	else if(ObType(o)->size)
	{
		FrogObject *ob = (*ObType(o)->size)(o);

		if(ob)
		{
			res = FIValue(ob);
		}
	}

	return res ? FrogTrue() : FrogFalse();
}

FrogObject *FrogCall_Not(FrogObject *o)
{
	return IsTrue(FrogCall_AsBool(o)) ? FrogFalse() : FrogTrue();
}

FrogObject *FrogCall_EQ(FrogObject *a, FrogObject *b)
{
	if(a == b) return FrogTrue();

	int res = -1;

	if(ObType(a)->simple_compare)
	{
		res = (*ObType(a)->simple_compare)(a, b);
	}

	if(res == 1)
		return FrogTrue();
	else if(res == 0)
		return FrogFalse();
	
	return NULL;
}

FrogObject *FrogCall_NE(FrogObject *a, FrogObject *b)
{
	int res = -1;

	if(ObType(a)->simple_compare)
	{
		res = (*ObType(a)->simple_compare)(a, b);
	}

	if(res == 0)
		return FrogTrue();
	else if(res == 1)
		return FrogFalse();

	return NULL;
}

int FrogCall_Compare(FrogObject *a, FrogObject *b)
{
	int res = -2;

	if(ObType(a)->complex_compare)
	{
		res = (*ObType(a)->complex_compare)(a, b);
	}

	if(res == -2)
	{
		FrogErr_Operator(a, b, "compare");
		return -2;
	}

	return res;
}

FrogObject *FrogCall_LO(FrogObject *a, FrogObject *b)
{
	int res = -2;

	if(ObType(a)->complex_compare)
	{
		res = (*ObType(a)->complex_compare)(a, b);
	}

	if(res == -2)
	{
		FrogErr_Operator(a, b, "<");
		return NULL;
	}

	return res == -1 ? FrogTrue() : FrogFalse();
}

FrogObject *FrogCall_GT(FrogObject *a, FrogObject *b)
{
	int res = -2;

	if(ObType(a)->complex_compare)
	{
		res = (*ObType(a)->complex_compare)(a, b);
	}

	if(res == -2)
	{
		FrogErr_Operator(a, b, ">");
		return NULL;
	}

	return res == 1 ? FrogTrue() : FrogFalse();
}

FrogObject *FrogCall_LE(FrogObject *a, FrogObject *b)
{
	int res = -2;

	if(ObType(a)->complex_compare)
	{
		res = (*ObType(a)->complex_compare)(a, b);
	}

	if(res == -2)
	{
		FrogErr_Operator(a, b, "<=");
		return NULL;
	}

	return res <= 0 ? FrogTrue() : FrogFalse();
}

FrogObject *FrogCall_GE(FrogObject *a, FrogObject *b)
{
	int res = -2;

	if(ObType(a)->complex_compare)
	{
		res = (*ObType(a)->complex_compare)(a, b);
	}

	if(res == -2)
	{
		FrogErr_Operator(a, b, ">=");
		return NULL;
	}

	return res >= 0 ? FrogTrue() : FrogFalse();
}

FrogObject *FrogCall_Exec(FrogObject *env, FrogObject *a)
{
	FrogType *type = ObType(a);

	if(type->exec)
		return (*type->exec)(env, a);
	return a;
}

FrogObject *FrogCall_Add(FrogObject *a, FrogObject *b)
{
	FrogType *type = ObType(a);

	if(type->as_number && type->as_number->add)
	{
		FrogObject *res = (*type->as_number->add)(a, b);
		if(res) return res;
	}

	FrogErr_Operator(a, b, "+");
	return NULL;
}

FrogObject *FrogCall_Sub(FrogObject *a, FrogObject *b)
{
	FrogType *type = ObType(a);

	if(type->as_number && type->as_number->sub)
	{
		FrogObject *res = (*type->as_number->sub)(a, b);
		if(res) return res;
	}

	FrogErr_Operator(a, b, "-");
	return NULL;
}

FrogObject *FrogCall_Pos(FrogObject *a)
{
	FrogType *type = ObType(a);

	if(type->as_number && type->as_number->pos)
	{
		FrogObject *res = (*type->as_number->pos)(a);
		if(res) return res;
	}

	FrogErr_Operator(a, NULL, "+");
	return NULL;
}

FrogObject *FrogCall_Neg(FrogObject *a)
{
	FrogType *type = ObType(a);

	if(type->as_number && type->as_number->neg)
	{
		FrogObject *res = (*type->as_number->neg)(a);
		if(res) return res;
	}

	FrogErr_Operator(a, NULL, "-");
	return NULL;
}

FrogObject *FrogCall_Mul(FrogObject *a, FrogObject *b)
{
	FrogType *type = ObType(a);

	if(type->as_number && type->as_number->mul)
	{
		FrogObject *res = (*type->as_number->mul)(a, b);
		if(res) return res;
	}

	FrogErr_Operator(a, b, "*");
	return NULL;
}

FrogObject *FrogCall_Div(FrogObject *a, FrogObject *b)
{
	FrogType *type = ObType(a);

	if(type->as_number && type->as_number->div)
	{
		FrogObject *res = (*type->as_number->div)(a, b);
		if(res) return res;
	}

	FrogErr_Operator(a, b, "/");
	return NULL;
}

FrogObject *FrogCall_DivF(FrogObject *a, FrogObject *b)
{
	FrogType *type = ObType(a);

	if(type->as_number && type->as_number->divfloor)
	{
		FrogObject *res = (*type->as_number->divfloor)(a, b);
		if(res) return res;
	}

	FrogErr_Operator(a, b, "//");
	return NULL;
}

FrogObject *FrogCall_Mod(FrogObject *a, FrogObject *b)
{
	FrogType *type = ObType(a);

	if(type->as_number && type->as_number->mod)
	{
		FrogObject *res = (*type->as_number->mod)(a, b);
		if(res) return res;
	}

	FrogErr_Operator(a, b, "%");
	return NULL;
}

FrogObject *FrogCall_Pow(FrogObject *a, FrogObject *b)
{
	FrogType *type = ObType(a);

	if(type->as_number && type->as_number->pow)
	{
		FrogObject *res = (*type->as_number->pow)(a, b);
		if(res) return res;
	}

	FrogErr_Operator(a, b, "**");
	return NULL;
}

FrogObject *FrogCall_Inv(FrogObject *a)
{
	FrogType *type = ObType(a);

	if(type->as_number && type->as_number->inv)
	{
		FrogObject *res = (*type->as_number->inv)(a);
		if(res) return res;
	}

	FrogErr_Operator(a, NULL, "~");
	return NULL;
}

FrogObject *FrogCall_LShift(FrogObject *a, FrogObject *b)
{
	FrogType *type = ObType(a);

	if(type->as_number && type->as_number->lshift)
	{
		FrogObject *res = (*type->as_number->lshift)(a, b);
		if(res) return res;
	}

	FrogErr_Operator(a, b, "<<");
	return NULL;
}

FrogObject *FrogCall_RShift(FrogObject *a, FrogObject *b)
{
	FrogType *type = ObType(a);

	if(type->as_number && type->as_number->rshift)
	{
		FrogObject *res = (*type->as_number->rshift)(a, b);
		if(res) return res;
	}

	FrogErr_Operator(a, b, ">>");
	return NULL;
}

FrogObject *FrogCall_And(FrogObject *a, FrogObject *b)
{
	FrogType *type = ObType(a);

	if(type->as_number && type->as_number->and)
	{
		FrogObject *res = (*type->as_number->and)(a, b);
		if(res) return res;
	}

	FrogErr_Operator(a, b, "&");
	return NULL;
}

FrogObject *FrogCall_Or(FrogObject *a, FrogObject *b)
{
	FrogType *type = ObType(a);

	if(type->as_number && type->as_number->or)
	{
		FrogObject *res = (*type->as_number->or)(a, b);
		if(res) return res;
	}

	FrogErr_Operator(a, b, "|");
	return NULL;
}

FrogObject *FrogCall_Xor(FrogObject *a, FrogObject *b)
{
	FrogType *type = ObType(a);

	if(type->as_number && type->as_number->xor)
	{
		FrogObject *res = (*type->as_number->xor)(a, b);
		if(res) return res;
	}

	FrogErr_Operator(a, b, "^");
	return NULL;
}

FrogObject *FrogCall_IAdd(FrogObject *a, FrogObject *b)
{
	FrogType *type = ObType(a);

	if(type->as_number && type->as_number->inplace_add)
	{
		FrogObject *res = (*type->as_number->inplace_add)(a, b);
		if(res) return res;
	}

	return FrogCall_Add(a, b);
}

FrogObject *FrogCall_ISub(FrogObject *a, FrogObject *b)
{
	FrogType *type = ObType(a);

	if(type->as_number && type->as_number->inplace_sub)
	{
		FrogObject *res = (*type->as_number->inplace_sub)(a, b);
		if(res) return res;
	}

	return FrogCall_Sub(a, b);
}

FrogObject *FrogCall_IMul(FrogObject *a, FrogObject *b)
{
	FrogType *type = ObType(a);

	if(type->as_number && type->as_number->inplace_mul)
	{
		FrogObject *res = (*type->as_number->inplace_mul)(a, b);
		if(res) return res;
	}

	return FrogCall_Mul(a, b);
}

FrogObject *FrogCall_IDiv(FrogObject *a, FrogObject *b)
{
	FrogType *type = ObType(a);

	if(type->as_number && type->as_number->inplace_div)
	{
		FrogObject *res = (*type->as_number->inplace_div)(a, b);
		if(res) return res;
	}

	return FrogCall_Div(a, b);
}

FrogObject *FrogCall_IDivF(FrogObject *a, FrogObject *b)
{
	FrogType *type = ObType(a);

	if(type->as_number && type->as_number->inplace_divfloor)
	{
		FrogObject *res = (*type->as_number->inplace_divfloor)(a, b);
		if(res) return res;
	}

	return FrogCall_DivF(a, b);
}

FrogObject *FrogCall_IMod(FrogObject *a, FrogObject *b)
{
	FrogType *type = ObType(a);

	if(type->as_number && type->as_number->inplace_mod)
	{
		FrogObject *res = (*type->as_number->inplace_mod)(a, b);
		if(res) return res;
	}

	return FrogCall_Mod(a, b);
}

FrogObject *FrogCall_IPow(FrogObject *a, FrogObject *b)
{
	FrogType *type = ObType(a);

	if(type->as_number && type->as_number->inplace_pow)
	{
		FrogObject *res = (*type->as_number->inplace_pow)(a, b);
		if(res) return res;
	}

	return FrogCall_Pow(a, b);
}

FrogObject *FrogCall_ILShift(FrogObject *a, FrogObject *b)
{
	FrogType *type = ObType(a);

	if(type->as_number && type->as_number->inplace_lshift)
	{
		FrogObject *res = (*type->as_number->inplace_lshift)(a, b);
		if(res) return res;
	}

	return FrogCall_LShift(a, b);
}

FrogObject *FrogCall_IRShift(FrogObject *a, FrogObject *b)
{
	FrogType *type = ObType(a);

	if(type->as_number && type->as_number->inplace_rshift)
	{
		FrogObject *res = (*type->as_number->inplace_rshift)(a, b);
		if(res) return res;
	}

	return FrogCall_RShift(a, b);
}

FrogObject *FrogCall_IAnd(FrogObject *a, FrogObject *b)
{
	FrogType *type = ObType(a);

	if(type->as_number && type->as_number->inplace_and)
	{
		FrogObject *res = (*type->as_number->inplace_and)(a, b);
		if(res) return res;
	}

	return FrogCall_And(a, b);
}

FrogObject *FrogCall_IOr(FrogObject *a, FrogObject *b)
{
	FrogType *type = ObType(a);

	if(type->as_number && type->as_number->inplace_or)
	{
		FrogObject *res = (*type->as_number->inplace_or)(a, b);
		if(res) return res;
	}

	return FrogCall_Or(a, b);
}

FrogObject *FrogCall_IXor(FrogObject *a, FrogObject *b)
{
	FrogType *type = ObType(a);

	if(type->as_number && type->as_number->inplace_xor)
	{
		FrogObject *res = (*type->as_number->inplace_xor)(a, b);
		if(res) return res;
	}

	return FrogCall_Xor(a, b);
}

FrogObject *FrogCall_Get(FrogObject *a, FrogObject *b)
{
	FrogType *type = ObType(a);
	FrogObject *res;

	if(!type->getter || !(res = (*type->getter)(a, b)))
	{
		FrogErr_Attribute(a, "get");
		return NULL;
	}

	return res;
}

FrogObject *FrogCall_Set(FrogObject *a, FrogObject *b, FrogObject *c, binaryfunction f)
{
	FrogType *type = ObType(a);
	FrogObject *res;

	if(!type->setter || !(res = (*type->setter)(a, b, c, f)))
	{
		FrogErr_Attribute(a, "set");
		return NULL;
	}

	return res;
}

FrogObject *FrogCall_SeqGet(FrogObject *a, FrogObject *b)
{
	FrogAsSequence *type = ObType(a)->as_sequence;

	if(!type || !type->get)
	{
		FrogErr_Attribute(a, "get (sequence)");
		return NULL;
	}

	return (*type->get)(a, b);
}

FrogObject *FrogCall_SeqSet(FrogObject *a, FrogObject *b, FrogObject *c, binaryfunction f)
{
	FrogAsSequence *type = ObType(a)->as_sequence;

	if(!type || !type->set)
	{
		FrogErr_Attribute(a, "set (sequence)");
		return NULL;
	}

	return (*type->set)(a, b, c, f);
}

FrogObject *FrogCall_Call(FrogObject *func, FrogObject **args, size_t len, stack *st)
{
	callfunction call = ObType(func)->call;

	if(!call)
	{
		FrogErr_Attribute(func, "call");
		return NULL;
	}

	return call(func, args, len, st);
}

FrogObject *FrogCall_IterInit(FrogObject *a)
{
	if(!FrogIsIter(a))
	{
		if(!ObType(a)->as_iterable)
		{
			FrogErr_Attribute(a, "iter");
			return NULL;
		}

		a = CreateIterable(a);
		if(!a) return NULL;
	}

	FrogIter *iter = (FrogIter *) a;
	return (iter->iterable->init)(a);
}

FrogObject *FrogCall_IterHasNext(FrogObject *a)
{
	FrogIter *iter = (FrogIter *) a;
	return (iter->iterable->hasnext)(a);
}

FrogObject *FrogCall_IterNext(FrogObject *a)
{
	FrogIter *iter = (FrogIter *) a;
	return (iter->iterable->next)(a);
}
