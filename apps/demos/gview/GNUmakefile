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
	$(INSTALL) -m 644 windmill.iv $(IVDEMODATADIR)
	$(INSTALL) -m 644 gviewIcons.iv $(IVDEMODATADIR)

HF = gviewIcons.iv.h
LTARGETS = $(HF)

$(HF): %.h: %
	(echo "const char gviewIcons[] ="; \
	sed -e '{ s/"/\\"/g; s/^/"/; s/$$/\\n"/; }' $^; echo \;) > $^.h

LDIRT += $(HF)

include $(IVCOMMONRULES)
