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
 * Copyright (C) 1995-96   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Classes:
 |	SoShapeStyleElement
 |
 |   Author(s)		: Alan Norton, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/elements/SoShapeStyleElement.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/elements/SoDrawStyleElement.h>
#include <Inventor/elements/SoComplexityTypeElement.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoLightModel.h>

const int INVISIBLE_BIT = 0x1;
const int BBOX_BIT = 0x2;
const int DELAY_TRANSP_BIT = 0x4;

SO_ELEMENT_SOURCE(SoShapeStyleElement);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: private
//
////////////////////////////////////////////////////////////////////////
SoShapeStyleElement::~SoShapeStyleElement()

{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes element
//
// Use: public
//
////////////////////////////////////////////////////////////////////////
void
SoShapeStyleElement::init(SoState *)
{
    delayFlags = 0;
    texEnabled = 0;
    texFunc = 0;
    needNorms = 1;
    renderCaseMask = SoVertexPropertyCache::ALL_FROM_STATE_BITS &
	(~SoVertexPropertyCache::TEXCOORD_BIT) &
	(~SoVertexPropertyCache::OVERRIDE_FROM_STATE_BIT);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: private
//
////////////////////////////////////////////////////////////////////////
const SoShapeStyleElement *
SoShapeStyleElement::get(SoState *state)

{
    return (SoShapeStyleElement *)getConstElement(state, classStackIndex);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Push
//
// Use: internal
//
////////////////////////////////////////////////////////////////////////
void
SoShapeStyleElement::push(SoState *state)

{
    SoShapeStyleElement *sse = (SoShapeStyleElement *)getNextInStack();
    delayFlags = sse->delayFlags;
    texEnabled = sse->texEnabled;
    texFunc = sse->texFunc;
    needNorms = sse->needNorms;
    renderCaseMask = sse->renderCaseMask;
    //Capture previous element:
    sse->capture(state);
}

/////////////////////////////////////////////////////////////////////////////
//
//  Description:
//    Set invisible bit if invisible drawstyle
//
//
////////////////////////////////////////////////////////////////////////
void
SoShapeStyleElement::setDrawStyle(SoState* state, int32_t value) 
{
    SoShapeStyleElement *elt = (SoShapeStyleElement *)
	getElement(state, classStackIndex);

    if (value == (int32_t)SoDrawStyleElement::INVISIBLE) {
	elt->delayFlags |= INVISIBLE_BIT;
    } else {
	elt->delayFlags &= ~INVISIBLE_BIT;
    }
}

/////////////////////////////////////////////////////////////////////////////
//
//  Description:
//    Set delayed transparency bit if anything other than screen door.
//
////////////////////////////////////////////////////////////////////////
void
SoShapeStyleElement::setTransparencyType(SoState* state, int32_t value) 
{
    SoShapeStyleElement *elt = (SoShapeStyleElement *)
	getElement(state, classStackIndex);

    if (value == (int32_t)SoGLRenderAction::SCREEN_DOOR) {
	elt->delayFlags &= ~DELAY_TRANSP_BIT;
    } else {
	elt->delayFlags |= DELAY_TRANSP_BIT;
    }
}

//////////////////////////////////////////////////////////////////////////////
//
//  Description:
//    Returns true if doing screen_door transparency
//
//
////////////////////////////////////////////////////////////////////////
SbBool
SoShapeStyleElement::isScreenDoor(SoState* state)
{
    SoShapeStyleElement *elt = (SoShapeStyleElement *)
	getElement(state, classStackIndex);

    return !(elt->delayFlags & DELAY_TRANSP_BIT);
}

//////////////////////////////////////////////////////////////////////////////
//
//  Description:
//    Set the BBOX_BIT based on whether or not the value is BOUNDING_BOX
//
//
////////////////////////////////////////////////////////////////////////
void
SoShapeStyleElement::setComplexityType( SoState* state, int32_t value) 
{
    SoShapeStyleElement *elt = (SoShapeStyleElement *)
	getElement(state, classStackIndex);

    if (value == (int32_t)SoComplexityTypeElement::BOUNDING_BOX) {
	elt->delayFlags |= BBOX_BIT;
    } else {
	elt->delayFlags &= ~BBOX_BIT;
    }
}

//////////////////////////////////////////////////////////////////////////////
//
//  Description:
//
//
////////////////////////////////////////////////////////////////////////
void
SoShapeStyleElement::setTextureEnabled(SoState *state, SbBool value)
{
    SoShapeStyleElement *elt = (SoShapeStyleElement *)
	getElement(state, classStackIndex);

    elt->texEnabled = value;

    // Clear renderCase bits
    elt->renderCaseMask &= ~SoVertexPropertyCache::TEXCOORD_BIT;

    // Set renderCase bits, if need texcoords:
    if (elt->needTexCoords())
	elt->renderCaseMask |= SoVertexPropertyCache::TEXCOORD_BIT;
}

//////////////////////////////////////////////////////////////////////////////
//
//  Description:
//
////////////////////////////////////////////////////////////////////////////
void
SoShapeStyleElement::setTextureFunction(SoState *state, SbBool value)
{
    SoShapeStyleElement *elt = (SoShapeStyleElement *)
	getElement(state, classStackIndex);

    elt->texFunc = value;
    if (value) 
	elt->renderCaseMask |= SoVertexPropertyCache::TEXTURE_FUNCTION_BIT;
    else
	elt->renderCaseMask &= ~SoVertexPropertyCache::TEXTURE_FUNCTION_BIT;

    // Clear renderCase bits
    elt->renderCaseMask &= ~SoVertexPropertyCache::TEXCOORD_BIT;

    // Set renderCase bits, if need texcoords:
    if (elt->needTexCoords())
	elt->renderCaseMask |= SoVertexPropertyCache::TEXCOORD_BIT;
}

//////////////////////////////////////////////////////////////////////////////
//
//  Description:
//	set light model
//
/////////////////////////////////////////////////////////////////////////////
void
SoShapeStyleElement::setLightModel(SoState *state, int32_t value)
{
    SoShapeStyleElement *elt = (SoShapeStyleElement *)
	getElement(state, classStackIndex);

    if (value == (int32_t)SoLightModel::BASE_COLOR) {
	elt->needNorms = FALSE;
    } else {
	elt->needNorms = TRUE;
    }

    // Clear renderCase bits
    elt->renderCaseMask &= ~SoVertexPropertyCache::NORMAL_BITS;

    // Set renderCase bits, if need normals:
    if (elt->needNormals())
	elt->renderCaseMask |= SoVertexPropertyCache::NORMAL_BITS;
}
///////////////////////////////////////////////////////////////////////
//
// Description:
//    Turn on or off the override mask, indicating that there is
//    an override of transparency, diffuse color, or material binding
//    Invoked as a side-effect of SoOverrideElement::set..
//
// use: public, static
////////////////////////////////////////////////////////////////////////
void
SoShapeStyleElement::setOverrides(SoState *state, SbBool value)
{
    SoShapeStyleElement *elt = (SoShapeStyleElement *)
	getElement(state, classStackIndex);
    if (value) elt->renderCaseMask |= 
	SoVertexPropertyCache::OVERRIDE_FROM_STATE_BIT;
    else elt->renderCaseMask &=
	(~SoVertexPropertyCache::OVERRIDE_FROM_STATE_BIT);
}   
////////////////////////////////////////////////////////////////////////
//
// Description:
//    Matches method for caches...
//
// Use: public
///////////////////////////////////////////////////////////////////////
SbBool
SoShapeStyleElement::matches(const SoElement *elt) const

{
    const SoShapeStyleElement *le = (const SoShapeStyleElement *) elt;
    return (delayFlags == le->delayFlags &&
	    renderCaseMask == le->renderCaseMask);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//     Create a copy of this instance suitable for calling matches()
//     on.
//
// Use: protected
//
////////////////////////////////////////////////////////////////////////
SoElement *
SoShapeStyleElement::copyMatchInfo() const

{
    SoShapeStyleElement *result =
	(SoShapeStyleElement *)getTypeId().createInstance();

    result->delayFlags = delayFlags;
    result->needNorms = needNorms;
    result->texEnabled = texEnabled;
    result->texFunc = texFunc;
    result->renderCaseMask = renderCaseMask;

    return result;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Prints element for debugging.
//
// Use: public
//
////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
void
SoShapeStyleElement::print(FILE *fp) const
{
    SoElement::print(fp);

    fprintf(fp, "\t");
    if (needNormals()) fprintf(fp, "need normals, ");
    if (needTexCoords()) fprintf(fp, "need texcoords, ");
    fprintf(fp, "delayFlags: %d\n", delayFlags);
}
#else  /* DEBUG */
void
SoShapeStyleElement::print(FILE *) const
{
}
#endif /* DEBUG */
