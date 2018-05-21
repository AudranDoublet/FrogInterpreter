#include "../frog.h"

static void consume_wait(worker *w, ast *tree, int spe);

static int ast2bc0(ast *tree, worker *w);

static long a2b_addins(worker *w, long ins)
{
	if(w->waiting != NULL)
	{
		consume_wait(w, NULL, 0);
	}

	if(w->size == w->capacity)
	{
		w->capacity += 64;
		w->ins = realloc(w->ins, w->capacity * sizeof(long));

		if(!w->ins)
		{
			errx(-1, "fatal error: memory");
		}
	}

	w->ins[w->size] = ins;
	return w->size++;
}

static long insert_varmap(hashmap *m, FrogObject *name)
{
	long pos = m->size;
	long *p = malloc(sizeof(long));
	
	if(!p)
	{
		errx(-1, "fatal error: memory");
	}

	*p = pos;
	put_hashmap(m, name, p);

	return pos;
}

static void create_setfunc(worker *w, FrogObject *name, int spe)
{
	long *res = get_hashmap(w->func, name);
	long pos;

	if(!res)
	{
		pos = insert_varmap(w->func, name);
		res = get_hashmap(w->func, name);
	}
	else
	{
		pos = *res;			
	}


	a2b_addins(w, spe ? CODE_NSETFUNC : CODE_SETFUNC);
	a2b_addins(w, pos);
}

static void consume_wait(worker *w, ast *tree, int spe)
{
	FrogObject *name;
	long *res;
	long pos;
	ast *wa = w->waiting;
	w->waiting = NULL;

	switch(wa->type)
	{
	case AST_VAR:
		name = wa->childs[0];

		if(spe)
		{
			create_setfunc(w, name, spe);
		}
		else if(tree)
		{
			ast2bc0(tree->childs[2], w);
			create_setfunc(w, name, spe);
		}
		else
		{
			res = get_hashmap(w->func, name);

			if(!res)
			{
				res = get_hashmap(w->file, name);

				if(!res)
				{
					pos = insert_varmap(w->file, name);	
				}
				else
				{
					pos = *res;
				}

				a2b_addins(w, CODE_GETFILE);
				a2b_addins(w, pos);
				break;
			}
			else
			{
				pos = *res;
			}

			a2b_addins(w, CODE_GETFUNC);
			a2b_addins(w, pos);
		}
	break;
	
	case AST_SUBVAR:
		if(spe)
		{
			a2b_addins(w, CODE_NSETSUB);
			a2b_addins(w, (long) wa->childs[1]);
		}
		else if(!tree)
		{
			a2b_addins(w, CODE_GETSUB);
			a2b_addins(w, (long) wa->childs[1]);
		}
		else
		{
			a2b_addins(w, CODE_PUSH);
			ast2bc0(tree->childs[2], w);
			a2b_addins(w, CODE_SETSUB);
			a2b_addins(w, (long) wa->childs[1]);
		}
	break;
	
	case AST_SUBARR:
		if(spe)
		{
			a2b_addins(w, CODE_NSETSEQ);
		}
		if(!tree)
		{
			a2b_addins(w, CODE_GETSEQ);
		}
		else
		{
			a2b_addins(w, CODE_PUSH);
			ast2bc0(tree->childs[2], w);
			a2b_addins(w, CODE_SETSEQ);
		}
	break;
	}

	if(tree)
		a2b_addins(w, (long) tree->childs[0]);
	if(spe)
		a2b_addins(w, 0);

	free(wa);
}

static void set_wait(worker *w, ast *waiting)
{
	if(w->waiting)
	{
		consume_wait(w, NULL, 0);
	}

	w->waiting = waiting;
}

static workstk *worker_push(workstk *cur, long p)
{
	workstk *tmp = malloc(sizeof(workstk));

	if(!tmp)
	{
		errx(-1, "fatal error: memory");
	}

	tmp->pos = p;
	tmp->next = cur;
	return tmp;
}

static void loop_start(worker *w)
{
	w->lstack = worker_push(w->lstack, -1);
	w->loop_start = worker_push(w->loop_start, w->size);
}

static void loop_end(worker *w, long pos)
{
	w->loop_start = w->loop_start->next;

	workstk *tmp;
	w->ins[pos] = w->size;

	while(w->lstack && w->lstack->pos != -1)
	{
		w->ins[w->lstack->pos] = w->size;

		tmp = w->lstack->next;
		free(w->lstack);
		w->lstack = tmp;
	}

	if(w->lstack)
	{
		tmp = w->lstack->next;
		free(w->lstack);
		w->lstack = tmp;
	}
}

static FrogObject *ast2func(worker *w, ast *tree, FrogObject **name)
{
	worker *fw = malloc(sizeof(worker));

	if(!fw)
	{
		errx(-1, "fatal error: memory");
	}

	fw->loop_start = NULL;
	fw->lstack = NULL;
	fw->file = w->file;
	fw->func = create_hashmap();
	fw->waiting = NULL;
	fw->ins = NULL;
	fw->size = 0;
	fw->capacity = 0;
	fw->module = w->module;
	fw->yield = 0;

	FrogObject **args = tree->childs[1];
	size_t pcount = 0;

	while(*args)
	{
		insert_varmap(fw->func, *args);
		args++;
		pcount++;
	}

	if(!ast2bc(tree->childs[2], fw))
	{
		return NULL;
	}

	FrogObject *func = NULL;

	if(!fw->yield)
		func = CreateFunction(tree->childs[0], w->module, fw->func->size,
			fw->ins, fw->size, pcount);
	else
		func = CreateIFunction(tree->childs[0], w->module, fw->func->size,
				fw->ins, fw->size, pcount);

	if(name)
		*name = tree->childs[0];

	free_hashmap(fw->func);
	free(fw);

	if(!func)
		return NULL;

	return func;
}

static int ast2bc0(ast *tree, worker *w)
{
	size_t pcount, pcount2;
	long pos, pos2;
	void **args;
	FrogObject *func, *name;
	hashmap *map;
	ast *tmp;

	switch(tree->type)
	{
	case AST_CONSTANT:
		a2b_addins(w, CODE_CONSTANT);
		a2b_addins(w, (long) tree->childs[0]);
	break;
	case AST_VAR:
		set_wait(w, tree);
		return 1;
	break;
	case AST_SUBARR:
		if(!ast2bc0(tree->childs[0], w))
			goto error;

		a2b_addins(w, CODE_PUSH);

		if(!ast2bc0(tree->childs[1], w))
			goto error;

		set_wait(w, tree);
		return 1;
	break;
	case AST_SUBVAR:
		if(!ast2bc0(tree->childs[0], w))
			goto error;

		set_wait(w, tree);
		return 1;
	break;
	case AST_ASSIGN:
		tmp = tree->childs[1];
		if(tmp->type == AST_TUPLE)
		{
			if(tree->childs[0])
			{
				FrogErr_Post("SyntaxError", "Bad operator with tuple");
				goto error;
			}

			if(!ast2bc0(tree->childs[2], w))
				goto error;

			args = tmp->childs[0];
			pcount = 0;
			while(args[pcount]) pcount++;

			a2b_addins(w, CODE_EXPLODE);
			a2b_addins(w, pcount);
			pcount2 = 0;

			while(pcount2 < pcount)
			{
				if(!ast2bc0(args[pcount2], w) || !w->waiting)
					goto error;

				consume_wait(w, NULL, 1);
				pcount2++;
			}

			break;
		}

		if(!ast2bc0(tree->childs[1], w))
			goto error;

		if(!w->waiting)
		{
			goto error; //FIXME error
		}

		consume_wait(w, tree, 0);
	break;
	case AST_KVAR:
		a2b_addins(w, CODE_GETFUNC);
		a2b_addins(w, (long) tree->childs[0]);
		a2b_addins(w, CODE_OP1);
		a2b_addins(w, (long) FrogCall_IterNext);
	break;
	case AST_OP1:
		if(!ast2bc0(tree->childs[1], w))
			goto error;

		a2b_addins(w, CODE_OP1);
		a2b_addins(w, (long) tree->childs[0]);
	break;
	case AST_OP2:
		if(!ast2bc0(tree->childs[1], w))
			goto error;

		a2b_addins(w, CODE_PUSH);
		
		if(!ast2bc0(tree->childs[2], w))
			goto error;

		a2b_addins(w, CODE_OP2);
		a2b_addins(w, (long) tree->childs[0]);
	break;
	case AST_EMPTY:
		free(tree);
		return 1;
	case AST_IF:
		if(!ast2bc0(tree->childs[0], w))
			goto error;
		
		a2b_addins(w, CODE_BRAIFN);
		pos = a2b_addins(w, 0);

		if(!ast2bc0(tree->childs[1], w))
			goto error;

		if(tree->childs[2])
		{
			pos2 = a2b_addins(w, CODE_GOTO);
			w->ins[pos] = pos2 + 2;

			pos = a2b_addins(w, 0);
			if(!ast2bc0(tree->childs[2], w))
				goto error;
		}

		w->ins[pos] = w->size;
	break;
	case AST_WHILE:
		loop_start(w);
	
		if(!ast2bc0(tree->childs[0], w))
			goto error;

		a2b_addins(w, CODE_BRAIFN);
		pos = a2b_addins(w, 0);
		
		if(!ast2bc0(tree->childs[1], w))
			goto error;

		a2b_addins(w, CODE_GOTO);
		a2b_addins(w, w->loop_start->pos);

		loop_end(w, pos);
	break;
	case AST_FOREACH:
		pos2 = w->func->size++; // add ghost variable

		if(!ast2bc0(tree->childs[1], w))
			goto error;

		a2b_addins(w, CODE_OP1);
		a2b_addins(w, (long) FrogCall_IterInit);
		a2b_addins(w, CODE_SETFUNC);
		a2b_addins(w, pos2);
		a2b_addins(w, 0);

		tmp = ast_create1(AST_KVAR, (void *) pos2);
		if(!tmp) goto error;

		tmp = ast_assign(NULL, tree->childs[0], tmp);
		if(!tmp) goto error;

		loop_start(w);

		a2b_addins(w, CODE_GETFUNC);
		a2b_addins(w, pos2);
		a2b_addins(w, CODE_OP1);
		a2b_addins(w, (long) FrogCall_IterHasNext);
		a2b_addins(w, CODE_BRAIFN);
		pos = a2b_addins(w, 0);

		if(!ast2bc0(tmp, w) || !ast2bc0(tree->childs[2], w))
			goto error;

		a2b_addins(w, CODE_GOTO);
		a2b_addins(w, w->loop_start->pos);

		loop_end(w, pos);
	break;
	case AST_NEXT:
		if(!ast2bc0(tree->childs[0], w))
			goto error;

		if(!ast2bc0(tree->childs[1], w))
			goto error;
	break;
	case AST_CONTINUE:
		if(!w->loop_start)
		{
			goto error; //FIXME
		}

		if(!ast2bc0(tree->childs[0], w))
			goto error;

		a2b_addins(w, CODE_GOTO);
		a2b_addins(w, w->loop_start->pos);
	break;
	case AST_BREAK:
		if(!w->loop_start)
		{
			goto error; //FIXME
		}

		if(!ast2bc0(tree->childs[0], w))
			goto error;

		a2b_addins(w, CODE_GOTO);
		w->lstack = worker_push(w->lstack, a2b_addins(w, 0));
	break;
	case AST_RETURN:
		if(!ast2bc0(tree->childs[0], w))
			goto error;

		a2b_addins(w, CODE_RETURN);
	break;
	case AST_FUNC:
		func = ast2func(w, tree, NULL);
		a2b_addins(w, CODE_CONSTANT);
		a2b_addins(w, (long) func);

		if(tree->childs[0])
		{
			create_setfunc(w, tree->childs[0], 0);
			a2b_addins(w, 0);
		}
	break;
	case AST_CALL:
		pcount = 0;
		args = tree->childs[1];

		while(args[pcount])
		{
			ast2bc0(args[pcount++], w);
			a2b_addins(w, CODE_PUSH);
		}

		ast2bc0(tree->childs[0], w);
		a2b_addins(w, CODE_CALL);
		a2b_addins(w, pcount);
	break;
	case AST_CLASS:
		map = create_hashmap();
		if(!map) goto error;

		args = tree->childs[2];

		while(*args)
		{
			func = ast2func(w, *args, &name);
			
			if(!func || !name)
			{
				free_hashmap(map);
				goto error;
			}

			put_hashmap(map, name, func);
			args++;
		}

		func = CreateClass(w->module, tree->childs[0], tree->childs[1], map);

		a2b_addins(w, CODE_CONSTANT);
		a2b_addins(w, (long) func);

		if(tree->childs[0])
		{
			create_setfunc(w, tree->childs[0], 0);
			a2b_addins(w, 0);
		}
	break;
	case AST_TUPLE:
		args = tree->childs[0];
		pcount = 0;

		while(args[pcount])
		{
			ast2bc0(args[pcount], w);
			a2b_addins(w, CODE_PUSH);
			pcount++;
		}

		a2b_addins(w, CODE_CONSTANT);
		a2b_addins(w, (long) function_create_tuple); //FIXME method
		a2b_addins(w, CODE_CALL);
		a2b_addins(w, pcount);
	break;
	case AST_YIELD:
		if(w->yield == -1)
			goto error;

		if(!ast2bc0(tree->childs[0], w))
			goto error;

		w->yield = 1;
		a2b_addins(w, CODE_YIELD);
	break;
	}

	free(tree);
	return 1;

error:
	free(tree);
	return 0;
}

int ast2bc(ast *tree, worker *w)
{
	if(!ast2bc0(tree, w))
		return 0;

	if(w->waiting)
		consume_wait(w, NULL, 0);

	return 1;
}
