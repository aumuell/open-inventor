IVDEPTH = ../../../../..
include $(IVDEPTH)/make/commonivdefs

DEPTH = ../../..
LDIRT = *.d
LCXXFLAGS = -I$(DEPTH)/include  -DLIBRARYBUILD -DDEBUG

CXXFILES =\
	SoErrorInit.c++ \
	SoError.c++ \
	SoDebugError.c++ \
	SoMemoryError.c++ \
	SoReadError.c++

all:: $(OBJECTS)

include $(COMMONRULES)
