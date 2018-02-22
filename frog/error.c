#include <stdio.h>
#include "frog.h"

static char buffer[1024];
static FrogObject *error = NULL;

static FrogObject *create_error(char *name, char *message)
{
	FrogError *err = malloc(sizeof(FrogError));

	if(err == NULL)
	{
		// not enough memory to create an error...
		//TODO better handling ?
		printf("out of memory");
		exit(-1);
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
}

void FrogErr_Post(char *name, char *message)
{
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

void FrogErr_Name(FrogObject *a)
{
	sprintf(buffer, "'%ls' not defined", ((FrogString *) a)->str);
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
