DEPTH = ../../..
IVDEPTH = ../../../../..
include $(IVDEPTH)/make/commonivdefs

MANPAGES = \
	SoBase.ivm \
	SoBaseList.ivm \
	SoByteStream.ivm \
	SoCallbackList.ivm \
	SoDB.ivm \
	SoInput.ivm \
	SoNodeKitPath.ivm \
	SoNodeList.ivm \
	SoOffscreenRenderer.ivm \
	SoOutput.ivm \
	SoPath.ivm \
	SoPathList.ivm \
	SoPickedPoint.ivm \
	SoPickedPointList.ivm \
	SoPrimitiveVertex.ivm \
	SoTranReceiver.ivm \
	SoTranSender.ivm \
	SoType.ivm \
	SoTypeList.ivm

OBJECTS = $(MANPAGES:.ivm=.3iv)

all: $(OBJECTS)

install: all
	@mkdir -p $(IVROOT)/usr/man/man3
	@for i in $(OBJECTS) ; do \
		cp -f $$i $(IVROOT)/usr/man/man3 ; \
	done

clean clobber:
	rm -f $(OBJECTS)
