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
 * 9 : == != <= < >= > in
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
	13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, // assignation
	10, 11,			// && ||
	9,			// in
};

int operator_assign[] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // assignation
	0, 0, 0
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
	NULL, // ,
	NULL, // =
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
	FrogCall_IXor,
	FrogCall_BAnd,
	FrogCall_BOr,
	FrogCall_In
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

ast **create_args0(tokenizer *tkz, int pos)
{
	ast *cur = parse_value(tkz, 11);

	if(!cur)
	{
		return NULL;
	}

	ast **res = NULL;

	if(current_token(tkz)->type == TOKEN_COMMA)
	{
		consume_token(tkz);
		res = create_args0(tkz, pos + 1);
	}
	else
	{
		res = calloc(sizeof(FrogObject *), (pos + 2));
	}

	if(!res)
		return NULL;

	res[pos] = cur;
	return res;

}

ast **create_args(tokenizer *tkz, int close)
{
	tokeninfo *tkn = current_token(tkz);

	if(tkn->type == close)
	{
		consume_token(tkz);
		return calloc(sizeof(ast *), 1);
	}

	ast **res = create_args0(tkz, 0);

	if(!res) return NULL;

	if(!eat_token(tkz, close))
	{
		free(res);
		return NULL;
	}

	return res;
}

ast **create_bargs0(tokenizer *tkz, int pos)
{
	ast *cur = parse_value(tkz, 11);

	if(!cur)
	{
		return NULL;
	}

	ast *val = NULL;

	if(!eat_token(tkz, TOKEN_SEP) || !(val = parse_value(tkz, 11)))
	{
		return NULL;
	}

	ast **res = NULL;

	if(current_token(tkz)->type == TOKEN_COMMA)
	{
		consume_token(tkz);
		res = create_bargs0(tkz, pos + 1);
	}
	else
	{
		res = calloc(sizeof(FrogObject *), (pos + 1) * 2 + 1);
	}

	if(!res)
		return NULL;

	res[pos * 2] = cur;
	res[pos * 2 + 1] = val;

	return res;

}

ast **create_bargs(tokenizer *tkz, int close)
{
	tokeninfo *tkn = current_token(tkz);

	if(tkn->type == close)
	{
		consume_token(tkz);
		return calloc(sizeof(ast *), 1);
	}

	ast **res = create_bargs0(tkz, 0);

	if(!res) return NULL;

	if(!eat_token(tkz, close))
	{
		free(res);
		return NULL;
	}

	return res;
}

ast *create_call(int type, ast *a, ast *b)
{
	void *func = operator_function[type];

	if(b == NULL && type == TOKEN_OPTPLS)
		func = FrogCall_Pos;
	else if(b == NULL && type == TOKEN_OPTMIN)
		func = FrogCall_Neg;

	if(operator_assign[type])
	{
		return ast_assign(func, a, b);
	}
	else if(b == NULL)
	{
		return ast_op1(func, a);
	}

	return ast_op2(func, a, b);
}

ast *parse_single_value(tokenizer *tkz)
{
	tokeninfo *tkn = current_token(tkz);
	ast *base = NULL;

	if(tkn->type == TOKEN_INTEGER
		|| tkn->type == TOKEN_STRING
		|| tkn->type == TOKEN_DECIMAL)
	{
		base = ast_constant(tkn->value);
		consume_token(tkz);
	}
	else if(tkn->type == TOKEN_INF)
	{
		base = ast_constant(FromNativeFloat(INFINITY));
		consume_token(tkz);
	}
	else if(tkn->type == TOKEN_ID)
	{
		base = ast_var(tkn->value);
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
		base = ast_constant(FrogNone());
		consume_token(tkz);
	}
	else if(tkn->type == TOKEN_TRUE)
	{
		base = ast_constant(FrogTrue());
		consume_token(tkz);
	}
	else if(tkn->type == TOKEN_FALSE)
	{
		base = ast_constant(FrogFalse());
		consume_token(tkz);
	}
	else if(tkn->type == TOKEN_IF)
	{
		base = parse_if(tkz);
	}
	else if(tkn->type == TOKEN_OPEN)
	{
		base = NULL;
		consume_token(tkz);

		tkn = current_token(tkz);

		while(tkn->type >= 0 && tkn->type != TOKEN_CLOSE)
		{
			ast *nxt = parse_instruction(tkz);
			if(!nxt)
			{
				goto error;
			}

			if(!base)
			{
				base = nxt;
			}
			else
			{
				base = ast_create2(AST_NEXT, base, nxt);
			}

			tkn = current_token(tkz);
		}

		if(!base) base = ast_empty();

		consume_token(tkz);
		tkz->need_semicolon = 0;
		return base;
	}
	else if(tkn->type == TOKEN_WHILE)
	{
		base = parse_while(tkz);
	}
	else if(tkn->type == TOKEN_FOREACH)
	{
		base = parse_foreach(tkz);
	}
	else if(tkn->type == TOKEN_CONTINUE)
	{
		return parse_breaker(tkz, AST_CONTINUE);
	}
	else if(tkn->type == TOKEN_RETURN)
	{
		return parse_breaker(tkz, AST_RETURN);
	}
	else if(tkn->type == TOKEN_YIELD)
	{
		return parse_breaker(tkz, AST_YIELD);
	}
	else if(tkn->type == TOKEN_BREAK)
	{
		return parse_breaker(tkz, AST_BREAK);
	}
	else if(tkn->type == TOKEN_FUNCTION)
	{
		return parse_function(tkz);
	}
	else if(tkn->type == TOKEN_EOF)
	{
		tkz->need_semicolon = 0;
		return ast_empty();
	}
	else if(tkn->type == TOKEN_CLASS)
	{
		return parse_class(tkz);
	}
	else if(tkn->type == TOKEN_AOPEN)
	{
		consume_token(tkz);
		ast **args = create_args(tkz, TOKEN_ACLOSE);

		if(!args) return NULL;
		base = ast_create2(AST_CALL, ast_constant(function_create_list), args);
	}
	else if(tkn->type == TOKEN_DICT)
	{
		consume_token(tkz);
		ast **args = create_bargs(tkz, TOKEN_CLOSE);

		if(!args) return NULL;
		base = ast_create2(AST_CALL, ast_constant(function_create_map), args);
	}
	else if(tkn->type == TOKEN_SET)
	{
		consume_token(tkz);
		ast **args = create_args(tkz, TOKEN_CLOSE);

		if(!args) return NULL;
		base = ast_create2(AST_CALL, ast_constant(function_create_list), args);
	}

	if(!base)
	{
		goto error;
	}

	for( ; ; consume_token(tkz))
	{
		tkn = current_token(tkz);

		if(tkn->type == TOKEN_SUB)
		{
			consume_token(tkz);
			tkn = current_token(tkz);

			if(tkn->type != TOKEN_ID)
				return NULL;

			base = ast_create2(AST_SUBVAR, base, tkn->value);
		}
		else if(tkn->type == TOKEN_SOPEN)
		{
			consume_token(tkz);
			ast **args = create_args(tkz, TOKEN_SCLOSE);

			if(!args)
				return NULL;

			base = ast_create2(AST_CALL, base, args);
		}
		else if(tkn->type == TOKEN_AOPEN)
		{
			consume_token(tkz);
			ast *obj = parse_value(tkz, MAX_PRIORITY);

			if(!obj || !eat_token(tkz, TOKEN_ACLOSE))
				return NULL;

			base = ast_subarr(base, obj);
		}
		else
		{
			break;
		}
	}

	return base;	

error:	return NULL;
}

ast *recursive_tree(tokenizer *tkz, int priority)
{
	tokeninfo *tkn;
	ast *a, *b;
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

ast *parse_value(tokenizer *tkz, int priority)
{
	tokeninfo *tkn;
	ast *a, *b;
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

		if(priority == 12 && current_token(tkz)->type == TOKEN_COMMA)
		{
			consume_token(tkz);
			ast **res = create_args0(tkz, 1);
			if(!res) goto error;

			res[0] = a;
			return ast_create1(AST_TUPLE, res);
		}

		while(operator_priority(current_token(tkz)) == priority)
		{
			type = current_token(tkz)->type;
			consume_token(tkz);
			b = parse_value(tkz, priority - 1);

			if(!b) goto error;

			if(priority == 11)
				a = ast_create2(AST_OR, a, b);
			else if(priority == 10)
				a = ast_create2(AST_AND, a, b);
			else a = create_call(type, a, b);
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

ast *parse_breaker(tokenizer *tkz, int type)
{
	consume_token(tkz);
	ast *o = parse_instruction0(tkz, 0);
	if(!o) return NULL;

	return ast_create1(type, o);
}

ast *parse_if(tokenizer *tkz)
{
	if(!eat_token(tkz, TOKEN_IF) || !eat_token(tkz, TOKEN_SOPEN))
		return NULL;

	ast *condition = parse_instruction0(tkz, 2);

	if(!condition || !eat_token(tkz, TOKEN_SCLOSE))
		return NULL;

	ast *thenBlock = parse_instruction(tkz);

	if(!thenBlock)
		return NULL;

	ast *elseBlock = NULL;
	
	if(current_token(tkz)->type == TOKEN_ELSE)
	{
		consume_token(tkz);
		elseBlock = parse_instruction(tkz);

		if(!elseBlock)
			return NULL;
	}

	return ast_create3(AST_IF, condition, thenBlock, elseBlock);
}

ast *parse_while(tokenizer *tkz)
{
	if(!eat_token(tkz, TOKEN_WHILE) || !eat_token(tkz, TOKEN_SOPEN))
		return NULL;

	ast *condition = parse_instruction0(tkz, 2);

	if(!condition || !eat_token(tkz, TOKEN_SCLOSE))
		return NULL;

	ast *block = parse_instruction(tkz);

	if(!block)
		return NULL;

	return ast_create2(AST_WHILE, condition, block);
}

ast *parse_foreach(tokenizer *tkz)
{
	if(!eat_token(tkz, TOKEN_FOREACH) || !eat_token(tkz, TOKEN_SOPEN))
		return NULL;

	ast *var = parse_instruction0(tkz, 2);

	if(!var || !eat_token(tkz, TOKEN_SEP))
		return NULL;

	ast *iter = parse_instruction0(tkz, 2);

	if(!iter || !eat_token(tkz, TOKEN_SCLOSE))
		return NULL;

	ast *block = parse_instruction(tkz);

	if(!block)
		return NULL;

	return ast_create3(AST_FOREACH, var, iter, block);
}

FrogObject **arguments(tokenizer *tkz, int pos)
{
	tokeninfo *tkn = current_token(tkz);
	FrogObject *id = tkn->value;

	if(tkn->type != TOKEN_ID)
	{
		return NULL;
	}

	consume_token(tkz);
	FrogObject **res = NULL;

	if(current_token(tkz)->type == TOKEN_COMMA)
	{
		consume_token(tkz);
		res = arguments(tkz, pos + 1);
	}
	else
	{
		res = calloc(sizeof(FrogObject *), (pos + 2));
	}

	if(!res)
		return NULL;

	res[pos] = id;
	return res;
}

ast *parse_function(tokenizer *tkz)
{
	if(!eat_token(tkz, TOKEN_FUNCTION))
		return NULL;

	FrogObject *id = NULL;

	if(current_token(tkz)->type == TOKEN_ID)
	{
		id = current_token(tkz)->value;
		consume_token(tkz);
	}

	if(!eat_token(tkz, TOKEN_SOPEN))
		return NULL;

	FrogObject **args;

	if(current_token(tkz)->type == TOKEN_SCLOSE)
	{
		args = calloc(sizeof(FrogObject *), 1);
	}
	else args = arguments(tkz, 0);

	if(!args)
		return NULL;

	if(!eat_token(tkz, TOKEN_SCLOSE))
		return NULL;

	ast *block = parse_instruction(tkz);

	if(!block)
		return NULL;

	return ast_create3(AST_FUNC, id, args, block);
}

static ast **parse_functions(tokenizer *tkz, int pos)
{
	tokeninfo *tkn = current_token(tkz);

	if(tkn->type != TOKEN_FUNCTION)
	{
		return tkn->type != TOKEN_CLOSE ? NULL
				: calloc(sizeof(ast *), pos + 1);
	}

	ast *func = parse_function(tkz);
	if(!func) return NULL;

	ast **res = parse_functions(tkz, pos + 1);
	if(!res) return NULL;

	res[pos] = func;
	return res;
}

ast *parse_class(tokenizer *tkz)
{
	if(!eat_token(tkz, TOKEN_CLASS))
	{
		return NULL;
	}

	FrogObject *name = NULL, *parent = NULL;
	tokeninfo *tkn = current_token(tkz);

	if(tkn->type == TOKEN_ID)
	{
		name = tkn->value;
		consume_token(tkz);
		tkn = current_token(tkz);
	}

	if(tkn->type == TOKEN_EXTENDS)
	{
		consume_token(tkz);
		tkn = current_token(tkz);

		if(tkn->type != TOKEN_ID)
			return NULL;

		parent = tkn->value;
		consume_token(tkz);
	}

	if(!eat_token(tkz, TOKEN_OPEN))
		return NULL;

	ast **funcs = parse_functions(tkz, 0);

	if(!funcs || !eat_token(tkz, TOKEN_CLOSE))
		return NULL;

	return ast_create3(AST_CLASS, name, parent, funcs);
}

ast *parse_instruction0(tokenizer *tkz, int iend)
{
	ast *obj;
	int current = current_token(tkz)->type;

	switch(current)
	{
		case TOKEN_IEND:
			if(iend == 2)
			{
				token_error(tkz);
				return NULL;
			}
			tkz->need_semicolon = 0;

			consume_token(tkz);
			return ast_empty();
		default:
			tkz->need_semicolon = 1;
			obj = parse_value(tkz, MAX_PRIORITY);

			if(obj && iend == 1 && tkz->need_semicolon)
			{
				if(is_iend(tkz))
					consume_token(tkz);
			}

			tkz->need_semicolon = 0;
			return obj;
	}
}

ast *parse_instruction(tokenizer *tkz)
{
	return parse_instruction0(tkz, 1);
}
