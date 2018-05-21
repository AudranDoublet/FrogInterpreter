void Frog_Free(FrogObject *v);

FrogObject *Frog_ToString(FrogObject *a);

FrogObject *Frog_Len(FrogObject *a);

int Frog_Print(FrogObject *a, FILE *out);

long Frog_Hash(FrogObject *a);

FrogObject *FrogCall_BAnd(FrogObject *a, FrogObject *b);

FrogObject *FrogCall_BOr(FrogObject *a, FrogObject *b);

FrogObject *FrogCall_AsBool(FrogObject *o);

FrogObject *FrogCall_Not(FrogObject *o);

FrogObject *FrogCall_Exec(FrogObject *env, FrogObject *a);

FrogObject *FrogCall_EQ(FrogObject *a, FrogObject *b);

FrogObject *FrogCall_NE(FrogObject *a, FrogObject *b);

int FrogCall_Compare(FrogObject *a, FrogObject *b);

FrogObject *FrogCall_LO(FrogObject *a, FrogObject *b);

FrogObject *FrogCall_GT(FrogObject *a, FrogObject *b);

FrogObject *FrogCall_LE(FrogObject *a, FrogObject *b);

FrogObject *FrogCall_GE(FrogObject *a, FrogObject *b);

FrogObject *FrogCall_Add(FrogObject *a, FrogObject *b);

FrogObject *FrogCall_Sub(FrogObject *a, FrogObject *b);

FrogObject *FrogCall_Pos(FrogObject *a);

FrogObject *FrogCall_Neg(FrogObject *a);

FrogObject *FrogCall_Mul(FrogObject *a, FrogObject *b);

FrogObject *FrogCall_Div(FrogObject *a, FrogObject *b);

FrogObject *FrogCall_DivF(FrogObject *a, FrogObject *b);

FrogObject *FrogCall_Mod(FrogObject *a, FrogObject *b);

FrogObject *FrogCall_Pow(FrogObject *a, FrogObject *b);

FrogObject *FrogCall_Inv(FrogObject *a);

FrogObject *FrogCall_LShift(FrogObject *a, FrogObject *b);

FrogObject *FrogCall_RShift(FrogObject *a, FrogObject *b);

FrogObject *FrogCall_And(FrogObject *a, FrogObject *b);

FrogObject *FrogCall_Or(FrogObject *a, FrogObject *b);

FrogObject *FrogCall_Xor(FrogObject *a, FrogObject *b);

FrogObject *FrogCall_IAdd(FrogObject *a, FrogObject *b);

FrogObject *FrogCall_ISub(FrogObject *a, FrogObject *b);

FrogObject *FrogCall_IMul(FrogObject *a, FrogObject *b);

FrogObject *FrogCall_IDiv(FrogObject *a, FrogObject *b);

FrogObject *FrogCall_IDivF(FrogObject *a, FrogObject *b);

FrogObject *FrogCall_IMod(FrogObject *a, FrogObject *b);

FrogObject *FrogCall_IPow(FrogObject *a, FrogObject *b);

FrogObject *FrogCall_INeg(FrogObject *a, FrogObject *b);

FrogObject *FrogCall_IPos(FrogObject *a, FrogObject *b);

FrogObject *FrogCall_IAbs(FrogObject *a, FrogObject *b);

FrogObject *FrogCall_IInv(FrogObject *a, FrogObject *b);

FrogObject *FrogCall_ILShift(FrogObject *a, FrogObject *b);

FrogObject *FrogCall_IRShift(FrogObject *a, FrogObject *b);

FrogObject *FrogCall_IAnd(FrogObject *a, FrogObject *b);

FrogObject *FrogCall_IOr(FrogObject *a, FrogObject *b);

FrogObject *FrogCall_IXor(FrogObject *a, FrogObject *b);

FrogObject *FrogCall_Get(FrogObject *a, FrogObject *b);

FrogObject *FrogCall_Set(FrogObject *a, FrogObject *b, FrogObject *c, binaryfunction f);

FrogObject *FrogCall_SeqGet(FrogObject *a, FrogObject *b);

FrogObject *FrogCall_SeqSet(FrogObject *a, FrogObject *b, FrogObject *c, binaryfunction f);

FrogObject *FrogCall_Call(FrogObject *func, FrogObject **args, size_t len, stack *stack);

FrogObject *FrogCall_IterInit(FrogObject *a);

FrogObject *FrogCall_IterHasNext(FrogObject *a);

FrogObject *FrogCall_IterNext(FrogObject *a);
