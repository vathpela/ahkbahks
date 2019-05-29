SRCDIR = $(realpath .)
TOPDIR = $(realpath .)

default: clean all test

include $(TOPDIR)/mk/version.mk
include $(TOPDIR)/mk/rules.mk
include $(TOPDIR)/mk/defaults.mk
include $(TOPDIR)/mk/coverity.mk
include $(TOPDIR)/mk/scan-build.mk
include $(TOPDIR)/mk/test.mk

SUBDIRS := include libahkbahks
TARGETS = test

install :
	$(INSTALL) -d -m 755 $(INSTALLROOT)$(docdir)/libahkbahks-$(VERSION)/
	$(INSTALL) -pm 644 COPYING $(INSTALLROOT)$(docdir)/libahkbahks-$(VERSION)/
	@$(call descend)

distclean : | clean

clean :
	@$(call removes)
	@$(call descend)

clean deps all : | mk/version.mk

deps all :
	@$(call descend)

$(SUBDIRS) :
	$(MAKE) -C $@ all

.PHONY: $(SUBDIRS)

GITTAG = $(VERSION)

test-archive:
	@rm -rf /tmp/libahkbahks-$(VERSION) /tmp/libahkbahks-$(VERSION)-tmp
	@mkdir -p /tmp/libahkbahks-$(VERSION)-tmp
	@git archive --format=tar $(shell git branch | awk '/^*/ { print $$2 }') | ( cd /tmp/libahkbahks-$(VERSION)-tmp/ ; tar x )
	@git diff | ( cd /tmp/libahkbahks-$(VERSION)-tmp/ ; patch -s -p1 -b -z .gitdiff )
	@mv /tmp/libahkbahks-$(VERSION)-tmp/ /tmp/libahkbahks-$(VERSION)/
	@dir=$$PWD; cd /tmp; tar -c --bzip2 -f $$dir/libahkbahks-$(VERSION).tar.bz2 libahkbahks-$(VERSION)
	@rm -rf /tmp/libahkbahks-$(VERSION)
	@echo "The archive is in libahkbahks-$(VERSION).tar.bz2"

tag:
	git tag -s $(GITTAG) refs/heads/master

archive: tag
	@rm -rf /tmp/libahkbahks-$(VERSION) /tmp/libahkbahks-$(VERSION)-tmp
	@mkdir -p /tmp/libahkbahks-$(VERSION)-tmp
	@git archive --format=tar $(GITTAG) | ( cd /tmp/libahkbahks-$(VERSION)-tmp/ ; tar x )
	@mv /tmp/libahkbahks-$(VERSION)-tmp/ /tmp/libahkbahks-$(VERSION)/
	@dir=$$PWD; cd /tmp; tar -c --bzip2 -f $$dir/libahkbahks-$(VERSION).tar.bz2 libahkbahks-$(VERSION)
	@rm -rf /tmp/libahkbahks-$(VERSION)
	@echo "The archive is in libahkbahks-$(VERSION).tar.bz2"
