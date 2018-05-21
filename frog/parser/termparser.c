#include "../frog.h"

#include <signal.h>

#include <readline/readline.h>
#include <readline/history.h>

/*static void kill_handler(int dummy)
{
	UNUSED(dummy);
	signal(SIGINT, kill_handler);
	//printf("Need to interrupt\n");
}*/

static fchar *nextline_terminal(tokenizer *tkz)
{
	UNUSED(tkz);

	while(1)
	{
		char *line;
		line = readline(tkz->prefix);

		if(line == NULL)
			return NULL;

		if(!*line && tkz->prefix == (char *) PREFIX_CON)
		{
			free(line);

			line = malloc(2 * sizeof(char));
			if(!line) return NULL;

			*line = ';';
			*(line + 1) = '\0';
		}
		else
		{
			add_history(line);
		}

		FrogString *str = (FrogString *) utf8tostr(line);

		if(str == NULL)
		{
			free(line);
			return NULL;
		}

		if(str->length == 0)
		{
			free(line);
			free(str->str);
			free(str);

			continue;
		}

		free(line);

		fchar *res = str->str;
		free(str);

		tkz->prefix = PREFIX_CON;

		return res;
	}
}

void parse_terminal(void)
{
	//signal(SIGINT, kill_handler);
	tokenizer *tkz = create_tokenizer(nextline_terminal, NULL);
	worker *w = malloc(sizeof(worker));
	FrogObject **filememory = NULL;
	stack *st = create_stack();

	FrogModule *module = (FrogModule *) EmptyModule("stdin");

	if(!w || !st || !module)
		return;

	w->loop_start = NULL;
	w->lstack = NULL;
	w->file = module->name2var;
	w->func = w->file;
	w->waiting = NULL;
	w->ins = NULL;
	w->size = 0;
	w->capacity = 0;
	w->module = (FrogObject *) module;
	w->yield = -1;

	FrogObject *res = NULL;

	filememory = module->memory;

	if(!tkz)
	{
		FrogErr_Memory();
		return;
	}

	while(!tkz->eof)
	{
		tkz->prefix = tkz->linepos ? PREFIX_CON : PREFIX_NEW;

		ast *v = parse_instruction(tkz);

		if(v)
		{
			if(!ast2bc(v, w))
				puts("oups");

			filememory = realloc(filememory, sizeof(FrogObject *) * w->file->size);
			module->memory = filememory;

			if(!filememory)
				return;

			res = execute(st, filememory, filememory, w->ins, w->size, NULL);

			if(!res)
			{
				FrogErr_DebugPrint();
				//Frog_Print(FrogErr_Get(), stdout);
			}

			free(w->ins);

			w->ins = NULL;
			w->size = 0;
			w->capacity = 0;
		}
		else if(!tkz->eof)
		{
			tkz->line = NULL;
			tkz->linepos = NULL;
			tkz->current = NULL;
			tkz->next = NULL;

			printf("syntax error\n");//FIXME
		}
	}

	printf("\n");

	free_tokenizer(tkz);
}
