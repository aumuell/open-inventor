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
 * Copyright (C) 1990,91,92   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Class:
 |	call initClasses for all node classes
 |
 |   Author(s)		: Ronen Barzel, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SoPrimitiveVertex.h>
#include <Inventor/actions/SoActions.h>
#include <Inventor/elements/SoElements.h>
#include <Inventor/nodes/SoNodes.h>
#include <SoUnknownNode.h>

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes ALL Inventor node classes.
//
// Use: internal

void
SoNode::initClasses()
//
////////////////////////////////////////////////////////////////////////
{
    // Base class must be initialized first
    SoNode::initClass();

    // Parent classes must always be initialized before their subclasses
    SoCamera::initClass();
    SoGroup::initClass();
    SoLight::initClass();
    SoProfile::initClass();
    SoSeparator::initClass();
    SoShape::initClass();
    SoTransformation::initClass();
    SoVertexShape::initClass();
    SoIndexedShape::initClass();
    SoNonIndexedShape::initClass();

    SoAnnotation::initClass();
    SoArray::initClass();
    SoAsciiText::initClass();
    SoBaseColor::initClass();
    SoCallback::initClass();
    SoClipPlane::initClass();
    SoColorIndex::initClass();
    SoComplexity::initClass();
    SoCone::initClass();
    SoCoordinate3::initClass();
    SoCoordinate4::initClass();
    SoCube::initClass();
    SoCylinder::initClass();
    SoDirectionalLight::initClass();
    SoDrawStyle::initClass();
    SoEnvironment::initClass();
    SoEventCallback::initClass();
    SoFaceSet::initClass();
    SoFile::initClass();
    SoFont::initClass();
    SoFontStyle::initClass();
    SoIndexedFaceSet::initClass();
    SoIndexedLineSet::initClass();
    SoIndexedNurbsCurve::initClass();
    SoIndexedNurbsSurface::initClass();
    SoIndexedTriangleStripSet::initClass();
    SoInfo::initClass();
    SoLabel::initClass();
    SoLevelOfDetail::initClass();
    SoLOD::initClass();
    SoLocateHighlight::initClass();
    SoLightModel::initClass();
    SoLineSet::initClass();
    SoLinearProfile::initClass();
    SoMaterial::initClass();
    SoMaterialBinding::initClass();
    SoMatrixTransform::initClass();
    SoMultipleCopy::initClass();
    SoNormal::initClass();
    SoNormalBinding::initClass();
    SoNurbsCurve::initClass();
    SoNurbsProfile::initClass();
    SoNurbsSurface::initClass();
    SoOrthographicCamera::initClass();
    SoPackedColor::initClass();
    SoPathSwitch::initClass();
    SoPerspectiveCamera::initClass();
    SoPickStyle::initClass();
    SoPointLight::initClass();
    SoPointSet::initClass();
    SoProfileCoordinate2::initClass();
    SoProfileCoordinate3::initClass();
    SoQuadMesh::initClass();
    SoResetTransform::initClass();
    SoRotation::initClass();
    SoRotationXYZ::initClass();
    SoScale::initClass();
    SoShapeHints::initClass();
    SoSphere::initClass();
    SoSpotLight::initClass();
    SoSwitch::initClass();
    SoText2::initClass();
    SoText3::initClass();
    SoTextureCoordinate2::initClass();
    SoTextureCoordinateBinding::initClass();
    SoTextureCoordinateFunction::initClass();
    SoTextureCoordinateDefault::initClass();
    SoTextureCoordinateEnvironment::initClass();
    SoTextureCoordinatePlane::initClass();
    SoTexture2::initClass();
    SoTexture2Transform::initClass();
    SoTransform::initClass();
    SoTransformSeparator::initClass();
    SoTranslation::initClass();
    SoTriangleStripSet::initClass();
    SoUnits::initClass();
    SoUnknownNode::initClass();
    SoVertexProperty::initClass();
    SoWWWAnchor::initClass();
    SoWWWInline::initClass();

    // Four self-animating nodes. These are subclassed from 
    // SoSwitch, SoRotation, and SoTranslation
    SoBlinker::initClass();
    SoPendulum::initClass();
    SoRotor::initClass();
    SoShuttle::initClass();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoAnnotation class.
//
// Use: internal

void
SoAnnotation::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoAnnotation, "Annotation", SoSeparator);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoArray class.
//
// Use: internal

void
SoArray::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoArray, "Array", SoGroup);
}

////////////////////////////////////////////////////////////////////////
//
//  This initializes the SoAsciiText class.
//  
void
SoAsciiText::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoAsciiText, "AsciiText", SoShape);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoBaseColor class.
//
// Use: internal

void
SoBaseColor::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoBaseColor, "BaseColor", SoNode);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoBlinker class.
//
// Use: internal

void
SoBlinker::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoBlinker, "Blinker", SoSwitch);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoCallback class.
//
// Use: internal

void
SoCallback::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoCallback, "Callback", SoNode);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoCamera class. Since this is an abstract
//    class, this doesn't need to deal with field data. Subclasses,
//    do, however.
//
// Use: internal

void
SoCamera::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_ABSTRACT_CLASS(SoCamera, "Camera", SoNode);

    // Enable elements used:

    SO_ENABLE(SoCallbackAction, SoFocalDistanceElement);
    SO_ENABLE(SoCallbackAction, SoProjectionMatrixElement);
    SO_ENABLE(SoCallbackAction, SoViewVolumeElement);
    SO_ENABLE(SoCallbackAction, SoViewingMatrixElement);
    SO_ENABLE(SoCallbackAction, SoViewportRegionElement);

    SO_ENABLE(SoGetBoundingBoxAction, SoFocalDistanceElement);
    SO_ENABLE(SoGetBoundingBoxAction, SoProjectionMatrixElement);
    SO_ENABLE(SoGetBoundingBoxAction, SoViewVolumeElement);
    SO_ENABLE(SoGetBoundingBoxAction, SoViewingMatrixElement);
    SO_ENABLE(SoGetBoundingBoxAction, SoViewportRegionElement);

    SO_ENABLE(SoGLRenderAction, SoFocalDistanceElement);
    SO_ENABLE(SoGLRenderAction, SoGLProjectionMatrixElement);
    SO_ENABLE(SoGLRenderAction, SoViewVolumeElement);
    SO_ENABLE(SoGLRenderAction, SoGLViewingMatrixElement);
    SO_ENABLE(SoGLRenderAction, SoGLViewportRegionElement);
    SO_ENABLE(SoGLRenderAction, SoGLUpdateAreaElement);

    SO_ENABLE(SoHandleEventAction, SoViewVolumeElement);
    SO_ENABLE(SoHandleEventAction, SoViewportRegionElement);

    SO_ENABLE(SoRayPickAction, SoFocalDistanceElement);
    SO_ENABLE(SoRayPickAction, SoProjectionMatrixElement);
    SO_ENABLE(SoRayPickAction, SoViewVolumeElement);
    SO_ENABLE(SoRayPickAction, SoViewingMatrixElement);
    SO_ENABLE(SoRayPickAction, SoViewportRegionElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoClipPlane class.
//
// Use: internal

void
SoClipPlane::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoClipPlane, "ClipPlane", SoNode);

    // Enable clip plane element
    SO_ENABLE(SoCallbackAction, SoClipPlaneElement);
    SO_ENABLE(SoPickAction,	SoClipPlaneElement);
    SO_ENABLE(SoGLRenderAction, SoGLClipPlaneElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoColorIndex class.
//
// Use: internal

void
SoColorIndex::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoColorIndex, "ColorIndex", SoNode);
    SO_ENABLE(SoGLRenderAction, SoGLLazyElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoComplexity class.
//
// Use: internal

void
SoComplexity::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoComplexity, "Complexity", SoNode);

    // Enable elements for appropriate actions:
    SO_ENABLE(SoCallbackAction, SoComplexityTypeElement);
    SO_ENABLE(SoCallbackAction, SoShapeStyleElement);
    SO_ENABLE(SoCallbackAction, SoComplexityElement);
    SO_ENABLE(SoGetBoundingBoxAction, SoComplexityTypeElement);
    SO_ENABLE(SoGetBoundingBoxAction, SoShapeStyleElement);
    SO_ENABLE(SoGetBoundingBoxAction, SoComplexityElement);
    SO_ENABLE(SoGLRenderAction, SoComplexityTypeElement);
    SO_ENABLE(SoGLRenderAction, SoShapeStyleElement);
    SO_ENABLE(SoGLRenderAction, SoComplexityElement);
    SO_ENABLE(SoGLRenderAction, SoTextureQualityElement);
    SO_ENABLE(SoGLRenderAction, SoTextureOverrideElement);
    SO_ENABLE(SoPickAction,     SoComplexityTypeElement);
    SO_ENABLE(SoPickAction,     SoShapeStyleElement);
    SO_ENABLE(SoPickAction,     SoComplexityElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoCone class.
//
// Use: internal

void
SoCone::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoCone, "Cone", SoShape);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoCoordinate4 class.
//
// Use: internal

void
SoCoordinate4::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoCoordinate4, "Coordinate4", SoNode);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoCube class.
//
// Use: internal

void
SoCube::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoCube, "Cube", SoShape);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoCylinder class.
//
// Use: internal

void
SoCylinder::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoCylinder, "Cylinder", SoShape);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoDirectionalLight class.
//
// Use: internal

void
SoDirectionalLight::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoDirectionalLight, "DirectionalLight", SoLight);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoDrawStyle class.
//
// Use: internal

void
SoDrawStyle::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoDrawStyle, "DrawStyle", SoNode);

    // Enable elements for appropriate actions:

    SO_ENABLE(SoCallbackAction, SoDrawStyleElement);
    SO_ENABLE(SoCallbackAction, SoLinePatternElement);
    SO_ENABLE(SoCallbackAction, SoLineWidthElement);
    SO_ENABLE(SoCallbackAction, SoPointSizeElement);
    SO_ENABLE(SoCallbackAction, SoShapeStyleElement);

    SO_ENABLE(SoGLRenderAction, SoGLDrawStyleElement);
    SO_ENABLE(SoGLRenderAction, SoGLLinePatternElement);
    SO_ENABLE(SoGLRenderAction, SoGLLineWidthElement);
    SO_ENABLE(SoGLRenderAction, SoGLPointSizeElement);
    SO_ENABLE(SoGLRenderAction, SoShapeStyleElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoEnvironment class.
//
// Use: internal

void
SoEnvironment::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoEnvironment, "Environment", SoNode);

    // Enable elements for appropriate actions:

    SO_ENABLE(SoCallbackAction, SoLightAttenuationElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initialize the node
//
// Use: public, internal
//
void
SoEventCallback::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoEventCallback, "EventCallback", SoNode);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoFaceSet class.
//
// Use: internal

void
SoFaceSet::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoFaceSet, "FaceSet", SoNonIndexedShape);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoFile class.
//
// Use: internal

void
SoFile::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoFile, "File", SoNode);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoFont class.
//
// Use: internal

void
SoFont::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoFont, "Font", SoNode);

    // Enable elements for appropriate actions:
    SO_ENABLE(SoGLRenderAction,		SoFontNameElement);
    SO_ENABLE(SoGLRenderAction,		SoFontSizeElement);
    SO_ENABLE(SoPickAction,		SoFontNameElement);
    SO_ENABLE(SoPickAction,		SoFontSizeElement);
    SO_ENABLE(SoGetBoundingBoxAction,	SoFontNameElement);
    SO_ENABLE(SoGetBoundingBoxAction,	SoFontSizeElement);
    SO_ENABLE(SoCallbackAction,		SoFontNameElement);
    SO_ENABLE(SoCallbackAction,		SoFontSizeElement);
}

////////////////////////////////////////////////////////////////////////
//
//  This initializes the SoFontStyle class.
//  
void
SoFontStyle::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoFontStyle, "FontStyle", SoFont);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoGroup class.
//
// Use: internal

void
SoGroup::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoGroup, "Group", SoNode);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoIndexedFaceSet class.
//
// Use: internal

void
SoIndexedFaceSet::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoIndexedFaceSet, "IndexedFaceSet", SoIndexedShape);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoIndexedLineSet class.
//
// Use: internal

void
SoIndexedLineSet::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoIndexedLineSet, "IndexedLineSet", SoIndexedShape);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoIndexedNurbsCurve class.
//
// Use: internal

void
SoIndexedNurbsCurve::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoIndexedNurbsCurve, "IndexedNurbsCurve", SoShape);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoIndexedNurbsSurface class.
//
// Use: internal

void
SoIndexedNurbsSurface::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoIndexedNurbsSurface, "IndexedNurbsSurface", SoShape);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoIndexedShape class.
//
// Use: internal

void
SoIndexedShape::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_ABSTRACT_CLASS(SoIndexedShape, "IndexedShape",
				 SoVertexShape);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoIndexedTriangleStripSet class.
//
// Use: internal

void
SoIndexedTriangleStripSet::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoIndexedTriangleStripSet,
			"IndexedTriangleStripSet", SoIndexedShape);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoInfo class.
//
// Use: internal

void
SoInfo::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoInfo, "Info", SoNode);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoLabel class.
//
// Use: internal

void
SoLabel::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoLabel, "Label", SoNode);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoLevelOfDetail class.
//
// Use: internal

void
SoLevelOfDetail::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoLevelOfDetail, "LevelOfDetail", SoGroup);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoLOD class.
//
// Use: internal

void
SoLOD::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoLOD, "LOD", SoGroup);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoLight class. Since this is an abstract
//    class, this doesn't need to deal with field data. Subclasses,
//    do, however.
//
// Use: internal

void
SoLight::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_ABSTRACT_CLASS(SoLight, "Light", SoNode);

    // Enable elements used by light source classes:
    SO_ENABLE(SoGLRenderAction, SoGLLightIdElement);
    SO_ENABLE(SoGLRenderAction, SoLightAttenuationElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoLightModel class.
//
// Use: internal

void
SoLightModel::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoLightModel, "LightModel", SoNode);

    SO_ENABLE(SoGLRenderAction, SoShapeStyleElement);
    SO_ENABLE(SoCallbackAction, SoShapeStyleElement);
    SO_ENABLE(SoGLRenderAction, SoGLLazyElement);
    SO_ENABLE(SoCallbackAction, SoLazyElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoLineSet class.
//
// Use: internal

void
SoLineSet::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoLineSet, "LineSet", SoNonIndexedShape);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoLinearProfile class.
//
// Use: internal

void
SoLinearProfile::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoLinearProfile, "LinearProfile", SoProfile);
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoMaterialBinding class.
//
// Use: internal

void
SoMaterialBinding::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoMaterialBinding, "MaterialBinding", SoNode);

    // Enable elements for appropriate actions:
    SO_ENABLE(SoGLRenderAction, SoMaterialBindingElement);
    SO_ENABLE(SoCallbackAction, SoMaterialBindingElement);
    SO_ENABLE(SoPickAction,     SoMaterialBindingElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoMatrixTransform class.
//
// Use: internal

void
SoMatrixTransform::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoMatrixTransform, "MatrixTransform", SoTransformation);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoMultipleCopy class.
//
// Use: internal

void
SoMultipleCopy::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoMultipleCopy, "MultipleCopy", SoGroup);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the base SoNode class.
//
// Use: internal

void
SoNode::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    nextActionMethodIndex = 0;
    // Allocate a new node type id
    // No real parent id
    classTypeId = SoType::createType(SoFieldContainer::getClassTypeId(),
				     "Node",
				     NULL,	// Cannot create, abstract
				     nextActionMethodIndex++);
				     
    // Start nodeIds at 10--that way values of 0 through 9 can be used
    // for special meanings to attach to nonexistent nodes, 
    // like "default" or "invalid"
    nextUniqueId = 10;

    // Add action methods
    SoCallbackAction::addMethod(classTypeId,		callbackS);
    SoGLRenderAction::addMethod(classTypeId,		GLRenderS);
    SoGetBoundingBoxAction::addMethod(classTypeId,	getBoundingBoxS);
    SoGetMatrixAction::addMethod(classTypeId,		getMatrixS);
    SoHandleEventAction::addMethod(classTypeId,		handleEventS);
    SoPickAction::addMethod(classTypeId,		pickS);
    SoRayPickAction::addMethod(classTypeId,		rayPickS);
    SoSearchAction::addMethod(classTypeId,		searchS);
    SoWriteAction::addMethod(classTypeId,		writeS);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoNonIndexedShape class.
//
// Use: internal

void
SoNonIndexedShape::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_ABSTRACT_CLASS(SoNonIndexedShape, "NonIndexedShape",
				 SoVertexShape);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoNormalBinding class.
//
// Use: internal

void
SoNormalBinding::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoNormalBinding, "NormalBinding", SoNode);

    // Enable elements for appropriate actions:
    SO_ENABLE(SoGLRenderAction, SoNormalBindingElement);
    SO_ENABLE(SoCallbackAction, SoNormalBindingElement);
    SO_ENABLE(SoPickAction, SoNormalBindingElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoNurbsCurve class.
//
// Use: internal

void
SoNurbsCurve::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoNurbsCurve, "NurbsCurve", SoShape);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoNurbsProfile class.
//
// Use: internal

void
SoNurbsProfile::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoNurbsProfile, "NurbsProfile", SoProfile);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoNurbsSurface class.
//
// Use: internal

void
SoNurbsSurface::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoNurbsSurface, "NurbsSurface", SoShape);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoOrthographicCamera class.
//
// Use: internal

void
SoOrthographicCamera::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoOrthographicCamera, "OrthographicCamera", SoCamera);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoPackedColor class.
//
// Use: internal

void
SoPackedColor::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoPackedColor, "PackedColor", SoNode);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoPathSwitch class.
//
// Use: internal

void
SoPathSwitch::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoPathSwitch, "PathSwitch", SoGroup);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoPendulum class.
//
// Use: internal

void
SoPendulum::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoPendulum, "Pendulum", SoRotation);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoPerspectiveCamera class.
//
// Use: internal

void
SoPerspectiveCamera::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoPerspectiveCamera, "PerspectiveCamera", SoCamera);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoPickStyle class.
//
// Use: internal

void
SoPickStyle::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoPickStyle, "PickStyle", SoNode);

    // Enable elements for picking actions:
    SO_ENABLE(SoCallbackAction, SoPickStyleElement);
    SO_ENABLE(SoPickAction,	SoPickStyleElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoPointLight class.
//
// Use: internal

void
SoPointLight::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoPointLight, "PointLight", SoLight);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoPointSet class.
//
// Use: internal

void
SoPointSet::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoPointSet, "PointSet", SoNonIndexedShape);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoProfile class.
//
// Use: internal

void
SoProfile::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_ABSTRACT_CLASS(SoProfile, "Profile", SoNode);

    SO_ENABLE(SoCallbackAction,		SoProfileElement);
    SO_ENABLE(SoCallbackAction,		SoProfileCoordinateElement);
    SO_ENABLE(SoGLRenderAction,		SoProfileElement);
    SO_ENABLE(SoGLRenderAction,		SoProfileCoordinateElement);
    SO_ENABLE(SoGetBoundingBoxAction,	SoProfileElement);
    SO_ENABLE(SoGetBoundingBoxAction,	SoProfileCoordinateElement);
    SO_ENABLE(SoPickAction,		SoProfileElement);
    SO_ENABLE(SoPickAction,		SoProfileCoordinateElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoProfileCoordinate2 class.
//
// Use: internal

void
SoProfileCoordinate2::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoProfileCoordinate2, "ProfileCoordinate2", SoNode);

    // Enable elements for appropriate actions:
    SO_ENABLE(SoGLRenderAction, SoProfileCoordinateElement);
    SO_ENABLE(SoCallbackAction, SoProfileCoordinateElement);
    SO_ENABLE(SoPickAction,	SoProfileCoordinateElement);
    SO_ENABLE(SoGetBoundingBoxAction,	SoProfileCoordinateElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoProfileCoordinate3 class.
//
// Use: internal

void
SoProfileCoordinate3::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoProfileCoordinate3, "ProfileCoordinate3", SoNode);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoQuadMesh class.
//
// Use: internal

void
SoQuadMesh::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoQuadMesh, "QuadMesh", SoNonIndexedShape);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoResetTransform class.
//
// Use: internal

void
SoResetTransform::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoResetTransform, "ResetTransform", SoTransformation);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoRotation class.
//
// Use: internal

void
SoRotation::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoRotation, "Rotation", SoTransformation);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoRotationXYZ class.
//
// Use: internal

void
SoRotationXYZ::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoRotationXYZ, "RotationXYZ", SoTransformation);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoRotor class.
//
// Use: internal

void
SoRotor::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoRotor, "Rotor", SoRotation);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoScale class.
//
// Use: internal

void
SoScale::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoScale, "Scale", SoTransformation);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoShape class.
//
// Use: internal

void
SoShape::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_ABSTRACT_CLASS(SoShape, "Shape", SoNode);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoShapeHints class.
//
// Use: internal

void
SoShapeHints::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoShapeHints, "ShapeHints", SoNode);

    SO_ENABLE(SoGLRenderAction, SoGLShapeHintsElement);
    SO_ENABLE(SoGLRenderAction, SoCreaseAngleElement);
    SO_ENABLE(SoCallbackAction, SoShapeHintsElement);
    SO_ENABLE(SoCallbackAction, SoCreaseAngleElement);
    SO_ENABLE(SoPickAction, SoShapeHintsElement);
    SO_ENABLE(SoPickAction, SoCreaseAngleElement);
    SO_ENABLE(SoGetBoundingBoxAction, SoShapeHintsElement);
    SO_ENABLE(SoGetBoundingBoxAction, SoCreaseAngleElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoShuttle class.
//
// Use: internal

void
SoShuttle::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoShuttle, "Shuttle", SoTranslation);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoSphere class.
//
// Use: internal

void
SoSphere::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoSphere, "Sphere", SoShape);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoSpotLight class.
//
// Use: internal

void
SoSpotLight::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoSpotLight, "SpotLight", SoLight);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoSwitch class.
//
// Use: internal

void
SoSwitch::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoSwitch, "Switch", SoGroup);

    SO_ENABLE(SoCallbackAction, SoSwitchElement);
    SO_ENABLE(SoGLRenderAction, SoSwitchElement);
    SO_ENABLE(SoGetBoundingBoxAction, SoSwitchElement);
    SO_ENABLE(SoGetMatrixAction, SoSwitchElement);
    SO_ENABLE(SoHandleEventAction, SoSwitchElement);
    SO_ENABLE(SoPickAction, SoSwitchElement);
    SO_ENABLE(SoSearchAction, SoSwitchElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoText2 class.
//
// Use: internal

void
SoText2::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoText2, "Text2", SoShape);

    // Font size/name are enabled by SoFont node
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoText3 class.
//
// Use: internal

void
SoText3::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoText3, "Text3", SoShape);

    // Font name and size elements are enabled by the SoFont node
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoTexture2 class.
//
// Use: internal

void
SoTexture2::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoTexture2, "Texture2", SoNode);

    SO_ENABLE(SoGLRenderAction, SoGLTextureImageElement);
    SO_ENABLE(SoCallbackAction, SoTextureImageElement);
    SO_ENABLE(SoGLRenderAction, SoTextureOverrideElement);
    SO_ENABLE(SoCallbackAction, SoTextureOverrideElement);
    SO_ENABLE(SoGLRenderAction, SoGLTextureEnabledElement);
    SO_ENABLE(SoGLRenderAction, SoShapeStyleElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoTexture2Transform class.
//
// Use: internal

void
SoTexture2Transform::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoTexture2Transform, "Texture2Transform", SoNode);

    SO_ENABLE(SoCallbackAction,		SoTextureMatrixElement);
    SO_ENABLE(SoPickAction,		SoTextureMatrixElement);
    SO_ENABLE(SoGLRenderAction,		SoGLTextureMatrixElement);
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoTextureCoordinateBinding class.
//
// Use: internal

void
SoTextureCoordinateBinding::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoTextureCoordinateBinding, "TextureCoordinateBinding", 
		       SoNode);

    // Enable elements for appropriate actions:
    SO_ENABLE(SoCallbackAction, SoTextureCoordinateBindingElement);
    SO_ENABLE(SoGLRenderAction, SoTextureCoordinateBindingElement);
    SO_ENABLE(SoPickAction, SoTextureCoordinateBindingElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoTextureCoordinateDefault class.
//
// Use: internal

void
SoTextureCoordinateDefault::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoTextureCoordinateDefault,
		      "TextureCoordinateDefault", SoTextureCoordinateFunction);

    // Elements are enabled by SoTextureCoordinate nodes.
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoTextureCoordinateEnvironment class.
//
// Use: internal

void
SoTextureCoordinateEnvironment::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoTextureCoordinateEnvironment,
		  "TextureCoordinateEnvironment", SoTextureCoordinateFunction);

    // Elements are enabled by SoTextureCoordinate nodes.
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoTextureCoordinateFunction class.
//
// Use: internal

void
SoTextureCoordinateFunction::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoTextureCoordinateFunction,
		      "TextureCoordinateFunction", SoNode);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoTextureCoordinatePlane class.
//
// Use: internal

void
SoTextureCoordinatePlane::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoTextureCoordinatePlane,
		      "TextureCoordinatePlane", SoTextureCoordinateFunction);

    // Elements are enabled by SoTextureCoordinate nodes.
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoTransform class.
//
// Use: internal

void
SoTransform::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoTransform, "Transform", SoTransformation);

    SO_ENABLE(SoCallbackAction,		SoModelMatrixElement);
    SO_ENABLE(SoGetBoundingBoxAction,	SoBBoxModelMatrixElement);
    SO_ENABLE(SoGetBoundingBoxAction,	SoLocalBBoxMatrixElement);
    SO_ENABLE(SoPickAction,		SoModelMatrixElement);
    SO_ENABLE(SoGLRenderAction,		SoGLModelMatrixElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoTransformSeparator class.
//
// Use: internal

void
SoTransformSeparator::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoTransformSeparator, "TransformSeparator", SoGroup);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoTransformation class.
//
// Use: internal

void
SoTransformation::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_ABSTRACT_CLASS(SoTransformation, "Transformation", SoNode);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoTranslation class.
//
// Use: internal

void
SoTranslation::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoTranslation, "Translation", SoTransformation);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoTriangleStripSet class.
//
// Use: internal

void
SoTriangleStripSet::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoTriangleStripSet, "TriangleStripSet",
			SoNonIndexedShape);

    SO_ENABLE(SoGLRenderAction,		SoShapeStyleElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoUnits class.
//
// Use: internal

void
SoUnits::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoUnits, "Units", SoTransformation);

    SO_ENABLE(SoCallbackAction,		SoUnitsElement);
    SO_ENABLE(SoGLRenderAction,		SoUnitsElement);
    SO_ENABLE(SoGetBoundingBoxAction,	SoUnitsElement);
    SO_ENABLE(SoGetMatrixAction,	SoUnitsElement);
    SO_ENABLE(SoPickAction,		SoUnitsElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoUnknownNode class.
//
// Use: internal

void
SoUnknownNode::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoUnknownNode, "UnknownNode", SoNode);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes the SoVertexShape class.
//
// Use: internal

void
SoVertexShape::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_ABSTRACT_CLASS(SoVertexShape, "VertexShape", SoShape);
}

////////////////////////////////////////////////////////////////////////
//
//  This initializes the SoWWWAnchor class.
//  
void
SoWWWAnchor::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoWWWAnchor, "WWWAnchor", SoLocateHighlight);
    emptyString.makeEmpty();
}

////////////////////////////////////////////////////////////////////////
//
//  This initializes the SoWWWInline class.
//  
void
SoWWWInline::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__NODE_INIT_CLASS(SoWWWInline, "WWWInline", SoNode);
}

