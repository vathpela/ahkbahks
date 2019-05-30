NAME	= ahkbahks
COMMIT_ID ?= $(shell git log -1 --pretty=%H 2>/dev/null || echo master)
prefix	?= /usr/
prefix	:= $(abspath $(prefix))/
libdir	?= $(prefix)lib64/
libdatadir ?= $(prefix)lib/
libexecdir ?= $(prefix)libexec/
datadir	?= $(prefix)share/
mandir	?= $(datadir)man/
includedir ?= $(prefix)include/
bindir	?= $(prefix)bin/
pcdir	?= $(libdir)pkgconfig/
docdir	?= $(prefix)share/doc/
DESTDIR	?=
INSTALLROOT = $(DESTDIR)

INSTALL	?= install
CROSS_COMPILE	?=

PKG_CONFIG ?= $(CROSS_COMPILE)pkg-config
CC	:= $(if $(filter default,$(origin CC)),$(CROSS_COMPILE)gcc,$(CC))
CCLD	:= $(if $(filter undefined,$(origin CCLD)),$(CC),$(CCLD))
CFLAGS	?= -Og -g3 -flto -fPIE
AS	?= $(CROSS_COMPILE)as
AR	?= $(CROSS_COMPILE)$(if $(filter $(CC),clang),llvm-ar,$(notdir $(CC))-ar)
RANLIB	?= $(CROSS_COMPILE)$(if $(filter $(CC),clang),llvm-ranlib,$(notdir $(CC))-ranlib)
OBJCOPY	?= $(CROSS_COMPILE)objcopy
LD	?= $(CROSS_COMPILE)ld
INSTALL	?= $(CROSS_COMPILE)install
ABIDW	?= abidw
ABIDIFF ?= abidiff

PKGS	=

HOSTARCH   := $(shell uname -m | sed s,i[3456789]86,i686,)
ARCH	   ?= $(shell uname -m | sed s,i[3456789]86,i686,)

SOFLAGS	?= -shared -fPIE
AFLAGS = $(filter-out -flto,$(CFLAGS)) -static -Wl,-static

clang_cppflags =
gcc_cppflags = -Wmaybe-uninitialized
CPPFLAGS += -I$(TOPDIR)/include/ -std=gnu11 \
	    -D_GNU_SOURCE -DLIB_VERSION=$(VERSION) \
	    -DLIB_MAJOR_VERSION=$(MAJOR_VERSION) \
	    -DLIB_MINOR_VERSION=$(MINOR_VERSION) \
	    -Wall -Wextra \
	    -Wsign-compare \
	    -Wno-unused-result \
	    -Wno-unused-function \
	    -Wno-unused-parameter \
	    -Wno-missing-field-initializers \
	    -Wno-nonnull-compare \
	    -Werror -Wno-error=cpp \
	    $(if $(filter $(CC),clang),$(clang_cppflags), ) \
	    $(if $(filter $(CC),gcc),$(gcc_cppflags), ) \
	    $(call pkg-config-cflags)

clang_cflags =
gcc_cflags = -grecord-gcc-switches \
	     $(foreach spec,$(wildcard /usr/lib/rpm/redhat/redhat-hardened-cc1),-specs=$(spec))
CFLAGS	+= $(ARCH3264) $(CPPFLAGS) \
	   -fvisibility=hidden \
	   $(if $(filter $(CC),clang),$(clang_cflags), ) \
	   $(if $(filter $(CC),gcc),$(gcc_cflags), )

CCLDFLAGS ?=
gcc_ccldflags = -fno-merge-constants -fkeep-inline-functions \
		-fvar-tracking -fvar-tracking-assignments \
		-Wl,--fatal-warnings -Wl,--no-allow-shlib-undefined \
		-Wl,--default-symver -Wl,-O2 \
		-Wl,-z,relro -Wl,-z,now -Wl,--add-needed -Wl,--build-id \
		-Wl,--no-copy-dt-needed-entries -Wl,--as-needed \
		$(foreach spec,$(wildcard /usr/lib/rpm/redhat/redhat-hardened-ld),-specs=$(spec))
clang_ccldflags =
CCLDFLAGS += $(CFLAGS) $(LDFLAGS) \
	     $(if $(filter $(CCLD),clang),$(clang_ccldflags),) \
	     $(if $(filter $(CCLD),gcc),$(gcc_ccldflags),) \
	     $(call pkg-config-ccldflags)
ASFLAGS	?= $(ARCH3264)
RANLIBFLAGS	?= $(if $(filter $(CC),gcc),-D)
gcc_arflags = -Dcvqs
clang_arflags = -cqvs
ARFLAGS := $(if $(filter $(CC),gcc),$(gcc_arflags))$(if $(filter $(CC),clang),$(clang_arflags))

LDLIBS	= $(foreach lib,$(LIBS),-l$(lib)) $(call pkg-config-ldlibs)

ifeq ($(ARCH), i686)
  ifeq ($(HOSTARCH), x86_64)
    ARCH3264 := -m32
  endif
endif

ifeq ($(ARCH), x86_64)
  ifeq ($(HOSTARCH), i686)
    ARCH3264 := -m64
  endif
endif
