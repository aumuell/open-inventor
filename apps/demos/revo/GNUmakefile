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

include $(IVCOMMONRULES)
