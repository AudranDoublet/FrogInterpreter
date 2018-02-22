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

FrogObject *parse_value(tokenizer *tkz, int priority);

FrogObject *parse_if(tokenizer *tkz);

FrogObject *parse_while(tokenizer *tkz);

FrogObject *parse_dowhile(tokenizer *tkz);

FrogObject *parse_for(tokenizer *tkz);

FrogObject *parse_breaker(tokenizer *tkz, int type);

/**
 * iend:
 * => 0: only ; => None, but no ; at end
 * => 1: ; need
 * => 2: no ;
 */
FrogObject *parse_instruction0(tokenizer *tkz, int iend);

FrogObject *parse_instruction(tokenizer *tkz);
#endif
