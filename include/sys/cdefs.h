#ifndef _SYS_CDEFS_H_
#define _SYS_CDEFS_H_

#if !defined(__GNUC__)
# define __attribute__(x)
#endif

#if defined(__cplusplus)
# define __BEGIN_DECL   extern "C" {
# define __END_DECL     }
# define __restrict
#else
# define __BEGIN_DECL
# define __END_DECL
# define __restrict restrict
#endif

#endif
