IVDEPTH = ../..
include $(IVDEPTH)/make/ivcommondefs

SUBDIRS = iv2toiv1 ivdowngrade

all install:
	$(SUBDIRS_MAKERULE)

COMMONPREF = xxx
$(COMMONTARGS): %: $(COMMONPREF)%
	$(SUBDIRS_MAKERULE)

include $(IVCOMMONRULES)
