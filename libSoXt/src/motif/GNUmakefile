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
	SoXtCntlPnl.c++ \
	SoXtFltTxt.c++ \
	SoXtLtSldrSet.c++ \
	SoXtMMSldrMax.c++ \
	SoXtMMSldrMin.c++ \
	SoXtMinMax.c++ \
	SoXtMltSldr.c++ \
	SoXtMltSldrs.c++ \
	SoXtMtlSSet.c++ \
	SoXtSSetBase.c++ \
	SoXtSldrMdl.c++ \
	SoXtSldrMdls.c++ \
	SoXtSldrSet.c++ \
	SoXtSldrTl.c++ \
	SoXtTfSldrSet.c++

all:: $(OBJECTS) 

include $(COMMONRULES)
