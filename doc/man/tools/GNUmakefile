IVDEPTH = ../../..
include $(IVDEPTH)/make/ivcommondefs

TARGETS = mdate

CFILES = mdate.c

all install: $(TARGETS)

$(TARGETS): $(OBJECTS)
	$(CC) $(LDOPTS) $(OBJECTS) $(LDLIBS) -o $(TARGETS)

include $(IVCOMMONRULES)
