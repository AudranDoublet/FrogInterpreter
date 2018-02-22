#include <stdlib.h>
#include <string.h>
#include "../frog.h"

#define SIZE_NODE_STRBUILDER 	64

/**
 * utility used for this implementation of strbuilder
 */
struct buildernode
{
	fchar string[SIZE_NODE_STRBUILDER];
	size_t length;

	struct buildernode *next;
};

/**
 * create a buildernode with the provided prev value
 */
static struct buildernode *create_node(struct buildernode *prev)
{
	struct buildernode *r = calloc(sizeof(struct buildernode), 1);
	if(prev)
		prev->next = r;

	return r;
}

/**
 * clear a buildernode
 */
static void clear_node(struct buildernode *node)
{
	memset(node, 0, sizeof(struct buildernode));
}

/**
 * free a buildernode
 */
static void free_node(struct buildernode *curr)
{
	struct buildernode *next = NULL;

	while(curr)
	{
		next = curr->next;
		free(curr);
		curr = next;
	}
}

struct strbuilder *create_strbuilder(void)
{
	struct strbuilder *r = malloc(sizeof(struct strbuilder));

	if(!r)
	{
		return NULL;
	}

	r->length = 0;
	r->last = create_node(NULL);
	r->first = r->last;
	r->reserve = NULL;

	return r;
}

void clear_strbuilder(struct strbuilder *builder)
{
	struct buildernode *last = builder->last;
	struct buildernode *first = builder->first;

	if(last != first)
	{
		if(builder->reserve)
			last->next = builder->reserve;
		builder->reserve = first->next;
	}

	builder->last = first;
	clear_node(first);
}

void add_strbuilder(struct strbuilder *builder, fchar c)
{
	struct buildernode *curr = builder->last;

	if(curr->length == SIZE_NODE_STRBUILDER)
	{
		curr = create_node(curr);
		builder->last = curr;
	}

	curr->string[curr->length++] = c;
	builder->length++;
}

void append_strbuilder(struct strbuilder *builder, fchar *str)
{
	for( ; *str ; str++ )
	{
		add_strbuilder(builder, *str);
	}
}

int addint_strbuilder(struct strbuilder *builder, long value, int base)
{
	char *digits;
	fchar buffer[64]; // maximal size to express a long as int
	size_t i = 0;

	if(base >= 2 && base <= 16)
	{
		digits = BASEN;
	}
	else if(base == 32)
	{
		digits = BASE32;
	}
	else if(base == 64)
	{
		digits = BASE64;
	}
	else
	{
		goto error;
	}

	if(value < 0)
	{
		value = -value;
		add_strbuilder(builder, '-');
	}

	do
	{
		buffer[i++] = digits[value % base];
		value /= base;
	} while(value > 0);


	for(; i > 0; i--)
		add_strbuilder(builder, buffer[i - 1]);

	return 1;
error:
	return 0;
}

fchar *tostr_strbuilder(struct strbuilder *builder)
{
	// allocate the memory which will be used to store the
	// result: length + 1 (for the final fcharacter \0
	fchar *str = malloc(sizeof(fchar) * (builder->length + 1));
	fchar *cur = str;
	struct buildernode *curr = builder->first;

	for( ; curr ; cur += curr->length, curr = curr->next )
	{
		memcpy(cur, curr->string, curr->length * sizeof(fchar));
	}

	str[builder->length] = L'\0';

	return str;
}

void free_strbuilder(struct strbuilder *builder)
{
	if(!builder)
		return;

	free_node(builder->first);
	free_node(builder->reserve);

	free(builder);
}
