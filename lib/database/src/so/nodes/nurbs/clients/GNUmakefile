IVDEPTH = ../../../../../../..
include $(IVDEPTH)/make/commonivdefs

DEPTH = ../../../../../..
LDIRT = *.d
LCXXFLAGS = -I$(DEPTH)/include -I../head  -DLIBRARYBUILD  -DNDEBUG -DDEBUG

CXXFILES=\
	gl4base.c++ \
	gl4curveval.c++ \
	gl4surfeval.c++ \
	isocurveeval.c++ \
	softcurvemaps.c++ \
	softcurveval.c++ \
	softcurvmap.c++ \
	softrenderer.c++ \
	softsurfeval.c++ \
	softsurfmap.c++ \
	softsurfmaps.c++ \
	typestrtab.c++

all:: $(OBJECTS)

include $(COMMONRULES)
