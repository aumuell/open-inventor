IVDEPTH = ..
include $(IVDEPTH)/make/ivcommondefs

SUBDIRS = samples nodes tools converters demos

all install:
	$(SUBDIRS_MAKERULE)

COMMONPREF = xxx
$(COMMONTARGS): %: $(COMMONPREF)%
	$(SUBDIRS_MAKERULE)

include $(IVCOMMONRULES)
