DEPTH = ../../..
IVDEPTH = ../../../../..
include $(IVDEPTH)/make/commonivdefs

MANPAGES = \
	SoXtConstrainedViewer.ivm \
	SoXtExaminerViewer.ivm \
	SoXtFlyViewer.ivm \
	SoXtFullViewer.ivm \
	SoXtPlaneViewer.ivm \
	SoXtViewer.ivm \
	SoXtWalkViewer.ivm

OBJECTS = $(MANPAGES:.ivm=.3iv)

all: $(OBJECTS)

install: all
	@mkdir -p $(IVROOT)/usr/man/man3
	@for i in $(OBJECTS) ; do \
		cp -f $$i $(IVROOT)/usr/man/man3 ; \
	done

clean clobber:
	rm -f $(OBJECTS)
