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
 |   $Revision $
 |
 |   Classes:
 |	SoProfileElement class.
 |
 |   Author(s)		: Paul S. Strauss, Nick Thompson, Thad Beier,
 |                        Dave Immel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifdef DEBUG
#include <assert.h>
#endif

#include <Inventor/elements/SoProfileElement.h>
#include <Inventor/nodes/SoProfile.h>

SO_ELEMENT_SOURCE(SoProfileElement);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: public

SoProfileElement::~SoProfileElement()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes element.
//
// Use: public

void
SoProfileElement::init(SoState *)
//
////////////////////////////////////////////////////////////////////////
{
    profiles.truncate(0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds the given profile to the list of profiles
//
// Use: public

void
SoProfileElement::add(SoState *state, SoProfile *profile)
//
////////////////////////////////////////////////////////////////////////
{
    SoProfileElement	*elt;

    elt = (SoProfileElement *) getElement(state, classStackIndex);

    if (elt != NULL && profile != NULL) {

        // append the given profile node to the list of profiles in the
        // element.  If the directive on the profile is START_FIRST,
        // truncate the profile list before adding this one.
        if (profile->linkage.getValue() == START_FIRST) {
            elt->profiles.truncate(0);
	    elt->clearNodeIds();
	}
	elt->profiles.append(profile);
	elt->addNodeId(profile);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the current profile node
//
// Use: public

const SoNodeList &
SoProfileElement::get(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
    const SoProfileElement *elt;
    elt = (const SoProfileElement *)
	getConstElement(state, classStackIndex);

    return elt->profiles;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Override push to copy the existing profiles from the previous
//    set.
//
// Use: public

void
SoProfileElement::push(SoState *)
//
////////////////////////////////////////////////////////////////////////
{
    SoProfileElement *elt;
    elt = (SoProfileElement *) getNextInStack();
    
    // Rely on SoNodeList::operator = to do the right thing...
    profiles = elt->profiles;
    nodeIds = elt->nodeIds;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Prints contents of element for debugging.
//
// Use: public
//
////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
void
SoProfileElement::print(FILE *fp) const
{
    SoAccumulatedElement::print(fp);
}
#else  /* DEBUG */
void
SoProfileElement::print(FILE *) const
{
}
#endif /* DEBUG */
