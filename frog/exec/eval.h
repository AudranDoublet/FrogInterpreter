// Getters and setters

#define CODE_CONSTANT	0x0000

#define CODE_SETFUNC	0x0001
#define CODE_GETFUNC	0x0002
#define CODE_SETFILE	0x0003
#define CODE_GETFILE	0x0004
#define CODE_GETSUB	0x0005
#define CODE_SETSUB	0x0006
#define CODE_GETARR	0x0007
#define CODE_SETARR	0x0008

// Stack

#define	CODE_PUSH	0x0010
#define CODE_POP	0x0011

// Operators

#define CODE_ADD	0x0100
#define CODE_SUB	0x0101
#define CODE_MUL	0x0102
#define CODE_DIV	0x0103
#define CODE_DIVF	0x0104
#define CODE_MOD	0x0105
#define CODE_POW	0x0106
#define CODE_NOT	0x0107
#define	CODE_BWLSF	0x0108
#define	CODE_BWRSF	0x0109
#define CODE_BWOR	0x010a
#define CODE_BWAND	0x010b
#define CODE_BWXOR	0x010c
#define CODE_BWNOT	0x010d

// Inplace Operators

#define CODE_IADD	0x0200
#define CODE_ISUB	0x0201
#define CODE_IMUL	0x0202
#define CODE_IDIV	0x0203
#define CODE_IDIVF	0x0204
#define CODE_IMOD	0x0205
#define CODE_IPOW	0x0206
#define CODE_INOT	0x0207
#define	CODE_IBWLSF	0x0208
#define	CODE_IBWRSF	0x0209
#define CODE_IBWOR	0x020a
#define CODE_IBWAND	0x020b
#define CODE_IBWXOR	0x020c
#define CODE_IBWNOT	0x020d

// Branchment

#define	CODE_GOTO	0x0300
#define CODE_BRAIF	0x0301
#define CODE_BRAIFN	0x0302

#define CODE_CALL	0x0310
#define CODE_RETURN	0x0311

// Stack implementation

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
