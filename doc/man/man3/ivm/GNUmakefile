IVDEPTH = ../../../..
include $(IVDEPTH)/make/ivcommondefs

SUBDIRS = \
	actions \
	basics \
	components \
	details \
	devices \
	draggers \
	engines \
	errors \
	events \
	fields \
	general \
	interaction \
	manips \
	nodekits \
	nodes \
	projectors \
	sensors \
	viewers

all install:
	$(SUBDIRS_MAKERULE)

COMMONPREF = xxx
$(COMMONTARGS): %: $(COMMONPREF)%
	$(SUBDIRS_MAKERULE)

include $(IVCOMMONRULES)
