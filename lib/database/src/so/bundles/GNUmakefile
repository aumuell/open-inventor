IVDEPTH = ../../../../..
include $(IVDEPTH)/make/commonivdefs

DEPTH = ../../..
LDIRT = *.d
LCXXFLAGS = -I$(DEPTH)/include -DLIBRARYBUILD -DDEBUG

CXXFILES =\
	SoMaterialBundle.c++ \
	SoNormalBundle.c++ \
	SoTextureCoordinateBundle.c++

all:: $(OBJECTS)

include $(COMMONRULES)
