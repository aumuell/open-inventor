IVDEPTH = ../..
include $(IVDEPTH)/make/ivcommondefs

ARCHIVE = libFL.a

LCINCS += -I/usr/include/freetype2

CFILES = fl.c flclient.c flfreetype.c

all install: link all_ivbin

link:
	if [ ! -L flclient.h ]; then ln -s ../src/flclient.h .; fi

include $(IVCOMMONRULES)
