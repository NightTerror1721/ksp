#include "error.h"

/* PRIVATE */

typedef struct
{
	int code;
	char msg[256];
} Error ;

static struct
{
	Error head;
	Error tail;
	size_t size;
} _errbuf;


/* PUBLIC */

void error_push(const char* msg);

BOOL error_has();

size_t error_pop(char* buf, const size_t buf_len);

size_t error_clear();
