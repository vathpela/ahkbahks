SRCDIR = $(realpath .)
TOPDIR = $(realpath ..)

include $(TOPDIR)/mk/version.mk
include $(TOPDIR)/mk/rules.mk
include $(TOPDIR)/mk/defaults.mk

install:
	$(INSTALL) -d -m 755 $(INSTALLROOT)$(PREFIX)/include/
	$(INSTALL) -m 644 *.h $(INSTALLROOT)$(PREFIX)/include/
