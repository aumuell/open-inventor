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
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Classes:
 |	SoCallbackAction
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/elements/SoComplexityElement.h>
#include <Inventor/elements/SoCoordinateElement.h>
#include <Inventor/elements/SoCreaseAngleElement.h>
#include <Inventor/elements/SoFocalDistanceElement.h>
#include <Inventor/elements/SoFontNameElement.h>
#include <Inventor/elements/SoFontSizeElement.h>
#include <Inventor/elements/SoLightAttenuationElement.h>
#include <Inventor/elements/SoLinePatternElement.h>
#include <Inventor/elements/SoLineWidthElement.h>
#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/elements/SoNormalElement.h>
#include <Inventor/elements/SoPointSizeElement.h>
#include <Inventor/elements/SoProfileCoordinateElement.h>
#include <Inventor/elements/SoProfileElement.h>
#include <Inventor/elements/SoProjectionMatrixElement.h>
#include <Inventor/elements/SoShapeHintsElement.h>
#include <Inventor/elements/SoSwitchElement.h>
#include <Inventor/elements/SoTextureCoordinateBindingElement.h>
#include <Inventor/elements/SoTextureCoordinateElement.h>
#include <Inventor/elements/SoTextureImageElement.h>
#include <Inventor/elements/SoTextureMatrixElement.h>
#include <Inventor/elements/SoUnitsElement.h>
#include <Inventor/elements/SoViewVolumeElement.h>
#include <Inventor/elements/SoViewingMatrixElement.h>
#include <Inventor/nodes/SoShape.h>

struct nodeTypeCallback {
    SoType					type;
    SoCallbackAction::SoCallbackActionCB	*cb;
    void					*data;
};

struct tailCallback {
    SoCallbackAction::SoCallbackActionCB	*cb;
    void					*data;
};

struct triangleCallback {
    SoType					type;
    SoTriangleCB				*cb;
    void					*data;
};

struct lineSegmentCallback {
    SoType					type;
    SoLineSegmentCB				*cb;
    void					*data;
};

struct pointCallback {
    SoType					type;
    SoPointCB					*cb;
    void					*data;
};

SO_ACTION_SOURCE(SoCallbackAction);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Default constructor.
//
// Use: public

SoCallbackAction::SoCallbackAction()
//
////////////////////////////////////////////////////////////////////////
{
    SO_ACTION_CONSTRUCTOR(SoCallbackAction);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: public

SoCallbackAction::~SoCallbackAction()
//
////////////////////////////////////////////////////////////////////////
{
    int i;

    // Free up the structures in the callback lists:

    for (i = 0; i < preCallbackList.getLength(); i++)
	delete (nodeTypeCallback *) preCallbackList[i];

    for (i = 0; i < postCallbackList.getLength(); i++)
	delete (nodeTypeCallback *) postCallbackList[i];

    for (i = 0; i < preTailCallbackList.getLength(); i++)
	delete (nodeTypeCallback *) preTailCallbackList[i];

    for (i = 0; i < postTailCallbackList.getLength(); i++)
	delete (nodeTypeCallback *) postTailCallbackList[i];

    for (i = 0; i < triangleCallbackList.getLength(); i++)
	delete (triangleCallback *) triangleCallbackList[i];

    for (i = 0; i < lineSegmentCallbackList.getLength(); i++)
	delete (lineSegmentCallback *) lineSegmentCallbackList[i];

    for (i = 0; i < pointCallbackList.getLength(); i++)
	delete (pointCallback *) pointCallbackList[i];
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    These all add callbacks to a callback list.
//
// Use: public (all of them)
//
////////////////////////////////////////////////////////////////////////

void
SoCallbackAction::addPreCallback(SoType type,
				 SoCallbackAction::SoCallbackActionCB cb,
				 void *data)
{
    nodeTypeCallback *cbStruct = new nodeTypeCallback;
    cbStruct->type = type;
    cbStruct->cb = cb;
    cbStruct->data = data;

    preCallbackList.append((void *)cbStruct);
}

void
SoCallbackAction::addPostCallback(SoType type,
				  SoCallbackAction::SoCallbackActionCB cb,
				  void *data)
{
    nodeTypeCallback *cbStruct = new nodeTypeCallback;
    cbStruct->type = type;
    cbStruct->cb = cb;
    cbStruct->data = data;

    postCallbackList.append((void *)cbStruct);
}

void
SoCallbackAction::addPreTailCallback(SoCallbackAction::SoCallbackActionCB cb,
				     void *data)
{
    tailCallback *cbStruct = new tailCallback;
    cbStruct->cb = cb;
    cbStruct->data = data;

    preTailCallbackList.append((void *)cbStruct);
}

void
SoCallbackAction::addPostTailCallback(SoCallbackAction::SoCallbackActionCB cb,
				      void *data)
{
    tailCallback *cbStruct = new tailCallback;
    cbStruct->cb = cb;
    cbStruct->data = data;

    postTailCallbackList.append((void *)cbStruct);
}

void
SoCallbackAction::addTriangleCallback(SoType type,
				      SoTriangleCB cb, void *data)
{
    triangleCallback *cbStruct = new triangleCallback;
    cbStruct->type = type;
    cbStruct->cb = cb;
    cbStruct->data = data;

    triangleCallbackList.append((void *)cbStruct);
}

void SoCallbackAction::addLineSegmentCallback(SoType type,
					      SoLineSegmentCB cb, void *data)
{
    lineSegmentCallback *cbStruct = new lineSegmentCallback;
    cbStruct->type = type;
    cbStruct->cb = cb;
    cbStruct->data = data;

    lineSegmentCallbackList.append((void *)cbStruct);
}

void
SoCallbackAction::addPointCallback(SoType type, SoPointCB cb, void *data)
{
    pointCallback *cbStruct = new pointCallback;
    cbStruct->type = type;
    cbStruct->cb = cb;
    cbStruct->data = data;

    pointCallbackList.append((void *)cbStruct);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    These all return various items accessed from elements in the
//    state. They can be called from callback functions.
//
// Use: public (all of them)

float
SoCallbackAction::getComplexity() const
{
    return SoComplexityElement::get(state);
}

SoComplexity::Type
SoCallbackAction::getComplexityType() const
{
    return (SoComplexity::Type) SoComplexityTypeElement::get(state);
}

int32_t
SoCallbackAction::getNumCoordinates() const
{
    return SoCoordinateElement::getInstance(state)->getNum();
}

const SbVec3f &
SoCallbackAction::getCoordinate3(int index) const
{
    return SoCoordinateElement::getInstance(state)->get3(index);
}

const SbVec4f &
SoCallbackAction::getCoordinate4(int index) const
{
    return SoCoordinateElement::getInstance(state)->get4(index);
}

SoDrawStyle::Style
SoCallbackAction::getDrawStyle() const
{
    return (SoDrawStyle::Style) SoDrawStyleElement::get(state);
}

u_short
SoCallbackAction::getLinePattern() const
{
    return SoLinePatternElement::get(state);
}

float
SoCallbackAction::getLineWidth() const
{
    return SoLineWidthElement::get(state);
}

float
SoCallbackAction::getPointSize() const
//
////////////////////////////////////////////////////////////////////////
{
    return SoPointSizeElement::get(state);
}

const SbName &
SoCallbackAction::getFontName() const
{
    return SoFontNameElement::get(state);
}

float
SoCallbackAction::getFontSize() const
{
    return SoFontSizeElement::get(state);
}

SoLightModel::Model
SoCallbackAction::getLightModel() const
{
    return (SoLightModel::Model) SoLazyElement::getLightModel(state);
}

const SbVec3f &
SoCallbackAction::getLightAttenuation() const
{
    return SoLightAttenuationElement::get(state);
}

void
SoCallbackAction::getMaterial(SbColor &ambient,
			      SbColor &diffuse,
			      SbColor &specular,
			      SbColor &emission,
			      float   &shininess,
			      float   &transparency,
			      int     mtlIndex) const
{
    const SoLazyElement* le = SoLazyElement::getInstance(state); 
    int32_t ind = le->getNumTransparencies() - 1; 

#ifdef DEBUG
    if ( mtlIndex > le->getNumDiffuse()){
	SoDebugError::post("SoCallbackAction::getMaterial", 
	    "not enough diffuse colors");
    }
    if (( mtlIndex > ind )&& (ind > 0)){
	SoDebugError::post("SoCallbackAction::getMaterial", 
	    "not enough transparencies");
    }
#endif /*DEBUG*/
    ambient = SoLazyElement::getAmbient(state);
    diffuse = SoLazyElement::getDiffuse(state, mtlIndex);
    specular = SoLazyElement::getSpecular(state);
    emission = SoLazyElement::getEmissive(state);
    shininess = SoLazyElement::getShininess(state);

    if (ind > 0) ind = mtlIndex;
	else ind = 0;
    transparency = SoLazyElement::getTransparency(state, ind);
}

SoMaterialBinding::Binding
SoCallbackAction::getMaterialBinding() const
{
    return (SoMaterialBinding::Binding) SoMaterialBindingElement::get(state);
}

int32_t
SoCallbackAction::getNumNormals() const
{
    return SoNormalElement::getInstance(state)->getNum();
}

const SbVec3f &
SoCallbackAction::getNormal(int index) const
{
    return SoNormalElement::getInstance(state)->get(index);
}

SoNormalBinding::Binding
SoCallbackAction::getNormalBinding() const
{
    return (SoNormalBinding::Binding) SoNormalBindingElement::get(state);
}

int32_t
SoCallbackAction::getNumProfileCoordinates() const
{
    return SoProfileCoordinateElement::getInstance(state)->getNum();
}

const SbVec2f &
SoCallbackAction::getProfileCoordinate2(int index) const
{
    return SoProfileCoordinateElement::getInstance(state)->get2(index);
}

const SbVec3f &
SoCallbackAction::getProfileCoordinate3(int index) const
{
    return SoProfileCoordinateElement::getInstance(state)->get3(index);
}

const SoNodeList &
SoCallbackAction::getProfile() const
{
    return SoProfileElement::get(state);
}

SoShapeHints::VertexOrdering
SoCallbackAction::getVertexOrdering() const
{
    SoShapeHintsElement::VertexOrdering	v;
    SoShapeHintsElement::ShapeType	s;
    SoShapeHintsElement::FaceType	f;

    SoShapeHintsElement::get(state, v, s, f);
    return (SoShapeHints::VertexOrdering) v;
}

SoShapeHints::ShapeType
SoCallbackAction::getShapeType() const
{
    SoShapeHintsElement::VertexOrdering	v;
    SoShapeHintsElement::ShapeType	s;
    SoShapeHintsElement::FaceType	f;

    SoShapeHintsElement::get(state, v, s, f);
    return (SoShapeHints::ShapeType) s;
}

SoShapeHints::FaceType
SoCallbackAction::getFaceType() const
{
    SoShapeHintsElement::VertexOrdering	v;
    SoShapeHintsElement::ShapeType	s;
    SoShapeHintsElement::FaceType	f;

    SoShapeHintsElement::get(state, v, s, f);
    return (SoShapeHints::FaceType) f;
}

float
SoCallbackAction::getCreaseAngle() const
{
    return SoCreaseAngleElement::get(state);
}

int32_t
SoCallbackAction::getNumTextureCoordinates() const
{
    return SoTextureCoordinateElement::getInstance(state)->getNum();
}

const SbVec2f &
SoCallbackAction::getTextureCoordinate2(int index) const
{
    return SoTextureCoordinateElement::getInstance(state)->get2(index);
}

const SbVec4f &
SoCallbackAction::getTextureCoordinate4(int index) const
{
    return SoTextureCoordinateElement::getInstance(state)->get4(index);
}

SoTextureCoordinateBinding::Binding
SoCallbackAction::getTextureCoordinateBinding() const
{
    return (SoTextureCoordinateBinding::Binding)
	SoTextureCoordinateBindingElement::get(state);
}

const SbColor &
SoCallbackAction::getTextureBlendColor() const
{
    SbVec2s size;
    int numComponents, wrapS, wrapT, model;
    static SbColor blendColor;
    const unsigned char *bytes;
    bytes = SoTextureImageElement::get(
	state, size, numComponents, wrapS, wrapT, model,
	blendColor);

    return blendColor;
}

const unsigned char *
SoCallbackAction::getTextureImage(SbVec2s &size, int &numComponents) const
{
    int wrapS, wrapT, model;
    static SbColor blendColor;
    const unsigned char *bytes;
    bytes = SoTextureImageElement::get(
	state, size, numComponents, wrapS, wrapT, model,
	blendColor);

    return bytes;
}

const SbMatrix &
SoCallbackAction::getTextureMatrix() const
{
    return SoTextureMatrixElement::get(state);
}

SoTexture2::Model
SoCallbackAction::getTextureModel() const
{
    SbVec2s size;
    int numComponents, wrapS, wrapT, model;
    static SbColor blendColor;
    const unsigned char *bytes;
    bytes = SoTextureImageElement::get(
	state, size, numComponents, wrapS, wrapT, model,
	blendColor);
    return (SoTexture2::Model)model;
}

SoTexture2::Wrap
SoCallbackAction::getTextureWrapS() const
{
    SbVec2s size;
    int numComponents, wrapS, wrapT, model;
    static SbColor blendColor;
    const unsigned char *bytes;
    bytes = SoTextureImageElement::get(
	state, size, numComponents, wrapS, wrapT, model,
	blendColor);
    return (SoTexture2::Wrap)wrapS;
}

SoTexture2::Wrap
SoCallbackAction::getTextureWrapT() const
{
    SbVec2s size;
    int numComponents, wrapS, wrapT, model;
    static SbColor blendColor;
    const unsigned char *bytes;
    bytes = SoTextureImageElement::get(
	state, size, numComponents, wrapS, wrapT, model,
	blendColor);
    return (SoTexture2::Wrap)wrapT;
}

const SbMatrix &
SoCallbackAction::getModelMatrix() const
{
    return SoModelMatrixElement::get(state);
}

SoUnits::Units
SoCallbackAction::getUnits() const
{
    return (SoUnits::Units) SoUnitsElement::get(state);
}

float
SoCallbackAction::getFocalDistance() const
{
    return SoFocalDistanceElement::get(state);
}

const SbMatrix &
SoCallbackAction::getProjectionMatrix() const
{
    return SoProjectionMatrixElement::get(state);
}

const SbMatrix &
SoCallbackAction::getViewingMatrix() const
{
    return SoViewingMatrixElement::get(state);
}

const SbViewVolume &
SoCallbackAction::getViewVolume() const
{
    return SoViewVolumeElement::get(state);
}

SoPickStyle::Style
SoCallbackAction::getPickStyle() const
{
    return (SoPickStyle::Style) SoPickStyleElement::get(state);
}

int32_t
SoCallbackAction::getSwitch() const
{
    return SoSwitchElement::get(state);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Invoke pre callbacks before a node is traversed.
//
// Use: internal

void
SoCallbackAction::invokePreCallbacks(const SoNode *node)
//
////////////////////////////////////////////////////////////////////////
{
    Response		newResponse;
    nodeTypeCallback	*typeCb;
    tailCallback	*tailCb;
    int			i;

    // If we had been pruning, stop. (We know that if this node is
    // traversed, it wasn't pruned.)
    if (response == PRUNE)
	response = CONTINUE;

    for (i = 0; i < preCallbackList.getLength(); i++) {
	typeCb = (nodeTypeCallback *)preCallbackList[i];
	if (node->isOfType(typeCb->type)) {
	    newResponse = (typeCb->cb)(typeCb->data, this, node);
	    if (newResponse != CONTINUE) {
		response = newResponse;
		if (newResponse == ABORT) {
		    setTerminated(TRUE);
		    return;
		}
	    }
	}
    }

    const SoPath *pathAppliedTo = getPathAppliedTo();

    if (preTailCallbackList.getLength() > 0 &&
	pathAppliedTo != NULL &&
	(*getCurPath()) == (*pathAppliedTo)) {

	for (i = 0; i < preTailCallbackList.getLength(); i++) {
	    tailCb = (tailCallback *)preTailCallbackList[i];
	    newResponse = (tailCb->cb)(tailCb->data, this, node);
	    if (newResponse != CONTINUE) {
		response = newResponse;
		if (newResponse == ABORT) {
		    setTerminated(TRUE);
		    return;
		}
	    }
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Invoke post callbacks.
//
// Use: internal

void
SoCallbackAction::invokePostCallbacks(const SoNode *node)
//
////////////////////////////////////////////////////////////////////////
{
    Response		newResponse;
    nodeTypeCallback	*typeCb;
    tailCallback	*tailCb;
    int			i;

    if (response == PRUNE)
	response = CONTINUE;

    for (i = 0; i < postCallbackList.getLength(); i++) {
	typeCb = (nodeTypeCallback *)postCallbackList[i];
	if (node->isOfType(typeCb->type)) {
	    newResponse = (typeCb->cb)(typeCb->data, this, node);
	    if (newResponse != CONTINUE)
		response = newResponse;
	    if (newResponse == ABORT) {
		setTerminated(TRUE);
		return;
	    }
	}
    }

    const SoPath *pathAppliedTo = getPathAppliedTo();

    if (postTailCallbackList.getLength() > 0 &&
	pathAppliedTo != NULL &&
	(*getCurPath()) == (*pathAppliedTo)) {

	for (i = 0; i < postTailCallbackList.getLength(); i++) {
	    tailCb = (tailCallback *)postTailCallbackList[i];
	    newResponse = (tailCb->cb)(tailCb->data, this, node);
	    if (newResponse != CONTINUE)
		response = newResponse;
	    if (newResponse == ABORT) {
		setTerminated(TRUE);
		return;
	    }
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Invoke triangle callbacks.
//
// Use: internal

void
SoCallbackAction::invokeTriangleCallbacks(const SoShape *shape,
					  const SoPrimitiveVertex *v1,
					  const SoPrimitiveVertex *v2,
					  const SoPrimitiveVertex *v3)
//
////////////////////////////////////////////////////////////////////////
{
    triangleCallback	*triCb;
    int			i;

    for (i = 0; i < triangleCallbackList.getLength(); i++) {
	triCb = (triangleCallback *)triangleCallbackList[i];
	if (shape->isOfType(triCb->type))
	    (triCb->cb)(triCb->data, this, v1, v2, v3);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Invoke line segment callbacks.
//
// Use: internal

void
SoCallbackAction::invokeLineSegmentCallbacks(const SoShape *shape,
					     const SoPrimitiveVertex *v1,
					     const SoPrimitiveVertex *v2)
//
////////////////////////////////////////////////////////////////////////
{
    lineSegmentCallback	*lsCb;
    int			i;

    for (i = 0; i < lineSegmentCallbackList.getLength(); i++) {
	lsCb = (lineSegmentCallback *)lineSegmentCallbackList[i];
	if (shape->isOfType(lsCb->type))
	    (lsCb->cb)(lsCb->data, this, v1, v2);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Invoke point callbacks.
//
// Use: internal

void
SoCallbackAction::invokePointCallbacks(const SoShape *shape,
				       const SoPrimitiveVertex *v)
//
////////////////////////////////////////////////////////////////////////
{
    pointCallback	*pntCb;
    int			i;

    for (i = 0; i < pointCallbackList.getLength(); i++) {
	pntCb = (pointCallback *)pointCallbackList[i];
	if (shape->isOfType(pntCb->type))
	    (pntCb->cb)(pntCb->data, this, v);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if the given node should have primitives generated
//    for it.  SoShape takes care of checking this flag and calling
//    (or not calling) the appropriate callbacks.
//
// Use: internal

SbBool
SoCallbackAction::shouldGeneratePrimitives(const SoShape *shape) const
//
////////////////////////////////////////////////////////////////////////
{
    triangleCallback	*triCb;
    lineSegmentCallback	*lsCb;
    pointCallback	*pntCb;
    int			i;

    // Look at each callback list to see if there is an occurrence of
    // this shape type.  Return TRUE if the type is found.
    for (i = 0; i < triangleCallbackList.getLength(); i++) {
	triCb = (triangleCallback *)triangleCallbackList[i];
	if (shape->isOfType(triCb->type))
            return TRUE; 
    }
    for (i = 0; i < lineSegmentCallbackList.getLength(); i++) {
	lsCb = (lineSegmentCallback *)lineSegmentCallbackList[i];
	if (shape->isOfType(lsCb->type))
            return TRUE;
    }
    for (i = 0; i < pointCallbackList.getLength(); i++) {
	pntCb = (pointCallback *)pointCallbackList[i];
	if (shape->isOfType(pntCb->type))
            return TRUE;
    }

    return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initiates action on a graph.
//
// Use: protected

void
SoCallbackAction::beginTraversal(SoNode *node)
//
////////////////////////////////////////////////////////////////////////
{
    response = CONTINUE;

    traverse(node);
}

/////////////////////////////////////////////////////////////////////////
//
// Description:
//     Provides current node being traversed
//     Relies on SoNode::callbackS setting the node, rather than
//     the path being accumulated during traversal.
//
// Use: public, virtual

SoNode *
SoCallbackAction::getCurPathTail()
{
#ifdef DEBUG
    if ( currentNode != ((SoFullPath*)getCurPath())->getTail()){
	SoDebugError::post("SoCallbackAction::getCurPathTail\n", 
	"Path tail inconsistent.  Did you change the scene graph\n"
	"during a callback action?\n");
    }
#endif /*DEBUG*/
    return(currentNode);
}
