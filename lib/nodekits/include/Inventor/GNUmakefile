IVDEPTH = ../../../..
include $(IVDEPTH)/make/commonivdefs

INST_HEADER_ROOT   = $(IVROOT)/usr/include/Inventor
HEADER_PATH        = $(INST_HEADER_ROOT)
NOSHIP_HEADER_PATH = $(HEADER_PATH)

INST_HEADERS = \
	SoNodeKitPath.h \

SUBDIRS = \
	details \
	misc \
	nodekits

LDIRT = $(INST_HEADER_ROOT)

install: headers
	@for d in $(SUBDIRS); do \
		$(ECHO) "\tcd $$d; $(MAKE) $@"; \
		cd $$d; $(MAKE) $@; cd ..; \
	done

include $(IVCOMMONRULES)
