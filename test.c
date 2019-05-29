/*
 * test.c
 * Copyright 2019 Peter Jones <pjones@redhat.com>
 *
 */

#include <stdlib.h>
#include <stdio.h>

int main(void)
{
	setenv("LIBAHKBAHKS_DEBUG", "1", 1);
	printf("hello\n");
	return 0;
}

// vim:fenc=utf-8:tw=75:noet
