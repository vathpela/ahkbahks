default : all

.PHONY: default all deps clean install install_systemd install_sysvinit test

include $(TOPDIR)/mk/version.mk

all : deps

deps :

clean :

install :

install_systemd :

install_sysvinit :

test :

%.a :
	$(AR) $(ARFLAGS) $@ $^
	$(RANLIB) $(RANLIBFLAGS) $@

% : %.o
	$(CCLD) $(CCLDFLAGS) -o $@ $^ $(LDLIBS)

%.so.$(VERSION) : %.so.$(MAJOR_VERSION)
	@ln -v $< $@

%.so.$(MAJOR_VERSION) : %.so
	@ln -v $< $@

%.so :
	$(CCLD) $(CCLDFLAGS) $(SOFLAGS) \
		-Wl,-soname,$(subst .so.$(VERSION),.so.$(MAJOR_VERSION),$@) \
		-Wl,--version-script=$(MAP) \
		-o $@ $^ $(LDLIBS) \
		$(foreach x,$(wildcard $(patsubst %.so,%.lds,$@)),-T $(x))

%.abixml : %.so
	$(ABIDW) --headers-dir $(TOPDIR)/include/ --out-file $@ $^
	@sed -i -s 's,$(TOPDIR)/,,g' $@

%.abicheck : %.so
	$(ABIDIFF) \
		--suppr abignore \
		--headers-dir2 $(TOPDIR)/include \
		$(patsubst %.so,%.abixml,$<) \
		$<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $(filter %.c %.o %.S,$^)

%.static.o: %.c
	$(CC) $(AFLAGS) -c -o $@ $(filter %.c %.o %.S,$^)

%.S: %.c
	$(CC) $(CFLAGS) -S $< -o $@

%.E: %.c
	$(CC) $(CFLAGS) -E $< -o $@

%.c : %.h

.%.d :

define substitute-version =
	sed							\
		-e "s,@@VERSION@@,$(VERSION),g"			\
		-e "s,@@MAJOR_VERSION@@,$(MAJOR_VERSION),g"	\
		-e "s,@@MINOR_VERSION@@,$(MINOR_VERSION),g"	\
		-e "s,@@LIBDIR@@,$(libdir),g"			\
		-e "s,@@LIBEXECDIR@@,$(libexecdir),g"		\
		$(1) > $(2)
endef

%.pc : %.pc.in
	@$(call substitute-version,$<,$@)

%.spec : %.spec.in
	@$(call substitute-version,$<,$@)

%.map : %.map.in
	@$(call substitute-version,$<,$@)

%.service : %.service.in
	@$(call substitute-version,$<,$@)

%.sysvinit : %.sysvinit.in
	@$(call substitute-version,$<,$@)

pkg-config-cflags = \
	$(shell if [ -n "$(PKGS)" ]; then $(PKG_CONFIG) --cflags $(PKGS); fi)
pkg-config-ldflags = \
	$(shell if [ -n "$(PKGS)" ]; then $(PKG_CONFIG) --libs-only-L --libs-only-other $(PKGS) ; fi)
pkg-config-ldlibs = \
	$(shell if [ -n "$(PKGS)" ]; then $(PKG_CONFIG) --libs-only-l $(PKGS) ; fi)

objects-of = \
	$(patsubst %.c,%.o,$(1))

define deps-of =
	$(foreach src,$(filter %.c,$(1)),$(patsubst %.c,.%.d,$(src))) \
	$(foreach src,$(filter %.S,$(1)),$(patsubst %.S,.%.d,$(src)))
endef

define get-config =
$(shell git config --local --get "$(NAME).$(1)")
endef

define removes
	if [ -n "$(TARGETS)" ]; then rm -rfv $(TARGETS) ; fi
	rm -rfv *.o *.a *.so *.so.[0123456789]* .*.d
	rm -rfv {vg,}core.[0123456789]*
endef

define descend
	set -e ; for x in $(SUBDIRS) ; do \
		$(MAKE) -C $$x $@ ; \
	done
endef
