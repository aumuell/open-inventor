IVDEPTH = ../../..
include $(IVDEPTH)/make/commonivdefs

LDIRT = *.d
CFILES = \
	mdate.c

TARGETS = mdate

$(TARGETS): $(OBJECTS)
	$(CC) $(LDOPTS) $(OBJECTS) $(LDLIBS) -o $(TARGETS)

all install: $(TARGETS)

include $(COMMONRULES)
