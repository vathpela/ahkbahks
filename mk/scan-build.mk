SCAN_BUILD ?= $(shell x=$$(which --skip-alias --skip-functions scan-build 2>/dev/null) ; [ -n "$$x" ] && echo 1)
ifeq ($(SCAN_BUILD),)
	SCAN_BUILD_ERROR = $(error scan-build not found)
endif

SCAN_CC = $(CROSS_COMPILE)clang
SCAN_AR = $(CROSS_COMPILE)llvm-ar
SCAN_ARFLAGS=-cqvs
SCAN_RANLIB = $(CROSS_COMPILE)llvm-ranlib

scan-test : ; $(SCAN_BUILD_ERROR)

scan-clean : clean
	@if [[ -d scan-results ]]; then rm -rf scan-results && echo "removed 'scan-results'"; fi

scan-build : | scan-test
scan-build : clean
	scan-build -o scan-results make $(DASHJ) \
		AR=$(SCAN_AR) ARFLAGS=$(SCAN_ARFLAGS) \
		RANLIB=$(SCAN_RANLIB) CC=$(SCAN_CC) \
		all

scan-build-all : | scan-test
scan-build-all : clean
	scan-build -o scan-results make $(DASHJ) \
		AR=$(SCAN_AR) ARFLAGS=$(SCAN_ARFLAGS) \
		RANLIB=$(SCAN_RANLIB) CC=$(SCAN_CC) \
		all

.PHONY : scan-build scan-clean

# vim:ft=make
