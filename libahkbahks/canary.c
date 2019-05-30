/*
 * canary.c
 * Copyright 2019 Peter Jones <pjones@redhat.com>
 *
 */

#include "ahkbahks.h"

#include <errno.h>
#include <dlfcn.h>
#include <link.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/random.h>
#include <syscall.h>
#include <unistd.h>

DECLARE_CANARY(finalize_canaries);

#if 0
struct canary_data {
	ElfW(Addr) canary_start;
	ElfW(Word) canary_size;
	const void *target;
	int i;
};

int cb(struct dl_phdr_info *phi, size_t sz, void *data)
{
	size_t pgsz = sysconf(_SC_PAGE_SIZE);
	struct canary_data *d = data;
	if (phi->dlpi_addr > (ElfW(Addr))d->target)
		return 0;

	fprintf(stderr, "target:%p\n", d->target);
	fprintf(stderr, "d[%d].dlpi_addr:0x%016lx\n", d->i, phi->dlpi_addr);
	fprintf(stderr, "d[%d].dlpi_name:%s\n", d->i, phi->dlpi_name);
	ElfW(Half) phn = phi->dlpi_phnum;
	const ElfW(Phdr) *phdrs = phi->dlpi_phdr;
	for (ElfW(Half) i = 0; i < phn; i++) {
		const ElfW(Phdr) *phdr = &phdrs[i];
		uintptr_t addr = phdr->p_vaddr + phi->dlpi_addr;
		printf("phdr[%d] type:0x%x offset:0x%lx paddr:0x%lx vaddr:0x%lx memsz:0x%lx\n",
		       i, phdr->p_type, phdr->p_offset, phdr->p_paddr, addr, phdr->p_memsz);
		if (addr > (ElfW(Addr))d->target)
			continue;
		if (addr + phdr->p_memsz < (ElfW(Addr))d->target)
			continue;

		d->canary_start = ALIGN_DOWN(addr, pgsz);
		d->canary_size = ALIGN(phdr->p_memsz, pgsz);
		printf("found it %p->%p\n", (void *)d->canary_start, (void *)(d->canary_start + d->canary_size));

		return 1;
	}

	d->i++;
	return 0;
}
#endif

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
#if 1
	uintptr_t first_page = (uintptr_t)&bss_ahkbahks_canary_start_;
	/*
	 * These are both actually the start of the next {page, entry}, but
	 * that works for us.
	 */
	uintptr_t last_page = (uintptr_t)&bss_ahkbahks_canary_pages_end_;
	uintptr_t last_entry = (uintptr_t)&bss_ahkbahks_canary_end_;
#else
	void *addr;
	struct canary_data cd = { 0, 0, &CANARY(finalize_canaries), 0 };

	rc = dl_iterate_phdr(cb, &cd);
	if (rc != 1)
		errx(1, "Could not get canary info");

	canary_start = (void *)cd.canary_start;
	canary_size = cd.canary_size;
#endif
	ahkbahks_canary_entries = (last_page - first_page) / sizeof(ahkbahks_canary_t);

	canary_size = last_entry - first_page;
	canary_region_size = last_page - first_page;

	if (ahkbahks_canary_entries && canary_size) {
		debugx("finalizing pages %p-%p (%s@%p:0x%016x)",
		       ahkbahks_canaries,
		       (void *)(canary_start + canary_size - 1),
		       STRING(CANARY(finalize_canaries)),
		       &CANARY(finalize_canaries),
		       CANARY(finalize_canaries));

#if 1
		rc = mprotect(ahkbahks_canaries, canary_region_size,
			      PROT_READ|PROT_WRITE);
		if (rc < 0)
			err(1, "Could not change canary protections");
#else
		rc = munmap(canary_start, canary_size);
		if (rc < 0)
			err(1, "Could not unmap canary address");

		debugx("remapping canary pages");

		addr = mmap(canary_start, canary_size, PROT_READ|PROT_WRITE,
			  MAP_FIXED|MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
		if (addr != canary_start)
			err(1, "Could not remap canary address");
#endif

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
		       STRING(CANARY(finalize_canaries)),
		       &CANARY(finalize_canaries), CANARY(finalize_canaries));

		rc = mprotect(ahkbahks_canaries, canary_region_size, PROT_READ);
		if (rc < 0)
			err(1, "Could not protect canary data");

		debugx("finalized pages %p-%p (%s @%p:0x%0x)",
		       ahkbahks_canaries,
		       (void *)(canary_start + canary_size - 1),
		       STRING(CANARY(finalize_canaries)),
		       &CANARY(finalize_canaries),
		       CANARY(finalize_canaries));
	}
}

// vim:fenc=utf-8:tw=75:noet
