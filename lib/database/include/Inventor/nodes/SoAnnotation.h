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
 |	This file defines the SoAnnotation node class.
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_ANNOTATION_
#define  _SO_ANNOTATION_

#include <Inventor/nodes/SoSeparator.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoAnnotation
//
//  Annotation group node: delays rendering its children until all
//  other nodes have been traversed, turning off depth buffer
//  comparisons first. The result is that the shapes under the
//  annotation node are rendered on top of the rest of the scene.
//
//  Note that if more than one annotation node is present in a graph,
//  the order in which they are traversed determines the stacking
//  order - later nodes are rendered on top of earlier ones. Also note
//  that since depth buffer comparisons are disabled, complex 3D
//  objects may not be rendered correctly when used under annotation
//  nodes.
//
//  All non-rendering actions are inherited as is from SoSeparator.
//
//////////////////////////////////////////////////////////////////////////////
// C-api: prefix=SoAnnot
class SoAnnotation : public SoSeparator {

    SO_NODE_HEADER(SoAnnotation);

  public:

    // No fields

    // Constructor
    SoAnnotation();

  SoEXTENDER public:
    // Implement actions
    virtual void	GLRender(SoGLRenderAction *action);

  SoINTERNAL public:
    static void		initClass();

  protected:
    // Destructor
    virtual ~SoAnnotation();
};

#endif /* _SO_ANNOTATION_ */
