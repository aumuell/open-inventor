DEPTH = ../../..
IVDEPTH = ../../../../..
include $(IVDEPTH)/make/commonivdefs

MANPAGES = \
	SoField.ivm \
	SoFieldContainer.ivm \
	SoFieldList.ivm \
	SoMFBitMask.ivm \
	SoMFBool.ivm \
	SoMFColor.ivm \
	SoMFEngine.ivm \
	SoMFEnum.ivm \
	SoMFFloat.ivm \
	SoMFInt32.ivm \
	SoMFLong.ivm \
	SoMFMatrix.ivm \
	SoMFName.ivm \
	SoMFNode.ivm \
	SoMFPath.ivm \
	SoMFPlane.ivm \
	SoMFRotation.ivm \
	SoMFShort.ivm \
	SoMFString.ivm \
	SoMFTime.ivm \
	SoMFUInt32.ivm \
	SoMFULong.ivm \
	SoMFUShort.ivm \
	SoMFVec2f.ivm \
	SoMFVec3f.ivm \
	SoMFVec4f.ivm \
	SoMField.ivm \
	SoSFBitMask.ivm \
	SoSFBool.ivm \
	SoSFColor.ivm \
	SoSFEngine.ivm \
	SoSFEnum.ivm \
	SoSFFloat.ivm \
	SoSFImage.ivm \
	SoSFInt32.ivm \
	SoSFLong.ivm \
	SoSFMatrix.ivm \
	SoSFName.ivm \
	SoSFNode.ivm \
	SoSFPath.ivm \
	SoSFPlane.ivm \
	SoSFRotation.ivm \
	SoSFShort.ivm \
	SoSFString.ivm \
	SoSFTime.ivm \
	SoSFTrigger.ivm \
	SoSFUInt32.ivm \
	SoSFULong.ivm \
	SoSFUShort.ivm \
	SoSFVec2f.ivm \
	SoSFVec3f.ivm \
	SoSFVec4f.ivm \
	SoSField.ivm

OBJECTS = $(MANPAGES:.ivm=.3iv)

all: $(OBJECTS)

install: all
	@mkdir -p $(IVROOT)/usr/man/man3
	@for i in $(OBJECTS) ; do \
		cp -f $$i $(IVROOT)/usr/man/man3 ; \
	done

clean clobber:
	rm -f $(OBJECTS)
