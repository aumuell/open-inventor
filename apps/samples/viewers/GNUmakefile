IVDEPTH = ../../..
include $(IVDEPTH)/make/ivcommondefs

PROGRAM = viewerTest

LCXXINCS += -I$(IVDEPTH)/libSoXt/src/viewers

CXXFILES = \
	MyExamVwr.c++ \
	MyFullVwr.c++ \
	MyViewer.c++ \
	MyBitmapButton.c++ \
	viewerTest.c++

LLDLIBS = -lInventor -lInventorXt

LTARGETS = getFiles

all: all_ivbin

HF = MyExaminerViewer.h MyFullViewer.h MyViewer.h
CF = MyExamVwr.c++ MyFullVwr.c++ MyViewer.c++ MyBitmapButton.c++ MyBitmapButton.h

getFiles: $(HF) $(CF)

$(HF): My%: $(IVDEPTH)/libSoXt/include/Inventor/Xt/viewers/SoXt%
	./nameScript $(IVDEPTH)/libSoXt/include/Inventor/Xt/viewers/$(subst My,SoXt,$@) \
		> $@
$(CF): My%: $(IVDEPTH)/libSoXt/src/viewers/SoXt%
	./nameScript $(IVDEPTH)/libSoXt/src/viewers/$(subst My,SoXt,$@) \
		> $@

LDIRT += $(HF) $(CF)

include $(IVCOMMONRULES)
