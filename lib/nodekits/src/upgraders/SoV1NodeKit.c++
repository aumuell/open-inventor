/*
 *
 *  Copyright (C) 2000 Silicon Graphics, Inc.  All Rights Reserved. 
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  Further, this software is distributed without any warranty that it is
 *  free of the rightful claim of any third person regarding infringement
 *  or the like.  Any license provided herein, whether implied or
 *  otherwise, applies only to this software file.  Patent licenses, if
 *  any, provided herein do not apply to combinations of this program with
 *  other software, or any other product whatsoever.
 * 
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Contact information: Silicon Graphics, Inc., 1600 Amphitheatre Pkwy,
 *  Mountain View, CA  94043, or:
 * 
 *  http://www.sgi.com 
 * 
 *  For further information regarding this notice, see: 
 * 
 *  http://oss.sgi.com/projects/GenInfo/NoticeExplan/
 *
 */

/*
 * Copyright (C) 1990, 1991   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Description:
 |      Definition of the SoV1NodeKit class
 |
 |   Author(s)          : Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/misc/upgraders/SoV1NodeKit.h>
#include <Inventor/misc/upgraders/SoV1AppearanceKit.h>
#include <Inventor/misc/upgraders/SoV1BaseKit.h>
#include <Inventor/misc/upgraders/SoV1CameraKit.h>
#include <Inventor/misc/upgraders/SoV1ConeKit.h>
#include <Inventor/misc/upgraders/SoV1CubeKit.h>
#include <Inventor/misc/upgraders/SoV1CylinderKit.h>
#include <Inventor/misc/upgraders/SoV1DirectionalLightKit.h>
#include <Inventor/misc/upgraders/SoV1FaceSetKit.h>
#include <Inventor/misc/upgraders/SoV1GroupKit.h>
#include <Inventor/misc/upgraders/SoV1IndexedFaceSetKit.h>
#include <Inventor/misc/upgraders/SoV1IndexedLineSetKit.h>
#include <Inventor/misc/upgraders/SoV1IndexedTriangleMeshKit.h>
#include <Inventor/misc/upgraders/SoV1IndexedNurbsCurveKit.h>
#include <Inventor/misc/upgraders/SoV1IndexedNurbsSurfaceKit.h>
#include <Inventor/misc/upgraders/SoV1LightKit.h>
#include <Inventor/misc/upgraders/SoV1LineSetKit.h>
#include <Inventor/misc/upgraders/SoV1NurbsCurveKit.h>
#include <Inventor/misc/upgraders/SoV1NurbsSurfaceKit.h>
#include <Inventor/misc/upgraders/SoV1OrthographicCameraKit.h>
#include <Inventor/misc/upgraders/SoV1PerspectiveCameraKit.h>
#include <Inventor/misc/upgraders/SoV1PointLightKit.h>
#include <Inventor/misc/upgraders/SoV1PointSetKit.h>
#include <Inventor/misc/upgraders/SoV1QuadMeshKit.h>
#include <Inventor/misc/upgraders/SoV1SceneKit.h>
#include <Inventor/misc/upgraders/SoV1ShapeKit.h>
#include <Inventor/misc/upgraders/SoV1SphereKit.h>
#include <Inventor/misc/upgraders/SoV1SpotLightKit.h>
#include <Inventor/misc/upgraders/SoV1Text2Kit.h>
#include <Inventor/misc/upgraders/SoV1Text3Kit.h>
#include <Inventor/misc/upgraders/SoV1TriangleStripSetKit.h>
#include <Inventor/misc/upgraders/SoV1VertexShapeKit.h>
#include <Inventor/misc/upgraders/SoV1WrapperKit.h>

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initialize all interaction class.
//
// Use: public

void
SoV1NodeKit::init()
//
////////////////////////////////////////////////////////////////////////
{
    //
    // initialize the nodekit catalog class
    //
    SoV1NodekitCatalog::initClass();

    //
    // initialize all the node classes
    //

    // base class
    SoV1BaseKit::initClass();

    // appearance class
    SoV1AppearanceKit::initClass();

    // group and wrapper 
    SoV1GroupKit::initClass();
    SoV1WrapperKit::initClass();

    // shape classes
    SoV1ShapeKit::initClass();
    SoV1ConeKit::initClass();
    SoV1CubeKit::initClass();
    SoV1CylinderKit::initClass();
    SoV1SphereKit::initClass();
    SoV1Text2Kit::initClass();
    SoV1Text3Kit::initClass();
    SoV1VertexShapeKit::initClass();
    SoV1FaceSetKit::initClass();
    SoV1LineSetKit::initClass();
    SoV1PointSetKit::initClass();
    SoV1IndexedFaceSetKit::initClass();
    SoV1IndexedLineSetKit::initClass();
    SoV1IndexedTriangleMeshKit::initClass();
    SoV1IndexedNurbsCurveKit::initClass();
    SoV1IndexedNurbsSurfaceKit::initClass();
    SoV1QuadMeshKit::initClass();
    SoV1TriangleStripSetKit::initClass();
    SoV1NurbsCurveKit::initClass();
    SoV1NurbsSurfaceKit::initClass();

    // lights
    SoV1LightKit::initClass();
    SoV1DirectionalLightKit::initClass();
    SoV1PointLightKit::initClass();
    SoV1SpotLightKit::initClass();

    // cameras
    SoV1CameraKit::initClass();
    SoV1OrthographicCameraKit::initClass();
    SoV1PerspectiveCameraKit::initClass();

    // scene
    SoV1SceneKit::initClass();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV1AppearanceKit class.
//
// Use: internal

void
SoV1AppearanceKit::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoV1AppearanceKit, "V1AppearanceKit", SoV1BaseKit);
    SO_REGISTER_UPGRADER(AppearanceKit,1.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV1BaseKit class.
//
// Use: internal

void
SoV1BaseKit::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoV1BaseKit, "V1BaseKit", SoV1CustomNode);
    SO_REGISTER_UPGRADER(BaseKit,1.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV1CameraKit class.
//
// Use: internal

void
SoV1CameraKit::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoV1CameraKit, "V1CameraKit", SoV1BaseKit );
    SO_REGISTER_UPGRADER(CameraKit,1.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV1ConeKit class.
//
// Use: internal

void
SoV1ConeKit::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoV1ConeKit, "V1ConeKit", SoV1ShapeKit);
    SO_REGISTER_UPGRADER(ConeKit,1.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV1CubeKit class.
//
// Use: internal

void
SoV1CubeKit::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoV1CubeKit, "V1CubeKit", SoV1ShapeKit);
    SO_REGISTER_UPGRADER(CubeKit,1.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV1CylinderKit class.
//
// Use: internal

void
SoV1CylinderKit::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoV1CylinderKit, "V1CylinderKit", SoV1ShapeKit);
    SO_REGISTER_UPGRADER(CylinderKit,1.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV1DirectionalLightKit class.
//
// Use: internal

void
SoV1DirectionalLightKit::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoV1DirectionalLightKit, "V1DirectionalLightKit",
		      SoV1LightKit);
    SO_REGISTER_UPGRADER(DirectionalLightKit,1.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV1FaceSetKit class.
//
// Use: internal

void
SoV1FaceSetKit::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoV1FaceSetKit, "V1FaceSetKit", SoV1VertexShapeKit);
    SO_REGISTER_UPGRADER(FaceSetKit,1.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV1GroupKit class.
//
// Use: internal

void
SoV1GroupKit::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoV1GroupKit, "V1GroupKit", SoV1BaseKit);
    SO_REGISTER_UPGRADER(GroupKit,1.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV1IndexedFaceSetKit class.
//
// Use: internal

void
SoV1IndexedFaceSetKit::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoV1IndexedFaceSetKit, "V1IndexedFaceSetKit", 
		      SoV1VertexShapeKit);
    SO_REGISTER_UPGRADER(IndexedFaceSetKit,1.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV1IndexedLineSetKit class.
//
// Use: internal

void
SoV1IndexedLineSetKit::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoV1IndexedLineSetKit, "V1IndexedLineSetKit", 
		      SoV1VertexShapeKit);
    SO_REGISTER_UPGRADER(IndexedLineSetKit,1.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV1IndexedNurbsCurveKit class.
//
// Use: internal

void
SoV1IndexedNurbsCurveKit::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoV1IndexedNurbsCurveKit, "V1IndexedNurbsCurveKit", 
		      SoV1ShapeKit );
    SO_REGISTER_UPGRADER(IndexedNurbsCurveKit,1.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV1IndexedNurbsSurfaceKit class.
//
// Use: internal

void
SoV1IndexedNurbsSurfaceKit::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoV1IndexedNurbsSurfaceKit, "V1IndexedNurbsSurfaceKit", 
		      SoV1ShapeKit );
    SO_REGISTER_UPGRADER(IndexedNurbsSurfaceKit,1.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV1IndexedTriangleMeshKit class.
//
// Use: internal

void
SoV1IndexedTriangleMeshKit::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoV1IndexedTriangleMeshKit, "V1IndexedTriangleMeshKit",
		      SoV1VertexShapeKit);
    SO_REGISTER_UPGRADER(IndexedTriangleMeshKit,1.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV1LightKit class.
//
// Use: internal

void
SoV1LightKit::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoV1LightKit, "V1LightKit", SoV1BaseKit );
    SO_REGISTER_UPGRADER(LightKit,1.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV1LineSetKit class.
//
// Use: internal

void
SoV1LineSetKit::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoV1LineSetKit, "V1LineSetKit", SoV1VertexShapeKit);
    SO_REGISTER_UPGRADER(LineSetKit,1.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes static variables.
//
// Use: public

void
SoV1NodekitCatalog::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    emptyName = new SbName("");
    emptyList = new SbPList;
    badType   = new SoType();
    *badType = SoType::badType();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV1NurbsCurveKit class.
//
// Use: internal

void
SoV1NurbsCurveKit::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoV1NurbsCurveKit, "V1NurbsCurveKit", SoV1ShapeKit);
    SO_REGISTER_UPGRADER(NurbsCurveKit,1.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV1NurbsSurfaceKit class.
//
// Use: internal

void
SoV1NurbsSurfaceKit::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoV1NurbsSurfaceKit, "V1NurbsSurfaceKit", SoV1ShapeKit);
    SO_REGISTER_UPGRADER(NurbsSurfaceKit,1.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV1OrthographicCameraKit class.
//
// Use: internal

void
SoV1OrthographicCameraKit::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoV1OrthographicCameraKit, "V1OrthographicCameraKit", 
		      SoV1CameraKit );
    SO_REGISTER_UPGRADER(OrthographicCameraKit,1.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV1PerspectiveCameraKit class.
//
// Use: internal

void
SoV1PerspectiveCameraKit::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoV1PerspectiveCameraKit, "V1PerspectiveCameraKit", 
		      SoV1CameraKit );
    SO_REGISTER_UPGRADER(PerspectiveCameraKit,1.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV1PointLightKit class.
//
// Use: internal

void
SoV1PointLightKit::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoV1PointLightKit, "V1PointLightKit", SoV1LightKit );
    SO_REGISTER_UPGRADER(PointLightKit,1.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV1PointSetKit class.
//
// Use: internal

void
SoV1PointSetKit::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoV1PointSetKit, "V1PointSetKit", SoV1VertexShapeKit);
    SO_REGISTER_UPGRADER(PointSetKit,1.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV1QuadMeshKit class.
//
// Use: internal

void
SoV1QuadMeshKit::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoV1QuadMeshKit, "V1QuadMeshKit", SoV1VertexShapeKit);
    SO_REGISTER_UPGRADER(QuadMeshKit,1.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV1SceneKit class.
//
// Use: internal

void
SoV1SceneKit::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoV1SceneKit, "V1SceneKit", SoV1BaseKit );
    SO_REGISTER_UPGRADER(SceneKit,1.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV1ShapeKit class.
//
// Use: internal

void
SoV1ShapeKit::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoV1ShapeKit, "V1ShapeKit", SoV1GroupKit );
    SO_REGISTER_UPGRADER(ShapeKit,1.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV1SphereKit class.
//
// Use: internal

void
SoV1SphereKit::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoV1SphereKit, "V1SphereKit", SoV1ShapeKit);
    SO_REGISTER_UPGRADER(SphereKit,1.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV1SpotLightKit class.
//
// Use: internal

void
SoV1SpotLightKit::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoV1SpotLightKit, "V1SpotLightKit", SoV1LightKit );
    SO_REGISTER_UPGRADER(SpotLightKit,1.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV1Text2Kit class.
//
// Use: internal

void
SoV1Text2Kit::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoV1Text2Kit, "V1Text2Kit", SoV1ShapeKit);
    SO_REGISTER_UPGRADER(Text2Kit,1.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV1Text3Kit class.
//
// Use: internal

void
SoV1Text3Kit::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoV1Text3Kit, "V1Text3Kit", SoV1ShapeKit);
    SO_REGISTER_UPGRADER(Text3Kit,1.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV1TriangleStripSetKit class.
//
// Use: internal

void
SoV1TriangleStripSetKit::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoV1TriangleStripSetKit, "V1TriangleStripSetKit", 
		      SoV1VertexShapeKit );
    SO_REGISTER_UPGRADER(TriangleStripSetKit,1.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV1VertexShapeKit class.
//
// Use: internal

void
SoV1VertexShapeKit::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoV1VertexShapeKit, "V1VertexShapeKit", SoV1ShapeKit);
    SO_REGISTER_UPGRADER(VertexShapeKit,1.0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoV1WrapperKit class.
//
// Use: internal

void
SoV1WrapperKit::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoV1WrapperKit, "V1WrapperKit", SoV1GroupKit);
    SO_REGISTER_UPGRADER(WrapperKit,1.0);
}

