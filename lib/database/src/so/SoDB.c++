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
 * Copyright (C) 1990, 1991   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.2 $
 |
 |   Description:
 |	Definition of the SoDB class
 | 
 |   Author(s)		: Paul S. Strauss, Nick Thompson, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <stdlib.h>
#include <Inventor/SbDict.h>
#include <Inventor/SbString.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/SoOutput.h>
#include <Inventor/SoPath.h>
#include <Inventor/SoType.h>
#include <Inventor/actions/SoAction.h>
#include <Inventor/details/SoDetail.h>
#include <Inventor/elements/SoElement.h>
#include <Inventor/engines/SoEngine.h>
#include <Inventor/engines/SoFieldConverter.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/errors/SoError.h>
#include <Inventor/errors/SoReadError.h>
#include <Inventor/events/SoEvent.h>
#include <Inventor/fields/SoField.h>
#include <Inventor/fields/SoSFTime.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/sensors/SoTimerSensor.h>

#include <SoDebug.h>

#include "upgraders/SoUpgraders.h"	    /* V1 upgraders */
#include "fields/SoGlobalField.h"	    /* GlobalField routines	*/

// Internal class for storing headers and the corresponding 
// callback functions.
SoINTERNAL struct SoDBHeaderData {
    SbString	    headerString;
    SbBool	    isBinary;
    float	    ivVersion;
    SoDBHeaderCB    *preCB;
    SoDBHeaderCB    *postCB;
    void 	    *userData;    
};

/////////////////////////////////////////////////////////////////////////////
//
// This class exists only for the "realTime" global field. It is
// exactly the same as SoSFTime in all respects except one: it
// redefines the connectionStatusChanged() method, allowing the global
// database to keep track of connections to realTime. This allows SoDB
// to disable the time-update sensor when nothing is connected to the
// realTime global field, improving performance.
//
// Note that this class does not set up a run-time type id; it is
// inherited as is from SoSFTime, meaning that realTime is treated
// just like any other SoSFTime field.
//

class SoSFRealTime : public SoSFTime {
  public:
    SoSFRealTime();
    virtual ~SoSFRealTime();

  private:
    virtual void	connectionStatusChanged(int numConnections);

    int totalNumConnections;
};

SoSFRealTime::SoSFRealTime()
{
    totalNumConnections = 0;
}

SoSFRealTime::~SoSFRealTime()
{
}

void
SoSFRealTime::connectionStatusChanged(int numConnections)
{
    // Determine if realTime is connected to anything.
    totalNumConnections += numConnections;

    // Enable sensor if we have any connections:
    SoDB::enableRealTimeSensor((totalNumConnections > 0));
}

// This identifies the current version of Inventor
const char	*SoDB::versionString = "SGI Open Inventor 2.1 Beta";

// This is the famous global database that you hear so much about
SoDB		*SoDB::globalDB = NULL;

// This keeps track of the number of current notifications. When this
// is decremented to 0, all immediate idle queue sensors are triggered.
int		SoDB::notifyCount = 0;

// This list stores information for the registered headers
SbPList		*SoDB::headerList;

// This dictionary stores field conversion engine types
SbDict		*SoDB::conversionDict;

// The global realTime field:
SoSFRealTime	*SoDB::realTime;

// Sensor which touches realTime every once in a while
SoTimerSensor	*SoDB::realTimeSensor;

/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the global database. After this call, the (private)
//    global variable "globalDB" refers to the global database. This
//    sets up all standard nodes, actions, elements, etc..
//
// Use: public, static

void
SoDB::init()
//
////////////////////////////////////////////////////////////////////////
{
    // Initialize global DB only once.
    if (globalDB == NULL) {

	globalDB = new SoDB;

	// Initialize the runtime type system
	SoType::init();

	// Setup for file reading (initializes list of directories to
	// search in).
	SoInput::init();

	// Set up field conversion dictionary
	conversionDict = new SbDict;
	
	//
	// Initialize all standard classes. The significant ordering
	// rules are:
	//	actions must be done before nodes
	//	elements must be done before actions
	//
	SoBase::initClass();
	SoFieldContainer::initClass();
	SoPath::initClass();
	SoGlobalField::initClass();

	SoError::initClasses();
	SoElement::initElements();
	SoAction::initClasses();
	SoNode::initClasses();
	SoField::initClasses();
	SoEngine::initClasses();
	SoEvent::initClasses();
	SoDetail::initClasses();

	SoUpgrader::initClasses();
	
	// Create the header list, and register valid headers we know about
	headerList = new SbPList;
	SoDB::registerHeader(SoOutput::getDefaultASCIIHeader(),  
			    	FALSE, 2.1f,
				NULL, NULL, NULL);
	SoDB::registerHeader(SoOutput::getDefaultBinaryHeader(), 
				TRUE, 2.1f,
				NULL, NULL, NULL);
	SoDB::registerHeader("#Inventor V2.0 ascii",  
				FALSE, 2.0f,
				NULL, NULL, NULL);
	SoDB::registerHeader("#Inventor V2.0 binary", 
				TRUE, 2.0f,
				NULL, NULL, NULL);
	SoDB::registerHeader("#Inventor V1.0 ascii",  
				FALSE, 1.0f,
				NULL, NULL, NULL);
	SoDB::registerHeader("#Inventor V1.0 binary", 
				TRUE,  1.0f,
				NULL, NULL, NULL);
				    
	// For now, treat VRML files as Inventor 2.1 files.
	// In the future, we might want to verify that the VRML file
	// contains strictly VRML nodes, i.e. any Inventor (non-VRML)
	// nodes in the file generate read warnings.
	SoDB::registerHeader("#VRML V1.0 ascii",  
				FALSE, 2.1f,
				NULL, NULL, NULL);
	
	// Create realTime global field. We have to bypass the
	// standard createGlobalField stuff because there is no
	// specific typeId info for SoSFRealTime.
	realTime = new SoSFRealTime;
	// Construct a global field (to add it to the dictionary); we
	// don't actually need a pointer to it.
	(void) new SoGlobalField("realTime", realTime);
	realTime->setValue(SbTime::getTimeOfDay());
	realTime->getContainer()->ref();

	// And setup the sensor to touch it periodically
	realTimeSensor = new SoTimerSensor;
	realTimeSensor->setFunction((SoSensorCB *)
				     &SoDB::realTimeSensorCallback);
#ifdef DEBUG
	if (SoDebug::GetEnv("IV_DEBUG_SENSORS")) {
	    SoDebug::NamePtr("realTimeSensor", realTimeSensor);
	}
#endif

	// This doesn't have to be scheduled very often, because
	// the SoXt viewers update realTime right after a redraw, so
	// anything that is continuously animating will constantly
	// redraw as fast as either swapbuffers() or the redraw (if
	// single-buffered) can occur.
	// I chose 1/12 of a second because it is a multiple of the
	// two most common vide rates-- 60 HZ and 72 HZ
	realTimeSensor->setInterval(SbTime(1.0/12.0));

	// Don't schedule the sensor until something is connected to
	// realTime. If nothing is connected, there's no sense wasting
	// time triggering the sensor.

	// Initialize delay queue timeout to 1/12th of a second to
	// make sure that redraws occur even when event processing is
	// time-consuming.
	setDelaySensorTimeout(SbTime(1.0/12.0));
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns a character string identifying the version of the
//    Inventor library in use.
//
// Use: public, static

const char *
SoDB::getVersion()
//
////////////////////////////////////////////////////////////////////////
{
    return versionString;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Registers callbacks to be invoked when a particular header
//    string is found.  Returns FALSE if the string is not a valid header
//    string. Returns TRUE if the header is successfully registered.
//    Note, nothing prevents you from registering the same string multiple
//    times.

//
// Use: public, static

SbBool
SoDB::registerHeader(const SbString &header, SbBool isBinary, float ivVersion,
		    SoDBHeaderCB *preCB, SoDBHeaderCB *postCB, void *userData)
//
////////////////////////////////////////////////////////////////////////
{
    // Header string cannot be greater than 80 characters in length,
    // and must have at least one character beyond the initial comment char.
    int headerLength = header.getLength();
    if (headerLength > 80 || headerLength < 2)
	return (FALSE);
	
    // The first character must be the comment character
    const char *string = header.getString();
    if (string[0] != '#')
	return (FALSE);
	
    // The string must not contain any newline characters.
    for (int i = 1; i < header.getLength(); i++)
	if (string[i] == '\n')
	    return (FALSE);
	 
    SoDBHeaderData *data = new SoDBHeaderData;
    
    // Binary headers *must* be padded for correct alignment, but to make things 
    // simpler when looking up headers, we'll just pad all headers - including 
    // ascii ones.
    data->headerString = SoOutput::padHeader(header);
    
    data->isBinary = isBinary;
    data->ivVersion = ivVersion;
    data->preCB = preCB;
    data->postCB = postCB;
    data->userData = userData;
    headerList->append(data);
     
    return (TRUE);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Return the number of registered headers
//
// Use: public, static

int
SoDB::getNumHeaders()
//
////////////////////////////////////////////////////////////////////////
{
    return (headerList->getLength());
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Return the i'th header
//
// Use: public, static

SbString
SoDB::getHeaderString(int whichHeader)
//
////////////////////////////////////////////////////////////////////////
{
    if (whichHeader < 0 || whichHeader >= headerList->getLength())
	return (SbString(""));
	
    SoDBHeaderData *data = NULL;
    data = (SoDBHeaderData *) (*headerList)[whichHeader];
    if (!data)
	return (SbString(""));
	
    return (data->headerString);	
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Look up the given header string in our list of registered 
//    headers, and pass back the data associated with that header i.e.
//    pointers to the pre- and post- callbacks, the user data, and
//    a boolean indicating whether it's binary or ascii. 
//    The return value is true if the string is found in the list of
//    registered headers. 
//    If the substringOK flag is TRUE, then we also look for registered
//    headers that are substrings of the given header string. 
//
// Use: public, static

SbBool
SoDB::getHeaderData(const SbString &header, 
		    SbBool &isBinary, float &ivVersion,
		    SoDBHeaderCB *&preCB, SoDBHeaderCB *&postCB, 
		    void *&userData, SbBool substringOK)
//
////////////////////////////////////////////////////////////////////////
{    
    int whichHeader = -1;
    SoDBHeaderData *data;
    SbString paddedHeader = SoOutput::padHeader(header);
    
    // First look for an exact match
    for (int i = headerList->getLength()-1; i >= 0 && whichHeader == -1; i--) {
	data = (SoDBHeaderData *) (*headerList)[i];
	SbString registeredString = data->headerString;
	
	if (paddedHeader == registeredString) {
		whichHeader = i;
		
	} 
    }
    
    // If we didn't find an exact match,
    // look for a substring that is a valid registered string
    if (whichHeader == -1 && substringOK) {
    
	for (int i = headerList->getLength()-1; i >= 0 && whichHeader == -1; i--) {
	    data = (SoDBHeaderData *) (*headerList)[i];
	    SbString registeredString = data->headerString;	
	
	    if (paddedHeader.getLength() >= registeredString.getLength()) {
	    
		// See how much padding there is in the registered header string
		const char *registeredStr = data->headerString.getString();
		int lastNonPadChar = registeredString.getLength() - 1;
		while (registeredStr[lastNonPadChar] == ' ' && lastNonPadChar > 0) 
		    lastNonPadChar--;
		
		// Is the registered header (minus the padding) a substring 
		// of the the given header string?
		if (registeredString.getSubString(0, lastNonPadChar) == 
			paddedHeader.getSubString(0, lastNonPadChar)) {
		    whichHeader = i;			
		}
	    }	    
	}
    }
   
    if (whichHeader == -1) {
	isBinary = FALSE;
	ivVersion = -1;
	preCB = NULL;
	postCB = NULL;
	userData = NULL;
	return (FALSE);
    }
    
    data = (SoDBHeaderData *) (*headerList)[whichHeader];
    isBinary = data->isBinary;
    ivVersion = data->ivVersion;
    preCB = data->preCB;
    postCB = data->postCB;
    userData = data->userData;   
    return (TRUE);	
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if the given character string is a valid registered
//    header string. Some trivial tests that can
//    be made on the string before calling this are: it must begin
//    with a '#'; it should be no more than 80 characters; newlines are
//    not allowed
//
// Use: public, static

SbBool
SoDB::isValidHeader(const char *testString)
//
////////////////////////////////////////////////////////////////////////
{
    char buf[81], *c;

    // Use only the first 80 characters
    strncpy(buf,testString,80);

    // Make sure it is NULL-terminated
    buf[80] = '\0';

    // Find first newline in string, if any, and end the string there
    if ((c = strchr(buf,'\n')) != NULL)
	*c = '\0';

    SbString paddedHeader = SoOutput::padHeader(SbString(buf)); 
       
    for (int i = headerList->getLength()-1; i >= 0; i--) {
	SoDBHeaderData *data = (SoDBHeaderData *) (*headerList)[i];
	if (data->headerString == paddedHeader)
	    return (TRUE);
    }
    
    // Didn't find this string in the list of valid headers.
    return (FALSE);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads a graph from the file specified by the given SoInput,
//    returning a pointer to the resulting root node in rootNode.
//    Returns FALSE on error.
//
// Use: public, static

SbBool
SoDB::read(SoInput *in, SoNode *&rootNode)
//
////////////////////////////////////////////////////////////////////////
{
    SoBase	*base;
    SbBool	ret;

    ret = read(in, base);

    if (base != NULL) {
	if (base->isOfType(SoNode::getClassTypeId()))
	    rootNode = (SoNode *) base;

	else {
	    SoReadError::post(in, "looking for a node but got %s",
			      base->getTypeId().getName().getString());
	    ret = FALSE;
	    
	    // Free the scene we just read by refing/unrefing
	    base->ref();
	    base->unref();
	}
    }

    else
	rootNode = NULL;

    return ret;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads a path from the file specified by the given SoInput,
//    returning a pointer to the resulting path in path. Returns FALSE
//    on error.
//
// Use: public, static

SbBool
SoDB::read(SoInput *in, SoPath *&path)
//
////////////////////////////////////////////////////////////////////////
{
    SoBase	*base;
    SbBool	ret;

    ret = read(in, base);

    if (base != NULL) {
	if (base->isOfType(SoPath::getClassTypeId()))
	    path = (SoPath *) base;

	else {
	    SoReadError::post(in, "looking for a path but got %s",
			      base->getTypeId().getName().getString());
	    ret = FALSE;
	    
	    // Free the scene we just read by refing/unrefing
	    base->ref();
	    base->unref();
	}
    }

    else
	path = NULL;

    return ret;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads all graphs from the file specified by the given SoInput.
//    If there is only one graph in the file and its root is an
//    SoSeparator, a pointer to the root is returned. Otherwise, this
//    creates an SoSeparator, adds the root nodes of all graphs read
//    as children of it, and returns a pointer to it. 
//    
//    If a graph is a path, it will be read and its root returned.
//
//    This returns NULL on error.
//
// Use: public, static

SoSeparator *
SoDB::readAll(SoInput *in)
//
////////////////////////////////////////////////////////////////////////
{
    SoBase	*base;
    SoSeparator *root = new SoSeparator;

    root->ref();

    // Keep on reading until there are no more graphs to read
    do {
	if (! read(in, base)) {
	    root->unref();
	    return NULL;
	}
	else if (base != NULL) {
	    // Did we read a node or a path?
	    if (base->isOfType(SoNode::getClassTypeId()))
		root->addChild((SoNode *) base);
	    else if (base->isOfType(SoPath::getClassTypeId())) {
		SoNode *pathHead = ((SoPath *) base)->getHead();
		if (pathHead != NULL) {
		    pathHead->ref();
		    root->addChild(pathHead);
		    pathHead->unref();
		}
	    }
	}
    } while (base != NULL);

    // If only one graph was read, and it had a separator for a root,
    // get rid of the one we created
    if (root->getNumChildren() == 1 &&
	root->getChild(0)->isOfType(SoSeparator::getClassTypeId())) {

	SoSeparator *graphRoot = (SoSeparator *) root->getChild(0);
	graphRoot->ref();
	root->unref();
	root = graphRoot;
    }

    root->unrefNoDelete();
    return root;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Just like UNIX select() call, but does our tasks while waiting.
//    Can be used in applications with their own event loops.
//
// Use: public, static

int
SoDB::doSelect(int nfds, fd_set *readfds, fd_set *writefds,
	       fd_set *exceptfds, struct timeval *userTimeOut)
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (globalDB == NULL) {
	SoDebugError::post("SoDB::doSelect", "SoDB::init() was never called");
	return 0;
    }
#endif /* DEBUG */

    return globalDB->sensorManager.doSelect(nfds, readfds, writefds,
					    exceptfds, userTimeOut);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Registers a field conversion engine that can be used to
//    convert from one type of field to another. The type id's of the
//    two fields are passed in, as is the type id of the field
//    converter engine (derived from SoFieldConverter).
//
// Use: extender, static

void
SoDB::addConverter(SoType fromField, SoType toField, SoType converterFunc)
{

#ifdef DEBUG
    // Make sure the converter is of the correct type
    if (! converterFunc.isDerivedFrom(SoFieldConverter::getClassTypeId())) {
	SoDebugError::post("SoDB::addConverter",
			   "class \"%s\" is not derived from SoFieldConverter",
			   converterFunc.getName().getString());
	return;
    }
#endif

    conversionDict->enter(getConversionKey(fromField, toField),
			  * (void **) &converterFunc);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the field conversion engine registered for the two
//    given field types. If no such engine exists, SoType::badType()
//    is returned.
//
// Use: extender, static

SoType
SoDB::getConverter(SoType fromField, SoType toField)
{
    void	*typePtr;

    if (conversionDict->find(getConversionKey(fromField, toField), typePtr))
	return * (SoType *) &typePtr;

    return SoType::badType();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Accesses sensor manager
//
// Use: internal, static

SoSensorManager *
SoDB::getSensorManager()
//
////////////////////////////////////////////////////////////////////////
{
    return &globalDB->sensorManager;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Enables/disables realTime sensor processing.
//
// Use: internal, static

void
SoDB::enableRealTimeSensor(SbBool enable)
//
////////////////////////////////////////////////////////////////////////
{
    if (enable && realTimeSensor->getInterval() != SbTime::zero()) {

	// If we are enabling the sensor now, call the callback once
	// to set the current time. Since the sensor may become
	// enabled because of a new connection, the realTime field may
	// be queried before the sensor callback is called. By calling
	// it now, the realTime field will contain the current time in
	// this case. However, we have to disable notification on the
	// realTime field since we area already in the process of
	// notifying.
	SbBool wasEnabled = realTime->enableNotify(FALSE);
	realTimeSensorCallback();
	realTime->enableNotify(wasEnabled);
	realTimeSensor->schedule();
    }
    else
	realTimeSensor->unschedule();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads an SoBase from the file specified by the given SoInput,
//    returning a pointer to the resulting instance in base. Returns
//    FALSE on error.
//
// Use: private, static

SbBool
SoDB::read(SoInput *in, SoBase *&base)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool	ret;
    const char	*dataFileName;
    char	*searchPath = NULL;

#ifdef DEBUG
    if (globalDB == NULL) {
	SoDebugError::post("SoDB::read", "SoDB::init() was never called");
	return FALSE;
    }
#endif /* DEBUG */

    // Before reading, see if the SoInput is reading from a named
    // file. If so, make sure the directory search path in the SoInput
    // is set up to read from the same directory the file is in.
    dataFileName = in->getCurFileName();

    if (dataFileName != NULL) {
	const char *slashPtr;

	// Set up the directory search stack if necessary. Look for
	// the last '/' in the path. If there is none, there's no
	// path. Otherwise, remove the slash and everything after it.
	if ((slashPtr = strrchr(dataFileName, '/')) != NULL) {
	    searchPath = strdup(dataFileName);
	    searchPath[slashPtr - dataFileName] = '\0';
	    SoInput::addDirectoryFirst(searchPath);
	}
    }

    ret = SoBase::read(in, base, SoBase::getClassTypeId());

    // If no valid base was read, but we haven't hit EOF, that means
    // that there's extra crap in the input that's not an Inventor
    // thing. If so, report an error.
    if (ret && base == NULL && ! in->eof()) {
	char	c;
	in->get(c);
	SoReadError::post(in, "Extra characters ('%c') found in input", c);
	ret = FALSE;
    }

    // Clean up directory list if necessary
    if (searchPath != NULL) {
	SoInput::removeDirectory(searchPath);
	free(searchPath);
    }

    return ret;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Create a global field with the given name and type.
//
// Use: public, static

SoField *
SoDB::createGlobalField(const SbName &name, SoType type)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool alreadyExists;
    SoGlobalField *result = SoGlobalField::create(name, type, alreadyExists);

    if (result == NULL)
	return NULL;
    
    return result->getMyField();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Get the global field with the given name.
//
// Use: public, static

SoField *
SoDB::getGlobalField(const SbName &name)
//
////////////////////////////////////////////////////////////////////////
{
    SoGlobalField *result = SoGlobalField::find(name);
    if (result == NULL) return NULL;
    
    return result->getMyField();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Rename the global field with the given name.
//
// Use: public, static

void
SoDB::renameGlobalField(const SbName &oldName, const SbName &newName)
//
////////////////////////////////////////////////////////////////////////
{
    if (oldName == newName) return;
    
    SoGlobalField *oldGlobalField = SoGlobalField::find(oldName);
    if (oldGlobalField == NULL) return;
    
    if (newName == SbName("")) {
	oldGlobalField->unref();
	return;
    }
    
    SoGlobalField *newGlobalField = SoGlobalField::find(newName);
    if (newGlobalField != NULL) {
	newGlobalField->unref();
    }

    oldGlobalField->changeName(newName);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Set how often we should touch realTime.
//
// Use: public, static

void
SoDB::setRealTimeInterval(const SbTime &deltaT)
//
////////////////////////////////////////////////////////////////////////
{
    if (deltaT == SbTime::zero()) {
	realTimeSensor->setInterval(deltaT);
	realTimeSensor->unschedule();
    }
    else {
	realTimeSensor->setInterval(deltaT);
	realTimeSensor->schedule();
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Get how often realTime is touched.
//
// Use: public, static

const SbTime &
SoDB::getRealTimeInterval()
//
////////////////////////////////////////////////////////////////////////
{
    return realTimeSensor->getInterval();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Set the delay queue sensor timeout.
//
// Use: public, static

void
SoDB::setDelaySensorTimeout(const SbTime &t)
//
////////////////////////////////////////////////////////////////////////
{
    getSensorManager()->setDelaySensorTimeout(t);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Get the delay queue sensor timeout.
//
// Use: public, static

const SbTime &
SoDB::getDelaySensorTimeout()
//
////////////////////////////////////////////////////////////////////////
{
    return getSensorManager()->getDelaySensorTimeout();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//	Callback to update the realTime global field.
//
// Use: private

void
SoDB::realTimeSensorCallback()
//
////////////////////////////////////////////////////////////////////////
{
    realTime->setValue(SbTime::getTimeOfDay());
}

