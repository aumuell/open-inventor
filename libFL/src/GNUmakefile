IVDEPTH = ../..
include $(IVDEPTH)/make/ivcommondefs

all install:
ifeq ($(usingIrix), 1)
	ln -fs $(ROOT)/usr/$(LIB_LOCATION)/libFL.a .
endif
ifeq ($(usingLinux), 1)
	ln -fs libFL_i386.a libFL.a
endif

include $(IVCOMMONRULES)
