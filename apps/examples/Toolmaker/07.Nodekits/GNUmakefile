IVDEPTH = ../../../..
include $(IVDEPTH)/make/ivcommondefs

PROGRAMS_EXT = jumpingJackKitTest pyramidKitTest

all install $(COMMONTARGS):
	for i in $(PROGRAMS_EXT); do $(MAKE) -f GNUmakefile.$$i $@; done

COMMONPREF = xxx
include $(IVCOMMONRULES)
