include $(ROOT)/usr/include/make/commonafsdefs

DEPTH   = ../
LDIRT   = *.d
CFILES  = MMfiles.c MMmain.c MMname.c MMwrite.c

TARGETS = makemake

$(TARGETS): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(LDOPTS) -o $(TARGETS)

include $(COMMONRULES)
