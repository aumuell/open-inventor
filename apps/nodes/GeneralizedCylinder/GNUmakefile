IVDEPTH = ../../..
include $(IVDEPTH)/make/ivcommondefs

DSO = GeneralizedCylinder.so

CXXFILES = \
	GeneralizedCylinder.c++ Triangulator.c++ \
	NurbMaker.c++

LLDLIBS = -lInventor -lInventorXt

all install: all_ivbin

include $(IVCOMMONRULES)
