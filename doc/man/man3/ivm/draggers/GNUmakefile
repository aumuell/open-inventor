
.SUFFIXES: .ivm .3iv


MANPAGES = \
	SoCenterballDragger.ivm \
	SoDirectionalLightDragger.ivm \
	SoDragPointDragger.ivm \
	SoDragger.ivm \
	SoHandleBoxDragger.ivm \
	SoJackDragger.ivm \
	SoPointLightDragger.ivm \
	SoRotateCylindricalDragger.ivm \
	SoRotateDiscDragger.ivm \
	SoRotateSphericalDragger.ivm \
	SoScale1Dragger.ivm \
	SoScale2Dragger.ivm \
	SoScale2UniformDragger.ivm \
	SoScaleUniformDragger.ivm \
	SoSpotLightDragger.ivm \
	SoTabBoxDragger.ivm \
	SoTabPlaneDragger.ivm \
	SoTrackballDragger.ivm \
	SoTransformBoxDragger.ivm \
	SoTransformerDragger.ivm \
	SoTranslate1Dragger.ivm \
	SoTranslate2Dragger.ivm


OBJECTS = $(MANPAGES:.ivm=.3iv)

all: $(OBJECTS)

.ivm.3iv:
	../../../ivman/ivman $< > $@

install: all
	@mkdir -p $(IVROOT)/usr/man/man3
	@for i in $(OBJECTS) ; do \
		cp -f $$i $(IVROOT)/usr/man/man3 ; \
	done

clean clobber:
	rm -f $(OBJECTS)

