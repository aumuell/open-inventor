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
	SoBoxHL.c++ \
	SoLineHL.c++

all:: $(OBJECTS)

include $(COMMONRULES)
