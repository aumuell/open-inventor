IVDEPTH = ../../..
include $(IVDEPTH)/make/commonivdefs

DEPTH = ../../..
LDIRT = *.d libcommon.a
LCXXFLAGS = -I$(DEPTH)/include \
	    -DLIBRARYBUILD \
	    -I$(DEPTH)/lib/database/include/ \
	    -I$(DEPTH)/lib/interaction/include \
	    -I$(DEPTH)/nodekits/include \
	    -I$(DEPTH)/libSoXt/include \
	    -I$(DEPTH)/libimage 

LLDOPTS =\
	-L$(DEPTH)/libimage -L$(DEPTH)/lib/ \
	-L/usr/X11R6/lib -L$(DEPTH)/libSoXt \
	-L$(DEPTH)/libFL/src \
	-L/usr/X11R6/lib 

LLDLIBS = \
	-lInventorXt -lInventor \
	-limage \
	-lGLw -L/usr/X11R6/lib -lXm -lXt -lXi -lX11 \
	-lFL -lGLU -lGL \
	-lXext \
	-lX11 -ldl -lm

all:: libcommon.a

CXXFILES=\
	LineManip.c++ \
	LineManipHilight.c++ \
	Useful.c++


libcommon.a: $(OBJECTS)
	ar clq $@ $(OBJECTS)

install: libcommon.a

include $(COMMONRULES)
