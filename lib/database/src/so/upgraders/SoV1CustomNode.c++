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
 |	SoV1CustomNode
 |
 |   Author(s)		: Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/misc/upgraders/SoV1CustomNode.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/nodes/SoInfo.h>

SO_NODE_SOURCE(SoV1CustomNode);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoV1CustomNode::SoV1CustomNode()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoV1CustomNode);

    SO_NODE_ADD_FIELD(className,  (""));
    SO_NODE_ADD_FIELD(fields,     (""));
    SO_NODE_ADD_FIELD(customData, (""));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoV1CustomNode::~SoV1CustomNode()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Create a version 2.0 SoCustomNode.
//
// Use: private

SbBool
SoV1CustomNode::upgrade(SoInput *in, const SbName &refName,
		    SoBase *&result)
//
////////////////////////////////////////////////////////////////////////
{
    // First, read in fields:
    const SoFieldData	*fieldData = getFieldData();
    SbBool notBuiltIn; // Not used
    if (! fieldData->read(in, this, FALSE, notBuiltIn))
	return FALSE;

    // Custom nodes must use one of the 'real' classes (stored in the
    // className field) to do the conversion, so search from
    // most-derived to least-derived to find an appropriate converter
    // and call convert() on it.  If there is no upgrader for any
    // subclasses, spit out an error and return NULL.

    SoV1CustomNode *upgrader = NULL;
    int i;

    for (i = className.getNum()-1; i >= 0 && upgrader == NULL; i--) {
	upgrader = (SoV1CustomNode *)
	    SoUpgrader::getUpgrader(className[i], 1.0);
    }
    if (upgrader == NULL) {
	// Not found, warning, and return a group with an Info node:
#ifdef DEBUG
	SoDebugError::postWarning("SoV1CustomNode::upgrade",
			   "No upgrader found for class %s\n",
			   className[className.getNum()-1].getString());
#endif
	SoGroup *g = SO_UPGRADER_CREATE_NEW(SoGroup);
	result = g;

	// Add to dictionary
	if (! (!refName))
	    in->addReference(refName, result);

	// Add an Info node so the group can be found later:
	SoInfo *info = SO_UPGRADER_CREATE_NEW(SoInfo);
	SbString infoString("Group converted from CustomNode class ");
	infoString += className[className.getNum()-1].getString();
	info->string = infoString;
	g->addChild(info);

	readChildren(in);
	for (int i = 0; i < getNumChildren(); i++)
	    g->addChild(getChild(i));
	
	return TRUE;
    }
    
    upgrader->ref();

    // Copy fields into upgrader:
    upgrader->className.setValues(0, className.getNum(),
				  className.getValues(0));
    upgrader->fields.setValues(0, fields.getNum(),
			       fields.getValues(0));
    upgrader->customData.setValues(0, customData.getNum(),
				   customData.getValues(0));

    result = upgrader->createNewNode();
    
    // Add to dictionary
    if (! (!refName))
	in->addReference(refName, result);
    
    // Read children into upgrader:
    upgrader->readChildren(in);
    
    // Set fields
    for (i = 0; i < fields.getNum(); i++) {
	upgrader->set(fields[i].getString(), in);
    }
    
    // Now, let each subclass have a chance to interpret its custom
    // data:
    for (i = 0; i < className.getNum(); i++) {
	SoV1CustomNode *base = (SoV1CustomNode *)
	    SoUpgrader::getUpgrader(className[i], 1.0);
	if (base) {
	    base->ref();
	    base->interpretCustomData(upgrader, i);
	    base->unref();
	}
    }
    
    upgrader->setUpNewNode((SoNode *)result);

    upgrader->unref();

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Create the V2.0 node to replace this node
//
// Use: public

SoNode *
SoV1CustomNode::createNewNode()
//
////////////////////////////////////////////////////////////////////////
{
    SoDebugError::post("SoV1CustomNode::createNewNode",
		       "Subclasses must implement createNewNode!");
    return NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This method is called to let subclass instances do whatever they
//    have to do based on the info in the customData field value with
//    the given index after the given instance has been read. This
//    default does nothing.
//
// Use: public

void
SoV1CustomNode::interpretCustomData(SoV1CustomNode *, int ) const
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Virtual method to setUp the new node.  Default does nothing.
//
// Use: public

void
SoV1CustomNode::setUpNewNode(SoNode *)
//
////////////////////////////////////////////////////////////////////////
{
}

