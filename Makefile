
#######################################################################
# Default compilation parameters. Normally don't edit these           #
#######################################################################

srcdir      ?= .

DEFINES     := -DHAVE_CONFIG_H
LDFLAGS     :=
INCLUDES    := -I. -I$(srcdir) -I$(srcdir)/engines
LIBS        :=
OBJS        :=
DEPDIR      := .deps

MODULES     :=
MODULE_DIRS :=

# Load the make rules generated by configure
-include config.mk

ifeq "$(HAVE_GCC)" "1"
	CXXFLAGS:= -Wall $(CXXFLAGS)
	# Turn off some annoying and not-so-useful warnings
	CXXFLAGS+= -Wno-long-long -Wno-multichar -Wno-unknown-pragmas -Wno-reorder
	# Enable even more warnings...
	CXXFLAGS+= -Wpointer-arith -Wcast-qual
	CXXFLAGS+= -Wshadow -Wnon-virtual-dtor -Wwrite-strings

	# Currently we disable this gcc flag, since it will also warn in cases,
	# where using GCC_PRINTF (means: __attribute__((format(printf, x, y))))
	# is not possible, thus it would fail compiliation with -Werror without
	# being helpful.
	#CXXFLAGS+= -Wmissing-format-attribute

ifneq "$(BACKEND)" "tizen"
	# Disable RTTI and exceptions. These settings cause tizen apps to crash
	CXXFLAGS+= -fno-rtti -fno-exceptions
endif

ifneq "$(HAVE_CLANG)" "1"
	# enable checking of pointers returned by "new", but only when we do not
	# build with clang
	CXXFLAGS+= -fcheck-new
endif
endif

ifeq "$(HAVE_CLANG)" "1"
	CXXFLAGS+= -Wno-conversion -Wno-shorten-64-to-32 -Wno-sign-compare -Wno-four-char-constants
	# We use a anonymous nested type declaration in an anonymous union in
	# common/str.h. This is no standard construct and clang warns about it.
	# It works for all our target systems though, thus we simply disable that
	# warning.
	CXXFLAGS+= -Wno-nested-anon-types
endif

ifeq "$(HAVE_ICC)" "1"
	# Disable some warnings:
	#  161: unrecognized #pragma
	# 1899: multicharacter character literal (potential portability problem)
	CXXFLAGS+= -diag-disable 161,1899
endif

#######################################################################
# Default commands - put the necessary replacements in config.mk      #
#######################################################################

CAT     ?= cat
CP      ?= cp
ECHO    ?= printf
INSTALL ?= install
MKDIR   ?= mkdir -p
RM      ?= rm -f
RM_REC  ?= $(RM) -r
ZIP     ?= zip -q

#######################################################################
# Misc stuff - you should never have to edit this                     #
#######################################################################

EXECUTABLE  := $(EXEPRE)residualvm$(EXEEXT)

include $(srcdir)/Makefile.common

# check if configure has been run or has been changed since last run
config.h config.mk: $(srcdir)/configure $(srcdir)/engines/configure.engines
ifeq "$(findstring config.mk,$(MAKEFILE_LIST))" "config.mk"
	@echo "Running $(srcdir)/configure with the last specified parameters"
	@sleep 2
	LDFLAGS="$(SAVED_LDFLAGS)" CXX="$(SAVED_CXX)" \
			CXXFLAGS="$(SAVED_CXXFLAGS)" CPPFLAGS="$(SAVED_CPPFLAGS)" \
			ASFLAGS="$(SAVED_ASFLAGS)" WINDRESFLAGS="$(SAVED_WINDRESFLAGS)" \
			$(srcdir)/configure $(SAVED_CONFIGFLAGS)
else
	$(error You need to run $(srcdir)/configure before you can run make. Check $(srcdir)/configure --help for a list of parameters)
endif

ifneq ($(origin port_mk), undefined)
include $(srcdir)/$(port_mk)
endif
