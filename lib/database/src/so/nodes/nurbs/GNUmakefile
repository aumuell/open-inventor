IVDEPTH = ../../../../../..
include $(IVDEPTH)/make/commonivdefs

DEPTH = ../../../..
LDIRT = *.d
LCXXFLAGS = -I$(DEPTH)/include -Ihead  -DLIBRARYBUILD  -DNDEBUG 

CXXFILES=\
	SoCurveMaps.c++ \
	SoCurveRender.c++ \
	SoCurveRndEvl.c++ \
	SoGLRender.c++ \
	SoPickEvals.c++ \
	SoPickMaps.c++ \
	SoPickRender.c++ \
	SoPrimEvals.c++ \
	SoPrimMaps.c++ \
	SoPrimRender.c++

SUBDIRS = clients libnurbs

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

