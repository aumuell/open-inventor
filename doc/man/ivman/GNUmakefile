IVDEPTH = ../../..
include $(IVDEPTH)/make/commonivdefs

LDIRT = *.d
LCXXFLAGS = -D__GNU_LIBRARY__ \
	-I$(IVDEPTH)/lib/database/include \
	-I$(IVDEPTH)/lib/interaction/include \
	-I$(IVDEPTH)/lib/nodekits/include \
	-I$(IVDEPTH)/libSoXt/include 
	
CXXFILES = \
	BuildIssues.c++ \
	ClassDef.c++ \
	Main.c++ \
	Man.c++ \
	Reader.c++ \
	Writer.c++

LLDOPTS = \
	-L$(IVDEPTH)/lib \
	-L$(IVDEPTH)/libSoXt \
	-L/usr/X11R6/lib 

LLDLIBS = \
	-lInventor \
	-lInventorXt \
	-lXm -lXt -lXext -lXi \
	-lGLU -lGL -ldl

TARGETS = ivman

$(TARGETS): $(OBJECTS)
	$(CXX) $(LDOPTS) $(OBJECTS) $(LDLIBS) -o $(TARGETS)

all install: $(TARGETS)

include $(COMMONRULES)
