IVDEPTH = ../..
include $(IVDEPTH)/make/ivcommondefs

FILES = `find . -name "*.env" -o -name "*.iv" -o -name "README"`

all:

install:
	mkdir -p $(IVDATADIR)
	(tar cf - $(FILES) | (cd $(IVDATADIR); tar xvf -)) || exit 0

include $(IVCOMMONRULES)
