DEPTH = ../../..
IVDEPTH = ../../../../..
include $(IVDEPTH)/make/commonivdefs

MANPAGES = \
	SbBool.ivm \
        SbBox2f.ivm \
        SbBox2s.ivm \
        SbBox3f.ivm \
        SbColor.ivm \
        SbCylinder.ivm \
        SbLine.ivm \
        SbMatrix.ivm \
        SbName.ivm \
        SbPList.ivm \
        SbPlane.ivm \
        SbRotation.ivm \
        SbSphere.ivm \
        SbString.ivm \
        SbTime.ivm \
        SbVec2f.ivm \
        SbVec2s.ivm \
        SbVec3f.ivm \
        SbVec4f.ivm \
        SbViewVolume.ivm \
        SbViewportRegion.ivm \
        SbXfBox3f.ivm

OBJECTS = $(MANPAGES:.ivm=.3iv)

all: $(OBJECTS)

install: all
	@mkdir -p $(IVROOT)/usr/man/man3
	@for i in $(OBJECTS) ; do \
		cp -f $$i $(IVROOT)/usr/man/man3 ; \
	done

clean clobber:
	rm -f $(OBJECTS)
