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
 |	This file defines the abstract SoEXTENDER SoAccumulatedElement
 |	class.
 |
 |   Classes:
 |	SoAccumulatedElement
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_ACCUMULATED_ELEMENT
#define  _SO_ACCUMULATED_ELEMENT

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoAccumulatedElement
//
//  This is the abstract base class for each state element whose value
//  may be accumulated when it is set. (This is rare.) Examples are
//  transformations and profiles.
//
//  Subclasses may need to override the push() method to copy values
//  from the next instance in the stack (using getNextInStack() ),
//  if the new one has to accumulate values on top of the old ones.
//
//  This class defines the matches() method to compare lists of
//  node-id's. The node-id's represent the states of all nodes that
//  changed the value of the element. SoAccumulatedElement provides
//  methods that maintain lists of node-id's of all nodes that affect
//  an instance. Subclasses must call these methods to make sure the
//  id's are up to date, if they plan to use the standard matches()
//  method. Otherwise, they can define matches() differently, if they
//  wish.
//
//////////////////////////////////////////////////////////////////////////////

#include <Inventor/elements/SoSubElement.h>

SoEXTENDER class SoAccumulatedElement : public SoElement {

    SO_ELEMENT_ABSTRACT_HEADER(SoAccumulatedElement);

  public:
    // Returns TRUE if the element matches another element, based on node-id's
    virtual SbBool	matches(const SoElement *elt) const;

    // Prints element (for debugging)
    virtual void	print(FILE *fp) const;

  SoINTERNAL public:
    // Initializes the SoAccumulatedElement class
    static void		initClass();

  protected:
    // Clears out the list of node id's
    void		clearNodeIds();

    // Adds the id of the given node to the current list
    void		addNodeId(const SoNode *node);

    // Sets the node id list to JUST the id of the given node
    void		setNodeId(const SoNode *node);

    // Create and return a copy of this element; this will copy the
    // nodeId list properly.
    virtual SoElement	*copyMatchInfo() const;

    // Override normal capture method to capture elements at other
    // depths that we are accumulating with.
    virtual void	captureThis(SoState *state) const;

    // Destructor
    virtual ~SoAccumulatedElement();

    // This stores the list of node id's as pointers, since they
    // should be the same length as int32_ts. The id's are sorted by
    // increasing value, to make comparisons easier.
    SbPList		nodeIds;
  private:

    SbBool		accumulatesWithParentFlag;
};

#endif /* _SO_ACCUMULATED_ELEMENT */
