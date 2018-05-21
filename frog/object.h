#ifndef __FROGOBJECTS_H__
#define __FROGOBJECTS_H__

typedef struct FrogObject FrogObject;
typedef struct FrogSObject FrogSObject;

typedef struct FrogType FrogType;

#define Ref(o) if(o) ObRefcnt(o)++
#define Unref(o) if(o) ObRefcnt(o)--

#define FrogObjHead FrogObject ob_base;
#define FrogSObjHead FrogSObject ob_base;

typedef struct stack stack;

typedef FrogObject *(*unaryfunction)(FrogObject *);
typedef FrogObject *(*binaryfunction)(FrogObject *, FrogObject *);
typedef FrogObject *(*ternaryfunction)(FrogObject *, FrogObject *, FrogObject *);
typedef FrogObject *(*setfunction)(FrogObject *, FrogObject *, FrogObject *, binaryfunction);
typedef FrogObject *(*callfunction)(FrogObject *, FrogObject **, size_t, stack *);
typedef FrogObject *(*ncallfunction)(FrogObject *, stack *);
typedef FrogObject *(*sizefunction)(FrogObject *);
typedef int (*intfunction)(FrogObject *);
typedef int (*biintfunction)(FrogObject *, FrogObject *);
typedef long (*longfunction)(FrogObject *);
typedef void (*printfunction)(FrogObject *, FILE *);
typedef void (*objfunction)(FrogObject *);

typedef struct {
	binaryfunction add;
	binaryfunction sub;
	binaryfunction mul;
	binaryfunction div;
	binaryfunction divfloor;
	binaryfunction mod;
	binaryfunction pow;
	unaryfunction  neg;
	unaryfunction  pos;
	unaryfunction  abs;
	unaryfunction inv;
	binaryfunction lshift;
	binaryfunction rshift;
	binaryfunction and;
	binaryfunction or;
	binaryfunction xor;

	binaryfunction inplace_add;
	binaryfunction inplace_sub;
	binaryfunction inplace_mul;
	binaryfunction inplace_div;
	binaryfunction inplace_divfloor;
	binaryfunction inplace_mod;
	binaryfunction inplace_pow;
	binaryfunction inplace_lshift;
	binaryfunction inplace_rshift;
	binaryfunction inplace_and;
	binaryfunction inplace_or;
	binaryfunction inplace_xor;
} FrogAsNumber;

typedef struct {
	unaryfunction init;
	unaryfunction next;
	unaryfunction hasnext;
} FrogAsIterable;

typedef struct {
	setfunction set;
	binaryfunction  get;
} FrogAsSequence;

struct FrogObject {
	ssize_t refcnt;
	FrogType *type;
};

#define ObRefcnt(ob)	(((FrogObject *)ob)->refcnt)
#define ObType(ob)	(((FrogObject *)ob)->type)

struct FrogSObject {
	FrogObjHead
	ssize_t size;
};

#define ObSize(ob)	(((FrogSObject *)ob)->size)

struct FrogType {
	FrogObjHead
	char *name;

	binaryfunction getter;
	setfunction setter;

	longfunction hash;
	sizefunction size;

	printfunction print;
	unaryfunction tostr;
	unaryfunction toint;
	binaryfunction exec;

	biintfunction simple_compare;
	biintfunction complex_compare;

	FrogAsNumber *as_number;
	FrogAsSequence *as_sequence;
	FrogAsIterable *as_iterable;

	callfunction call;
	objfunction free;
};

FrogObject *ob_tostr(FrogObject *ob);

long ob_hash(FrogObject *ob);
int ob_compare(FrogObject *a, FrogObject b);

#endif
