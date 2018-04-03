#define ECASE(v)	case CODE_##v:
#define EBREAK(v)	break;

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

	return s->last->value[s->last->pos--];
}

FrogObject *execute(stack *stack,
		FrogObject *filememory,
		FrogObject *funcmemory,
		long *ins,
		size_t inslen)
{
	size_t pos = 0;
	FrogObject *current = FrogNone();
	FrogObject *scd = NULL;

	long cur = 0;

	for(pos < inslen)
	{
		cur = ins[pos];

		if(cur & 0xff00 == 0x0100)
		{
			scd = POP_STACK(stack);

			switch(cur)
			{
				ECASE(ADD)
					current = FrogCall_Add
			}:
		}

		switch(cur)
		{
		// Stack
		ECASE(PUSH)
			PUSH_STACK(stack, current);
		EBREAK()
		ECASE(POP)
			current = POP_STACK(stack);
		EBREAK()

		// Branchment
		ECASE(GOTO)
			pos = ins[pos + 1];
		EBREAK()
		ECASE(BRAIF)
			if(IsTrue(FrogCall_AsBool(current)))
				pos = ins[pos + 1];
			else pos += 1;
		EBREAK()
		ECASE(BRAIFN)
			if(IsFalse(FrogCall_AsBool(current)))
				pos = ins[pos + 1];
			else pos += 1;
		EBREAK()

		ECASE(RETURN)
			return current;
		ECASE(CALL)
			// FIXME
		EBREAK()
		}
	}
}
