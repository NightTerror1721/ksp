#ifndef KSP_SUPPORT_ERROR_H
#define KSP_SUPPORT_ERROR_H

#include "ctypes.h"

void error_push(const char* msg);

BOOL error_has();

size_t error_pop(char* buf, const size_t buf_len);

size_t error_clear();

#endif
