IVDEPTH = ../../../../..
include $(IVDEPTH)/make/commonivdefs

DEPTH = ../../..
LDIRT = *.d
LCXXFLAGS = -I$(DEPTH)/include -DLIBRARYBUILD -DDEBUG

CXXFILES =\
	SoCache.c++ \
	SoBoundingBoxCache.c++ \
	SoGLRenderCache.c++ \
	SoNormalCache.c++ \
	SoGLCacheList.c++

all:: $(OBJECTS)

include $(COMMONRULES)
