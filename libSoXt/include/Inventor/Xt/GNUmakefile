IVDEPTH = ../../../..
include $(IVDEPTH)/make/ivcommondefs

HDRSUBDIR = Xt

HEADERS = \
	SoXt.h \
	SoXtCursors.h \
	SoXtClipboard.h \
	SoXtComponent.h \
	SoXtDirectionalLightEditor.h \
	SoXtGLWidget.h \
	SoXtIcons.h \
	SoXtLightSliderSet.h \
	SoXtMaterialEditor.h \
	SoXtMaterialList.h \
	SoXtMaterialSliderSet.h \
	SoXtPrintDialog.h \
	SoXtRenderArea.h \
	SoXtResource.h \
	SoXtSliderSet.h \
	SoXtSliderSetBase.h \
	SoXtTransformSliderSet.h

HEADERS_SUBDIRS = devices viewers

all:

install: install_ivheaders
	$(HEADERS_SUBDIRS_MAKERULE)

COMMONPREF = xxx
$(COMMONTARGS): %: $(COMMONPREF)%
	$(HEADERS_SUBDIRS_MAKERULE)

include $(IVCOMMONRULES)
