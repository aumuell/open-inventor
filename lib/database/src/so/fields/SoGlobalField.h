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
 * Copyright (C) 1990,91,92   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Description:
 |	This file contains the GlobalField class declaration.
 |      GlobalField is really only a special kind of container for
 |      ordinary SoFields.
 |      It is an internal class.
 |
 |   Classes:
 |	SoGlobalField
 |
 |   Author(s)		: Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_GLOBALFIELD_
#define  _SO_GLOBALFIELD_

#include <Inventor/SbDict.h>
#include <Inventor/fields/SoSFName.h>
#include <Inventor/fields/SoFieldContainer.h>
#include <Inventor/fields/SoFieldData.h>

class SoGlobalField : public SoFieldContainer {
    
  public:
    // Setup type information
    static void initClass();

    // Create a new global field with the given name and of the given
    // type, if one doesn't yet exists.  This returns NULL if one
    // can't be created for some reason. "alreadyExists" will be set
    // to TRUE if a global field of the same name and type already exists.
    static SoGlobalField *create(const SbName &name, SoType type,
				 SbBool &alreadyExists);

    // Find a global field with the given name.  This returns NULL if
    // there is none.
    static SoGlobalField *find(const SbName &name);

    // Called by SoBase::read to read a global field.
    static SoGlobalField *read(SoInput *in);

    // Write out a global field's innards.
    virtual void	writeInstance(SoOutput *out);

    // Change the name of a global field.
    void changeName(const SbName &newName);

    static SoType	getClassTypeId()	{ return classTypeId; }
    virtual SoType	getTypeId() const;

    SoField *		getMyField() const;
    virtual		SbName getName() const;

    // Override to always write out:
    virtual void	addWriteReference(SoOutput *out,
					  SbBool isFromField = FALSE);
  private:

    SoGlobalField(const SbName &, SoField *);
    ~SoGlobalField();
    
    SoSFName typeField;	/* The type of field this contains */

    SoType getType() const;	/* Convenience used internally */

    static SbDict	*nameDict;
    static SoType	classTypeId;

    SoField *value;	/* The field all this is for! */
    SoFieldData		*fieldData;	/* FieldData containing value */

    virtual const SoFieldData *	getFieldData() const;

friend class SoDB;
};    

#endif  /* _SO_GLOBALFIELD_ */
