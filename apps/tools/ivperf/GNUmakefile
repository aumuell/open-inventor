IVDEPTH = ../../..
include $(IVDEPTH)/make/commonivdefs

DEPTH = ../..
CXXFLAGS = -I$(DEPTH)/include \
	   -I../../../lib/database/include/ \
	   -I../../../lib/interaction/include \
	   -I../../../lib/nodekits/include \
	   -I../../nodes/BarChart \
	   -I../../../libSoXt/include

TARGETS = ivperf

CXXFILES = ivperf.c++

ifeq ($OSTYPE,"Linux")
  BARLD=-Wl,-rpath,../../nodes/BarChart
else
  BARLD=-rpath ../../nodes/BarChart
endif
	
LLDOPTS = -L../../../libimage \
	-L../../../lib/ \
	-L/usr/X11R6/lib \
	-L../../nodes/BarChart \
        -L../../../libSoXt \
        -Wl,-rpath,../../../libSoXt \
	$(BARLD)

ifeq ($(usingLinux),1)
LLDOPTS += -L../../../libFL/src
endif

LLDLIBS = \
	-lBarChart \
	-lInventor \
	-lInventorXt \
	-limage \
	-lFL -lGLU -lGL \
	-lXm -lXt -lXext -lXi -lX11 -ldl -lm

$(TARGETS): $(OBJECTS)
	$(CXX) $(LDOPTS) $(OBJECTS) $(LDLIBS) -o $(TARGETS)

install: $(TARGETS)
	@mkdir -p $(IVROOT)/usr/bin
	@cp -f $(TARGETS) $(IVROOT)/usr/bin

include $(COMMONRULES)

