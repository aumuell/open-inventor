IVDEPTH = ../../../..
include $(IVDEPTH)/make/commonivdefs

DEPTH = ../../..
LDIRT = *.d
LCXXFLAGS = -I$(DEPTH)/database/include \
	    -I../../include \
	    -I$(DEPTH)/include \
	    -DLIBRARYBUILD \
	    -I$(DEPTH)/nodekits/include -DDEBUG

CXXFILES = \
	SoDraggerInit.c++ SoCenterballDragger.c++ \
	SoDirectionalLightDragger.c++ SoDragPointDragger.c++ \
	SoDragger.c++ SoHandleBoxDragger.c++ \
	SoJackDragger.c++ SoPointLightDragger.c++ \
	SoRotateCylindricalDragger.c++ SoRotateDiscDragger.c++ \
	SoRotateSphericalDragger.c++ SoScale1Dragger.c++ \
	SoScale2Dragger.c++ SoScale2UniformDragger.c++ \
	SoScaleUniformDragger.c++ SoSpotLightDragger.c++ \
	SoTabBoxDragger.c++ SoTabPlaneDragger.c++ \
	SoTrackballDragger.c++ SoTransformBoxDragger.c++ \
	SoTransformerDragger.c++ SoTranslate1Dragger.c++ \
	SoTranslate2Dragger.c++

all:: $(OBJECTS)

include $(COMMONRULES)
