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
 |   $Revision $
 |
 |   Description:
 |	This file contains definitions of the SoTranSender and SoTranReceiver
 |	classes used for transcribing Inventor data. Transcribing is the
 |	process of packaging changes to a database and sending them over a
 |	"wire" to another database.
 |
 |   Classes:
 |	SoTranSender, SoTranReceiver
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_TRANSCRIBE_
#define  _SO_TRANSCRIBE_

#include <Inventor/misc/SoBasic.h>
#include <Inventor/SbString.h>
#include <Inventor/SbDict.h>

class SoGroup;
class SoInput;
class SoNode;
class SoOutput;

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoTranSender
//
// This stores database changes that are to be transcribed. The
// changes are written to an SoOutput instance allocated and managed
// by the caller.
//
// These database change routines are supported:
//
//   INSERT node [parent n]
//	Creates and adds a node to other database. With 1 argument,
//	the node is added as the last child of the root of the
//	database. With 3 arguments, the node is added as the nth
//	child of the given parent. (The parent may be NULL to
//	indicate the root.) Note that if the inserted node is already
//	in the graph, a link is made from the parent to the existing
//	version of the node. (Therefore, if the new node is different
//	from the old one, any changes are lost.)
//
//   REMOVE parent n
//	Removes nth child from the given parent node. (The parent may
//	be NULL to indicate the root.)
//
//   REPLACE parent n newNode
//	This is exactly equivalent to "REMOVE parent n" followed by
//	"INSERT newNode parent n".
//
//   MODIFY node
//	Updates the field data for the given node to the new
//	contents. Note that this changes only field data; children of
//	groups are not affected, nor is any non-field instance data.
//
//////////////////////////////////////////////////////////////////////////////

class SoTranSender {
  public:

    // Constructor: takes pointer to SoOutput instance
    SoTranSender(SoOutput *output);

    // Destructor
    ~SoTranSender()				{}

    // Returns pointer to SoOutput
    SoOutput *		getOutput() const	{ return out; }

    // Database change routines
    void		insert(SoNode *node);
    // C-api: name=insertUnder
    void		insert(SoNode *node, SoNode *parent, int n);
    void		remove(SoNode *parent, int n);
    void		replace(SoNode *parent, int n, SoNode *newNode);
    void		modify(SoNode *node);

    // This is called to make sure the data is ready to send
    void		prepareToSend();

  private:
    SoOutput		*out;

    // Adding items to send
    void		addBytes(const void *bytes, size_t nBytes);
    void		addCommand(int command);
    void		addInt(int n);
    void		addNode(SoNode *node, SbBool addNames = TRUE);
    void		addNodeNames(const SoNode *root);
    void		addNodeRef(const SoNode *node);
    void		addString(const char *cmdString);

friend class SoTranReceiver;
};

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoTranReceiver
//
// An SoTranReceiver is used on the receiving end to interpret the
// data packaged up by an SoTranSender. It is given a root node that
// is the default place to add incoming nodes. The input for the
// receiver comes from an SoInput.
//
//////////////////////////////////////////////////////////////////////////////

class SoTranReceiver {

  public:

    // Constructor takes default root node
    SoTranReceiver(SoGroup *rootNode);

    // Destructor
    ~SoTranReceiver();

    // Interprets data from SoInput
    SbBool	interpret(SoInput *in);

  private:
    SoGroup	*root;
    SbDict	nameToEntryDict;	// Maps node keyname to SoTranDictEntry
    SbDict	nodeToNameDict;		// Maps node pointer to node keyname

    // Interprets one database change command (with given code) from stream.
    // Sets done to TRUE if end command was found. Returns T/F error status.
    SbBool	interpretCommand(int commandCode, SoInput *in, SbBool &done);

    // Gets a node and node names from the input
    SbBool	getNodeAndNames(SoInput *in, SoNode *&node);

    // Gets node from input
    SbBool	getNode(SoInput *in, SoNode *&root);

    // Recursively gets node names and sets up dictionaries.
    SbBool	getNodeNames(SoInput *in, SoNode *root,
			     SbBool lookForNode, SoNode *&oldRoot);

    // Gets reference to a node, looks it up in dictionary, returns
    // node pointer.
    SbBool	getNodeReference(SoInput *in, SoNode *&node);

    // Removes reference to node in dictionaries, recursively.
    void	removeNodeReferences(SoNode *node);

    // Adds an entry to the dictionaries
    void	addEntry(SoNode *node, SbName &name);

    // Deletes (frees up) an entry from the nodeDict
    static void	deleteDictEntry(unsigned long key, void *value);

    // Returns 

};

#endif /* _SO_TRANSCRIBE_ */

