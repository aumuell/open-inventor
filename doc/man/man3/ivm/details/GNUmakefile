DEPTH = ../../..
IVDEPTH = ../../../../..
include $(IVDEPTH)/make/commonivdefs

MANPAGES = \
	SoConeDetail.ivm \
	SoCubeDetail.ivm \
	SoCylinderDetail.ivm \
	SoDetail.ivm \
	SoDetailList.ivm \
	SoFaceDetail.ivm \
	SoLineDetail.ivm \
	SoNodeKitDetail.ivm \
	SoPointDetail.ivm \
	SoTextDetail.ivm

OBJECTS = $(MANPAGES:.ivm=.3iv)

all: $(OBJECTS)

install: all
	@mkdir -p $(IVROOT)/usr/man/man3
	@for i in $(OBJECTS) ; do \
		cp -f $$i $(IVROOT)/usr/man/man3 ; \
	done

clean clobber:
	rm -f $(OBJECTS)
