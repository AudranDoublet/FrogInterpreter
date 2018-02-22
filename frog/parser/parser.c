#include "../frog.h"

int prioritytype[] = {
	0, // value
	1, // {OP}value
	2, // value{OP}value, left to right
	3 // v
};

/**
 * -1: error
 * 0 : value
 * 1 : **
 * 2 : + - ~ !
 * 3 : * / // %
 * 4 : + -
 * 5 : << >>
 * 6 : &
 * 7 : ^
 * 8 : |
 * 9 : == != <= < >= >
 * 10: &&
 * 11: ||
 * 12: ,
 * 13: += è= *= /= //= %= **= <<= >>= |= &= ^=
 */
int priorities[] =
{
	0,			// EOF
	9, 9, 9, 9, 9, 9, 	// == != < > <= >=
	4, 4,			// + -
	3, 3, 3, 3,		// * / // %
	1,			// **
	2,			// !
	5, 5,			// << >>
	8,			// |
	6,			// &
	7,			// ^
	2,			// ~
	12,			// ,
	13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13 // assignation
};

int operator_assign[] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 // assignation
};

void *operator_function[] =
{
	NULL,
	FrogCall_EQ,
	FrogCall_NE,
	FrogCall_LO,
	FrogCall_GT,
	FrogCall_LE,
	FrogCall_GE,
	FrogCall_Add,
	FrogCall_Sub,
	FrogCall_Mul,
	FrogCall_Div,
	FrogCall_DivF,
	FrogCall_Mod,
	FrogCall_Pow,
	FrogCall_Not,
	FrogCall_LShift,
	FrogCall_RShift,
	FrogCall_Or,
	FrogCall_And,
	FrogCall_Xor,
	FrogCall_Inv,
	NULL,
	NULL,
	FrogCall_IAdd,
	FrogCall_ISub,
	FrogCall_IMul,
	FrogCall_IDiv,
	FrogCall_IDivF,
	FrogCall_IMod,
	FrogCall_IPow,
	FrogCall_ILShift,
	FrogCall_IRShift,
	FrogCall_IOr,
	FrogCall_IAnd,
	FrogCall_IXor
};

/* 0: value
   1: {OP}value
   2: value{OP}value (left to right)
   3: value{OP}value (right to left)
*/
int priority_type[] =
{
	0,	// type 0
	2,	// type 1
	1,	// type 2
	2,	// type 3
	2,	// type 4
	2,	// type 5
	2,	// type 6
	2,	// type 7
	2,	// type 8
	2,	// type 9
	2,	// type 10
	2,	// type 11
	2,	// type 12 FIXME
	3	// type 13
};

/**
 * return the operator priority corresponding to the token; if not an operator, return 0
 */
static inline int operator_priority(tokeninfo *tkn)
{
	return tkn->type >= OPERATORS_END ? -1 : priorities[tkn->type];
}

static inline int operator_type(int priority)
{
	return priority_type[priority];
}

FrogObject *create_call(int type, FrogObject *a, FrogObject *b)
{
	void *func = operator_function[type];

	if(b == NULL && type == TOKEN_OPTPLS)
		func = FrogCall_Pos;
	else if(b == NULL && type == TOKEN_OPTMIN)
		func = FrogCall_Neg;

	return CreateCaller(operator_assign[type], func, a, b, NULL);
}

FrogObject *parse_single_value(tokenizer *tkz)
{
	tokeninfo *tkn = current_token(tkz);
	FrogObject *base = NULL;

	if(tkn->type == TOKEN_INTEGER
		|| tkn->type == TOKEN_STRING)
	{
		base = tkn->value;
		consume_token(tkz);
	}
	else if(tkn->type == TOKEN_ID)
	{
		base = CreateHybrid(NULL, tkn->value, 1);
		consume_token(tkz);
	}
	else if(tkn->type == TOKEN_SOPEN)
	{
		consume_token(tkz);
		base = parse_value(tkz, MAX_PRIORITY);

		if(!base || !eat_token(tkz, TOKEN_SCLOSE))
			goto error;
	}
	else if(tkn->type == TOKEN_NONE)
	{
		base = FrogNone();
		consume_token(tkz);
	}
	else if(tkn->type == TOKEN_TRUE)
	{
		base = FrogTrue();
		consume_token(tkz);
	}
	else if(tkn->type == TOKEN_FALSE)
	{
		base = FrogFalse();
		consume_token(tkz);
	}
	else if(tkn->type == TOKEN_IF)
	{
		base = parse_if(tkz);
	}
	else if(tkn->type == TOKEN_OPEN)
	{
		base = CreateBlock();
		consume_token(tkz);

		if(!base) goto error;
		tkn = current_token(tkz);

		while(tkn->type >= 0 && tkn->type != TOKEN_CLOSE)
		{
			FrogObject *nxt = parse_instruction(tkz);
			if(!nxt)
			{
				Frog_Free(base);
				goto error;
			}

			AddBlockElement(base, nxt);
			tkn = current_token(tkz);
		}
	}
	else if(tkn->type == TOKEN_WHILE)
	{
		base = parse_while(tkz);
	}
	else if(tkn->type == TOKEN_CONTINUE)
	{
		return parse_breaker(tkz, BREAK_CONTINUE);
	}
	else if(tkn->type == TOKEN_BREAK)
	{
		return parse_breaker(tkz, BREAK_BREAK);
	}
	//FIXME liste, map

	if(!base)
	{
		goto error;
	}

	for( ; ; consume_token(tkz))
	{
		tkn = current_token(tkz);

		if(tkn->type == TOKEN_SUB)
		{

		}
		else if(tkn->type == TOKEN_SOPEN)
		{

		}
		else if(tkn->type == TOKEN_AOPEN)
		{
			consume_token(tkz);
			FrogObject *obj = parse_value(tkz, MAX_PRIORITY);

			if(!obj || !eat_token(tkz, TOKEN_ACLOSE))
				return NULL;

			base = CreateHybrid(base, obj, 2);
		}
		else
		{
			break;
		}
	}

	return base;	

error:	return NULL;
}

FrogObject *recursive_tree(tokenizer *tkz, int priority)
{
	tokeninfo *tkn;
	FrogObject *a, *b;
	int type;

	a = parse_value(tkz, priority - 1);
	if(!a) goto error;

	tkn = current_token(tkz);
	type = tkn->type;

	if(operator_priority(tkn) != priority)
		return a;

	consume_token(tkz);
	b = recursive_tree(tkz, priority);
	if(!b) goto error;

	return create_call(type, a, b);
error:
	return NULL;
}

FrogObject *parse_value(tokenizer *tkz, int priority)
{
	tokeninfo *tkn;
	FrogObject *a, *b;
	int type;
	int ptype = operator_type(priority);
	int p;

	if(ptype == 0)
	{
		return parse_single_value(tkz);
	}
	else if(ptype == 1)
	{
		tkn = current_token(tkz);
		type = tkn->type;
		p = operator_priority(tkn);

		if(p == priority || p == 4) // p == 4 : + -
		{
			consume_token(tkz);
			a = parse_value(tkz, priority - 1);

			if(!a) goto error;

			return create_call(type, a, NULL);
		}
		else
		{
			return parse_value(tkz, priority - 1);
		}
	}
	else if(ptype == 2)
	{
		a = parse_value(tkz, priority - 1);
		if(!a) goto error;

		while(operator_priority(current_token(tkz)) == priority)
		{
			type = current_token(tkz)->type;
			consume_token(tkz);
			b = parse_value(tkz, priority - 1);

			if(!b) goto error;
			a = create_call(type, a, b);
		}

		return a;
	}
	else
	{
		return recursive_tree(tkz, priority);
	}
error:
	return NULL;
}

void token_error(tokenizer *tkz)
{
	UNUSED(tkz); //FIXME
	FrogErr_Post("SyntaxError", "FIXME");
}

FrogObject *parse_breaker(tokenizer *tkz, int type)
{
	consume_token(tkz);
	FrogObject *o = parse_instruction0(tkz, 0);
	if(!o) return NULL;
	
	return CreateBreaker(type, o);
}

FrogObject *parse_if(tokenizer *tkz)
{
	if(!eat_token(tkz, TOKEN_IF) || !eat_token(tkz, TOKEN_SOPEN))
		return NULL;

	FrogObject *condition = parse_instruction0(tkz, 2);

	if(!condition || !eat_token(tkz, TOKEN_SCLOSE))
		return NULL;

	FrogObject *thenBlock = parse_instruction(tkz);

	if(!thenBlock)
		return NULL;

	FrogObject *elseBlock = NULL;
	
	if(current_token(tkz)->type == TOKEN_ELSE)
	{
		consume_token(tkz);
		elseBlock = parse_instruction(tkz);

		if(!elseBlock)
			return NULL;
	}

	return CreateCondition(condition, thenBlock, elseBlock);
}

FrogObject *parse_while(tokenizer *tkz)
{
	if(!eat_token(tkz, TOKEN_WHILE) || !eat_token(tkz, TOKEN_SOPEN))
		return NULL;

	FrogObject *condition = parse_instruction0(tkz, 2);

	if(!condition || !eat_token(tkz, TOKEN_SCLOSE))
		return NULL;

	FrogObject *block = parse_instruction(tkz);

	if(!block)
		return NULL;

	return CreateWhile(0, condition, block);
}

FrogObject *parse_instruction0(tokenizer *tkz, int iend)
{
	FrogObject *obj;
	int current = current_token(tkz)->type;

	switch(current)
	{
		case TOKEN_IEND:
			if(iend == 2)
			{
				token_error(tkz);
				return NULL;
			}

			consume_token(tkz);
			return FrogNone();
		default:
			obj = parse_value(tkz, MAX_PRIORITY); //FIXME condition, ect

			if(iend == 1)
			{
				if(is_iend(tkz))
					consume_token(tkz);
			}

			return obj;
	}
}

FrogObject *parse_instruction(tokenizer *tkz)
{
	return parse_instruction0(tkz, 1);
}
