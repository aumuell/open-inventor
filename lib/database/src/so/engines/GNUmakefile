IVDEPTH = ../../../../..
include $(IVDEPTH)/make/commonivdefs

DEPTH = ../../..
LDIRT = *.d y.tab.*
LCXXFLAGS += -I$(DEPTH)/include  -DLIBRARYBUILD -DDEBUG
LCFLAGS += -I$(DEPTH)/include  -DLIBRARYBUILD -DDEBUG
CC=$(CXX)


CXXFILES =\
	SoConcatenate.c++ \
	SoEngineInit.c++ \
	SoFieldConverters.c++ \
	SoGate.c++ \
	SoSelectOne.c++ \
	SoBoolOperation.c++ \
	SoCalcExpr.c++ \
	SoCalcParse.c++ \
	SoCalculator.c++ \
	SoCompose.c++ \
	SoComputeBoundingBox.c++ \
	SoCounter.c++ \
	SoElapsedTime.c++ \
	SoEngine.c++ \
	SoFieldConverter.c++ \
	SoInterpolate.c++ \
	SoOnOff.c++ \
	SoOneShot.c++ \
	SoOutputData.c++ \
	SoTimeCounter.c++ \
	SoTransformVec3f.c++ \
	SoTriggerAny.c++ \
	SoUnknownEngine.c++ 

all:: $(OBJECTS)

YACC = yacc

#.y.o:
#	$(CXX) $(CXXFLAGS) -c SoCalcParse.c++
#	$(YACC) $(YFLAGS) $<
#	mv -f y.tab.c $@


include $(COMMONRULES)
