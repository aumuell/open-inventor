IVDEPTH = ../../..
include $(IVDEPTH)/make/commonivdefs

DEPTH = ../..
LDIRT = *.d
LCXXFLAGS = -I$(DEPTH)/database/include \
	    -I../include \
	    -I$(DEPTH)/include \
	    -DLIBRARYBUILD \
	    -I../../nodekits/include  -DDEBUG


CXXFILES =  \
	SoInteraction.c++ SoSceneMgr.c++ \
	SoSelection.c++

SUBDIRS = actions draggers manips nodekits nodes

all: $(OBJECTS)
	@for d in $(SUBDIRS); do \
		$(ECHO) "\tcd $$d; $(MAKE) $@"; \
		cd $$d; $(MAKE) $@; cd ..; \
	done

install:
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
