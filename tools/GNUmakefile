IVDEPTH = ..
include $(IVDEPTH)/make/ivcommondefs

SUBDIRS = ppp

all install:
	$(SUBDIRS_MAKERULE)

COMMONPREF = xxx
$(COMMONTARGS): %: $(COMMONPREF)%
	$(SUBDIRS_MAKERULE)

include $(IVCOMMONRULES)
