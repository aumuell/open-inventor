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
 |   $Revision: 1.1.1.1 $
 |
 |   Description:
 |	This file defines the SoUnknownNode node class.
 |
 |   Author(s)		: Dave Immel, Paul S. Strauss, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_UNKNOWN_NODE_
#define  _SO_UNKNOWN_NODE_

#include <Inventor/fields/SoFieldData.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoSubNode.h>
#include <Inventor/misc/SoChildList.h>

class SoSFNode;
class SoMFString;

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoUnknownNode
//
//  The Unknown Node! This node is created during file reading when a
//  node is read that is unrecognized by Inventor and a DSO cannot be found
//  for it.  Note that even though it is derived from SoGroup, we lie
//  and tell SoType that it is derived from SoNode.  This, along with
//  the fact that users are not shipped this header file (so they
//  can't call SoGroup methods directly), has the effect of making the
//  normally public children hidden.
//
//////////////////////////////////////////////////////////////////////////////

class SoUnknownNode : public SoGroup {

    SO_NODE_HEADER(SoUnknownNode);

  public:
    // Constructor
    SoUnknownNode();

  SoEXTENDER public:
    virtual void	write(SoWriteAction *action);

  SoINTERNAL public:
    static void		initClass();
    void                setClassName(const char *name);

  protected:
    // Reads field type information, fields, and children.
    virtual SbBool	readInstance(SoInput *in, unsigned short flags);

    // Override search so we don't search alternateRep
    virtual void	search(SoSearchAction *action);

    // Returns file format name
    virtual const char *getFileFormatName() const;

    // Recursively adds this node and all nodes under it to the copy
    // dictionary. Returns the copy of this node.
    virtual SoNode *	addToCopyDict() const;

    // Copies the contents of the given node into this instance
    virtual void	copyContents(const SoFieldContainer *fromFC,
				     SbBool copyConnections);

  private:
    // Used during reading to create an alternateRep from an isA
    // specification.
    void		createFromIsA(SoMFString *isA);

    // Per-instance field data
    SoFieldData		*instanceFieldData;

    // Real class name
    const char *className;

    // List of hidden children.
    SoChildList		hiddenChildren;
  
    // Destructor
    virtual ~SoUnknownNode();

    // Will be true if read in with children
    SbBool hasChildren;
};

#endif /* _SO_UNKNOWN_NODE_ */
