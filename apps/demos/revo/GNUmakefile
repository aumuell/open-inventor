IVDEPTH = ../../..
include $(IVDEPTH)/make/ivcommondefs

PROGRAM = revo
DEMO = $(PROGRAM)

LCXXINCS += -I../../samples/common

CXXFILES = revo.c++ RevClass.c++ profile.c++

LLDOPTS += -L../../samples/common

LLDLIBS = -lcommon -lInventor -lInventorXt

all: all_ivbin

install: install_ivdemo
	$(INSTALL) -m 644 $(DEMO).about $(IVDEMOBINDIR)

include $(IVCOMMONRULES)
