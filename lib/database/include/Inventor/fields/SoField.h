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
 |	Defines the SoField class, which is the base class for all fields.
 |
 |   Author(s)		: Paul S. Strauss, Nick Thompson, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_FIELD_
#define  _SO_FIELD_

#include <Inventor/SbString.h>
#include <Inventor/SoType.h>
#include <Inventor/misc/SoNotification.h>

class SoEngineOutput;
class SoFieldContainer;
class SoFieldAuditorInfo;
class SoFieldConverter;
class SoFieldList;
class SoInput;
class SoNotList;
class SoOutput;

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoField
//
//  Base class for all kinds of fields.  SoField maintains the state
//  (ignored, modified, default, ...) of the field.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: abstract
class SoField {
  public:
    // Destructor
    // C-api: expose
    virtual ~SoField();

    // Sets/returns ignored flag
    void		setIgnored(SbBool ig);
    SbBool		isIgnored() const	{ return flags.ignored; }

    // Returns default flag
    // C-api: name=isDflt
    SbBool		isDefault() const	{ return flags.hasDefault; }

    // Returns type identifier for SoField class
    // C-api: expose
    static SoType	getClassTypeId()	{ return classTypeId; }

    // Returns type identifier for field
    // C-api: expose
    virtual SoType	getTypeId() const = 0;

    // Returns TRUE if field is of given type or is derived from it
    SbBool		isOfType(SoType type) const;

    // Sets/returns whether connection to field or engine is
    // enabled. This flag may be set even if no connection is
    // currently established. The flag will remain in effect until
    // changed again.
    // C-api: name=enableConn
    void		enableConnection(SbBool flag);
    // C-api: name=isConnEnabled
    SbBool		isConnectionEnabled() const
	{ return flags.connectionEnabled; }

    // Connects the field to the given output of an engine or to
    // another field. Returns FALSE if the connection could not be made.
    // C-api: name=connFrom
    SbBool		connectFrom(SoEngineOutput *engineOutput);
    // C-api: name=connFromField
    SbBool		connectFrom(SoField *field);

    // Disconnects the field from whatever it's connected to. Harmless
    // if not already connected.
    // C-api: name=disconn
    void		disconnect();

    // Returns TRUE if the field is connected. The last two return
    // TRUE if the field is connected specifically to an engine
    // output or field.
    // C-api: name=isConn
    SbBool		isConnected() const	{ return flags.connected; }
    // C-api: name=isConnFromEngine
    SbBool		isConnectedFromEngine() const
	{ return (flags.connected &&   flags.fromEngine); }
    // C-api: name=isConnFromField
    SbBool		isConnectedFromField() const
	{ return (flags.connected && ! flags.fromEngine); }

    // Returns the engine output or field the field is connected to.
    // Returns FALSE if there is no connection of the appropriate type.
    // C-api: name=getConnEngine
    SbBool		getConnectedEngine(SoEngineOutput *&engineOutput) const;
    // C-api: name=getConnField
    SbBool		getConnectedField(SoField *&field) const;

    // Returns the number of fields (in Engines or Nodes) that this
    // field is writing to, and adds pointers to those fields to the
    // given field list.
    // C-api: name=getForwardConn
    int			getForwardConnections(SoFieldList &list) const;

    // Returns the containing node or engine
    SoFieldContainer *	getContainer() const;

    // Sets value of field from the Inventor data file format
    // information in the value string.  Returns TRUE if successful,
    // FALSE otherwise.
    // C-api: name=setFromStr
    SbBool		set(const char *valueString);

    // Stores field value (in the same format expected by the set()
    // method) in the given SbString
    // C-api: name=getIntoStr
    void		get(SbString &valueString);

    // Simulates a change to the field data, causing attached sensors
    // to fire, connected engines to be marked as needing evaluation, etc.
    // Equivalent to performing setValue(getValue()) on a derived class
    // instance, except that the isDefault() status is unchanged.
    virtual void	touch();

    // Returns TRUE/FALSE if the given field is of the same type and
    // has the same value(s) as this
    int			operator ==(const SoField &f) const
	{ return isSame(f); }
    int			operator !=(const SoField &f) const
	{ return ! isSame(f); }

  SoEXTENDER protected:
    // Constuctor:
    SoField();

  SoINTERNAL public:
    static void		initClass();

    // Initialize ALL Inventor node classes
    static void		initClasses();

    // Sets default flag
    void		setDefault(SbBool def)	{ flags.hasDefault = def; }

    // Initiates or propagates notification through container
    virtual void	startNotify();
    virtual void	notify(SoNotList *list);

    // Sets the containing node.  This also calls enableNotify(TRUE)
    // and setDefault(TRUE).
    void		setContainer(SoFieldContainer *cont);

    // Returns TRUE if the field really needs to be written out.
    // Fields with default values that aren't ignored and
    // aren't connected to anything will return FALSE.
    SbBool		shouldWrite() const;

    // Adds/removes an auditor to/from list
    void		addAuditor(void *auditor, SoNotRec::Type type);
    void		removeAuditor(void *auditor, SoNotRec::Type type);

    // Indicates whether notification will propagate as the result of
    // setting the field value. Engines turn this off when writing
    // results into fields, since notification has already propagated.
    SbBool		enableNotify(SbBool flag);
    SbBool		isNotifyEnabled() const
	{ return flags.notifyEnabled; }

    // Indicates to a field that a change has been made involving a
    // connection from it (as source) to another field. Passed the
    // number of things being connected to the field; the number will
    // be negative when things are disconnected.
    // The default method does nothing.
    virtual void	connectionStatusChanged(int numConnections);

    // If this returns TRUE, it means we're in the middle of doing a
    // setValue()+valueChanged() and values from an upstream
    // connection shouldn't write into this field.
    SbBool		isReadOnly() const { return flags.readOnly; }

    // Returns TRUE if the given field is of the same type and has the
    // same value(s) as this. Subclasses must define this as well as
    // an == operator.
    virtual SbBool	isSame(const SoField &f) const = 0;

    // Copies the value from one field to another, assuming same subclass
    virtual void	copyFrom(const SoField &f) = 0;

    // After a field value has been copied using copyFrom(),
    // this is called to allow fields to update the copy. This is used
    // by node, engine, and path fields to make sure instances are
    // handled properly. The default implementation does nothing.
    virtual void	fixCopy(SbBool copyConnections);

    // This returns TRUE if this field contains a reference to a node
    // or engine that is copied during a copy operation (i.e., it is
    // "inside"). The default method just checks if the field is
    // connected to such a node or engine. Subclasses may contain
    // other tests, such as those that contain pointers to nodes or
    // engines.
    virtual SbBool	referencesCopy() const;

    // Copies connection from one field to another. Assumes fields are
    // the same subclass and that this field is connected.
    void		copyConnection(const SoField *fromField);

    // Reads value of field (with given name) from file as defined by
    // SoInput. This does the work common to all fields, then calls
    // other read methods to do the rest.
    virtual SbBool	read(SoInput *in, const SbName &name);

    // Writes field (with given name) to file as defined by SoOutput
    virtual void	write(SoOutput *out, const SbName &name) const;

    // Counts write-references on field to prepare for writing
    virtual void	countWriteRefs(SoOutput *out) const;

    // Evaluates the field from whatever it's connected to. If
    // there's no connection or we don't need to evaluate, this does
    // nothing. This has to be const because it's used by getValue
    // methods.
    void		evaluate() const
	{ if (flags.dirty) evaluateConnection(); }

  protected:
    // Called by an instance to indicate that a value has changed. If
    // resetDefault is TRUE, this turns off default flag. Initiates
    // notification, if necessary.
    void		valueChanged(SbBool resetDefault = TRUE);

    // Evaluates the field or engine the field is connected to,
    // storing the result in the field. This is const because it is
    // called by evaluate().
    virtual void	evaluateConnection() const;

    // Reads value(s) of field
    virtual SbBool	readValue(SoInput *in) = 0;

    // Reads connection.
    virtual SbBool	readConnection(SoInput *in);

    // Writes value(s) of field
    virtual void	writeValue(SoOutput *out) const = 0;

    // Writes field connection
    virtual void	writeConnection(SoOutput *out) const;

  private:
    static SoType	classTypeId;

    // These are used by SoField::get() to hold the returned field string
    static char		*fieldBuf;
    static int		fieldBufSize;

    // The "flags" field contains several bit flags:
    struct {
	unsigned int hasDefault		: 1; // Field is set to default value
	unsigned int ignored		: 1; // Field value is to be ignored
	unsigned int connected		: 1; // Field connected from something
	unsigned int converted		: 1; // Connection required converter
	unsigned int fromEngine		: 1; // Connection is from engine
	unsigned int connectionEnabled	: 1; // Connection is enabled
	unsigned int notifyEnabled	: 1; // Notification is enabled
	unsigned int hasAuditors	: 1; // Connected, or FieldSensor
	unsigned int isEngineModifying	: 1; // Engine evaluating
	unsigned int readOnly		: 1; // Must not write into
					     // this field
	unsigned int dirty		: 1; // Field was notified and
					     // needs evaluation
    }			flags;

    // If no other auditors, the container for this field is stored
    // directly here.  If the field has other auditors (flags.hasAuditors)
    // then the connection information and the container are
    // stored in an opaque SoFieldAuditorInfo structure.  This is
    // done to save space in the common case.
    union {
	SoFieldContainer *container;
	SoFieldAuditorInfo *auditorInfo;
    };

    // Creates auditorInfo strucuture, if necessary:
    void		createAuditorInfo();

    // Creates a converter engine to convert from the given field
    // type to the type of this field. Returns NULL on error.
    SoFieldConverter	*createConverter(const SoType &fromFieldType) const;

    // Really disconnects field from whatever it's connected to
    void		reallyDisconnect();

    // Returns pointer to field converter, if fields was connected
    // through one
    SoFieldConverter *	getConverter() const;

    // Looks up what field connection is from (container and field/output name)
    void		getConnectionInfo(SoFieldContainer *&, SbName&) const;

    // This is used to reallocate the string buffer used by
    // SoField::get()
    static void *	reallocFieldBuf(void *ptr, size_t newSize);

friend class SoEngineOutput;
};

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoSField
//
//  Field that always has only one value.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: abstract
class SoSField : public SoField {
  public:
    // Destructor
    virtual ~SoSField();

    // Returns type identifier for SoSField class
    static SoType	getClassTypeId()	{ return classTypeId; }

  protected:
    // Constructor
    SoSField();

  SoINTERNAL public:
    static void		initClass();

    static void *	createInstance();

  private:
    static SoType	classTypeId;

    // Reads value of field
    virtual SbBool	readValue(SoInput *in) = 0;

    // Writes value of field
    virtual void	writeValue(SoOutput *out) const = 0;
};

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoMField
//
//  Field that can have multiple values.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: abstract
class SoMField : public SoField {

  public:
    // Destructor
    virtual ~SoMField();

    // Returns number of values
    int			getNum() const { evaluate(); return num; }

    // Adjust the number of values to be num, adding or deleting
    // values at the end as needed.  If adding, the initial values
    // in the new space are undefined.
    void		setNum(int num);

    // Delete num values, starting at start. A num of -1 (the default)
    // means delete all values after start, inclusive
    // C-api: expose
    // C-api: name=del
    virtual void	deleteValues(int start, int num = -1);

    // Insert space for num values starting at start.  The initial
    // values in the new space are undefined.
    // C-api: expose
    virtual void	insertSpace(int start, int num);

    // These are equivalent to the SoField::set() and SoField::get()
    // methods, but operate on only the value given by the index.
    SbBool		set1(int index, const char *valueString);
    void		get1(int index, SbString &valueString);

    // Returns type identifier for SoMField class
    static SoType	getClassTypeId()	{ return classTypeId; }

  SoINTERNAL public:
    static void		initClass();

    static void *	createInstance();

  protected:
    int			num;		// Number of values
    int			maxNum;		// Number of values allocated

    // Constructor
    SoMField();

    // Make sure there is room for newNum vals
    virtual void	makeRoom(int newNum);

  private:
    static SoType	classTypeId;

    // These are used by SoMField::get1() to hold the returned field string
    static char		*fieldBuf;
    static int		fieldBufSize;

    // Allocates room for num values. Copies old values (if any) into
    // new area. Deletes old area, if any.
    virtual void	allocValues(int num) = 0;

    // Deletes all current values
    virtual void	deleteAllValues() = 0;

    // Copies value indexed by "from" to value indexed by "to"
    virtual void	copyValue(int to, int from) = 0;

    // Reads all values of field
    virtual SbBool	readValue(SoInput *in);

    // Reads one indexed value of field from file
    virtual SbBool	read1Value(SoInput *in, int index) = 0;

    // Writes all values of field
    virtual void	writeValue(SoOutput *out) const;

    // Writes indexed value to file
    virtual void	write1Value(SoOutput *out, int index) const = 0;

    // Reads array of binary values from file
    virtual SbBool	readBinaryValues(SoInput *in, int numToRead);

    // Writes array of binary values to file
    virtual void	writeBinaryValues(SoOutput *out) const;

    // Returns number of ASCII values to write per output line (default 1)
    virtual int		getNumValuesPerLine() const;

    // This is used to reallocate the string buffer used by
    // SoField::get()
    static void *	reallocFieldBuf(void *ptr, size_t newSize);
};

#endif /* _SO_FIELD_ */
