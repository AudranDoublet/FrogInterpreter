typedef struct workstk workstk;

struct workstk {
	long pos;
	workstk *next;
};

typedef struct {
	long *ins;
	long size;
	long capacity;

	workstk *loop_start;
	workstk *lstack;

	hashmap *file, *func;

	ast *waiting;
	FrogObject *module;

	int yield;
} worker;
 
int ast2bc(ast *tree, worker *w);
