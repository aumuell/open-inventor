IVDEPTH = ../../..
include $(IVDEPTH)/make/ivcommondefs

PROGRAM = textomatic
DEMO = $(PROGRAM)

CXXFILES = textomatic.c++ TextGraph.c++ TextWrapper.c++

LLDOPTS += -L../../samples/common

LLDLIBS = -lcommon -lInventor -lInventorXt

all: all_ivbin

install: install_ivdemo
	mkdir -p $(IVAPPLOADDIR)
	$(INSTALL) -m 644 Textomatic $(IVAPPLOADDIR)

include $(IVCOMMONRULES)
