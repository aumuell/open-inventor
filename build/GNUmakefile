IVDEPTH = ..
include $(IVDEPTH)/make/ivcommondefs

SPECS = sgi-OpenInventor.spec \
	sgi-OpenInventor-data.spec \
	sgi-OpenInventor-devel.spec

RPMDIR = /usr/src/redhat

RPMCMD = env IVROOT=$(RPMDIR)/BUILD LD_LIBRARY_PATH=$(RPMDIR)/BUILD/usr/lib \
		rpmbuild -bb \
		--define "buildroot $(RPMDIR)/BUILD" \
		--define "_topdir $(RPMDIR)" \
		--define "__spec_install_post /bin/true" \
		--define "_unpackaged_files_terminate_build 0"

TARBALL = $(RPMDIR)/SOURCES/sgi-OpenInventor.tar.gz

all install: rpms

$(TARBALL): $(SPECS)
	rm -f $@
	cd $(IVDEPTH)/..; tar zcvf $@ inventor

rpms: $(TARBALL)
	for s in $(SPECS); do \
	  $(RPMCMD) $$s; \
	done;

LDIRT += $(TARBALL)

include $(IVCOMMONRULES)
