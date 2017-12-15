#ifndef _STDDEF_H
#define _STDDEF_H 1

#include <sys/cdefs.h>

#include <sys/_types/_null.h>

#define offsetof(st, m)    __builtin_offsetof(st, m)

#include <sys/_types/_ptrdiff_t.h>
#include <sys/_types/_wchar_t.h>
#include <sys/_types/_size_t.h>

#endif
