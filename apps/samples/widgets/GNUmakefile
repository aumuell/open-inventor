IVDEPTH = ../../..
include $(IVDEPTH)/make/ivcommondefs

PROGRAMS_EXT = libInventorWidget.a componentTest

all install $(COMMONTARGS):
	for i in $(PROGRAMS_EXT); do $(MAKE) -f GNUmakefile.$$i $@; done

COMMONPREF = xxx
include $(IVCOMMONRULES)
