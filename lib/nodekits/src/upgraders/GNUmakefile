IVDEPTH = ../../../..
include $(IVDEPTH)/make/commonivdefs

DEPTH = ../../..
LDIRT = *.d
LCXXFLAGS = -I$(DEPTH)/database/include \
	    -I../../include \
	    -I$(DEPTH)/include \
	    -DLIBRARYBUILD \
	    -I$(DEPTH)/nodekits/include \
	    -I$(DEPTH)/interaction/include -DDEBUG

CXXFILES = SoV1AppearKit.c++ SoV1BaseKit.c++ \
	SoV1CameraKit.c++ SoV1ConeKit.c++ \
	SoV1CubeKit.c++ SoV1CylinderKit.c++ \
	SoV1DirLtKit.c++ SoV1FaceSetKit.c++ \
	SoV1GroupKit.c++ SoV1IFaceSetKit.c++ \
	SoV1ILineSetKit.c++ SoV1INrbCurvKit.c++ \
	SoV1INrbSurfKit.c++ SoV1ITMshSetKit.c++ \
	SoV1LightKit.c++ SoV1LineSetKit.c++ \
	SoV1NkCatalog.c++ SoV1NkParts.c++ \
	SoV1NodeKit.c++ SoV1NurbCurvKit.c++ \
	SoV1NurbSurfKit.c++ SoV1OrthoCamKit.c++ \
	SoV1PerspCamKit.c++ SoV1PointLtKit.c++ \
	SoV1PointSetKit.c++ SoV1QuadMeshKit.c++ \
	SoV1SceneKit.c++ SoV1ShapeKit.c++ \
	SoV1SphereKit.c++ SoV1SpotLtKit.c++ \
	SoV1Text2Kit.c++ SoV1Text3Kit.c++ \
	SoV1TriStripKit.c++ SoV1VtxShapeKit.c++ \
	SoV1WrapperKit.c++

all:: $(OBJECTS)

include $(COMMONRULES)
