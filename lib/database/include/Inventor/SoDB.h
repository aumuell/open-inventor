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
 |   $Revision: 1.1 $
 |
 |   Description:
 |	Defines the SoDB class
 |
 |   Author(s)		: Paul S. Strauss, Nick Thompson, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_DB_
#define  _SO_DB_

#include <Inventor/sensors/SoSensorManager.h>
#include <Inventor/SoType.h>
#include <Inventor/SbPList.h>

class SoBase;
class SoSFRealTime;
class SoSeparator;
class SoInput;
class SoPath;
class SoNode;
class SoField;

typedef void SoDBHeaderCB(void *userData, SoInput *in);

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoDB
//
//  Inventor database class.  This class maintains all the global
//  information necessary for reading and writing, naming classes, and
//  so on. All public methods on this class are static - users should
//  never explicitly create instances of this class. Initialization of
//  the database class causes a global instance to be created.
//
//////////////////////////////////////////////////////////////////////////////

class SoDB {

  public:
    // Initialization routine. Creates global database
    static void		init();

    // Returns a character string identifying the version of the
    // Inventor library in use
    static const char	*getVersion();

    // Reads a graph from the file specified by the given SoInput,
    // returning a pointer to the resulting root node in rootNode.
    // Returns FALSE on error.
    static SbBool	read(SoInput *in, SoNode *&rootNode);

    // Reads a path from the file specified by the given SoInput,
    // returning a pointer to the resulting path in path. Returns
    // FALSE on error.
    // C-api: name=ReadPath
    static SbBool	read(SoInput *in, SoPath *&path);

    // Reads all graphs from the file specified by the given SoInput.
    // If there is only one graph in the file and its root is an
    // SoSeparator, a pointer to the root is returned. Otherwise, this
    // creates an SoSeparator, adds the root nodes of all graphs read
    // as children of it, and returns a pointer to it. This returns
    // NULL on error.
    static SoSeparator	*readAll(SoInput *in);

    // Register callbacks to be invoked when a particular header string
    // is found.  Returns FALSE if the string is not a valid header
    // string. A valid string must start with '#' and cannot exceed 
    // 80 characters in length. Returns TRUE if the header is successfully 
    // registered. Note, nothing prevents you from registering the same 
    // string multiple times.
    static SbBool	registerHeader(const SbString &headerString,
					SbBool isBinary, 
					float ivVersion, 
					SoDBHeaderCB *preCB,
					SoDBHeaderCB *postCB,
					void *userData = NULL);
    // Returns TRUE if the given header string is found in the list of 
    // registered headers.  If the substringOK flag is TRUE, then
    // also returns TRUE if a subset of the given string is found.
    static SbBool	getHeaderData(const SbString &string,
					SbBool &isBinary,  
					float &ivVersion,
					SoDBHeaderCB *&preCB, 
					SoDBHeaderCB *&postCB, 
					void *&userData, 
					SbBool substringOK = FALSE);					    
    static int		getNumHeaders();
    static SbString	getHeaderString(int i);
    
    // Returns TRUE if the given character string is a valid registered
    // header string. Some trivial tests that can
    // be made on the string before calling this are: it must begin
    // with a '#'; it should be no more than 80 characters; newlines are
    // not allowed
    static SbBool	isValidHeader(const char *testString);

    // Get a global field of the given name and type.  This will
    // return NULL if there is already a global field with the given
    // name but a different type.  There is only ever one global field
    // with a given name.
    static SoField *	createGlobalField(const SbName &name,
					  SoType type);
    
    // Get the global field wih the given name.  It will be a type
    // derived from SoField.
    static SoField *	getGlobalField(const SbName &name);

    // Give a global field a different name.  Naming it ""
    // deletes it.  Giving it the name of another global field deletes
    // the other global field.
    static void		renameGlobalField(const SbName &oldName,
					  const SbName &newName);

    // Sets/returns the realTime global field update interval (default
    // is 60 times/second).  Setting this to zero turns off realTime
    // update.
    static void		setRealTimeInterval(const SbTime &deltaT);
    static const SbTime &getRealTimeInterval();

    // Set/returns delay queue sensors (OneShot, Node, Path, etc)
    // timeout value.  Delay sensor go off whenever there is idle time
    // or when the timeout expires.
    static void		setDelaySensorTimeout(const SbTime &t);
    static const SbTime &getDelaySensorTimeout();

    // Just like UNIX select() call, but does our tasks while waiting.
    // Can be used in applications with their own event loops.
    static int		doSelect(int nfds, fd_set *readfds, fd_set *writefds,
				 fd_set *exceptfds,
				 struct timeval *userTimeOut);

  SoEXTENDER public:
    // Registers a field conversion engine that can be used to
    // convert from one type of field to another. The type id's of the
    // two fields are passed in, as is the type id of the field
    // converter engine (derived from SoFieldConverter).
    static void		addConverter(SoType fromField, SoType toField,
				     SoType converterEngine);

    // Returns the field conversion engine registered for the two
    // given field types. If no such engine exists,
    // SoType::badType() is returned.
    static SoType	getConverter(SoType fromField, SoType toField);

  SoINTERNAL public:
    // Accesses sensor manager
    static SoSensorManager *	getSensorManager();

    // Returns TRUE if database is initialized (for error checking)
    static SbBool	isInitialized()	{ return (globalDB != NULL); }

    // This is called when some instance begins or ends a notification
    // process. It increments or decrements a counter of notifications
    // in progress. When the counter reaches 0, all priority 0
    // (immediate) delay queue sensors are triggered. By doing this,
    // all notification has a chance to finish before any evaluation
    // (due to data sensors, primarily) takes place.
    static void		startNotify()		{ notifyCount++; }
    static SbBool	isNotifying()		{ return (notifyCount > 0); }
    static void		endNotify()
	{ if (--notifyCount == 0)
	      globalDB->sensorManager.processImmediateQueue(); }

    // Enables/disables realTime sensor processing
    static void		enableRealTimeSensor(SbBool enable);

    // Reads a base of any type, returning a pointer to it. Returns
    // FALSE on error.
    static SbBool	read(SoInput *in, SoBase *&base);
    
  private:
    static const char	*versionString;	// Returned by getVersion()
    static SoDB		*globalDB;	// Pointer to the global database
    SoSensorManager	sensorManager;	// Its global sensor manager 	

    static int		notifyCount;	// How many current notifications

    static SoTimerSensor *realTimeSensor;// touches realTime global field 
    static void		realTimeSensorCallback();
    static SoSFRealTime	*realTime;
    
    // List of valid header strings, and their corresponding callbacks
    static SbPList	*headerList;

    // This dictionary stores field conversion engine types. The key
    // is created by mangling the types of the two fields to convert
    // from. The dictionary maps the key to the type of the conversion
    // engine.
    static SbDict	*conversionDict;

    // Returns a conversionDict key from the two given field type id's.
    static uint32_t 	getConversionKey(SoType fromField, SoType toField)
	{ return (fromField.getKey() << 16) | toField.getKey(); }

    static void		iv2_1PreCallback(void *userData, SoInput *in);
    static void		iv2_0PreCallback(void *userData, SoInput *in);
    static void		iv1_0PreCallback(void *userData, SoInput *in);
    static void		iv2_0PostCallback(void *userData, SoInput *in);
    static void		iv1_0PostCallback(void *userData, SoInput *in);
    
};

#endif /* _SO_DB_ */
