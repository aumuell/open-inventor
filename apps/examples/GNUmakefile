IVDEPTH = ../..
include $(IVDEPTH)/make/ivcommondefs

SUBDIRS = Mentor Toolmaker

all install:
	$(SUBDIRS_MAKERULE)

COMMONPREF = xxx
$(COMMONTARGS): %: $(COMMONPREF)%
	$(SUBDIRS_MAKERULE)

include $(IVCOMMONRULES)
