IVDEPTH = ../../..
include $(IVDEPTH)/make/commonivdefs

DEPTH = ../..
LDIRT = *.d
LCXXFLAGS= -I$(DEPTH)/include \
	   -DLIBRARYBUILD \
	   -I../../../lib/database/include/ \
	   -I../../../lib/interaction/include \
	   -I../../../lib/nodekits/include 

CXXFILES = ivcat.c++

LLDOPTS = \
	-L../../../libimage \
	-L../../../lib/ \
	-L/usr/X11R6/lib 

ifeq ($(usingLinux),1)
LLDOPTS += -L../../../libFL/src
endif

LLDLIBS  = \
	-lInventor \
	-lFL -lGLU -lGL \
	-lXext -lX11 -ldl -lm

TARGETS = ivcat

$(TARGETS): $(OBJECTS)
	$(CXX) $(LDOPTS) $(OBJECTS) $(LDLIBS) -o $(TARGETS)

install: $(TARGETS)
	@mkdir -p $(IVROOT)/usr/bin
	@cp -f $(TARGETS) $(IVROOT)/usr/bin

include $(COMMONRULES)

