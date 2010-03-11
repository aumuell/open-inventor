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
 |      SoFile
 |
 |   Author(s)          : Paul S. Strauss, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SoDB.h>
#include <Inventor/actions/SoActions.h>
#include <Inventor/errors/SoReadError.h>
#include <Inventor/nodes/SoFile.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/sensors/SoFieldSensor.h>

SO_NODE_SOURCE(SoFile);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoFile::SoFile() : children(this)
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoFile);
    SO_NODE_ADD_FIELD(name, ("<Undefined file>"));

    // Set up sensors to read in the file when the filename field
    // changes.
    // Sensors are used instead of field to field connections or raw
    // notification so that the fields can still be attached to/from
    // other fields.
    nameChangedSensor = new SoFieldSensor(nameChangedCB, this);
    nameChangedSensor->setPriority(0);
    nameChangedSensor->attach(&name);

    readOK = TRUE;
    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoFile::~SoFile()
//
////////////////////////////////////////////////////////////////////////
{
    delete nameChangedSensor;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Return a copy of our hidden children.
//
// Use: public

SoGroup *
SoFile::copyChildren() const
//
////////////////////////////////////////////////////////////////////////
{
    // Create a new SoGroup with our children, and return a copy of
    // it. This will ensure that connections are copied properly.
    SoGroup *holder = new SoGroup;
    holder->ref();

    for (int i = 0; i < children.getLength(); i++)
	holder->addChild(children[i]);

    SoGroup *result = (SoGroup *) holder->copy(TRUE);

    holder->unref();

    return result;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Copies the contents of the given node into this instance.
//
// Use: protected, virtual

void
SoFile::copyContents(const SoFieldContainer *fromFC, SbBool copyConnections)
//
////////////////////////////////////////////////////////////////////////
{
    // Detach sensor temporarily so we do not read the file again
    nameChangedSensor->detach();

    // Copy the usual stuff
    SoNode::copyContents(fromFC, copyConnections);

    // Copy the kids
    const SoFile *fromGroup = (const SoFile *) fromFC;
    SoChildList *fromChildren = fromGroup->getChildren();
    for (int i = 0; i < fromChildren->getLength(); i++) {

	// If this node is being copied, it must be "inside" (see
	// SoNode::copy() for details.) Therefore, all of its children
	// must be inside, as well.
	SoNode *fromKid = (*fromChildren)[i];
	SoNode *kidCopy = (SoNode *) findCopy(fromKid, copyConnections);

#ifdef DEBUG
	if (kidCopy == NULL)
	    SoDebugError::post("(internal) SoFile::copyContents",
			       "Child %d has not been copied yet", i);
#endif /* DEBUG */

	children.append(kidCopy);
    }
    
    // Reattach sensor
    nameChangedSensor->attach(&name);
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads into instance of SoFile. Returns FALSE on error.
//
// Use: protected

SbBool
SoFile::readInstance(SoInput *in, unsigned short flags)
//
////////////////////////////////////////////////////////////////////////
{
    // Detach sensor temporarily
    nameChangedSensor->detach();

    // Read field info as usual.
    if (! SoNode::readInstance(in, flags))
	readOK = FALSE;

    // If file name is default, there's a problem, since the default
    // file name is not a valid one
    else if (name.isDefault()) {
	SoReadError::post(in, "\"name\" field of SoFile node was never set");
	readOK = FALSE;
    }
    else {
	// Call nameChangedCB to read in children.  There is a really
	// cool bug that occurs if we let the sensor do this for us.
	// The sensor is called right after notification, in
	// processImmediateQueue.  It would then call nameChanged,
	// which calls SoDB::read, which sets up the directory search
	// path.  If there is another File node in that directory
	// search path, its name field will be set, but, since we are
	// already in the middle of a processImmediateQueue, its field
	// sensor isn't called right away.  The SoDB::read returns,
	// removing the directory it added to the search path,
	// nameChanged returns, and THEN the field sensor for the
	// inner File node goes off.  But, by then it is too late--
	// the directory search path no longer contains the directory
	// of the containing File node.
	nameChangedCB(this, NULL);
    }

    // Reattach sensor
    nameChangedSensor->attach(&name);

    return readOK;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback that reads in the file when the name field changes.
//
// Use: static, internal

void
SoFile::nameChangedCB(void *data, SoSensor *)
//
////////////////////////////////////////////////////////////////////////
{
    SoFile *f = (SoFile *)data;

    f->children.truncate(0);

    SoInput in;
    const char *filename = f->name.getValue().getString();
    
    // Open file
    f->readOK = TRUE;
    if (! in.openFile(filename, TRUE)) {
	f->readOK = FALSE;
	SoReadError::post(&in, "Can't open included file \"%s\" in File node",
			  filename);
    }

    if (f->readOK) {
	SoNode	*node;

	// Read children from opened file.

	while (TRUE) {
	    if (SoDB::read(&in, node)) {
		if (node != NULL)
		    f->children.append(node);
		else
		    break;
	    }
	    else
		f->readOK = FALSE;
	}
	in.closeFile();
    }
    // Note: if there is an error reading one of the children, the
    // other children will still be added properly...
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns pointer to children.
//
// Use: internal

SoChildList *
SoFile::getChildren() const
//
////////////////////////////////////////////////////////////////////////
{
    return (SoChildList *) &children;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements typical traversal.
//
// Use: extender public

void
SoFile::doAction(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    int		numIndices;
    const int	*indices;

    if (action->getPathCode(numIndices, indices) == SoAction::IN_PATH)
	children.traverse(action, 0, indices[numIndices - 1]);

    else
	children.traverse(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does the callback action
//
// Use: extender

void
SoFile::callback(SoCallbackAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoFile::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does the GL render action
//
// Use: extender

void
SoFile::GLRender(SoGLRenderAction *action)

////////////////////////////////////////////////////////////////////////
{
    SoFile::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does the get bounding box action.  This takes care of averaging
//    the centers of all children to get a combined center.
//
// Use: extender

void
SoFile::getBoundingBox(SoGetBoundingBoxAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SbVec3f	totalCenter(0,0,0);
    int		numCenters = 0;
    int		numIndices;
    const int	*indices;
    int		lastChild;

    if (action->getPathCode(numIndices, indices) == SoAction::IN_PATH)
	lastChild = indices[numIndices - 1];
    else
	lastChild = children.getLength() - 1;

    for (int i = 0; i <= lastChild; i++) {
	children.traverse(action, i, i);
	if (action->isCenterSet()) {
	    totalCenter += action->getCenter();
	    numCenters++;
	    action->resetCenter();
	}
    }
    // Now, set the center to be the average:
    if (numCenters != 0)
	action->setCenter(totalCenter / numCenters, FALSE);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does the handle event thing
//
// Use: extender

void
SoFile::handleEvent(SoHandleEventAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoFile::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Pick.
//
// Use: extender

void
SoFile::pick(SoPickAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoFile::doAction(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements get matrix action.
//
// Use: extender

void
SoFile::getMatrix(SoGetMatrixAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    int		numIndices;
    const int	*indices;

    // Only need to compute matrix if group is a node in middle of
    // current path chain or is off path chain (since the only way
    // this could be called if it is off the chain is if the group is
    // under a group that affects the chain).

    switch (action->getPathCode(numIndices, indices)) {

      case SoAction::NO_PATH:
	break;

      case SoAction::IN_PATH:
	children.traverse(action, 0, indices[numIndices - 1]);
	break;

      case SoAction::BELOW_PATH:
	break;

      case SoAction::OFF_PATH:
	children.traverse(action);
	break;
    }
}
