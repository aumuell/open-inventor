IVDEPTH = ../../../..
include $(IVDEPTH)/make/commonivdefs

DEPTH = ../..
LDIRT = *.d
LCXXFLAGS += -I$(DEPTH)/include -DLIBRARYBUILD \
	    -DDEBUG  \
	    -I$(IVDEPTH)/libimage

CXXFILES = \
	   SoAuditorList.c++ \
	   SoBase.c++ \
	   SoChildList.c++ \
	   SoDB.c++ \
	   SoLists.c++ \
	   SoNotification.c++ \
	   SoPath.c++ \
	   SoTempPath.c++ \
	   SoType.c++ \
	   SoInput.c++ \
	   SoOutput.c++ \
	   SoNormalGenerator.c++ \
	   SoCompactPathList.c++ \
	   SoCBList.c++ \
	   SoPickedPoint.c++ \
	   SoPrimitiveVertex.c++ \
	   SoTranscribe.c++ \
	   SoOffscreenRenderer.c++ \
	   SoByteStream.c++ \
	   SoDebug.c++ \
	   SoState.c++ \
	   linux.c++

SUBDIRS=actions bundles caches details elements engines \
	errors events fields nodes sensors upgraders

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

