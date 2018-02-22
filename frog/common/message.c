#include <stdlib.h>
#include "message.h"

int throwmessage(messagelistener listener, int type, char *text)
{
	int r;
	struct message *msg = malloc(sizeof(struct message));

	if(msg)
	{
		msg->type = type;
		msg->text = text;

		r = (*listener)(msg);
		free(msg);
	
		return r;
	}

	return 0;
}
