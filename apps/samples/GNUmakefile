IVDEPTH = ../..
include $(IVDEPTH)/make/ivcommondefs

SUBDIRS = common widgets

all install:
	$(SUBDIRS_MAKERULE)

COMMONPREF = xxx
$(COMMONTARGS): %: $(COMMONPREF)%
	$(SUBDIRS_MAKERULE)

include $(IVCOMMONRULES)
