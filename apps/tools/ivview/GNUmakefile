IVDEPTH = ../../..
include $(IVDEPTH)/make/ivcommondefs

PROGRAM = ivview

CXXFILES = ivview.c++

LLDOPTS += -L../../samples/widgets

LLDLIBS = -lInventorWidget -lInventorXt -lInventor

all: all_ivbin

install: install_ivbin
	mkdir -p $(IVHELPDIR)/$(PROGRAM)
	$(INSTALL) -m 644 $(PROGRAM).about $(IVHELPDIR)/$(PROGRAM)

include $(IVCOMMONRULES)
