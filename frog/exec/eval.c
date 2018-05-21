#include "../frog.h"

#define ECASE(v)	case CODE_##v:
#define EBREAK(v)	break;

#define OCASE(v1, v2) ECASE(v1) current = FrogCall_##v2(scd, current); EBREAK()

static inline void PUSH_STACK(stack *s, void *v)
{
	if(ISFULL(s->last))
	{
		struct stackp *n = malloc(sizeof(struct stackp));

		if(!n)
		{
			errx(-1, "fatal memory error");
		}

		n->prev = s->last;
		n->pos = 0;
		s->last = n;
	}

	s->last->value[s->last->pos++] = v;
}

static inline void *POP_STACK(stack *s)
{
	if(ISEMPTY(s->last))
	{
		if(s->last->prev == NULL)
		{
			errx(-1, "fatal error: empty stack");
		}

		struct stackp *n = s->last->prev;

		free(s->last);
		s->last = n;
	}

	return s->last->value[--s->last->pos];
}

stack *create_stack(void)
{
	stack *st = malloc(sizeof(stack));

	if(!st)
	{
		return NULL;
	}

	st->last = NULL;
	return st;
}

FrogObject *execute(stack *stack,
		FrogObject **filememory,
		FrogObject **funcmemory,
		long *ins,
		size_t inslen,
		yieldmg *mg)
{
	size_t pos = mg ? mg->pos : 0, i = 0;
	size_t len;

	FrogObject **args;
	FrogObject *current = FrogNone();
	FrogObject *scd = NULL;
	FrogTuple *tuple = NULL;

	binaryfunction f = NULL;

	long cur = 0;

	for( ; pos < inslen; )
	{
		cur = ins[pos];

		switch(cur)
		{
		// Stack
		ECASE(PUSH)
			PUSH_STACK(stack, current);
		EBREAK()
		ECASE(POP)
			current = POP_STACK(stack);
		EBREAK()
		ECASE(EXPLODE)
			if(!FrogIsTuple(current))
			{
				FrogErr_Attribute(current, "explode");
				return NULL;
			}

			tuple = (FrogTuple *) current;

			if(tuple->length != (size_t) ins[++pos])
			{
				FrogErr_Post("ValueError", "Bad amount of item to explode");
				return NULL;
			}

			for(i = tuple->length; i > 0; i--)
			{
				PUSH_STACK(stack, tuple->array[i - 1]);
			}
		EBREAK()
		// Branchment
		ECASE(GOTO)
			pos = ins[pos + 1];
			continue;
		ECASE(BRAIF)
			if(IsTrue(FrogCall_AsBool(current)))
				pos = ins[pos + 1] - 1;
			else pos++;
		EBREAK()
		ECASE(BRAIFN)
			if(IsFalse(FrogCall_AsBool(current)))
				pos = ins[pos + 1] - 1;
			else pos += 1;
		EBREAK()
		ECASE(RETURN)
			if(mg)
				mg->end = 1;
			return current;
		EBREAK()
		ECASE(YIELD)
			if(!mg)
			{
				FrogErr_Post("FatalError", "Unexcepted yield instruction");
				return NULL;
			}

			mg->pos = pos + 1;
			return current;
		EBREAK()
		ECASE(CALL)
			len  = ins[pos + 1];
			args = malloc(sizeof(size_t) * len);

			if(len != 0 && !args)
			{
				FrogErr_Memory();
				current =  NULL;
				break;
			}

			for(size_t i = len; i > 0; i--)
			{
				args[i - 1] = POP_STACK(stack);
			}

			current = FrogCall_Call(current, args, len, stack);
			pos += 1;
		EBREAK()

		// Operators
		ECASE(OP1)
			current = ((unaryfunction) ins[++pos])(current);
		EBREAK()

		ECASE(OP2)
			scd = (FrogObject *) POP_STACK(stack);
			current = ((binaryfunction) ins[++pos])(scd, current);
		EBREAK()

		// Getters and setters
		ECASE(CONSTANT)
			current = (FrogObject *) ins[++pos];
		EBREAK()
			// sequence
		ECASE(SETSEQ)
			scd = POP_STACK(stack);
			current = FrogCall_SeqSet(POP_STACK(stack), scd, current,
					(binaryfunction) ins[++pos]);
		EBREAK()
		ECASE(NSETSEQ)
			scd = POP_STACK(stack);
			current = FrogCall_SeqSet(scd, current, POP_STACK(stack),
					(binaryfunction) ins[++pos]);
		EBREAK()
		ECASE(GETSEQ)
			scd = POP_STACK(stack);
			current = FrogCall_SeqGet(scd, current);
		EBREAK()
			// file
		ECASE(GETFILE)
			current = *(filememory + ins[++pos]);

			if(!current)
			{
				current = FrogNone();
			}
		EBREAK()
			// func
		ECASE(GETFUNC)
			current = *(funcmemory + ins[++pos]);
		
			if(!current)
			{
				current = FrogNone();
			}
		EBREAK()
		ECASE(SETFUNC)
			f = (binaryfunction) ins[pos + 2];

			if(f)
			{
				if(*(funcmemory + ins[pos + 1]) == NULL)
				{
					*(funcmemory + ins[pos + 1]) = FrogNone();
				}

				current = f(*(funcmemory + ins[pos + 1]), current);
			}

			*(funcmemory + ins[pos + 1]) = current;
			pos += 2;
		EBREAK()
		ECASE(NSETFUNC)
			f = (binaryfunction) ins[pos + 2];
			current = POP_STACK(stack);

			if(f)
			{
				if(*(funcmemory + ins[pos + 1]) == NULL)
				{
					*(funcmemory + ins[pos + 1]) = FrogNone();
				}

				current = f(*(funcmemory + ins[pos + 1]), current);
			}

			*(funcmemory + ins[pos + 1]) = current;
			pos += 2;
		EBREAK()
			// sub
		ECASE(GETSUB)
			current = FrogCall_Get(current, (FrogObject *) ins[++pos]);
		EBREAK()
		ECASE(SETSUB)
			current = FrogCall_Set(POP_STACK(stack),
					(FrogObject *) ins[pos + 1], current,
					(binaryfunction) ins[pos + 2]);

			pos += 2;
		EBREAK()
		ECASE(NSETSUB)
			current = FrogCall_Set(current,
					(FrogObject *) ins[pos + 1], POP_STACK(stack),
					(binaryfunction) ins[pos + 2]);

			pos += 2;
		EBREAK()
		}

		if(current == NULL)
		{
			return NULL;
		}
		pos++;
	}

	if(mg) mg->end = 1;
	return FrogNone();
}
