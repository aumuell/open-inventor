IVDEPTH = ../../../..
include $(IVDEPTH)/make/commonivdefs

DEPTH = ../../..
LDIRT = *.d
LCXXFLAGS = -I$(DEPTH)/database/include \
	    -I../../include \
	    -I$(DEPTH)/include \
	    -DLIBRARYBUILD \
	    -I$(DEPTH)/nodekits/include -DDEBUG

CXXFILES = \
	SoTransformManip.c++ \
	SoCenterballManip.c++ \
	SoTabBoxManip.c++ \
	SoHbox.c++ \
	SoTball.c++ \
	SoXformBox.c++ \
	SoDirLtManip.c++ \
	SoPtLtManip.c++ \
	SoSpotLtManip.c++ \
	SoTransformerManip.c++ \
	SoJackManip.c++



all:: $(OBJECTS)

include $(COMMONRULES)
