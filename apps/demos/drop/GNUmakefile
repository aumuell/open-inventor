IVDEPTH = ../../..
include $(IVDEPTH)/make/ivcommondefs

PROGRAM = drop
DEMO = $(PROGRAM)

CXXFILES = \
	TsBlock.c++ TsPiece.c++ TsPieces.c++ TsField.c++ \
	TsTrickle.c++ TsEvents.c++ TsRemoval.c++ TsViewer.c++ drop.c++

LLDLIBS = -lInventor -lInventorXt

all: all_ivbin

install: install_ivdemo

include $(IVCOMMONRULES)
