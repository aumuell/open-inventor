IVDEPTH = ..
include $(IVDEPTH)/make/ivcommondefs

SUBDIRS =
ifdef FREETYPE
SUBDIRS += freetype
endif

all install: link
	$(SUBDIRS_MAKERULE)

link:
	if [ ! -L freetype ]; then ln -s ang freetype; fi

COMMONPREF = xxx
$(COMMONTARGS): %: $(COMMONPREF)%
	$(SUBDIRS_MAKERULE)

include $(IVCOMMONRULES)
