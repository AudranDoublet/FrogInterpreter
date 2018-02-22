#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#define MESSAGE_INFO		0
#define MESSAGE_WARNING		1
#define MESSAGE_ERROR		2

struct message
{
	int type;
	char *text;
};

/**
 * Should be called with a message. The return value is not
 * defined (depending on the used implementation).
 */
typedef int (*messagelistener)(struct message*);

/**
 * Create a "struct message" and call the provided listener
 * Return 0 if there is a problem while allocating the message
 * otherwise return the result of the listener
 */
int throwmessage(messagelistener listener, int type, char *text);
#endif
