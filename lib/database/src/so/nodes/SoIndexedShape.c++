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
 |      SoIndexedShape
 |
 |   Author(s)          : Paul S. Strauss, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/elements/SoCoordinateElement.h>
#include <Inventor/elements/SoTextureCoordinateBindingElement.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/nodes/SoIndexedShape.h>

SO_NODE_ABSTRACT_SOURCE(SoIndexedShape);

int32_t *SoIndexedShape::consecutiveIndices = NULL;
int SoIndexedShape::numConsecutiveIndicesAllocated = 0;

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: protected

SoIndexedShape::SoIndexedShape()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoIndexedShape);

    SO_NODE_ADD_FIELD(coordIndex,	  (0));
    SO_NODE_ADD_FIELD(materialIndex,	 (-1));
    SO_NODE_ADD_FIELD(normalIndex,	 (-1));
    SO_NODE_ADD_FIELD(textureCoordIndex, (-1));
    colorI = normalI = texCoordI = NULL;
    // force reevaluation of binding:
    materialBinding = normalBinding = texCoordBinding = 0;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoIndexedShape::~SoIndexedShape()
//
////////////////////////////////////////////////////////////////////////
{
    if (materialBinding == SoMaterialBindingElement::PER_VERTEX)      
	    delete[] ((int32_t *)colorI);
	
    if (normalBinding == SoNormalBindingElement::PER_VERTEX) 
	    delete[] ((int32_t *)normalI);
	    
    if (texCoordBinding == SoTextureCoordinateBindingElement::PER_VERTEX) 
	    delete[] ((int32_t *)texCoordI);

}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Keep things up to date when my fields change
//
// Use: protected

void
SoIndexedShape::notify(SoNotList *list)
//
////////////////////////////////////////////////////////////////////////
{
    if (list->getLastRec()->getType() == SoNotRec::CONTAINER) {
	if (list->getLastField() == &coordIndex ||
	    list->getLastField() == &materialIndex ||
	    list->getLastField() == &normalIndex ||
	    list->getLastField() == &textureCoordIndex) {

	    // Force re-evaluation:
	    materialBinding = normalBinding = texCoordBinding = 0;
	}
    }

    SoVertexShape::notify(list);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This handles bounding box computation for all shapes derived
//    from this class. It sets the bounding box to contain all
//    vertices of the shape, assuming that the shape uses the
//    coordinates indexed by all non-negative values in the coordIndex
//    field. It also sets the center to the average of the vertices'
//    coordinates.
//
// Use: protected

void
SoIndexedShape::computeBBox(SoAction *action, SbBox3f &box, SbVec3f &center)
//
////////////////////////////////////////////////////////////////////////
{
    int32_t			i, numIndices, numUsed;
    const int32_t			*indices;
    const SoCoordinateElement	*ce = NULL;
    const SbVec3f 		*vpCoords = NULL;

    SoVertexProperty *vp = (SoVertexProperty *)vertexProperty.getValue();
    if (vp && vp->vertex.getNum() > 0) {
	vpCoords = vp->vertex.getValues(0);
    } else {
	ce = SoCoordinateElement::getInstance(action->getState());
    }

    // Start with an empty box and zero sum
    center.setValue(0, 0, 0);
    box.makeEmpty();

    // Loop through coordinates, keeping bounding box and sum of coords
    numIndices = coordIndex.getNum();
    indices    = coordIndex.getValues(0);
    numUsed    = 0;
    for (i = 0; i < numIndices; i++) {

	// Look only at non-negative index values
	if (indices[i] >= 0) {
	    const SbVec3f &v = (ce ? ce->get3((int) indices[i]) :
				vpCoords[indices[i]]);
	    box.extendBy(v);
	    center += v;
	    numUsed++;
	}
    }

    // Center is average of all coordinates
    center /= (float) numUsed;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if texture coordinates should be indexed.
//
// Use: protected, static

SbBool
SoIndexedShape::areTexCoordsIndexed(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    return SoTextureCoordinateBindingElement::get(action->getState()) 
	    == SoTextureCoordinateBindingElement::PER_VERTEX_INDEXED;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Starting at a given index in the coordIndex array, figure out
//    how many vertices there are before either the end of the array
//    or the next 'border' index:
//
// Use: protected

int
SoIndexedShape::getNumVerts(int startCoord)
//
////////////////////////////////////////////////////////////////////////
{
    int result = 0;
    int numIndices = coordIndex.getNum();
    
    while (startCoord+result < numIndices &&
	   coordIndex[startCoord+result] >= 0) {
	result++;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//
// Setup for fast rendering.  This should be called by subclasses,
// which can then use the texCoordI/colorI/normalI arrays (which
// will either point to one of the coordIndex arrays, or to a
// consective array of integers.  This must be called AFTER the
// vpCache has been filled in.
//
// Use: protected

void
SoIndexedShape::setupIndices(int numParts, int numFaces,
			     SbBool needNormals, SbBool needTexCoords)
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (vpCache.vertexPtr == NULL) {
	SoDebugError::post("SoIndexedShape::setupIndices",
		   "vpCache.fillInCache must be called first!\n");
    }
    //Check for valid indices:
    for (int j = 0; j < coordIndex.getNum(); j++){	
    	if (coordIndex[j] != -1) {
	    if (coordIndex[j] >= vpCache.getNumVertices() ||
		coordIndex[j] < 0 ){
		SoDebugError::post("SoIndexedShape",
			"vertex index %d is out of range 0 - %d", 
			coordIndex[j], vpCache.getNumVertices());
	    }
	}
    }
    for (j = 0; j < materialIndex.getNum(); j++){	
    	if (materialIndex[j] != -1) {
	    if (materialIndex[j] >= vpCache.getNumColors() ||
		materialIndex[j] < 0 ){
		SoDebugError::post("SoIndexedShape",
			"material index %d is out of range 0 - %d", 
			materialIndex[j], vpCache.getNumColors());
	    }
	}
    }
    if (needNormals) {
	for (j = 0; j < normalIndex.getNum(); j++){	
    	    if (normalIndex[j] != -1) {
		if (normalIndex[j] >= vpCache.getNumNormals() ||
		    normalIndex[j] < 0 ){
		    SoDebugError::post("SoIndexedShape",
			"normal index %d is out of range 0 - %d", 
			normalIndex[j], vpCache.getNumNormals());
		}
	    }
	}
    }
    if (needTexCoords){
	for (j = 0; j < textureCoordIndex.getNum(); j++){	
    	    if (textureCoordIndex[j] != -1) {
		if (textureCoordIndex[j] >= vpCache.getNumTexCoords() ||
		    textureCoordIndex[j] < 0 ){
		    SoDebugError::post("SoIndexedShape",
			"texture coordinate index %d is out of range 0 - %d", 
			textureCoordIndex[j], vpCache.getNumTexCoords());
		}
	    }
	}
    }
#endif /*DEBUG*/ 

    if (materialBinding != vpCache.getMaterialBinding()) {
	// Free if old binding was PER_VERTEX:
	if (materialBinding == SoMaterialBindingElement::PER_VERTEX) {
	    // Ok to cast const away here:
	    delete[] ((int32_t *)colorI);
	    colorI = NULL;
	}
	materialBinding = vpCache.getMaterialBinding();

#ifdef DEBUG
	int numIndices = 0;
	int numIndicesNeeded = 0;
	SbBool useCoordIndexOK = TRUE;
	int numColorsNeeded = 0;
	const char *bindingString = NULL;
#endif

	switch (vpCache.getMaterialBinding()) {
	  case SoMaterialBindingElement::OVERALL:
	    break;
	  case SoMaterialBindingElement::PER_PART:
	    allocateSequential(numParts);
	    colorI = NULL;
#ifdef DEBUG
	    numColorsNeeded = numParts;
	    bindingString = "PER_PART";
#endif
	    break;
	  case SoMaterialBindingElement::PER_FACE:
	    allocateSequential(numFaces);
	    colorI = NULL;
#ifdef DEBUG
	    numColorsNeeded = numFaces;
	    bindingString = "PER_FACE";
#endif
	    break;

	  case SoMaterialBindingElement::PER_VERTEX:
	    // The annoying case:
	    colorI = allocateSequentialWithHoles();
#ifdef DEBUG
	    for (int i = 0; i < coordIndex.getNum(); i++) {
		if (coordIndex[i] >= 0) ++numColorsNeeded;
	    }
	    bindingString = "PER_VERTEX";
#endif
	    break;

	  case SoMaterialBindingElement::PER_PART_INDEXED:
	    if (materialIndex[0] < 0)
		colorI = coordIndex.getValues(0);
	    else
		colorI = materialIndex.getValues(0);
#ifdef DEBUG
	    useCoordIndexOK = FALSE;
	    numIndicesNeeded = numParts;
	    bindingString = "PER_PART_INDEXED";
	    if (colorI == coordIndex.getValues(0)) {
		numIndices = coordIndex.getNum();
	    } else {
		numIndices = materialIndex.getNum();
	    }
#endif
	    break;
	  case SoMaterialBindingElement::PER_FACE_INDEXED:
	    if (materialIndex[0] < 0)
		colorI = coordIndex.getValues(0);
	    else
		colorI = materialIndex.getValues(0);
#ifdef DEBUG
	    useCoordIndexOK = FALSE;
	    numIndicesNeeded = numFaces;
	    bindingString = "PER_FACE_INDEXED";
	    if (colorI == coordIndex.getValues(0)) {
		numIndices = coordIndex.getNum();
	    } else {
		numIndices = materialIndex.getNum();
	    }
#endif
	    break;
	  case SoMaterialBindingElement::PER_VERTEX_INDEXED:
	    if (materialIndex[0] < 0)
		colorI = coordIndex.getValues(0);
	    else
		colorI = materialIndex.getValues(0);
#ifdef DEBUG
	    useCoordIndexOK = TRUE;
	    numIndicesNeeded = coordIndex.getNum();
	    bindingString = "PER_VERTEX_INDEXED";
	    if (colorI == coordIndex.getValues(0)) {
		numIndices = coordIndex.getNum();
	    } else {
		numIndices = materialIndex.getNum();
	    }
#endif
	    break;
	}

#ifdef DEBUG
	// Check for mis-use of default materialIndex field
	if (useCoordIndexOK == FALSE && 
	    colorI == coordIndex.getValues(0)) {
	    SoDebugError::post("SoIndexedShape",
		"Material binding is %s"
		" but materialIndex[0] < 0; coordIndex"
		" will be used, which is probably not what"
		" you want", bindingString);
	}
	// Check for enough indices:
	if (numIndices < numIndicesNeeded) {
	    SoDebugError::post("SoIndexedShape",
		"Need %d indices for %s "
		" material binding, have only %d",
		numIndicesNeeded, bindingString, numIndices);
	}
	else if (numIndices > 0) {
	    // Find greatest index:
	    for (int i = 0; i < numIndices; i++) {
		if (colorI[i] > numColorsNeeded)
		    numColorsNeeded = colorI[i];
	    }
	}
	if (vpCache.getNumColors() < numColorsNeeded) {
	    SoDebugError::post("SoIndexedShape",
		"Material binding is %s, but only %d"
		" colors given (%d needed)",
		bindingString, vpCache.getNumColors(), numColorsNeeded);
	}
#endif
    }
    if (needNormals && (normalBinding != vpCache.getNormalBinding())) {
	// Free if old binding was PER_VERTEX:
	if (normalBinding == SoNormalBindingElement::PER_VERTEX) {
	    // Ok to cast const away here:
	    delete[] ((int32_t *)normalI);
	    normalI = NULL;
	}
	normalBinding = vpCache.getNormalBinding();

#ifdef DEBUG
	int numIndices = 0;
	int numIndicesNeeded = 0;
	SbBool useCoordIndexOK = TRUE;
	int numNormalsNeeded = 0;
	const char *bindingString = NULL;
#endif

	switch (vpCache.getNormalBinding()) {
	  case SoNormalBindingElement::OVERALL:
	    break;
	  case SoNormalBindingElement::PER_PART:
	    allocateSequential(numParts);
	    normalI = NULL;
#ifdef DEBUG
	    numNormalsNeeded = numParts;
	    bindingString = "PER_PART";
#endif
	    break;
	  case SoNormalBindingElement::PER_FACE:
	    allocateSequential(numFaces);
	    normalI = NULL;
#ifdef DEBUG
	    numNormalsNeeded = numFaces;
	    bindingString = "PER_FACE";
#endif
	    break;

	  case SoNormalBindingElement::PER_VERTEX:
	    // The annoying case:
	    normalI = allocateSequentialWithHoles();
#ifdef DEBUG
	    for (int i = 0; i < coordIndex.getNum(); i++) {
		if (coordIndex[i] >= 0) ++numNormalsNeeded;
	    }
	    bindingString = "PER_VERTEX";
#endif
	    break;

	  case SoNormalBindingElement::PER_PART_INDEXED:
	    if (normalIndex[0] < 0)
		normalI = coordIndex.getValues(0);
	    else
		normalI = normalIndex.getValues(0);
#ifdef DEBUG
	    useCoordIndexOK = FALSE;
	    numIndicesNeeded = numParts;
	    bindingString = "PER_PART_INDEXED";
	    if (normalI == coordIndex.getValues(0)) {
		numIndices = coordIndex.getNum();
	    } else {
		numIndices = normalIndex.getNum();
	    }
#endif
	    break;
	  case SoNormalBindingElement::PER_FACE_INDEXED:
	    if (normalIndex[0] < 0)
		normalI = coordIndex.getValues(0);
	    else
		normalI = normalIndex.getValues(0);
#ifdef DEBUG
	    useCoordIndexOK = FALSE;
	    numIndicesNeeded = numFaces;
	    bindingString = "PER_FACE_INDEXED";
	    if (normalI == coordIndex.getValues(0)) {
		numIndices = coordIndex.getNum();
	    } else {
		numIndices = normalIndex.getNum();
	    }
#endif
	    break;
	  case SoNormalBindingElement::PER_VERTEX_INDEXED:
	    if (normalIndex[0] < 0)
		normalI = coordIndex.getValues(0);
	    else
		normalI = normalIndex.getValues(0);
#ifdef DEBUG
	    useCoordIndexOK = TRUE;
	    numIndicesNeeded = coordIndex.getNum();
	    bindingString = "PER_VERTEX_INDEXED";
	    if (normalI == coordIndex.getValues(0)) {
		numIndices = coordIndex.getNum();
	    } else {
		numIndices = normalIndex.getNum();
	    }
#endif
	    break;
	}

#ifdef DEBUG
	// Check for mis-use of default normalIndex field
	if (useCoordIndexOK == FALSE && 
	    normalI == coordIndex.getValues(0)) {
	    SoDebugError::post("SoIndexedShape",
		"Normal binding is %s"
		" but normalIndex[0] < 0; coordIndex"
		" will be used, which is probably not what"
		" you want", bindingString);
	}
	// Check for enough indices:
	if (numIndices < numIndicesNeeded) {
	    SoDebugError::post("SoIndexedShape",
		"Need %d indices for %s "
		" normal binding, have only %d",
		numIndicesNeeded, bindingString, numIndices);
	}
	else if (numIndices > 0) {
	    // Find greatest index:
	    for (int i = 0; i < numIndices; i++) {
		if (normalI[i] > numNormalsNeeded)
		    numNormalsNeeded = normalI[i];
	    }
	}
	if (vpCache.getNumNormals() < numNormalsNeeded) {
	    SoDebugError::post("SoIndexedShape",
		"Normal binding is %s, but only %d"
		" normals given (%d needed)",
		bindingString, vpCache.getNumNormals(), numNormalsNeeded);
	}
#endif
    }

    if (needTexCoords && (texCoordBinding != vpCache.getTexCoordBinding())) {
	// Free if old binding was PER_VERTEX:
	if (texCoordBinding == SoTextureCoordinateBindingElement::PER_VERTEX) {
	    // Ok to cast const away here:
	    delete[] ((int32_t *)texCoordI);
	    texCoordI = NULL;
	}
	texCoordBinding = vpCache.getTexCoordBinding();

#ifdef DEBUG
	int numIndices = 0;
	int numIndicesNeeded = 0;
	int numTexCoordsNeeded = 0;
	const char *bindingString = NULL;
#endif
	switch (vpCache.getTexCoordBinding()) {
	  case SoTextureCoordinateBindingElement::PER_VERTEX:
	    // The annoying case:
	    texCoordI = allocateSequentialWithHoles();
#ifdef DEBUG
	    for (int i = 0; i < coordIndex.getNum(); i++) {
		if (coordIndex[i] >= 0) ++numTexCoordsNeeded;
	    }
	    bindingString = "PER_VERTEX";
#endif
	    break;

	  case SoTextureCoordinateBindingElement::PER_VERTEX_INDEXED:
	    if (textureCoordIndex[0] < 0)
		texCoordI = coordIndex.getValues(0);
	    else
		texCoordI = textureCoordIndex.getValues(0);
#ifdef DEBUG
	    numIndicesNeeded = numFaces;
	    bindingString = "PER_VERTEX_INDEXED";
	    if (texCoordI == coordIndex.getValues(0)) {
		numIndices = coordIndex.getNum();
	    } else {
		numIndices = textureCoordIndex.getNum();
	    }
#endif
	    break;
	}
#ifdef DEBUG
	// Check for enough indices:
	if (numIndices < numIndicesNeeded) {
	    SoDebugError::post("SoIndexedShape",
		"Need %d indices for %s "
		" texCoord binding, have only %d",
		bindingString, numFaces, numIndices);
	}
	else if (numIndices > 0) {
	    // Find greatest index:
	    for (int i = 0; i < numIndices; i++) {
		if (texCoordI[i] > numTexCoordsNeeded)
		    numTexCoordsNeeded = texCoordI[i];
	    }
	}
	if (vpCache.getNumTexCoords() < numTexCoordsNeeded) {
	    SoDebugError::post("SoIndexedShape",
		"TexCoord binding is %s, but only %d"
		" texCoords given (%d needed)",
		bindingString, vpCache.getNumTexCoords(), numTexCoordsNeeded);
	}
#endif
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Re-allocate the consecutiveIndices array, if necessary
//
// Use: private

void
SoIndexedShape::allocateSequential(int howMany)
//
////////////////////////////////////////////////////////////////////////
{
    if (howMany > numConsecutiveIndicesAllocated) {
	numConsecutiveIndicesAllocated = howMany;
	if (consecutiveIndices != NULL) {
	    delete[] consecutiveIndices;
	}
	consecutiveIndices = new int32_t[howMany];
	for (int i = 0; i < howMany; i++) {
	    consecutiveIndices[i] = i;
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Return an array for PER_VERTEX bindings (with -1's in the right
//    spots)
//
// Use: private

int32_t *
SoIndexedShape::allocateSequentialWithHoles()
//
////////////////////////////////////////////////////////////////////////
{
    int count = 0;
    int num = coordIndex.getNum();
    int32_t *result = new int32_t[num];
    for (int i = 0; i < num; i++) {
	if (coordIndex[i] >= 0) {
	    result[i] = count;
	    count++;
	}
	else
	    result[i] = coordIndex[i]; // Just copy-over negatives
    }
    return result;
}
