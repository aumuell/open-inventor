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
 |	SoOverrideElement
 |
 |   Author(s)		: Roger Chickering
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/elements/SoOverrideElement.h>
#include <Inventor/elements/SoShapeStyleElement.h>

SO_ELEMENT_SOURCE(SoOverrideElement);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes element.  All flags are initialized to FALSE (0).
//
// Use: public

void
SoOverrideElement::init(SoState *)
//
////////////////////////////////////////////////////////////////////////
{
    flags = 0;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Pushes element, copying flags from previous top instance.
//
// Use: public
void
SoOverrideElement::push(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
    SoOverrideElement *elt = (SoOverrideElement *)getNextInStack();

    flags = elt->flags;
    elt->capture(state);  // Capture previous element
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//     Create a copy of this instance suitable for calling matches()
//     on.
//
// Use: protected

SoElement *
SoOverrideElement::copyMatchInfo() const
//
////////////////////////////////////////////////////////////////////////
{
    SoOverrideElement *result =
	(SoOverrideElement *)getTypeId().createInstance();

    result->flags = flags;

    return result;
}
/////////////////////////////////////////////////////////////////////////
//
//  Description:  set diffuseColorOverride (plus set shapeStyle)
//
//
//  Use: static, public.
/////////////////////////////////////////////////////////////////////////
void
SoOverrideElement::setDiffuseColorOverride(SoState *state, SoNode *, 
    SbBool override)
{
    SoOverrideElement	*elt =
	(SoOverrideElement *)getElement(state, classStackIndex); 
    if (elt != NULL) { 
        if (override){
	    elt->flags |= DIFFUSE_COLOR;
	    SoShapeStyleElement::setOverrides(state, TRUE);
	} 
        else {
            elt->flags &= ~DIFFUSE_COLOR;
	    if (!(elt->flags & MATERIAL_BINDING))
		SoShapeStyleElement::setOverrides(state, FALSE);
	}
    }	          
}

/////////////////////////////////////////////////////////////////////////
//
//  Description:  set transparencyOverride (plus set shapeStyle)
//	This is same as diffuse color override
//
//  Use: static, public.
/////////////////////////////////////////////////////////////////////////
void
SoOverrideElement::setTransparencyOverride(SoState *state, SoNode *, 
    SbBool override)
{
    SoOverrideElement	*elt =
	(SoOverrideElement *)getElement(state, classStackIndex); 
    if (elt != NULL) { 
        if (override){
	    elt->flags |= TRANSPARENCY;
	    SoShapeStyleElement::setOverrides(state, TRUE);
	} 
        else {
            elt->flags &= ~TRANSPARENCY;
	    if (!(elt->flags & MATERIAL_BINDING))
		SoShapeStyleElement::setOverrides(state, FALSE);
	}
    }	          
}
/////////////////////////////////////////////////////////////////////////
//
//  Description:  set Material Binding Override (plus set shapeStyle)
//
//
//  Use: static, public.
/////////////////////////////////////////////////////////////////////////
void
SoOverrideElement::setMaterialBindingOverride(SoState *state, SoNode *, 
    SbBool override)
{
    SoOverrideElement	*elt =
	(SoOverrideElement *)getElement(state, classStackIndex); 
    if (elt != NULL) { 
        if (override){
	    elt->flags |= MATERIAL_BINDING;
	    SoShapeStyleElement::setOverrides(state, TRUE);
	} 
        else {
            elt->flags &= ~MATERIAL_BINDING;
	    if (!(elt->flags & (DIFFUSE_COLOR|TRANSPARENCY)))
		SoShapeStyleElement::setOverrides(state, FALSE);
	}
    }	          
}
////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if elements match
//
// Use: public

SbBool
SoOverrideElement::matches(const SoElement *elt) const
//
////////////////////////////////////////////////////////////////////////
{
    return (flags == ((const SoOverrideElement *) elt)->flags);
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
SoOverrideElement::print(FILE *fp) const
{
    pFlag(fp, "AMBIENT_COLOR", AMBIENT_COLOR);
    pFlag(fp, "COLOR_INDEX", COLOR_INDEX);
    pFlag(fp, "COMPLEXITY", COMPLEXITY);
    pFlag(fp, "COMPLEXITY_TYPE", COMPLEXITY_TYPE);
    pFlag(fp, "CREASE_ANGLE", CREASE_ANGLE);
    pFlag(fp, "DIFFUSE_COLOR", DIFFUSE_COLOR);
    pFlag(fp, "DRAW_STYLE", DRAW_STYLE);
    pFlag(fp, "EMISSIVE_COLOR", EMISSIVE_COLOR);
    pFlag(fp, "FONT_NAME", FONT_NAME);
    pFlag(fp, "FONT_SIZE", FONT_SIZE);
    pFlag(fp, "LIGHT_MODEL", LIGHT_MODEL);
    pFlag(fp, "LINE_PATTERN", LINE_PATTERN);
    pFlag(fp, "LINE_WIDTH", LINE_WIDTH);
    pFlag(fp, "POINT_SIZE", POINT_SIZE);
    pFlag(fp, "PICK_STYLE", PICK_STYLE);
    pFlag(fp, "SHAPE_HINTS", SHAPE_HINTS);
    pFlag(fp, "SHININESS", SHININESS);
    pFlag(fp, "SPECULAR_COLOR", SPECULAR_COLOR);
    pFlag(fp, "TRANSPARENCY", TRANSPARENCY);
    putc('\n', fp);
}
#else  /* DEBUG */
void
SoOverrideElement::print(FILE *) const
{
}
#endif /* DEBUG */

void
SoOverrideElement::pFlag(FILE *fp, const char *flagName, int flagBit) const
{
    if (flags & flagBit)
	fprintf(fp, "%s\t", flagName);
}
