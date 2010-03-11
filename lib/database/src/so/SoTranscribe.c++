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
 |	SoTranSender, SoTranReceiver
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/SoOutput.h>
#include <Inventor/misc/SoTranscribe.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/fields/SoFieldData.h>
#include <Inventor/nodes/SoGroup.h>

// Internal command codes
#define TRAN_INSERT	0
#define TRAN_INSERTP	1
#define TRAN_REMOVE	2
#define TRAN_REPLACE	3
#define TRAN_MODIFY	4
#define TRAN_END	5
#define TRAN_NUM	6	/* Number of transcription commands */

//////////////////////////////////////////////////////////////////////////////
//
// SoTranSender class
//
//////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor - takes SoOutput to write transcription to.
//
// Use: public

SoTranSender::SoTranSender(SoOutput *output)
//
////////////////////////////////////////////////////////////////////////
{
    out = output;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds INSERT command to output. 
//
// Use: public

void
SoTranSender::insert(SoNode *node)
//
////////////////////////////////////////////////////////////////////////
{
    node->ref();
    addCommand(TRAN_INSERT);
    addNode(node);
    node->unref();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds INSERTP command to output. 
//
// Use: public

void
SoTranSender::insert(SoNode *node, SoNode *parent, int n)
//
////////////////////////////////////////////////////////////////////////
{
    node->ref();
    if (parent != NULL)
	parent->ref();
    addCommand(TRAN_INSERTP);
    addNode(node);
    addNodeRef(parent);
    addInt(n);
    node->unref();
    if (parent != NULL)
	parent->unref();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds REMOVE command to output.
//
// Use: public

void
SoTranSender::remove(SoNode *parent, int n)
//
////////////////////////////////////////////////////////////////////////
{
    if (parent != NULL)
	parent->ref();
    addCommand(TRAN_REMOVE);
    addNodeRef(parent);
    addInt(n);
    if (parent != NULL)
	parent->unref();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds REPLACE command to output.
//
// Use: public

void
SoTranSender::replace(SoNode *parent, int n, SoNode *newNode)
//
////////////////////////////////////////////////////////////////////////
{
    if (parent != NULL)
	parent->ref();
    newNode->ref();
    addCommand(TRAN_REPLACE);
    addNodeRef(parent);
    addInt(n);
    addNode(newNode);
    if (parent != NULL)
	parent->unref();
    newNode->unref();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds MODIFY command to output.
//
// Use: public

void
SoTranSender::modify(SoNode *node)
//
////////////////////////////////////////////////////////////////////////
{
    node->ref();
    addCommand(TRAN_MODIFY);
    addNodeRef(node);
    addNode(node, FALSE);
    node->unref();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This is called to make sure the data is ready to send. It adds
//    an END command to the output.
//
// Use: public

void
SoTranSender::prepareToSend()
//
////////////////////////////////////////////////////////////////////////
{
    addCommand(TRAN_END);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds string corresponding to command code to buffer.
//
// Use: private

void
SoTranSender::addCommand(int command)
//
////////////////////////////////////////////////////////////////////////
{
    out->write(command);

    if (! out->isBinary())
	out->write('\n');
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds data for node to output. Uses Inventor database write to do
//    the deed. If addNames is TRUE (the default), also adds names.
//
// Use: private

void
SoTranSender::addNode(SoNode *node, SbBool addNames)
//
////////////////////////////////////////////////////////////////////////
{
    SoWriteAction	wa(out);

    wa.apply(node);

    // Add a list of node "names" in order
    if (addNames)
	addNodeNames(node);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds a reference to the given node to the output. The reference
//    is written as a string containing the address of the node. The
//    actual address doesn't matter - just that all nodes have unique
//    references.
//
// Use: private

void
SoTranSender::addNodeRef(const SoNode *node)
//
////////////////////////////////////////////////////////////////////////
{
    // Convert the node pointer to a string
    char	s[16];

    sprintf(s, "%#x", node);

    out->write(s);

    if (! out->isBinary())
	out->write('\n');
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds an integer to the buffer.
//
// Use: private

void
SoTranSender::addInt(int n)
//
////////////////////////////////////////////////////////////////////////
{
    out->write(n);

    if (! out->isBinary())
	out->write('\n');
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds a dictionary of references for all nodes under the given
//    root to the output. This traverses the graph in the usual style;
//    the receiver must traverse in the same manner to keep references
//    correct.
//
// Use: private

void
SoTranSender::addNodeNames(const SoNode *root)
//
////////////////////////////////////////////////////////////////////////
{
    addNodeRef(root);

    if (root->isOfType(SoGroup::getClassTypeId())) {

	const SoGroup	*group = (const SoGroup *) root;
	int		i;

	for (i = 0; i < group->getNumChildren(); i++)
	    addNodeNames(group->getChild(i));
    }
}

//////////////////////////////////////////////////////////////////////////////
//
// SoTranReceiver class
//
//////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
//
// This structure is added to the nameToEntryDict dictionary that
// correlates unique node names to node pointers. The unique names are
// created by printing the pointer to the transcribed node into a
// string buffer. This structure stores a pointer to the node on the
// receiving end. It also stores a reference count for that node so we
// can tell how many times it has been added to the dictionary. When
// this gets decremented to 0, the dictionary entry is removed, so we
// can free up some memory. Without this, transcription just uses up
// memory because new nodes keep getting inserted and none get removed
// from the dictionary.
//
///////////////////////////////////////////////////////

struct SoTranDictEntry {
    SoNode	*node;		// Pointer to node
    int32_t	refCount;	// Number of times node is added to dict
};

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor - takes default root node.
//
// Use: public

SoTranReceiver::SoTranReceiver(SoGroup *rootNode)
//
////////////////////////////////////////////////////////////////////////
{
    char	s[16];

    // Add root to dictionaries with name for NULL, since that is how
    // the root is referred to.
    sprintf(s, "%#x", NULL);
    SbName	name(s);
    addEntry(rootNode, name);

    root = rootNode;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: public

SoTranReceiver::~SoTranReceiver()
//
////////////////////////////////////////////////////////////////////////
{
    // Free up SoTranDictEntry instances stored in dictionary
    nameToEntryDict.applyToAll(SoTranReceiver::deleteDictEntry);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Interprets transcription commands from the given SoInput.
//    Returns FALSE on error.
//
// Use: public

SbBool
SoTranReceiver::interpret(SoInput *in)
//
////////////////////////////////////////////////////////////////////////
{
    int		commandCode;
    SbBool	done = FALSE;

    // Run through commands in buffer, changing local DB
    do {

	if (! in->read(commandCode))
	    return FALSE;

	// Process command
	if (! interpretCommand(commandCode, in, done)) {
	    SoDebugError::post("SoTranReceiver::interpret",
			       "in command \"%d\"", commandCode);
	    return FALSE;
	}

    } while (! done);

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Interprets one database change command (with given code) from
//    input. Sets done to TRUE if end command was found. Returns T/F
//    error status.
//
// Use: private

SbBool
SoTranReceiver::interpretCommand(int commandCode, SoInput *in, SbBool &done)
//
////////////////////////////////////////////////////////////////////////
{
    SoNode		*node, *parentNode, *nodeRef;
    SoGroup		*parent;
    const SoFieldData	*fd;
    int			i;

    switch (commandCode) {

      case TRAN_INSERT:

	if (! getNodeAndNames(in, node))		// New node and names
	    return FALSE;

	root->addChild(node);
	node->unref();
	break;

      case TRAN_INSERTP:

	if (! getNodeAndNames(in, node)		||	// New node and names
	    ! getNodeReference(in, parentNode)	||	// Parent node
	    ! in->read(i))				// Child index
	    return FALSE;

	parent = (SoGroup *) parentNode;
	parent->insertChild(node, i);
	node->unref();
	break;

      case TRAN_REMOVE:

	if (! getNodeReference(in, parentNode) ||	// Parent of node
	    ! in->read(i))				// Child index
	    return FALSE;

	parent = (SoGroup *) parentNode;
	if (i < parent->getNumChildren()) {
	    // Remove references, recursively
	    removeNodeReferences(parent->getChild(i));
	    parent->removeChild(i);
	}

	break;

      case TRAN_REPLACE:

	if (! getNodeReference(in, parentNode)	||	// Parent of node
	    ! in->read(i)			||	// Child index
	    ! getNodeAndNames(in, node))		// New node and names
	    return FALSE;

	// Out with the old, in with the new
	parent = (SoGroup *) parentNode;

	if (i < parent->getNumChildren()) {
	    // Remove references, recursively
	    removeNodeReferences(parent->getChild(i));

	    parent->removeChild(i);
	    parent->insertChild(node, i);
	    node->unref();
	}

	break;

      case TRAN_MODIFY:

	if (! getNodeReference(in, nodeRef)	||	// Reference to node
	    ! getNode(in, node))			// New node definition
	    return FALSE;

	// Change node fields to new contents
	fd = nodeRef->getFieldData();
	if (fd != NULL)
	    fd->overlay(nodeRef, node, TRUE);

	// Get rid of new definition
	node->unref();

	break;

      case TRAN_END:
	done = TRUE;
	break;

      default:
	return FALSE;
    }

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads a node (and its childrem, if any) from the buffer,
//    returning the node. This also reads the node names data from the
//    buffer, updating the node dictionaries. Returns FALSE on error.
//
// Use: private

SbBool
SoTranReceiver::getNodeAndNames(SoInput *in, SoNode *&node)
//
////////////////////////////////////////////////////////////////////////
{
    SoNode	*oldNode;

    // Read node
    if (! getNode(in, node))
	return FALSE;

    // Read node names, looking for existing node with that name
    if (! getNodeNames(in, node, TRUE, oldNode))
	return FALSE;

    // If node already existed, get rid of new stuff and return old node
    if (oldNode != NULL) {
	node->unref();
	node = oldNode;
	node->ref();
    }
    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads a node (and its childrem, if any) from the buffer,
//    returning the node (after ref'ing it). Returns FALSE on error.
//
// Use: private

SbBool
SoTranReceiver::getNode(SoInput *in, SoNode *&node)
//
////////////////////////////////////////////////////////////////////////
{
    if (! SoDB::read(in, node))
	return FALSE;

    node->ref();

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads reference names for each node under root. Assumes that the
//    order of traversal is the same here as when sending the name. If
//    lookForNode is TRUE, it looks for the node in the dictionary. If
//    it is found, oldRoot is set to point to it. Otherwise, oldRoot
//    is set to NULL. If lookForNode is FALSE, this just recurses on
//    the children, if any, to keep the name-to-node correspondences
//    correct.
//
// Use: private

SbBool
SoTranReceiver::getNodeNames(SoInput *in, SoNode *root,
			     SbBool lookForNode, SoNode *&oldRoot)
//
////////////////////////////////////////////////////////////////////////
{
    SbName		name;
    void		*ptr;
    SoTranDictEntry	*e;

    // Read reference as SbName to get unique pointer
    if (! in->read(name))
	return FALSE;

    if (lookForNode) {

	// See if the node is already in the dictionary. If so,
	// increment the reference count and return the old root.
	if (nameToEntryDict.find((unsigned long) name.getString(), ptr)) {
	    e = (SoTranDictEntry *) ptr;
	    e->refCount++;

	    oldRoot = e->node;
	}

	// Otherwise, add a new entry
	else {
	    addEntry(root, name);
	    oldRoot = NULL;
	}
    }

    else
	oldRoot = NULL;

    if (root->isOfType(SoGroup::getClassTypeId())) {

	SoNode		*child;
	SoGroup		*group = (SoGroup *) root;
	int		i;

	for (i = 0; i < group->getNumChildren(); i++) {

	    // Recurse
	    if (! getNodeNames(in, group->getChild(i), oldRoot == NULL, child))
		return FALSE;

	    // If child node was already in dictionary, replace child
	    // of group with node from dictionary.
	    if (child != NULL)
		group->replaceChild(i, child);
	}
    }

    return TRUE;
}    

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads a node reference from the input and returns (in node) a
//    pointer to the node referenced by it. If no such node is found,
//    NULL is returned. Returns FALSE on error.
//
// Use: private

SbBool
SoTranReceiver::getNodeReference(SoInput *in, SoNode *&node)
//
////////////////////////////////////////////////////////////////////////
{
    SbName	name;
    void	*ptr;

    // Read reference as SbName to get unique pointer
    if (! in->read(name))
	return FALSE;

    // Look up pointer in dictionary
    if (nameToEntryDict.find((unsigned long) name.getString(), ptr))
	node = ((SoTranDictEntry *) ptr)->node;
    else {
	node = NULL;
	return FALSE;
    }

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Removes reference to node in dictionaries. The passed node is a
//    node in the graph on the receiving side. If there are no more
//    references left, removes dictionary entry completely.
//    (Otherwise, just decrements dictionary entry reference count.)
//    This is a recursive procedure.
//
// Use: private

void
SoTranReceiver::removeNodeReferences(SoNode *node)
//
////////////////////////////////////////////////////////////////////////
{
    void		*ptr;
    const char		*nameString;
    SoTranDictEntry	*e;

    // Find the name of the node, using the name dictionary
    nodeToNameDict.find((unsigned long) node, ptr);
    nameString = (const char *) ptr;

    // Find the dictionary entry for the node, using the name
    SbName		name(nameString);
    nameToEntryDict.find((unsigned long) name.getString(), ptr);
    e = (SoTranDictEntry *) ptr;

    // Decrement the reference count and check for 0
    if (--e->refCount == 0) {

	// This node is going away. Remove its dictionary entries and
	// free up the SoTranDictEntry
	nameToEntryDict.remove((unsigned long) name.getString());
	nodeToNameDict.remove((unsigned long) node);
	e->node->unref();
	delete e;

	// Recurse on its children, if it has any
	if (node->isOfType(SoGroup::getClassTypeId())) {

	    const SoGroup	*group = (const SoGroup *) node;
	    int		i;

	    for (i = 0; i < group->getNumChildren(); i++)
		removeNodeReferences(group->getChild(i));
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds an entry to the dictionaries.
//
// Use: private

void
SoTranReceiver::addEntry(SoNode *node, SbName &name)
//
////////////////////////////////////////////////////////////////////////
{
    SoTranDictEntry	*e = new SoTranDictEntry;

    e->node = node;
    e->node->ref();
    e->refCount = 1;

    nameToEntryDict.enter((unsigned long) name.getString(), (void *) e);
    nodeToNameDict.enter((unsigned long) node, (void *) name.getString());
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Deletes (frees up) an entry from the nameToEntryDict.
//
// Use: private

void
SoTranReceiver::deleteDictEntry(unsigned long, void *value)
//
////////////////////////////////////////////////////////////////////////
{
    SoTranDictEntry	*e = (SoTranDictEntry *) value;

    e->node->unref();
    delete e;
}
