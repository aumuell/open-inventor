IVDEPTH = ../../../../..
include $(IVDEPTH)/make/commonivdefs

INST_HEADER_ROOT   = $(IVROOT)/usr/include/Inventor/misc
HEADER_PATH        = $(INST_HEADER_ROOT)
NOSHIP_HEADER_PATH = $(HEADER_PATH)

INST_HEADERS = \
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


SUBDIRS = \
	upgraders

LDIRT = $(INST_HEADER_ROOT)

install: headers
	@for d in $(SUBDIRS); do \
		$(ECHO) "\tcd $$d; $(MAKE) $@"; \
		cd $$d; $(MAKE) $@; cd ..; \
	done

include $(IVCOMMONRULES)
