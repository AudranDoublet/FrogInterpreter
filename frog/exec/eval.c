static inline void PUSH_STACK(stack *s, long v)
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

static inline long POP_STACK(stack *s)
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

	for(pos < inslen)
	{
		switch(ins[pos])
		{
			case 
		}
	}
}
