IVDEPTH = ..
include $(IVDEPTH)/make/ivcommondefs

LIBDSO = libInventorXt.so

LLDLIBS = \
	-lXm -lXt -lXext -lXi -lX11 -lm \
	-lInventor -lGLU -lGL

LLDLIBS += $(shell [ -e $(X11LIBDIR)/libXp.so ] && echo -lXp)

OBJECTS = \
	./src/devices/SoXtFocus.o \
	./src/devices/SoXtKeybd.o \
	./src/devices/SoXtMouse.o \
	./src/devices/SoXtSpball.o \
	./src/lightEditor/SoXtDirLitEd.o \
	./src/mixedMode/SoFloatCBList.o \
	./src/mixedMode/SoXtColEd.o \
	./src/mixedMode/SoXtColPatch.o \
	./src/mixedMode/SoXtColSlider.o \
	./src/mixedMode/SoXtColWheel.o \
	./src/mixedMode/SoXtSlider.o \
	./src/mixedMode/SoXtUIRegion.o \
	./src/motif/SoXtCntlPnl.o \
	./src/motif/SoXtFltTxt.o \
	./src/motif/SoXtLtSldrSet.o \
	./src/motif/SoXtMMSldrMax.o \
	./src/motif/SoXtMMSldrMin.o \
	./src/motif/SoXtMinMax.o \
	./src/motif/SoXtMltSldr.o \
	./src/motif/SoXtMltSldrs.o \
	./src/motif/SoXtMtlSSet.o \
	./src/motif/SoXtSSetBase.o \
	./src/motif/SoXtSldrMdl.o \
	./src/motif/SoXtSldrMdls.o \
	./src/motif/SoXtSldrSet.o \
	./src/motif/SoXtSldrTl.o \
	./src/motif/SoXtTfSldrSet.o \
	./src/mtlEdit/SoXtMtlEdit.o \
	./src/mtlEdit/SoXtMtlList.o \
	./src/viewers/SoXtBitmapButton.o \
	./src/viewers/SoXtConstVwr.o \
	./src/viewers/SoXtCursors.o \
	./src/viewers/SoXtExamVwr.o \
	./src/viewers/SoXtFlyVwr.o \
	./src/viewers/SoXtFullVwr.o \
	./src/viewers/SoXtIcons.o \
	./src/viewers/SoXtPlaneVwr.o \
	./src/viewers/SoXtViewer.o \
	./src/viewers/SoXtWalkVwr.o \
	./src/viewers/Sgm/ThumbWheel.o \
	./src/SoGLwDAUtil.o \
	./src/SoGLwDrawA.o \
	./src/SoGLwMDrawA.o \
	./src/SoXt.o \
	./src/SoXtClipboard.o \
	./src/SoXtComponent.o \
	./src/SoXtGLWidget.o \
	./src/SoXtImportInterest.o \
	./src/SoXtPrintDlog.o \
	./src/SoXtRendrArea.o \
	./src/SoXtRsrc.o \
	./src/sgi_visual.o

LARCHIVE = libInventorXt.a

SUBDIRS = src

all: all_ivbin

HEADERS_SUBDIRS = include

install: install_ivlibdso
	$(HEADERS_SUBDIRS_MAKERULE)

COMMONPREF = xxx
$(COMMONTARGS): %: $(COMMONPREF)%
	$(SUBDIRS_MAKERULE)

include $(IVCOMMONRULES)
