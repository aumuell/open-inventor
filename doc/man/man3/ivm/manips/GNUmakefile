DEPTH = ../../..
IVDEPTH = ../../../../..
include $(IVDEPTH)/make/commonivdefs

MANPAGES = \
	SoCenterballManip.ivm \
	SoDirectionalLightManip.ivm \
	SoHandleBoxManip.ivm \
	SoJackManip.ivm \
	SoPointLightManip.ivm \
	SoSpotLightManip.ivm \
	SoTabBoxManip.ivm \
	SoTrackballManip.ivm \
	SoTransformBoxManip.ivm \
	SoTransformManip.ivm \
	SoTransformerManip.ivm

OBJECTS = $(MANPAGES:.ivm=.3iv)

all: $(OBJECTS)

install: all
	@mkdir -p $(IVROOT)/usr/man/man3
	@for i in $(OBJECTS) ; do \
		cp -f $$i $(IVROOT)/usr/man/man3 ; \
	done

clean clobber:
	rm -f $(OBJECTS)
