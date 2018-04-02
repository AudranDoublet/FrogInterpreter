#define CODE_SETFUNC	0
#define CODE_GETFUNC	1
#define CODE_SETFILE	2
#define CODE_GETFILE	3
#define CODE_

#define CODE_ADD	0x0100
#define CODE_

#define MAX 1024

#define ISEMPTY(l) (l->pos == 0)
#define ISFULL(l) (l->pos == MAX - 1)

struct stackp {
	struct stackp  *prev;
	long 		value[MAX];
	size_t 		pos;
};

typedef struct {
	struct stackp  *last;
	size_t 		size;
} stack;
