IVDEPTH = ../../..
include $(IVDEPTH)/make/ivcommondefs

PROGRAM = gview
DEMO = $(PROGRAM)

LTARGETS = getHeader

CXXFILES = \
	gview.c++ DisplayGraph.c++ Error.c++ FieldEditor.c++ \
	GraphIcon.c++ GraphViewer.c++ NodeCreator.c++

LLDLIBS = -lInventor -lInventorXt

all: all_ivbin

install: install_ivdemo
	$(INSTALL) -m 755 $(DEMO).RUNME $(IVDEMOBINDIR)
	$(INSTALL) -m 644 windmill.iv $(IVDEMODATADIR)
	$(INSTALL) -m 644 gviewIcons.iv $(IVDEMODATADIR)

HF = gviewIcons.iv.h

getHeader: $(HF)

$(HF): %.h: %
	(echo "const char gviewIcons[] = \""; \
	sed -e 's/\"/\\"/g' $^; echo "\";") > $^.h

LDIRT += $(HF)

include $(IVCOMMONRULES)
