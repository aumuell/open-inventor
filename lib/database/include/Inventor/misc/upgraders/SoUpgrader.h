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
 |	This file defines the SoUpgrader class, which is used to
 |      read in and convert old nodes into new nodes.
 |
 |   Author(s)		: Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_UPGRADER_
#define  _SO_UPGRADER_

#include <Inventor/nodes/SoGroup.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoUpgrader
//
//////////////////////////////////////////////////////////////////////////////

class SbDict;
// C-api: abstract
// C-api: prefix=SoUpgr
class SoUpgrader : public SoGroup {

    SO_NODE_ABSTRACT_HEADER(SoUpgrader);

  public:
    // Find out if an upgrader exists for a specific class and a
    // specific version of the file format.
    // C-api: name=getUpgr
    static SoUpgrader 	*getUpgrader(const SbName &className,
				     float fileFormatVersion);
  protected:
    // Register a converter for a specific class and a specific file
    // format.
    // C-api: name=regUpgr
    static void		registerUpgrader(const SoType &type,
					 const SbName &className,
					 float version);
  SoINTERNAL public:
    static void		initClass();

    // Initialize ALL upgraders.
    static void		initClasses();

    // This is the key method that reads in fields, calls the
    // createNewNode() method (which is responsible for looking at the
    // fields read and setting the appropriate fields in the new
    // node), and then reads in and adds children to the new node if
    // it is derived from SoGroup.
    virtual SbBool	upgrade(SoInput *in, const SbName &refName,
				SoBase *&result);

  protected:
    // This is set to TRUE in the SoUpgrader constructor. A subclass
    // should set this to FALSE in its constructor if its
    // createNewNode() method returns a group node but the node being
    // upgraded is not really a group; it keeps children from being
    // read.
    SbBool		isGroup;

    // Subclasses MUST define this...
    virtual SoNode	*createNewNode() = 0;

    // Constructor.  Subclass constructors should be protected or
    // private (upgraders are created on demand by SoDB, and should
    // never be created using 'new').
    SoUpgrader();

    virtual ~SoUpgrader();

  private:
    // The dictionaries associating class names with upgrader types.
    static SbDict	*getUpgradeDict(float version);
    static SbDict	*upgradeDictV1;
    static SbDict	*upgradeDictV2;
};

// Macro to initialize upgrader classes.  Pass in the name of the
// upgrader's class.
#define SO_UPGRADER_INIT_CLASS(className)				      \
    SO__NODE_INIT_CLASS(className, SO__QUOTE(className), SoUpgrader)

// Macro to register a converter.  This should be put in the initClass
// method just after SO_UPGRADER_INIT_CLASS.  This also marks the upgrader
// class as internal (upgraders are transient, and should only exist
// while reading an old Inventor file-- you shouldn't be able to just
// create one using SoType::createInstance).  Pass in the name of the
// class being upgraded (e.g. if this is MyV1Upgrader that upgrades
// the MyClass node, you would SO_REGISTER_UPGRADER(MyClass, 1.0)).
#define SO_REGISTER_UPGRADER(oldClassName, version)			      \
	do {								      \
	    classTypeId.makeInternal();					      \
	    registerUpgrader(classTypeId, SO__QUOTE(oldClassName), version);  \
	} while (0)

// Shorthand macro to copy a field value (and flags) from an upgrader
// instance into a real node. The first version can be used if the
// fields have different names; the second assumes they're the same.
#define SO_UPGRADER_COPY_FIELD2(oldName, newName, nodeVar)		      \
	do {								      \
	    if (! oldName.isDefault())					      \
		nodeVar->newName = oldName;				      \
	    if (oldName.isIgnored())					      \
		nodeVar->newName.setIgnored(TRUE);			      \
	} while (0)

#define SO_UPGRADER_COPY_FIELD(fieldName, nodeVar)			      \
	SO_UPGRADER_COPY_FIELD2(fieldName, fieldName, nodeVar)

// Use this macro instead of calling "new" on the className. 
// This ensures that overridden types are created during an upgrade.
#define SO_UPGRADER_CREATE_NEW(className)				      \
	((className *) className::getClassTypeId().createInstance())

#endif /* _SO_UPGRADER_ */
