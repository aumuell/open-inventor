IVDEPTH = ../../..
include $(IVDEPTH)/make/commonivdefs

DEPTH = ../..
LDIRT = *.d
LCXXFLAGS = -DLIBRARYBUILD \
	    -I../../../lib/database/include/ \
  	    -I../../../lib/interaction/include \
	    -I../../../lib/nodekits/include -O3 \
	    -D__NO_MATH_INLINES \
	    -I../../../libSoXt/include

TARGETS = textomatic

LLDOPTS = \
	-L../../../libimage \
	-L../../../lib \
	-L/usr/X11R6/lib \
	-L../../../libSoXt \
	-Wl,-rpath,../../../libSoXt \
	-L../../samples/common \
	-L/usr/X11R6/lib \
	-Wl,-rpath,../../libimage \
	-Wl,-rpath,../../../lib

ifeq ($(usingLinux),1)
LLDOPTS +=	-L../../../libFL/src \
	-Wl,-rpath,../../../libFL/src
endif

LLDLIBS = \
	-limage \
	-lcommon \
	-lInventorXt \
	-lInventor \
	-lXm -lXt -lXi -lX11 \
	-lFL -lGLU -lGL \
	-lXext \
	-lX11 -ldl -lm 

CXXFILES =\
	TextGraph.c++ \
	TextWrapper.c++ \
	textomatic.c++


$(TARGETS): $(OBJECTS)
	$(CXX) $(LDOPTS) $(OBJECTS) $(LDLIBS) -o $(TARGETS)

install: $(TARGETS)
	@mkdir -p $(IVROOT)/opt/SGIDemos/Inventor/bin
	@cp -f $(TARGETS) $(IVROOT)/opt/SGIDemos/Inventor/bin
	@cp -f $(TARGETS).about $(IVROOT)/opt/SGIDemos/Inventor/bin

include $(COMMONRULES)
