IVDEPTH = .
include $(IVDEPTH)/make/ivcommondefs


# Use of LSUBDIRS is to allow override using environment variable without
# conflicting with SUBDIRS in subdirectory makefiles.

LSUBDIRS = libimage tools libFL lib libSoXt doc data apps
SUBDIRS = $(LSUBDIRS)

all install $(COMMONTARGS):
	$(SUBDIRS_MAKERULE)


SPECS = sgi-OpenInventor-clients.rpm.spec \
	sgi-OpenInventor-data.rpm.spec \
	sgi-OpenInventor-devel.rpm.spec

TARBALL = oiv.tar.gz

rpms:
	cd ..; rm -f $(TARBALL); tar zcvf $(TARBALL) inventor
	su -c "cp -f ../$(TARBALL) /usr/src/redhat/SOURCES"

	for s in $(SPECS); do \
	  (su -c "cd build; rpm -bb $$s") \
	done;
