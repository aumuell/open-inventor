IVDEPTH = ../..
include $(IVDEPTH)/make/ivcommondefs

CXXFILES = \
	SoXt.c++ SoXtRsrc.c++ \
	SoXtComponent.c++ SoXtGLWidget.c++ \
	SoXtRendrArea.c++ \
	SoXtClipboard.c++ SoXtPrintDlog.c++ \
	SoXtImportInterest.c++

CFILES = \
	SoGLwDAUtil.c \
	SoGLwDrawA.c \
	SoGLwMDrawA.c \
	sgi_visual.c

SUBDIRS = \
	devices mixedMode viewers \
	lightEditor mtlEdit motif

all install: $(OBJECTS) 
	$(SUBDIRS_MAKERULE)

COMMONPREF = xxx
$(COMMONTARGS): %: $(COMMONPREF)%
	$(SUBDIRS_MAKERULE)

include $(IVCOMMONRULES)
