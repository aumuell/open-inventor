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
 |   $Revision: 1.1 $
 |
 |   Description:
 |	Definition of SoBase, the base class for several other Inventor
 |	classes. This class handles reference counting and
 |	notification and is the main entry point for reading and
 |	writing derived classes.
 |
 |   Author(s)		: Paul S. Strauss, Nick Thompson, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_BASE_
#define  _SO_BASE_

#include <Inventor/misc/SoBasic.h>
#include <Inventor/misc/SoAuditorList.h>
#include <Inventor/SbString.h>
#include <Inventor/SoType.h>

class SoBaseList;
class SoInput;
class SoNode;
class SoNotList;
class SoOutput;
class SoPath;

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoBase
//
//  Base class for most important SO classes. All subclasses of this
//  class may be read and written, in the form:
//	NameOfClass {
//	    <stuff inside>
//	}
//  where NameOfClass is the thing returned by getFileName(). For example,
//  the "FileName" of the SoPath class is "Path".
//
//////////////////////////////////////////////////////////////////////////////

class SoBase {
  public:

    // Adds a reference to an instance
    // This generates a C++ warning.
    void			ref() const;

    // Removes a reference to an instance, deleting it if count is now 0
    void			unref() const;

    // Removes a reference to an instance, NOT deleting it if count is now 0
    void			unrefNoDelete() const;

    // Marks an instance as modified, simulating a change to it. This
    // will notify all auditors of the instance.
    void			touch()		{ startNotify(); }

    // Returns type identifier for SoBase class
    // C-api: expose
    static SoType		getClassTypeId() { return classTypeId; }

    // Returns type identifier for base
    // C-api: expose
    virtual SoType		getTypeId() const = 0;

    // Returns TRUE if base is of given type or is derived from it
    SbBool			isOfType(SoType type) const;

    // Routines for naming objects:
    // getName() returns SbName("") if the object has no name.
    // C-api: expose
    virtual SbName		getName() const;

    // setName(SbName("")) to un-name an object
    // C-api: expose
    virtual void		setName(const SbName &name);

  protected:
    // Constructor is protected - this is an abstract class
    SoBase();

    // Virtual destructor so that subclasses are deleted properly
    virtual ~SoBase();

    // Actually deletes an instance. Allows subclasses to do other
    // stuff before the deletion if necessary.
    virtual void		destroy();

    // Returns current write counter
    static uint32_t	getCurrentWriteCounter()
	{ return currentWriteCounter; }

    // Returns TRUE if the instance has multiple write references
    SbBool			hasMultipleWriteRefs() const
	{ return writeStuff.multWriteRef; }

    // Writes a header (name, open brace) or footer (close brace) to
    // file defined by SoOutput. writeHeader returns TRUE if no
    // further writing is necessary for an instance.
    // isEngine/isGroup are exactly what they sound like, and must be
    // passed in so that unknown nodes/engines can be correctly
    // created when reading the binary file format.
    SbBool	writeHeader(SoOutput *out,
			    SbBool isGroup, SbBool isEngine) const;
    void	writeFooter(SoOutput *out) const;

    // Unknown nodes and engines write a different name for themselves
    // than their typeId; this virtual method lets them do that (by
    // default the typeId name is returned)
    virtual const char *getFileFormatName() const;

    // This set of enums is used when reading and writing the base.
    // Each enum represents a different bit in a bit field
    // which will be written.
    enum BaseFlags {
        IS_ENGINE        = 1,
        IS_GROUP         = 2
    };

    // Reads stuff into instance of subclass. Return FALSE on error.
    // If reading binary file format, the flags specify whether the
    // object was written as an engine or a group; unknown nodes and
    // groups need this information to read themselves in properly.
    virtual SbBool	readInstance(SoInput *in, unsigned short flags) = 0;

  SoINTERNAL public:

    // Setup type information
    static void		initClass();

    // Increments the current write counter at the start of a write operation
    static void		incrementCurrentWriteCounter()
	{ currentWriteCounter++; }

    // Decrements the current write counter after a write operation,
    // in some rare cases
    static void		decrementCurrentWriteCounter()
	{ currentWriteCounter--; }

    // Initiates notification from an instance. The default method
    // does nothing, because some classes (path, sensor) never
    // initiate notification. This is used by touch().
    virtual void	startNotify();

    // Propagates modification notification through an instance. The
    // default method here does not create and add a new record. It
    // merely propagates the current record list to all auditors. This
    // method may be used by subclasses to do the propagation after
    // modifying the list appropriately.
    virtual void	notify(SoNotList *list);

    // Adds/removes an auditor to/from list
    void		addAuditor(void *auditor, SoNotRec::Type type);
    void		removeAuditor(void *auditor, SoNotRec::Type type);

    // Returns auditor list-- used by SoField and SoEngineOutput to
    // trace forward connections
    const SoAuditorList &getAuditors() { return auditors; }

    // Turns on/off reference count tracing (for debugging)
    static SbBool	traceRefs;

    // Internal methods used to maintain the global name dictionary
    static void		addName(SoBase *, const char *);
    static void		removeName(SoBase *, const char *);

    // Helper routines used to get stuff out of nameDict
    static SoBase	*getNamedBase(const SbName &, SoType);
    static int		getNamedBases(const SbName &, SoBaseList &,
				      SoType);

    // Returns current reference count
    int			getRefCount() const	{ return refCount; }

    // Reads one instance of some subclass of SoBase. Returns pointer
    // to read-in instance in base, or NULL on EOF. Returns FALSE on
    // error. The last parameter is a subclass type to match. If
    // the returned base is not of this type, it is an error. A type
    // of SoBase::getClassTypeId() will match any base.
    static SbBool	read(SoInput *in, SoBase *&base,
			     SoType expectedType);

    // Adds a reference to the instance when writing. isFromField
    // indicates whether the reference is from a field-to-field
    // connection.
    virtual void	addWriteReference(SoOutput *out,
					  SbBool isFromField = FALSE);

    // Returns TRUE if the instance should be written, based on the
    // write-reference info already accumulated
    SbBool		shouldWrite();

    // This defaults to "+" and is used when naming nodes that are DEF's and USE'd.
    // The ivdowngrade converter needs to set it to other than "+" since that was
    // an illegal character for Inventor V1.0 names.
    static void		setInstancePrefix(const SbString &c) { instancePrefix = c; }
  
  private:

    static SoType	classTypeId;
    
    static SbString	instancePrefix;

    // This is incremented once per write operation. It is used to
    // determine which instances to write.
    static uint32_t currentWriteCounter;

    // Reference count
    int			refCount;

    // List of auditors: objects to pass notification to
    SoAuditorList	auditors;    

    // These are all packed into one word to save space
    struct {

	// This contains the value of the counter the last time the
	// instance was written.
	unsigned int writeCounter : 29;

	// This is TRUE if the object is named.
	unsigned int hasName : 1;

	// This is TRUE if more than one reference is made to the
	// instance for writing, meaning that we need to DEF it
	unsigned int multWriteRef : 1;

	// This is TRUE if the instance was referenced via a
	// field-to-field connection, which is not a "real" reference
	unsigned int writeRefFromField : 1;

    } writeStuff;

    // These are used internally by writeHeader()
    void		writeDef(SoOutput *, int) const;
    void		writeRef(SoOutput *, int) const;
    void		writeAnnotation(SoOutput *) const;

    // These are all used internally by read()
    static SbBool	readReference(SoInput *in, SoBase *&base);
    static SbBool	readBase(SoInput *in, SbName &className,
				 SoBase *&base);
    static SbBool	readBaseInstance(SoInput *in, const SbName &className,
					 const SbName &refName, SoBase *&base);
    static SoBase	*createInstance(SoInput *in, SbName className,
					unsigned short ioFlags);
    static void		flushInput(SoInput *in);

    // This dictionary stores SbPLists keyed by name (the SbPLists
    // contain SoBases-- a BaseList isn't used because we don't want
    // the items on the list to be reference counted, otherwise they
    // will never get deleted).
    static SbDict	*nameObjDict;

    // And this dictionary maps the other way, from an SoBase * to a
    // name.
    static SbDict	*objNameDict;
};

#endif /* _SO_BASE_ */
