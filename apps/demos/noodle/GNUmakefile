IVDEPTH = ../../..
include $(IVDEPTH)/make/commonivdefs

DEPTH = ../..
LDIRT = *.d
LCXXFLAGS = -DLIBRARYBUILD \
	    -I../../samples/common/ \
	    -I../../nodes/GeneralizedCylinder/ \
	    -I../../../lib/database/include/ \
  	    -I../../../lib/interaction/include \
	    -I../../../lib/nodekits/include -O3 \
	    -I../../../libSoXt/include 

TARGETS = noodle

ifeq ($(usingLinux),1)
CYLLDPATH=-Wl,-rpath,../../nodes/GeneralizedCylinder
else
CYLLDPATH=-rpath ../../nodes/GeneralizedCylinder
endif
LLDOPTS = \
	-L../../../libimage \
	-L../../../lib \
	-L/usr/X11R6/lib \
	-L../../../libSoXt \
        -L../../nodes/GeneralizedCylinder \
	-L../../samples/common \
	-L/usr/X11R6/lib \
	$(CYLLDPATH)

ifeq ($(usingLinux),1)
LLDOPTS += -L../../../libFL/src
endif

LLDLIBS = \
	-limage \
	-lInventorXt \
	-lInventor \
	-lcommon \
	-lGeneralizedCylinder \
	-lXm -lXt -lXi -lX11 \
	-lFL -lGLU -lGL \
	-lXext \
	-lX11 -ldl -lm 

CXXFILES =\
	Interface.c++ \
	LineManip.c++ \
	LineManipHilight.c++ \
	NoodleSlider.c++ \
	NoodleSurfaceGizmo.c++ \
	NoodleTextureGizmo.c++ \
	WorldInfo.c++ \
	noodle.c++ \
	profile.c++


$(TARGETS): $(OBJECTS)
	$(CXX) $(LDOPTS) $(OBJECTS) $(LDLIBS) -o $(TARGETS)

install: $(TARGETS)
	@mkdir -p $(IVROOT)/opt/SGIDemos/Inventor/bin
	@mkdir -p $(IVROOT)/opt/SGIDemos/Inventor/data
	@cp -f $(TARGETS) $(IVROOT)/opt/SGIDemos/Inventor/bin
	@cp -f $(TARGETS).RUNME $(IVROOT)/opt/SGIDemos/Inventor/bin
	@cp -f $(TARGETS).about $(IVROOT)/opt/SGIDemos/Inventor/data
	@cp -f defaultTexture.rgb $(IVROOT)/opt/SGIDemos/Inventor/data

include $(COMMONRULES)
