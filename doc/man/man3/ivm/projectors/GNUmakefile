DEPTH = ../../..
IVDEPTH = ../../../../..
include $(IVDEPTH)/make/commonivdefs

MANPAGES = \
	SbCylinderPlaneProjector.ivm \
	SbCylinderProjector.ivm \
	SbCylinderSectionProjector.ivm \
	SbCylinderSheetProjector.ivm \
	SbLineProjector.ivm \
	SbPlaneProjector.ivm \
	SbProjector.ivm \
	SbSpherePlaneProjector.ivm \
	SbSphereProjector.ivm \
	SbSphereSectionProjector.ivm \
	SbSphereSheetProjector.ivm

OBJECTS = $(MANPAGES:.ivm=.3iv)

all: $(OBJECTS)

install: all
	@mkdir -p $(IVROOT)/usr/man/man3
	@for i in $(OBJECTS) ; do \
		cp -f $$i $(IVROOT)/usr/man/man3 ; \
	done

clean clobber:
	rm -f $(OBJECTS)
