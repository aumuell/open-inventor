IVDEPTH = ../../../../..
include $(IVDEPTH)/make/ivcommondefs

HDRSUBDIR = misc

HEADERS = \
	SoAuditorList.h \
	SoBase.h \
	SoBasic.h \
	SoByteStream.h \
	SoCallbackList.h \
	SoChildList.h \
	SoCompactPathList.h \
	SoNormalGenerator.h \
	SoNotification.h \
	SoState.h \
	SoTempPath.h \
	SoTranscribe.h

HEADERS_SUBDIRS = upgraders

all:

install: install_ivheaders
	$(HEADERS_SUBDIRS_MAKERULE)

COMMONPREF = xxx
$(COMMONTARGS): %: $(COMMONPREF)%
	$(HEADERS_SUBDIRS_MAKERULE)

include $(IVCOMMONRULES)
