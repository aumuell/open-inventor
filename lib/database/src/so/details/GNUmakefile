IVDEPTH = ../../../../..
include $(IVDEPTH)/make/commonivdefs

DEPTH = ../../..
LDIRT = *.d
LCXXFLAGS = -I$(DEPTH)/include  -DLIBRARYBUILD -DDEBUG

CXXFILES =\
	SoDetailInit.c++ \
	SoDetail.c++ \
	SoConeDetail.c++ \
	SoCubeDetail.c++ \
	SoCylinderDetail.c++ \
	SoFaceDetail.c++ \
	SoLineDetail.c++ \
	SoPointDetail.c++ \
	SoTextDetail.c++

all:: $(OBJECTS)

include $(COMMONRULES)
