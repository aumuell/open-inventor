#!gmake
SHELL = /bin/sh

install all::
	cd tools/ppp; $(MAKE) install
	cd libimage;$(MAKE) install
	# uncomment the following to get a debug build
	#export LIBTYPE=debug
	cd lib;$(MAKE) install
	cd libSoXt;$(MAKE) install
	if [ ! -d $(IVROOT)/usr/lib/Inventor/Debug ]; then \
	  mkdir -p $(IVROOT)/usr/lib/Inventor/Debug ; \
	fi
	#cp -f $(IVROOT)/usr/lib/libInventor.so $(IVROOT)/usr/lib/Inventor/Debug
	#cp -f $(IVROOT)/usr/lib/libInventorXt.so $(IVROOT)/usr/lib/Inventor/Debug
	cd apps;$(MAKE) install
	cd data;$(MAKE) install
	cd doc;$(MAKE) install

clobber::
	cd tools/ppp; $(MAKE) clobber
	cd libimage;$(MAKE) clobber
	cd lib;$(MAKE) clobber
	cd libSoXt;$(MAKE) clobber
	cd apps;$(MAKE) clobber
	cd doc;$(MAKE) clobber
	if [ -e usr ] ; then \
		rm -rf usr ; \
	fi
	if [ -e opt ] ; then \
		rm -rf opt ; \
	fi

rpms:
	cd .. ; rm -f oiv.tar.gz ; tar zcvf oiv.tar.gz inventor
	su -c "cp -f ../oiv.tar.gz /usr/src/redhat/SOURCES"
	su -c "cd build ; rpm -bb sgi-OpenInventor.rpm.spec"
	su -c "cd build ; rpm -bb sgi-OpenInventor-devel.rpm.spec"
	su -c "cd build ; rpm -bb sgi-OpenInventor-data.rpm.spec"
	su -c "cd build ; rpm -bb sgi-OpenInventor-demos.rpm.spec"

