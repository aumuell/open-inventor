IVDEPTH = ../../..
include $(IVDEPTH)/make/commonivdefs

DEPTH = ../../..

DSO = 	libDecal.so

C++FILES = \
	Decal.c++

LCXXFLAGS = -I$(DEPTH)/include \
	    -I../../samples/common/ \
	    -I../../../lib/database/include/ \
  	    -I../../../lib/interaction/include \
	    -I../../../lib/nodekits/include -O3 \
	    -I../../../libSoXt/include 

LLDOPTS = \
	-L../../../libimage \
	-L../../../lib \
	-L/usr/X11R6/lib \
	-L../../../libSoXt \
	-L../../../libFL/src \
	-L../../samples/common \
	-L/usr/X11R6/lib \

LLDLIBS = \
	-limage \
	-lcommon \
	-lInventorXt \
	-lInventor \
	-lXm  -lXt -lXi -lX11 \
	-lFL -lGLU -lGL \
	-lXext \
	-lX11 -ldl -lm 

$(DSO) install all: $(OBJECTS)
	$(LD) -shared $(LDOPTS) $(OBJECTS) $(LIBS) -o $(DSO)

include $(COMMONRULES)
