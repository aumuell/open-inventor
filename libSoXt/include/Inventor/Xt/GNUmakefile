IVDEPTH = ../../../..
include $(IVDEPTH)/make/commonivdefs

INST_HEADER_ROOT   = $(IVROOT)/usr/include/Inventor/Xt
HEADER_PATH        = $(INST_HEADER_ROOT)
NOSHIP_HEADER_PATH = $(HEADER_PATH)

INST_HEADERS = \
	SoXt.h \
	SoXtClipboard.h \
	SoXtComponent.h \
	SoXtCursors.h \
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


SUBDIRS = \
	devices \
	viewers

LDIRT = $(INST_HEADER_ROOT)

install: headers
	@for d in $(SUBDIRS); do \
		$(ECHO) "\tcd $$d; $(MAKE) $@"; \
		cd $$d; $(MAKE) $@; cd ..; \
	done

include $(IVCOMMONRULES)
