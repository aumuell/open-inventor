IVDEPTH = ../../..
include $(IVDEPTH)/make/ivcommondefs

PROGRAM = maze
DEMO = $(PROGRAM)

LCXXDEFS += -DNO_AUDIO

CXXFILES = maze.c++

LLDLIBS = -lInventor -lInventorXt

all: all_ivbin

install: install_ivdemo

include $(IVCOMMONRULES)
