IVDEPTH = ../../../..
include $(IVDEPTH)/make/commonivdefs

DEPTH = ../..
LDIRT = *.d
LCXXFLAGS = -I$(DEPTH)/include -DLIBRARYBUILD -DDEBUG

CXXFILES =\
	SbBox.c++ \
	SbColor.c++ \
	SbCylinder.c++ \
	SbDict.c++ \
	SbLine.c++ \
	SbMatrix.c++ \
	SbName.c++ \
	SbPList.c++ \
	SbPlane.c++ \
	SbRotation.c++ \
	SbSphere.c++ \
	SbString.c++ \
	SbTime.c++ \
	SbVec.c++ \
	SbView.c++ \
	SbViewportRegion.c++

SUBDIRS = projectors

all: $(OBJECTS)
	@for d in $(SUBDIRS); do \
		$(ECHO) "\tcd $$d; $(MAKE) $@"; \
		cd $$d; $(MAKE) $@; cd ..; \
	done

install: all
	@for d in $(SUBDIRS); do \
		$(ECHO) "\tcd $$d; $(MAKE) $@"; \
		cd $$d; $(MAKE) $@; cd ..; \
	done

include $(COMMONRULES)

clean clobber:
	rm -f *.o *.d
	@for d in $(SUBDIRS); do \
		$(ECHO) "\tcd $$d; $(MAKE) $@"; \
		cd $$d; $(MAKE) $@; cd ..; \
	done

