IVDEPTH = ../..
include $(IVDEPTH)/make/ivcommondefs

SUBDIRS = tools ivman man1 man3

all install:
	$(SUBDIRS_MAKERULE)

COMMONPREF = xxx
$(COMMONTARGS): %: $(COMMONPREF)%
	$(SUBDIRS_MAKERULE)

include $(IVCOMMONRULES)
