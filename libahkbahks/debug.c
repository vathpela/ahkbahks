/*
 * debug.c
 * Copyright 2019 Peter Jones <pjones@redhat.com>
 *
 */

#include "ahkbahks.h"

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static int std_err_fileno = -1;
static FILE *std_err;

void
debug_ctor(void)
{
	std_err_fileno = fcntl(STDERR_FILENO, F_DUPFD_CLOEXEC, 3);
	if (std_err_fileno == -1)
		goto err;

	std_err = fdopen(std_err_fileno, "a");
	if (!std_err)
		goto err;

	setvbuf(std_err, NULL, _IONBF, 0);

	return;
err:
	if (std_err != NULL) {
		fclose(std_err);
		std_err = NULL;
	}
	if (std_err_fileno > 0) {
		close(std_err_fileno);
		std_err_fileno = -1;
	}
}

void
debug_dtor(void)
{
	if (std_err)
		fclose(std_err);
	if (std_err_fileno >= 0)
		close(std_err_fileno);
}

static void
ahkbahks_vwarn_common(const char * const fmt, va_list ap, bool show)
{
	int fd;

	if (!std_err)
		return;

	fd = fileno(stdout);
	if (isatty(fd))
		fflush(stdout);
	fprintf(std_err, "%s: ", program_invocation_short_name);
	vfprintf(std_err, fmt, ap);
	fprintf(std_err, show ? ": %m\n" : "\n");
}

void
ahkbahks_vwarn(const char * const fmt, va_list ap)
{
	ahkbahks_vwarn_common(fmt, ap, true);
}

void
ahkbahks_vwarnx(const char * const fmt, va_list ap)
{
	ahkbahks_vwarn_common(fmt, ap, false);
}

void
ahkbahks_warn(const char * const fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	ahkbahks_vwarn_common(fmt, ap, true);
	va_end(ap);
}

void
ahkbahks_warnx(const char * const fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	ahkbahks_vwarn_common(fmt, ap, false);
	va_end(ap);
}

static void
ahkbahks_vdebug_common(const char * const fi, const int line,
		       const char * const fu, const char * const fmt,
		       va_list ap, bool show)
{
	int fd;

	if (!getenv("LIBAHKBAHKS_DEBUG") || !std_err)
		return;

	fd = fileno(stdout);
	if (isatty(fd))
		fflush(stdout);
	fprintf(std_err, "%s:%s:%d:%s(): ",
		program_invocation_short_name, fi, line, fu);
	vfprintf(std_err, fmt, ap);
	fprintf(std_err, show ? ": %m\n" : "\n");
}

void
ahkbahks_vdebug(const char * const fi, const int line, const char * const fu,
		const char * const fmt, va_list ap)
{
	ahkbahks_vdebug_common(fi, line, fu, fmt, ap, true);
}

void
ahkbahks_vdebugx(const char * const fi, const int line, const char * const fu,
		 const char * const fmt, va_list ap)
{
	ahkbahks_vdebug_common(fi, line, fu, fmt, ap, false);
}

void
ahkbahks_debug(const char * const fi, const int line, const char * const fu,
	       const char * const fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	ahkbahks_vdebug_common(fi, line, fu, fmt, ap, true);
	va_end(ap);
}

void
ahkbahks_debugx(const char * const fi, const int line, const char * const fu,
		const char * const fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	ahkbahks_vdebug_common(fi, line, fu, fmt, ap, false);
	va_end(ap);
}


static void NORETURN
ahkbahks_verr_common(int ret, const char * const fmt, va_list ap, bool show)
{
	ahkbahks_vwarn_common(fmt, ap, show);
	exit(ret);
}

void NORETURN
ahkbahks_verr(int ret, const char * const fmt, va_list ap)
{
	ahkbahks_verr_common(ret, fmt, ap, true);
}

void NORETURN
ahkbahks_verrx(int ret, const char * const fmt, va_list ap)
{
	ahkbahks_verr_common(ret, fmt, ap, false);
}

void NORETURN
ahkbahks_err(int ret, const char * const fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	ahkbahks_verr_common(ret, fmt, ap, true);
}

void NORETURN
ahkbahks_errx(int ret, const char * const fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	ahkbahks_verr_common(ret, fmt, ap, false);
}

// vim:fenc=utf-8:tw=75:noet
