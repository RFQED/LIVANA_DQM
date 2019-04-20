######################################################
# A generic Makefile which does dependencies         #
# autmoatically                                      #
#                                                    #
# Original by Kurt Rinnert                           #
# Modified by Carl Gwilliam                          #
#                                                    #
# 1. Place in your top dir                           #
# 2. Change HEADERPAT and SRCPAT to your extension   #
#    naming (e.g. C cpp cxx h hh)                    #
# 3. Change LIBNAME to the wanted library name       #
# 4. Do 'gmake setup' to make dir structure and      #
#    copy files there:                               #
#    a. headers in include                           #
#    b. source files in src                          #
#    c. program files (containing main) in prg       #
#                                                    #
######################################################

#RQ
FILE = /scratch/abaxter/MDC3/MDC3Miniproduction/lz_Miniproduction_110_lzap.root

COMMONDIR = common

#directories
LIBDIR = lib
BINDIR = bin
INCDIR = include
SRCDIR = src
PRGDIR = prg
TMPDIR = tmp
RQDIR  = rqlib
CONFIGDIR = config
PYDIR = python
SHDIR = scripts
INDIR = inputs
BINOBJSDIR = $(TMPDIR)/bin
LIBOBJSDIR = $(TMPDIR)/lib
DEPSDIR    = $(TMPDIR)/deps

#paterns
HEADERPAT = h
SOURCEPAT = cxx

#source files
INCLUDEFILES = $(wildcard $(INCDIR)/*.$(HEADERPAT))
LIBCPPFILES  = $(wildcard $(SRCDIR)/*.$(SOURCEPAT))
PRGCPPFILES  = $(wildcard $(PRGDIR)/*.$(SOURCEPAT))

#targets
LIBNAME = LIVLZ
LIB     = $(LIBDIR)/lib$(LIBNAME).a
LIBOBJS = $(subst $(SRCDIR),$(LIBOBJSDIR),$(subst .$(SOURCEPAT),.o,$(LIBCPPFILES)))
BINS    = $(subst $(PRGDIR),$(BINDIR),$(subst .$(SOURCEPAT),,$(PRGCPPFILES)))
BINOBJS = $(subst $(PRGDIR),$(BINOBJSDIR),$(subst .$(SOURCEPAT),.o,$(PRGCPPFILES)))
LIBDEPS = $(subst $(SRCDIR),$(DEPSDIR),$(LIBCPPFILES:.$(SOURCEPAT)=.d))
BINDEPS = $(subst $(PRGDIR),$(DEPSDIR),$(PRGCPPFILES:.$(SOURCEPAT)=.d))

#tools
CPP = g++
LD  = g++
AR  = ar
DEP = gcc 

#flags
#CPPBASEFLAGS = -c -Wall -Werror -ansi -pedantic -I$(INCDIR)
CPPBASEFLAGS = -c -Wall -std=c++11 -I$(INCDIR) -I$(RQDIR)
DEPFLAGS = -std=c++11 -MM -I$(INCDIR) -I$(shell root-config --incdir) -I$(RQDIR) 

ifeq ($(DEBUG),yes)
CPPDBGFLAGS = -g
else
CPPDBGFLAGS =
endif

ifeq ($(OPTIMIZE),yes)
CPPOPTFLAGS = -O2 
else
CPPOPTFLAGS =
endif

ifeq ($(PROFILING),yes)
CPPPROFLAGS = -pg
else
CPPPROFLAGS =
endif


ROOTCPPFLAGS = $(shell root-config --cflags) -Wno-long-long
#ROOTCPPFLAGS = $(shell root-config --cflags) -Wno-long-long
ROOTLDFLAGS  = $(shell root-config --libs) 
RQLDFLAGS = rqlib/rqlib.so

CPPFLAGS = $(CPPDBGFLAGS) $(CPPPROFLAGS) $(CPPOPTFLAGS) $(CPPBASEFLAGS) $(ROOTCPPFLAGS) 

ARFLAGS = -rc

LDFLAGS = $(ROOTLDFLAGS) -lEG -lMinuit $(RQLDFLAGS) -l TreePlayer 

ifeq ($(DEBUG),yes)
override LDFLAGS += -g 
endif
ifeq ($(PROFILING),yes)
override LDFLAGS += -pg 
endif

LOADLIBS = -L$(LIBDIR) -l$(LIBNAME)

# All depends on bin -> will "make" it if not up-to-date
all: bin
	echo "* all done."

# bin depends on static library (.a) for classes + binary objects for prgs (.o) + binarys for prgs
bin: $(LIB) $(BINOBJS) $(BINS)

# lib dependes on static library for classes (LIB).  Will "make" if not up-to-date
lib: $(LIB)

$(BINS):$(BINDIR)%:$(BINOBJSDIR)/%.o $(LIB)
	echo "* linking: $(@F)"
	$(LD) $(LDFLAGS) -o $@ $< $(LOADLIBS)

# Static library (LIB) depends on library objects (.o) for each class (LIBOBJS).  
$(LIB):$(LIBOBJS)
	echo "* building library: $(@F)"
	$(AR) $(ARFLAGS) $@ $(LIBOBJS)

# Library object (.o) for each class (LIBOBJS) depends on corresponding source file
# (in srcdir with .o replaced by .SOURCEPAT) and will compile if not up-to-date  
$(LIBOBJS):$(LIBOBJSDIR)/%.o:$(SRCDIR)/%.$(SOURCEPAT) 
	echo "* compiling: $(<F)"
	$(CPP) $(CPPFLAGS) -o $@ $<

# Binary object for each prg (BINOBJS) depends on corresponding main file 
# (in prgdir with .o replaced by .SOURCEPAT) and will compile if no up-to-date 
$(BINOBJS):$(BINOBJSDIR)/%.o:$(PRGDIR)/%.$(SOURCEPAT) 
	echo "* compiling: $(<F)"
	$(CPP) $(CPPFLAGS) -o $@ $<

$(LIBDEPS):$(DEPSDIR)/%.d:$(SRCDIR)/%.$(SOURCEPAT)
	echo "* creating lib dependencies: $(<F)"
	set -e; $(DEP) $(DEPFLAGS) $< | sed 's!\w.*\.o[ :]*!$(LIBOBJSDIR)/$*.o $@ : !' > $@;\
	[ -s $@ ] || rm -f $@

$(BINDEPS):$(DEPSDIR)/%.d:$(PRGDIR)/%.$(SOURCEPAT)
	echo "* creating bin dependencies: $(<F)"
	set -e; $(DEP) $(DEPFLAGS) $< | sed 's!\w.*\.o[ :]*!$(BINOBJSDIR)/$*.o $@ : !' > $@;\
	[ -s $@ ] || rm -f $@

$(SRCDIR)/%.$(SOURCEPAT): $(RQDIR)/rqlib.so
	echo -n # HACK

$(RQDIR)/rqlib.so:
	echo "* compiling rqlib"
	root -b -q $(COMMONDIR)/configure.C\(\"$(FILE)\"\)		

# Clean up by removing all binaries, binary objects, libaries, library objects 
# and dependencies for both
clean:
	echo "* removing all targets"
	rm -f $(BINS) $(BINOBJS) $(LIB) $(LIBOBJS) $(BINDEPS) $(LIBDEPS) $(BINOBJSDIR)/*.o $(LIBOBJSDIR)/*.o $(DEPSDIR)/*.d $(BINDIR)/*
	echo "* clean done"

rqclean:
	echo "* removing $(RQDIR)"
	rm -rf $(RQDIR)

purge: rqclean clean

# Make correct directory structure and copy files into it.  Files ending in:
# - .HEADERPAT go to INCDIR
# - .SRCPAT go to
#   - PRGDIR if contain main()
#   - SRCDIR otherwise
setup:
	echo "* creating directory structure"
	mkdir -p $(SRCDIR) $(PRGDIR) $(BINDIR) $(LIBDIR) $(INCDIR) $(TMPDIR)/deps $(TMPDIR)/lib $(TMPDIR)/bin $(CONFIGDIR) $(PYDIR) $(SHDIR) $(INDIR)
	echo "* Moving program files to $(PRGDIR)"
	mv $(shell grep -H "main()" *.$(SOURCEPAT) | cut -d : -f 1) $(PRGDIR)/.
	echo "* Moving header files to $(INDIR)"
	mv *.$(HEADERPAT) $(INCDIR)/.
	echo "* Moving source files to $(SRCDIR)"
	mv *.$(SOURCEPAT) $(SRCDIR)/.

common:
	root -b -q $(COMMONDIR)/configure.C\(\"$(FILE)\"\)		
dust:
	echo "* cough, cough"

ifneq ($(VERBOSE),yes)
.SILENT:
endif

.PHONY: all clean dust lib bin setup common purge rqclean

# include the dependency files if the target does not contain "clean", "setup", "common"
# the "-" suppresses warnings if files do not exist
ifeq (,$(findstring clean,$(MAKECMDGOALS)))
ifeq (,$(findstring setup,$(MAKECMDGOALS)))
ifeq (,$(findstring common,$(MAKECMDGOALS)))
ifeq (,$(findstring purge,$(MAKECMDGOALS)))
-include $(BINDEPS)
-include $(LIBDEPS)
endif
endif
endif
endif


# MACROS:
#$@ Full name of the current target. 
#$? A list of files for current dependency which are out-of-date. 
#$< The source file of the current (single) dependency.


