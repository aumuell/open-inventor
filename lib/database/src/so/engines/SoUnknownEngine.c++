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
 * Copyright (C) 1990,91,92,93   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.2 $
 |
 |   Classes:
 |      SoUnknownEngine
 |
 |   Author(s)          : Dave Immel, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <stdlib.h>
#include <Inventor/SoInput.h>
#include <Inventor/SoOutput.h>
#include <Inventor/fields/SoField.h>
#include <Inventor/engines/SoOutputData.h>
#include <SoUnknownEngine.h>

// Can't use regular SO_ENGINE_SOURCE macro because of special
// per-instance input and output data:
SO__ENGINE_VARS(SoUnknownEngine);

SoType
SoUnknownEngine::getTypeId() const
{
    return classTypeId;
}
const SoFieldData *
SoUnknownEngine::getFieldData() const
{
    return instanceInputData;
}
const SoEngineOutputData *
SoUnknownEngine::getOutputData() const
{
    return instanceOutputData;
}
void *
SoUnknownEngine::createInstance()
{
    return (void *)(new SoUnknownEngine);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoUnknownEngine::SoUnknownEngine()
//
////////////////////////////////////////////////////////////////////////
{
    SO_ENGINE_CONSTRUCTOR(SoUnknownEngine);

    className = NULL;
    instanceInputData = new SoFieldData(inputData);
    instanceOutputData = new SoEngineOutputData(outputData);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoUnknownEngine::~SoUnknownEngine()
//
////////////////////////////////////////////////////////////////////////
{
    //
    // The fields of the unknown engine have been allocated by the SoFieldData
    // class.  The unknown engine will delete them here, because the field
    // data does not have enough information to know where its fields are
    // stored.  This could be redesigned.
    //
    SoFieldList fieldList;
    int         numFields = getFields(fieldList);

    for (int i=0; i<numFields; i++)
        delete fieldList[i];

    // Delete the Engine Outputs that have also been allocated.
    SoEngineOutputList outputList;
    int         numOutputs = getOutputs(outputList);

    for (int j=0; j<numOutputs; j++)
        delete outputList[j];

    if (className) free((void *)className);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Set the class name of the unknown engine
//
// Use: public

void
SoUnknownEngine::setClassName( const char *name )

//
////////////////////////////////////////////////////////////////////////
{
    className = strdup( name );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Evaluation routine; does nothing
//
// Use: private

void
SoUnknownEngine::evaluate()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Copies the contents of the given engine into this instance.
//
// Use: protected, virtual

void
SoUnknownEngine::copyContents(const SoFieldContainer *fromFC,
			      SbBool copyConnections)
//
////////////////////////////////////////////////////////////////////////
{
    // Make sure the copy has the correct class name
    const SoUnknownEngine *fromUnk = (const SoUnknownEngine *) fromFC;
    setClassName(fromUnk->className);

    // For each input in the original engine, create a new input and add
    // it to the new engine

    // NOTE: We can't use SoEngine::copyContents() to copy the field
    // data, since that uses SoFieldData::overlay(), which assumes the
    // fields have the same offsets in both engines. Instead, we just
    // copy the field values ourselves.

    const SoFieldData *fromData = fromUnk->getFieldData();
    SoFieldData  *toData	= (SoFieldData *) getFieldData();
    int i;
    for (i = 0; i < fromData->getNumFields(); i++) {

	SoField      *fromField	= fromData->getField(fromUnk, i);
        const SbName fieldName	= fromData->getFieldName(i);
        SoType       fieldType	= fromField->getTypeId();
        SoField      *toField	= (SoField *) (fieldType.createInstance());

        toField->enableNotify(FALSE);
        toField->setContainer(this);
        toField->setDefault(TRUE);
        toField->enableNotify(TRUE);

        toData->addField(this, fieldName.getString(), toField);

	toField->setContainer(this);
	toField->copyFrom(*fromField);
	toField->setIgnored(fromField->isIgnored());
	toField->setDefault(fromField->isDefault());
	toField->fixCopy(copyConnections);
	if (fromField->isConnected() && copyConnections)
	    toField->copyConnection(fromField);
    }

    // Copy the outputs
    SoEngineOutputData *toOutData = (SoEngineOutputData *) getOutputData();

    SoEngineOutputList outList;
    fromUnk->getOutputs(outList);

    for(i = 0; i < outList.getLength(); i++) {
        SoEngineOutput *newOut = new SoEngineOutput;
        const SoType outType = outList[i]->getConnectionType();
	SbName outName;
        getOutputName( outList[i], outName );
	toOutData->addOutput(this, outName.getString(), newOut, outType);
	newOut->setContainer(this);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Called by SoBase::write to get the class name that should be
//    written to file.
//
// Use: internal

const char *
SoUnknownEngine::getFileFormatName() const
//
////////////////////////////////////////////////////////////////////////
{
    return className;
}
