#!gmake
IVDEPTH = ../..
include $(IVDEPTH)/make/commonivdefs

LDIRT     = *.d
CXXFILES  = ppp.c++
LCXXFLAGS += -I../../lib/database/include 
TARGETS   = ppp

$(TARGETS) all: $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) $(LDOPTS) -o $(TARGETS) -lm

install: all

include $(COMMONRULES)

clean clobber:
	rm -f *.o *.d ppp
