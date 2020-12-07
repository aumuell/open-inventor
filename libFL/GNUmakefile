IVDEPTH = ..
include $(IVDEPTH)/make/ivcommondefs

LCDEFS = $(LCXXDEFS)

ARCHIVE = libFLiv.a

LCINCS += -I/usr/include/freetype2

CFILES = fl.c flclient.c flfreetype.c

CFLAGS += -Wall -g

all install: all_ivbin

include $(IVCOMMONRULES)
