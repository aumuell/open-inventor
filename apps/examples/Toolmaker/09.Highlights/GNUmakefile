IVDEPTH = ../../../..
include $(IVDEPTH)/make/ivcommondefs

PROGRAMS_EXT = selectionTest overlayTest

all install $(COMMONTARGS):
	for i in $(PROGRAMS_EXT); do $(MAKE) -f GNUmakefile.$$i $@; done

COMMONPREF = xxx
include $(IVCOMMONRULES)
