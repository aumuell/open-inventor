IVDEPTH = ../../..
include $(IVDEPTH)/make/ivcommondefs

PROGRAM = ivperf

LCXXINCS += -I../../nodes/BarChart

CXXFILES = ivperf.c++

LLDLIBS = ../../nodes/BarChart/BarChart.o -lInventor -lInventorXt -lGL -lXm -lXt -lX11

all: all_ivbin

install: install_ivbin

include $(IVCOMMONRULES)
