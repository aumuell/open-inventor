
.SUFFIXES: .ivm .3iv

MANPAGES = \
	SoAppearanceKit.ivm \
	SoBaseKit.ivm \
	SoCameraKit.ivm \
	SoInteractionKit.ivm \
	SoLightKit.ivm \
	SoNodeKit.ivm \
	SoNodeKitListPart.ivm \
	SoNodekitCatalog.ivm \
	SoSceneKit.ivm \
	SoSeparatorKit.ivm \
	SoShapeKit.ivm \
	SoWrapperKit.ivm


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

