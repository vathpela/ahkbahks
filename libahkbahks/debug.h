/*
 * debug.h
 * Copyright 2019 Peter Jones <pjones@redhat.com>
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#include <stdarg.h>

extern void debug_ctor(void);
extern void debug_dtor(void);

extern void ahkbahks_vwarn(const char * const fmt, va_list ap);
extern void ahkbahks_vwarnx(const char * const fmt, va_list ap);
extern void ahkbahks_warn(const char * const fmt, ...);
extern void ahkbahks_warnx(const char * const fmt, ...);

#define vwarn(fmt, ap) ahkbahks_vwarn(fmt, ap)
#define vwarnx(fmt, ap) ahkbahks_vwarnx(fmt, ap)
#define warn(fmt, args...) ahkbahks_warn(fmt, ## args)
#define warnx(fmt, args...) ahkbahks_warnx(fmt, ## args)

extern void ahkbahks_vdebug(const char * const file, const int line,
			    const char * const function,
			    const char * const fmt, va_list ap);
extern void ahkbahks_vdebugx(const char * const file, const int line,
			     const char * const function,
			     const char * const fmt, va_list ap);
extern void ahkbahks_debug(const char * const file, const int line,
			   const char * const function,
			   const char * const fmt, ...);
extern void ahkbahks_debugx(const char * const file, const int line,
			    const char * const function,
			    const char * const fmt, ...);

#define vdebug(fmt, ap) ahkbahks_vdebug(__FILE__, __LINE__, __PRETTY_FUNCTION__, fmt, ap)
#define vdebugx(fmt, ap) ahkbahks_vdebugx(__FILE__, __LINE__, __PRETTY_FUNCTION__, fmt, ap)
#define debug(fmt, args...) ahkbahks_debug(__FILE__, __LINE__, __PRETTY_FUNCTION__, fmt, ## args)
#define debugx(fmt, args...) ahkbahks_debugx(__FILE__, __LINE__, __PRETTY_FUNCTION__, fmt, ## args)

extern void ahkbahks_verr(int ret, const char * const fmt, va_list ap);
extern void ahkbahks_verrx(int ret, const char * const fmt, va_list ap);
extern void ahkbahks_err(int ret, const char * const fmt, ...);
extern void ahkbahks_errx(int ret, const char * const fmt, ...);

#define verr(ret, fmt, ap) ahkbahks_verr(ret, fmt, ap)
#define verrx(ret, fmt, ap) ahkbahks_verrx(ret, fmt, ap)
#define err(ret, fmt, args...) ahkbahks_err(ret, fmt, ## args)
#define errx(ret, fmt, args...) ahkbahks_errx(ret, fmt, ## args)

#endif /* !DEBUG_H_ */
// vim:fenc=utf-8:tw=75:noet
