IVDEPTH = ../..
include $(IVDEPTH)/make/ivcommondefs

SUBDIRS = \
	SceneViewer \
	drop \
	gview \
	maze \
	noodle \
	qmorf \
	revo \
	textomatic

all install:
	$(SUBDIRS_MAKERULE)

COMMONPREF = xxx
$(COMMONTARGS): %: $(COMMONPREF)%
	$(SUBDIRS_MAKERULE)

include $(IVCOMMONRULES)
