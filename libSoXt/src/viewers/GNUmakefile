#!gmake
IVDEPTH = ../../..
include $(IVDEPTH)/make/commonivdefs

DEPTH = ../..
LDIRT = *.d
LCXXFLAGS += -I$(DEPTH)/include \
	    -DLIBRARYBUILD \
	    -I. \
	    -I$(DEPTH)/../lib/database/include \
	    -I$(DEPTH)/../lib/interaction/include \
	    -I$(DEPTH)/../lib/nodekits/include -DDEBUG
 
CXXFILES =\
	SoXtBitmapButton.c++ \
	SoXtConstVwr.c++ \
	SoXtCursors.c++ \
	SoXtExamVwr.c++ \
	SoXtFlyVwr.c++ \
	SoXtFullVwr.c++ \
	SoXtIcons.c++ \
	SoXtPlaneVwr.c++ \
	SoXtViewer.c++ \
	SoXtWalkVwr.c++ 

all:: $(OBJECTS) 
	cd Sgm ; $(MAKE)

include $(COMMONRULES)

clean clobber:
	rm -f *.o *.d
	cd Sgm ; make clobber

