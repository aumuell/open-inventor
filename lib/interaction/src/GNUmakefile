IVDEPTH = ../../..
include $(IVDEPTH)/make/ivcommondefs

CXXFILES =  \
	SoInteraction.c++ SoSceneMgr.c++ \
	SoSelection.c++

SUBDIRS = actions draggers manips nodekits nodes

all install: $(OBJECTS)
	$(SUBDIRS_MAKERULE)

COMMONPREF = xxx
$(COMMONTARGS): %: $(COMMONPREF)%
	$(SUBDIRS_MAKERULE)

include $(IVCOMMONRULES)
