IVDEPTH = ../../../../..
include $(IVDEPTH)/make/commonivdefs

DEPTH = ../../..
LDIRT = *.d
LCXXFLAGS = -I$(DEPTH)/include -DLIBRARYBUILD \
	    -DDEBUG 

CXXFILES =\
	SoActionInit.c++ \
	SoAction.c++ \
	SoCallbackAction.c++ \
	SoGLRenderAction.c++ \
	SoGetBoundingBoxAction.c++ \
	SoGetMatrixAction.c++ \
	SoHandleEventAction.c++ \
	SoPickAction.c++ \
	SoRayPickAction.c++ \
	SoSearchAction.c++ \
	SoWriteAction.c++

all:: $(OBJECTS)

include $(COMMONRULES)
