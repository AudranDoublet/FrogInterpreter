#include "../frog.h"

FrogType block_type, cond_type, while_type, breaker_type;

FrogObject *CreateBlock(void);

FrogObject *CreateWhile(int dowhile, FrogObject *condition, FrogObject *block);

int AddBlockElement(FrogObject *block, FrogObject *v)
{
	FrogBlock *b = (FrogBlock *) block;
	struct BlockElement *el = malloc(sizeof(struct BlockElement));

	if(!el)
	{
		FrogErr_Memory();
		return 0;
	}

	Ref(v);
	el->val = v;
	el->next = NULL;

	if(b->last)
	{
		b->last->next = el;
		b->last = el;
	}
	else
	{
		b->first = el;
		b->last = el;
	}

	return 1;
}

FrogObject *CreateBlock(void)
{
	FrogBlock *b = malloc(sizeof(FrogBlock));

	if(!b) return NULL;

	ObType(b) = &block_type;
	b->first = NULL;
	b->last = NULL;

	return (FrogObject *) b;
}

void free_be(struct BlockElement *e)
{
	if(!e) return;
	Unref(e->val);
	free_be(e->next);
	free(e);
}

void free_block(FrogObject *o)
{
	FrogBlock *b = (FrogBlock *) o;
	free_be(b->first);	
}

FrogObject *block_exec(FrogObject *env, FrogObject *o)
{
	FrogBlock *b = (FrogBlock *) o;
	FrogObject *v = FrogNone();

	for( struct BlockElement *e = b->first; e; e = e->next )
	{
		v = FrogCall_Exec(env, e->val);
		if(!v) return NULL;
	}

	return v;
}

FrogObject *CreateCondition(FrogObject *condition, FrogObject *thenBlock, FrogObject *elseBlock)
{
	FrogCondition *res = malloc(sizeof(FrogCondition));

	if(!res)
	{
		FrogErr_Memory();
		return NULL;
	}

	ObType(res) = &cond_type;
	res->condition = condition;
	res->thenBlock = thenBlock;
	res->elseBlock = elseBlock;

	return (FrogObject *) res;
}

FrogObject *condition_exec(FrogObject *env, FrogObject *o)
{
	FrogCondition *cond = (FrogCondition *) o;
	FrogObject *res = FrogCall_Exec(env, cond->condition);

	if(!res) return NULL;

	if( IsTrue(FrogCall_AsBool(res)) )
	{
		return FrogCall_Exec(env, cond->thenBlock);
	}
	else if(cond->elseBlock)
	{
		return FrogCall_Exec(env, cond->elseBlock);
	}
	else
	{
		return FrogNone();
	}
}

void free_cond(FrogObject *o)
{
	FrogCondition *cond = (FrogCondition *) o;

	Unref(cond->condition);
	Unref(cond->thenBlock);
	Unref(cond->elseBlock);
}

FrogObject *CreateWhile(int dowhile, FrogObject *condition, FrogObject *block)
{
	FrogWhile *res = malloc(sizeof(FrogWhile));

	if(!res)
	{
		FrogErr_Memory();
		return NULL;
	}

	ObType(res) = &while_type;
	res->condition = condition;
	res->block = block;
	res->dowhile = dowhile;

	return (FrogObject *) res;
}

int condition(FrogObject *env, FrogWhile *w)
{
	FrogObject *res = FrogCall_Exec(env, w->condition);

	if(!res) return -1;
	return IsTrue(FrogCall_AsBool(res));
}

FrogObject *while_exec(FrogObject *env, FrogObject *o)
{
	FrogEnv *e = (FrogEnv *) env;
	FrogWhile *w = (FrogWhile *) o;
	FrogObject *res = FrogNone();

	int condr = 0;

	if(w->dowhile) goto core;

cond:	if( (condr = condition(env, w)) == 0) goto end;
	else if(condr == 2) goto error;

core:	res = FrogCall_Exec(env, w->block);
	if(!res)
	{
		if(e->breaktype == BREAK_BREAK)
		{
			e->breaktype = BREAK_NONE;
			res = e->breakvalue;
			goto end;
		}
		else if(e->breaktype == BREAK_CONTINUE)
		{
			e->breaktype = BREAK_NONE;
			res = e->breakvalue;
		}
		else goto error;
	}

	goto cond;

end:	return res;

error:	return NULL;
}

void free_while(FrogObject *o)
{
	FrogWhile *cond = (FrogWhile *) o;

	Unref(cond->condition);
	Unref(cond->block);
}

FrogObject *CreateBreaker(int type, FrogObject *value)
{
	FrogBreaker *res = malloc(sizeof(FrogBreaker));

	if(!res)
	{
		FrogErr_Memory();
		return NULL;
	}

	ObType(res) = &breaker_type;
	res->type = type;
	res->value = value;

	return (FrogObject *) res;
}

FrogObject *breaker_exec(FrogObject *env, FrogObject *o)
{
	FrogEnv *e = (FrogEnv *) env;
	FrogBreaker *w = (FrogBreaker *) o;
	FrogObject *res = FrogNone();

	if(w->value)
	{
		res = FrogCall_Exec(env, w->value);
		if(!res) return NULL;
	}

	e->breaktype = w->type;
	e->breakvalue = res;

	return NULL;
}

void free_breaker(FrogObject *o)
{
	FrogBreaker *cond = (FrogBreaker *) o;
	Unref(cond->value);
}

FrogType cond_type = {
	{
		-1,		// FIXME type
		NULL		// refcnt
	}, 
	"condition",		// type name
	NULL,			// getter
	NULL,			// setter
	NULL,			// hash
	NULL,			// size
	NULL,			// print
	NULL,			// tostr
	NULL,			// toint
	condition_exec,		// exec
	NULL,			// compare
	NULL,			// compare 2
	NULL,			// as number
	NULL,			// as sequence
	NULL,			// call	
	free_cond		// free
};

FrogType while_type = {
	{
		-1,		// FIXME type
		NULL		// refcnt
	}, 
	"while",		// type name
	NULL,			// getter
	NULL,			// setter
	NULL,			// hash
	NULL,			// size
	NULL,			// print
	NULL,			// tostr
	NULL,			// toint
	while_exec,		// exec
	NULL,			// compare
	NULL,			// compare 2
	NULL,			// as number
	NULL,			// as sequence
	NULL,			// call	
	free_while		// free
};

FrogType block_type = {
	{
		-1,		// FIXME type
		NULL		// refcnt
	}, 
	"block",		// type name
	NULL,			// getter
	NULL,			// setter
	NULL,			// hash
	NULL,			// size
	NULL,			// print
	NULL,			// tostr
	NULL,			// toint
	block_exec,		// exec
	NULL,			// compare
	NULL,			// compare 2
	NULL,			// as number
	NULL,			// as sequence
	NULL,			// call	
	free_block		// free
};

FrogType breaker_type = {
	{
		-1,		// FIXME type
		NULL		// refcnt
	}, 
	"breaker",		// type name
	NULL,			// getter
	NULL,			// setter
	NULL,			// hash
	NULL,			// size
	NULL,			// print
	NULL,			// tostr
	NULL,			// toint
	breaker_exec,		// exec
	NULL,			// compare
	NULL,			// compare 2
	NULL,			// as number
	NULL,			// as sequence
	NULL,			// call	
	free_breaker		// free
};
