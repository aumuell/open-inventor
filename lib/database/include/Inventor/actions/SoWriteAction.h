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
 |	Defines the SoWriteAction class
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_WRITE_ACTION_
#define  _SO_WRITE_ACTION_

#include <Inventor/actions/SoAction.h>
#include <Inventor/actions/SoSubAction.h>
#include <Inventor/SoOutput.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoWriteAction
//
//  For writing scene graphs to files. There are two stages in this
//  action. First it has to count references to nodes and paths, since
//  multiply-referenced nodes and paths need to have temporary names
//  defined for them. The second stage is writing the stuff out.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoWriteAct
class SoWriteAction : public SoAction {

    SO_ACTION_HEADER(SoWriteAction);

  public:
    // Constructor (default action writes to stdout)
    SoWriteAction();

    // Constructor that takes SoOutput to write to
    // C-api: name=CreateOut
    SoWriteAction(SoOutput *out);

    // Destructor
    virtual ~SoWriteAction();

    // Returns SoOutput
    // C-api: name=getOut
    SoOutput *		getOutput() const		{ return output; }

  SoINTERNAL public:
    static void		initClass();

    // Continues write action on a graph or path
    void		continueToApply(SoNode *node);
    void		continueToApply(SoPath *path);

  protected:
    // Initiates action on graph
    virtual void	beginTraversal(SoNode *node);

    // Override method to return FALSE, since this action applies
    // itself to each path separately, so it doesn't need the extra
    // overhead of compacting the list.
    virtual SbBool	shouldCompactPathLists() const;

  private:
    SoOutput		*output;	// Output info
    SbBool		createdOutput;	// TRUE if output created by action
    SbBool		continuing;	// TRUE only if continueToApply()
					// was used to apply action
    SbBool		doOneStage;	// TRUE if only supposed to do 1 stage
    SbPList		savedLists;	// Path lists saved for later apply

    // Performs traversal on a path list, which is a little tricker
    void		traversePathList(SoNode *node);
};

#endif /* _SO_WRITE_ACTION_ */
