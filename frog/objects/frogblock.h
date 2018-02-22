#ifndef __FROG_OBJECTS_BLOCK_H__
#define __FROG_OBJECTS_BLOCK_H__

struct BlockElement {
	FrogObject *val;
	struct BlockElement *next;
};

typedef struct {
	FrogObjHead
	struct BlockElement *first, *last;	
} FrogBlock;

typedef struct {
	FrogObjHead
	struct FrogObject *condition, *thenBlock, *elseBlock;
} FrogCondition;

typedef struct {
	FrogObjHead
	struct FrogObject *condition, *block;
	int dowhile;
} FrogWhile;

typedef struct {
	FrogObjHead
	int type;	
	FrogObject *value;
} FrogBreaker;

typedef struct {
	FrogObjHead
	FrogObject *value;
} FrogPrint;

FrogObject *CreateBlock(void);

FrogObject *CreateCondition(FrogObject *condition, FrogObject *thenBlock, FrogObject *elseBlock);

FrogObject *CreateWhile(int dowhile, FrogObject *condition, FrogObject *block);

FrogObject *CreateBreaker(int type, FrogObject *value);

FrogObject *CreatePrint(FrogObject *value);

int AddBlockElement(FrogObject *block, FrogObject *el);
#endif
