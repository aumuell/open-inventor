IVDEPTH = ../../..
include $(IVDEPTH)/make/ivcommondefs

SUBDIRS = sb so

all install:
	$(SUBDIRS_MAKERULE)

COMMONPREF = xxx
$(COMMONTARGS): %: $(COMMONPREF)%
	$(SUBDIRS_MAKERULE)

include $(IVCOMMONRULES)
