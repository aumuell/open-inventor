IVDEPTH = ../../..
include $(IVDEPTH)/make/ivcommondefs

PROGRAM = textomatic
DEMO = $(PROGRAM)

CXXFILES = textomatic.c++ TextGraph.c++ TextWrapper.c++

LLDOPTS += -L../../samples/common

LLDLIBS = -lcommon -lInventor -lInventorXt -lGL -lXm -lXt -lX11

all: all_ivbin

install: install_ivdemo

include $(IVCOMMONRULES)
