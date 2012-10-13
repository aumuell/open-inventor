IVDEPTH = ../../..
include $(IVDEPTH)/make/ivcommondefs

DSO = GeneralizedCylinder.so

CXXFILES = \
	GeneralizedCylinder.c++ Triangulator.c++ \
	NurbMaker.c++

LLDLIBS = -L$(IVDEPTH)/lib -lInventor -L$(IVDEPTH)/libSoXt -lInventorXt

all install: all_ivbin

include $(IVCOMMONRULES)
