#include "../frog.h"

#define OFFSET		14695981039346656037U
#define FNV_PRIME 	1099511628211

#define HASH(x)		(hash = (hash ^ (x)) * FNV_PRIME)

hashmap *modules = NULL;
hashmap *functions = NULL;
FrogObject **function_mem = NULL;
FrogType module_type;

FrogObject *function_create_list;
FrogObject *function_create_map;
FrogObject *function_create_tuple;

static FrogObject *add_bifunc(char *name, builtinfunc fc, ssize_t param)
{
	FrogObject *nm = utf8tostr(name);

	if(!nm)
	{
		errx(-1, "fatal error: memory");
	}

	FrogObject *func = CreateBIFunction(nm, NULL, fc, param);	

	if(!func)
	{
		errx(-1, "fatal error: memory");
	}

	size_t pos = functions->size;
	size_t *p = malloc(sizeof(size_t));

	if(!p) errx(-1, "fatal error: memory");

	*p = pos;

	put_hashmap(functions, nm, p);
	function_mem[pos] = func;

	return func;	
}

void init_builtin_functions(void)
{
	modules = create_hashmap();
	functions = create_hashmap();

	if(!modules || !functions) errx(-1, "fatal error: memory");
	size_t count = 30;

	function_mem = calloc(sizeof(FrogObject *), count);
	if(!function_mem) errx(-1, "fatal error: memory");

	add_bifunc("print", global_print, -1);
	add_bifunc("max", global_max, -1);
	add_bifunc("min", global_min, -1);
	add_bifunc("len", global_len, 1);
	add_bifunc("str", global_str, 1);
	add_bifunc("id", global_id, 1);
	add_bifunc("include", global_include, 1);
	add_bifunc("native", global_native, 1);
	add_bifunc("bla", global_import, 1);
	add_bifunc("import", global_import, 1);
	add_bifunc("range", global_range, -1);
	function_create_list = add_bifunc("list", global_list, -1);
	function_create_map = add_bifunc("dict", global_dict, -1);
	function_create_tuple = add_bifunc("tuple", global_tuple, -1);
}

long module_hash(FrogObject *o)
{
	FrogModule *f = (FrogModule *) o;

	if(f->hash != -1)
		return f->hash;

	long hash = (long) OFFSET;

	if(f->name)
		HASH( str_hash(f->name) );

	for(size_t i = 0; i < f->len; i++)
		HASH(f->ins[i]);

	//FIXME use hashmap

	f->hash = hash;
	return hash;
}

void module_print(FrogObject *o, FILE *f)
{
	FrogModule *fu = (FrogModule *) o;

	fprintf(f, "<module ");
	Frog_Print(fu->name, f);
	fprintf(f, ">");
}

FrogObject *module_tostr(FrogObject *o)
{
	UNUSED(o);
	return utf8tostr("<module>"); //FIXME
}

void module_free(FrogObject *o)
{
	FrogModule *func = (FrogModule *) o;

	free(func->ins);
	Frog_Free(func->name);
	free_hashmap(func->name2var);
	free(func->memory);
	free(func);
}

FrogObject *EmptyModule(char *name)
{
	FrogModule *m = malloc(sizeof(FrogModule));

	if(!m)
	{
		FrogErr_Memory();
		return NULL;
	}

	m->name = utf8tostr(name);
	m->len = 0;
	m->ins = NULL;
	m->memory = malloc(sizeof(FrogObject *) * functions->size);

	if(!m->memory)
	{
		Frog_Free(m->name);
		free(m);
		FrogErr_Memory();
		return NULL;
	}

	memcpy(m->memory, function_mem, sizeof(FrogObject *) * functions->size);
	m->name2var = copy_hashmap(functions);

	if(!m->name2var)
	{
		Frog_Free(m->name);
		free(m);
		FrogErr_Memory();
		return NULL;
	}

	m->hash = -1;
	ObType(m) = &module_type;
	return (FrogObject *) m;
}

int ModuleExec(FrogObject *o, stack *s)
{
	FrogModule *m = (FrogModule *) o;

	if(m->len == 0) return 1;

	size_t len = m->len;
	m->len = 0;

	int r = execute(s, m->memory, m->memory, m->ins, len, NULL) != NULL;	
	free(m->ins);

	return r;
}

static fchar *nextline_file(tokenizer *tkz)
{
	char *str = NULL;
	size_t len = 0;
	ssize_t read = -1;

	if((read = getline(&str, &len, tkz->file)) != -1)
	{
		FrogString *o = (FrogString *) utf8tostr(str);

		if(!o)
		{
			return NULL;
		}

		fchar *res = o->str;
		free(o);
		free(str);

		return res;
	}
	else
	{
		return NULL;
	}
}

FrogObject *LoadModule(char *name, FILE *input)
{
	FrogObject *nameo = utf8tostr(name);
	FrogObject *res = get_hashmap(modules, nameo);

	if(res) return res;

	FrogModule *module = (FrogModule *) EmptyModule(name);	
	tokenizer *tkz = create_tokenizer(nextline_file, input);
	worker *w = malloc(sizeof(worker));

	if(!module || !tkz || !w)
	{
		goto merror;
	}

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

	while(!tkz->eof)
	{
		ast *v = parse_instruction(tkz);

		if(!v)
		{
			FrogErr_Syntax(tkz->linenb, tkz->colnb);
			return NULL;
		}

		if(!ast2bc(v, w))
		{
			FrogErr_Post("Syntax error", "compiling");
			return NULL;
		}
	}

	module->memory = realloc(module->memory, w->file->size * sizeof(FrogObject *));
	module->ins = w->ins;
	module->len = w->size;


	if(!module->memory) goto merror;
	put_hashmap(modules, nameo, module);

	return (FrogObject *) module;

merror:	FrogErr_Memory();
	if(module) Frog_Free((FrogObject *) module);
	if(tkz) free_tokenizer(tkz);
	if(w) free(w);

	return NULL;
}

FrogObject *module_getter(FrogObject *o, FrogObject *var)
{
	FrogModule *module = (FrogModule *) o;
	long *res = get_hashmap(module->name2var, var);
	FrogObject *obj = NULL;


	if(!res || !(obj = module->memory[*res]))
	{
		FrogErr_SubName(o, var);
		return NULL;
	}

	return obj;
}

FrogObject *module_setter(FrogObject *o, FrogObject *var, FrogObject *value,
				binaryfunction operator)
{
	FrogModule *module = (FrogModule *) o;
	long *res = get_hashmap(module->name2var, var);

	if(!res)
	{
		res = malloc(sizeof(long));

		if(!res)
		{
			FrogErr_Memory();
			return NULL;
		}

		*res = module->name2var->size;
		put_hashmap(module->name2var, var, res);

		module->memory = realloc(module->memory, module->name2var->size
					* sizeof(FrogObject *));

		if(!module->memory)
		{
			errx(-1, "Fatal error: memory");
		}
	}

	if(operator)
	{
		if(!module->memory[*res])
			module->memory[*res] = FrogNone();

		value = operator(module->memory[*res], value);
		if(!value) return NULL;
	}

	module->memory[*res] = value;
	return value;
}

FrogType module_type =
{
	{
		-1,
		NULL//FIXME
	},
	"module",
	module_getter,
	module_setter,
	module_hash,
	NULL,
	module_print,
	module_tostr,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	module_free
};
