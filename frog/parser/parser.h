#ifndef __PARSER_H__
#define __PARSER_H__

#define MAX_PRIORITY 13

#include "tokenizer.h"

/**
 * parse frog from terminal
 */
void parse_terminal(void);

/**
 * parse frog from file
 */
FrogObject *parse_file(char *filename);

ast *parse_value(tokenizer *tkz, int priority);

ast *parse_if(tokenizer *tkz);

ast *parse_while(tokenizer *tkz);

ast *parse_dowhile(tokenizer *tkz);

ast *parse_for(tokenizer *tkz);

ast *parse_foreach(tokenizer *tkz);

ast *parse_breaker(tokenizer *tkz, int type);

ast *parse_function(tokenizer *tkz);

ast *parse_class(tokenizer *tkz);

/**
 * iend:
 * => 0: only ; => None, but no ; at end
 * => 1: ; need
 * => 2: no ;
 */
ast *parse_instruction0(tokenizer *tkz, int iend);

ast *parse_instruction(tokenizer *tkz);
#endif
