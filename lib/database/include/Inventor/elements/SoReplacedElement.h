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

//  -*- C++ -*-

/*
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Description:
 |	This file defines the abstract SoEXTENDER SoReplacedElement class.
 |
 |   Classes:
 |	SoReplacedElement
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_REPLACED_ELEMENT
#define  _SO_REPLACED_ELEMENT

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoReplacedElement
//
//  This is the abstract base class for each state element whose value
//  is replaced whenever it is set. (Most state elements fall into
//  this category, so most are derived from this class.)
//
//  This class overrides the SoElement::getElement() method to store
//  the node-id of the node that is about to set the value in the
//  element (i.e., the node that is passed to getElement()). This
//  class also defines the SoElement::matches() method to return TRUE
//  if the node-id's of the two elements match. Subclasses can change
//  this behavior by defining matches() differently, if they wish. For
//  example, they can compare the elements' values, instead.
//
//////////////////////////////////////////////////////////////////////////////

#include <Inventor/elements/SoSubElement.h>

SoEXTENDER class SoReplacedElement : public SoElement {

    SO_ELEMENT_ABSTRACT_HEADER(SoReplacedElement);

  public:
    // Initializes node id in element
    virtual void	init(SoState *state);

    // Returns TRUE if the element matches another element, based on node-id
    virtual SbBool	matches(const SoElement *elt) const;

    // Prints element (for debugging)
    virtual void	print(FILE *fp) const;

  SoINTERNAL public:
    // Initializes the SoReplacedElement class
    static void		initClass();

    // Create and return a copy of this element.  The only operation
    // supported by the copy is matches() (the copy will be
    // unitialized except for the nodeId).
    virtual SoElement	*copyMatchInfo() const;

    // Return nodeId.  This was added so the SoTransformSeparator
    // class can figure out whether or not it contains a camera:
    uint32_t		getNodeId() const { return nodeId; }

  protected:
    uint32_t		nodeId;

    // Overrides SoElement::getElement() to set the nodeId in the
    // element instance before returning it.
    static SoElement *	getElement(SoState *state, int stackIndex,
				   SoNode *node);

    // Destructor
    virtual ~SoReplacedElement();
};

#endif /* _SO_REPLACED_ELEMENT */
