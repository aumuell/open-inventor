IVDEPTH = ../../..
include $(IVDEPTH)/make/commonivdefs

DEPTH = ../..
LDIRT = *.d
LCXXFLAGS += -DLIBRARYBUILD \
	    -I../../../lib/database/include/ \
  	    -I../../../lib/interaction/include \
	    -I../../../lib/nodekits/include -O3 \
	    -I../../../libSoXt/include \

TARGETS = gview

LLDOPTS = \
	-L../../../libimage \
	-L../../../lib \
	-L/usr/X11R6/lib \
	-L../../../libSoXt \
	-L/usr/X11R6/lib \

ifeq ($(usingLinux),1)
LLDOPTS += -L../../../libFL/src
endif

LLDLIBS = \
	-limage \
	-lInventorXt \
	-lInventor \
	-lXm -lXt -lXi -lX11 \
	-lFL -lGLU -lGL \
	-lXext \
	-lX11 -ldl -lm 

CXXFILES =\
	gview.c++ \
	DisplayGraph.c++ \
	Error.c++ \
	FieldEditor.c++ \
	GraphIcon.c++ \
	GraphViewer.c++ \
	NodeCreator.c++

$(TARGETS): $(OBJECTS)
	$(CXX) $(LDOPTS) $(OBJECTS) $(LDLIBS) -o $(TARGETS)

install: $(TARGETS)
	@mkdir -p $(IVROOT)/opt/SGIDemos/Inventor/bin
	@mkdir -p $(IVROOT)/opt/SGIDemos/Inventor/data
	@cp -f $(TARGETS) $(IVROOT)/opt/SGIDemos/Inventor/bin
	@cp -f $(TARGETS).RUNME $(IVROOT)/opt/SGIDemos/Inventor/bin
	@cp -f windmill.iv $(IVROOT)/opt/SGIDemos/Inventor/data
	@cp -f gviewIcons.iv $(IVROOT)/opt/SGIDemos/Inventor/data

include $(COMMONRULES)
