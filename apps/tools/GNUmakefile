IVDEPTH = ../..
include $(IVDEPTH)/make/ivcommondefs

SUBDIRS = ivcat ivfix ivinfo ivquicken ivview

all install:
	$(SUBDIRS_MAKERULE)

COMMONPREF = xxx
$(COMMONTARGS): %: $(COMMONPREF)%
	$(SUBDIRS_MAKERULE)

include $(IVCOMMONRULES)
