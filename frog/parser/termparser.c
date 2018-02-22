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

void safe_environment(FrogObject *env)
{
	FrogEnv *e = (FrogEnv *) env;

	if(e->breaktype != BREAK_NONE)
	{
		switch(e->breaktype)
		{
			case BREAK_BREAK:
				FrogErr_Post("SyntaxError", "break outside a loop");
				break;
			case BREAK_CONTINUE:
				FrogErr_Post("SyntaxError", "continue outside a loop");
				break;
			case BREAK_RETURN:
				FrogErr_Post("SyntaxError", "return outside a function");
				break;
		}

		e->breaktype = BREAK_NONE;
	}
}

void parse_terminal(void)
{
	//signal(SIGINT, kill_handler);
	tokenizer *tkz = create_tokenizer(nextline_terminal, NULL);
	FrogObject *env = CreateEnvironment(NULL);

	if(!tkz)
	{
		FrogErr_Memory();
		return;
	}

	while(!tkz->eof)
	{
		tkz->prefix = tkz->linepos ? PREFIX_CON : PREFIX_NEW;

		FrogObject *v = parse_instruction(tkz);

		if(v)
		{
			v = FrogCall_Exec(env, v);
			v = GetHybrid(v);

			if(v)
			{
				Frog_Print(v, stdout);
				printf("\n");
			}
			else
			{
				safe_environment(env);
				FrogErr_DebugPrint();
			}
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
