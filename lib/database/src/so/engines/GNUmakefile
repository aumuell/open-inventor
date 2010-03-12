IVDEPTH = ../../../../..
include $(IVDEPTH)/make/ivcommondefs

CXXFILES = \
	SoEngines.c++ \
	SoConcatenate.c++ \
	SoEngineInit.c++ \
	SoFieldConverters.c++ \
	SoGate.c++ \
	SoSelectOne.c++

all install: $(OBJECTS)

SoEngines.c++: SoCalcParse.c++

SoCalcParse.c++: SoCalcParse.y
	$(BISON) -y SoCalcParse.y
	sed -e 's/yy/SoCalc_yy/g' -e 's/2093/12093/' < y.tab.c > SoCalcParse.c++

LDIRT += y.tab.c SoCalcParse.c++

include $(IVCOMMONRULES)
