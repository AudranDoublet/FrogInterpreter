#include <stdio.h>
#include "frog.h"
#include <readline/readline.h>
#include <readline/history.h>

fchar *nextline1(tokenizer *tkz)
{
	UNUSED(tkz);

	char *line;
	line = readline(">>> ");

	FrogString *str = (FrogString *) utf8tostr(line);
	free(line);

	if(str == NULL)
	{
		return NULL;
	}

	if(str->length == 0)
	{
		free(str->str);
		free(str);

		return NULL;
	}

	fchar *res = str->str;
	free(str);

	return res;
}

int main(void)
{
	parse_terminal();
	return 0;
}
