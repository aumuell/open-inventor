IVDEPTH = ../../..
include $(IVDEPTH)/make/commonivdefs

DEPTH = ../../..
LDIRT = *.d libInventorWidget.a componentTest
LCXXFLAGS += -DLIBRARYBUILD \
	    -I$(DEPTH)/lib/database/include/ \
	    -I$(DEPTH)/lib/interaction/include \
	    -I$(DEPTH)/nodekits/include \
	    -D__NO_MATH_INLINES \
	    -I$(DEPTH)/libSoXt/include \
	    -I$(DEPTH)/libimage 

LLDOPTS =\
	-L$(DEPTH)/libimage -L$(DEPTH)/lib/ \
	-L/usr/X11R6/lib -L$(DEPTH)/libSoXt \
	-L/usr/X11R6/lib

ifeq ($(usingLinux),1)
LLDOPTS += -L../../../libFL/src
endif

LLDLIBS = \
	-lInventorXt -lInventor \
	-limage \
	-lGLw -L/usr/X11R6/lib -lXm -lXt -lXi -lX11 \
	-lFL -lGLU -lGL \
	-lXext \
	-lX11 -ldl -lm

all:: libInventorWidget.a componentTest

CXXFILES=\
	MyUIRegion.c++ \
	MyColEd.c++ \
	MyColPatch.c++ \
	MyColSlider.c++ \
	MyColWheel.c++ \
	MyDropSite.c++ \
	MyFileRead.c++ \
	MySlider.c++ \
	MyThumbWhl.c++ \
	MyFloatCBList.c++ \
	MySimpleMtlEditor.c++ \
	MyMtlPal.c++  \
	MyTextureEd.c++

libInventorWidget.a: $(OBJECTS)
	ar clq $@ $(OBJECTS)

componentTest: $(OBJECTS) componentTest.o
	$(CXX) $(LDOPTS) $(OBJECTS) componentTest.o $(LDLIBS) -o $@

install: libInventorWidget.a

include $(COMMONRULES)
