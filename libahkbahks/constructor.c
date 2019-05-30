/*
 * constructor.c
 * Copyright 2019 Peter Jones <pjones@redhat.com>
 *
 */

#include "ahkbahks.h"

static void CONSTRUCTOR
ahkbahks_ctor(void)
{
	debug_ctor();
	canaries_ctor();
}

static void DESTRUCTOR
ahkbahks_dtor(void)
{
	debug_dtor();
}

// vim:fenc=utf-8:tw=75:noet
