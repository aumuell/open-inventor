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
 |   $Revision: 1.3 $
 |
 |   Classes:
 |	SoField
 |	SoSField
 |	SoMField
 |
 |   Author(s)		: Paul S. Strauss, Nick Thompson
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/misc/SoNotification.h>
#include <Inventor/SoOutput.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/engines/SoEngine.h>
#include <Inventor/engines/SoFieldConverter.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/errors/SoReadError.h>
#include <Inventor/fields/SoFieldContainer.h>
#include <Inventor/fields/SoSubField.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/sensors/SoDataSensor.h>
#include <stdlib.h>
#include "SoGlobalField.h"

// Special characters in files
#define OPEN_BRACE_CHAR		'['
#define CLOSE_BRACE_CHAR	']'
#define VALUE_SEPARATOR_CHAR	','
#define IGNORE_CHAR		'~'
#define CONNECTION_CHAR		'='
#define FIELD_SEP_CHAR		'.'

// Bit flags in flags token in binary files
#define FIELD_IGNORED		0x01
#define FIELD_CONNECTED		0x02
#define FIELD_DEFAULT		0x04

// Amount of values to allocate at a time when reading in multiple values.
#define VALUE_CHUNK_SIZE	32

// If the field is connected or there are any FieldSensors attached to
// this field, flags.hasAuditors will be TRUE, and this structure is
// used to contain the extra information needed.  Done this way to
// save space in the common case.
struct SoFieldAuditorInfo {
    SoFieldContainer *container;
    
    // List of auditors: objects to pass notification to.
    SoAuditorList	auditors;

    // The "connection" field points to either an engine output or
    // another field:
    union {
	SoEngineOutput		*engineOutput;
	SoField			*field;
    } connection;

};


//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoField
//
//	Useful stuff for ALL fields, regardless of type or multiplicity
//
//////////////////////////////////////////////////////////////////////////////

SoType	SoField::classTypeId;		// Type identifier
char	*SoField::fieldBuf;		// Used by SoField::get()
int	SoField::fieldBufSize;		// Used by SoField::get()

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Helper routine, allocates the auditorInfo structure, if needed:
//
// Use: private, static

void
SoField::createAuditorInfo()
//
////////////////////////////////////////////////////////////////////////
{
    if (!flags.hasAuditors) {
	SoFieldContainer *myContainer = container;
	auditorInfo = new SoFieldAuditorInfo;
	auditorInfo->container = myContainer;
	auditorInfo->connection.field = NULL;
	flags.hasAuditors = TRUE;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor.
//
// Use: public

SoField::SoField()
//
////////////////////////////////////////////////////////////////////////
{
    container = NULL;
    flags.hasDefault		= TRUE;
    flags.ignored		= FALSE;
    flags.connected		= FALSE;
    flags.converted		= FALSE;
    flags.connectionEnabled	= TRUE;
    flags.notifyEnabled		= FALSE;
    flags.hasAuditors		= FALSE;
    flags.isEngineModifying	= FALSE;
    flags.readOnly		= FALSE;;
    flags.dirty			= FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.  Also disconnects if connected.
//
// Use: public

SoField::~SoField()
//
////////////////////////////////////////////////////////////////////////
{
    if (flags.connected)
	reallyDisconnect();

    // Make sure any auditors are notified of the impending doom of
    // this field. We don't have to notify the container, since we
    // can't be destroyed unless it is already being destroyed. So
    // just notify the rest of the auditors. The only type of auditors
    // that can be attached to a field are sensors and other fields
    // connected from this one. Both of these are given a chance to
    // detach/disconnect themselves.

    if (flags.hasAuditors) {
	SoAuditorList &auditors = auditorInfo->auditors;
	for (int i = auditors.getLength() - 1; i >= 0; i--) {
	    switch (auditors.getType(i)) {
	      case SoNotRec::SENSOR:
		// Tell sensor that we are going away. (This must be a
		// field sensor, but cast it to a data sensor for ease.)
		((SoDataSensor *) auditors.getObject(i))->dyingReference();

		// The call to dyingReference() might remove auditors,
		// shortening the auditors list; make sure we're not
		// trying to access past the end.
		if (i > auditors.getLength())
		    i = auditors.getLength();
		break;

	      case SoNotRec::FIELD:
		{
		// Disconnect other field from this one
		SoField *f = (SoField *)auditors.getObject(i);
		SoFieldContainer *fc = f->getContainer();

		// If connected to a converter, must remove the other
		// side of the connection:
		if (fc->isOfType(SoFieldConverter::getClassTypeId())) {
		    SoFieldConverter *converter =
			(SoFieldConverter *)fc;
		    SoFieldList fieldList;
		    converter->getForwardConnections(fieldList);
		    for (int j = 0; j < fieldList.getLength(); j++) {
			fieldList[0]->disconnect();
		    }
		    // The converter will be deleted and will
		    // disconnect itself from us when all of its
		    // forward connections go away.
		}
		else {
		    f->disconnect();
		}
		}
		break;

	      default:
		SoDebugError::post("(internal) SoField::~SoField",
				   "Got an auditor of type %d",
				   (int) auditors.getType(i));
		break;
	    }
	}
	delete auditorInfo;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if field is an instance of a field of the given type
//    or an instance of a subclass of it.
//
// Use: public

SbBool
SoField::isOfType(SoType type) const
//
////////////////////////////////////////////////////////////////////////
{
    return getTypeId().isDerivedFrom(type);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets whether connection to field or engine is enabled. This
//    flag may be set even if no connection is currently established.
//    The flag will remain in effect until changed again.
//
// Use: public

void
SoField::enableConnection(SbBool flag)
//
////////////////////////////////////////////////////////////////////////
{
    if (flags.connectionEnabled == flag) return;

    // Before disabling, pull value through if out-of-date:
    if (!flag) {
	evaluate();
	flags.connectionEnabled = FALSE;
	flags.readOnly = TRUE;

	if (isConnectedFromField() && !flags.converted)
	    auditorInfo->connection.field->connectionStatusChanged(-1);
    }
    else {
	// Mark the field dirty when re-enabling so it will get
	// evaluated.
	flags.readOnly = FALSE;
	flags.connectionEnabled = TRUE;
	flags.dirty = TRUE;

	if (isConnectedFromEngine() || flags.converted) {
	    // Mark engine as needing evaluation to force it to write
	    // value (just as if we added a connection):
	    auditorInfo->connection.engineOutput->addConnection(NULL);
	}
	else if (isConnectedFromField()) {
	    auditorInfo->connection.field->connectionStatusChanged(1);
	}
	evaluate(); // Pull value through
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Connects the field to the given output of an engine. Returns
//    FALSE if the connection could not be made.
//
// Use: public

SbBool
SoField::connectFrom(SoEngineOutput *engineOutput)
//
////////////////////////////////////////////////////////////////////////
{
    SoFieldContainer	*engineCont = engineOutput->getContainer();

#ifdef DEBUG
    // Make sure everything is contained, or this won't work at all
    if (engineCont == NULL) {
	SoDebugError::post("SoField::connectFrom",
			   "Can't connect to an engine output "
			   "that is not contained in an engine");
	return FALSE;
    }
#endif

    // Ref the engine, just in case it's currently connected, and this
    // is its only ref
    engineCont->ref();

    // Disconnect any previous connection
    disconnect();

    // Check the type of the field and the output for compatability
    SoType outputType = engineOutput->getConnectionType();
    if (getTypeId() != outputType) {

	SbBool			ret;
	SoFieldConverter	*converter;

	converter = createConverter(outputType);

	// Check for error
	if (converter == NULL)
	    ret = FALSE;
	else {
	    converter->ref();

	    // Hook the converter up to the other field first, then
	    // hook this up to the converter, to avoid multiple
	    // notifications or evaluations if something downstream is
	    // pulling values during notification:
	    // converter:
	    SoField *c_input = converter->getInput(outputType);
	    SoEngineOutput *c_output =
		converter->getOutput(getTypeId());
#ifdef DEBUG
	    if (c_input == NULL || c_output == NULL) {
		SoDebugError::post("SoField::connectFrom",
				   "Created converter, but converter"
				   "input or output is NULL");
		return FALSE;
	    }
#endif
	    // Making the connection may result in downstream engines
	    // requesting evaluation, so this must be set before the
	    // connection is made:
	    flags.converted = TRUE;
	    flags.fromEngine = TRUE;

	    c_input->connectFrom(engineOutput);
	    connectFrom(c_output);

	    converter->unref();
	    ret = TRUE;
	}
	// See comment below
	engineCont->unrefNoDelete();

	return ret;
    }

    createAuditorInfo();

    flags.connected       = TRUE;

    // If converted, this flag was set when the converter was created:
    if (!flags.converted)
	flags.fromEngine      = TRUE;

    auditorInfo->connection.engineOutput = engineOutput;

    // Tell the engine output about this connection
    engineOutput->addConnection(this);

    if (isConnectionEnabled() && engineOutput->isEnabled()) {
	// A connection means that the field no longer contains the
	// default value
	setDefault(FALSE);

	// Notify
	startNotify();
    }

    // Get rid of the extra reference.  Note that if the container was
    // handed to us with 0 references, we don't want to delete it,
    // since presumably whoever asked for the connection still wants
    // it, and will ref it later.
    engineCont->unrefNoDelete();

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Connects the field to the given field. Returns FALSE if the
//    connection could not be made.
//
// Use: public

SbBool
SoField::connectFrom(SoField *field)
//
////////////////////////////////////////////////////////////////////////
{
    // Disconnect any previous connection
    disconnect();

    // Check the types of the two fields
    SoType fieldType = field->getTypeId();
    if (getTypeId() != fieldType) {

	SbBool			ret;
	SoFieldConverter	*converter;

	converter = createConverter(fieldType);

	// Check for error
	if (converter == NULL)
	    ret = FALSE;
	else {
	    converter->ref();

	    // Hook the converter up to the other field first, then
	    // hook this up to the converter, to avoid multiple
	    // notifications or evaluations if something downstream is
	    // pulling values during notification:
	    // converter:
	    SoField *c_input = converter->getInput(fieldType);
	    SoEngineOutput *c_output =
		converter->getOutput(getTypeId());
#ifdef DEBUG
	    if (c_input == NULL || c_output == NULL) {
		SoDebugError::post("SoField::connectFrom",
				   "Created converter, but converter"
				   "input or output is NULL");
		return FALSE;
	    }
#endif
	    // Making the connection may result in downstream engines
	    // requesting evaluation, so this must be set before the
	    // connection is made:
	    flags.converted = TRUE;
	    flags.fromEngine = FALSE;

	    c_input->connectFrom(field);
	    connectFrom(c_output);

	    converter->unref();
	    ret = TRUE;
	}
	return ret;
    }

    createAuditorInfo();

    flags.connected       = TRUE;

    // If converted, this flag was set when the converter was created:
    if (!flags.converted)
	flags.fromEngine      = FALSE;

    auditorInfo->connection.field = field;

    // Make sure this field gets notified when the connected field
    // changes 
    field->addAuditor(this, SoNotRec::FIELD);

    if (isConnectionEnabled()) {
	// A connection means that the field no longer contains the
	// default value
	setDefault(FALSE);

	// Notify
	startNotify();
    }

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Disconnects the field from whatever it's connected to. Harmless
//    if not already connected.
//
// Use: public

void
SoField::disconnect()
//
////////////////////////////////////////////////////////////////////////
{
    if (flags.connected) {

	// Make sure the field is evaluated first. (There may have
	// been a notification of the field but no corresponding
	// evaluation before the field was disconnected.)
	evaluate();

	reallyDisconnect();
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the engine output the field is connected to. Returns
//    FALSE if there is no connection to an engine.
//
// Use: public

SbBool
SoField::getConnectedEngine(SoEngineOutput *&engineOut) const
//
////////////////////////////////////////////////////////////////////////
{
    if (! isConnectedFromEngine())
	return FALSE;

    // Skip over field converter, if any
    const SoField *connectedField =
	(! flags.converted ? this : getConverter()->getConnectedInput());

    engineOut = connectedField->auditorInfo->connection.engineOutput;

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the field the field is connected to. Returns FALSE if
//    there is no connection to a field.
//
// Use: public

SbBool
SoField::getConnectedField(SoField *&field) const
//
////////////////////////////////////////////////////////////////////////
{
    if (! isConnectedFromField())
	return FALSE;

    // Skip over field converter, if any
    const SoField *connectedField =
	(! flags.converted ? this : getConverter()->getConnectedInput());

    field = connectedField->auditorInfo->connection.field;

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the number of fields this field is writing into, and
//    adds pointers to those fields to the given field list.
//
// Use: public

int
SoField::getForwardConnections(SoFieldList &list) const
//
////////////////////////////////////////////////////////////////////////
{
    if (!flags.hasAuditors) return 0;

    int	numAuditors, numConnections = 0, i;

    // Loop through our auditor list, finding fields we are connected
    // to.
    SoAuditorList &auditors = auditorInfo->auditors;
    numAuditors = auditors.getLength();
    for (i = 0; i < numAuditors; i++) {
	if (auditors.getType(i) == SoNotRec::FIELD) {

	    SoField	*field = (SoField *) auditors.getObject(i);

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
    }

    return numConnections;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the containing node or engine.
//
// Use: public

SoFieldContainer *
SoField::getContainer() const
//
////////////////////////////////////////////////////////////////////////
{
    if (flags.hasAuditors)
	return auditorInfo->container;
    else
	return container;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets value of field from the Inventor data file format
//    information in the value string.  Returns TRUE if successful,
//    FALSE otherwise.
//
// Use: public

SbBool
SoField::set(const char *valueString)
//
////////////////////////////////////////////////////////////////////////
{
    SoInput	in;
    in.setBuffer((void *) valueString, strlen(valueString));
    return read(&in, "<field passed to SoField::set>");
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Stores field value (in the same format expected by the set()
//    method) in the given SbString. This always returns a string,
//    even if the field has a default value.
//
// Use: public

void
SoField::get(SbString &valueString)
//
////////////////////////////////////////////////////////////////////////
{
    SoOutput	out;

    SbBool	wasDefault = isDefault();

    if (wasDefault)
	setDefault(FALSE);

    // Make sure the field value is up to date
    evaluate();

    // Prepare a character buffer and SoOutput for writing field strings
    if (fieldBufSize == 0) {
	fieldBufSize = 1028;
	fieldBuf = (char *) malloc((unsigned) fieldBufSize);
    }

    // Set up output into a string buffer
    out.setBuffer((void *) fieldBuf, fieldBufSize, &SoField::reallocFieldBuf);

    // Make sure that the file header and lots of white space will NOT
    // be written into the string
    out.setCompact(TRUE);

    // Prepare to begin writing by incrementing the 
    // current write counter in SoBase
    SoBase::incrementCurrentWriteCounter();

    // Count references first
    out.setStage(SoOutput::COUNT_REFS);
    countWriteRefs(&out);

    // Next, do actual writing
    out.setStage(SoOutput::WRITE);
    writeValue(&out);

    // SoField::get() is used by field converters that convert to
    // strings when evaluating. Because this can happen in the middle
    // of writing a scene graph, we have to restore things to the way
    // they were before the get(). 
    SoBase::decrementCurrentWriteCounter();
    out.setStage(SoOutput::COUNT_REFS);
    countWriteRefs(&out);

    // Make sure the string is terminated
    out.reset();

    // Store the result in the passed SbString
    valueString = fieldBuf;

    setDefault(wasDefault);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Touches a field, starts notification
//
// Use: public

void
SoField::touch()
//
////////////////////////////////////////////////////////////////////////
{
    evaluate();
    valueChanged(FALSE);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the ignore flag on a field. Causes notification.
//
// Use: public

void
SoField::setIgnored(SbBool ig)
//
////////////////////////////////////////////////////////////////////////
{
    if (flags.ignored != ig) {
	flags.ignored = ig;

	// Indicate that the value changed, but leave the default flag as is
	valueChanged(FALSE);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initiates notification due to a change in the field.
//
// Use: internal

void
SoField::startNotify()
//
////////////////////////////////////////////////////////////////////////
{
    if (!flags.notifyEnabled || flags.isEngineModifying)
	return;

    SoDB::startNotify();

    // Create a new notification list:
    SoNotList	list;

    // Use notify() method to do all the work:
    notify(&list);

    SoDB::endNotify();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Propagates notification through the field. The notification must
//    be coming from the engine output or field from which this
//    field is connected.
//
// Use: internal

void
SoField::notify(SoNotList *list)
//
////////////////////////////////////////////////////////////////////////
{
    // If notification is disabled, don't do anything.
    if (flags.dirty || ! flags.notifyEnabled || flags.isEngineModifying)
	return;

    // If being notified through a connection and the connection is
    // disabled, don't notify:
    if (!flags.connectionEnabled && (list->getLastRec() != NULL)) {

	SoNotRec::Type t = list->getLastRec()->getType();
	if (t == SoNotRec::ENGINE || t == SoNotRec::FIELD)
	    return;
    }

    // Indicate that we are notifying, to break future cycles.
    // SFTrigger relies on this being done BEFORE checking for a NULL
    // field container.
    flags.dirty = TRUE;

    // Propagate to all auditors.
    // NOTE: Since this may be done for fields that are being
    // constructed, we have to check for a NULL container first.
    // NOTE: SFTrigger fields set their container to NULL temporarily
    // when being read in to prevent notification propogating when
    // they're read.
    SoFieldContainer	*cont = getContainer();

    if (cont != NULL) {

	// First create and append a new record to indicate we passed
	// through this field.
	SoNotRec	rec(cont);
	list->append(&rec, this);
	list->setLastType(SoNotRec::CONTAINER);

	// If no auditors, just notify container
	if (! flags.hasAuditors)
	    cont->notify(list);

	// Otherwise, notify container and auditors. We have to make
	// sure to copy the list before notifying anyone.
	else {
	    SoNotList listCopy(*list);
	    cont->notify(list);
	    auditorInfo->auditors.notify(&listCopy);
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the containing node, engine, or global.  Also marks this
//    field as Default, and turns on notification (this saves the
//    nodes from having to do it).
//
// Use: internal

void
SoField::setContainer(SoFieldContainer *cont)
//
////////////////////////////////////////////////////////////////////////
{
    if (flags.hasAuditors)
	auditorInfo->container = cont;
    else
	container = cont;
    setDefault(TRUE);
    enableNotify(TRUE);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if the field (or its description) needs to be
//    written out.
//
// Use: internal

SbBool
SoField::shouldWrite() const
//
////////////////////////////////////////////////////////////////////////
{
    if (!isDefault() || isConnected() || isIgnored()) return TRUE;

    // The hard case; if we have any forward connections, should also
    // write ourselves out (actually, that's only strictly necessary
    // if the field is part of a non-builtin node...).

    if (!flags.hasAuditors) return FALSE;

    // Loop through our auditor list, finding fields we are connected
    // to.
    SoAuditorList &auditors = auditorInfo->auditors;
    int numAuditors = auditors.getLength();
    for (int i = 0; i < numAuditors; i++) {
	if (auditors.getType(i) == SoNotRec::FIELD) {
	    return TRUE;
	}
    }
    return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds an auditor to list of auditors.
//
// Use: private

void
SoField::addAuditor(void *auditor, SoNotRec::Type type)
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (auditor == NULL) {
	SoDebugError::post("SoField::addAuditor", 
			   "ACK! Trying to add a NULL auditor");
	return;
    }
#endif
    createAuditorInfo();

    auditorInfo->auditors.append(auditor, type);

    // Connection status is different
    connectionStatusChanged(1);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Removes an auditor from list of auditors.
//
// Use: private

void
SoField::removeAuditor(void *auditor, SoNotRec::Type type)
//
////////////////////////////////////////////////////////////////////////
{
    int	audIndex = -1;
    if (flags.hasAuditors)
	audIndex = auditorInfo->auditors.find(auditor, type);

#ifdef DEBUG
    if (audIndex < 0) {
	SoDebugError::post("SoField::removeAuditor",
			   "can't find auditor %#x\n", auditor);
	return;
    }
#endif /* DEBUG */

    auditorInfo->auditors.remove(audIndex);

    // Connection status is different
    connectionStatusChanged(-1);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Enables or disables notification
//
// Use: internal

SbBool
SoField::enableNotify(SbBool flag)
//
////////////////////////////////////////////////////////////////////////
{
    if (flags.notifyEnabled == flag) return flag;

    if (flag) evaluate();

    flags.notifyEnabled = flag;
    return !flag;  // Previous state of flag is returned
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Indicates to a field that a change has been made involving a
//    connection from it (as source) to another field. The default
//    method does nothing.  Used by the subclass of SFTime used by the
//    realTime global field to disable/enable the real time update
//    sensor as things are connected from it or disconnected from it.
//
// Use: internal, virtual

void
SoField::connectionStatusChanged(int)
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    After a field value has been copied using copyFrom(),
//    this is called to allow fields to update the copy. This is used
//    by node, engine, and path fields to make sure instances are
//    handled properly. The default implementation does nothing.
//
// Use: internal, virtual

void
SoField::fixCopy(SbBool)
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This returns TRUE if this field contains a reference to a node
//    or engine that is copied during a copy operation (i.e., it is
//    "inside"). The default method just checks if the field is
//    connected to such a node or engine. Subclasses may contain other
//    tests, such as those that contain pointers to nodes or engines.
//
// Use: internal, virtual

SbBool
SoField::referencesCopy() const
//
////////////////////////////////////////////////////////////////////////
{
    // If this field is not connected, it doesn't reference anything
    if (! isConnected())
	return FALSE;

    // Find the container of the connected field or engine
    // output, and determine if it is an engine
    SoFieldContainer *container;
    SbBool	     containerKnownToBeEngine = FALSE;

    if (isConnectedFromField()) {
	SoField	*connectedField;
	getConnectedField(connectedField);
	container = connectedField->getContainer();
	containerKnownToBeEngine = FALSE;
    }
    else {
	SoEngineOutput *connectedOutput;
	getConnectedEngine(connectedOutput);
	container = (SoEngine *) connectedOutput->getContainer();
	containerKnownToBeEngine = TRUE;
    }

    // If a copy of the container exists, this field references a copy
    if (SoFieldContainer::checkCopy(container) != NULL)
	return TRUE;

    // If the container is an engine, see if that engine should be
    // copied, recursively.
    // ??? Optimize this by keeping a dict of "outside"
    // ??? engines so we never do the same work twice?
    // ??? We could actually store a pointer to the original engine in
    // ??? the dictionary if it is outside, but then we have to be
    // ??? careful to test for this when we call checkCopy(). It's
    // ??? probably not worth the effort, since people won't copy
    // ??? complicated engine networks often.
    if (containerKnownToBeEngine ||
	container->isOfType(SoEngine::getClassTypeId()) &&
	((SoEngine *) container)->shouldCopy())
	return TRUE;

    return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Copies connection from one field to another. Assumes fields are
//    the same subclass and that fromField is connected. The index of
//    the field in the SoFieldData instance is passed in.
//
// Use: internal

void
SoField::copyConnection(const SoField *fromField)
//
////////////////////////////////////////////////////////////////////////
{
    // Connected from another field:
    if (fromField->isConnectedFromField()) {
	SoField	*connectedField;
	fromField->getConnectedField(connectedField);

	// Find the index of the connected field in its container
	SoFieldContainer *connectedFC = connectedField->getContainer();
	const SoFieldData *fieldData = connectedFC->getFieldData();
	int index = fieldData->getIndex(connectedFC, connectedField);

	// We may need to copy the field's container
	SoFieldContainer *FCCopy = connectedFC->copyThroughConnection();

	// Get the other field data in case it is different
	const SoFieldData *otherFieldData = FCCopy->getFieldData();

	// Connect from the corresponding field in the container
	connectFrom(otherFieldData->getField(FCCopy, index));
    }

    // Connected from engine:
    else {
	SoEngineOutput *connectedOutput;
	fromField->getConnectedEngine(connectedOutput);

	// Find the index of this output in the containing engine
	SoEngine *connectedEngine = connectedOutput->getContainer();
	const SoEngineOutputData *outputData =connectedEngine->getOutputData();
	int outputIndex = outputData->getIndex(connectedEngine,
					       connectedOutput);

	// We may need to copy the engine itself
	SoEngine *engineCopy =
	    (SoEngine *) connectedEngine->copyThroughConnection();

	// Get the other output data in case it is different
	const SoEngineOutputData *outputDataCopy = engineCopy->getOutputData();

	// Connect from the corresponding output in the engine copy
	connectFrom(outputDataCopy->getOutput(engineCopy, outputIndex));
    }

    // Make sure state of connection is identical
    if (! fromField->isConnectionEnabled())
	enableConnection(FALSE);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Called by an instance to indicate that a value has changed. If
//    resetDefault is TRUE, this turns off default flag. Initiates
//    notification to clear downstream dirty bits, and evaluates
//    (after protecting the value in the field) to clear upstream
//    dirty bits.
//
// Use: protected

void
SoField::valueChanged(SbBool resetDefault)	// Default is TRUE
//
////////////////////////////////////////////////////////////////////////
{
    if (resetDefault)
	setDefault(FALSE);
    
    flags.readOnly = TRUE;
    flags.dirty = FALSE; // So notification WILL happen, at least as
			 // far as sensors/field container
    startNotify();
    evaluate();
    flags.readOnly = !flags.connectionEnabled;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Really disconnects the field from whatever it's connected to.
//
// Use: private

void
SoField::reallyDisconnect()
//
////////////////////////////////////////////////////////////////////////
{
    if (flags.fromEngine || flags.converted) {
	SoEngineOutput *out = auditorInfo->connection.engineOutput;
	out->removeConnection(this);
	auditorInfo->connection.engineOutput = NULL;
    }
    else {
	SoField *field = auditorInfo->connection.field;
	field->removeAuditor(this, SoNotRec::FIELD);
	auditorInfo->connection.field = NULL;
    }
    flags.connected = FALSE;
    flags.converted = FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Evaluates the field or engine the field is connected to,
//    storing the result in the field.
//
// Use: private

void
SoField::evaluateConnection() const
//
////////////////////////////////////////////////////////////////////////
{
    // Cast away the nasty const...
    SoField	*f = (SoField *) this;
    f->flags.dirty = FALSE; // This must be done before upstream
			    // evaluation to break evaluation cycles.

    if (flags.isEngineModifying ||
	!flags.connected  || !flags.connectionEnabled) return;

    // If connected to an engine, evaluate that engine. This will
    // cause the value to be written into the field by the engine.
    if (flags.fromEngine || flags.converted) {

	SoEngine *e =
	    auditorInfo->connection.engineOutput->getContainer();
	e->evaluateWrapper();
    }

    // If connected to a field, just copy the value from that field
    // UNLESS the readOnly bit is set.
    // This uses the virtual "=" operator, which should do the right thing.
    else if (!f->isReadOnly()) {
	// Disable notification since we already did it
	SbBool notifySave = f->flags.notifyEnabled;
	f->flags.notifyEnabled = FALSE;

	// Copy value
	f->copyFrom(*auditorInfo->connection.field);

	// Reenable notification
	f->flags.notifyEnabled = notifySave;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Creates a converter engine to convert from the given field
//    type to the type of this field. Returns NULL on error.
//
// Use: private

SoFieldConverter *
SoField::createConverter(const SoType &fromFieldType) const
//
////////////////////////////////////////////////////////////////////////
{
    SoFieldConverter	*converter;

    // Find the type of the conversion engine (if any)
    SoType converterType = SoDB::getConverter(fromFieldType, getTypeId());

    // If no engine exists for these types, don't connect
    if (converterType.isBad()) {

#ifdef DEBUG
	const char *fromName = fromFieldType.getName().getString();
	SoDebugError::post("SoField::connectFrom",
			   "Connection failed - no conversion supported "
			   "for types (%s --> %s)",
			   fromName, getTypeId().getName().getString());
#endif /* DEBUG */
	return NULL;
    }

    // If there is a converter, instantiate it and link it in
    converter = (SoFieldConverter *) converterType.createInstance();

#ifdef DEBUG
    // If it's an abstract class, error. This should never happen.
    if (converter == NULL)
	SoDebugError::post("SoField::connectFrom",
			   "Connection failed - unable to create an "
			   "instance of \"%s\" converter",
			   converterType.getName().getString());
#endif /* DEBUG */

    return converter;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads field from file. Works for ASCII and binary input. The
//    name of the field has already been read and is passed here (for
//    error messages).
//
// Use: private

SbBool
SoField::read(SoInput *in, const SbName &name)
//
////////////////////////////////////////////////////////////////////////
{
    char	c;
    SbBool	shouldReadConnection = FALSE;
    SbBool	gotValue = FALSE;

    // Turn off notification during reading process.  It is turned
    // back on and notification is started below:
    SbBool wasNotifyEnabled = flags.notifyEnabled;
    flags.notifyEnabled = FALSE;

    if (in->isBinary()) {
	short	readFlags;

	if (! readValue(in)) {
	    SoReadError::post(in,
			      "Couldn't read binary value for field \"%s\"",
			      name.getString());
	    flags.notifyEnabled = wasNotifyEnabled;
	    return FALSE;
	}

	// Read flags
	if (! in->read(readFlags)) {
	    SoReadError::post(in,
			      "Couldn't read binary flags for field \"%s\"",
			      name.getString());
	    flags.notifyEnabled = wasNotifyEnabled;
	    return FALSE;
	}

	// (Don't use setIgnored() to set this, since it would cause
	// notification, which we don't want to have happen for trigger
	// fields.)
	flags.ignored = (readFlags & FIELD_IGNORED) != 0;
	shouldReadConnection = (readFlags & FIELD_CONNECTED) != 0;
	setDefault((readFlags & FIELD_DEFAULT) != 0);
	gotValue      = TRUE;
    }

    // ASCII version...
    else {
	// Check for ignore flag with no value
	if (in->read(c) && c == IGNORE_CHAR) {
	    setDefault(TRUE);
	    setIgnored(TRUE);

	    // Check for connection to engine/field
	    if (in->read(c) && c == CONNECTION_CHAR)
		shouldReadConnection = TRUE;
	    else
		in->putBack(c);
	    gotValue = FALSE;
	}

	else {
	    setIgnored(FALSE);

	    // If character is connection character, we just use the
	    // default value and skip the reading-value stuff
	    if (c != CONNECTION_CHAR) {
		in->putBack(c);

		if (! readValue(in)) {
		    SoReadError::post(in,
				      "Couldn't read value for field \"%s\"",
				      name.getString());
		    flags.notifyEnabled = wasNotifyEnabled;
		    return FALSE;
		}

		gotValue = TRUE;
		setDefault(FALSE);

		// Check for ignore flag after value
		if (in->read(c) && c == IGNORE_CHAR) {
		    // (Don't use setIgnored() to set this, since it
		    // would cause notification, which we don't want
		    // to have happen for trigger fields.)
		    flags.ignored = TRUE;

		    // Get character to check for connection to
		    // engine/field below.
		    in->read(c);
		}
	    } else {
		gotValue = FALSE;
	    }

	    // Check for connection to engine/field
	    if (c == CONNECTION_CHAR)
		shouldReadConnection = TRUE;
	    else
		in->putBack(c);
	}
    }

    // Read connection info if necessary.
    if (shouldReadConnection  && !readConnection(in)) {
	flags.notifyEnabled = wasNotifyEnabled;
	return FALSE;
    }

    // Turn notification back the way it was:
    flags.notifyEnabled = wasNotifyEnabled;

    // If a value was read (even if there's a connection), call
    // valueChanged. Otherwise, just notify.
    if (gotValue) {
	valueChanged(FALSE);
    } else {
	startNotify();
    }

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads field connection from file. Works for ASCII and binary
//    input. Returns FALSE on error.  The upstream connection is
//    evaluated before connecting so we don't end up with dirty fields
//    upstream from clean ones (fields are clean once they've been
//    read).
//
// Use: private

SbBool
SoField::readConnection(SoInput *in)
//
////////////////////////////////////////////////////////////////////////
{
    SoFieldContainer	*connContainer;
    SbName		fieldName;
    SbBool		gotChar;
    char		c;

    // The field is connected to either an engine output or to a field
    // of an engine or node. First, read the node or engine as a base.
    SoBase *baseTemp;
    if (! SoBase::read(in, baseTemp, SoFieldContainer::getClassTypeId()))
	return FALSE;

    if (baseTemp == NULL) {
	SoReadError::post(in, "Missing node or engine name in "
			  "connection specification");
	return FALSE;
    }

    connContainer = (SoFieldContainer *) baseTemp;

    // Read the field separator character (ASCII only)
    if (! in->isBinary() &&
	(! (gotChar = in->read(c)) || c != FIELD_SEP_CHAR)) {

	if (gotChar)
	    SoReadError::post(in, "Expected '%c'; got '%c'",
			      FIELD_SEP_CHAR, c);
	else
	    SoReadError::post(in, "Expected '%c'; got EOF", FIELD_SEP_CHAR);

	return FALSE;
    }

    // Read the field name or output name
    if (! in->read(fieldName, TRUE)) {
	SoReadError::post(in,
			  "Premature end of file before connection was read");
	return FALSE;
    }

    // If the base is a node, make sure the field name is valid and
    // then connect to it.
    if (connContainer->isOfType(SoNode::getClassTypeId())) {
	SoNode	*node = (SoNode *) connContainer;
	SoField	*connField;

	connField = node->getField(fieldName);

	if (connField == NULL) {
	    const char *nodeName = node->getTypeId().getName().getString();
	    SoReadError::post(in, "No such field \"%s\" in node %s",
			      fieldName.getString(), nodeName);
	    return FALSE;
	}

	if (! connectFrom(connField)) {
	    const char *nodeName = node->getTypeId().getName().getString();
	    SoReadError::post(in, "Can't connect to field \"%s.%s\"",
			      nodeName, fieldName.getString());
	    return FALSE;
	}
    }

    // If the base is an engine, make sure the name is a valid field
    // or output and then connect to it
    else if (connContainer->isOfType(SoEngine::getClassTypeId())) {
	SoEngine	*engine = (SoEngine *) connContainer;
	SoField		*connField;
	SoEngineOutput	*connOutput;

	connField = engine->getField(fieldName);

	if (connField == NULL) {

	    // See if it's an output
	    connOutput = engine->getOutput(fieldName);

	    if (connOutput == NULL) {
		const char *eName = engine->getTypeId().getName().getString();
		SoReadError::post(in, "No such field or output \"%s\" "
				  "in engine %s",
				  fieldName.getString(), eName);
		return FALSE;
	    }

	    if (! connectFrom(connOutput)) {
		const char *eName = engine->getTypeId().getName().getString();
		SoReadError::post(in,
				  "Can't connect to engine output \"%s.%s\"",
				  eName, fieldName.getString());
		return FALSE;
	    }
	}
	else {
	    if (! connectFrom(connField)) {
		const char *eName = engine->getTypeId().getName().getString();
		SoReadError::post(in, "Can't connect to field \"%s.%s\"",
				  eName, fieldName.getString());
		return FALSE;
	    }
	}
    }

    // If it is a global field...
    else if (connContainer->isOfType(SoGlobalField::getClassTypeId())) {

	SoGlobalField *gf = (SoGlobalField *) connContainer;
	SoField *connField = gf->getMyField();

	// Make sure the name in the input is the name of the global field
	if (fieldName != gf->getName()) {
	    const char *globalName = gf->getName().getString();
	    SoReadError::post(in, "Wrong field name (\"%s\") for global "
			      "field \"%s\"",
			      fieldName.getString(), globalName);
	    return FALSE;
	}

	if (! connectFrom(connField)) {
	    SoReadError::post(in, "Can't connect to global field \"%s\"",
			      gf->getName().getString());
	    return FALSE;
	}
    }

    // If it's not a node, engine or global field, problem
    else {
	SoReadError::post(in, "Trying to connect to a %s",
			  connContainer->getTypeId().getName().getString());
	return FALSE;
    }

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Writes field to file. Works for ASCII and binary output. May be
//    called for either stage of writing.
//
// Use: internal

void
SoField::write(SoOutput *out, const SbName &name) const
//
////////////////////////////////////////////////////////////////////////
{
    if (out->getStage() == SoOutput::COUNT_REFS)
	countWriteRefs(out);

    else {
	// Actually write the field
	evaluate();

	SbBool writeConn = isConnected() && isConnectionEnabled();

	// We should write a field-to-field connection if one exists
	// and the container at the other end should be written
	if (writeConn && isConnectedFromField()) {
	    SoField	*field;
	    getConnectedField(field);
	    SoFieldContainer *container = field->getContainer();
	    if (container == NULL || ! container->shouldWrite())
		writeConn = FALSE;
	}

	if (out->isBinary()) {
	    short	flags = 0;

	    out->write(name.getString());
	    writeValue(out);

	    if (isIgnored())
		flags |= FIELD_IGNORED;
	    if (writeConn)
		flags |= FIELD_CONNECTED;
	    if (isDefault())
		flags |= FIELD_DEFAULT;

	    out->write(flags);

	    // Recurse on connected stuff if necessary
	    if (writeConn)
		writeConnection(out);
	}

	// Only write out IF connected, not default, or ignored:
	else if (writeConn || !isDefault() || isIgnored()) {
	    out->indent();
	    out->write(name.getString());
	    out->write(out->isCompact() ? ' ' : '\t');

	    if (! isDefault())
		writeValue(out);

	    if (isIgnored()) {
		if (! isDefault())
		    out->write(' ');
		out->write(IGNORE_CHAR);
	    }

	    // Recurse on connected stuff if necessary
	    if (writeConn)
		writeConnection(out);

	    // Annotate if necessary
	    if (out->getAnnotation() & SoOutput::ADDRESSES) {
		char buf[100];
		sprintf(buf, " # %#x", this);
		out->write(buf);
	    }
	    out->write(out->isCompact() ? ' ' : '\n');
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Counts write-references on field to prepare for writing.
//
// Use: internal

void
SoField::countWriteRefs(SoOutput *out) const
//
////////////////////////////////////////////////////////////////////////
{
    if (! isConnected())
	return;

    // If we are connected from an engine output, add a reference
    // to that engine
    if (flags.fromEngine || flags.converted) {
	// We can't use getConnectedEngine() here because we want to
	// make sure we get the field converter if we're connected to one
	SoEngine *container = 
	    auditorInfo->connection.engineOutput->getContainer();
	if (container != NULL)
	    container->addWriteReference(out);
    }

    // If we are connected from another field, add a reference to
    // the field's container, but mark it as having come from a
    // field-to-field connection
    else {
	SoField	*field;
	getConnectedField(field);
	SoFieldContainer *container = field->getContainer();
	if (container != NULL)
	    container->addWriteReference(out, TRUE);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Writes field connection to file. Works for ASCII and binary
//    output. This is called only for the last stage of writing.
//
// Use: private

void
SoField::writeConnection(SoOutput *out) const
//
////////////////////////////////////////////////////////////////////////
{
    SoFieldContainer	*container;
    SbName		fieldName;

    // Write out connection character (ASCII only)
    if (! out->isBinary()) {
	out->write(' ');
	out->write(CONNECTION_CHAR);
	if (out->getAnnotation()&SoOutput::ADDRESSES) {
	    char buf[100];
	    sprintf(buf, " # %#x", this);
	    out->write(buf);
	}
	out->write('\n');
    }

    // Base will be indented a little more
    out->incrementIndent();

    getConnectionInfo(container, fieldName);

    // Recurse on container (node, engine, or global field)
    container->writeInstance(out);

    // Write out separator character (ASCII only)
    if (! out->isBinary()) {
	out->indent();
	out->write(FIELD_SEP_CHAR);
	out->write(' ');
    }

    out->write(fieldName.getString());

    if (out->getAnnotation()&SoOutput::ADDRESSES) {
	char	buf[100];
	void	*ptr;

	if (flags.fromEngine) {
	    SoEngineOutput *eo;
	    getConnectedEngine(eo);
	    ptr = eo;
	}
	else {
	    SoField *f;
	    getConnectedField(f);
	    ptr = f;
	}
	sprintf(buf, " # %#x", ptr);
	out->write(buf);
    }

    if (! out->isBinary())
	out->write('\n');

    // Base was indented
    out->decrementIndent();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Utility function that returns converter through which this field
//    is connected.
//
// Use: private

SoFieldConverter *
SoField::getConverter() const
{
    return (SoFieldConverter *)
	auditorInfo->connection.engineOutput->getContainer();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This is a utility function that determines what a field is
// connected from.  Transparently skips over field converter engines,
// so it reconstructs the original connection that was specified.
//
// Use: private

void
SoField::getConnectionInfo(SoFieldContainer *&container,
			   SbName &fieldName) const
{
    // If it's connected through a converter, recurse
    if (flags.converted)
	getConverter()->getConnectedInput()->getConnectionInfo(container,
							       fieldName);

    // Field-to-field connection?
    else if (! flags.fromEngine) {
	container = auditorInfo->connection.field->getContainer();
	container->getFieldName(auditorInfo->connection.field, fieldName);
	return;
    }

    // It's an engine.
    else {
	container = auditorInfo->connection.engineOutput->getContainer();
	((SoEngine *) container)->getOutputName(
	    auditorInfo->connection.engineOutput, fieldName);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This is a callback function passed to SoOutput::setBuffer() by
//    SoField::get(). It is used to reallocate the buffer in which the
//    returned field info string is stored. Since we need to keep
//    track of the pointer to the buffer and its size, we can't just
//    call realloc() instead.
//
// Use: private, static

void *
SoField::reallocFieldBuf(void *ptr, size_t newSize)
//
////////////////////////////////////////////////////////////////////////
{
    fieldBuf = (char *) realloc(ptr, newSize);
    fieldBufSize = newSize;

    return fieldBuf;
}

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoSField
//
//	Single-valued fields
//
//////////////////////////////////////////////////////////////////////////////

SoType	SoSField::classTypeId;

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoSField class.
//
// Use: internal

void
SoSField::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__FIELD_INIT_CLASS(SoSField, "SField", SoField);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: public

SoSField::~SoSField()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Use by SoType::createInstance; in this case, return NULL, since
//    this is an abstract type that has no instances...
//
// Use: public

void *
SoSField::createInstance()
//
////////////////////////////////////////////////////////////////////////
{
    return NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor - keeps subclass constructors from being too complex
//    for poor old C++.
//
// Use: protected

SoSField::SoSField()
//
////////////////////////////////////////////////////////////////////////
{
}

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoMField
//
//	Multiple-valued fields
//
//////////////////////////////////////////////////////////////////////////////

char	*SoMField::fieldBuf;		// Used by SoMField::get1()
int	SoMField::fieldBufSize;		// Used by SoMField::get1()
SoType	SoMField::classTypeId;

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoMField class.
//
// Use: internal

void
SoMField::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__FIELD_INIT_CLASS(SoMField, "MField", SoField);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//
// Use: public

SoMField::~SoMField()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Use by SoType::createInstance; in this case, return NULL, since
//    this is an abstract type that has no instances...
//
// Use: public

void *
SoMField::createInstance()
//
////////////////////////////////////////////////////////////////////////
{
    return NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: protected

SoMField::SoMField()
//
////////////////////////////////////////////////////////////////////////
{
    maxNum = num = 0;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Inserts or deletes values to adjust to the given size
//
// Use: public

void
SoMField::setNum(int n)
{
    if (n > num)
	insertSpace(num, n-num);
    else if (n < num)
	deleteValues(n);
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Deletes num values starting at index start. A num of -1 (the default)
//    means delete all values after start, inclusive.
//
// Use: public

void
SoMField::deleteValues(int start,	// Starting index
		       int numToDelete)	// Number of values to delete
//
////////////////////////////////////////////////////////////////////////
{
    int	lastToCopy, i;

    if (numToDelete < 0)
	numToDelete = getNum() - start;

    // Special case of deleting all values
    if (numToDelete == getNum())
	deleteAllValues();

    else {
	// Copy from the end of the array to the middle
	lastToCopy = (getNum() - 1) - numToDelete;
	for (i = start; i <= lastToCopy; i++)
	    copyValue(i, i + numToDelete);

	// Truncate the array
	makeRoom(getNum() - numToDelete);
    }

    // The field value has changed...
    valueChanged();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Inserts space for num values starting at start.  The initial
//    values in the new space are undefined.
//
// Use: public

void
SoMField::insertSpace(int start,	// Starting index
		      int numToInsert)		// Number of spaces to insert
//
////////////////////////////////////////////////////////////////////////
{
    // Expand the array
    makeRoom(getNum() + numToInsert);

    // Copy stuff out of the inserted area to later in the array
    for (int i = num - 1; i >= start + numToInsert; --i)
	copyValue(i, i - numToInsert);

    // The field value has changed...
    valueChanged();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This is equivalent to the SoField::set() method, but operates on
//    only the value given by the index.
//
// Use: public

SbBool
SoMField::set1(int index, const char *valueString)
//
////////////////////////////////////////////////////////////////////////
{
    SoInput	in;
    in.setBuffer((void *) valueString, strlen(valueString));

    if (read1Value(&in, index)) {

	// We have to do this here because read1Value() doesn't
	// indicate that values have changed, since it's usually used
	// in a larger reading context.
	valueChanged();

	return TRUE;
    }
    return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This is equivalent to the SoField::get() method, but operates on
//    only the value given by the index.
//
// Use: public

void
SoMField::get1(int index, SbString &valueString)
//
////////////////////////////////////////////////////////////////////////
{
    SoOutput	out;

    // Make sure the field value is up to date
    evaluate();

    // Prepare a character buffer and SoOutput for writing field strings
    if (fieldBufSize == 0) {
	fieldBufSize = 1028;
	fieldBuf = (char *) malloc((unsigned) fieldBufSize);
    }

    // Set up output into a string buffer
    out.setBuffer((void *) fieldBuf, fieldBufSize, &SoMField::reallocFieldBuf);

    // Make sure that the file header and lots of white space will NOT
    // be written into the string
    out.setCompact(TRUE);

    // Write the data
    write1Value(&out, index);

    // Make sure the string is terminated
    out.reset();

    // Store the result in the passed SbString
    valueString = fieldBuf;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This makes sure there is enough memory allocated to hold
//    "newNum" values. It reallocates the values if necessary.
//
// Use: protected

void
SoMField::makeRoom(int newNum)		// New number of values
//
////////////////////////////////////////////////////////////////////////
{
    // Allocate room to hold all values if necessary
    if (newNum != num) {
	allocValues(newNum);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads all field values from file. Works for ASCII and binary
//    output. Binary values are read as a chunk.
//
// Use: private

SbBool
SoMField::readValue(SoInput *in)
//
////////////////////////////////////////////////////////////////////////
{
    if (in->isBinary()) {
	int	numToRead;

	// Read number of values
	if (! in->read(numToRead)) {
	    SoReadError::post(in, "Couldn't read number of binary values "
			      "in multiple-value field");
	    return FALSE;
	}

	// Make space for values; also sets number of values
	makeRoom(numToRead);

	// Read values
        if (! readBinaryValues(in, numToRead))
            return FALSE;
    }

    else {
	char	c;
	int	curIndex = 0;

	// Check for multiple field values
	if (in->read(c) && c == OPEN_BRACE_CHAR) {

	    // Check for no values: just an open and close brace
	    if (in->read(c) && c == CLOSE_BRACE_CHAR)
		;					// Do nothing now

	    else {
		in->putBack(c);

		while (TRUE) {

		    // Make some room at end if necessary
		    if (curIndex >= getNum())
			makeRoom(getNum() + VALUE_CHUNK_SIZE);

		    if (! read1Value(in, curIndex++) || ! in->read(c)) {
			SoReadError::post(in,
					  "Couldn't read value %d of field",
					  curIndex);
			return FALSE;
		    }

		    if (c == VALUE_SEPARATOR_CHAR) {

			// See if this is a trailing separator (right before 
			// the closing brace). This is legal, but ignored.

			if (in->read(c)) {
			    if (c == CLOSE_BRACE_CHAR)
				break;
			    else
				in->putBack(c);
			}
		    }

		    else if (c == CLOSE_BRACE_CHAR)
			break;

		    else {
			SoReadError::post(in,
					  "Expected '%c' or '%c' but got "
					  "'%c' while reading value %d",
					  VALUE_SEPARATOR_CHAR,
					  CLOSE_BRACE_CHAR, c,
					  curIndex);
			return FALSE;
		    }
		}
	    }

	    // If extra space left over, nuke it
	    if (curIndex < getNum())
		makeRoom(curIndex);
	}

	else {
	    // Try reading 1 value
	    in->putBack(c);
	    makeRoom(1);
	    if (! read1Value(in, 0))
		return FALSE;
	}
    }

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Writes all values of field to file. Works for ASCII and binary
//    output. Binary output writes out the number of values and then
//    the values as one big chunk o'memory.
//
// Use: private

void
SoMField::writeValue(SoOutput *out) const
//
////////////////////////////////////////////////////////////////////////
{
    int	i;

    if (out->isBinary()) {
	out->write(num);
        writeBinaryValues(out);
    }

    else {
	if (num == 1)
	    write1Value(out, 0);

	else {
	    int	numOnLine = 0, maxOnLine = getNumValuesPerLine();

	    out->write(OPEN_BRACE_CHAR);
	    out->write(' ');

	    out->incrementIndent(4);

	    for (i = 0; i < num; i++) {

		write1Value(out, i);

		if (i < num - 1) {
		    out->write(VALUE_SEPARATOR_CHAR);
		    if (++numOnLine == maxOnLine && ! out->isCompact()) {
			out->write('\n');
			out->indent();
			out->write(' ');
			out->write(' ');
			numOnLine = 0;
		    }
		    else
			out->write(' ');
		}
	    }

	    out->write(' ');
	    out->write(CLOSE_BRACE_CHAR);

	    out->decrementIndent(4);
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Writes array of binary values to file as one chunk.
//
// Use: private

void
SoMField::writeBinaryValues(SoOutput *out) const // Defines writing action

//
////////////////////////////////////////////////////////////////////////
{
    int	i;

    for (i = 0; i < num; i++)
        write1Value(out, i);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads array of binary values from file as one chunk.
//
// Use: private

SbBool
SoMField::readBinaryValues(SoInput *in,    // Reading specification
                    	   int numToRead)  // Number of values to read
//
////////////////////////////////////////////////////////////////////////
{
    int	i;

    for (i = 0; i < numToRead; i++)
        if (! read1Value(in, i))
            return FALSE;

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns number of ASCII values to write per output line. This
//    defines the default to be 1, just to play it safe.
//
// Use: private

int
SoMField::getNumValuesPerLine() const
//
////////////////////////////////////////////////////////////////////////
{
    return 1;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This is a callback function passed to SoOutput::setBuffer() by
//    SoMField::get1(). It is used to reallocate the buffer in which the
//    returned field info string is stored. Since we need to keep
//    track of the pointer to the buffer and its size, we can't just
//    call realloc() instead.
//
// Use: private, static

void *
SoMField::reallocFieldBuf(void *ptr, size_t newSize)
//
////////////////////////////////////////////////////////////////////////
{
    fieldBuf = (char *) realloc(ptr, newSize);
    fieldBufSize = newSize;

    return fieldBuf;
}



