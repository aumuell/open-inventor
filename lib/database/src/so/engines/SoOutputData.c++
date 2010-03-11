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
 |	SoEngineOutputData
 |
 |   Author(s)		: Paul S. Strauss, Nick Thompson, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SbString.h>
#include <Inventor/SoInput.h>
#include <Inventor/SoOutput.h>
#include <Inventor/engines/SoOutputData.h>
#include <Inventor/engines/SoEngine.h>
#include <Inventor/errors/SoDebugError.h>

// Syntax for reading/writing type information to files
#define OPEN_BRACE_CHAR		'['
#define CLOSE_BRACE_CHAR	']'
#define VALUE_SEPARATOR_CHAR	','

//////////////////////////////////////////////////////////////////////////////
//
//  This structure holds the name and offset of a output in an
//  SoEngineOutputData.  It is internal to Inventor.
//
//////////////////////////////////////////////////////////////////////////////

struct SoOutputEntry {
    SbName		name;		// Name of output
    int32_t		offset;		// Offset of output within object
    SoType type;			// Type of output
};

////////////////////////////////////////////////////////////////////////
//
// Description:
//    like a copy constructor, but takes a pointer
//
SoEngineOutputData::SoEngineOutputData(const SoEngineOutputData *from)
//
////////////////////////////////////////////////////////////////////////
{
    int			i;
    struct SoOutputEntry *fromOutput, *toOutput;

    if (from == NULL)
	return;

    for (i = 0; i < from->outputs.getLength(); i++) {

	fromOutput = (struct SoOutputEntry *) from->outputs[i];

	toOutput = new struct SoOutputEntry;
	*toOutput = *fromOutput;

	outputs.append((void *) toOutput);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.  Frees up entries.
//
SoEngineOutputData::~SoEngineOutputData()
//
////////////////////////////////////////////////////////////////////////
{
    for (int i = 0; i < outputs.getLength(); i++) {
	delete (struct SoOutputEntry *)outputs[i];
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds an output to current outputData.  Called inside SoEngine
//    subclass constructors, from SO_ENGINE_ADD_OUTPUT macro.
//
// Use: internal

void
SoEngineOutputData::addOutput(
  const SoEngine *defEngine,    // pointer to container
  const char *outputName,	// Name of output
  const SoEngineOutput *output, // Pointer to output in container
  SoType type)			// Type of output
//
////////////////////////////////////////////////////////////////////////
{
    struct SoOutputEntry *newOutput = new struct SoOutputEntry;

    newOutput->name   = outputName;
    newOutput->offset = (const char *) output - (const char *) defEngine;
    newOutput->type = type;

    outputs.append((void *) newOutput);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns name of output with given index in given object.
//
// Use: internal

const SbName &
SoEngineOutputData::getOutputName(int index) const
//
////////////////////////////////////////////////////////////////////////
{
    return ((SoOutputEntry *) outputs[index])->name;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns pointer to output with given index within given node instance.
//
// Use: internal

SoEngineOutput *
SoEngineOutputData::getOutput(const SoEngine *func, int index) const
//
////////////////////////////////////////////////////////////////////////
{
    // This generates a CC warning; there's not much we can do about it...
    return (SoEngineOutput *) ((char *) func +
				 ((SoOutputEntry *) outputs[index])->offset);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns index to output, given output and engine
//
// Use: internal

int
SoEngineOutputData::getIndex(const SoEngine *func, 
			       const SoEngineOutput *output) const
//
////////////////////////////////////////////////////////////////////////
{
    int offset = (const char *) output - (const char *) func;

    // Loop through the list looking for the correct offset:
    // (we'll assume this won't be very slow, since the list will
    // typically be very short):
    for (int i = 0; i < outputs.getLength(); i++) {
	SoOutputEntry *entry = (SoOutputEntry *)outputs[i];
	if (entry->offset == offset) return i;
    }

    // This should never happen.
    SoDebugError::post("(internal) SoEngineOutputData::getIndex",
		       "Did not find engineOutput");
    return 0;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns type of the output with given index
//
// Use: internal

const SoType &
SoEngineOutputData::getType(int index) const
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (index >= outputs.getLength())
	SoDebugError::post("SoEngineOutputData::getType",
			   "Trying to get type of output %d, but engine has "
			   "only %d outputs", index, outputs.getLength());
#endif /* DEBUG */
    return ((SoOutputEntry *)(outputs[index]))->type;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads output type info
//
// Use: internal

SbBool
SoEngineOutputData::readDescriptions(SoInput *in, SoEngine *object) const

//
////////////////////////////////////////////////////////////////////////
{
    SbBool gotChar;
    SbName fieldType, fieldName;
    char   c;

    // TRUE if reading an Inventor file pre-Inventor 2.1:
    SbBool	oldFileFormat = (in->getIVVersion() < 2.1f);

    // TRUE if reading binary file format:
    SbBool	isBinary = in->isBinary();

    unsigned short numDescriptions = 1<<15;  // Any huge number will work
    if (isBinary) {
	if (oldFileFormat) {
	    // Read and ignore "outputs" string:
	    SbString junk;
	    if (!in->read(junk) || junk != "outputs") return FALSE;

	    int num;  // NumDescriptions written as integer in old
		      // file format
	    if (!in->read(num)) return FALSE;
	    numDescriptions = (short) num;
	} else {
	    if (!in->read(numDescriptions))
		return FALSE;
	}
    }

    SbBool hadOutputsDefined = outputs.getLength() > 0;

    if (!isBinary) {
	if (! ((gotChar = in->read(c)) || c != OPEN_BRACE_CHAR))
	    return FALSE;
    }

    for (int i = 0; i < numDescriptions; i++) {

	// Check for closing brace:
	if (!isBinary) {
	    // Check for closing brace:
	    if (in->read(c) && c == CLOSE_BRACE_CHAR)
		return TRUE;
	    else in->putBack(c);
	}

	SbName type, outputName;
	if (!in->read(type, TRUE)) return FALSE;
	if (!in->read(outputName, TRUE)) return FALSE;

	SoType outputType = SoType::fromName(type);

	if (!hadOutputsDefined) {
	    // Only create outputs if none defined yet.

	    if (outputType.isBad())
		return FALSE;

	    // Create and initialize an instance of the output.
	    // Add it to the field data.
	    SoEngineOutput *output = new SoEngineOutput;
	    output->setContainer(object);

	    // Cast const away to add info:
	    SoEngineOutputData *This = (SoEngineOutputData *)this;
	    This->addOutput(object, outputName.getString(),
			    output, outputType);
	}
#ifdef DEBUG
	else {
	    // Check to make sure specification matches reality:
	    SoEngineOutput *o = object->getOutput(outputName);
	    if (o == NULL) {
		SoDebugError::post("SoEngineOutputData::readDescriptions",
				   "%s does not have a field named %s",
				   object->getTypeId().getName().getString(),
				   outputName.getString());
	    }
	    else if (!outputType.isDerivedFrom(o->getConnectionType())) {
		SoDebugError::post("SoFieldData::readFieldDescriptions",
			   "%s.%s is type %s, NOT type %s",
			   object->getTypeId().getName().getString(),
			   outputName.getString(),
			   o->getConnectionType().getName().getString(),
			   type.getString());
	    }
	}
#endif
	if (!isBinary) {
	    // Better get a ',' or a ']' at this point:
            if (! in->read(c))
                return FALSE;
            if (c != VALUE_SEPARATOR_CHAR) {
		if (c == CLOSE_BRACE_CHAR)
		    return TRUE;
		else return FALSE;
	    }
	    // Got a ',', continue reading
	}
    }

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads output type info
//
// Use: internal

void
SoEngineOutputData::writeDescriptions(SoOutput *out, SoEngine *object) const

//
////////////////////////////////////////////////////////////////////////
{
    SbBool isBinary = out->isBinary();

    if (!isBinary) {
        out->indent();
	out->write("outputs");
        out->write(' ');
        out->write(OPEN_BRACE_CHAR);
        out->write(' ');
        out->incrementIndent(2);
    } else {
	unsigned short numDescriptions = getNumOutputs();
	out->write(numDescriptions);
    }

    // For each output, write out the output type name and output name.
    int i = 0;
    int numWritten = 0;
    for (i = 0; i < getNumOutputs(); i++) {

	SoEngineOutput *output = getOutput(object, i);

        SoType  type = output->getConnectionType();

        out->write(type.getName().getString());
        if (! isBinary)
            out->write(' ');
        out->write(getOutputName(i).getString());

        // Format nice and pretty for ascii
        if (! isBinary) {
            if (i != getNumOutputs() - 1) {
                out->write(VALUE_SEPARATOR_CHAR);
		++numWritten;
		if ((numWritten%4) == 0) { // 4 pairs per line
		    out->write('\n');
		    out->indent();
		} else out->write(' ');
            }
        }
    }

    // Write out the closing brace of the field type information.
    if (! out->isBinary()) {
        out->write(' ');
        out->write(CLOSE_BRACE_CHAR);
        out->write('\n');
        out->decrementIndent(2);
    }
}

