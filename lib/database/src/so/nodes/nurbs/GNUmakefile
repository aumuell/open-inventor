IVDEPTH = ../../../../../..
include $(IVDEPTH)/make/ivcommondefs

LCXXINCS += -Ihead
LCXXDEFS += -DLIBRARYBUILD

CXXFILES = SoNurbs.c++

SUBDIRS = clients libnurbs

all install: $(OBJECTS)
	$(SUBDIRS_MAKERULE)

COMMONPREF = xxx
$(COMMONTARGS): %: $(COMMONPREF)%
	$(SUBDIRS_MAKERULE)

include $(IVCOMMONRULES)
