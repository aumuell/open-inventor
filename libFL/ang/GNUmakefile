IVDEPTH = ../..
include $(IVDEPTH)/make/ivcommondefs

LCDEFS = $(LCXXDEFS)

ARCHIVE = libFL.a

LCINCS += -I/usr/include/freetype2

CFILES = fl.c flclient.c flfreetype.c

CFLAGS += -Wall -g

all install: link all_ivbin

link:
	if [ ! -L flclient.h ]; then ln -s ../src/flclient.h .; fi

include $(IVCOMMONRULES)
