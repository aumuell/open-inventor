IVDEPTH = ../../..
include $(IVDEPTH)/make/commonivdefs

DEPTH = ../..
CXXFLAGS += -I$(DEPTH)/include \
	   -I../../../lib/database/include/ \
	   -I../../../lib/interaction/include \
	   -I../../../lib/nodekits/include 

TARGETS = ivnorm

CXXFILES = \
	Edges.c++ \
	Faces.c++ \
	FindNormals.c++ \
	ivnorm.c++
	
LLDOPTS = -L../../../libimage \
	-L../../../lib/ \
	-L/usr/X11R6/lib 

ifeq ($(usingLinux),1)
LLDOPTS += -L../../../libFL/src
endif

LLDLIBS = \
	-lInventor \
	-limage \
	-lFL -lGLU -lGL \
	-lXext -lX11 -ldl -lm

$(TARGETS): $(OBJECTS)
	$(CXX) $(LDOPTS) $(OBJECTS) $(LDLIBS) -o $(TARGETS)

install: $(TARGETS)
	@mkdir -p $(IVROOT)/usr/bin
	@cp -f $(TARGETS) $(IVROOT)/usr/bin

include $(COMMONRULES)

