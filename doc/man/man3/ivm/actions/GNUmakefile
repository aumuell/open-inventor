DEPTH = ../../..
IVDEPTH = ../../../../..
include $(IVDEPTH)/make/commonivdefs

MANPAGES = \
	SoAction.ivm \
	SoCallbackAction.ivm \
	SoGLRenderAction.ivm \
	SoGetBoundingBoxAction.ivm \
	SoGetMatrixAction.ivm \
	SoHandleEventAction.ivm \
	SoPickAction.ivm \
	SoRayPickAction.ivm \
	SoSearchAction.ivm \
	SoWriteAction.ivm

OBJECTS = $(MANPAGES:.ivm=.3iv)

all: $(OBJECTS)

install: all
	@mkdir -p $(IVROOT)/usr/man/man3
	@for i in $(OBJECTS) ; do \
		cp -f $$i $(IVROOT)/usr/man/man3 ; \
	done

clean clobber:
	rm -f $(OBJECTS)
