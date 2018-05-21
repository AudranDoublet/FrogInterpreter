#ifndef __FROGERROR_H__
#define __FROGERROR_H__

typedef struct {
	FrogObjHead
	FrogObject *name;
	FrogObject *message;
} FrogError;

FrogObject *FrogErr_Get(void);

void FrogErr_DebugPrint(void);

void FrogErr_Post(char *name, char *message);

void FrogErr_Operator(FrogObject *a, FrogObject *b, char *op);

// msg should contains %ls
void FrogErr_Type(char *msg, FrogObject *a);

void FrogErr_Attribute(FrogObject *a, char *type);

void FrogErr_Key(FrogObject *a);

void FrogErr_Name(FrogObject *a);

void FrogErr_ClassName(FrogObject *a);

void FrogErr_SubName(FrogObject *obj, FrogObject *search);

void FrogErr_InstanceName(char *s);

void FrogErr_Module(FrogObject *a);

void FrogErr_BadArgCount(FrogObject *func, size_t good_count, size_t bad_count);

void FrogErr_BadArgCountS(char *func, size_t good_count, size_t bad_count);

void FrogErr_Syntax(int line, int column);

#define FrogErr_Index() FrogErr_Post("IndexError", "Index out of bounds");

#define FrogErr_TypeS(m) FrogErr_Post("TypeError", m);

#define FrogErr_Value(m) FrogErr_Post("ValueError", m)

#define FrogErr_Memory() FrogErr_Post("MemoryError", "Memory error");

#define FrogErr_Div0() FrogErr_Post("ArithmeticError", "Division or \
modulo by zero!")

#endif
