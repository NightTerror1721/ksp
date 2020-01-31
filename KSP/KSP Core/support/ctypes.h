#ifndef KSP_SUPPORT_CTYPES_H
#define KSP_SUPPORT_CTYPES_H

#include <stdint.h>

typedef enum {
	FALSE = 0,
	TRUE  = 1
} BOOL;

#define BOOL_TEST(_Expr) (((BOOL) (_Expr)) != FALSE)

#endif
