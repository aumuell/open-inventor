IVDEPTH = ../../..
include $(IVDEPTH)/make/commonivdefs

DEPTH = ../..
LDIRT = *.d
LCXXFLAGS = -I$(DEPTH)/include \
	    -DLIBRARYBUILD \
	    -I$(DEPTH)/../lib/database/include \
	    -I$(DEPTH)/../lib/interaction/include \
	    -I$(DEPTH)/../lib/nodekits/include -DDEBUG

CXXFILES = \
	SoXtFocus.c++ \
	SoXtKeybd.c++ \
	SoXtMouse.c++ \
	SoXtSpball.c++ 

all:: $(OBJECTS)

include $(COMMONRULES)

