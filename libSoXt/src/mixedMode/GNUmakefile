IVDEPTH = ../../..
include $(IVDEPTH)/make/commonivdefs

DEPTH = ../..
LDIRT = *.d
CXXFLAGS += -I$(DEPTH)/include \
	   -DLIBRARYBUILD \
	   -I$(DEPTH)/../lib/database/include \
	   -I$(DEPTH)/../lib/interaction/include \
	   -I$(DEPTH)/../lib/nodekits/include -DDEBUG

CXXFILES = \
	SoFloatCBList.c++ \
	SoXtColEd.c++ \
	SoXtColPatch.c++ \
	SoXtColSlider.c++ \
	SoXtColWheel.c++ \
	SoXtSlider.c++ \
	SoXtUIRegion.c++

all:: $(OBJECTS) 

include $(COMMONRULES)
