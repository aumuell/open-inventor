IVDEPTH = ../../..
include $(IVDEPTH)/make/ivcommondefs

PROGRAM = drop
DEMO = $(PROGRAM)

CXXFILES = \
	TsBlock.c++ TsPiece.c++ TsPieces.c++ TsField.c++ \
	TsTrickle.c++ TsEvents.c++ TsRemoval.c++ TsViewer.c++ drop.c++

LLDLIBS = -lInventor -lInventorXt -lGL -lXm -lXt -lX11

all: all_ivbin

install: install_ivdemo

include $(IVCOMMONRULES)
