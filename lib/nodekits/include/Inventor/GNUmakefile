IVDEPTH = ../../../..
include $(IVDEPTH)/make/ivcommondefs

HDRSUBDIR =

HEADERS = SoNodeKitPath.h

HEADERS_SUBDIRS = details misc nodekits

all:

install: install_ivheaders
	$(HEADERS_SUBDIRS_MAKERULE)

COMMONPREF = xxx
$(COMMONTARGS): %: $(COMMONPREF)%
	$(HEADERS_SUBDIRS_MAKERULE)

include $(IVCOMMONRULES)
