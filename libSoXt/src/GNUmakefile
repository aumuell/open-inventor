IVDEPTH = ../..
include $(IVDEPTH)/make/commonivdefs

DEPTH = ..
LDIRT = *.d
LCXXFLAGS += -I$(DEPTH)/include \
	    -DLIBRARYBUILD \
	    -I../../lib/database/include \
	    -I../../lib/interaction/include \
	    -I../../lib/nodekits/include -DDEBUG

CXXFILES = \
	SoXt.c++ \
	SoXtClipboard.c++ \
	SoXtComponent.c++ \
	SoXtGLWidget.c++ \
	SoXtImportInterest.c++ \
	SoXtPrintDlog.c++ \
	SoXtRendrArea.c++ \
	SoXtRsrc.c++

CFILES = \
	SoGLwDAUtil.c \
	SoGLwDrawA.c \
	SoGLwMDrawA.c \
	sgi_visual.c


SUBDIRS = \
	devices \
	lightEditor \
	mixedMode \
	motif \
	mtlEdit \
	viewers

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

