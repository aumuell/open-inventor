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
 |   $Revision: 1.1 $
 |
 |   Classes:
 |      SoIndexedLineSet
 |
@@ This next line is true for the output file only - ignore it here:
 |   Note: This file was preprocessed from another file. Do not edit it.
 |
 |   Author(s)          : Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <GL/gl.h>
#include <Inventor/SoPrimitiveVertex.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/bundles/SoMaterialBundle.h>
#include <Inventor/bundles/SoTextureCoordinateBundle.h>
#include <Inventor/caches/SoBoundingBoxCache.h>
#include <Inventor/details/SoLineDetail.h>
#include <Inventor/elements/SoDrawStyleElement.h>
#include <Inventor/elements/SoGLCoordinateElement.h>
#include <Inventor/elements/SoGLTextureCoordinateElement.h>
#include <Inventor/elements/SoGLNormalElement.h>
#include <Inventor/elements/SoLightModelElement.h>
#include <Inventor/elements/SoMaterialBindingElement.h>
#include <Inventor/elements/SoNormalBindingElement.h>
#include <Inventor/elements/SoTextureCoordinateBindingElement.h>
#include <Inventor/elements/SoGLCacheContextElement.h>
#include <Inventor/misc/SoState.h>
#include <Inventor/nodes/SoIndexedLineSet.h>

SO_NODE_SOURCE(SoIndexedLineSet);

// Constants for influencing auto-caching algorithm:
const int AUTO_CACHE_ILS_MIN_WITHOUT_VP = 20;

// And the number above which we'll say caches definitely SHOULDN'T be
// built (because they'll use too much memory):
const int AUTO_CACHE_ILS_MAX = 1000;

// 32 different rendering loops; the 5 bits used to determine the
// rendering case are:
// 43210  BITS            Routine suffix
// -----  ----            --------------
// 00...  Overall mtl     (Om)
// 01...  Part mtl        (Pm)
// 10...  Face mtl        (Fm)
// 11...  Vtx mtl         (Vm)
// ..00.  Overall/No norm (On)
// ..01.  Part norm       (Pn)
// ..10.  Face norm       (Fn)
// ..11.  Vtx norm        (Vn)
// ....0  No texcoord     -none-
// ....1  Vtx texcoord    (T)
//
SoIndexedLineSet::PMILS SoIndexedLineSet::renderFunc[32] = {
    &SoIndexedLineSet::OmOn, &SoIndexedLineSet::OmOnT,
    &SoIndexedLineSet::OmPn, &SoIndexedLineSet::OmPnT,
    &SoIndexedLineSet::OmFn, &SoIndexedLineSet::OmFnT,
    &SoIndexedLineSet::OmVn, &SoIndexedLineSet::OmVnT,
    &SoIndexedLineSet::PmOn, &SoIndexedLineSet::PmOnT,
    &SoIndexedLineSet::PmPn, &SoIndexedLineSet::PmPnT,
    &SoIndexedLineSet::PmFn, &SoIndexedLineSet::PmFnT,
    &SoIndexedLineSet::PmVn, &SoIndexedLineSet::PmVnT,
    &SoIndexedLineSet::FmOn, &SoIndexedLineSet::FmOnT,
    &SoIndexedLineSet::FmPn, &SoIndexedLineSet::FmPnT,
    &SoIndexedLineSet::FmFn, &SoIndexedLineSet::FmFnT,
    &SoIndexedLineSet::FmVn, &SoIndexedLineSet::FmVnT,
    &SoIndexedLineSet::VmOn, &SoIndexedLineSet::VmOnT,
    &SoIndexedLineSet::VmPn, &SoIndexedLineSet::VmPnT,
    &SoIndexedLineSet::VmFn, &SoIndexedLineSet::VmFnT,
    &SoIndexedLineSet::VmVn, &SoIndexedLineSet::VmVnT,
    };

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoIndexedLineSet::SoIndexedLineSet()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoIndexedLineSet);
    isBuiltIn = TRUE;
    numSegments = numPolylines = -1;
    numVertices = NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoIndexedLineSet::~SoIndexedLineSet()
//
////////////////////////////////////////////////////////////////////////
{
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Generates line segments representing a line set.
//
// Use: protected

void
SoIndexedLineSet::generatePrimitives(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoState *state = action->getState();

    state->push();
    //  put VertProp into state, if it exists.
    SoVertexProperty *vp = (SoVertexProperty*)vertexProperty.getValue();
    if(vp){
	vp->doAction(action);
    }
    // When generating primitives for picking, delay computing default
    // texture coordinates
    SbBool forPicking = action->isOfType(SoRayPickAction::getClassTypeId());

    SoPrimitiveVertex		pvs[2], *pv;
    SoLineDetail		detail;
    SoPointDetail		pd;
    SoTextureCoordinateBundle	tcb(action, FALSE, ! forPicking);
    const SoCoordinateElement	*ce;
    int				curLine, curSeg, curVert, curCoord;
    int				curMaterial, curNormal, curTexCoord;
    int				vertsInLine;
    int				numIndices;
    const int32_t			*coordIndices, *matlIndices;
    const int32_t			*normIndices, *texCoordIndices;
    Binding			materialBinding, normalBinding;
    SbBool			texCoordsIndexed;

    materialBinding  = getMaterialBinding(action);
    normalBinding    = getNormalBinding(action);
    texCoordsIndexed = areTexCoordsIndexed(action);

    // Test for auto-normal case
    const SoNormalElement *ne = SoNormalElement::getInstance(state);
    if (ne->getNum() == 0) {
	normalBinding = OVERALL;
    }

    curLine = curSeg = curCoord = curVert = vertsInLine = 0;

    ce = SoCoordinateElement::getInstance(state);

    pvs[0].setDetail(&detail);
    pvs[1].setDetail(&detail);

    numIndices      = coordIndex.getNum();
    coordIndices    = coordIndex.getValues(0);
    matlIndices     = materialIndex.getValues(0);
    normIndices     = normalIndex.getValues(0);
    texCoordIndices = textureCoordIndex.getValues(0);

    // Check for special case of 1 index of SO_END_LINE_INDEX. This
    // means that coord indices are to be used for materials, normals,
    // or texture coords as well
    if (materialIndex.getNum() == 1 && matlIndices[0] == SO_END_LINE_INDEX)
	matlIndices = coordIndices;
    if (normalIndex.getNum() == 1 && normIndices[0] == SO_END_LINE_INDEX) 
	normIndices = coordIndices;
    if (textureCoordIndex.getNum() == 1 &&
	texCoordIndices[0] == SO_END_LINE_INDEX)
	texCoordIndices = coordIndices;

    if (forPicking) {
	SbVec4f	tc(0.0, 0.0, 0.0, 0.0);
	pvs[0].setTextureCoords(tc);
	pvs[1].setTextureCoords(tc);
    }

    // Step through all the coordinate indices, building lines out
    // of them, until we run out of coordinate indices.

    while (curCoord < numIndices) {

	detail.setLineIndex(curLine);

	// Loop through all vertices of current line
	while (curCoord < numIndices &&
	       coordIndices[curCoord] != SO_END_LINE_INDEX) {

	    switch (materialBinding) {
	      case OVERALL:
		curMaterial = 0;
		break;
	      case PER_SEGMENT:
		curMaterial = curSeg;
		break;
	      case PER_SEGMENT_INDEXED:
		curMaterial = (int) matlIndices[curSeg];
		break;
	      case PER_LINE:
		curMaterial = curLine;
		break;
	      case PER_LINE_INDEXED:
		curMaterial = (int) matlIndices[curLine];
		break;
	      case PER_VERTEX:
		curMaterial = curVert;
		break;
	      case PER_VERTEX_INDEXED:
		curMaterial = (int) matlIndices[curCoord];
		break;
	    }
	    switch (normalBinding) {
	      case OVERALL:
		curNormal = 0;
		break;
	      case PER_SEGMENT:
		curNormal = curSeg;
		break;
	      case PER_SEGMENT_INDEXED:
		curNormal = (int) normIndices[curSeg];
		break;
	      case PER_LINE:
		curNormal = curLine;
		break;
	      case PER_LINE_INDEXED:
		curNormal = (int) normIndices[curLine];
		break;
	      case PER_VERTEX:
		curNormal = curVert;
		break;
	      case PER_VERTEX_INDEXED:
		curNormal = (int) normIndices[curCoord];
		break;
	    }
	    curTexCoord = (texCoordsIndexed ?
			   (int) texCoordIndices[curCoord] : curCoord);

	    pv = &pvs[curVert % 2];

	    pv->setPoint(ce->get3((int) coordIndices[curCoord]));
	    pv->setMaterialIndex(curMaterial);
	    if (curNormal < ne->getNum())
		pv->setNormal(ne->get(curNormal));
	    else pv->setNormal(SbVec3f(0,0,0));

	    // Set up a point detail for the current vertex
	    pd.setCoordinateIndex((int) coordIndices[curCoord]);
	    pd.setMaterialIndex(curMaterial);
	    pd.setNormalIndex(curNormal);
	    pd.setTextureCoordIndex(curTexCoord);

	    // Replace the appropriate point detail in the line
	    // detail, based on the vertex index
	    if ((curVert & 1) == 0)
		detail.setPoint0(&pd);
	    else
		detail.setPoint1(&pd);

	    if (tcb.isFunction()) {
		if (! forPicking)
		    pv->setTextureCoords(tcb.get(pv->getPoint(),
						 pv->getNormal()));
	    }
	    else
		pv->setTextureCoords(tcb.get(curTexCoord));

	    // If we have at least two vertices in the current line
	    if (++vertsInLine >= 2) {
		detail.setPartIndex(curSeg);

		// Handle per-segment stuff specially, since we have
		// to make sure both points/details are the same
		if (materialBinding == PER_SEGMENT ||
		    materialBinding == PER_SEGMENT_INDEXED) {
		    pvs[0].setMaterialIndex(curMaterial);
		    pvs[1].setMaterialIndex(curMaterial);
		}
		if (normalBinding == PER_SEGMENT ||
		    normalBinding == PER_SEGMENT_INDEXED) {
		    pvs[0].setNormal(ne->get(curNormal));
		    pvs[1].setNormal(ne->get(curNormal));
		}

		invokeLineSegmentCallbacks(action,
					   &pvs[(curVert - 1) % 2],
					   &pvs[(curVert - 0) % 2]);

		//
		// Increment per-segment stuff
		//
		curSeg++;
	    }

	    //
	    // Increment per-vertex stuff
	    //
	    curVert++;
	    curCoord++;
	}

	//
	// Increment per-line stuff
	//
	curCoord++; 	// Skip over the END_LINE_INDEX
	curLine++;
	vertsInLine = 0;
    }
    state->pop();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Redefines this to tell open caches that they contain lines.
//
// Use: extender

void
SoIndexedLineSet::getBoundingBox(SoGetBoundingBoxAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // Let our parent class do the real work
    SoIndexedShape::getBoundingBox(action);

    // If there are any open bounding box caches, tell them that they
    // contain lines
    SoBoundingBoxCache::setHasLinesOrPoints(action->getState());
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Overrides standard method to create an SoLineDetail instance
//    representing a picked intersection with a vertex of a line set.
//
// Use: protected, virtual

SoDetail *
SoIndexedLineSet::createLineSegmentDetail(SoRayPickAction *action,
					  const SoPrimitiveVertex *v1,
					  const SoPrimitiveVertex *,
					  SoPickedPoint *pp)
//
////////////////////////////////////////////////////////////////////////
{
    SoLineDetail	*detail = new SoLineDetail;
    const SoLineDetail	*d;

    d = (const SoLineDetail *) v1->getDetail();

    detail->setPoint0(d->getPoint0());
    detail->setPoint1(d->getPoint1());

    // Compute texture coordinates at intersection point and store it
    // in the picked point
    SoTextureCoordinateBundle	tcb(action, FALSE);
    if (tcb.isFunction())
	pp->setObjectTextureCoords(tcb.get(pp->getObjectPoint(),
					   pp->getObjectNormal()));

    // The face/part indices are in the incoming details
    detail->setLineIndex(d->getLineIndex());
    detail->setPartIndex(d->getPartIndex());

    return detail;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns current material binding from action's state.
//
// Use: private, static

SoIndexedLineSet::Binding
SoIndexedLineSet::getMaterialBinding(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    switch (SoMaterialBindingElement::get(action->getState())) {

      case SoMaterialBindingElement::OVERALL:
	return OVERALL;

      case SoMaterialBindingElement::PER_PART:
	return PER_SEGMENT;

      case SoMaterialBindingElement::PER_PART_INDEXED:
	return PER_SEGMENT_INDEXED;

      case SoMaterialBindingElement::PER_FACE:
	return PER_LINE;

      case SoMaterialBindingElement::PER_FACE_INDEXED:
	return PER_LINE_INDEXED;

      case SoMaterialBindingElement::PER_VERTEX:
	return PER_VERTEX;

      case SoMaterialBindingElement::PER_VERTEX_INDEXED:
	return PER_VERTEX_INDEXED;
    }
    return OVERALL; // Shut up C++ compiler
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns current normal binding from action's state.
//
// Use: private, static

SoIndexedLineSet::Binding
SoIndexedLineSet::getNormalBinding(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    switch (SoNormalBindingElement::get(action->getState())) {
      case SoNormalBindingElement::OVERALL:
	return OVERALL;

      case SoNormalBindingElement::PER_PART:
	return PER_SEGMENT;

      case SoNormalBindingElement::PER_PART_INDEXED:
	return PER_SEGMENT_INDEXED;

      case SoNormalBindingElement::PER_FACE:
	return PER_LINE;

      case SoNormalBindingElement::PER_FACE_INDEXED:
	return PER_LINE_INDEXED;

      case SoNormalBindingElement::PER_VERTEX:
	return PER_VERTEX;

      case SoNormalBindingElement::PER_VERTEX_INDEXED:
	return PER_VERTEX_INDEXED;
    }
    return OVERALL; // Shut up C++ compiler
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if normal binding is AUTO and there aren't enough
//    PER_VERTEX normals, in which case we turn off lighting and draw
//    BASE_COLOR.
//
// Use: private, static

SbBool
SoIndexedLineSet::wouldGenerateNormals(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
    if (SoNormalBindingElement::get(state) ==
	SoNormalBindingElement::PER_VERTEX_INDEXED) {
	
	// Figure out how many normals we need:
	int i;
	int numNeeded = 0;
	const SoMFInt32 *nIndices;

	if (normalIndex.getNum() == 1 &&
	    normalIndex[0] == SO_END_LINE_INDEX) {
	    nIndices = &coordIndex;
	}
	else {
	    nIndices = &normalIndex;
	}
	// Find greatest index:
	for (i = 0; i < nIndices->getNum(); i++) {
	    if ((*nIndices)[i] > numNeeded)
		numNeeded = (int) (*nIndices)[i];
	}
	const SoNormalElement *ne = SoNormalElement::getInstance(state);
	if (numNeeded > ne->getNum()) return TRUE;
    }
    
    return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements GL rendering.
//
// Use: extender

void
SoIndexedLineSet::GLRender(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoState *state = action->getState();

    // Get ShapeStyleElement
    SoShapeStyleElement *shapeStyle = 
	(SoShapeStyleElement*)SoShapeStyleElement::get(state);

    // First see if the object is visible and should be rendered now:
    if (shapeStyle->mightNotRender()) {
        if (! shouldGLRender(action))
            return;
    }

    if (vpCache.mightNeedSomethingFromState(shapeStyle)) {
	// Push state, in case need change to base_color lighting:
	// Rather than generate normals, we just turn off lighting if
	// normals are needed.
	state->push();

	SoVertexProperty *vp = (SoVertexProperty *)vertexProperty.getValue();
        vpCache.fillInCache(vp, state);

	if (vpCache.shouldGenerateNormals(shapeStyle)) {
	    // turn off lighting
	    SoGLLazyElement::setLightModel(state, SoLazyElement::BASE_COLOR);
	    // reobtain shapeStyleElement 
	    shapeStyle = 	
		(SoShapeStyleElement*)SoShapeStyleElement::get(state);
        }

	// Setup numVertices, numPolylines and numSegments:
	if (numPolylines < 0)
	    countPolylinesAndSegments();

	SoTextureCoordinateBundle *tcb = NULL;
	uint32_t useTexCoordsAnyway = 0;
        if (vpCache.shouldGenerateTexCoords(shapeStyle)) {	
	    tcb = new SoTextureCoordinateBundle(action, TRUE, TRUE);
	}
	else if (shapeStyle->isTextureFunction() && vpCache.haveTexCoordsInVP()){
	    useTexCoordsAnyway = SoVertexPropertyCache::TEXCOORD_BIT;
	    SoGLTextureCoordinateElement::setTexGen(state, this, NULL);
	}
	 
	// set up pointers
	// (this is a method on SoIndexedShape):
	// note that segments correspond to parts, polylines to faces
	setupIndices(numSegments, numPolylines, shapeStyle->needNormals(), 
		(useTexCoordsAnyway || shapeStyle->needTexCoords()));

	//If lighting or texturing is off, this vpCache and other things
	//need to be reconstructed when lighting or texturing is turned
	//on, so we set the bits in the VP cache:
	if(! shapeStyle->needNormals()) vpCache.needFromState |= 
		SoVertexPropertyCache::NORMAL_BITS;
	if(! shapeStyle->needTexCoords()) vpCache.needFromState |= 
		SoVertexPropertyCache::TEXCOORD_BIT;

	// If doing multiple colors, turn on ColorMaterial:
	if (vpCache.getNumColors() > 1) {
	    SoGLLazyElement::setColorMaterial(state, TRUE);
	}
	//
	// Ask LazyElement to setup:
	//
	SoGLLazyElement *lazyElt = (SoGLLazyElement *)
	    SoLazyElement::getInstance(state);

	if(vpCache.colorIsInVtxProp()){
	    lazyElt->send(state, SoLazyElement::ALL_MASK);
	    lazyElt->sendVPPacked(state, (const unsigned char*)
		vpCache.getColors(0));
	}
	else lazyElt->send(state, SoLazyElement::ALL_MASK);
	
	// Call the appropriate render loop:
	(this->*renderFunc[useTexCoordsAnyway | 
		vpCache.getRenderCase(shapeStyle)])(action);
	
	// If doing multiple colors, turn off ColorMaterial:
	if (vpCache.getNumColors() > 1) {
	    SoGLLazyElement::setColorMaterial(state, FALSE);
	    ((SoGLLazyElement *)SoLazyElement::getInstance(state))->
	    	reset(state, SoLazyElement::DIFFUSE_MASK);
	}
	    
	
	// Influence auto-caching algorithm:
	if (coordIndex.getNum() < AUTO_CACHE_ILS_MIN_WITHOUT_VP &&
	    vpCache.mightNeedSomethingFromState(shapeStyle)) {
	    SoGLCacheContextElement::shouldAutoCache(state,
		SoGLCacheContextElement::DO_AUTO_CACHE);
	} else if (coordIndex.getNum() > AUTO_CACHE_ILS_MAX &&
		   !SoGLCacheContextElement::getIsRemoteRendering(state)) {
	    SoGLCacheContextElement::shouldAutoCache(state,
			SoGLCacheContextElement::DONT_AUTO_CACHE);
	}	    

        if (tcb) {
	    delete tcb;
	}
	state->pop();
    }
    else {
	// If doing multiple colors, turn on ColorMaterial:
	if (vpCache.getNumColors() > 1) {
	    SoGLLazyElement::setColorMaterial(state, TRUE);
	}
	//
	// Ask LazyElement to setup:
	//
	SoGLLazyElement *lazyElt = (SoGLLazyElement *)
	    SoLazyElement::getInstance(state);
	
	if(vpCache.colorIsInVtxProp()){
	    lazyElt->send(state, SoLazyElement::ALL_MASK);
	    lazyElt->sendVPPacked(state, (const unsigned char*)
		vpCache.getColors(0));
	}
	else lazyElt->send(state, SoLazyElement::ALL_MASK);

	// Call the appropriate render loop:
	(this->*renderFunc[vpCache.getRenderCase(shapeStyle)])(action);	
	if (vpCache.getNumColors() > 1) {
	    SoGLLazyElement::setColorMaterial(state, FALSE);
	    ((SoGLLazyElement *)SoLazyElement::getInstance(state))->
	    	reset(state, SoLazyElement::DIFFUSE_MASK);
	}

	// Influence auto-caching algorithm:
	if (coordIndex.getNum() > AUTO_CACHE_ILS_MAX &&
	    !SoGLCacheContextElement::getIsRemoteRendering(state)) {

	    SoGLCacheContextElement::shouldAutoCache(state,
			SoGLCacheContextElement::DONT_AUTO_CACHE);
	}	    
    }
    return;
}

///////////////////////////////////////////////////////////////////////////
//
// Description:
//	Count vertices in each polyline, construct numVertices array.
//	must be invoked whenever vertexPropertyCache is built.
//
// use: private
//
///////////////////////////////////////////////////////////////////////////
void 
SoIndexedLineSet::countPolylinesAndSegments()
{
    if (numPolylines > 0) return; // Already counted
    numPolylines = 0;
    int i, numVerts = 0;
    for(i = 0; i < coordIndex.getNum(); i++){
	if (coordIndex[i] == SO_END_LINE_INDEX || 
	    (i == coordIndex.getNum()-1)) {
	    ++numPolylines;
	} 
	if (coordIndex[i] != SO_END_LINE_INDEX) {
	    ++numVerts;
	}
    }
    numSegments = numVerts - numPolylines;

    numVertices = new int[numPolylines];
    // Then fill in its values:
    int np = 0;
    int nv = 0;
    for(i = 0; i< coordIndex.getNum(); i++){
	if (coordIndex[i] == SO_END_LINE_INDEX ){
	    numVertices[np] = nv;
	    nv=0;
	    np++;	        
	}
	else {
	    nv++;
	    if (i == coordIndex.getNum()-1){
		numVertices[np] = nv;
	    }
	}	
    }
}    
    
////////////////////////////////////////////////////////////////////////
//
// Description:
//    Keep things up to date when my fields change
//
// Use: protected

void
SoIndexedLineSet::notify(SoNotList *list)
//
////////////////////////////////////////////////////////////////////////
{
    // If coordIndex changes, must recount:
    if (list->getLastRec()->getType() == SoNotRec::CONTAINER &&
	list->getLastField() == &coordIndex) {
	if (numVertices)
	    delete[] numVertices;
	numVertices = NULL;
	numPolylines = numSegments = -1;
    }

    SoIndexedShape::notify(list);
}    

//////////////////////////////////////////////////////////////////////////
// Following preprocessor-generated routines handle all combinations of
// Normal binding (per vertex, per face, per part, overall/none)
// Color Binding (per vertex, per face, per part, overall)
// Textures (on or off)
//////////////////////////////////////////////////////////////////////////
@@ ----------------------------------------------------------------------
@@ General prototype of renderfunction is as follows:
@@
@={ RenderFunction
void
SoIndexedLineSet::
@ FuncName 
    (SoGLRenderAction* action ) {
    const int np = numPolylines;
    const int* numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    SbBool renderAsPoints = (SoDrawStyleElement::get(action->getState()) ==
		      SoDrawStyleElement::POINTS);

@?{ On
    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
@?}
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
@?{ Pm | Fm | Vm
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
@?}
@?{ Pn
    int nrmCtr = 0;
@?}
@?{ Pm
    int clrCtr = 0;
@?}
@?{ Pn | Fn | Vn
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();
@?}

@?{ T
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int32_t *const tCoordIndx = getTexCoordIndices();
@?} 
    int vtxCtr = 0;
    int v;
    for (int polyline = 0; polyline < np; polyline++) {
@?{ Fm
	(*colorFunc)(colorPtr+colorStride*colorIndx[polyline]);
@?}
@?{ Fn
	(*normalFunc)(normalPtr+normalStride*normalIndx[polyline]);
@?}
@?{ Pn | Pm
	const int nv = (*numverts) -1;
@?}
@?{ Fn | Vn | On
@?{ Fm | Vm | Om
	const int nv = (*numverts);	 
@?}
@?}	    
	if(renderAsPoints){
	    glBegin(GL_POINTS);
	}
	else {
@?{ Pn | Pm
	    glBegin(GL_LINES);
@?{ Vm
	    (*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);
@?}
@?{ Vn
	    (*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr]);	
@?}	
@?}

@?{ Fn | Vn | On
@?{ Fm | Vm | Om
	    glBegin(GL_LINE_STRIP);	 
@?}
@?}	    
	}
	for (v = 0; v < nv; v++) {		    
@?{ Pm | Pn
@?{ Pm
		(*colorFunc)(colorPtr+colorStride*colorIndx[clrCtr++]); 
@?}
@?{ Pn
		(*normalFunc)(normalPtr+normalStride*normalIndx[nrmCtr++]); 		
@?}
@?{ T
		(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
@?}
		(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);
@?{ Vn
		(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr]);
@?}
@?{ Vm
		(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);	   	   
@?}
@?{ T
		(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
@?}
		(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);
@?}  //end Pm | Pn
@?{ Fn | Vn | On
@?{ Fm | Vm | Om
@?{ Vn
		(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr]);
@?}
@?{ Vm
		(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);	   	   
@?}
@?{ T
		(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);	       
@?}
		(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);	   		 
@?}
@?}	   	 
	}
	glEnd();
@?{ Fn | Vn | On
@?{ Fm | Vm | Om
	vtxCtr++;  //skip over -1 at end of polyline
@?}
@?}
@?{ Pm | Pn
	vtxCtr+=2;  // Skip over -1 at end of polyline, plus last vtx.
@?}
	++numverts;
    }
}

@=}

// Material overall:

@= FuncName OmOn
@= Om ON
@= On ON
@= Pm
@= Fm
@= Vm
@= Pn
@= Fn
@= Vn
@= T
@ RenderFunction

@= FuncName OmOnT
@= Om ON
@= On ON
@= Pm
@= Fm
@= Vm
@= Pn
@= Fn
@= Vn
@= T  ON
@ RenderFunction

@= FuncName OmPn
@= Om ON
@= On
@= Pm
@= Fm
@= Vm
@= Pn ON
@= Fn
@= Vn
@= T
@ RenderFunction

@= FuncName OmPnT
@= Om ON
@= On
@= Pm
@= Fm
@= Vm
@= Pn ON
@= Fn
@= Vn
@= T  ON
@ RenderFunction

@= FuncName OmFn
@= Om ON
@= On
@= Pm
@= Fm
@= Vm
@= Pn
@= Fn ON
@= Vn
@= T
@ RenderFunction

@= FuncName OmFnT
@= Om ON
@= On
@= Pm
@= Fm
@= Vm
@= Pn
@= Fn ON
@= Vn
@= T  ON
@ RenderFunction

@= FuncName OmVn
@= Om ON
@= On
@= Pm
@= Fm
@= Vm
@= Pn
@= Fn
@= Vn ON
@= T
@ RenderFunction

@= FuncName OmVnT
@= Om ON
@= On
@= Pm
@= Fm
@= Vm
@= Pn
@= Fn
@= Vn ON
@= T  ON
@ RenderFunction

@= FuncName PmOn
@= Om
@= On ON
@= Pm ON
@= Fm
@= Vm
@= Pn
@= Fn
@= Vn
@= T
@ RenderFunction

@= FuncName PmOnT
@= Om
@= On ON
@= Pm ON
@= Fm
@= Vm
@= Pn
@= Fn
@= Vn
@= T  ON
@ RenderFunction

@= FuncName PmPn
@= Om
@= On
@= Pm ON
@= Fm
@= Vm
@= Pn ON
@= Fn
@= Vn
@= T
@ RenderFunction

@= FuncName PmPnT
@= Om
@= On
@= Pm ON
@= Fm
@= Vm
@= Pn ON
@= Fn
@= Vn
@= T  ON
@ RenderFunction

@= FuncName PmFn
@= Om
@= On
@= Pm ON
@= Fm
@= Vm
@= Pn
@= Fn ON
@= Vn
@= T
@ RenderFunction

@= FuncName PmFnT
@= Om
@= On
@= Pm ON
@= Fm
@= Vm
@= Pn
@= Fn ON
@= Vn
@= T  ON
@ RenderFunction

@= FuncName PmVn
@= Om
@= On
@= Pm ON
@= Fm
@= Vm
@= Pn
@= Fn
@= Vn ON
@= T
@ RenderFunction

@= FuncName PmVnT
@= Om
@= On
@= Pm ON
@= Fm
@= Vm
@= Pn
@= Fn
@= Vn ON
@= T  ON
@ RenderFunction

@= FuncName FmOn
@= Om
@= On ON
@= Pm
@= Fm ON
@= Vm
@= Pn
@= Fn
@= Vn
@= T
@ RenderFunction

@= FuncName FmOnT
@= Om
@= On ON
@= Pm
@= Fm ON
@= Vm
@= Pn
@= Fn
@= Vn
@= T  ON
@ RenderFunction

@= FuncName FmPn
@= Om
@= On
@= Pm
@= Fm ON
@= Vm
@= Pn ON
@= Fn
@= Vn
@= T
@ RenderFunction

@= FuncName FmPnT
@= Om
@= On
@= Pm
@= Fm ON
@= Vm
@= Pn ON
@= Fn
@= Vn
@= T  ON
@ RenderFunction

@= FuncName FmFn
@= Om
@= On
@= Pm
@= Fm ON
@= Vm
@= Pn
@= Fn ON
@= Vn
@= T
@ RenderFunction

@= FuncName FmFnT
@= Om
@= On
@= Pm
@= Fm ON
@= Vm
@= Pn
@= Fn ON
@= Vn
@= T  ON
@ RenderFunction

@= FuncName FmVn
@= Om
@= On
@= Pm
@= Fm ON
@= Vm
@= Pn
@= Fn  
@= Vn ON
@= T   
@ RenderFunction

@= FuncName FmVnT
@= Om
@= On
@= Pm
@= Fm ON
@= Vm
@= Pn
@= Fn  
@= Vn ON
@= T  ON   
@ RenderFunction

@= FuncName VmOn
@= Om
@= On ON
@= Pm
@= Fm
@= Vm ON
@= Pn
@= Fn
@= Vn
@= T
@ RenderFunction

@= FuncName VmOnT
@= Om
@= On ON
@= Pm
@= Fm
@= Vm ON
@= Pn
@= Fn
@= Vn
@= T  ON
@ RenderFunction

@= FuncName VmPn
@= Om
@= On
@= Pm
@= Fm
@= Vm ON
@= Pn ON
@= Fn
@= Vn
@= T
@ RenderFunction

@= FuncName VmPnT
@= Om
@= On
@= Pm
@= Fm
@= Vm ON
@= Pn ON
@= Fn
@= Vn
@= T  ON
@ RenderFunction

@= FuncName VmFn
@= Om
@= On
@= Pm
@= Fm
@= Vm ON
@= Pn  
@= Fn On
@= Vn
@= T   
@ RenderFunction

@= FuncName VmFnT
@= Om
@= On
@= Pm
@= Fm
@= Vm ON
@= Pn
@= Fn ON
@= Vn  
@= T  ON
@ RenderFunction

@= FuncName VmVn
@= Om
@= On
@= Pm
@= Fm
@= Vm ON
@= Pn
@= Fn
@= Vn ON
@= T
@ RenderFunction

@= FuncName VmVnT
@= Om
@= On
@= Pm
@= Fm
@= Vm ON
@= Pn
@= Fn
@= Vn ON
@= T  ON
@ RenderFunction
