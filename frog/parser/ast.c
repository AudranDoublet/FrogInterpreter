#include "../frog.h"

ast *ast_(int type, int count)
{
	ast *ast = malloc(sizeof(ast) * 2);

	if(!ast)
		return NULL;

	ast->type = type;
	ast->childcount = count;
	ast->childs = calloc(sizeof(void **), count);

	if(!ast->childs)
	{
		free(ast);
		return NULL;
	}

	return ast;
}

ast *ast_empty()
{
	return ast_(AST_EMPTY, 0);
}

ast *ast_create1(int type, void *child)
{
	ast *r = ast_(type, 1);
	r->childs[0] = child;

	return r;
}

ast *ast_create2(int type, void *a, void *b)
{
	ast *r = ast_(type, 2);
	
	if(!r)
		return NULL;

	r->childs[0] = a;
	r->childs[1] = b;

	return r;
}


ast *ast_create3(int type, void *a, void *b, void *c)
{
	ast *r = ast_(type, 3);
	
	if(!r)
		return NULL;

	r->childs[0] = a;
	r->childs[1] = b;
	r->childs[2] = c;

	return r;
}

ast *ast_constant(FrogObject *v)
{
	return ast_create1(AST_CONSTANT, v);
}

ast *ast_var(FrogObject *v)
{
	return ast_create1(AST_VAR, v);
}

ast *ast_subvar(ast *left, FrogObject *v)
{
	return ast_create2(AST_SUBVAR, left, v);
}

ast *ast_subarr(ast *left, ast *v)
{
	return ast_create2(AST_SUBARR, left, v);
}

ast *ast_assign(void *func, ast *a, ast *b)
{
	return ast_create3(AST_ASSIGN, func, a, b);
}

ast *ast_op2(void *func, ast *a, ast *b)
{
	return ast_create3(AST_OP2, func, a, b);
}

ast *ast_op1(void *func, ast *a)
{
	return ast_create2(AST_OP1, func, a);
}
