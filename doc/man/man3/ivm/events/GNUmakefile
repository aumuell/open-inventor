
.SUFFIXES: .ivm .3iv

MANPAGES = \
	SoButtonEvent.ivm \
	SoEvent.ivm \
	SoKeyboardEvent.ivm \
	SoLocation2Event.ivm \
	SoMotion3Event.ivm \
	SoMouseButtonEvent.ivm \
	SoSpaceballButtonEvent.ivm


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

