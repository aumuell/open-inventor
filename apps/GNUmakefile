IVDEPTH = ..
include $(IVDEPTH)/make/ivcommondefs

SUBDIRS = samples tools converters nodes demos

all install:
	$(SUBDIRS_MAKERULE)

COMMONPREF = xxx
$(COMMONTARGS): %: $(COMMONPREF)%
	$(SUBDIRS_MAKERULE)

include $(IVCOMMONRULES)
