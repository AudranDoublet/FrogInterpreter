#ifndef __STRUTILS_H__
#define __STRUTILS_H__

#define BASEN "0123456789abcdef" // base 2 to 16
#define BASE32 "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567" // base 32
#define BASE64 "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/" // base 64

/**
 * represent a string builder
 * to see how you can use it, see above functions
 * a string builder can be used to concatenates string more
 * efficiencly
 */
struct strbuilder
{
	size_t length;
	void *first;
	void *last;
	void *reserve;
};

/**
 * create an new strbuilder
 */
struct strbuilder *create_strbuilder(void);

/**
 * clear the strbuilder to use it another time without
 * reallocating memory
 */
void clear_strbuilder(struct strbuilder *builder);

/**
 * add the given string to the builder
 */
void append_strbuilder(struct strbuilder *builder, fchar *str);

/**
 * add the given character to the builder
 */
void add_strbuilder(struct strbuilder *builder, fchar chr);

/**
 * add the given integer in base 'base' to the builder
 * return 0 if the function fail, 1 otherwise
 */
int addint_strbuilder(struct strbuilder *builder, long value, int base);

/**
 * get the current value in a unique string
 */
fchar *tostr_strbuilder(struct strbuilder *builder);

/**
 * free the given builder
 */
void free_strbuilder(struct strbuilder *builder);
#endif
