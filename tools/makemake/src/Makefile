#
# Copyright (C) 1990,1991  Silicon Graphics, Inc.
#
#_______________________________________________________________________
#______________  S I L I C O N   G R A P H I C S   I N C .  ____________
#
#   File Name : 
#	usr/src/toolkits/Inventor/tools/makemake/src/Makefile
#
#   RCS Revision:	$Revision: 1.1 $
#
#______________  S I L I C O N   G R A P H I C S   I N C .  ____________
#_______________________________________________________________________
#

DEPTH = ../
include $(ROOT)/usr/include/make/commondefs

TARGETDIR = $(DEPTH)obj
TARGETS = $(TARGETDIR)/makemake
VPATH	  = $(TARGETDIR)

default: do_all

CFILES = MMfiles.c MMmain.c MMname.c MMwrite.c

LCINCS = -I.

include $(COMMONRULES)

.SUFFIXES : .c

.c.o:
	$(CC) -c $(CFLAGS) $(LDOPTS) $< -o $(TARGETDIR)/$*.o

do_all: $(TARGETS)

$(TARGETS): $(OBJECTS)
	(cd $(TARGETDIR); $(CC) $(LDOPTS) -o makemake $(OBJECTS))
