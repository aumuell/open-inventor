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
 * Copyright (C) 1990,91,92   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Classes:
 |	SoEngine, SoEngineOutput
 |
 |   Author(s)		: Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SoDB.h>
#include <Inventor/SoOutput.h>
#include <Inventor/SoInput.h>
#include <Inventor/engines/SoEngine.h>
#include <Inventor/engines/SoFieldConverter.h>
#include <Inventor/engines/SoOutputData.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/fields/SoField.h>
#include <Inventor/fields/SoFieldData.h>
#include <Inventor/misc/SoNotification.h>

SoType	SoEngine::classTypeId;				// Type identifier

// Syntax for reading/writing type information to files
#define OPEN_BRACE_CHAR		'['
#define CLOSE_BRACE_CHAR	']'
#define VALUE_SEPARATOR_CHAR	','

//////////////////////////////////////////////////////////////////////
//
//	SoEngine class
//
//////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor.  Engines are assumed to need evaluation when
//    first created (this is probably being paranoid, since
//    needsEvaluation is set when a connection is made to a engine
//    output, but hi-ho...).
//
// Use: protected

SoEngine::SoEngine()
//
////////////////////////////////////////////////////////////////////////
{
    needsEvaluation = TRUE;
    notifying = FALSE;
#ifdef DEBUG
    if (! SoDB::isInitialized())
	SoDebugError::post("SoEngine::SoEngine",
			   "Cannot construct engines before "
			   "calling SoDB::init()");
#endif /* DEBUG */
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.  Called when its ref count goes to zero.
//
// Use: protected

SoEngine::~SoEngine()
//
////////////////////////////////////////////////////////////////////////
{
    // Note: the SoEngine destructor doesn't have to worry about
    // unhooking connections to its inputs or outputs for the
    // following reasons:
    // -- a engine will only be deleted when it's outputs are
    // disconnected from everything, since making a connection to an
    // output increments the engines reference count.  So outputs
    // are OK.
    // -- The inputs are taken care of by their destructors (which
    // call disconnect()) and by SoBase::~SoBase, which disconnects
    // any forward connections to the engine's input fields.

    // In the debug case, our output's destructors will make sure they
    // aren't connected to anything, and will complain if they are.
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns a list of outputs in this engine.
//
// Use: public

int
SoEngine::getOutputs(SoEngineOutputList &list) const
//
////////////////////////////////////////////////////////////////////////
{
    int				i;
    const SoEngineOutputData	*od = getOutputData();

    if (od == NULL)
	return 0;

    for (i = 0; i < od->getNumOutputs(); i++) {
	list.append(od->getOutput(this, i));
    }
    return od->getNumOutputs();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns a pointer to the output with the given name. If no such
//    output exists, NULL is returned.
//
// Use: public

SoEngineOutput *
SoEngine::getOutput(const SbName &outputName) const
//
////////////////////////////////////////////////////////////////////////
{
    int				i;
    const SoEngineOutputData	*od = getOutputData();

    if (od == NULL)
	return NULL;

    // Search outputs for one with given name
    for (i = 0; i < od->getNumOutputs(); i++)
	if (od->getOutputName(i) == outputName)
	    return od->getOutput(this, i);

    // Not found...
    return NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns (in outputName) the name of the output pointed to.
//    Returns FALSE if the output is not contained within the engine
//    instance.
//
// Use: public

SbBool
SoEngine::getOutputName(const SoEngineOutput *output,
			  SbName &outputName) const
//
////////////////////////////////////////////////////////////////////////
{
    int				i;
    const SoEngineOutputData	*od = getOutputData();

    if (od == NULL)
	return FALSE;

    // Search outputs for one with given pointer
    for (i = 0; i < od->getNumOutputs(); i++) {
	if (od->getOutput(this, i) == output) {
	    outputName = od->getOutputName(i);
	    return TRUE;
	}
    }

    // Not found...
    return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Creates and returns a copy of the engine. All connections to
//    inputs are copied as is (without copying what's at the other end).
//
// Use: public

SoEngine *
SoEngine::copy() const
//
////////////////////////////////////////////////////////////////////////
{
    // Set up an empty copy dictionary. The dictionary has to exist
    // for copying to work, even though this operation will never
    // store anything in it.
    initCopyDict();

    // Create a copy of this engine
    SoEngine *newEngine = (SoEngine *) getTypeId().createInstance();
    newEngine->ref();

    // Copy the contents
    newEngine->copyContents(this, TRUE);

    // Clean up
    copyDone();

    // Return the copy
    newEngine->unrefNoDelete();
    return newEngine;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the last engine given the name 'name'.  Returns NULL if
//    there is no engine with the given name.
//
// Use: public

SoEngine *
SoEngine::getByName(const SbName &name)
//
////////////////////////////////////////////////////////////////////////
{
    return (SoEngine *)getNamedBase(name, SoEngine::getClassTypeId());
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds all engines named 'name' to the list.  Returns the number of
//    engines found.
//
// Use: public

int
SoEngine::getByName(const SbName &name, SoEngineList &list)
//
////////////////////////////////////////////////////////////////////////
{
    return getNamedBases(name, list, SoEngine::getClassTypeId());
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This engine is a wrapper around the user's evaluate() routine.
//    It takes care of some bookkeeping tasks common to all engines,
//    and is called by SoField::getValue() routines.
//
// Use: internal

void
SoEngine::evaluateWrapper()
//
////////////////////////////////////////////////////////////////////////
{
    if (!needsEvaluation) return;

    // Break cycles:
    needsEvaluation = FALSE;

    int i;
    const SoEngineOutputData *od = getOutputData();
    for (i = 0; i < od->getNumOutputs(); i++) {
	SoEngineOutput *out = od->getOutput(this, i);
	out->prepareToWrite();
    }
    // Evaluate all our inputs:
    // (this works around some problems with engines that
    // don't always get their inputs during evaluation):
    const SoFieldData *fieldData = getFieldData();
    for (i = 0; i < fieldData->getNumFields(); i++) {
	SoField *inputField = fieldData->getField(this, i);
	inputField->evaluate();
    }

    evaluate();

    for (i = 0; i < od->getNumOutputs(); i++) {
	SoEngineOutput *out = od->getOutput(this, i);
	out->doneWriting();
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Propagates notification through the engine.
//
// Use: internal

void
SoEngine::notify(SoNotList *list)
//
////////////////////////////////////////////////////////////////////////
{
    if (notifying) return; // Avoid potential loops

    // There's an annoying bug with engines that enable and disable
    // their outputs in their inputChanged method that requires this
    // flag:
    notifying = TRUE;

    needsEvaluation = TRUE;

    SoNotRec *lastRec = list->getLastRec();
    SbBool notifiedFromContainer = 
	(lastRec && lastRec->getType() == SoNotRec::CONTAINER);

    // If we are being notified by a change to one of our fields
    // (notification type is CONTAINER), pass that info on to any
    // subclass that cares.
    if (notifiedFromContainer) {
	inputChanged(list->getLastField());
    }

    // We may have auditors (fields that point to this engine), so
    // notify them before we add to the list
    SoFieldContainer::notify(list);

    // Append a record of type ENGINE with the base set to this
    SoNotRec	rec(this);
    rec.setType(SoNotRec::ENGINE);
    list->append(&rec);

    // Now notify our outputs' connected fields
    SoNotList	workingList(list);
    SbBool	firstConnection = TRUE;

    const SoEngineOutputData *od = getOutputData();
    for (int i = 0; i < od->getNumOutputs(); i++) {
	SoEngineOutput *out = od->getOutput(this, i);
	if (out->isEnabled()) {
	    for (int j = 0; j < out->getNumConnections(); j++) {

		// Make sure we save the original list for use each
		// time through the loop
		if (firstConnection)
		    firstConnection = FALSE;
		else
		    workingList = *list;

		(*out)[j]->notify(&workingList);
	    }
	}
    }
    notifying = FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This is called whenever the value of an input is changed. The
//    default method does nothing. Subclasses can override this to
//    detect when a specific field is changed.
//
// Use: protected

void
SoEngine::inputChanged(SoField *)
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Writes instance to SoOutput.
//
// Use: internal

void
SoEngine::writeInstance(SoOutput *out)
//
////////////////////////////////////////////////////////////////////////
{
    if (! writeHeader(out, FALSE, TRUE)) {

	const SoFieldData *fieldData = getFieldData();

	fieldData->write(out, this);
	if (! isBuiltIn) getOutputData()->writeDescriptions(out, this);

	writeFooter(out);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads stuff into instance of subclass of SoEngine.
//    Returns FALSE on error.
//
// Use: protected

SbBool
SoEngine::readInstance(SoInput *in, unsigned short /* flags not used */)
//
////////////////////////////////////////////////////////////////////////
{
    if (in->isBinary()) {
	SbBool notBuiltIn;
	SbBool readOK = getFieldData()->read(in, this, TRUE, notBuiltIn);

	if (readOK && notBuiltIn) 
	    readOK = getOutputData()->readDescriptions(in, this);

	return readOK;
    } else {
	SbBool notBuiltIn; // Not used
	SbBool readOK = getFieldData()->read(in, this, FALSE, notBuiltIn);
	if (!readOK) return readOK;

	// See if "outputs" keyword is there:
	SbName outputs;
	if (!in->read(outputs, TRUE) || outputs != "outputs") {
	    return readOK;
	}
	// If it is, read outputs:
	return getOutputData()->readDescriptions(in, this);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Writes output type information to SoOutput.
//
// Use: internal, protected

void
SoEngine::writeOutputTypes(SoOutput *out)
//
////////////////////////////////////////////////////////////////////////
{
    SbName             outName;
    SoEngineOutputList outList;

    (void)getOutputs( outList );

    // Write the output keyword
    if (!out->isBinary())
        out->indent();
    out->write( "outputs" );

    if (out->isBinary()) 
        out->write(outList.getLength());
    else {
        out->write( ' ' );
        out->write( OPEN_BRACE_CHAR );
        out->write( ' ' );
        out->incrementIndent(2);
    }

    // For each output, write out the output type and name
    for( int i=0; i<outList.getLength(); i++ ) {

        const SoType outType = outList[i]->getConnectionType();
        getOutputName( outList[i], outName );

        out->write( outType.getName().getString() );
        if (!out->isBinary())
            out->write( ' ' );
        out->write( outName.getString() );

        if (!out->isBinary()) {
            if( i != outList.getLength()-1 ) {
                out->write( VALUE_SEPARATOR_CHAR );
                out->write( '\n' );
                out->indent();
            }
        }
    }
    if (!out->isBinary()) {
        out->write( ' ' );
        out->write( CLOSE_BRACE_CHAR );
        out->write( '\n' );
        out->decrementIndent(2);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    During a copy operation, this copies an instance that is
//    encountered through a field connection.
//
// Use: internal, virtual

SoFieldContainer *
SoEngine::copyThroughConnection() const
//
////////////////////////////////////////////////////////////////////////
{
    // If there is already a copy of this engine, use it
    SoFieldContainer *copy = findCopy(this, TRUE);
    if (copy != NULL)
	return copy;

    // Otherwise, we need to figure out whether to create a copy of
    // this engine or to just return "this". We should return a copy
    // if this engine is "inside" (see SoNode::copy() for details). To
    // determine this, we need to check if we are connected from any
    // inside node or engine. (We know that if we get here, we must be
    // connected TO at least one inside node or engine, so we don't
    // have to worry about that direction.)

    if (shouldCopy()) {

	// Create and add a new instance to the dictionary
	SoEngine *newEngine = (SoEngine *) getTypeId().createInstance();
	newEngine->ref();
	addCopy(this, newEngine);		// Adds a ref()
	newEngine->unrefNoDelete();

	// Find the copy and return it; this has the side effect of
	// copying the contents and letting the dictionary know it has
	// been copied once.
	return findCopy(this, TRUE);
    }

    // Otherwise, just return this
    return (SoFieldContainer *) this;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Recursive procedure that determines if this engine should be
//    copied during a copy operation, or just referenced as is.
//
// Use: internal

SbBool
SoEngine::shouldCopy() const
//
////////////////////////////////////////////////////////////////////////
{
    // We need to determine if we are connected from any "inside" node
    // or engine. Look through each connected input to determine this.

    const SoFieldData *fieldData = getFieldData();
    if (fieldData != NULL) {
	int numFields = fieldData->getNumFields();
	for (int i = 0; i < numFields; i++) {
	    SoField *inputField = fieldData->getField(this, i);
	    if (inputField->referencesCopy())
		return TRUE;
	}
    }

    // If we get here, we didn't find any inside connections
    return FALSE;
}

//////////////////////////////////////////////////////////////////////
//
//	SoEngineOutput class
//
//////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor.  EngineOutputs may only be constructed inside
//    SoEngine instances.
//
// Use: internal

SoEngineOutput::SoEngineOutput()
//
////////////////////////////////////////////////////////////////////////
{
    enabled = TRUE;
    container = NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.  Called automatically when SoEngine destructor is
//    called.
//
// Use: internal

SoEngineOutput::~SoEngineOutput()
//
////////////////////////////////////////////////////////////////////////
{
    // A engine should only be deleted once it has nothing connected
    // to its outputs.  So, in the DEBUG case, we'll make sure there
    // are no more connections, and complain if there are.
#ifdef DEBUG
    if (getNumConnections() != 0)
	SoDebugError::post("SoEngineOutput::~SoEngineOutput",
			   "Engine with output connections deleted.\n"
			   "Did you unref() an engine that you didn't ref()?");
#endif /* DEBUG */
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Return the type of SoField this output can write to.
//
// Use: public

SoType
SoEngineOutput::getConnectionType() const
//
////////////////////////////////////////////////////////////////////////
{
    // The connection type is stored in our container's
    // static EngineOutputData member (because it is the same for
    // all engine instances).
#ifdef DEBUG
    if (container == NULL) {
	SoDebugError::post("SoEngineOutput::getConnectionType",
			   "container is NULL!");
	return SoType::badType();
    }
#endif /* DEBUG */

    const SoEngineOutputData *od = container->getOutputData();

#ifdef DEBUG
    if (od == NULL) {
	SoDebugError::post("SoEngineOutput::getConnectionType",
			   "container has no output data!");
	return SoType::badType();
    }
#endif /* DEBUG */

    return od->getType(od->getIndex(container, this));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the number of fields this output is writing to, and
//    adds pointers to those fields to the given list:
//
// Use: public

int
SoEngineOutput::getForwardConnections(SoFieldList &list) const
//
////////////////////////////////////////////////////////////////////////
{
    int numConnections = 0;

    for (int i = 0; i < connections.getLength(); i++) {
	SoField	*field = connections[i];

	// Skip over converter, if any
	SoFieldContainer *container = field->getContainer();
	if (container->isOfType(SoFieldConverter::getClassTypeId()))
	    numConnections += ((SoFieldConverter *) container)->
		getForwardConnections(list);

	else {
	    list.append(field);
	    numConnections++;
	}
    }

    return numConnections;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Enables or disables all connections from this
//    output. The enabled flag is checked at SO_ENGINE_OUTPUT time and
//    at notify() to prevent notification/evaluation through disabled
//    outputs.
//
// Use: public

void
SoEngineOutput::enable(SbBool flag)
//
////////////////////////////////////////////////////////////////////////
{
    if (enabled != flag) {
	enabled = flag;

	// Notify if re-enabling connections
	if (flag) {

	    // A very annoying double notification occurs with engines
	    // that enable their outputs during inputChanged that we
	    // prevent by not bothering to start notification if we're
	    // already in the middle of notification:
	    SoEngine *e = getContainer();
	    if (e && e->isNotifying()) return;

	    for (int j = 0; j < getNumConnections(); j++) {
		SoField *f = (*this)[j];

		if (!f->flags.isEngineModifying) {
		    f->startNotify();
		}
	    }
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Called by SoField connection code to add a connection.  Can pass
//    NULL to just mark engine as needing evaluation.
//
// Use: internal

void
SoEngineOutput::addConnection(SoField *field)
//
////////////////////////////////////////////////////////////////////////
{
    if (field) {
	// Add to lists of connections
	connections.append(field);

	// Increment containing engine's reference count
	container->ref();
    }

    // This forces the engine to write to the new connection.
    container->needsEvaluation = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Called by SoField::disconnect() to remove a connection.
//
// Use: internal

void
SoEngineOutput::removeConnection(SoField *field)
//
////////////////////////////////////////////////////////////////////////
{
    int index = connections.find(field);
#ifdef DEBUG
    if (index == -1)
	SoDebugError::post("SoEngineOutput::removeConnection",
			   "Field is not connected!");
#endif /* DEBUG */

    connections.remove(index);

    // Decrement reference count of containing engine
    container->unref();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Prepare for writing into fields.
//
// Use: internal

void
SoEngineOutput::prepareToWrite() const
//
////////////////////////////////////////////////////////////////////////
{
    for (int i = connections.getLength()-1; i >= 0; i--) {
	SoField *f = connections.get(i);
#ifdef DEBUG
	if (f->flags.isEngineModifying) {
	    SoDebugError::post("SoEngineOutput::prepareToWrite",
			       "Internal field flags are wrong; "
			       "did you call engine->evaluate() "
			       "instead of engine->evaluateWrapper?");
	}
#endif
	f->flags.isEngineModifying = TRUE;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Done writing, reset notification flags.
//
// Use: internal

void
SoEngineOutput::doneWriting() const
//
////////////////////////////////////////////////////////////////////////
{
    for (int i = connections.getLength()-1; i >= 0; i--) {
	SoField *f = connections.get(i);
	f->flags.isEngineModifying = FALSE;
    }
}
