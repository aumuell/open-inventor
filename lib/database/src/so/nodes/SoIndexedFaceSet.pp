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
 |      SoIndexedFaceSet
 |
@@ This next line is true for the output file only - ignore it here:
 |   Note: This file was preprocessed from another file. Do not edit it.
 |
 |   Author(s)          : Paul S. Strauss, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <GL/gl.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/SoPrimitiveVertex.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/bundles/SoMaterialBundle.h>
#include <Inventor/bundles/SoNormalBundle.h>
#include <Inventor/bundles/SoTextureCoordinateBundle.h>
#include <Inventor/caches/SoNormalCache.h>
#include <Inventor/details/SoFaceDetail.h>
#include <Inventor/elements/SoGLCoordinateElement.h>
#include <Inventor/elements/SoGLTextureCoordinateElement.h>
#include <Inventor/elements/SoGLCacheContextElement.h>
#include <Inventor/elements/SoGLLazyElement.h>
#include <Inventor/elements/SoNormalBindingElement.h>
#include <Inventor/elements/SoShapeHintsElement.h>
#include <Inventor/elements/SoTextureCoordinateBindingElement.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoNormalBinding.h>

SO_NODE_SOURCE(SoIndexedFaceSet);

// Constants for influencing auto-caching algorithm:

const int AUTO_CACHE_IFS_MIN_WITHOUT_VP = 20;

// And the number above which we'll say caches definitely SHOULDN'T be
// built (because they'll use too much memory):
const int AUTO_CACHE_IFS_MAX = 1000;

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoIndexedFaceSet::SoIndexedFaceSet()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoIndexedFaceSet);

    numTris = numQuads = numFaces = -1;

    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoIndexedFaceSet::~SoIndexedFaceSet()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Performs GL rendering of a indexedFaceSet.
//
// Use: protected

void
SoIndexedFaceSet::GLRender(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoState *state = action->getState();

    // Get ShapeStyleElement
    const SoShapeStyleElement *shapeStyle = SoShapeStyleElement::get(state);

    // First see if the object is visible and should be rendered now:
    if (shapeStyle->mightNotRender()) {
        if (! shouldGLRender(action))
            return;
    }

    if (vpCache.mightNeedSomethingFromState(shapeStyle)) {
	// Set up numTris/Quads/Faces
	if (numTris < 0)
	    setupNumTrisQuadsFaces();

	// If faces might be concave, we have to send them through GLU:
	SoShapeHintsElement::VertexOrdering vo;
	SoShapeHintsElement::ShapeType st;
	SoShapeHintsElement::FaceType ft;
	SoShapeHintsElement::get(state, vo, st, ft);

	if ((numQuads > 0 || numFaces > 0)
	    && ft != SoShapeHintsElement::CONVEX) {

	    // Use generatePrimitives for now...
	    SoShape::GLRender(action);
	    return;
	}
	int numPolys = numTris+numQuads+numFaces;
	if (numPolys == 0 ) return;

	SoVertexProperty *vp = (SoVertexProperty *)vertexProperty.getValue();
        vpCache.fillInCache(vp, state);
	
        if (vpCache.shouldGenerateNormals(shapeStyle)) {
	    // See if there is a valid normal cache. If so, use it:
	    SoNormalCache *normCache = getNormalCache();
	    if (normCache == NULL || !normCache->isValid(state)) {

		// Compute the number of vertices. This is just an
		// approximation, so using 3 verts per unknown polygon
		// is close enough. 
		int numVerts = 3 * numTris + 4 * numQuads + 3 * numFaces;

		SoNormalBundle nb(action, FALSE);
		nb.initGenerator(numVerts);
		generateDefaultNormals(state, &nb);
		normCache = getNormalCache();
	    }
	    vpCache.numNorms = normCache->getNum();
	    vpCache.normalPtr = (const char *)normCache->getNormals();
	    
        }

	SoTextureCoordinateBundle *tcb = NULL;
	uint32_t useTexCoordsAnyway = 0;
        if (vpCache.shouldGenerateTexCoords(shapeStyle)) {
	    state->push();
	    tcb = new SoTextureCoordinateBundle(action, TRUE, TRUE);
	}
	else if (shapeStyle->isTextureFunction() && vpCache.haveTexCoordsInVP()){
	    state->push();
	    useTexCoordsAnyway = SoVertexPropertyCache::TEXCOORD_BIT;
	    SoGLTextureCoordinateElement::setTexGen(state, this, NULL);
	}
	    
	// Now that normals have been generated, can set up pointers
	// (this is a method on SoIndexedShape):
	
	setupIndices(numPolys, numPolys, shapeStyle->needNormals(), 
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

	if (vpCache.colorIsInVtxProp()) {
	    lazyElt->send(state, SoLazyElement::ALL_MASK);
	    lazyElt->sendVPPacked(state, (const unsigned char*)
		vpCache.getColors(0));
	}
	else lazyElt->send(state, SoLazyElement::ALL_MASK);
	
	// Call the appropriate render loops:
	if (numTris > 0)
	    (this->*TriRenderFunc[useTexCoordsAnyway | 
		vpCache.getRenderCase(shapeStyle)])(action);
	if (numQuads > 0)
	    (this->*QuadRenderFunc[useTexCoordsAnyway | 
		vpCache.getRenderCase(shapeStyle)])(action);
	if (numFaces > 0)
	    (this->*GenRenderFunc[useTexCoordsAnyway | 
		vpCache.getRenderCase(shapeStyle)])(action);
	
	// If doing multiple colors, turn off ColorMaterial:
	if (vpCache.getNumColors() > 1) {
	    SoGLLazyElement::setColorMaterial(state, FALSE);
	    ((SoGLLazyElement *)SoLazyElement::getInstance(state))->
	    	reset(state, SoLazyElement::DIFFUSE_MASK);
	}
	
	if (coordIndex.getNum() < AUTO_CACHE_IFS_MIN_WITHOUT_VP &&
	    vpCache.mightNeedSomethingFromState(shapeStyle)) {
	    SoGLCacheContextElement::shouldAutoCache(state,
		SoGLCacheContextElement::DO_AUTO_CACHE);
	} else if (coordIndex.getNum() > AUTO_CACHE_IFS_MAX &&
		   !SoGLCacheContextElement::getIsRemoteRendering(state)) {
	    SoGLCacheContextElement::shouldAutoCache(state,
			SoGLCacheContextElement::DONT_AUTO_CACHE);
	}	    

        if (tcb) {
	    delete tcb;
	    state->pop();
	} else if (useTexCoordsAnyway)
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

	// Call the appropriate render loops:
	if (numTris > 0)
	    (this->*TriRenderFunc[vpCache.getRenderCase(shapeStyle)])(action);
	if (numQuads > 0)
	    (this->*QuadRenderFunc[vpCache.getRenderCase(shapeStyle)])(action);
	if (numFaces > 0)
	    (this->*GenRenderFunc[vpCache.getRenderCase(shapeStyle)])(action);

	// If doing multiple colors, turn off ColorMaterial:
	if (vpCache.getNumColors() > 1) {
	    SoGLLazyElement::setColorMaterial(state, FALSE);
	    ((SoGLLazyElement *)SoLazyElement::getInstance(state))->
	    	reset(state, SoLazyElement::DIFFUSE_MASK);
	}
	// Influence auto-caching algorithm:
	if (coordIndex.getNum() > AUTO_CACHE_IFS_MAX &&
	    !SoGLCacheContextElement::getIsRemoteRendering(state)) {

	    SoGLCacheContextElement::shouldAutoCache(state,
			SoGLCacheContextElement::DONT_AUTO_CACHE);
	}	    
    }
    return;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Generates polys representing a face set.
//
// Use: protected

void
SoIndexedFaceSet::generatePrimitives(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoState *state = action->getState();
    // Put vertexProperty stuff into state:
    SoVertexProperty *vp = (SoVertexProperty
			    *)vertexProperty.getValue();
    state->push();
    if (vp) {
	vp->doAction(action);
    }
    
    // When generating primitives for picking, there is no need to
    // create details now, since they will be created in
    // createTriangleDetail() when an intersection is found (but we
    // need to use the face detail to figure out the rest of it).
    // Otherwise, we create a face detail containing the 3 points of
    // the generated triangle, using the stuff in SoShape.
    // We also delay computing default texture coordinates.
    SbBool forPicking = action->isOfType(SoRayPickAction::getClassTypeId());

    SoPrimitiveVertex		pv;
    SoFaceDetail		fd;
    SoPointDetail		pd;
    SoNormalBundle		nb(action, FALSE);
    SoTextureCoordinateBundle	tcb(action, FALSE, ! forPicking);
    const SoCoordinateElement	*ce;
    int				curVert, curIndex, curFace, curCoord;
    int				numIndices;
    const int32_t			*coordIndices, *matlIndices;
    const int32_t			*normIndices, *texCoordIndices;
    Binding			materialBinding, normalBinding;
    SbBool			texCoordsIndexed;

    ce = SoCoordinateElement::getInstance(action->getState());

    materialBinding  = getMaterialBinding(action);
    normalBinding    = getNormalBinding(action, &nb);
    texCoordsIndexed = areTexCoordsIndexed(action);

    numIndices      = coordIndex.getNum();
    coordIndices    = coordIndex.getValues(0);
    matlIndices     = materialIndex.getValues(0);
    normIndices     = normalIndex.getValues(0);
    texCoordIndices = textureCoordIndex.getValues(0);

    // Check for special case of 1 index of SO_END_FACE_INDEX. This
    // means that coord indices are to be used for materials, normals,
    // or texture coords as well
    if (materialIndex.getNum() == 1 && matlIndices[0] == SO_END_FACE_INDEX)
	matlIndices = coordIndices;
    if (normalIndex.getNum() == 1 && normIndices[0] == SO_END_FACE_INDEX) 
	normIndices = coordIndices;
    if (textureCoordIndex.getNum() == 1 &&
	texCoordIndices[0] == SO_END_FACE_INDEX)
	texCoordIndices = coordIndices;

    if (forPicking) {
	pv.setTextureCoords(SbVec4f(0.0, 0.0, 0.0, 0.0));
	pv.setDetail(&fd);
    }
    else
	pv.setDetail(&pd);

    // Also, save normal binding in instance so createTriangleDetail()
    // will know what it is without having to reconstruct a normal bundle
    savedNormalBinding = normalBinding;

    // Step through all the coordinate indices, building faces out
    // of them, until we run out of coordinate indices.
    curFace = curVert = curIndex = 0;

    while (curIndex < numIndices) {

	int vertsInFace = getNumVerts(curIndex);

	// Forget about faces with fewer than three vertices...
	if (vertsInFace < 3) {
	    curIndex += vertsInFace+1;
            curFace++;
	    continue;
	}

	if (forPicking) {
	    fd.setFaceIndex(curFace);
	    fd.setPartIndex(curFace);
	}

	beginShape(action, POLYGON, forPicking ? NULL : &fd);

	// Loop through all vertices of current face
	while (curIndex < numIndices &&
	       (curCoord = (int)coordIndices[curIndex]) != SO_END_FACE_INDEX) {

	    // Check for duplicate vertices; GLU doesn't handle them
	    // very well. Discard current index if it's the same as
	    // the previous one.
	    if (curIndex == 0 || (curCoord != coordIndices[curIndex - 1])) {

		int matlIndex, normIndex, tcIndex;

		switch (materialBinding) {
		  case OVERALL:
		    matlIndex = 0;
		    break;
		  case PER_FACE:
		    matlIndex = curFace;
		    break;
		  case PER_FACE_INDEXED:
		    matlIndex = (int) matlIndices[curFace];
		    break;
		  case PER_VERTEX:
		    matlIndex = curVert;
		    break;
		  case PER_VERTEX_INDEXED:
		    matlIndex = (int) matlIndices[curIndex];
		    break;
		}
		switch (normalBinding) {
		  case OVERALL:
		    normIndex = 0;
		    break;
		  case PER_FACE:
		    normIndex = curFace;
		    break;
		  case PER_FACE_INDEXED:
		    normIndex = (int) normIndices[curFace];
		    break;
		  case PER_VERTEX:
		    normIndex = curVert;
		    break;
		  case PER_VERTEX_INDEXED:
		    normIndex = (int) normIndices[curIndex];
		    break;
		}
		tcIndex = (texCoordsIndexed ?
			   (int) texCoordIndices[curIndex] : curVert);

		pv.setPoint(ce->get3(curCoord));
		pv.setNormal(nb.get(normIndex));
		pv.setMaterialIndex(matlIndex);

		if (! tcb.isFunction())
		    pv.setTextureCoords(tcb.get(tcIndex));

		if (! forPicking) {
		    if (tcb.isFunction())
			pv.setTextureCoords(tcb.get(pv.getPoint(),
						    pv.getNormal()));
		    pd.setCoordinateIndex(curCoord);
		    pd.setMaterialIndex(matlIndex);
		    pd.setNormalIndex(normIndex);
		    pd.setTextureCoordIndex(tcIndex);
		}

		shapeVertex(&pv);
	    }

	    curVert++;
	    curIndex++;
	}

	endShape();

	curIndex++; 	// Skip over the END_FACE_INDEX
	curFace++;
    }
    state->pop();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Overrides standard method to create an SoFaceDetail instance
//    representing a picked intersection with a triangle in the set.
//
// Use: protected, virtual

SoDetail *
SoIndexedFaceSet::createTriangleDetail(SoRayPickAction *action,
				       const SoPrimitiveVertex *v1,
				       const SoPrimitiveVertex *,
				       const SoPrimitiveVertex *,
				       SoPickedPoint *pp)
//
////////////////////////////////////////////////////////////////////////
{
    // When we get here, the detail in each vertex already points to an
    // SoFaceDetail (on the stack in generatePrimitives()). This detail
    // contains the correct face/part index for the intersected face.
    // However, this face detail does not contain any point details.
    // We have to create a new SoFaceDetail instance and set up the
    // SoPointDetail instances inside it to contain the vertices of
    // the picked face. We also need to compute texture coordinates.

    SoFaceDetail	*newFD = new SoFaceDetail;
    const SoFaceDetail	*oldFD = (const SoFaceDetail *) v1->getDetail();

    // Get pointers to all indices, just in case
    const int32_t	*coordIndices, *matlIndices;
    const int32_t	*normIndices, *texCoordIndices;

    coordIndices    = coordIndex.getValues(0);
    matlIndices     = materialIndex.getValues(0);
    normIndices     = normalIndex.getValues(0);
    texCoordIndices = textureCoordIndex.getValues(0);
    if (materialIndex.getNum() == 1 && matlIndices[0] == SO_END_FACE_INDEX)
	matlIndices = coordIndices;
    if (normalIndex.getNum() == 1 && normIndices[0] == SO_END_FACE_INDEX) 
	normIndices = coordIndices;
    if (textureCoordIndex.getNum() == 1 &&
	texCoordIndices[0] == SO_END_FACE_INDEX)
	texCoordIndices = coordIndices;

    // Find out which face was hit
    int hitFace = (int) oldFD->getFaceIndex();

    // Find index of coordinate of first vertex of face by skipping
    // over previous faces. Also, count the number of vertices used.
    int curIndex = 0, curVert = 0, vertsInFace;
    for (int face = 0; face < hitFace; face++) {
	vertsInFace = getNumVerts(curIndex);
	curVert  += vertsInFace;
	curIndex += vertsInFace + 1;	// Skip over the END_FACE_INDEX
    }

    // Get number of vertices in the hit face
    vertsInFace = getNumVerts(curIndex);

    // Make room in the detail for vertices
    newFD->setNumPoints(vertsInFace);

    // We need the bindings to set up the material/normals stuff. The
    // normal binding was saved for us in generatePrimitives() so we
    // don't have to create a normal bundle to recompute it.
    Binding	materialBinding  = getMaterialBinding(action);
    Binding	normalBinding    = savedNormalBinding;
    SbBool	texCoordsIndexed = areTexCoordsIndexed(action);

    // Store each vertex in the detail
    SoPointDetail		pd;
    SoTextureCoordinateBundle	tcb(action, FALSE);
    for (int vert = 0; vert < vertsInFace; vert++) {

	int matlIndex, normIndex, tcIndex;

	switch (materialBinding) {
	  case OVERALL:
	    matlIndex = 0;
	    break;
	  case PER_FACE:
	    matlIndex = hitFace;
	    break;
	  case PER_FACE_INDEXED:
	    matlIndex = (int) matlIndices[hitFace];
	    break;
	  case PER_VERTEX:
	    matlIndex = curVert;
	    break;
	  case PER_VERTEX_INDEXED:
	    matlIndex = (int) matlIndices[curIndex];
	    break;
	}
	switch (normalBinding) {
	  case OVERALL:
	    normIndex = 0;
	    break;
	  case PER_FACE:
	    normIndex = hitFace;
	    break;
	  case PER_FACE_INDEXED:
	    normIndex = (int) normIndices[hitFace];
	    break;
	  case PER_VERTEX:
	    normIndex = curVert;
	    break;
	  case PER_VERTEX_INDEXED:
	    normIndex = (int) normIndices[curIndex];
	    break;
	}
	tcIndex = (texCoordsIndexed ?
		   (int) texCoordIndices[curIndex] : curVert);

	pd.setCoordinateIndex(coordIndices[curIndex]);
	pd.setMaterialIndex(matlIndex);
	pd.setNormalIndex(normIndex);
	pd.setTextureCoordIndex(tcb.isFunction() ? 0 : tcIndex);

	newFD->setPoint(vert, &pd);

	curVert++;
	curIndex++;
    }

    // Compute texture coordinates at intersection point and store it
    // in the picked point
    if (tcb.isFunction())
	pp->setObjectTextureCoords(tcb.get(pp->getObjectPoint(),
					   pp->getObjectNormal()));

    // The face/part indices are in the incoming details
    newFD->setFaceIndex(hitFace);
    newFD->setPartIndex(hitFace);

    return newFD;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns current material binding from action's state.
//
// Use: private, static

SoIndexedFaceSet::Binding
SoIndexedFaceSet::getMaterialBinding(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    switch (SoMaterialBindingElement::get(action->getState())) {
      case SoMaterialBindingElement::OVERALL:
	return OVERALL;
      case SoMaterialBindingElement::PER_PART:
      case SoMaterialBindingElement::PER_FACE:
	return PER_FACE;
      case SoMaterialBindingElement::PER_PART_INDEXED:
      case SoMaterialBindingElement::PER_FACE_INDEXED:
	return PER_FACE_INDEXED;
      case SoMaterialBindingElement::PER_VERTEX:
	return PER_VERTEX;
      case SoMaterialBindingElement::PER_VERTEX_INDEXED:
	return PER_VERTEX_INDEXED;
    }
    return OVERALL;	// Shut SGI compiler up
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns current normal binding from action's state.
//
// Use: private, static

SoIndexedFaceSet::Binding
SoIndexedFaceSet::getNormalBinding(SoAction *action, SoNormalBundle *nb)
//
////////////////////////////////////////////////////////////////////////
{
    if (nb && figureNormals(action->getState(), nb))
	return PER_VERTEX;

    switch (SoNormalBindingElement::get(action->getState())) {
      case SoNormalBindingElement::OVERALL:
	return OVERALL;

      case SoNormalBindingElement::PER_PART:
      case SoNormalBindingElement::PER_FACE:
	return PER_FACE;

      case SoNormalBindingElement::PER_PART_INDEXED:
      case SoNormalBindingElement::PER_FACE_INDEXED:
	return PER_FACE_INDEXED;

      case SoNormalBindingElement::PER_VERTEX:
	return PER_VERTEX;

      case SoNormalBindingElement::PER_VERTEX_INDEXED:
	return PER_VERTEX_INDEXED;
    }
    return OVERALL;	// Shut SGI compiler up
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Figures out normals, if necessary.  Returns TRUE if normals were
//    figured out (and the normal binding should be PER_VERTEX).
//
// Use: private

SbBool
SoIndexedFaceSet::figureNormals(SoState *state, SoNormalBundle *nb)
//
////////////////////////////////////////////////////////////////////////
{
    // See if there is a valid normal cache. If so, tell the normal
    // bundle to use it.
    SoNormalCache *normCache = getNormalCache();
    if (normCache != NULL && normCache->isValid(state)) {
	nb->set(normCache->getNum(), normCache->getNormals());
	return TRUE;
    }

    int			numNeeded = 0, i, numVertices = 0;
    const SoMFInt32	*nIndices;

    if (normalIndex.getNum() == 1 && normalIndex[0] == SO_END_FACE_INDEX)
	nIndices = &coordIndex;
    else
	nIndices = &normalIndex;

    // Find greatest index:
    for (i = 0; i < nIndices->getNum(); i++) {
	if ((*nIndices)[i] > numNeeded)
	    numNeeded = (int) (*nIndices)[i];
	if ((*nIndices)[i] >= 0) // Count vertices
	    ++numVertices;
    }

    if (nb->shouldGenerate(numNeeded)) {
	generateDefaultNormals(state, nb);
	return TRUE;
    }
    return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Generates default normals using the given state and normal
//    bundle. Returns TRUE if normals were generated.
//
// Use: extender, virtual

SbBool
SoIndexedFaceSet::generateDefaultNormals(SoState *state, SoNormalBundle *nb)
//
////////////////////////////////////////////////////////////////////////
{
    int				numIndices = coordIndex.getNum(), curIndex = 0;
    const SoCoordinateElement	*ce = NULL;
    const SbVec3f 		*vpCoords = NULL;

    SoVertexProperty *vp = (SoVertexProperty *)vertexProperty.getValue();
    if (vp && vp->vertex.getNum() > 0) {
	vpCoords = vp->vertex.getValues(0);
    } else {
	ce = SoCoordinateElement::getInstance(state);
    }

    while (curIndex < numIndices) {

	// Figure out how many vertices in this face:
        int vertsInFace;
	for (vertsInFace = 0;
	     vertsInFace + curIndex < numIndices &&
	     coordIndex[vertsInFace + curIndex] != SO_END_FACE_INDEX;
	     vertsInFace++)
	    ;

	// Forget about faces with fewer than three vertices...
	if (vertsInFace < 3) {
	    curIndex += vertsInFace + 1;
	    continue;
	}

	nb->beginPolygon();

	// Loop through all vertices of current face
	while (curIndex < numIndices &&
	       coordIndex[curIndex] != SO_END_FACE_INDEX) {

	    if (ce)
		nb->polygonVertex(ce->get3((int)coordIndex[curIndex]));
	    else
		nb->polygonVertex(vpCoords[coordIndex[curIndex]]);

	    curIndex++;
	}
	nb->endPolygon();
    }
    nb->generate();

    // Cache the resulting normals
    setNormalCache(state, nb->getNumGeneratedNormals(),
		   nb->getGeneratedNormals());

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    invalidate vpCache when notified
//
// Use: private 

void
SoIndexedFaceSet::notify(SoNotList *list)
//
////////////////////////////////////////////////////////////////////////
{
    if (list->getLastRec()->getType() == SoNotRec::CONTAINER &&
	list->getLastField() == &coordIndex) {
	numTris = numQuads = numFaces = -1;
    }

    SoIndexedShape::notify(list);
}

///////////////////////////////////////////////////////////////////////////
//
// Description:
//	Find out how many tris, quads, and polygons there are:
//
// use: private
//
///////////////////////////////////////////////////////////////////////////
void 
SoIndexedFaceSet::setupNumTrisQuadsFaces()
{
    numTris = numQuads = numFaces = 0;
    int i = 0;
    const int numCoords = coordIndex.getNum();
    while ((i < numCoords - 2) && 
	((i+3 == numCoords)||(coordIndex[i+3] == SO_END_FACE_INDEX))) {
	++numTris;
	i += 4;  // Skip past three vertex indices and END_OF_FACE
		 // marker
    }
    while ((i < numCoords - 3) && 
	((i+4 == numCoords) ||(coordIndex[i+4] == SO_END_FACE_INDEX)) &&
	   (coordIndex[i+3] != SO_END_FACE_INDEX)) {

	++numQuads;
	i += 5;  // Skip past three vertex indices and END_OF_FACE
		 // marker
    }
    /* if there aren't 3 vertices, no polygons are rendered */
    if (i > numCoords - 3 ) return;
    while (i < numCoords) {
	if ((i+1 == numCoords)||(coordIndex[i] == SO_END_FACE_INDEX))
	     ++numFaces;
	++i;
    }
}    
    
//////////////////////////////////////////////////////////////////////////
// Following preprocessor-generated routines handle all combinations of
// Normal binding (per vertex, per face, per part, overall/none)
// Color Binding (per vertex, per face, per part, overall)
// Textures (on or off)
//////////////////////////////////////////////////////////////////////////

@@ ----------------------------------------------------------------------
@@ Triangle rendering:
@@
@={ TriRenderFunction
void
SoIndexedFaceSet::
@ FuncName 
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
@?{ On
    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
@?}
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
@?{ Fm | Vm
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
@?}
@?{ Fn | Vn
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

    glBegin(GL_TRIANGLES);
    int vtxCtr = 0;
    for (int tri = 0; tri < numTris; tri++) {
@?{ Fm
	(*colorFunc)(colorPtr+colorStride*colorIndx[tri]);
@?}
@?{ Fn
	(*normalFunc)(normalPtr+normalStride*normalIndx[tri]);
@?}

@?{ Vm
	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);
@?}
@?{ Vn
	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr]);
@?}
@?{ T
	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
@?}
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);

@?{ Vm
	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr+1]);
@?}
@?{ Vn
	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr+1]);
@?}
@?{ T
	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+1]);
@?}
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+1]);

@?{ Vm
	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr+2]);
@?}
@?{ Vn
	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr+2]);
@?}
@?{ T
	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+2]);
@?}
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+2]);
	vtxCtr += 4; // Skip past END_OF_FACE_INDEX
    }
    glEnd();
}

@=}

@@ ----------------------------------------------------------------------
@@ Quad rendering:
@@
@={ QuadRenderFunction
void
SoIndexedFaceSet::
@ FuncName 
    (SoGLRenderAction* ) {
    const int32_t *const vertexIndex = coordIndex.getValues(0);
@?{ On
    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
@?}
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
@?{ Fm | Vm
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
@?}
@?{ Fn | Vn
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

    glBegin(GL_QUADS);
    int vtxCtr = numTris*4;
@?{ Fn | Fm
    int faceCtr = numTris;
@?}	
    for (int quad = 0; quad < numQuads; quad++) {
@?{ Fm
	(*colorFunc)(colorPtr+colorStride*colorIndx[faceCtr]);
@?}
@?{ Fn
	(*normalFunc)(normalPtr+normalStride*normalIndx[faceCtr]);
@?}
@?{ Fn | Fm
	++faceCtr;
@?}	

@?{ Vm
	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);
@?}
@?{ Vn
	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr]);
@?}
@?{ T
	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
@?}
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);

@?{ Vm
	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr+1]);
@?}
@?{ Vn
	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr+1]);
@?}
@?{ T
	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+1]);
@?}
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+1]);

@?{ Vm
	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr+2]);
@?}
@?{ Vn
	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr+2]);
@?}
@?{ T
	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+2]);
@?}
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+2]);

@?{ Vm
	(*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr+3]);
@?}
@?{ Vn
	(*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr+3]);
@?}
@?{ T
	(*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr+3]);
@?}
	(*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr+3]);
	vtxCtr += 5; // Skip past END_OF_FACE_INDEX
    }
    glEnd();
}

@=}

@@ ----------------------------------------------------------------------
@@ General polygon rendering:
@@
@={ GenRenderFunction
void
SoIndexedFaceSet::
@ FuncName 
    (SoGLRenderAction* )
{
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const int numVI = coordIndex.getNum();
@?{ On
    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
@?}
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
@?{ Fm | Vm
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
@?}
@?{ Fn | Vn
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
    int vtxCtr = numQuads*5 + numTris*4;
@?{ Fn | Fm
    int faceCtr = numQuads + numTris;
@?}
    while (vtxCtr < numVI) {
@?{ Fm
	(*colorFunc)(colorPtr+colorStride*colorIndx[faceCtr]);
@?}
@?{ Fn
	(*normalFunc)(normalPtr+normalStride*normalIndx[faceCtr]);
@?}
@?{ Fn | Fm
	++faceCtr;
@?}	
	glBegin(GL_POLYGON);
	while (vtxCtr < numVI &&
	       (vertexIndex[vtxCtr] != SO_END_FACE_INDEX)) {
@?{ Vm
	    (*colorFunc)(colorPtr+colorStride*colorIndx[vtxCtr]);
@?}
@?{ Vn
	    (*normalFunc)(normalPtr+normalStride*normalIndx[vtxCtr]);
@?}
@?{ T
	    (*texCoordFunc)(texCoordPtr+texCoordStride*tCoordIndx[vtxCtr]);
@?}
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr]);
	    vtxCtr++;
	}
	vtxCtr++; // Skip over END_FACE_INDEX
	glEnd();
    }
}

@=}

// 32 different rendering loops; the 5 bits used to determine the
// rendering case are:
// 43210  BITS            Routine suffix
// -----  ----            --------------
// 00...  Overall mtl     (Om)
// 01...  Part mtl        (Pm)  NOT GENERATED, Fm is same!
// 10...  Face mtl        (Fm)
// 11...  Vtx mtl         (Vm)
// ..00.  Overall/No norm (On)
// ..01.  Part norm       (Pn)  NOT GENERATED, Fn is same!
// ..10.  Face norm       (Fn)
// ..11.  Vtx norm        (Vn)
// ....0  No texcoord     -none-
// ....1  Vtx texcoord    (T)
//
SoIndexedFaceSet::PMFS SoIndexedFaceSet::TriRenderFunc[32] = {
    &SoIndexedFaceSet::TriOmOn, &SoIndexedFaceSet::TriOmOnT,
    &SoIndexedFaceSet::TriOmFn, &SoIndexedFaceSet::TriOmFnT,
    &SoIndexedFaceSet::TriOmFn, &SoIndexedFaceSet::TriOmFnT,
    &SoIndexedFaceSet::TriOmVn, &SoIndexedFaceSet::TriOmVnT,
    &SoIndexedFaceSet::TriFmOn, &SoIndexedFaceSet::TriFmOnT,
    &SoIndexedFaceSet::TriFmFn, &SoIndexedFaceSet::TriFmFnT,
    &SoIndexedFaceSet::TriFmFn, &SoIndexedFaceSet::TriFmFnT,
    &SoIndexedFaceSet::TriFmVn, &SoIndexedFaceSet::TriFmVnT,
    &SoIndexedFaceSet::TriFmOn, &SoIndexedFaceSet::TriFmOnT,
    &SoIndexedFaceSet::TriFmFn, &SoIndexedFaceSet::TriFmFnT,
    &SoIndexedFaceSet::TriFmFn, &SoIndexedFaceSet::TriFmFnT,
    &SoIndexedFaceSet::TriFmVn, &SoIndexedFaceSet::TriFmVnT,
    &SoIndexedFaceSet::TriVmOn, &SoIndexedFaceSet::TriVmOnT,
    &SoIndexedFaceSet::TriVmFn, &SoIndexedFaceSet::TriVmFnT,
    &SoIndexedFaceSet::TriVmFn, &SoIndexedFaceSet::TriVmFnT,
    &SoIndexedFaceSet::TriVmVn, &SoIndexedFaceSet::TriVmVnT,
    };
SoIndexedFaceSet::PMFS SoIndexedFaceSet::QuadRenderFunc[32] = {
    &SoIndexedFaceSet::QuadOmOn, &SoIndexedFaceSet::QuadOmOnT,
    &SoIndexedFaceSet::QuadOmFn, &SoIndexedFaceSet::QuadOmFnT,
    &SoIndexedFaceSet::QuadOmFn, &SoIndexedFaceSet::QuadOmFnT,
    &SoIndexedFaceSet::QuadOmVn, &SoIndexedFaceSet::QuadOmVnT,
    &SoIndexedFaceSet::QuadFmOn, &SoIndexedFaceSet::QuadFmOnT,
    &SoIndexedFaceSet::QuadFmFn, &SoIndexedFaceSet::QuadFmFnT,
    &SoIndexedFaceSet::QuadFmFn, &SoIndexedFaceSet::QuadFmFnT,
    &SoIndexedFaceSet::QuadFmVn, &SoIndexedFaceSet::QuadFmVnT,
    &SoIndexedFaceSet::QuadFmOn, &SoIndexedFaceSet::QuadFmOnT,
    &SoIndexedFaceSet::QuadFmFn, &SoIndexedFaceSet::QuadFmFnT,
    &SoIndexedFaceSet::QuadFmFn, &SoIndexedFaceSet::QuadFmFnT,
    &SoIndexedFaceSet::QuadFmVn, &SoIndexedFaceSet::QuadFmVnT,
    &SoIndexedFaceSet::QuadVmOn, &SoIndexedFaceSet::QuadVmOnT,
    &SoIndexedFaceSet::QuadVmFn, &SoIndexedFaceSet::QuadVmFnT,
    &SoIndexedFaceSet::QuadVmFn, &SoIndexedFaceSet::QuadVmFnT,
    &SoIndexedFaceSet::QuadVmVn, &SoIndexedFaceSet::QuadVmVnT,
    };
SoIndexedFaceSet::PMFS SoIndexedFaceSet::GenRenderFunc[32] = {
    &SoIndexedFaceSet::GenOmOn, &SoIndexedFaceSet::GenOmOnT,
    &SoIndexedFaceSet::GenOmFn, &SoIndexedFaceSet::GenOmFnT,
    &SoIndexedFaceSet::GenOmFn, &SoIndexedFaceSet::GenOmFnT,
    &SoIndexedFaceSet::GenOmVn, &SoIndexedFaceSet::GenOmVnT,
    &SoIndexedFaceSet::GenFmOn, &SoIndexedFaceSet::GenFmOnT,
    &SoIndexedFaceSet::GenFmFn, &SoIndexedFaceSet::GenFmFnT,
    &SoIndexedFaceSet::GenFmFn, &SoIndexedFaceSet::GenFmFnT,
    &SoIndexedFaceSet::GenFmVn, &SoIndexedFaceSet::GenFmVnT,
    &SoIndexedFaceSet::GenFmOn, &SoIndexedFaceSet::GenFmOnT,
    &SoIndexedFaceSet::GenFmFn, &SoIndexedFaceSet::GenFmFnT,
    &SoIndexedFaceSet::GenFmFn, &SoIndexedFaceSet::GenFmFnT,
    &SoIndexedFaceSet::GenFmVn, &SoIndexedFaceSet::GenFmVnT,
    &SoIndexedFaceSet::GenVmOn, &SoIndexedFaceSet::GenVmOnT,
    &SoIndexedFaceSet::GenVmFn, &SoIndexedFaceSet::GenVmFnT,
    &SoIndexedFaceSet::GenVmFn, &SoIndexedFaceSet::GenVmFnT,
    &SoIndexedFaceSet::GenVmVn, &SoIndexedFaceSet::GenVmVnT,
    };

@= Om ON
@= On ON
@= Fm
@= Vm
@= Fn
@= Vn
@= T
@= FuncName TriOmOn
@ TriRenderFunction
@= FuncName QuadOmOn
@ QuadRenderFunction
@= FuncName GenOmOn
@ GenRenderFunction

@= Om ON
@= On ON
@= Fm
@= Vm
@= Fn
@= Vn
@= T ON
@= FuncName TriOmOnT
@ TriRenderFunction
@= FuncName QuadOmOnT
@ QuadRenderFunction
@= FuncName GenOmOnT
@ GenRenderFunction

@= Om ON
@= On
@= Fm
@= Vm
@= Fn ON
@= Vn
@= T
@= FuncName TriOmFn
@ TriRenderFunction
@= FuncName QuadOmFn
@ QuadRenderFunction
@= FuncName GenOmFn
@ GenRenderFunction

@= Om ON
@= On
@= Fm
@= Vm
@= Fn ON
@= Vn
@= T ON
@= FuncName TriOmFnT
@ TriRenderFunction
@= FuncName QuadOmFnT
@ QuadRenderFunction
@= FuncName GenOmFnT
@ GenRenderFunction

@= Om ON
@= On
@= Fm
@= Vm
@= Fn
@= Vn ON
@= T
@= FuncName TriOmVn
@ TriRenderFunction
@= FuncName QuadOmVn
@ QuadRenderFunction
@= FuncName GenOmVn
@ GenRenderFunction

@= Om ON
@= On
@= Fm
@= Vm
@= Fn
@= Vn ON
@= T ON
@= FuncName TriOmVnT
@ TriRenderFunction
@= FuncName QuadOmVnT
@ QuadRenderFunction
@= FuncName GenOmVnT
@ GenRenderFunction


@= Om
@= On ON
@= Fm ON
@= Vm
@= Fn
@= Vn
@= T
@= FuncName TriFmOn
@ TriRenderFunction
@= FuncName QuadFmOn
@ QuadRenderFunction
@= FuncName GenFmOn
@ GenRenderFunction

@= Om
@= On ON
@= Fm ON
@= Vm
@= Fn
@= Vn
@= T ON
@= FuncName TriFmOnT
@ TriRenderFunction
@= FuncName QuadFmOnT
@ QuadRenderFunction
@= FuncName GenFmOnT
@ GenRenderFunction

@= Om
@= On
@= Fm ON
@= Vm
@= Fn ON
@= Vn
@= T
@= FuncName TriFmFn
@ TriRenderFunction
@= FuncName QuadFmFn
@ QuadRenderFunction
@= FuncName GenFmFn
@ GenRenderFunction

@= Om
@= On
@= Fm ON
@= Vm
@= Fn ON
@= Vn
@= T ON
@= FuncName TriFmFnT
@ TriRenderFunction
@= FuncName QuadFmFnT
@ QuadRenderFunction
@= FuncName GenFmFnT
@ GenRenderFunction

@= Om
@= On
@= Fm ON
@= Vm
@= Fn
@= Vn ON
@= T
@= FuncName TriFmVn
@ TriRenderFunction
@= FuncName QuadFmVn
@ QuadRenderFunction
@= FuncName GenFmVn
@ GenRenderFunction

@= Om
@= On
@= Fm ON
@= Vm
@= Fn
@= Vn ON
@= T ON
@= FuncName TriFmVnT
@ TriRenderFunction
@= FuncName QuadFmVnT
@ QuadRenderFunction
@= FuncName GenFmVnT
@ GenRenderFunction


@= Om
@= On ON
@= Fm
@= Vm ON
@= Fn
@= Vn
@= T
@= FuncName TriVmOn
@ TriRenderFunction
@= FuncName QuadVmOn
@ QuadRenderFunction
@= FuncName GenVmOn
@ GenRenderFunction

@= Om
@= On ON
@= Fm
@= Vm ON
@= Fn
@= Vn
@= T ON
@= FuncName TriVmOnT
@ TriRenderFunction
@= FuncName QuadVmOnT
@ QuadRenderFunction
@= FuncName GenVmOnT
@ GenRenderFunction

@= Om
@= On
@= Fm
@= Vm ON
@= Fn ON
@= Vn
@= T
@= FuncName TriVmFn
@ TriRenderFunction
@= FuncName QuadVmFn
@ QuadRenderFunction
@= FuncName GenVmFn
@ GenRenderFunction

@= Om
@= On
@= Fm
@= Vm ON
@= Fn ON
@= Vn
@= T ON
@= FuncName TriVmFnT
@ TriRenderFunction
@= FuncName QuadVmFnT
@ QuadRenderFunction
@= FuncName GenVmFnT
@ GenRenderFunction

@= Om
@= On
@= Fm
@= Vm ON
@= Fn
@= Vn ON
@= T
@= FuncName TriVmVn
@ TriRenderFunction
@= FuncName QuadVmVn
@ QuadRenderFunction
@= FuncName GenVmVn
@ GenRenderFunction

@= Om
@= On
@= Fm
@= Vm ON
@= Fn
@= Vn ON
@= T ON
@= FuncName TriVmVnT
@ TriRenderFunction
@= FuncName QuadVmVnT
@ QuadRenderFunction
@= FuncName GenVmVnT
@ GenRenderFunction

