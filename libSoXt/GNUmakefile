IVDEPTH = ..
include $(IVDEPTH)/make/commonivdefs

DEPTH = ..

OBJS = \
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

LDIRT = libInventorXt.so

library: all
	$(LD) -o libInventorXt.so -shared  $(OBJS)

install: library
	@mkdir -p $(IVROOT)/usr/lib
	@cp -f libInventorXt.so $(IVROOT)/usr/lib

SUBDIRS = src include #test

COMMONPREF = xxx

$(COMMONTARGS) all:: 
	$(SUBDIRS_MAKERULE)

clean clobber::
	rm -f $(IVROOT)/usr/lib/libInventorXt.so
	rm -f libInventorXt.so
	$(SUBDIRS_MAKERULE)

