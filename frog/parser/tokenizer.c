#include <stdlib.h>
#include <ctype.h>
#include "../frog.h"

static tokeninfo token_err = {TOKEN_ERROR, NULL};
static tokeninfo token_eof = {TOKEN_EOF, NULL};

struct automatanode *create_automata(int tokentype)
{
	struct automatanode *r = calloc(sizeof(struct automatanode), 1);
	r->tokentype = tokentype;

	return r;
}

struct automatanode *simple_automata(struct automatanode *node,
				char *val, int tokentype)
{
	for( ; *val ; val++ )
	{
		int i = (int) *val;
		
		if(node->nexts[i])
		{
			node = node->nexts[i];
		}
		else
		{
			node->nexts[i]  = create_automata(TOKEN_ERROR);
			node = node->nexts[i];
		}
	}

	node->tokentype = tokentype;
	return node;
}

void free_automata(struct automatanode *node)
{
	if(!node) return;

	for(size_t i = 0; i < 128; i++)
	{
		free_automata(node->nexts[i]);
	}

	free(node);
}

struct automatanode *frog_automata(void)
{
	static struct automatanode *node = NULL;

	if(node)
		return node;

	node = create_automata(TOKEN_ERROR);
	simple_automata(node, "True", TOKEN_TRUE);
	simple_automata(node, "False", TOKEN_FALSE);
	simple_automata(node, "None", TOKEN_NONE);

	simple_automata(node, "if", TOKEN_IF);
	simple_automata(node, "else", TOKEN_ELSE);	
	simple_automata(node, "while", TOKEN_WHILE);
	simple_automata(node, "for", TOKEN_FOR);
	simple_automata(node, "foreach", TOKEN_FOREACH);
	simple_automata(node, "do", TOKEN_DO);
	simple_automata(node, "continue", TOKEN_CONTINUE);
	simple_automata(node, "break", TOKEN_BREAK);
	simple_automata(node, "try", TOKEN_TRY);
	simple_automata(node, "catch", TOKEN_CATCH);

	simple_automata(node, "return", TOKEN_RETURN);
	simple_automata(node, "yield", TOKEN_YIELD);
	simple_automata(node, "function", TOKEN_FUNCTION);
	simple_automata(node, "static", TOKEN_STATIC);
	simple_automata(node, "abstract", TOKEN_ABSTRACT);
	simple_automata(node, "private", TOKEN_PRIVATE);
	simple_automata(node, "protected", TOKEN_PROTECTED);
	simple_automata(node, "print", TOKEN_PRINT);

	simple_automata(node, "class", TOKEN_CLASS);

	simple_automata(node, "include", TOKEN_INCLUDE);
	simple_automata(node, "{", TOKEN_OPEN);
	simple_automata(node, "}", TOKEN_CLOSE);
	simple_automata(node, "(", TOKEN_SOPEN);
	simple_automata(node, ")", TOKEN_SCLOSE);
	simple_automata(node, "[", TOKEN_AOPEN);
	simple_automata(node, "]", TOKEN_ACLOSE);
	simple_automata(node, ";", TOKEN_IEND);
	simple_automata(node, ".", TOKEN_SUB);

	simple_automata(node, "==", TOKEN_CMPEQ);
	simple_automata(node, "!=", TOKEN_CMPNE);
	simple_automata(node, "<", TOKEN_CMPLO);
	simple_automata(node, ">", TOKEN_CMPGT);
	simple_automata(node, "<=", TOKEN_CMPLE);
	simple_automata(node, ">=", TOKEN_CMPGE);

	simple_automata(node, "+", TOKEN_OPTPLS);
	simple_automata(node, "-", TOKEN_OPTMIN);
	simple_automata(node, "*", TOKEN_OPTMUL);
	simple_automata(node, "/", TOKEN_OPTDIV);
	simple_automata(node, "//", TOKEN_OPTDIVF);
	simple_automata(node, "%", TOKEN_OPTMOD);
	simple_automata(node, "**", TOKEN_OPTPOW);
	simple_automata(node, "!", TOKEN_OPTNOT);
	simple_automata(node, "<<", TOKEN_BWOLSF);
	simple_automata(node, ">>", TOKEN_BWORSF);
	simple_automata(node, "|", TOKEN_BWOOR);
	simple_automata(node, "&", TOKEN_BWOAND);
	simple_automata(node, "^", TOKEN_BWOXOR);
	simple_automata(node, "~", TOKEN_BWONOT);
	simple_automata(node, ",", TOKEN_COMMA);
	simple_automata(node, "=", TOKEN_ASSIGN);
	simple_automata(node, "+=", TOKEN_AOPTPLS);
	simple_automata(node, "-=", TOKEN_AOPTMIN);
	simple_automata(node, "*=", TOKEN_AOPTMUL);
	simple_automata(node, "/=", TOKEN_AOPTDIV);
	simple_automata(node, "//=", TOKEN_AOPTDIVF);
	simple_automata(node, "%=", TOKEN_AOPTMOD);
	simple_automata(node, "**=", TOKEN_AOPTPOW);
	simple_automata(node, "<<=", TOKEN_ABWOLSF);
	simple_automata(node, ">>=", TOKEN_ABWORSF);
	simple_automata(node, "|=", TOKEN_ABWOOR);
	simple_automata(node, "&=", TOKEN_ABWOAND);
	simple_automata(node, "^=", TOKEN_ABWOXOR);

	return node;
}

tokenizer *create_tokenizer(nextline linegetter, void *file)
{
	tokenizer *tkz = malloc(sizeof(tokenizer));

	if(!tkz)
		return NULL;

	tkz->eof = 0;
	tkz->line = NULL;
	tkz->linepos = NULL;
	tkz->file = file;
	tkz->linegetter = linegetter;
	tkz->head = frog_automata();
	tkz->current = NULL;
	tkz->next = NULL;
	tkz->prefix = PREFIX_NEW;

	return tkz;
}

void free_tokeninfo(tokeninfo *tki)
{
	if(tki && tki != &token_err && tki != &token_eof)
		free(tki);
}

static int next_line(tokenizer *tkz)
{
	free(tkz->line);
	tkz->line = (*tkz->linegetter)(tkz);
	tkz->linepos = tkz->line;

	return tkz->line != NULL;
}

static tokeninfo *ctoken(int type, FrogObject *value)
{
	tokeninfo *res = malloc(sizeof(tokeninfo));

	if(res == NULL)
	{
		return &token_err;
	}
	
	res->type = type;
	res->value = value;
	
	return res;
}

static fchar next_char(tokenizer *tkz)
{
	if(tkz->linepos && !*tkz->linepos)
	{
		tkz->linepos = NULL;
		return '\n';
	}

	if((!tkz->linepos || *tkz->linepos == 0) && !next_line(tkz))
		return 0;

	return *tkz->linepos;
}

static inline void consume_char(tokenizer *tkz)
{
	if(tkz->linepos) tkz->linepos++;
} 

static inline long digit16(fchar c)
{
	if(isdigit(c))
		return c - '0';
	else if(c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	else if(c >= 'A' && c <= 'F')
		return c - 'A' + 10;

	return -1;
}

static inline long digit10(fchar c)
{
	if(c >= '0' && c <= '9')
		return c - '0';
	return -1;
}

static inline long digit2(fchar c)
{
	if(c == '0')
		return 0;
	else if(c == '1')
		return 1;
	else return -1;
}

tokeninfo *read_int16(tokenizer *tkz)
{
	long digit = digit16(next_char(tkz));
	long result = 0;

	for(; digit != -1; digit = digit16(next_char(tkz)))
	{
		result = (result << 4) | digit;
		consume_char(tkz);
	}

	return ctoken(TOKEN_INTEGER, FromNativeInteger(result));
}

tokeninfo *read_int2(tokenizer *tkz)
{
	long digit = digit2(next_char(tkz));
	long result = 0;

	for(; digit != -1; digit = digit2(next_char(tkz)))
	{
		result = (result << 1) | digit;
		consume_char(tkz);
	}

	return ctoken(TOKEN_INTEGER, FromNativeInteger(result));
}

long read_int10s(tokenizer *tkz)
{
	long digit = digit10(next_char(tkz));
	long result = 0;

	for(; digit != -1; digit = digit10(next_char(tkz)))
	{
		result = (result * 10) + digit;
		consume_char(tkz);
	}

	return result;
}

tokeninfo *read_int10(tokenizer *tkz)
{
	long base = read_int10s(tkz);
	return ctoken(TOKEN_INTEGER, FromNativeInteger(base));
}

tokeninfo *read_number(tokenizer *tkz)
{
	if(next_char(tkz) == '0')
	{
		consume_char(tkz);

		if(next_char(tkz) == 'x')
		{
			consume_char(tkz);
			return read_int16(tkz);
		}
		else if(next_char(tkz) == 'b')
		{
			consume_char(tkz);
			return read_int2(tkz);
		}
	}

	return read_int10(tkz);
}

fchar read_escape_sequence(tokenizer *tkz)
{
	consume_char(tkz);
	fchar c = next_char(tkz);
	fchar res = c;

	int tmp;
	int max = 4;

	if(!c) return '\0';

	switch(c)
	{
		case '\n':
		case '\r':
		case '\t':
		case ' ':
			return (fchar) -1;
		case 'n':
			return '\n';
		case 't':
			return '\t';
		case 'r':
			return '\r';
		case 'v':
			return '\v';
		case 'x':
		case 'U':
		case 'u':
			max = c == 'U' ? 8 : (c == 'x' ? 2 : 4);
			res = 0;
			for(int i = 0; i < max; i++)
			{
				consume_char(tkz);
				c = next_char(tkz);
				tmp = digit16(c);

				if(tmp < 0) return 0;
				res = (res << 4) | tmp;
			}
			return res;
	}

	return res; //FIXME
}

tokeninfo *read_string(tokenizer *tkz, fchar end, int escape)
{
	struct strbuilder *builder = NULL;
	consume_char(tkz);	
	fchar c = next_char(tkz);

	builder = create_strbuilder();
	if(!builder)
		goto error;

	for( ; c != end; consume_char(tkz), c = next_char(tkz))
	{
		if(escape && c == '\\')
		{
			c = read_escape_sequence(tkz);
			if(!c) goto error;
			if(c == (fchar)-1) continue;
		}

		add_strbuilder(builder, c);
	}

	consume_char(tkz);

	fchar *res = tostr_strbuilder(builder);
	if(!res) goto error;

	FrogObject *o = utf32tostr(res);

	if(!o)
	{
		free(res);
		goto error;
	}

	free_strbuilder(builder);
	return ctoken(TOKEN_STRING, o);

error:
	free_strbuilder(builder);
	return &token_err;
}

tokeninfo *read_keyword(tokenizer *tkz)
{
	struct automatanode *head;
	int alphanum, tokentype;
	FrogObject *res;
	struct strbuilder *builder = NULL;
	fchar c;
	fchar *str;
	size_t len = 0;

	head = tkz->head;
	alphanum = 1;
	builder = create_strbuilder();
	res = NULL;
	c = next_char(tkz);

	if(!builder)
	{
		goto error;
	}

	for( ; ; consume_char(tkz), c = next_char(tkz))
	{
		if(c > 127 || !head->nexts[c])
		{
			break;
		}

		len++;
		alphanum = alphanum && (isalnum(c) || c == '_');
		add_strbuilder(builder, c);
		head = head->nexts[c];
	}

	tokentype = head->tokentype;

	if(head->tokentype == TOKEN_ERROR)
	{
		if(!alphanum)
		{
			goto error;
		}
		else
		{
			for( ; ; consume_char(tkz), c = next_char(tkz))
			{
				if(c > 127 || (!isalnum(c) && c != '_'))
				{
					break;
				}

				len++;
				add_strbuilder(builder, c);
			}

			if(len == 0)
				goto error;

			str = tostr_strbuilder(builder);
			res = utf32tostr(str);

			if(res == NULL)
			{
				free(str);
				goto error;
			}

			tokentype = TOKEN_ID;
		}
	}

	free_strbuilder(builder);
	return ctoken(tokentype, res);

error:
	free_strbuilder(builder);
	return &token_err;
}

tokeninfo *read_token(tokenizer *tkz)
{
	fchar c;

start:
	if(tkz->linepos == NULL && !next_line(tkz))
	{
		goto eof;
	}

	while( isspace(next_char(tkz)) )
	{
		consume_char(tkz);
	}
	c = next_char(tkz);

	if(c == 0)
	{
		goto eof;
	}
	else if(c > 127)
	{
		goto error;
	}

	if(c == '#')
	{
		// one line comment, only skip line
		tkz->linepos = NULL;
		goto start; // recall function (but it's useless to enlarge stack)
	}
	else if(isdigit(c))
	{
		return read_number(tkz);
	}
	else if(c == '"' || c == '\'')
	{
		return read_string(tkz, c, 1);
	}
	else // keyword or identifier
	{
		return read_keyword(tkz);
	}

error:
	return &token_err;

eof:
	tkz->eof = 1;
	return &token_eof;
}

tokeninfo *next_token(tokenizer *tkz)
{
	if(tkz->current)
	{
		free_tokeninfo(tkz->last);
		tkz->last = tkz->current;
	}

	if(tkz->next != NULL)
	{
		tkz->current = tkz->next;
		tkz->next = NULL;

		return tkz->current;
	}

	return tkz->current = read_token(tkz);
}

tokeninfo *peek_token(tokenizer *tkz)
{
	if(tkz->next != NULL)
	{
		return tkz->next;
	}

	return tkz->next = read_token(tkz);
}

tokeninfo *current_token(tokenizer *tkz)
{
	if(tkz->current == NULL)
		return next_token(tkz);
	return tkz->current;
}

int eat_token(tokenizer *tkz, int type)
{
	int res = current_token(tkz)->type == type;
	consume_token(tkz);

	return res;
}

int is_iend(tokenizer *tkz)
{
	if(tkz->last && tkz->last->type == TOKEN_IEND)
		return 2;
	return current_token(tkz)->type == TOKEN_IEND;
}

void consume_token(tokenizer *tkz)
{
	if(!tkz->current) return;

	free_tokeninfo(tkz->last);
	tkz->last = tkz->current;
	tkz->current = NULL;
}

void free_tokenizer(tokenizer *tkz)
{
	if(!tkz)
		return;

	free_tokeninfo(tkz->last);
	free_tokeninfo(tkz->current);
	free_tokeninfo(tkz->next);
	free(tkz->line);

	free_automata(tkz->head);
	free(tkz);	
}
