IVDEPTH = ../../..
include $(IVDEPTH)/make/ivcommondefs

SUBDIRS = \
	02.Nodes \
	03.Fields \
	04.Actions \
	05.Elements \
	06.Engines \
	07.Nodekits \
	08.Manips \
	09.Highlights \
	10.Components \
	11.Events

all install:
	$(SUBDIRS_MAKERULE)

COMMONPREF = xxx
$(COMMONTARGS): %: $(COMMONPREF)%
	$(SUBDIRS_MAKERULE)

include $(IVCOMMONRULES)
