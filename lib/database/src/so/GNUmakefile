IVDEPTH = ../../../..
include $(IVDEPTH)/make/ivcommondefs

CXXFILES = So.c++ SoState.c++ linux.c++

SUBDIRS = \
	actions \
	bundles \
	caches \
	details \
	elements \
	engines \
	errors \
	events \
	fields \
	nodes \
	sensors \
	upgraders

all install: $(OBJECTS)
	$(SUBDIRS_MAKERULE)

COMMONPREF = xxx
$(COMMONTARGS): %: $(COMMONPREF)%
	$(SUBDIRS_MAKERULE)

include $(IVCOMMONRULES)
