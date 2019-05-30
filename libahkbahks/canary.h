/*
 * canary.h
 * Copyright 2019 Peter Jones <pjones@redhat.com>
 */

#ifndef CANARY_H_
#define CANARY_H_

#include <sys/random.h>

typedef int ahkbahks_canary_t;

#define CANARY(name) CAT(name,_ahkbahks_canary_)
#define CANARY_(name, line, count)					\
	const ahkbahks_canary_t USED PUBLIC				\
		SECTION(".bss.ahkbahks.canary")				\
		CANARY(name);						\
	const char USED PUBLIC SECTION(".ahkbahks.canary")		\
		CAT(CANARY(name),marker_)[]				\
		ALIGNED(1)						\
		=							\
			__FILE__ "\000"					\
			STRING(__LINE__) "\000"				\
			STRING(count) "\000"				\
			STRING(name) "\000"				\
			STRING(CANARY(name)) "\000";

//	asm(".pushsection .data.ahkbahks.canary,\"aS\"\n");
//	asm(".popsection\n")

#define DECLARE_CANARY(name) CANARY_(name, __LINE__, __COUNTER__)

extern ahkbahks_canary_t *ahkbahks_canaries;
extern size_t ahkbahks_canary_entries;

extern void canaries_ctor(void);

#endif /* !CANARY_H_ */
// vim:fenc=utf-8:tw=75:noet
