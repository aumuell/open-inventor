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
 |	SoComplexityTypeElement
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/elements/SoComplexityTypeElement.h>
#include <Inventor/elements/SoShapeStyleElement.h>

SO_ELEMENT_SOURCE(SoComplexityTypeElement);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: private

SoComplexityTypeElement::~SoComplexityTypeElement()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes element
//
// Use: public

void
SoComplexityTypeElement::init(SoState *)
//
////////////////////////////////////////////////////////////////////////
{
    data = getDefault();
}
///////////////////////////////////////////////////////////////////////
//
// Description:
//  Set the complexity type in the state
//
void SoComplexityTypeElement::set(SoState *state, Type type)
{
    SoInt32Element::set(classStackIndex, state, (int32_t)type); 
    SoShapeStyleElement::setComplexityType(state,(int32_t)type);
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
SoComplexityTypeElement::print(FILE *fp) const
{
    const char *typeName;

    SoElement::print(fp);

    switch ((Type)data) {

      case OBJECT_SPACE:
	typeName = "OBJECT_SPACE";
	break;

      case SCREEN_SPACE:
	typeName = "SCREEN_SPACE";
	break;

      case BOUNDING_BOX:
	typeName = "BOUNDING_BOX";
	break;
    }

    fprintf(fp, "\tComplexity Type = %s\n", typeName);
}
#else  /* DEBUG */
void
SoComplexityTypeElement::print(FILE *) const
{
}
#endif /* DEBUG */
