IVDEPTH = ../../../..
include $(IVDEPTH)/make/ivcommondefs

HDRSUBDIR =

HEADERS = \
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

HEADERS_SUBDIRS = \
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

all:

install: install_ivheaders
	$(HEADERS_SUBDIRS_MAKERULE)

COMMONPREF = xxx
$(COMMONTARGS): %: $(COMMONPREF)%
	$(HEADERS_SUBDIRS_MAKERULE)

include $(IVCOMMONRULES)
