IVDEPTH = ../../../..
include $(IVDEPTH)/make/commonivdefs

INST_HEADER_ROOT   = $(IVROOT)/usr/include/Inventor
HEADER_PATH        = $(INST_HEADER_ROOT)
NOSHIP_HEADER_PATH = $(HEADER_PATH)

INST_HEADERS = \
	Sb.h \
	SbBasic.h \
	SbBox.h \
	SbColor.h \
	SbDict.h \
	SbLinear.h \
	SbPList.h \
	SbString.h \
	SbTime.h \
	SbViewportRegion.h \
	So.h \
	SoDB.h \
	SoInput.h \
	SoLists.h \
	SoOffscreenRenderer.h \
	SoOutput.h \
	SoPath.h \
	SoPickedPoint.h \
	SoPrimitiveVertex.h \
	SoType.h


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
	misc \
	nodes \
	projectors \
	sensors


LDIRT = $(INST_HEADER_ROOT)

install: headers
	@for d in $(SUBDIRS); do \
		$(ECHO) "\tcd $$d; $(MAKE) $@"; \
		cd $$d; $(MAKE) $@; cd ..; \
	done

include $(IVCOMMONRULES)
