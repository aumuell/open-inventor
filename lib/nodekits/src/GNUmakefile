IVDEPTH = ../../..
include $(IVDEPTH)/make/ivcommondefs

CXXFILES = SoNodeKitPath.c++

SUBDIRS = details nodekits upgraders

all install: $(OBJECTS)
	$(SUBDIRS_MAKERULE)

COMMONPREF = xxx
$(COMMONTARGS): %: $(COMMONPREF)%
	$(SUBDIRS_MAKERULE)

include $(IVCOMMONRULES)
