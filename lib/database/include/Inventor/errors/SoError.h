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
 |	This file defines the base SoError class.
 |
 |   Classes:
 |	SoError
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_ERROR
#define  _SO_ERROR

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoError
//
//  This is the base class for all error classes, which provide error
//  handling for applications.
//
//  There are two facets to errors: posting and handling. An error is
//  posted when some bad condition occurs. Posting is done primarily
//  by the Inventor library itself, but extenders can post their own
//  errors. Posting an error creates an instance of the appropriate
//  error class (or subclass) and then passes it to the active error
//  handler. The default handler just prints an appropriate message to
//  stderr. Applications can override this behavior by supplying a
//  different handler (by specifying a callback function).
//
//  Each subclass of SoError supports the setHandlerCallback() method,
//  which is used to set the callback function to handle errors. The
//  callback for the most derived class of a specific error instance
//  is used to handle an event.
//
//  The error instance passed to a callback is deleted immediately
//  after the callback is called; an application that wishes to save
//  info from the instance has to copy it out first.
//
//  Each error class contains a run-time class type id (SoType) that
//  can be used to determine the type of an instance. The base class
//  defines a character string that represents a detailed error
//  message that is printed by the default handler.
//
//  All handlers are called by the SoError::handleError() method. When
//  debugging, you can set a breakpoint on this method to stop right
//  before an error is handled.
//
//////////////////////////////////////////////////////////////////////////////

#include <Inventor/SbString.h>
#include <Inventor/SoType.h>

class SoError;		// Forward reference

class SoBase;
class SoEngine;
class SoNode;
class SoPath;

// Error handling callbacks are of this type:
typedef void	SoErrorCB(const SoError *error, void *data);

// C-api: prefix=SoErr
class SoError {

  public:
    // Sets/returns handler callback for SoError class
    // C-api: name=setHndlrCB
    static void		setHandlerCallback(SoErrorCB *cb, void *data)
	{ handlerCB = cb; cbData = data; }
    // C-api: name=getHndlrCB
    static SoErrorCB *	getHandlerCallback()	{ return handlerCB; }
    // C-api: name=getHndlrData
    static void *	getHandlerData()	{ return cbData; }

    // Returns debug string containing full error info from instance
    // C-api: name=getDbgStr
    const SbString &	getDebugString() const { return debugString; }

    // Returns type identifier for SoError class
    static SoType	getClassTypeId()	{ return classTypeId; }

    // Returns type identifier for error instance
    // C-api: expose
    virtual SoType	getTypeId() const;

    // Returns TRUE if instance is of given type or is derived from it
    SbBool		isOfType(SoType type) const;

  SoEXTENDER public:
    // Posts an error. The debugString will be created from the given
    // arguments, which are in printf() format
    static void		post(const char *formatString ...);

    // These are convenience functions that return a printable string
    // representing the given object. For example, a node is
    // represented by name (if it has one) and by address; the
    // returned string will be something like:
    //    node named "squid" at address 0x1004dcba
    static SbString	getString(const SoNode *node);
    static SbString	getString(const SoPath *path);
    static SbString	getString(const SoEngine *engine);

  SoINTERNAL public:
    // Initializes SoError class
    static void		initClass();

    // Initialize ALL Inventor error classes
    static void		initClasses();

  protected:
    // The default error handler callback - it just prints to stderr
    static void		defaultHandlerCB(const SoError *error, void *data);

    // Returns handler callback (and data) to use for a given instance
    virtual SoErrorCB *	getHandler(void *&data) const;

    // Sets/appends to the debug string
    void		setDebugString(const char *string)
	{ debugString = string; }
    void		appendToDebugString(const char *string)
	{ debugString += string; }

    // Calls appropriate handler for an error instance. Application
    // writers can set breakpoints at this when debugging.
    void		handleError();

  private:
    static SoType	classTypeId;	// Type id of SoError class
    static SoErrorCB	*handlerCB;	// Handler callback for SoError class
    static void		*cbData;	// User data for callback
    static SbBool	wasInitted;	// TRUE if error class initialized
    SbString		debugString;	// Detailed error message string

    // The getString() methods use this one
    static SbString	getBaseString(const SoBase *base, const char *what);
};

#endif /* _SO_ERROR */
