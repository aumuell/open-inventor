IVDEPTH = ../../..
include $(IVDEPTH)/make/commonivdefs

DEPTH = ../..
LDIRT = *.d
LCXXFLAGS = -I$(DEPTH)/include \
	    -I../../../lib/database/include/ \
	    -I../../../lib/interaction/include \
	    -I../../../lib/nodekits/include 

TARGETS = ivfix

CXXFILES = ivfix.c++ 	\
	IfBuilder.c++	\
	IfCollector.c++	\
	IfCondenser.c++	\
	IfFixer.c++	\
	IfFlattener.c++	\
	IfHasher.c++	\
	IfHolder.c++	\
	IfMerger.c++	\
	IfReplacer.c++	\
	IfReporter.c++	\
	IfShape.c++	\
	IfShapeList.c++	\
	IfSorter.c++	\
	IfStripper.c++	\
	IfTypes.c++	\
	IfWeeder.c++


OTHERSOURCE = README

LLDOPTS = \
	-L../../../libimage \
	-L../../../lib/ \
	-L../../../libFL/src \
	-L/usr/X11R6/lib 

LLDLIBS = \
	-lFL -lGLU -lGL \
	-limage \
	-lInventor \
	-lXext -lX11 -ldl -lm

$(TARGETS): $(OBJECTS)
	$(CXX) $(LDOPTS) $(OBJECTS) $(LDLIBS) -o $(TARGETS)

install: $(TARGETS)
	@mkdir -p $(IVROOT)/usr/bin
	@cp -f $(TARGETS) $(IVROOT)/usr/bin

include $(COMMONRULES)

