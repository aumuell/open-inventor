IVDEPTH = ../../..
include $(IVDEPTH)/make/commonivdefs

DEPTH = ../..
LDIRT = *.d
LCXXFLAGS += -DLIBRARYBUILD \
	    -I$(IVDEPTH)/lib/database/include/ \
	    -I$(IVDEPTH)/lib/interaction/include \
	    -I$(IVDEPTH)/lib/nodekits/include \
	    -O3 \
	    -I../../../libSoXt/include 

TARGETS = SceneViewer

LLDOPTS = \
	-L../../../lib \
	-L/usr/X11R6/lib \
	-L../../../libSoXt \
	-L/usr/X11R6/lib \
	-L../../samples/widgets 

ifeq ($(usingLinux),1)
LLDOPTS += -L../../../libFL/src
endif

LLDLIBS = \
	-lInventorWidget \
	-lInventorXt \
	-lInventor \
	-lXm -lXt -lXi -lX11 \
	-lFL -lGLU -lGL \
	-lXext \
	-lX11 -ldl -lm 

CXXFILES = SceneViewer.c++ SoSceneViewer.c++ SvManipList.c++

$(TARGETS): $(OBJECTS)
	$(CXX) $(LDOPTS) $(OBJECTS) $(LDLIBS) -o $(TARGETS)

install: $(TARGETS)
	@mkdir -p $(IVROOT)/usr/bin
	@mkdir -p $(IVROOT)/opt/SGIDemos/Inventor/bin
	@mkdir -p $(IVROOT)/opt/SGIDemos/Inventor/data
	@cp -f $(TARGETS) $(IVROOT)/usr/bin
	@cp -f $(TARGETS) $(IVROOT)/opt/SGIDemos/Inventor/bin
	@cp -f $(TARGETS).RUNME $(IVROOT)/opt/SGIDemos/Inventor/bin
	@cp -f $(TARGETS).about $(IVROOT)/opt/SGIDemos/Inventor/bin
	@cp -f jackInTheBox.iv $(IVROOT)/opt/SGIDemos/Inventor/data


include $(COMMONRULES)
