IVDEPTH = ../..
include $(IVDEPTH)/make/ivcommondefs

HEADERS_SUBDIRS = Inventor/Xt

all:

install:
	$(HEADERS_SUBDIRS_MAKERULE)

COMMONPREF = xxx
$(COMMONTARGS): %: $(COMMONPREF)%
	$(HEADERS_SUBDIRS_MAKERULE)

include $(IVCOMMONRULES)
