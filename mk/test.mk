#
# test.mk
# Peter Jones, 2019-05-29 13:23
#

valgrind : all
	valgrind --leak-check=full --track-origins=yes ./libahkbahks/test

strace : all
	strace -tt -s 1024 -v -f -o test.strace -o /dev/stdout ./libahkbahks/test

gdb : all
	gdb ./libahkbahks/test


# vim:ft=make
#
