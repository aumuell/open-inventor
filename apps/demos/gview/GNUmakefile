IVDEPTH = ../../..
include $(IVDEPTH)/make/ivcommondefs

PROGRAM = gview
DEMO = $(PROGRAM)

CXXFILES = \
	gview.c++ DisplayGraph.c++ Error.c++ FieldEditor.c++ \
	GraphIcon.c++ GraphViewer.c++ NodeCreator.c++

LLDLIBS = -lInventor -lInventorXt

all: all_ivbin

install: install_ivdemo
	$(INSTALL) -m 755 $(DEMO).RUNME $(IVDEMOBINDIR)
	$(INSTALL) -m 644 windmill.iv $(IVDEMODATADIR)
	$(INSTALL) -m 644 gviewIcons.iv $(IVDEMODATADIR)

include $(IVCOMMONRULES)
