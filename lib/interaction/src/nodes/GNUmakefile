IVDEPTH = ../../../..
include $(IVDEPTH)/make/commonivdefs

DEPTH = ../../..
LDIRT = *.d
LCXXFLAGS = -I$(DEPTH)/database/include \
	    -I../../include \
	    -I$(DEPTH)/include \
	    -DLIBRARYBUILD \
	    -I$(DEPTH)/nodekits/include -DDEBUG

CXXFILES = SoAntiSquish.c++ SoSurroundScale.c++

all:: $(OBJECTS)

include $(COMMONRULES)
