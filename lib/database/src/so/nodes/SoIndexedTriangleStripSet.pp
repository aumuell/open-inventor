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
 |      SoIndexedTriangleStripSet
 |
@@ This next line is true for the output file only - ignore it here:
 |   Note: This file was preprocessed from another file. Do not edit it.
 |
 |   Author(s)          : Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <GL/gl.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/SoPrimitiveVertex.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/bundles/SoNormalBundle.h>
#include <Inventor/bundles/SoTextureCoordinateBundle.h>
#include <Inventor/caches/SoNormalCache.h>
#include <Inventor/details/SoFaceDetail.h>
#include <Inventor/elements/SoGLCacheContextElement.h>
#include <Inventor/elements/SoGLCoordinateElement.h>
#include <Inventor/elements/SoGLTextureCoordinateElement.h>
#include <Inventor/elements/SoNormalBindingElement.h>
#include <Inventor/elements/SoTextureCoordinateBindingElement.h>
#include <Inventor/elements/SoShapeHintsElement.h>
#include <Inventor/elements/SoGLLazyElement.h>
#include <Inventor/elements/SoDrawStyleElement.h>
#include <Inventor/elements/SoCreaseAngleElement.h>
#include <Inventor/nodes/SoIndexedTriangleStripSet.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoNormalBinding.h>
#include <Inventor/nodes/SoVertexProperty.h>

SO_NODE_SOURCE(SoIndexedTriangleStripSet);

// Constants for influencing auto-caching algorithm:
const int AUTO_CACHE_ITSS_MIN_WITHOUT_VP = 20;

// And the number above which we'll say caches definitely SHOULDN'T be
// built (because they'll use too much memory):
const int AUTO_CACHE_ITSS_MAX = 1000;

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
SoIndexedTriangleStripSet::PMTSS SoIndexedTriangleStripSet::renderFunc[32] = {
    &SoIndexedTriangleStripSet::OmOn, &SoIndexedTriangleStripSet::OmOnT,
    &SoIndexedTriangleStripSet::OmPn, &SoIndexedTriangleStripSet::OmPnT,
    &SoIndexedTriangleStripSet::OmFn, &SoIndexedTriangleStripSet::OmFnT,
    &SoIndexedTriangleStripSet::OmVn, &SoIndexedTriangleStripSet::OmVnT,
    &SoIndexedTriangleStripSet::PmOn, &SoIndexedTriangleStripSet::PmOnT,
    &SoIndexedTriangleStripSet::PmPn, &SoIndexedTriangleStripSet::PmPnT,
    &SoIndexedTriangleStripSet::PmFn, &SoIndexedTriangleStripSet::PmFnT,
    &SoIndexedTriangleStripSet::PmVn, &SoIndexedTriangleStripSet::PmVnT,
    &SoIndexedTriangleStripSet::FmOn, &SoIndexedTriangleStripSet::FmOnT,
    &SoIndexedTriangleStripSet::FmPn, &SoIndexedTriangleStripSet::FmPnT,
    &SoIndexedTriangleStripSet::FmFn, &SoIndexedTriangleStripSet::FmFnT,
    &SoIndexedTriangleStripSet::FmVn, &SoIndexedTriangleStripSet::FmVnT,
    &SoIndexedTriangleStripSet::VmOn, &SoIndexedTriangleStripSet::VmOnT,
    &SoIndexedTriangleStripSet::VmPn, &SoIndexedTriangleStripSet::VmPnT,
    &SoIndexedTriangleStripSet::VmFn, &SoIndexedTriangleStripSet::VmFnT,
    &SoIndexedTriangleStripSet::VmVn, &SoIndexedTriangleStripSet::VmVnT,
    };

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoIndexedTriangleStripSet::SoIndexedTriangleStripSet()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoIndexedTriangleStripSet);

    numStrips = numTriangles = -1;
    numVertices = NULL;

    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoIndexedTriangleStripSet::~SoIndexedTriangleStripSet()
//
////////////////////////////////////////////////////////////////////////
{
    if (numVertices) delete[] numVertices;
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Generates polys representing a triangleStrip set.
//
// Use: protected

void
SoIndexedTriangleStripSet::generatePrimitives(SoAction *action)
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
    
    // When generating primitives for picking, delay computing default
    // texture coordinates
    SbBool forPicking = action->isOfType(SoRayPickAction::getClassTypeId());

    SoPrimitiveVertex		pvs[3];
    SoFaceDetail		detail;
    SoNormalBundle		nb(action, FALSE);
    SoTextureCoordinateBundle	tcb(action, FALSE, ! forPicking);
    const SoCoordinateElement	*ce;
    int				curIndex, curStrip = 0;
    int				curTri = 0;
    int				curMaterial = -1, curNormal = -1;
    int				curTexCoord = -1;
    int				numIndices;
    const int32_t			*coordIndices, *matlIndices;
    const int32_t			*normIndices, *texCoordIndices;
    Binding			materialBinding, normalBinding;
    SbBool			texCoordsIndexed;

    ce = SoCoordinateElement::getInstance(action->getState());

    materialBinding  = getMaterialBinding(action);
    texCoordsIndexed = areTexCoordsIndexed(action);
    normalBinding    = getNormalBinding(action, nb);

    curIndex = 0;

    numIndices      = coordIndex.getNum();
    coordIndices    = coordIndex.getValues(0);
    matlIndices     = materialIndex.getValues(0);
    normIndices     = normalIndex.getValues(0);
    texCoordIndices = textureCoordIndex.getValues(0);

    // Check for special case of 1 index of SO_END_TRIANGLESTRIP_INDEX. This
    // means that coord indices are to be used for materials, normals,
    // or texture coords as well
    if (materialIndex.getNum() == 1 && matlIndices[0] == SO_END_STRIP_INDEX)
	matlIndices = coordIndices;
    if (normalIndex.getNum() == 1 && normIndices[0] == SO_END_STRIP_INDEX) 
	normIndices = coordIndices;
    if (textureCoordIndex.getNum() == 1 &&
	texCoordIndices[0] == SO_END_STRIP_INDEX)
	texCoordIndices = coordIndices;

    if (forPicking) {
	SbVec4f	tc(0.0, 0.0, 0.0, 0.0);
	pvs[0].setTextureCoords(tc);
	pvs[1].setTextureCoords(tc);
	pvs[2].setTextureCoords(tc);
    }

    detail.setNumPoints(3);
    SoPointDetail *pd = detail.getPoints();

    pvs[0].setDetail(&detail);
    pvs[1].setDetail(&detail);
    pvs[2].setDetail(&detail);

    // Step through all the coordinate indices, building triangleStrips out
    // of them, until we run out of coordinate indices.
    while (curIndex < numIndices) {

	// Figure out how many vertices are in this strip:
        int vertsInStrip;
	for (vertsInStrip = 0; vertsInStrip+curIndex <
	     numIndices && coordIndex[vertsInStrip+curIndex] !=
	     SO_END_STRIP_INDEX; vertsInStrip++);

        // Check to see whether to skip this strip due to 
	//  too few polygons:
        if (vertsInStrip < 3 ) {
          
	    curIndex += vertsInStrip+1;
            curStrip++;
            if (materialBinding == PER_VERTEX)
                curMaterial += vertsInStrip;
            if (normalBinding == PER_VERTEX)
                curNormal += vertsInStrip;
	    if (vertsInStrip > 2)
		curTri += vertsInStrip-2;
            continue;
        }

	detail.setPartIndex(curStrip);

	// Loop through all vertices of current triangleStrip
	for (int j = 0; j < vertsInStrip; j++) {

	    switch (materialBinding) {
	      case OVERALL:
		curMaterial = 0;
		break;
	      case PER_STRIP:
		curMaterial = curStrip;
		break;
	      case PER_STRIP_INDEXED:
		curMaterial = (int) matlIndices[curStrip];
		break;
	      case PER_TRIANGLE:
		curMaterial = (int) curTri;
		break;
	      case PER_TRIANGLE_INDEXED:
		curMaterial = (int) matlIndices[curTri];
		break;
	      case PER_VERTEX:
		curMaterial++;
		break;
	      case PER_VERTEX_INDEXED:
		curMaterial = (int) matlIndices[curIndex];
		break;
	    }

	    switch (normalBinding) {
	      case OVERALL:
		curNormal = 0;
		break;
	      case PER_STRIP:
		curNormal = curStrip;
		break;
	      case PER_STRIP_INDEXED:
		curNormal = (int) normIndices[curStrip];
		break;
	      case PER_TRIANGLE:
		curNormal = curTri;
		break;
	      case PER_TRIANGLE_INDEXED:
		curNormal = (int) normIndices[curTri];
		break;
	      case PER_VERTEX:
		curNormal++;
		break;
	      case PER_VERTEX_INDEXED:
		curNormal = (int) normIndices[curIndex];
		break;
	    }

	    curTexCoord = (texCoordsIndexed ?
			   (int) texCoordIndices[curIndex] : curIndex);

	    int thisVert = j%3;

	    detail.setFaceIndex(curTri);

	    // Set up a point detail for the current vertex
	    pd[thisVert].setCoordinateIndex((int) coordIndices[curIndex]);
	    pd[thisVert].setMaterialIndex(curMaterial);
	    pd[thisVert].setNormalIndex(curNormal);
	    pd[thisVert].setTextureCoordIndex(curTexCoord);

	    detail.setPoint(thisVert, &pd[thisVert]);

	    pvs[thisVert].setPoint(ce->get3((int) coordIndices[curIndex]));
	    pvs[thisVert].setNormal(nb.get(curNormal));
	    pvs[thisVert].setMaterialIndex(curMaterial);
	    if (tcb.isFunction()) {
		if (! forPicking)
		    pvs[thisVert].setTextureCoords(
					tcb.get(pvs[thisVert].getPoint(),
						pvs[thisVert].getNormal()));
	    }
	    else
		pvs[thisVert].setTextureCoords(tcb.get(curTexCoord));

	    if (j >= 2) {
		// Must handle per-triangle normals or materials
		// specially:
		if (materialBinding == PER_TRIANGLE ||
		    materialBinding == PER_TRIANGLE_INDEXED) {
		    int v = (j-1)%3;
		    pd[v].setMaterialIndex(curMaterial);
		    pvs[v].setMaterialIndex(curMaterial);
		    v = (j-2)%3;
		    pd[v].setMaterialIndex(curMaterial);
		    pvs[v].setMaterialIndex(curMaterial);
		}
		if (normalBinding == PER_TRIANGLE ||
		    normalBinding == PER_TRIANGLE_INDEXED) {
		    int v = (j-1)%3;
		    pd[v].setNormalIndex(curNormal);
		    pvs[v].setNormal(nb.get(curNormal));
		    v = (j-2)%3;
		    pd[v].setNormalIndex(curNormal);
		    pvs[v].setNormal(nb.get(curNormal));
		}

		// Do three vertices of the triangle, being careful to
		// keep them oriented correctly (the orientation switches
		// on every other triangle):
		if (j & 1) {
		    invokeTriangleCallbacks(action, &pvs[0], &pvs[2],
					    &pvs[1]);
		} else {
		    invokeTriangleCallbacks(action, &pvs[0], &pvs[1],
					    &pvs[2]);
		}
		curTri++;
	    }

	    //
	    // Increment per-vertex stuff
	    //
	    curIndex++;
	}

	//
	// Increment per-strip stuff
	//
	curIndex++; 	// Skip over the END_STRIP_INDEX
	curStrip++;
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
SoIndexedTriangleStripSet::createTriangleDetail(SoRayPickAction *action,
						const SoPrimitiveVertex *v1,
						const SoPrimitiveVertex *,
						const SoPrimitiveVertex *,
						SoPickedPoint *pp)
//
////////////////////////////////////////////////////////////////////////
{
    SoFaceDetail	*detail = new SoFaceDetail;
    const SoFaceDetail	*d;

    detail->setNumPoints(3);

    d = (const SoFaceDetail *) v1->getDetail();

    // Copy the three point details
    detail->setPoint(0, d->getPoint(0));
    detail->setPoint(1, d->getPoint(1));
    detail->setPoint(2, d->getPoint(2));

    // The face/part indices are in the incoming details
    detail->setFaceIndex(d->getFaceIndex());
    detail->setPartIndex(d->getPartIndex());

    // Compute texture coordinates at intersection point and store it
    // in the picked point
    SoTextureCoordinateBundle	tcb(action, FALSE);
    if (tcb.isFunction())
	pp->setObjectTextureCoords(tcb.get(pp->getObjectPoint(),
					   pp->getObjectNormal()));

    return detail;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns current material binding from action's state.
//
// Use: private, static

SoIndexedTriangleStripSet::Binding
SoIndexedTriangleStripSet::getMaterialBinding(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    switch (SoMaterialBindingElement::get(action->getState())) {
      case SoMaterialBindingElement::OVERALL:
	return OVERALL;
      case SoMaterialBindingElement::PER_PART:
	return PER_STRIP;
      case SoMaterialBindingElement::PER_PART_INDEXED:
	return PER_STRIP_INDEXED;
      case SoMaterialBindingElement::PER_FACE:
	return PER_TRIANGLE;
      case SoMaterialBindingElement::PER_FACE_INDEXED:
	return PER_TRIANGLE_INDEXED;
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

SoIndexedTriangleStripSet::Binding
SoIndexedTriangleStripSet::getNormalBinding(SoAction *action,
				   SoNormalBundle &nb)
//
////////////////////////////////////////////////////////////////////////
{
    if (figureNormals(action->getState(), &nb))
	return PER_VERTEX;

    switch (SoNormalBindingElement::get(action->getState())) {
      case SoNormalBindingElement::OVERALL:
	return OVERALL;

      case SoNormalBindingElement::PER_PART:
	return PER_STRIP;
      case SoNormalBindingElement::PER_PART_INDEXED:
	return PER_STRIP_INDEXED;
      case SoNormalBindingElement::PER_FACE:
	return PER_TRIANGLE;
      case SoNormalBindingElement::PER_FACE_INDEXED:
	return PER_TRIANGLE_INDEXED;
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
//    Figures out normals, if necessary.  Returns TRUE if normals were
//    figured out (and the normal binding should be PER_VERTEX).
//
// Use: private

SbBool
SoIndexedTriangleStripSet::figureNormals(SoState *state, SoNormalBundle *nb)
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

    int			numNeeded = 0, i, numTris = 0, numV = 0;
    const SoMFInt32	*nIndices;

    if (normalIndex.getNum() == 1 && normalIndex[0] == SO_END_STRIP_INDEX)
	nIndices = &coordIndex;
    else
	nIndices = &normalIndex;

    // Find greatest index:
    for (i = 0; i < nIndices->getNum(); i++) {
	if ((*nIndices)[i] > numNeeded)
	    numNeeded = (int) (*nIndices)[i];

	// Count number of triangles, too.  numV counts how many
	// vertices we've got since the beginning of the strip:
	if ((*nIndices)[i] >= 0) {
	    ++numV;
	    if (numV >= 3) ++numTris;
	}
	else numV = 0;
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
SoIndexedTriangleStripSet::generateDefaultNormals(SoState *state,
						  SoNormalBundle *nb)
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

	// Reset vars for first strip:
	SbVec3f verts[3];
	int whichVert = 0;
	int numInStrip = 0;

	// Loop through all vertices of current triangleStrip
	while (curIndex < numIndices &&
	       coordIndex[curIndex] != SO_END_STRIP_INDEX) {

	    if (ce)
		verts[whichVert%3] = ce->get3((int)coordIndex[curIndex]);
	    else
		verts[whichVert%3] = vpCoords[coordIndex[curIndex]];
		
	    ++numInStrip;

	    if (numInStrip >= 3) {

		// Spit out a triangle: NOTE: the code below
		// assumes that the third vertex in each triangle
		// sent to the normal generator is the newest
		// vertex.
		nb->beginPolygon();
		if ((numInStrip % 2) != 0) {
		    nb->polygonVertex(verts[(whichVert-2)%3]);
		    nb->polygonVertex(verts[(whichVert-1)%3]);
		    nb->polygonVertex(verts[whichVert%3]);
		}
		else {
		    nb->polygonVertex(verts[(whichVert-1)%3]);
		    nb->polygonVertex(verts[(whichVert-2)%3]);
		    nb->polygonVertex(verts[whichVert%3]);
		}			
		nb->endPolygon();
	    }			
	    ++whichVert;
	    curIndex++;
	}
	curIndex++;		// Skip past END_STRIP_INDEX
    }
    nb->generate();


    // Ok, we now have more normals than we need because we sent
    // most vertices three times.  So, we'll go through and
    // rearrange things to correspond to IndexedTriStrip's idea of
    // per-vertex normals:
    curIndex = 0;
    int triIndex = 0, curVert = 0;
    while (curIndex < numIndices) {

	SbVec3f n;

	// Figure out how many vertices are in this strip:
        int vertsInStrip;
	for (vertsInStrip = 0;
	     vertsInStrip + curIndex < numIndices &&
	     coordIndex[vertsInStrip + curIndex] != SO_END_STRIP_INDEX;
	     vertsInStrip++)
	    ;

	if (vertsInStrip > 2) {
	    // The first three vertices are correct, but then we
	    // only need to take one vertex per triangle after the
	    // first three:
            int j;
	    for (j = 0; j < 3; j++) {
		n = nb->generator->getNormal(triIndex*3+j);
		nb->generator->setNormal(curVert++, n);
	    }
	    triIndex++;
	    for (j = 3; j < vertsInStrip; j++, triIndex++) {
		n = nb->generator->getNormal(triIndex*3+2);
		nb->generator->setNormal(curVert++, n);
	    }
	}
	curIndex += vertsInStrip + 1; // Go to next strip
    }

    // Cache the resulting normals
    setNormalCache(state, nb->getNumGeneratedNormals(),
		   nb->getGeneratedNormals());

    return TRUE;
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements GL rendering.
//
// Use: extender

void
SoIndexedTriangleStripSet::GLRender(SoGLRenderAction *action)
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
	SoVertexProperty *vp = (SoVertexProperty *)vertexProperty.getValue();
        vpCache.fillInCache(vp, state);
	
	// Setup numVertices, numStrips and numTriangles:
	if (numStrips < 0)
	    countStripsAndTris();

        if (vpCache.shouldGenerateNormals(shapeStyle)) {

	    // See if there is a normal cache we can use. If not,
	    // generate normals and cache them.
	    SoNormalCache *normCache = getNormalCache();
	    if (normCache == NULL || ! normCache->isValid(state)) {

		int numVerts = 0;
		for (int i = 0; i < numStrips; i++)
		    numVerts += numVertices[i];

		SoNormalBundle nb(action, FALSE);
		nb.initGenerator(numVerts);
		generateDefaultNormals(state, &nb);
		normCache = getNormalCache();
	    }
	    vpCache.numNorms = normCache->getNum();
	    vpCache.normalPtr = (const char *) normCache->getNormals();
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
	setupIndices(numStrips, numTriangles, shapeStyle->needNormals(), 
		(shapeStyle->needTexCoords() || useTexCoordsAnyway));

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
	if (coordIndex.getNum() < AUTO_CACHE_ITSS_MIN_WITHOUT_VP &&
	    vpCache.mightNeedSomethingFromState(shapeStyle)) {
	    SoGLCacheContextElement::shouldAutoCache(state,
		SoGLCacheContextElement::DO_AUTO_CACHE);
	} else if (coordIndex.getNum() > AUTO_CACHE_ITSS_MAX &&
		   !SoGLCacheContextElement::getIsRemoteRendering(state)) {
	    SoGLCacheContextElement::shouldAutoCache(state,
			SoGLCacheContextElement::DONT_AUTO_CACHE);
	}	    

        if (tcb) {
	    delete tcb;
	    state->pop();
	}
	else if (useTexCoordsAnyway) 
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
	if (coordIndex.getNum() > AUTO_CACHE_ITSS_MAX &&
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
//	Count vertices in each strip, construct numVertices array.
//	must be invoked whenever is built, before
//	normal generation.
//
// use: private
//
///////////////////////////////////////////////////////////////////////////
void 
SoIndexedTriangleStripSet::countStripsAndTris()
{
    if (numStrips > 0) return; // Already counted
    numStrips = 0;
    int i, numVerts = 0;
    for(i = 0; i < coordIndex.getNum(); i++){
	if (coordIndex[i] == SO_END_STRIP_INDEX || 
	    (i == coordIndex.getNum()-1)) {
	    ++numStrips;
	} 
	if (coordIndex[i] != SO_END_STRIP_INDEX) {
	    ++numVerts;
	}
    }
    numTriangles = numVerts - 2*numStrips;

    numVertices = new int[numStrips];
    // Then fill in its values:
    int ns = 0;
    int nv = 0;
    for(i = 0; i< coordIndex.getNum(); i++){
	if (coordIndex[i] == SO_END_STRIP_INDEX ){
	    numVertices[ns] = nv;
	    nv=0;
	    ns++;	        
	}
	else {
	    nv++;
	    if (i == coordIndex.getNum()-1){
	        numVertices[ns] = nv;
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
SoIndexedTriangleStripSet::notify(SoNotList *list)
//
////////////////////////////////////////////////////////////////////////
{
    // If coordIndex changes, must recount:
    if (list->getLastRec()->getType() == SoNotRec::CONTAINER &&
	list->getLastField() == &coordIndex) {
	if (numVertices)
	    delete[] numVertices;
	numVertices = NULL;
	numStrips = numTriangles = -1;
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
SoIndexedTriangleStripSet::
@ FuncName 
    (SoGLRenderAction* ) {
    const int ns = numStrips;
    const int* numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
@?{ On
    // Send one normal, if there are any normals in vpCache:
    if (vpCache.getNumNormals() > 0)
	vpCache.sendNormal(vpCache.getNormals(0));
@?}
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
@?{ Pm | Fm
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
@?}
@?{ Fm
    int clrCtr=0;
@?}
@?{ Vm
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const int32_t *const colorIndx = getColorIndices();
@?}
@?{ Pn | Fn
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int32_t *const normalIndx = getNormalIndices();
@?}
@?{ Fn
    int nrmCtr=0;
@?}
@?{ Vn
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
@?{ Fm | Fn
    glShadeModel(GL_FLAT);
@?} 
    int v;
    int vtxCtr = 0;
    for (int strip = 0; strip < ns; strip++) {
@?{ Pm
	(*colorFunc)(colorPtr+colorStride*colorIndx[strip]);
@?}
@?{ Pn
	(*normalFunc)(normalPtr+normalStride*normalIndx[strip]);
@?}
	const int nv = (*numverts);
	glBegin(GL_TRIANGLE_STRIP);
	for (v = 0; v < nv-1; v+=2) {
@?{ Fm | Fn
	    // Per-face cases:
	    if (v) {
@?{ Fm
		(*colorFunc)(colorPtr+colorStride*colorIndx[clrCtr++]); 		
@?}
@?{ Fn
		(*normalFunc)(normalPtr+normalStride*normalIndx[nrmCtr++]); 		
@?}
	    }
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
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);
	    
@?{ Fm | Fn
	    // Per-face cases:
	    if (v) {
@?{ Fm
		(*colorFunc)(colorPtr+colorStride*colorIndx[clrCtr++]); 		 
@?}
@?{ Fn	       
		(*normalFunc)(normalPtr+normalStride*normalIndx[nrmCtr++]); 		
@?}
	    }
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
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);	   
	}
	if (v < nv) { // Leftovers
@?{ Fm
	    (*colorFunc)(colorPtr+colorStride*colorIndx[clrCtr++]);	
@?}
@?{ Fn
	    (*normalFunc)(normalPtr+normalStride*normalIndx[nrmCtr++]);	   
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
	    (*vertexFunc)(vertexPtr+vertexStride*vertexIndex[vtxCtr++]);	 
	}
	glEnd();
	vtxCtr++;
	++numverts;
    }
@?{ Fm | Fn
    glShadeModel(GL_SMOOTH);
@?}
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

void
SoIndexedTriangleStripSet::FmVn(SoGLRenderAction *) {
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int ns = numStrips;
    const int *numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const int32_t *const normalIndx = getNormalIndices();
    const int32_t *const colorIndx = getColorIndices();

    unsigned int f;
    unsigned int v = 0;
    int clrCtr = 0;
    glBegin(GL_TRIANGLES);
    for (int strip = 0; strip < ns; strip++) {
	const int nf = (*numverts)-2;

	v += 2;
	for (f = 0; f < nf; f++) {
	    // Per-face:
	    (*colorFunc)(colorPtr+colorStride*colorIndx[clrCtr++]); 

	    // Now three vertices, alternating ordering:
	    if (f & 1) {
		(*normalFunc)(normalPtr+normalIndx[(v-0)]*normalStride);
		(*vertexFunc)(vertexPtr+vertexIndex[(v-0)]*vertexStride);
		(*normalFunc)(normalPtr+normalIndx[(v-1)]*normalStride);
		(*vertexFunc)(vertexPtr+vertexIndex[(v-1)]*vertexStride);
		(*normalFunc)(normalPtr+normalIndx[(v-2)]*normalStride);
		(*vertexFunc)(vertexPtr+vertexIndex[(v-2)]*vertexStride);
	    } else {
		(*normalFunc)(normalPtr+normalIndx[(v-2)]*normalStride);
		(*vertexFunc)(vertexPtr+vertexIndex[(v-2)]*vertexStride);
		(*normalFunc)(normalPtr+normalIndx[(v-1)]*normalStride);
		(*vertexFunc)(vertexPtr+vertexIndex[(v-1)]*vertexStride);
		(*normalFunc)(normalPtr+normalIndx[(v-0)]*normalStride);
		(*vertexFunc)(vertexPtr+vertexIndex[(v-0)]*vertexStride);
	    }
	    ++v;
	}
	++numverts;
	++v;
    }
    glEnd();
   
}

void
SoIndexedTriangleStripSet::FmVnT(SoGLRenderAction *) {
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int ns = numStrips;
    const int *numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const int32_t *const normalIndx = getNormalIndices();
    const int32_t *const colorIndx = getColorIndices();
    const int32_t *const tCoordIndx = getTexCoordIndices();

    unsigned int f;
    unsigned int v = 0;
    int clrCtr =0;
    glBegin(GL_TRIANGLES);
    for (int strip = 0; strip < ns; strip++) {
	const int nf = (*numverts)-2;

	v += 2;
	for (f = 0; f < nf; f++) {
	    // Per-face:
	    (*colorFunc)(colorPtr+colorStride*colorIndx[clrCtr++]);

	    // Now three vertices, alternating ordering:
	    if (f & 1) {
		(*normalFunc)(normalPtr+normalIndx[(v-0)]*normalStride);
		(*texCoordFunc)(texCoordPtr+tCoordIndx[(v-0)]*texCoordStride);
		(*vertexFunc)(vertexPtr+vertexIndex[(v-0)]*vertexStride);
		(*normalFunc)(normalPtr+normalIndx[(v-1)]*normalStride);
		(*texCoordFunc)(texCoordPtr+tCoordIndx[(v-1)]*texCoordStride);
		(*vertexFunc)(vertexPtr+vertexIndex[(v-1)]*vertexStride);
		(*normalFunc)(normalPtr+normalIndx[(v-2)]*normalStride);
		(*texCoordFunc)(texCoordPtr+tCoordIndx[(v-2)]*texCoordStride);
		(*vertexFunc)(vertexPtr+vertexIndex[(v-2)]*vertexStride);
	    } else {
		(*normalFunc)(normalPtr+normalIndx[(v-2)]*normalStride);
		(*texCoordFunc)(texCoordPtr+tCoordIndx[(v-2)]*texCoordStride);
		(*vertexFunc)(vertexPtr+vertexIndex[(v-2)]*vertexStride);
		(*normalFunc)(normalPtr+normalIndx[(v-1)]*normalStride);
		(*texCoordFunc)(texCoordPtr+tCoordIndx[(v-1)]*texCoordStride);
		(*vertexFunc)(vertexPtr+vertexIndex[(v-1)]*vertexStride);
		(*normalFunc)(normalPtr+normalIndx[(v-0)]*normalStride);
		(*texCoordFunc)(texCoordPtr+tCoordIndx[(v-0)]*texCoordStride);
		(*vertexFunc)(vertexPtr+vertexIndex[(v-0)]*vertexStride);
	    }
	    ++v;
	}
	++numverts;
	++v;
    }
    glEnd();
   
}

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

void
SoIndexedTriangleStripSet::VmFn(SoGLRenderAction *) {
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const int ns = numStrips;
    const int * numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const int32_t *const normalIndx = getNormalIndices();
    const int32_t *const colorIndx = getColorIndices();

    unsigned int f;
    unsigned int v = 0;
    int nrmCtr=0;
    glBegin(GL_TRIANGLES);
    for (int strip = 0; strip < ns; strip++) {
	const int nf = (*numverts)-2;

	v += 2;
	for (f = 0; f < nf; f++) {
	    // Per-face:
	    (*normalFunc)(normalPtr+normalIndx[nrmCtr++]*normalStride);

	    // Now three vertices, alternating ordering:
	    if (f & 1) {
		(*colorFunc)(colorPtr+colorIndx[(v-0)]*colorStride);
		(*vertexFunc)(vertexPtr+vertexIndex[(v-0)]*vertexStride);
		(*colorFunc)(colorPtr+colorIndx[(v-1)]*colorStride);
		(*vertexFunc)(vertexPtr+vertexIndex[(v-1)]*vertexStride);
		(*colorFunc)(colorPtr+colorIndx[(v-2)]*colorStride);
		(*vertexFunc)(vertexPtr+vertexIndex[(v-2)]*vertexStride);
	    } else {
		(*colorFunc)(colorPtr+colorIndx[(v-2)]*colorStride);
		(*vertexFunc)(vertexPtr+vertexIndex[(v-2)]*vertexStride);
		(*colorFunc)(colorPtr+colorIndx[(v-1)]*colorStride);
		(*vertexFunc)(vertexPtr+vertexIndex[(v-1)]*vertexStride);
		(*colorFunc)(colorPtr+colorIndx[(v-0)]*colorStride);
		(*vertexFunc)(vertexPtr+vertexIndex[(v-0)]*vertexStride);
	    }
	    ++v;
	}
	++numverts;
	++v;
    }
    glEnd();
    
}
void
SoIndexedTriangleStripSet::VmFnT(SoGLRenderAction *) {
    const char *vertexPtr = vpCache.getVertices(0);
    const unsigned int vertexStride = vpCache.getVertexStride();
    SoVPCacheFunc *const vertexFunc = vpCache.vertexFunc;
    const char *colorPtr = vpCache.getColors(0);
    const unsigned int colorStride = vpCache.getColorStride();
    SoVPCacheFunc *const colorFunc = vpCache.colorFunc;
    const char *normalPtr = vpCache.getNormals(0);
    const unsigned int normalStride = vpCache.getNormalStride();
    SoVPCacheFunc *const normalFunc = vpCache.normalFunc;
    const char *texCoordPtr = vpCache.getTexCoords(0);
    const unsigned int texCoordStride = vpCache.getTexCoordStride();
    SoVPCacheFunc *const texCoordFunc = vpCache.texCoordFunc;
    const int ns = numStrips;
    const int *numverts = numVertices;
    const int32_t *const vertexIndex = coordIndex.getValues(0);
    const int32_t *const normalIndx = getNormalIndices();
    const int32_t *const colorIndx = getColorIndices();
    const int32_t *const tCoordIndx = getTexCoordIndices();

    unsigned int f;
    unsigned int v = 0;
    int nrmCtr = 0;
    glBegin(GL_TRIANGLES);
    for (int strip = 0; strip < ns; strip++) {
	const int nf = (*numverts)-2;

	v += 2;
	for (f = 0; f < nf; f++) {
	    // Per-face:
	    
	    (*normalFunc)(normalPtr+normalIndx[nrmCtr++]*normalStride);

	    // Now three vertices, alternating ordering:
	    if (f & 1) {
		(*colorFunc)(colorPtr+colorIndx[(v-0)]*colorStride);
		(*texCoordFunc)(texCoordPtr+tCoordIndx[(v-0)]*texCoordStride);
		(*vertexFunc)(vertexPtr+vertexIndex[(v-0)]*vertexStride);
		(*colorFunc)(colorPtr+colorIndx[(v-1)]*colorStride);
		(*texCoordFunc)(texCoordPtr+tCoordIndx[(v-1)]*texCoordStride);
		(*vertexFunc)(vertexPtr+vertexIndex[(v-1)]*vertexStride);
		(*colorFunc)(colorPtr+colorIndx[(v-2)]*colorStride);
		(*texCoordFunc)(texCoordPtr+tCoordIndx[(v-2)]*texCoordStride);
		(*vertexFunc)(vertexPtr+vertexIndex[(v-2)]*vertexStride);
	    } else {
		(*colorFunc)(colorPtr+colorIndx[(v-2)]*colorStride);
		(*texCoordFunc)(texCoordPtr+tCoordIndx[(v-2)]*texCoordStride);
		(*vertexFunc)(vertexPtr+vertexIndex[(v-2)]*vertexStride);
		(*colorFunc)(colorPtr+colorIndx[(v-1)]*colorStride);
		(*texCoordFunc)(texCoordPtr+tCoordIndx[(v-1)]*texCoordStride);
		(*vertexFunc)(vertexPtr+vertexIndex[(v-1)]*vertexStride);
		(*colorFunc)(colorPtr+colorIndx[(v-0)]*colorStride);
		(*texCoordFunc)(texCoordPtr+tCoordIndx[(v-0)]*texCoordStride);
		(*vertexFunc)(vertexPtr+vertexIndex[(v-0)]*vertexStride);
	    }
	    ++v;
	}
	++numverts;
	++v;
    }
    glEnd();
   
}

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
@= Vn
@= T  ON
@ RenderFunction
