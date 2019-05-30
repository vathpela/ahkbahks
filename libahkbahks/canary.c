/*
 * canary.c
 * Copyright 2019 Peter Jones <pjones@redhat.com>
 *
 */

#include "ahkbahks.h"

#include <errno.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/random.h>
#include <syscall.h>
#include <unistd.h>

DECLARE_CANARY(finalize_canaries);

extern int bss_ahkbahks_canary_start_;
extern int bss_ahkbahks_canary_end_;
extern int bss_ahkbahks_canary_pages_end_;

ahkbahks_canary_t *ahkbahks_canaries = &bss_ahkbahks_canary_start_;
size_t ahkbahks_canary_entries;

void
canaries_ctor(void)
{
	uintptr_t canary_start = (uintptr_t)&bss_ahkbahks_canary_start_;
	size_t canary_size = 0, canary_region_size;
	int rc;
	uintptr_t first_page = (uintptr_t)&bss_ahkbahks_canary_start_;
	/*
	 * These are both actually the start of the next {page, entry}, but
	 * that works for us.
	 */
	uintptr_t last_page = (uintptr_t)&bss_ahkbahks_canary_pages_end_;
	uintptr_t last_entry = (uintptr_t)&bss_ahkbahks_canary_end_;

	ahkbahks_canary_entries = (last_page - first_page) / sizeof(ahkbahks_canary_t);

	canary_size = last_entry - first_page;
	canary_region_size = last_page - first_page;

	if (!ahkbahks_canary_entries || !canary_size)
		errx(1, "Could not find canary region");

	debugx("finalizing pages %p-%p (%s@%p:0x%016x)",
	       ahkbahks_canaries, (void *)(canary_start + canary_size - 1),
	       STRING(CANARY(finalize_canaries)), &CANARY(finalize_canaries),
	       CANARY(finalize_canaries));

	rc = mprotect(ahkbahks_canaries, canary_region_size,
		      PROT_READ|PROT_WRITE);
	if (rc < 0)
		err(1, "Could not change canary protections");

	debugx("randomizing canaries @%p-%p", ahkbahks_canaries,
	       (void *)(canary_start + canary_size - 1));

	rc = getrandom(ahkbahks_canaries, canary_size, 0);
	if (rc < 0) {
		err(1, "Could not initialize canary data");
	} else if (rc != (int)canary_size) {
		err(1, "Could only initialize %d of %zd bytes of canary data",
		    rc, canary_size);
	}

	debugx("syncing data @%p-%p", ahkbahks_canaries,
	       (void *)(canary_start + canary_size - 1));

	rc = msync(ahkbahks_canaries, canary_size, MS_SYNC);
	if (rc < 0)
		err(1, "Could not write back canary data");

	debugx("new canary data for %s @%p:0x%0x",
	       STRING(CANARY(finalize_canaries)), &CANARY(finalize_canaries),
	       CANARY(finalize_canaries));

	rc = mprotect(ahkbahks_canaries, canary_region_size, PROT_READ);
	if (rc < 0)
		err(1, "Could not protect canary data");

	debugx("finalized pages %p-%p (%s @%p:0x%0x)",
	       ahkbahks_canaries, (void *)(canary_start + canary_size - 1),
	       STRING(CANARY(finalize_canaries)), &CANARY(finalize_canaries),
	       CANARY(finalize_canaries));
}

// vim:fenc=utf-8:tw=75:noet
