DEPTH = ../../..
IVDEPTH = ../../../../..
include $(IVDEPTH)/make/commonivdefs

MANPAGES = \
	SoXt.ivm \
	SoXtClipboard.ivm \
	SoXtComponent.ivm \
	SoXtDirectionalLightEditor.ivm \
	SoXtGLWidget.ivm \
	SoXtLightSliderSet.ivm \
	SoXtMaterialEditor.ivm \
	SoXtMaterialList.ivm \
	SoXtMaterialSliderSet.ivm \
	SoXtPrintDialog.ivm \
	SoXtRenderArea.ivm \
	SoXtResource.ivm \
	SoXtSliderSet.ivm \
	SoXtSliderSetBase.ivm \
	SoXtTransformSliderSet.ivm

OBJECTS = $(MANPAGES:.ivm=.3iv)

all: $(OBJECTS)

install: all
	@mkdir -p $(IVROOT)/usr/man/man3
	@for i in $(OBJECTS) ; do \
		cp -f $$i $(IVROOT)/usr/man/man3 ; \
	done

clean clobber:
	rm -f $(OBJECTS)
