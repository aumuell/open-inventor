IVDEPTH = ../../..
include $(IVDEPTH)/make/ivcommondefs

PROGRAM = SceneViewer
DEMO = $(PROGRAM)

CXXFILES = SceneViewer.c++ SoSceneViewer.c++ SvManipList.c++

LLDOPTS += -L../../samples/widgets

LLDLIBS = -lInventorWidget -lInventor -lInventorXt

all: all_ivbin

install: install_ivbin install_ivdemo
	$(INSTALL) -m 644 jackInTheBox.iv $(IVDEMODATADIR)

include $(IVCOMMONRULES)
