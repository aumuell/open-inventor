IVDEPTH = ../..
include $(IVDEPTH)/make/ivcommondefs

SUBDIRS = BarChart Decal GeneralizedCylinder

all install:
	$(SUBDIRS_MAKERULE)

COMMONPREF = xxx
$(COMMONTARGS): %: $(COMMONPREF)%
	$(SUBDIRS_MAKERULE)

include $(IVCOMMONRULES)
