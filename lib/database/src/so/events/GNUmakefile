IVDEPTH = ../../../../..
include $(IVDEPTH)/make/commonivdefs

DEPTH = ../../..
LDIRT = *.d
LCXXFLAGS = -I$(DEPTH)/include -DLIBRARYBUILD -DDEBUG

CXXFILES =\
	SoEventInit.c++ \
	SoButtonEvent.c++ \
	SoEvent.c++ \
	SoKeyboardEvent.c++ \
	SoLocation2Event.c++ \
	SoMouseButtonEvent.c++ \
	SoMotion3Event.c++ \
	SoSpaceballButtonEvent.c++

all:: $(OBJECTS)

include $(COMMONRULES)
