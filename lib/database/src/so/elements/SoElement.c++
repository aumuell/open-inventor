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
 |	SoElement base class.
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


#include <Inventor/SoLists.h>
#include <Inventor/elements/SoCacheElement.h>
#include <Inventor/elements/SoElement.h>
#include <Inventor/misc/SoState.h>
#include <Inventor/nodes/SoNode.h>

// Static variables declared in SoElement.h:

int		SoElement::classStackIndex;
SoType		SoElement::classTypeId;
int		SoElement::nextStackIndex;
SoTypeList     *SoElement::stackToType;

// If we run out of slots in the array of free lists, add this many more
#define FREE_LIST_INCREMENT	10

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor.
//
// Use: protected

SoElement::SoElement()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: private

SoElement::~SoElement()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes element. Default does nothing.
//
// Use: public

void
SoElement::init(SoState *)
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Pushes element. Default does nothing
//
// Use: public

void
SoElement::push(SoState *)
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Pops element. Default does nothing
//
// Use: public

void
SoElement::pop(SoState *, const SoElement *)
//
////////////////////////////////////////////////////////////////////////
{
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
SoElement::print(FILE *fp) const
{
    fprintf(fp, "Element: type %s, depth %d\n",
	    typeId.getName().getString(), depth);
}
#else  /* DEBUG */
void
SoElement::print(FILE *) const
{
}
#endif /* DEBUG */

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the id for the element with the given stack index.
//
// Use: internal static, debug only!

SoType
SoElement::getIdFromStackIndex(int stackIndex)
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    return (*stackToType)[stackIndex];
#else
    return SoType::badType();
#endif
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Creates and returns a new stack index for an element class to
//    use within SoState instances. It is passed the type id of the
//    element class so we can correlate stack indices to type id's for
//    debugging purposes.
//
// Use: internal protected

int
SoElement::createStackIndex(SoType id)
//
////////////////////////////////////////////////////////////////////////
{
    int	stackIndex = nextStackIndex++;

#ifdef DEBUG
    // Store id in list so we can get it from stack index later
    stackToType->set(stackIndex, id);
#endif

    return stackIndex;
}

#undef FREE_LIST_INCREMENT

////////////////////////////////////////////////////////////////////////
//
// Description:
//    "Captures" this element for caching purposes. The element is
//    added to all currently open caches, using the SoCacheElement.
//
// Use: virtual, protected

void
SoElement::captureThis(SoState *state) const
//
////////////////////////////////////////////////////////////////////////
{
    SoCacheElement::addElement(state, this);
}

