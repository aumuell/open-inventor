IVDEPTH = ../../..
include $(IVDEPTH)/make/ivcommondefs

PROGRAMS_2 = qmorf quadSphere quadCube quadSheet
DEMO = qmorf

CXXFILES = QuadThing.c++ Background.c++ FlashBackground.c++

LLDLIBS = -lInventor -lInventorXt

all: all_ivbin

install: install_ivdemo
	$(INSTALL) -m 755 $(DEMO).RUNME $(IVDEMOBINDIR)
	$(INSTALL) -m 644 $(DEMO).about $(IVDEMOBINDIR)

include $(IVCOMMONRULES)
