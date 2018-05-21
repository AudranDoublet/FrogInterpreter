// Getters and setters

#define CODE_CONSTANT	0x0000

#define CODE_SETFUNC	0x0001
#define CODE_GETFUNC	0x0002
#define CODE_NSETFUNC	0x0003
#define CODE_GETFILE	0x0004
#define CODE_GETSUB	0x0005
#define CODE_SETSUB	0x0006
#define CODE_NSETSUB	0x0007
#define CODE_GETSEQ	0x0008
#define CODE_SETSEQ	0x0009
#define CODE_NSETSEQ	0x000a

// Stack

#define	CODE_PUSH	0x0010
#define CODE_POP	0x0011
#define CODE_EXPLODE	0x0012

// Operators

#define CODE_OP1	0x1000
#define CODE_OP2	0x1001

// Branchment

#define	CODE_GOTO	0x0300
#define CODE_BRAIF	0x0301
#define CODE_BRAIFN	0x0302

#define CODE_CALL	0x0310
#define CODE_RETURN	0x0311
#define CODE_YIELD	0x0312

// Stack implementation

#define MAX 1024

#define ISEMPTY(l) (!l || l->pos == 0)
#define ISFULL(l) (!l || l->pos == MAX - 1)

struct stackp {
	struct stackp  *prev;
	void 	       *value[MAX];
	size_t 		pos;
};

struct stack {
	struct stackp  *last;
};

typedef struct {
	int end;
	int pos;
} yieldmg;

stack *create_stack(void);

FrogObject *execute(stack *stack,
		FrogObject **filememory,
		FrogObject **funcmemory,
		long *ins,
		size_t inslen,
		yieldmg *yield);
