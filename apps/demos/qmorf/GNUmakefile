IVDEPTH = ../../..
include $(IVDEPTH)/make/commonivdefs

DEPTH = ../..
LDIRT = *.d
LCXXFLAGS = -DLIBRARYBUILD \
	    -I../../samples/common/ \
	    -I../../../lib/database/include/ \
  	    -I../../../lib/interaction/include \
	    -I../../../lib/nodekits/include -O3 \
	    -I../../../libSoXt/include 

TARGETS = qmorf

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
	-limage \
	-lInventorXt \
	-lInventor \
	-lcommon \
	-lXm -lXt -lXi -lX11 \
	-lFL -lGLU -lGL \
	-lXext \
	-lX11 -ldl -lm 

CXXFILES =\
	Background.c++ \
	FlashBackground.c++ \
	QuadThing.c++

OBJECTS=\
	Background.o \
	FlashBackground.o \
	QuadThing.o

$(TARGETS): qmorf quadSphere quadCube quadSheet


qmorf: $(OBJECTS) qmorf.o
	$(CXX) $(LDOPTS) qmorf.o $(OBJECTS) $(LDLIBS) -o qmorf

quadSphere: $(OBJECTS) quadSphere.o
	$(CXX) $(LDOPTS) quadSphere.o $(OBJECTS) $(LDLIBS) -o $@

quadCube: $(OBJECTS) quadCube.o
	$(CXX) $(LDOPTS) quadCube.o $(OBJECTS) $(LDLIBS) -o $@

quadSheet: $(OBJECTS) quadSheet.o
	$(CXX) $(LDOPTS) quadSheet.o $(OBJECTS) $(LDLIBS) -o $@

install: qmorf
	@mkdir -p $(IVROOT)/opt/SGIDemos/Inventor/bin
	@cp -f qmorf $(IVROOT)/opt/SGIDemos/Inventor/bin
	@cp -f qmorf.RUNME.linux $(IVROOT)/opt/SGIDemos/Inventor/bin/qmorf.RUNME
	@cp -f qmorf.about $(IVROOT)/opt/SGIDemos/Inventor/bin

include $(COMMONRULES)
