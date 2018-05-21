#ifndef __TOKENIZER_H__
#define __TOKENIZER_H__

#define TOKEN_ERROR		-1
#define TOKEN_EOF		0x00

// Comparaison operators
#define TOKEN_CMPEQ	0x01 // ==
#define TOKEN_CMPNE	0x02 // !=
#define TOKEN_CMPLO	0x03 // <
#define TOKEN_CMPGT	0x04 // >
#define TOKEN_CMPLE	0x05 // <=
#define TOKEN_CMPGE	0x06 // >=

// Operators
#define TOKEN_OPTPLS	0x07 // +
#define TOKEN_OPTMIN	0x08 // -
#define TOKEN_OPTMUL	0x09 // *
#define TOKEN_OPTDIV	0x0a // /
#define TOKEN_OPTDIVF	0x0b // //
#define TOKEN_OPTMOD	0x0c // %
#define TOKEN_OPTPOW	0x0d // **
#define TOKEN_OPTNOT	0x0e // !

// Bitwise operators
#define TOKEN_BWOLSF	0x0f // <<
#define TOKEN_BWORSF	0x10 // >>
#define TOKEN_BWOOR	0x11 // |
#define TOKEN_BWOAND	0x12 // &
#define TOKEN_BWOXOR	0x13 // ^
#define TOKEN_BWONOT	0x14 // ~

#define TOKEN_COMMA	0x15 // ,

// Assignation operators
#define TOKEN_ASSIGN    0x16 // =
#define TOKEN_AOPTPLS	0x17 // +=
#define TOKEN_AOPTMIN	0x18 // -=
#define TOKEN_AOPTMUL	0x19 // *=
#define TOKEN_AOPTDIV	0x1a // /=
#define TOKEN_AOPTDIVF	0x1b // //=
#define TOKEN_AOPTMOD	0x1c // %=
#define TOKEN_AOPTPOW	0x1d // **=

// Bitwise assignation operators
#define TOKEN_ABWOLSF	0x1e // <<=
#define TOKEN_ABWORSF	0x1f // >>=
#define TOKEN_ABWOOR	0x20 // |=
#define TOKEN_ABWOAND	0x21 // &=
#define TOKEN_ABWOXOR	0x22 // ^=

#define OPERATORS_END	0x22

// Value
#define TOKEN_INTEGER	0x30 // 10 0xaa
#define TOKEN_DECIMAL	0x31 // 0.10 0.10e10
#define TOKEN_STRING	0x32 // "example" 'example' @"example\:p"
#define TOKEN_TRUE	0x33 // True
#define TOKEN_FALSE	0x34 // False
#define TOKEN_NONE	0x35 // None
#define TOKEN_ID	0x36 // Identifier

// Branchement instructions
#define TOKEN_IF	0x40 // if
#define TOKEN_ELSE	0x41 // else
#define TOKEN_WHILE	0x42 // while
#define TOKEN_FOR	0x43 // for
#define TOKEN_FOREACH	0x44 // foreach
#define TOKEN_DO	0x45 // do
#define TOKEN_CONTINUE	0x46 // continue
#define TOKEN_BREAK	0x47 // break
#define TOKEN_TRY	0x48 // try
#define TOKEN_CATCH	0x49 // catch

// Functions instructions
#define TOKEN_RETURN	0x50 // return
#define TOKEN_YIELD	0x51 // yield
#define TOKEN_FUNCTION	0x52 // func

// Classes instruction
#define TOKEN_CLASS	0x60
#define TOKEN_EXTENDS	0x61

//  General instructions
#define TOKEN_OPEN	0x71 // {
#define TOKEN_CLOSE	0x72 // }
#define TOKEN_SOPEN	0x73 // (
#define TOKEN_SCLOSE	0x74 // )
#define TOKEN_AOPEN	0x75 // [
#define TOKEN_ACLOSE	0x76 // ]
#define TOKEN_IEND	0x77 // ;
#define TOKEN_SUB	0x78 // .
#define TOKEN_SEP	0x79 // :
#define TOKEN_DICT	0x7a // dict{

#define TOKEN_INF	0x80 // inf

#define PREFIX_NEW ">>> "
#define PREFIX_CON "... "

typedef struct tokenizer tokenizer;
typedef fchar *(*nextline)(tokenizer *ptr);

typedef struct
{
	int type;
	FrogObject *value;
} tokeninfo;

struct automatanode
{
	struct automatanode *nexts[128];
	int tokentype;
};

struct tokenizer
{
	fchar *line;
	fchar *linepos;

	void *file;
	nextline linegetter;
	int eof;

	struct automatanode *head;
	tokeninfo *current;
	tokeninfo *next;
	tokeninfo *last;

	int linenb, colnb;
	int need_semicolon;
	char *prefix;
};

/**
 * Create a new tokenizer
 */
tokenizer *create_tokenizer(nextline linegetter, void *file);

/**
 * Change value of tokenizer->current (return the newvalue) and set tokenizer->next to NULL.
 */
tokeninfo *next_token(tokenizer *tkz);

/**
 * Change value of tokenizer->next and return the new value
 */
tokeninfo *peek_token(tokenizer *tkz);

/**
 * Return tokenizer->current if not NULL, otherwise call next_token.
 */
tokeninfo *current_token(tokenizer *tkz);

/**
 * Verify that the result of next_token is of the right
 * token type.
 */
int eat_token(tokenizer *tkz, int type);

/**
 * If last token was an IEND, return 2
 * If current token is an IEND, return 1
 * Otherwise return 0
 */
int is_iend(tokenizer *tkz);

/**
 * Consume current token without parsing the next one
 */
void consume_token(tokenizer *tkz);

/**
 * Free the given parser
 */
void free_tokenizer(tokenizer *tkz);
#endif
