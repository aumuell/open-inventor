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
 |   $Revision: 1.2 $
 |
 |   Description:
 |	This file defines the SoMemoryError class.
 |
 |   Classes:
 |	SoMemoryError
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_MEMORY_ERROR
#define  _SO_MEMORY_ERROR

#include <Inventor/errors/SoError.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoMemoryError
//
//  This class is used for errors reported due to lack of memory. The
//  post() method takes a string describing the item that was to be
//  allocated, such as "cylinder vertex array".
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoMemErr

#if _COMPILER_VERSION>=710
#  pragma set woff 1375
#endif

class SoMemoryError : public SoError {

  public:
    // Sets/returns handler callback for SoMemoryError class
    // C-api: name=setHndlrCB
    static void		setHandlerCallback(SoErrorCB *cb, void *data)
	{ handlerCB = cb; cbData = data; }
    // C-api: name=getHndlrCB
    static SoErrorCB *	getHandlerCallback()	{ return handlerCB; }
    // C-api: name=getHndlrData
    static void *	getHandlerData()	{ return cbData; }

    // Returns type identifier for SoMemoryError class
    static SoType	getClassTypeId()	{ return classTypeId; }

    // Returns type identifier for error instance
    virtual SoType	getTypeId() const;

  SoEXTENDER public:
    // Posts an error
    static void		post(const char *whatWasAllocated);

  SoINTERNAL public:
    // Initializes SoMemoryError class
    static void		initClass();

  protected:
    // Returns handler callback (and data) to use for a given instance
    virtual SoErrorCB *	getHandler(void *&data) const;

    virtual ~SoMemoryError() {}

  private:
    static SoType	classTypeId;	// Type id of SoMemoryError class
    static SoErrorCB	*handlerCB;	// Handler callback for class
    static void		*cbData;	// User data for callback
};

#if _COMPILER_VERSION>=710
#  pragma reset woff 1375
#endif

#endif /* _SO_MEMORY_ERROR */
