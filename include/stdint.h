#ifndef _STDINT_H
#define _STDINT_H 1

#include <sys/cdefs.h>

#include <sys/_types/_int8_t.h>
#include <sys/_types/_int16_t.h>
#include <sys/_types/_int32_t.h>
#include <sys/_types/_int64_t.h>

#include <sys/_types/_uint8_t.h>
#include <sys/_types/_uint16_t.h>
#include <sys/_types/_uint32_t.h>
#include <sys/_types/_uint64_t.h>

typedef int8_t int_least8_t;
typedef int16_t int_least16_t;
typedef int32_t int_least32_t;
typedef int64_t int_least64_t;

typedef uint8_t uint_least8_t;
typedef uint16_t uint_least16_t;
typedef uint32_t uint_least32_t;
typedef uint64_t uint_least64_t;

typedef int8_t int_fast8_t;
typedef int16_t int_fast16_t;
typedef int32_t int_fast32_t;
typedef int64_t int_fast64_t;

typedef uint8_t uint_fast8_t;
typedef uint16_t uint_fast16_t;
typedef uint32_t uint_fast32_t;
typedef uint64_t uint_fast64_t;

#include <sys/_types/_intptr_t.h>
#include <sys/_types/_uintptr_t.h>

#include <sys/_types/_intmax_t.h>
#include <sys/_types/_uintmax_t.h>

#define INT8_MIN            -128
#define INT16_MIN           -32768
#define INT32_MIN           (-2147483647-1)
#define INT64_MIN           (-9223372036854775807LL-1)

#define INT8_MAX            127
#define INT16_MAX           32767
#define INT32_MAX           2147483647
#define INT64_MAX           9223372036854775807LL

#define UINT8_MAX           255
#define UINT16_MAX          65535
#define UINT32_MAX          4294967295
#define UINT64_MAX          18446744073709551616LL

#define INT_LEAST8_MIN      INT8_MIN
#define INT_LEAST16_MIN     INT16_MIN
#define INT_LEAST32_MIN     INT32_MIN
#define INT_LEAST64_MIN     INT64_MIN

#define INT_LEAST8_MAX      INT8_MAX
#define INT_LEAST16_MAX     INT16_MAX
#define INT_LEAST32_MAX     INT32_MAX
#define INT_LEAST64_MAX     INT64_MAX

#define UINT_LEAST8_MAX     UINT8_MAX
#define UINT_LEAST16_MAX    UINT16_MAX
#define UINT_LEAST32_MAX    UINT32_MAX
#define UINT_LEAST64_MAX    UINT64_MAX

#define INT_FAST8_MIN       INT8_MIN
#define INT_FAST16_MIN      INT16_MIN
#define INT_FAST32_MIN      INT32_MIN
#define INT_FAST64_MIN      INT64_MIN

#define INT_FAST8_MAX       INT8_MAX
#define INT_FAST16_MAX      INT16_MAX
#define INT_FAST32_MAX      INT32_MAX
#define INT_FAST64_MAX      INT64_MAX

#define UINT_FAST8_MAX      UINT8_MAX
#define UINT_FAST16_MAX     UINT16_MAX
#define UINT_FAST32_MAX     UINT32_MAX
#define UINT_FAST64_MAX     UINT64_MAX

#define INTPTR_MIN          INT64_MIN
#define INTPTR_MAX          INT64_MAX
#define UINTPTR_MAX         UINT64_MAX

#define INTMAX_MIN          INT64_MIN
#define INTMAX_MAX          INT64_MAX
#define UINTMAX_MAX         UINT64_MAX

#define PTRDIFF_MIN         INT64_MIN
#define PTRDIFF_MAX         INT64_MAX

#define SIG_ATOMIC_MIN      INT64_MIN
#define SIG_ATOMIC_MAX      INT64_MAX

#define SIZE_MAX            UINT64_MAX

#define WCHAR_MIN           INT32_MIN
#define WCHAR_MAX           INT32_MAX

#define WINT_MIN            INT32_MIN
#define WINT_MAX            INT32_MAX

#define INT8_C(a)           a
#define INT16_C(a)          a
#define INT32_C(a)          a
#define INT64_C(a)          (a ## LL)

#define UINT8_C(a)          (a ## U)
#define UINT16_C(a)         (a ## U)
#define UINT32_C(a)         (a ## U)
#define UINT64_C(a)         (a ## ULL)

#define INTMAX_C(a)         (a ## LL)
#define UINTMAX_C(a)        (a ## ULL)

#endif
