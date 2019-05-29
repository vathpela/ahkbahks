SRCDIR = $(realpath .)
TOPDIR = $(realpath ..)

include $(TOPDIR)/mk/version.mk
include $(TOPDIR)/mk/rules.mk
include $(TOPDIR)/mk/defaults.mk

.%.d : %.c
	$(CC) $(cflags) $(CPPFLAGS) -MM -MG -MF $@ $^

.%.d : %.S
	$(CC) $(cflags) $(CPPFLAGS) -MM -MG -MF $@ $^

SOURCES ?=
deps : $(call deps-of,$(filter-out %.h,$(SOURCES)))
