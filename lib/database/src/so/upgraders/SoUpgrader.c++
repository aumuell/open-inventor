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
 |	SoUpgrader
 |
 |   Author(s)		: Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <assert.h>
#include <Inventor/SoInput.h>
#include <Inventor/SbDict.h>
#include <Inventor/misc/upgraders/SoUpgrader.h>

SO_NODE_ABSTRACT_SOURCE(SoUpgrader);

// Static variables:
SbDict *SoUpgrader::upgradeDictV1 = NULL;
SbDict *SoUpgrader::upgradeDictV2 = NULL;

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoUpgrader::SoUpgrader()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoUpgrader);

    isGroup = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoUpgrader::~SoUpgrader()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
//  Return either the V1.0 or V2.0 dictionary
//
SbDict *
SoUpgrader::getUpgradeDict(float version)
//
////////////////////////////////////////////////////////////////////////
{
    SbDict *upgradeDict = NULL;
    if (version == 1.0)
	upgradeDict = upgradeDictV1;
    else if (version == 2.0)
	upgradeDict = upgradeDictV2;

    return upgradeDict;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Register a converter with the converter dictionary.
//
// Use: protected, static

void
SoUpgrader::registerUpgrader(const SoType &type, const SbName &className,
			     float version)
//
////////////////////////////////////////////////////////////////////////
{
    const char *str = className.getString();

#ifdef DEBUG
    // There are 1.0 and 2.0 upgraders right now...
    if (version != 1.0 && version != 2.0) {
	SoDebugError::post("SoUpgrader::register", "Cannot handle "
			   "converters from version %f, can only "
			   "handle converters from version 1.0 or 2.0",
			   version);
    }
#endif

    SbDict *upgradeDict = getUpgradeDict(version);
    assert(upgradeDict != NULL);
    	
#ifdef DEBUG
    void *t;
    if (upgradeDict->find((unsigned long)str, t) != 0) {
	SoDebugError::post("SoUpgrader::register", "Upgrader already "
			   "registered for class %s", str);
    }
#endif

    upgradeDict->enter((unsigned long)str, (void *)&type);    
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Looks in the dictionary to figure out if a converter exists...
//
// Use: public, static

SoUpgrader *
SoUpgrader::getUpgrader(const SbName &className, float version)
//
////////////////////////////////////////////////////////////////////////
{
    // There are 1.0 and 2.0 upgraders right now...
    if (version != 1.0 && version != 2.0) return NULL;
    
    // Look in dictionary for converter type:
    void *t;
    if (! getUpgradeDict(version)->find((unsigned long) className.getString(), t))
	return NULL;

    SoType *type = (SoType *) t;
    
    return (SoUpgrader *)type->createInstance();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Does the grunt work of reading in the old node-- relies on
//    createNewNode() to actually create a node or scene graph that
//    can be added to the scene.  Returns TRUE if everything was
//    successful.
//
// Use: extender, virtual

SbBool
SoUpgrader::upgrade(SoInput *in, const SbName &refName, SoBase *&result)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool returnValue = TRUE;

    // Read in fields:
    const SoFieldData	*fieldData = getFieldData();
    SbBool notBuiltIn; // Not used
    if (! fieldData->read(in, this, FALSE, notBuiltIn))
	return FALSE;

    // Create instance:
    result = createNewNode();
    
    // Oops, something wrong...
    if (result == NULL) return FALSE;

    // Add to name dictionary:
    if (! (!refName))
	in->addReference(refName, result);
    
    // And read children, if node being upgraded is derived from group:
    if (isGroup && result->isOfType(SoGroup::getClassTypeId())) {
	returnValue = readChildren(in);
	if (returnValue) {

	    SoGroup *g = (SoGroup *)result;

	    // Turn off notification on result while we set it up:
	    SbBool notifySave = g->enableNotify(FALSE);

	    for (int i = 0; i < getNumChildren(); i++) {
		g->addChild(getChild(i));
	    }

	    // Re-enable notification.
	    g->enableNotify(notifySave);
	}
	// No need to remove children, we're about to be unreffed
	// anyway (by SoBase::readBaseInstance).
    }
    
    return returnValue;
}
