#!gmake
IVDEPTH = ../..
include $(IVDEPTH)/make/commonivdefs

install:
	mkdir -p $(IVROOT)/usr/share/data/ ; cd .. ; tar cvf models.tar models ; cp -f models.tar $(IVROOT)/usr/share/data/ ; rm -f models.tar
	cd $(IVROOT)/usr/share/data/ ; tar xvf models.tar ; rm -f models.tar
all:
