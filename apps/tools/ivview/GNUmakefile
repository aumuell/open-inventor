IVDEPTH = ../../..
include $(IVDEPTH)/make/commonivdefs

DEPTH = ../..
LDIRT = *.d
LCXXFLAGS += -I/tmp/include \
	    -I$(DEPTH)/include \
	    -DLIBRARYBUILD \
	    -I../../../lib/database/include/ \
	    -I../../../lib/interaction/include \
	    -I../../../lib/nodekits/include \
            -I../../../libSoXt/include

CXXFILES = ivview.c++

TARGETS = ivview

LLDOPTS = \
	-L../../../libimage \
	-L../../../lib/ \
	-L../../samples/widgets \
	-L../../../libSoXt/ \
	-L/usr/X11R6/lib \
	-L/usr/X11R6/lib \

ifeq ($(usingLinux),1)
LLDOPTS += -L../../../libFL/src
endif

LLDLIBS = \
	-lInventorWidget \
	-lInventorXt -lInventor \
	-limage \
	-lFL -lGLU -lGL \
	-lXm -lXt -lXext -lXmu -lXi -lX11 -ldl -lm

$(TARGETS): $(OBJECTS)
	$(CXX) $(LDOPTS) $(OBJECTS) $(LDLIBS) -o $(TARGETS)

install: $(TARGETS)
	@mkdir -p $(IVROOT)/usr/bin
	@cp -f $(TARGETS) $(IVROOT)/usr/bin

include $(COMMONRULES)

