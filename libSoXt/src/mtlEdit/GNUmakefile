IVDEPTH = ../../..
include $(IVDEPTH)/make/commonivdefs

DEPTH = ../..
LDIRT = *.d
CXXFLAGS += -I$(DEPTH)/include \
	   -DLIBRARYBUILD \
	   -I$(DEPTH)/../lib/database/include \
	   -I$(DEPTH)/../lib/interaction/include \
	   -I$(DEPTH)/../lib/nodekits/include -DDEBUG

CXXFILES =\
	SoXtMtlEdit.c++ \
	SoXtMtlList.c++

all:: $(OBJECTS) 

include $(COMMONRULES)
