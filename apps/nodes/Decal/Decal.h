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

#ifndef _DECAL_
#define _DECAL_

#include <Inventor/nodes/SoSeparator.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: Decal
//
//  Decal node used to correctly render coplanar polygons
//
//  This class is derived from SoSeparator with a different rendering
//  behavior.  The first child is the base node and the other children
//  are layers to be drawn above the base (in the order in which they
//  are defined).  Some or all of the layers can be coplanar with the 
//  base.  The layer polygons must lie within the base polygon otherwise
//  the results are incorrect
//  
//  All non-rendering actions are inherited as is from SoSeparator.
//
//////////////////////////////////////////////////////////////////////////////
class Decal : public SoSeparator {

    SO_NODE_HEADER(Decal);

  public:

    // No fields.  Exercise for the reader:  Add an SoSFFloat field
    // that specifies how much depth-buffer offset to give to each
    // layer.

    // Constructor
    Decal();

  SoEXTENDER public:
    // Implement render action:
    virtual void	GLRenderBelowPath(SoGLRenderAction *action);

    // All other rendering cases, and all other actions, are handled
    // correctly by base class (SoSeparator) code.

  SoINTERNAL public:
    static void		initClass();

  protected:
    // Destructor
    virtual ~Decal();
};

#endif  /* _DECAL_ */
