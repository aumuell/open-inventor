DEPTH = ../../..
IVDEPTH = ../../../../..
include $(IVDEPTH)/make/commonivdefs

MANPAGES = \
	SoBoolOperation.ivm \
	SoCalculator.ivm \
	SoComposeMatrix.ivm \
	SoComposeRotation.ivm \
	SoComposeRotationFromTo.ivm \
	SoComposeVec2f.ivm \
	SoComposeVec3f.ivm \
	SoComposeVec4f.ivm \
	SoComputeBoundingBox.ivm \
	SoConcatenate.ivm \
	SoCounter.ivm \
	SoDecomposeMatrix.ivm \
	SoDecomposeRotation.ivm \
	SoDecomposeVec2f.ivm \
	SoDecomposeVec3f.ivm \
	SoDecomposeVec4f.ivm \
	SoElapsedTime.ivm \
	SoEngine.ivm \
	SoEngineList.ivm \
	SoEngineOutput.ivm \
	SoEngineOutputList.ivm \
	SoGate.ivm \
	SoInterpolate.ivm \
	SoInterpolateFloat.ivm \
	SoInterpolateRotation.ivm \
	SoInterpolateVec2f.ivm \
	SoInterpolateVec3f.ivm \
	SoInterpolateVec4f.ivm \
	SoOnOff.ivm \
	SoOneShot.ivm \
	SoSelectOne.ivm \
	SoTimeCounter.ivm \
	SoTransformVec3f.ivm \
	SoTriggerAny.ivm

OBJECTS = $(MANPAGES:.ivm=.3iv)

all: $(OBJECTS)

install: all
	@mkdir -p $(IVROOT)/usr/man/man3
	@for i in $(OBJECTS) ; do \
		cp -f $$i $(IVROOT)/usr/man/man3 ; \
	done

clean clobber:
	rm -f $(OBJECTS)
