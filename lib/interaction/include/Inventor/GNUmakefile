IVDEPTH = ../../../..
include $(IVDEPTH)/make/ivcommondefs

HDRSUBDIR =

HEADERS = SoInteraction.h SoSceneManager.h

HEADERS_SUBDIRS = actions draggers manips nodekits nodes

all:

install: install_ivheaders
	$(HEADERS_SUBDIRS_MAKERULE)

COMMONPREF = xxx
$(COMMONTARGS): %: $(COMMONPREF)%
	$(HEADERS_SUBDIRS_MAKERULE)

include $(IVCOMMONRULES)
