IVDEPTH = ../../..
include $(IVDEPTH)/make/ivcommondefs

PROGRAM = ivview

CXXFILES = ivview.c++

LLDOPTS += -L../../samples/widgets

LLDLIBS = -lInventorWidget -lInventorXt -lInventor -lGL -lXm -lXt -lX11

all: all_ivbin

install: install_ivbin
	mkdir -p $(IVHELPDIR)/$(PROGRAM)
	$(INSTALL) -m 644 $(PROGRAM).about.pdf $(IVHELPDIR)/$(PROGRAM)

include $(IVCOMMONRULES)
