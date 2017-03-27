IVDEPTH = ../../..
include $(IVDEPTH)/make/ivcommondefs

PROGRAM = ivman

CXXFILES = Main.c++ ClassDef.c++ Man.c++ Reader.c++ Writer.c++ BuildIssues.c++

LLDLIBS = ../../../lib/libInventor.a -L../../../libFL/freetype $(FLLIB) -L../../../libimage -limage -ljpeg

all install: $(PROGRAM)

include $(IVCOMMONRULES)
