IVDEPTH = ../../..
include $(IVDEPTH)/make/ivcommondefs

CXXFILES = \
	SoXtCursors.c++ \
	SoXtIcons.c++ \
	SoXtBitmapButton.c++ \
	SoXtViewer.c++ \
	SoXtFullVwr.c++ \
	SoXtExamVwr.c++ \
	SoXtPlaneVwr.c++ \
	SoXtConstVwr.c++ \
	SoXtWalkVwr.c++ \
	SoXtFlyVwr.c++

SUBDIRS = Sgm

all install: $(OBJECTS)
	$(SUBDIRS_MAKERULE)

COMMONPREF = xxx
$(COMMONTARGS): %: $(COMMONPREF)%
	$(SUBDIRS_MAKERULE)

include $(IVCOMMONRULES)
