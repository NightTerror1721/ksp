#include "buffer.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "ctypes.h"

#define INIT_SIZE 8


/* PRIVATE FUNCTIONS */

static void realloc_data(Buffer* b)
{
	size_t newsize = b->nalloc * 2;
	char* data = (char*)malloc(newsize);
	if (data)
	{
		memcpy(data, b->data, b->len);
		free(b->data);
		b->data = data;
		b->nalloc = newsize;
	}
}
#define NEED_REALLOC(_Buff, _Amount) if((_Buff)->nalloc <= (_Buff)->len + (_Amount)) realloc_data((_Buff))

static char* quote(char c)
{
	switch (c)
	{
		case '\"': return "\\\"";
		case '\\': return "\\\\";
		case '\b': return "\\b";
		case '\f': return "\\f";
		case '\n': return "\\n";
		case '\r': return "\\r";
		case '\t': return "\\t";
	}
	return NULL;
}

static void print(Buffer* b, char c)
{
	char* q = quote(c);
	if (q)
		buffer_printf(b, "%s", q);
	else if (isprint(c))
		buffer_printf(b, "%c", c);
	else
		buffer_printf(b, "\\x%02x", c);
}

#define buffer_local_new(_Varname) Buffer _Varname; buffer_init(&_Varname)



/* PUBLIC FUNCTIONS */

Buffer* buffer_init(Buffer* b)
{
	if (b)
	{
		b->data = (char*)malloc(INIT_SIZE);
		if (b->data)
		{
			b->nalloc = INIT_SIZE;
			b->len = 0;
		}
	}
	return b;
}

Buffer* buffer_new() { return buffer_init((Buffer*)malloc(sizeof(Buffer))); }

char* buffer_data(Buffer* b) { return b->data; }

size_t buffer_len(const Buffer* b) { return b->len; }

Buffer* buffer_write(Buffer* b, char c)
{
	NEED_REALLOC(b, 1);
	b->data[b->len++] = c;
	return b;
}
Buffer* buffer_append(Buffer* b, const char* s, const size_t size)
{
	NEED_REALLOC(b, size);
	for (size_t i = 0; i < size; ++i)
		b->data[b->len++] = s[i];
	return b;
}
Buffer* buffer_printf(Buffer* b, const char* fmt, ...)
{
	va_list args;
	for (;;)
	{
		int avail = b->nalloc - b->len;
		va_start(args, fmt);
		int written = vsnprintf_s(b->data + b->len, avail, _TRUNCATE, fmt, args);
		va_end(args);
		if (avail <= written)
		{
			realloc_data(b);
			continue;
		}
		b->len += written;
		break;
	}
}


char* vformat(const char* fmt, va_list ap)
{
	buffer_local_new(b);
	va_list aq;
	for (;;)
	{
		int avail = b.nalloc - b.len;
		va_copy(aq, ap);
		int written = vsnprintf_s(b.data + b.len, avail, _TRUNCATE, fmt, aq);
		va_end(aq);
		if (avail <= written)
		{
			realloc_data(&b);
			continue;
		}
		b.len += written;
		break;
	}
	return b.data;
}
char* format(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	char* s = vformat(fmt, ap);
	va_end(ap);
	return s;
}

char* quote_cstring(const char* p)
{
	buffer_local_new(b);
	while (*p)
		print(&b, *p++);
	return b.data;
}
char* quote_cstring_len(const char* p, const size_t len)
{
	buffer_local_new(b);
	for (size_t i = 0; i < len; ++i)
		print(&b, p[i]);
	return b.data;
}
char* quote_char(const char c)
{
	switch (c)
	{
		case '\\': return "\\\\";
		case '\'': return "\\\'";
		default: return format("%c", c);
	}
}
