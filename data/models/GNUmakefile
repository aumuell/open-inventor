#! gmake

IVDEPTH = ../..
include $(IVDEPTH)/make/commonivdefs

DST = $(IVROOT)/usr/share/data/models
FILES = `find . -name "*.env" -o -name "*.iv" -o -name "README"`

install:
	mkdir -p $(DST)
	tar cf - $(FILES) | (cd $(DST); tar xvf -)

all:
