IVDEPTH = ../../../..
include $(IVDEPTH)/make/ivcommondefs

CXXFILES = Sb.c++

SUBDIRS = projectors

all install: $(OBJECTS)
	$(SUBDIRS_MAKERULE)

COMMONPREF = xxx
$(COMMONTARGS): %: $(COMMONPREF)%
	$(SUBDIRS_MAKERULE)

include $(IVCOMMONRULES)
