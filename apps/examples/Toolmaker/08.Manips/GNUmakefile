IVDEPTH = ../../../..
include $(IVDEPTH)/make/ivcommondefs

PROGRAM = newManips

CXXFILES = \
	Coordinate3Manip.c++ RotTransDragger.c++ \
	RotTransManip.c++ TranslateRadialDragger.c++ \
	TranslateRadialManip.c++ newManips.c++

LLDLIBS = -lInventor -lInventorXt

all: all_ivbin

include $(IVCOMMONRULES)
