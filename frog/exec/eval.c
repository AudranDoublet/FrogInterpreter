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

	return s->last->value[s->last->pos--];
}

FrogObject *execute(stack *stack,
		FrogObject **filememory,
		FrogObject **funcmemory,
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
				OCASE(ADD, Add)
				OCASE(SUB, Sub)
				OCASE(MUL, Mul)
				OCASE(DIV, Div)
				OCASE(DIVF, DivF)
				OCASE(MOD, Mod)
				OCASE(POW, Pow)
				OCASE(BWLSF, LShift)
				OCASE(BWRSF, RShift)
				OCASE(BWOR, Or)
				OCASE(BWAND, And)
				OCASE(BWXOR, Xor)
				OCASE(CMPEQ, EQ)
				OCASE(CMPNE, NE)
				OCASE(CMPLO, LO)
				OCASE(CMPGT, GT)
				OCASE(CMPLE, LE)
				OCASE(CMPGE, GE)
				OCASE(IADD, IAdd)
				OCASE(ISUB, ISub)
				OCASE(IMUL, IMul)
				OCASE(IDIV, IDiv)
				OCASE(IDIVF, IDivF)
				OCASE(IMOD, IMod)
				OCASE(IPOW, IPow)
				OCASE(IBWLSF, ILShift)
				OCASE(IBWRSF, IRShift)
				OCASE(IBWOR, IOr)
				OCASE(IBWAND, IAnd)
				OCASE(IBWXOR, IXor)
			}

			continue;
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
		EBREAK()
		ECASE(CALL)
			// FIXME
		EBREAK()

		// One operand operators
		ECASE(NOT)
			current = FrogCall_Not(current);
		EBREAK()
		ECASE(BWNOT)
			current = FrogCall_Inv(current);
		EBREAK()

		// Getters and setters
		ECASE(CONSTANT)
			current = (FrogObject *) ins[++pos];
		EBREAK()
			// sequence
		ECASE(SETSEQ)
			scd = POP_STACK(stack);
			current = FrogCall_SeqSet(POP_STACK(stack), scd, current);
		EBREAK()
		ECASE(GETSEQ)
			scd = POP_STACK(stack);
			current = FrogCall_SeqGet(scd, current);
		EBREAK()
			// file
		ECASE(GETFILE)
			current = *(filememory + ins[++pos]);
		EBREAK()
		ECASE(SETFILE)
			*(filememory + ins[++pos]) = current;
		EBREAK()
			// func
		ECASE(GETFUNC)
			current = *(funcmemory + ins[++pos]);
		EBREAK()
		ECASE(SETFUNC)
			*(funcmemory + ins[++pos]) = current;
		EBREAK()
			// sub
		ECASE(GETSUB)
			current = FrogCall_Get(current, (FrogObject *) ins[++pos]);
		EBREAK()
		ECASE(SETSUB)
			current = FrogCall_Set(POP_STACK(stack),
					(FrogObject *) ins[++pos], current);
		EBREAK()
		}

		if(current == NULL)
			return NULL;
	}
}
