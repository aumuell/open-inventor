IVDEPTH = ../../..
include $(IVDEPTH)/make/commonivdefs

DEPTH = ../..
LDIRT = *.d
LCXXFLAGS += -DLIBRARYBUILD \
	    -I../../samples/common/ \
	    -I../../../lib/database/include/ \
  	    -I../../../lib/interaction/include \
	    -I../../../lib/nodekits/include -O3 \
	    -I../../../libSoXt/include 

TARGETS = drop

LLDOPTS = \
	-L../../../libimage \
	-L../../../lib \
	-L/usr/X11R6/lib \
	-L../../../libSoXt \
	-L../../samples/common \
	-L/usr/X11R6/lib 

ifeq ($(usingLinux),1)
LLDOPTS += -L../../../libFL/src
endif

LLDLIBS = \
	-lInventorXt \
	-lInventor \
	-lcommon \
	-lXm -lXt -lXi -lX11 \
	-lFL -lGLU -lGL \
	-lXext \
	-lX11 -ldl -lm 

CXXFILES =\
	TsBlock.c++ \
	TsEvents.c++ \
	TsField.c++ \
	TsPiece.c++ \
	TsPieces.c++ \
	TsRemoval.c++ \
	TsTrickle.c++ \
	TsViewer.c++ \
	drop.c++


$(TARGETS): $(OBJECTS)
	$(CXX) $(LDOPTS) $(OBJECTS) $(LDLIBS) -o $(TARGETS)

install: $(TARGETS)
	@mkdir -p $(IVROOT)/opt/SGIDemos/Inventor/bin
	@cp -f $(TARGETS) $(IVROOT)/opt/SGIDemos/Inventor/bin

include $(COMMONRULES)
