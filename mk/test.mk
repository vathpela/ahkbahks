#
# test.mk
# Peter Jones, 2019-05-29 13:23
#

valgrind : test
	LIBAHKBAHKS_DEBUG=1 LD_LIBRARY_PATH=$$PWD/libahkbahks/ valgrind --leak-check=full --track-origins=yes ./test

strace : all
	LIBAHKBAHKS_DEBUG=1 LD_LIBRARY_PATH=$$PWD/libahkbahks/ strace -tt -s 1024 -v -f -o test.strace -o /dev/stdout ./test

gdb : all
	LIBAHKBAHKS_DEBUG=1 LD_LIBRARY_PATH=$$PWD/libahkbahks/ gdb ./test

test: | all
test: test.c
	$(CC) $(CFLAGS) -o $@ $^ -Llibahkbahks -lahkbahks

# vim:ft=make
#
