IVDEPTH = ../../..
include $(IVDEPTH)/make/ivcommondefs

PROGRAM = ivman

CXXFILES = Main.c++ ClassDef.c++ Man.c++ Reader.c++ Writer.c++ BuildIssues.c++

LLDLIBS = ../../../lib/libInventor.a -L../../../libFL $(FLLIB) -L../../../libimage -limage -ljpeg -lGLU -lGL -lX11 $(DLLIB) $(MATHLIB)

all install: $(PROGRAM)

include $(IVCOMMONRULES)
