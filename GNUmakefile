IVDEPTH = .
include $(IVDEPTH)/make/ivcommondefs

# Use of LSUBDIRS is to allow override using environment variable without
# conflicting with SUBDIRS in subdirectory makefiles.

LSUBDIRS = libimage tools libFL lib libSoXt doc data apps
SUBDIRS = $(LSUBDIRS)

all install $(COMMONTARGS):
	$(SUBDIRS_MAKERULE)
