SRCDIR = $(realpath .)
TOPDIR = $(realpath ..)

include $(TOPDIR)/mk/version.mk
include $(TOPDIR)/mk/defaults.mk
include $(TOPDIR)/mk/rules.mk

LIBTARGETS=libahkbahks.so.$(VERSION)
TARGETS=$(LIBTARGETS)

LIBAHKBAHKS_SOURCES = $(sort $(wildcard *.c))
ALL_SOURCES=$(LIBAHKBAHKS_SOURCES)
-include $(call deps-of,$(ALL_SOURCES))

all : deps $(TARGETS)

libahkbahks.so : $(call objects-of,$(LIBAHKBAHKS_SOURCES))
libahkbahks.so : | $(wildcard *.h) $(wildcard $(TOPDIR)/include/*.h)
libahkbahks.so : | libahkbahks.map
libahkbahks.so : LIBS= #LIBS=pthread
libahkbahks.so : MAP=libahkbahks.map

deps : $(ALL_SOURCES)
	$(MAKE) -f $(TOPDIR)/mk/deps.mk deps SOURCES="$(ALL_SOURCES)"

clean :
	@rm -rfv *~ *.o *.a *.so *.so.* .*.d *.map

install :
	$(INSTALL) -d -m 755 $(DESTDIR)$(libdir)
	$(foreach x,$(LIBTARGETS), \
		$(INSTALL) -m 755 $(x) $(DESTDIR)$(libdir)/$(x).$(VERSION) ;\
		ln -fs $(x).$(VERSION) $(DESTDIR)$(libdir)/$(x).$(MAJOR_VERSION) ;\
		ln -fs $(x).$(VERSION) $(DESTDIR)$(libdir)/$(x); )
	$(foreach x,$(STATICLIBTARGETS), \
		$(INSTALL) -m 644 $(x) $(DESTDIR)$(libdir)/$(x); )

.SECONDARY : $(foreach x,$(LIBTARGETS),$(x).$(VERSION) $(x).$(MAJOR_VERSION))
