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
 |	This file contains the definition of the SoOutput class.
 |
 |   Classes:
 |	SoOutput
 |
 |   Author(s)		: Paul S. Strauss, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_OUTPUT_
#define  _SO_OUTPUT_

#include <Inventor/misc/SoBasic.h>
#include <Inventor/SbDict.h>
#include <Inventor/SbString.h>

class SoBase;

typedef void *SoOutputReallocCB(void *ptr, size_t newSize);

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoOutput
//
//  This file contains the definition of the SoOutput (output stream)
//  class. This class is used for writing Inventor data files; it
//  supports both ASCII (default) and binary formats and provides some
//  convenience functions for handling files. It also supports easy
//  indentation.
//
//  Output can go to: (1) a file pointer created by the caller, (2) a
//  file opened by SoOutput, or (3) a buffer in memory created by the
//  caller. In the third case, the caller supplies a pointer to the
//  buffer, the initial size of the buffer, and a reallocation
//  function to call when that size is exceeded. At any time during
//  the writing, the caller can inquire the address of the buffer and
//  how many bytes have been inserted into it.
//
//  SoOutput also contains a dictionary that correlates node and path
//  pointers to temporary names written to files. This is so
//  references to previously defined nodes and paths are written
//  correctly.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoOut
class SoOutput {
 public:

    // Constructor (default SoOutput writes to stdout)
    SoOutput();

    // Destructor closes file if SoOutput opened it.
    ~SoOutput();

    // Sets file pointer to write to
    // C-api: name=setFilePtr
    void		setFilePointer(FILE *newFP);

    // Returns file pointer writing to, or NULL if writing to buffer
    // C-api: name=getFilePtr
    FILE *		getFilePointer() const;

    // Opens named file, sets file pointer to result. Returns FALSE on error
    SbBool		openFile(const char *fileName);

    // Closes file if opened with openFile. (Does nothing if not.)
    void		closeFile();

    // Sets up buffer to write to, initial size, reallocation function,
    // and offset in buffer at which to begin writing.
    // C-api: name=setBuf
    void		setBuffer(void *bufPointer, size_t initSize,
				  SoOutputReallocCB *reallocFunc, 
				  int32_t offset = 0);

    // Returns pointer to buffer and the total bytes written in the buffer
    // from the start of the buffer (not from offset!)
    // Returns FALSE if not writing into buffer.
    // C-api: name=getBuf
    SbBool		getBuffer(void *&bufPointer, size_t &nBytes) const;

    // The actual number of bytes allocated to the buffer may be larger
    // than the number of bytes written. This returns that actual number.
    size_t		getBufferSize() const { return bufSize; }
    
    // Resets buffer for output again; output starts over at beginning
    // C-api: name=resetBuf
    void		resetBuffer();

    // Indicates whether output should be ASCII (default) or binary
    // C-api: name=setBin
    void		setBinary(SbBool flag);

    // Returns current state of binary flag
    // C-api: name=isBin
    SbBool		isBinary() const		{ return binary; }

    // Specify the header used when writing the file
    // C-api: name=setHdrStr
    void		setHeaderString(const SbString &str);
    // Specify to use the default header (ascii or binary)
    // C-api: name=resetHdrStr
    void		resetHeaderString();
    
    // C-api: name=getDfltASCIIHdr
    static SbString	getDefaultASCIIHeader();
    // C-api: name=getDfltBinHdr
    static SbString	getDefaultBinaryHeader();

    // Sets the precision for outputing real numbers
    // C-api: name=setFltPrcsn
    void		setFloatPrecision(int precision);
							
  SoEXTENDER public:

    // Enum that determines stage of operation of writing
    enum Stage {
	COUNT_REFS,		// Count write references
	WRITE			// Actually write to file
    };

    // Sets/returns current stage of operation of action
    void		setStage(Stage stage)		{ curStage = stage; }
    Stage		getStage() const		{ return curStage; }

    // Increments/decrements indentation level by amount (default 1).
    // Each indentation level is 4 spaces.
    void		incrementIndent(int amount = 1)
	{ indentLevel += amount; }
    void		decrementIndent(int amount = 1)
	{ indentLevel -= amount; }

    // Writes item of particular type to current file pointer/buffer
    // C-api: name=writeChar
    void		write(char	     c);
    // C-api: name=writeCharStr
    void		write(const char    *s);
    // C-api: name=writeString
    void		write(const SbString &s);
    // C-api: name=writeName
    void		write(const SbName   &n);
    // C-api: name=writeInt
    void		write(int	     i);
    // C-api: name=writeUInt
    void		write(unsigned int   i);
    // C-api: name=writeShort
    void		write(short	     s);
    // C-api: name=writeUShort
    void		write(unsigned short s);
    // was ... C-api: name=writeInt32
    //     but typedef makes this redundant
    //void		write(int32_t	     l);
    // was ... C-api: name=writeUInt32
    //     but typedef makes this redundant
    //void		write(uint32_t	     l);
    // C-api: name=writeFloat
    void		write(float	     f);
    // C-api: name=writeDbl
    void		write(double	     d);
    // C-api: name=writeBinCharArray
    void                writeBinaryArray(unsigned char *c, int length);
    // C-api: name=writeBinInt32Array
    void                writeBinaryArray(int32_t *l, int length);
    // C-api: name=writeBinFloatArray
    void                writeBinaryArray(float *f, int length);
    // C-api: name=writeBinDblArray
    void                writeBinaryArray(double *d, int length);

    // Writes indentation to file/buffer based on current indentation level
    void		indent();

  SoINTERNAL public:
    // Constructor that gets reference dictionary from another SoOutput
    SoOutput(SoOutput *dictOut);

    // Resets things for writing to a new file or changing files
    void		reset();

    // Prevents header and extra white space from being written.
    // Useful for producing compact strings of data.
    void		setCompact(SbBool flag)		{ compact = flag; }
    SbBool		isCompact() const		{ return compact; }

    // Bit mask for various output annotations.  Note that annotation is
    // automatically disabled for non-compact or binary format.
    enum Annotations {
	ADDRESSES  = (1<<0),	// pointer values
	REF_COUNTS = (1<<1)	// node reference counts
    };
    void		setAnnotation(uint32_t bits)
	{ annotation = bits; }
    uint32_t		getAnnotation()
	{ return (isCompact() || isBinary()) ? 0 : annotation; }

  private:
    FILE		*fp;		// File writing to
    SbBool		toBuffer;	// TRUE if writing to buffer
    char                *tmpBuffer;     // For binary write to file
    void		*buffer;	// Buffer writing to
    char		*curBuf;	// Current pointer in buffer
    size_t		bufSize;	// Maximum buffer size
    size_t		tmpBufSize;	// Maximum temporary buffer size
    SoOutputReallocCB	*reallocFunc;	// Reallocation function for buffer
    SbBool		openedHere;	// TRUE if SoOutput opened file
    SbBool		binary;		// TRUE if writing binary data
    SbBool		compact;	// TRUE if writing in compact form
    SbBool		wroteHeader;	// TRUE if header was written
    int			indentLevel;	// Current indentation level
    SbDict		*refDict;	// SoBase pointer -> reference ID
    SbBool		borrowedDict;	// TRUE if dict from another SoOutput
    int			refIdCount;	// Counter for generating ref IDs
    SbBool		anyRef;		// TRUE if any reference in dictionary
    uint32_t		annotation;	// annotation bit mask
    Stage		curStage;	// Stage of operation
    SbString		headerString;	// The header 
    SbString		fmtString;	// Output format

    // Writes correct file header string to current file/buffer
    void		writeHeader();
    
    // Pad a header so that it is correctly aligned for reading from
    // binary files into memory
    static SbString	padHeader(const SbString &inString);

    // Returns TRUE if writing into memory buffer rather than file
    SbBool		isToBuffer() const
	{ return toBuffer; }

    // Returns number of bytes in current buffer
    size_t		bytesInBuf() const
	{ return (curBuf - (char *) buffer); }

    // Makes sure current buffer can contain nBytes more bytes
    SbBool		makeRoomInBuf(size_t nBytes);

    // Makes sure temporary buffer can contain nBytes more bytes
    SbBool		makeRoomInTmpBuf(size_t nBytes);

    // Adds a reference to dictionary, returning the reference id
    // created for it.
    int			addReference(const SoBase *base);

    // Looks up a reference, returning the reference id or -1 if it
    // hasn't been added yet.
    int			findReference(const SoBase *base) const;

    // Convert datatypes to network format during writing
    void                convertShort(short s, char *to);
    void                convertInt32(int32_t l, char *to);
    void                convertFloat(float f, char *to);
    void                convertDouble(double d, char *to);
    void                convertShortArray( short *from, char *to, int len);
    void                convertInt32Array( int32_t *from, char *to, int len);
    void                convertFloatArray( float *from, char *to, int len);
    void                convertDoubleArray( double *from, char *to, int len);

friend class SoBase;
friend class SoDB;
};

#endif /* _SO_OUTPUT_ */

