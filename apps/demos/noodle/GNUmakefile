IVDEPTH = ../../..
include $(IVDEPTH)/make/ivcommondefs

PROGRAM = noodle
DEMO = $(PROGRAM)

LCXXINCS += -I../../nodes/GeneralizedCylinder

CXXFILES = \
	NoodleSlider.c++ \
	Interface.c++ NoodleSurfaceGizmo.c++ WorldInfo.c++ LineManip.c++ \
	NoodleTextureGizmo.c++ noodle.c++ LineManipHilight.c++  \
	profile.c++

LLDLIBS = \
	../../nodes/GeneralizedCylinder/NurbMaker.o \
	../../nodes/GeneralizedCylinder/Triangulator.o \
	../../nodes/GeneralizedCylinder/GeneralizedCylinder.o \
	-lInventor -lInventorXt

all: all_ivbin

install: install_ivdemo
	$(INSTALL) -m 644 defaultTexture.rgb $(IVDEMODATADIR)

include $(IVCOMMONRULES)
