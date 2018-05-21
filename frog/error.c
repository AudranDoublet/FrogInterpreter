#include <stdio.h>
#include "frog.h"

_Thread_local static char buffer[1024];
_Thread_local static FrogObject *error = NULL;

static FrogObject *create_error(char *name, char *message)
{
	FrogError *err = malloc(sizeof(FrogError));

	if(err == NULL)
	{
		errx(-1, "fatal error: out of memory");
	}

	err->name = utf8tostr(name);
	err->message = message ? utf8tostr(message) : NULL; //FIXME: None

	return (FrogObject *) err;
}

FrogObject *FrogErr_Get(void)
{
	return error;
}

void FrogErr_DebugPrint(void)
{
	FrogError *err = (FrogError *) error;
	Frog_Print(err->name, stdout);
	printf(": ");
	Frog_Print(err->message, stdout);
	printf("\n");
	error = NULL;
}

void FrogErr_Post(char *name, char *message)
{
	if(!error)
		error = create_error(name, message);
}

void FrogErr_Type(char *msg, FrogObject *a)
{
	sprintf(buffer, msg, ObType(a)->name);
	FrogErr_Post("TypeError", buffer);
}

void FrogErr_Attribute(FrogObject *a, char *type)
{
	sprintf(buffer, "'%s' not supported for type '%s'",
		type, ObType(a)->name);
	FrogErr_Post("AttributeError", buffer);
}

void FrogErr_Key(FrogObject *a)
{
	sprintf(buffer, "%ls", ((FrogString *) a)->str);
	FrogErr_Post("KeyError", buffer);
}


void FrogErr_Name(FrogObject *a)
{
	sprintf(buffer, "'%ls' not defined", ((FrogString *) a)->str);
	FrogErr_Post("NameError", buffer);
}

void FrogErr_ClassName(FrogObject *a)
{
	sprintf(buffer, "class '%ls' not defined", ((FrogString *) a)->str);
	FrogErr_Post("NameError", buffer);
}

void FrogErr_SubName(FrogObject *obj, FrogObject *s)
{
	FrogString *obstr = (FrogString *) Frog_ToString(obj);

	sprintf(buffer, "'%ls' has no attribute '%ls'", obstr->str, ((FrogString *) s)->str);
	FrogErr_Post("AttributeError", buffer);
}

void FrogErr_InstanceName(char *name)
{
	sprintf(buffer, "this instance has no attribute '%s'", name);
	FrogErr_Post("AttributeError", buffer);
}

void FrogErr_Module(FrogObject *a)
{
	sprintf(buffer, "'%ls' module can't be found", ((FrogString *) a)->str);
	FrogErr_Post("NameError", buffer);
}

void FrogErr_Operator(FrogObject *a, FrogObject *b, char *op)
{
	if(!b)
	{
		sprintf(buffer, "%s not supported on '%s'", op, ObType(a)->name);
	}
	else sprintf(buffer, "%s not supported between '%s' and '%s'",op, 
		ObType(a)->name, ObType(b)->name);
	FrogErr_Post("TypeError", buffer);
}

void FrogErr_BadArgCount(FrogObject *func, size_t good_count, size_t bad_count)
{
	FrogString *str = (FrogString *) Frog_ToString(func);
	sprintf(buffer, "%ls need %zu arguments but %zu were given.",
		str->str, good_count, bad_count);

	FrogErr_Post("CallError", buffer);
}

void FrogErr_BadArgCountS(char *s, size_t good_count, size_t bad_count)
{
	sprintf(buffer, "%s need %zu arguments but %zu were given.",
		s, good_count, bad_count);

	FrogErr_Post("CallError", buffer);
}


void FrogErr_Syntax(int line, int col)
{
	sprintf(buffer, "Unexcepted character at line %i, column %i.", line, col);
	FrogErr_Post("SyntaxError", buffer);
}
