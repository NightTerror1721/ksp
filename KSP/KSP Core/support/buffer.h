#ifndef KSP_SUPPORT_BUFFER_H
#define KSP_SUPPORT_BUFFER_H

#include <stdlib.h>

typedef struct
{
	char* data;
	size_t nalloc;
	size_t len;
} Buffer;

Buffer* buffer_init(Buffer* b);

Buffer* buffer_new();

char* buffer_data(Buffer* b);

size_t buffer_len(const Buffer* b);

Buffer* buffer_write(Buffer* b, char c);
Buffer* buffer_append(Buffer* b, const char* s, const size_t size);
Buffer* buffer_printf(Buffer* b, const char* fmt, ...);

char* vformat(const char* fmt, va_list ap);
char* format(const char* fmt, ...);

char* quote_cstring(const char* p);
char* quote_cstring_len(const char* p, const size_t len);
char* quote_char(const char c);


#endif


