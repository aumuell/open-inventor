IVDEPTH = ../../../..
include $(IVDEPTH)/make/ivcommondefs

PROGRAMS_EXT = viewer tumble

all install $(COMMONTARGS):
	for i in $(PROGRAMS_EXT); do $(MAKE) -f GNUmakefile.$$i $@; done

COMMONPREF = xxx
include $(IVCOMMONRULES)
