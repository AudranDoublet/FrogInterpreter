#define AST_CONSTANT	0x00
#define AST_VAR		0x01
#define AST_FUNC	0x02
#define AST_SUBVAR	0x03
#define AST_SUBARR	0x04

#define AST_TUPLE	0x05
#define AST_ASSIGN	0x06
#define AST_OP1		0x07
#define AST_OP2		0x08

#define AST_EMPTY	0x09
#define AST_IF		0x0a
#define AST_WHILE	0x0b
#define AST_FOREACH	0x0c
#define AST_NEXT	0x0d

#define AST_CONTINUE	0x0e
#define AST_BREAK	0x0f
#define AST_RETURN	0x10
#define AST_YIELD	0x11

#define AST_CALL	0x12
#define AST_CLASS	0x13
#define AST_KVAR	0x14

typedef struct {
	int type;
	int childcount;
	void **childs;
} ast;

ast *ast_empty();

ast *ast_create1(int type, void *child);

ast *ast_create2(int type, void *a, void *b);

ast *ast_create3(int type, void *a, void *b, void *c);

ast *ast_constant(FrogObject *v);

ast *ast_var(FrogObject *v);

ast *ast_subvar(ast *left, FrogObject *v);

ast *ast_subarr(ast *left, ast *v);

ast *ast_assign(void *func, ast *a, ast *b);

ast *ast_op2(void *func, ast *a, ast *b);

ast *ast_op1(void *func, ast *a);
