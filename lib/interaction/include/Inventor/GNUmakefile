IVDEPTH = ../../../..
include $(IVDEPTH)/make/commonivdefs

INST_HEADER_ROOT   = $(IVROOT)/usr/include/Inventor
HEADER_PATH        = $(INST_HEADER_ROOT)
NOSHIP_HEADER_PATH = $(HEADER_PATH)

INST_HEADERS = \
	SoInteraction.h \
	SoSceneManager.h

SUBDIRS = \
	actions \
	draggers \
	manips \
	nodekits \
	nodes

LDIRT = $(INST_HEADER_ROOT)

install: headers
	@for d in $(SUBDIRS); do \
		$(ECHO) "\tcd $$d; $(MAKE) $@"; \
		cd $$d; $(MAKE) $@; cd ..; \
	done

include $(IVCOMMONRULES)
