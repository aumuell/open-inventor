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
 |   $Revision: 1.1 $
 |
 |   Classes:
 |	SoInput, SoInputFile
 |
 |   Notes: This file includes any machine-dependent `dgl' (data goo lib)
 |   code for each machine.  See dgl.h.
 |
 |   Author(s)	: Paul S. Strauss, Gavin Bell, Dave Immel (dgl/cray)
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <machine.h>		// This is included in the Inventor tree
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <Inventor/misc/SoBase.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/SoOutput.h>
#include <Inventor/SoPath.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/errors/SoReadError.h>

// Static list of directories to search in.  Allocated by SoInput::init.
SbStringList *SoInput::directories = NULL;

#define COMMENT_CHAR '#'


/////////////////////////////////////////////////////////////////////////
//
// Note: The following dgl code is for machine specific implemenations.
//       The SGI implementation is defined in dgl.h and does not
//       require any functions (macros only).
//
////////////////////// BEGIN CRAY-DEPENDENT CODE ////////////////////////
//
// Following are functions that are used to convert from the
// network format to the host format for CRAY machines:
//
//	dgl_ntoh_short(f)	network to host - short
//	dgl_ntoh_int32(f)	network to host - int32_t
//	dgl_ntoh_float(PC)	network to host - float
//	dgl_ntoh_double(PC)	network to host - double
//
////////////////////////////////////////////////////////////////////////

#ifdef _CRAY

short dgl_ntoh_short( char *f )
{
    int val;
    val = f[0] << 8 | f[1];
    if (f[0] & 0x80) val |= ~0xffff;
    return (val);
}
int32_t dgl_ntoh_int32( char *f )
{
    int32_t val;
    val = f[0] << 24 | f[1] << 16 | f[2] << 8 | f[3];
    if (f[0] & 0x80) val |= ~0xffffffff;
    return (val);
}

//
// BIG_IEEE: no conversion necessary (FLOAT)
//
// (Not portable.  This routine works on Crays.)
// What IEEE single precision floating point looks like on a Cray

struct	ieee_single {
    unsigned int	zero	: 32;	// Upper 32 bits are junk
    unsigned int	sign	: 1;
    unsigned int	exp	: 8;
    unsigned int	mantissa: 23;	// 24-bit mantissa with 1 hidden bit
};

// Cray floating point, partitioned for easy conversion to IEEE single
struct	cray_single {
    unsigned int	sign	 : 1;
    unsigned int	exp	 : 15;
    unsigned int	mantissa : 24;
    unsigned int	mantissa2: 24;
};

struct	cray_double {
    unsigned int	sign	 : 1;
    unsigned int	exp	 : 15;
    unsigned int	mantissa : 48;
};

#define	CRAY_BIAS	040001

// Cray exponent limits for conversion to IEEE single
#define	MAX_CRAY_SNG	(0x100 + CRAY_BIAS - IEEE_SNG_BIAS)
#define	MIN_CRAY_SNG	(0x00 + CRAY_BIAS - IEEE_SNG_BIAS)

static struct cray_single max_sng_cray = { 0, 0x6000, 0, 0 } ;
static struct cray_double max_dbl_cray = { 0, 0x6000, 0 } ;

#define IEEE_SNG_BIAS   0x7f

float dgl_ntoh_float( char *PC ) { 		// CRAY-dependent
    union {
	struct ieee_single is;
	int32_t l;
    } c;
    union {
	struct cray_single vc;
	float iis;
    } ieee;

    c.l = dgl_ntoh_int32(PC);

    if(c.is.exp == 0) { ieee.iis = 0.0; }
    else if ( c.is.exp == 0xff ) {
	// If the IEEE float we are decoding indicates an IEEE overflow
	// condition, we manufacture a Cray overflow condition.
	ieee.vc = max_sng_cray ;
    }
    else {
	ieee.vc.sign = c.is.sign;
	ieee.vc.exp = c.is.exp - IEEE_SNG_BIAS + CRAY_BIAS;
	ieee.vc.mantissa = c.is.mantissa | (1 << 23);
	ieee.vc.mantissa2 = 0;
    }
    return (ieee.iis);
}

// What IEEE double precision floating point looks like on a Cray
// (the first word, anyway.  Second word is pure mantissa2.)
struct  ieee_double {
    unsigned int    sign      : 1;
    unsigned int    exp       : 11;
    unsigned int    mantissa  : 52;
};

#define IEEE_DBL_BIAS   0x3ff

float dgl_ntoh_double( char *PC ) { 	// CRAY-dependent
    union {
	struct ieee_double is;
	char l[SIZEOF(double)];
    } c;
    union {
	struct	cray_double vc;
	float iis;
    } cr;

    bcopy(PC, c.l, SIZEOF(double));
    if(c.is.exp == 0) { cr.iis = 0.0; }
    else if ( c.is.exp == 0xff ) {
	// If the IEEE float we are decoding indicates an IEEE overflow
	// condition, we manufacture a Cray overflow condition.
	cr.vc = max_dbl_cray ;
    }
    else {
	cr.vc.sign = c.is.sign;
	cr.vc.exp = c.is.exp - IEEE_DBL_BIAS + CRAY_BIAS;
	cr.vc.mantissa = (c.is.mantissa >> 5) | (1 << 47);
    }
    return (cr.iis);
}
#endif // _CRAY
//////////////////////// END CRAY-DEPENDENT CODE ////////////////////////



////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor for SoInputFile.
//
// Use: internal

SoInputFile::SoInputFile()
//
////////////////////////////////////////////////////////////////////////
{
    // Initialize variables:
    name.makeEmpty();
    fullName.makeEmpty();
    fp = NULL;
    buffer = NULL;
    curBuf = NULL;
    openedHere = FALSE;
    binary = FALSE;
    readHeader = FALSE;
    headerOk = FALSE;
    refDict = NULL;
    borrowedDict = FALSE;
    ivVersion = 0.;
    headerString.makeEmpty();
    postReadCB = NULL;
    CBData = NULL;

}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor - sets input to stdin by default.
//
// Use: public

SoInput::SoInput()
//
////////////////////////////////////////////////////////////////////////
{
    // Create new file and push on stack
    curFile = new struct SoInputFile;
    files.append((void *) curFile);

    // Make it read from stdin
    initFile(stdin, "<stdin>", NULL, FALSE);

    backBufIndex = -1;	// No buffer

    tmpBuffer = NULL;
    tmpBufSize = 0;

    backupBufUsed = FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Init routine-- called by SoDB::init, it sets up the list of
//    directories to search in.
//
// Use: internal, static

void
SoInput::init()
//
////////////////////////////////////////////////////////////////////////
{
    directories = new SbStringList;

    // Default directory search path is current directory
    directories->append(new SbString("."));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor that gets reference dictionary from another SoInput.
//
// Use: internal

SoInput::SoInput(SoInput *dictIn)
//
////////////////////////////////////////////////////////////////////////
{
    // Create new file and push on stack
    curFile = new struct SoInputFile;
    files.append((void *) curFile);

    // Make it read from stdin and use the passed dictionary
    initFile(stdin, "<stdin>", NULL, FALSE,
	     (dictIn == NULL ? NULL : dictIn->curFile->refDict));

    backBufIndex = -1;	// No buffer

    tmpBuffer = NULL;
    tmpBufSize = 0;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor - closes all input files opened by SoInput.
//
// Use: public

SoInput::~SoInput()
//
////////////////////////////////////////////////////////////////////////
{
    closeFile();

    // closeFile() leaves the topmost file on the stack, so delete it
    if (curFile->refDict != NULL && ! curFile->borrowedDict)
	delete curFile->refDict;
    delete curFile;

    if (tmpBuffer != NULL) {
        free (tmpBuffer);
        tmpBuffer = NULL;
        tmpBufSize = 0;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds a directory to beginning of list of directories to search to
//    find named files to open.
//
// Use: public, static

void
SoInput::addDirectoryFirst(const char *dirName)
//
////////////////////////////////////////////////////////////////////////
{
    directories->insert(new SbString(dirName), 0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds a directory to end of list of directories to search to
//    find named files to open.
//
// Use: public, static

void
SoInput::addDirectoryLast(const char *dirName)
//
////////////////////////////////////////////////////////////////////////
{
    directories->append(new SbString(dirName));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds directories that are named in the value of the given
//    environment variable to the beginning of the list of directories.
//    Directories may be separated by colons or whitespace in the value.
//
// Use: public, static

void
SoInput::addEnvDirectoriesFirst(const char *envVarName)
//
////////////////////////////////////////////////////////////////////////
{
    char *dirs = getenv(envVarName);

    if (dirs != NULL) {
	char	*d = strdup(dirs);
	char	*dir;
	int	i = 0;

	// Parse colon- or space-separated directories from string
	dir = strtok(d, ": \t");

	while (dir != NULL) {
	    // Make sure directories are added in the same order
	    directories->insert(new SbString(dir), i++);
	    dir = strtok(NULL, ": \t");
	}

	free(d);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds directories that are named in the value of the given
//    environment variable to the end of the list of directories.
//    Directories may be separated by colons or whitespace in the value.
//
// Use: public, static

void
SoInput::addEnvDirectoriesLast(const char *envVarName)
//
////////////////////////////////////////////////////////////////////////
{
    char *dirs = getenv(envVarName);

    if (dirs != NULL) {
	char *d = strdup(dirs);
	char *dir;

	// Parse colon- or space-separated directories from string
	dir = strtok(d, ": \t");

	while (dir != NULL) {
	    addDirectoryLast(dir);
	    dir = strtok(NULL, ": \t");
	}

	free(d);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Removes given directory from list.
//
// Use: public, static

void
SoInput::removeDirectory(const char *dirName)
//
////////////////////////////////////////////////////////////////////////
{
    int		i;
    SbString	*dir;

    for (i = 0; i < directories->getLength(); i++) {

	dir = (*directories)[i];

	if (*dir == dirName) {
	    directories->remove(i);
	    delete dir;
	    break;
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Clears list of directories, including the current directory.
//
// Use: public, static

void
SoInput::clearDirectories()
//
////////////////////////////////////////////////////////////////////////
{
    int	i;

    for (i = 0; i < directories->getLength(); i++)
	delete (*directories)[i];

    directories->truncate(0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the list of search directories.
//
// Use: public, static

const SbStringList &
SoInput::getDirectories()

//
////////////////////////////////////////////////////////////////////////
{
    return *directories;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets file pointer to read from. Clears stack if necessary.
//
// Use: public

void
SoInput::setFilePointer(FILE *newFP)		// New file pointer
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (newFP == NULL)
	SoDebugError::postWarning("SoInput::setFilePointer",
				  "Setting file pointer to NULL - "
				  "may cause problems");
#endif /* DEBUG */

    // Close open files, if any
    closeFile();

    // Initialize reading from file
    initFile(newFP, "<user-defined file pointer>", NULL, FALSE);

    if (tmpBuffer == NULL) {
        tmpBuffer = malloc(64);
        tmpBufSize = 64;
        curTmpBuf = (char *)tmpBuffer;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Opens named file, sets file pointer to result. If it can't open
//    the file, it returns FALSE. If okIfNotFound is FALSE (the
//    default), it prints an error message if the file could not be
//    found.
//
// Use: public

SbBool
SoInput::openFile(const char *fileName, SbBool okIfNotFound)
//
////////////////////////////////////////////////////////////////////////
{
    FILE *newFP = NULL;
    SbString	fullName;

    if (fileName != NULL && fileName[0] != '\0') {
	newFP = findFile(fileName, fullName);
    }

    if (newFP == NULL) {
	if (! okIfNotFound)
	    SoReadError::post(this,
			      "Can't open file \"%s\" for reading", fileName);
	return FALSE;
    }

    // Close open files, if any
    closeFile();

    // Initialize reading from file
    initFile(newFP, fileName, &fullName, TRUE);

    if (tmpBuffer == NULL) {
        tmpBuffer = malloc(64);
        tmpBufSize = 64;
        curTmpBuf = (char *)tmpBuffer;
    }

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Opens named file, pushes file pointer to result. If it can't open
//    the file, it prints an error message and returns FALSE.
//
// Use: public

SbBool
SoInput::pushFile(const char *fileName)	// Name of file
//
////////////////////////////////////////////////////////////////////////
{
    SbString	fullName;

    FILE *newFP = findFile(fileName, fullName);

    if (newFP == NULL) {
	SoDebugError::post("SoInput::pushFile",
			   "Can't open file \"%s\" for reading", fileName);
	return FALSE;
    }

    // Allocate a new file structure and push onto stack
    curFile = new struct SoInputFile;
    files.append(curFile);

    // Initialize reading from file
    initFile(newFP, fileName, &fullName, TRUE);

    if (tmpBuffer == NULL) {
        tmpBuffer = malloc(64);
        tmpBufSize = 64;
        curTmpBuf = (char *)tmpBuffer;
    }

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Closes current files that were opened by SoInput. It does
//    nothing to files that were not opened here, so it's always safe
//    to call. This also removes all of the files from the stack and
//    resets things to read from stdin.
//
// Use: public

void
SoInput::closeFile()
//
////////////////////////////////////////////////////////////////////////
{
    int			i;
    struct SoInputFile	*f;

    // Close all files opened here
    for (i = 0; i < files.getLength(); i++) {
	f = (struct SoInputFile *) files[i];

	if (f->openedHere)
	    fclose(f->fp);

	// Free up storage used for all but topmost file
	if (i > 0) {
	    delete f->refDict;
	    delete f;
	}
    }

    // Remove all but the first file from the stack
    if (files.getLength() > 1)
	files.truncate(1);

    // Reset to read from stdin again
    initFile(stdin, "<stdin>", NULL, FALSE);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if currently open file is a valid file;
//    that is, it begins with a valid header that has been registered
//    through SoDB::registerHeader.
//
// Use: public

SbBool
SoInput::isValidFile()
//
////////////////////////////////////////////////////////////////////////
{
    // Make sure the header was read
    if (! curFile->readHeader)
	(void) checkHeader();

    return curFile->headerOk;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns pointer to current file, or NULL if reading from buffer.
//
// Use: public

FILE *
SoInput::getCurFile() const
//
////////////////////////////////////////////////////////////////////////
{
    return fromBuffer() ? NULL : curFile->fp;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns full name of current file, or NULL if reading from buffer.
//
// Use: public

const char *
SoInput::getCurFileName() const
//
////////////////////////////////////////////////////////////////////////
{
    return fromBuffer() ? NULL : curFile->fullName.getString();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets up buffer to read from and its size.
//
// Use: public

void
SoInput::setBuffer(void *bufPointer, size_t bufSize)
//
////////////////////////////////////////////////////////////////////////
{
    // Close open files, if any
    closeFile();

    // Initialize reading from buffer
    curFile->name	= "<user-defined buffer in memory>";
    curFile->fp		= NULL;
    curFile->buffer	= bufPointer;
    curFile->curBuf	= (char *) bufPointer;
    curFile->bufSize	= bufSize;
    curFile->lineNum	= 1;
    curFile->openedHere	= FALSE;

    // Start with a fresh dictionary
    if (curFile->refDict != NULL && ! curFile->borrowedDict)
	curFile->refDict->clear();
    else
	curFile->refDict = new SbDict;

    // Assume file is ASCII until header is checked
    curFile->binary	= FALSE;
    curFile->readHeader	= FALSE;
    curFile->headerOk	= TRUE;
    curFile->ivVersion	= 0.;
    curFile->headerString.makeEmpty();

    // Delete the temporary buffer if it has been allocated
    if (tmpBuffer != NULL) {
        free (tmpBuffer);
        tmpBuffer = NULL;
        tmpBufSize = 0;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns number of bytes read from buffer. Returns 0 if not
//    reading from a buffer.
//
// Use: public

size_t
SoInput::getNumBytesRead() const
//
////////////////////////////////////////////////////////////////////////
{
    if (! fromBuffer())
	return 0;

    return curFile->curBuf - (char *) curFile->buffer;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns whether current file being read is binary. This may have
//    to check the header to determine this info.
//
// Use: public

SbBool
SoInput::isBinary()
//
////////////////////////////////////////////////////////////////////////
{
    // Check header if not already done
    if (! curFile->readHeader)
	(void) checkHeader();

    return curFile->binary;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the header of the file being read
//
// Use: public

SbString
SoInput::getHeader()
//
////////////////////////////////////////////////////////////////////////
{
    // Check header if not already done
    if (! curFile->readHeader)
	(void) checkHeader();

    return curFile->headerString;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads next character from current file/buffer. Returns FALSE on EOF.
//
// Use: public

SbBool
SoInput::get(char &c)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool ret;

    // Read from backBuf if it is not empty
    if (backBufIndex >= 0) {
	c = backBuf.getString()[backBufIndex++];

	if (c != '\0')
	    return TRUE;

	// Back buffer ran out of characters
	backBuf.makeEmpty();
	backBufIndex = -1;
    }

    if (! curFile->readHeader && ! checkHeader())
	return FALSE;

    if (eof()) {
	c = (char)EOF;
	ret = FALSE;
    }

    else if (curFile->binary) {
        if (fromBuffer()) {
	    c = *curFile->curBuf++;
            curFile->curBuf++;
            curFile->curBuf++;
            curFile->curBuf++;
	    ret = TRUE;
        }
        else {
            char pad[3];
            int i = fread((void *)&c, sizeof(char), 1, curFile->fp);
            (void)fread((void *)pad, sizeof(char), 3, curFile->fp);
            ret = (i == 1) ? TRUE : FALSE;
        }
    }

    else {

	if (! fromBuffer()) {

	    int i = getc(curFile->fp);

	    if (i == EOF) {
		c = (char)EOF;	// Set c to EOF so putpack(c) will fail 
		ret = FALSE;
	    }

	    else {
		c = (char) i;
		ret = TRUE;
	    }
	}

	else {
	    c = *curFile->curBuf++;
	    ret = TRUE;
	}
    }

    return ret;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads next ASCII character from current buffer. Returns FALSE on EOF.
//
// Use: public

SbBool
SoInput::getASCIIBuffer(char &c)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool ret;

    // Read from backBuf if it is not empty
    if (backBufIndex >= 0) {
	c = backBuf.getString()[backBufIndex++];

	if (c != '\0')
	    return TRUE;

	// Back buffer ran out of characters
	backBuf.makeEmpty();
	backBufIndex = -1;
    }

    if (freeBytesInBuf() == 0) {
	c = (char)EOF;
	ret = FALSE;
    }
    else {
	c = *curFile->curBuf++;
	ret = TRUE;
    }

    return ret;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads next ASCII character from current file. Returns FALSE on EOF.
//
// Use: public

SbBool
SoInput::getASCIIFile(char &c)
//
////////////////////////////////////////////////////////////////////////
{
    int		i;
    SbBool	ret;

    // Read from backBuf if it is not empty
    if (backBufIndex >= 0) {
	c = backBuf.getString()[backBufIndex++];

	if (c != '\0')
	    return TRUE;

	// Back buffer ran out of characters
	backBuf.makeEmpty();
	backBufIndex = -1;
    }

    i = getc(curFile->fp);
    c = (char)i;

    ret =  (i == EOF) ? FALSE : TRUE;

    return ret;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads a character from current file/buffer after skipping white space.
//    Returns FALSE on EOF.
//
// Use: public

SbBool
SoInput::read(char &c)
//
////////////////////////////////////////////////////////////////////////
{
    return (skipWhiteSpace() && get(c));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads a character string from current file/buffer. If the character
//    string begins with a double quote, this reads until a matching
//    close quote is found. Otherwise, it reads until the next white
//    space is found. This returns FALSE on EOF.
//
// Use: public

SbBool
SoInput::read(SbString &s)
//
////////////////////////////////////////////////////////////////////////
{
    if (! skipWhiteSpace())
	return FALSE;

    if (curFile->binary) {
	// If there is a string in the putback buffer, use it.
	if (backBufIndex >= 0) {
	    s = backBuf;
	    backBuf.makeEmpty();
	    backBufIndex = -1;
	    return TRUE;
	}

        //
        // Reading from the binary format consists of reading a word
        // representing the string length, followed by the string,
        // followed by the padding to the word boundary.  Put a NULL
        // character at the end of the string.
        //
        if (fromBuffer()) {
	    if (eof())
		return FALSE;

            int n = * (int *) curFile->curBuf;
#ifdef __i386__
                DGL_NTOH_INT32( n, n );
#endif

            // A marker was read.  Return without incrementing the buffer
            if (n < 0)
                return FALSE;

	    char buffer[1024], *buf;
	    if (n > 1023)
		buf = new char [n + 1];
	    else
		buf = buffer;
            curFile->curBuf += 4;
            bcopy(curFile->curBuf, buf, n);
            buf[n] = '\0';
            curFile->curBuf += (n+3) & ~0003;
            s = buf;
	    if (n > 1023)
		delete [] buf;
            return TRUE;
        }
        else {
            // Break out the case where an eof is hit.  This will only be
            // The case in SoFile nodes which don't know how many children
            // they have.  Reading keeps happening until eof is hit.

	    int n;
            if (fread((void *) &n, sizeof(int), 1, curFile->fp) == 1) {
#ifdef __i386__
                DGL_NTOH_INT32( n, n );
#endif
                if (n < 0) {
                    // A marker was read.  Put it in the backup buffer
                    // so the next read will read it.
                    int *tint = (int *) backupBuf;
                    *tint = n;
                    backupBufUsed = TRUE;
                    return FALSE;
                }

		char buffer[1024], *buf;
		if (n > 1023)
		    buf = new char [n + 1];
		else
		    buf = buffer;
		SbBool ok =
		    (fread((void *) buf, sizeof(char), n, curFile->fp) == n);
		if (ok) {
		    int pad = ((n+3) & ~003) - n;
		    char padbuf[4];
		    ok = (fread((void *) padbuf, sizeof(char), pad,
				curFile->fp) == pad);

		    if (ok) {
			buf[n] = '\0';
			s = buf;
		    }
		}
		if (n > 1023)
		    delete [] buf;

		if (! ok)
                    return FALSE;
            }
            else
                s = "";

            return TRUE;
        }
    }

    else {
	SbBool      quoted;
	char        c;
	char        bufStore[256];
	char        *buf;
	int         bytesLeft;

	s.makeEmpty();

	// Read first character - if none, EOF
	if (! get(c))
	    return FALSE;

	// If quoted string
	quoted = (c == '\"');
	if (! quoted)
	    putBack(c);

	do {
	    buf       = bufStore;
	    bytesLeft = sizeof(bufStore) - 1;

	    // Read a bufferfull
	    while (bytesLeft > 0) {

		// Terminate on EOF
		if (! get(*buf))
		    break;

		if (quoted) {
		    // Terminate on close quote
		    if (*buf == '\"')
			break;

		    // Check for escaped double quote
		    if (*buf == '\\') {
			if ((get(c)) && c == '\"')
			    *buf = '\"';
			else
			    putBack(c);
		    }

		    if (*buf == '\n')
			curFile->lineNum++;
		}

		// If unquoted string, terminate at whitespace
		else if (isspace(*buf)) {
		    // Put back the whitespace
		    putBack(*buf);
		    break;
		}

		buf++;
		bytesLeft--;
	    }
	    *buf = '\0';

	    // Tack the buffer onto the string
	    s += bufStore;

	} while (bytesLeft == 0);
    }

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads a character string SbName from current file/buffer. If the
//    character string begins with a double quote, this reads until a
//    matching close quote is found. Otherwise, it reads until the
//    next white space is found. This returns FALSE on EOF.
//
//    If "validIdent" is TRUE, this reads only strings that are valid
//    identifiers as defined in the SbName class. The default for this
//    is FALSE. Identifiers may not be within quotes.
//
// Use: public

SbBool
SoInput::read(SbName &n,		// Name to read into
	      SbBool validIdent)	// TRUE => name must be
					// identifier (default is FALSE)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool	gotChar;

    if (! skipWhiteSpace())
	return FALSE;

    // If binary input or not an identifer, read as just a regular string
    if (curFile->binary || ! validIdent) {
	SbString	s;

	if (! read(s)) {

	    // We may have just discovered EOF when trying to read the
	    // string. If so, and there's another file open on the
	    // stack, call this method again to try reading from the
	    // next file.

	    if (curFile->binary && eof() && files.getLength() > 1)
		return read(n, validIdent);

	    return FALSE;
	}

	n = s;
    }

    else {
	// Read identifier, watching for validity
	char	buf[256];
	char	*b = buf;
	char	c;

	if (fromBuffer()) {
	    if ((gotChar = getASCIIBuffer(c)) && SbName::isIdentStartChar(c)) {
		*b++ = c;
    
		while ((gotChar= getASCIIBuffer(c)) && SbName::isIdentChar(c)) {
		    // If the identifier is too long, it will be silently
		    // truncated.
		    if (b - buf < 255)
			*b++ = c;
		}
	    }
	}
	else {
	    if ((gotChar = getASCIIFile(c)) && SbName::isIdentStartChar(c)) {
		*b++ = c;
    
		while ((gotChar = getASCIIFile(c)) && SbName::isIdentChar(c)) {
		    // If the identifier is too long, it will be silently
		    // truncated.
		    if (b - buf < 255)
			*b++ = c;
		}
	    }
	}
	*b = '\0';

	// Put the terminating character (if any) back in the stream.
	if (gotChar)
	    putBack(c);

	n = buf;
    }

    return TRUE;
}



////////////////////////////////////////////////////////////////////////
//
// Description:
//    These all read a number of some type from current file/buffer.
//    Returns FALSE on EOF or if no valid number was read.
//	These macros are used by all the functions that read integers/reals.
//
// Use: public
//

#define READ_NUM(reader, readType, num, dglFunc, type, dglType)               \
    SbBool ok;                                                                \
    if (! skipWhiteSpace())                                                   \
        ok = FALSE;                                                           \
    else if (curFile->binary) {                                               \
        int n = SIZEOF(dglType);                                              \
        int pad = ((n+3) & ~0003) - n;                                        \
        dglType tnum;                                                         \
        if (fromBuffer()) {                                                   \
            if (eof())                                                        \
                ok = FALSE;                                                   \
            else {                                                            \
                ok = TRUE;                                                    \
                dglFunc(curFile->curBuf, (dglType *)&tnum);                   \
                curFile->curBuf += SIZEOF(dglType) + pad;                     \
            }                                                                 \
        }                                                                     \
        else {                                                                \
            if (backupBufUsed == TRUE) {                                      \
                num = (type)(*(type *)backupBuf);                             \
                backupBufUsed = FALSE;                                        \
                return TRUE;                                                  \
            }                                                                 \
            char padbuf[4];                                                   \
            makeRoomInBuf(SIZEOF(dglType));                                   \
            ok = fread(tmpBuffer, SIZEOF(dglType), 1, curFile->fp);           \
            dglFunc((char *)tmpBuffer, (dglType *)&tnum);                     \
            if (pad != 0)                                                     \
                ok = fread((void *)padbuf, SIZEOF(char), pad, curFile->fp);   \
        }                                                                     \
        num = (type)tnum;                                                     \
    }                                                                         \
    else {                                                                    \
        readType _tmp;                                                        \
        ok = reader(_tmp);                                                    \
        if (ok)                                                               \
            num = (type) _tmp;                                                \
    }                                                                         \
    return ok

#define READ_BIN_ARRAY(array, length, dglFunc, type)   			      \
    SbBool ok = TRUE;							      \
    if (! skipWhiteSpace())						      \
	ok = FALSE;							      \
    else if (fromBuffer()) {                                                  \
	if (eof())						              \
	    ok = FALSE;					                      \
	else {								      \
	    dglFunc(curFile->curBuf, (type *)array, length);	              \
	    curFile->curBuf += length * SIZEOF(type);                         \
        }								      \
    }                                                                         \
    else { 								      \
        makeRoomInBuf(length * SIZEOF(type));				      \
        int i = fread(tmpBuffer, SIZEOF(type), length, curFile->fp);          \
        if (i != length)                                                      \
            return FALSE;                                                     \
        dglFunc((char *)tmpBuffer, (type *)array, length);		      \
    } 									      \
    return ok

#define READ_INTEGER(num, dglFunc, type, dglType)			      \
	READ_NUM(readInteger, int32_t, num, dglFunc, type, dglType)

#define READ_UNSIGNED_INTEGER(num, dFunc, type, dType)			      \
	READ_NUM(readUnsignedInteger, uint32_t, num, dFunc, type, dType)

#define READ_REAL(num, dglFunc, type, dglType)				      \
	READ_NUM(readReal, double, num, dglFunc, type, dglType)

SbBool
SoInput::read(int &i)
{
    READ_INTEGER(i, convertInt32, int, int32_t);
}

SbBool
SoInput::read(unsigned int &i)
{
    READ_UNSIGNED_INTEGER(i, convertInt32, unsigned int, int32_t);
}

SbBool
SoInput::read(short &s)
{
    READ_INTEGER(s, convertInt32, short, int32_t);
}

SbBool
SoInput::read(unsigned short &s)
{
    READ_UNSIGNED_INTEGER(s, convertInt32, unsigned short, int32_t);
}

//       made redundant by typedef of int32_t
//SbBool
//SoInput::read(int32_t &l)
//{
//    READ_INTEGER(l, convertInt32, int32_t, int32_t);
//}

//       made redundant by typedef of uint32_t
//SbBool
//SoInput::read(uint32_t &l)
//{
//    READ_UNSIGNED_INTEGER(l, convertInt32, uint32_t, int32_t);
//}

SbBool
SoInput::read(float &f)
{
    READ_REAL(f, convertFloat, float, float);
}

SbBool
SoInput::read(double &d)
{
    READ_REAL(d, convertDouble, double, double);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads an array of unsigned chars from current file/buffer.
//
// Use: public

SbBool
SoInput::readBinaryArray(unsigned char *c, int length)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool ok = TRUE;
    if (! skipWhiteSpace())
	ok = FALSE;
    else if (fromBuffer()) {
	if (eof())
	    ok = FALSE;
	else {
	    bcopy(curFile->curBuf, (unsigned char *)c, length);
	    curFile->curBuf += length * SIZEOF(unsigned char);
	}
    }
    else {
	int i = fread(c, SIZEOF(unsigned char), length, curFile->fp);
	if (i != length)
	    return FALSE;
    }
    return ok;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads an array of int32_ts from current file/buffer.
//
// Use: public

SbBool
SoInput::readBinaryArray(int32_t *l, int length)
//
////////////////////////////////////////////////////////////////////////
{
    READ_BIN_ARRAY(l, length, convertInt32Array, int32_t);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads an array of floats from current file/buffer.
//
// Use: public

SbBool
SoInput::readBinaryArray(float *f, int length)
//
////////////////////////////////////////////////////////////////////////
{
    READ_BIN_ARRAY(f, length, convertFloatArray, float);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads an array of doubles from current file/buffer.
//
// Use: public

SbBool
SoInput::readBinaryArray(double *d, int length)
//
////////////////////////////////////////////////////////////////////////
{
    READ_BIN_ARRAY(d, length, convertDoubleArray, double);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Converts short from network format and puts in buffer.
//
// Use: private

void
SoInput::convertShort(char *from, short *s)
//
////////////////////////////////////////////////////////////////////////
{
    int32_t l;

    DGL_NTOH_INT32( l, INT32(from) );
    *s = (short)l;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Converts long from network format and puts in buffer.
//
// Use: private

void
SoInput::convertInt32(char *from, int32_t *l)
//
////////////////////////////////////////////////////////////////////////
{
    DGL_NTOH_INT32( *l, INT32(from) );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Converts float from network format and puts in buffer.
//
// Use: private

void
SoInput::convertFloat(char *from, float *f)
//
////////////////////////////////////////////////////////////////////////
{
    DGL_NTOH_FLOAT( *f, FLOAT(from) );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Converts double from network format and puts in buffer.
//
// Use: private

void
SoInput::convertDouble(char *from, double *d)
//
////////////////////////////////////////////////////////////////////////
{
    DGL_NTOH_DOUBLE( *d, DOUBLE(from) );
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Converts array of shorts in read buffer from network format and
//    puts in array.
//
// Use: private

void
SoInput::convertShortArray( char *from,
                            register short *to,
                            register int len)
//
////////////////////////////////////////////////////////////////////////
{
    register char *b = from;

    len >>= 1;			// convert bytes to short
    while (len > 4) {		// unroll the loop a bit
	DGL_NTOH_SHORT( to[0], SHORT(b));
	DGL_NTOH_SHORT( to[1], SHORT(b + SIZEOF(short)));
	DGL_NTOH_SHORT( to[2], SHORT(b + SIZEOF(short)*2));
	DGL_NTOH_SHORT( to[3], SHORT(b + SIZEOF(short)*3));
	to += 4;
	b  += SIZEOF(short)*4;
	len -= 4;
    }
    while (len-- > 0) {
	DGL_NTOH_SHORT( *to, SHORT(b));
	to++;
	b += SIZEOF(short);
    }
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Converts array of int32_ts in read buffer from network format and
//    puts in array.
//
// Use: private

void
SoInput::convertInt32Array( char *from,
                           register int32_t *to,
                           register int len)
//
////////////////////////////////////////////////////////////////////////
{
    register int32_t  *t = to;
    register char  *b = from;

    while (len > 4) {		// unroll the loop a bit
	DGL_NTOH_INT32( t[0], INT32(b));
	DGL_NTOH_INT32( t[1], INT32(b + SIZEOF(int32_t)));
	DGL_NTOH_INT32( t[2], INT32(b + SIZEOF(int32_t)*2));
	DGL_NTOH_INT32( t[3], INT32(b + SIZEOF(int32_t)*3));
	t += 4;
	b += SIZEOF(int32_t)*4;
	len -= 4;
    }
    while (len-- > 0) {
	DGL_NTOH_INT32( *t, INT32(b));
	t++;
	b += SIZEOF(int32_t);
    }
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Converts array of floats in read buffer from network format and
//    puts in array.
//
// Use: private

void
SoInput::convertFloatArray( char  *from,
                            float *to,
                            register int len)
//
////////////////////////////////////////////////////////////////////////
{
    register float *t = to;
    register char  *b = from;

    while (len > 4) {		// unroll the loop a bit
	DGL_NTOH_FLOAT( t[0], FLOAT(b));
	DGL_NTOH_FLOAT( t[1], FLOAT(b + SIZEOF(float)));
	DGL_NTOH_FLOAT( t[2], FLOAT(b + SIZEOF(float)*2));
	DGL_NTOH_FLOAT( t[3], FLOAT(b + SIZEOF(float)*3));
	t += 4;
	b += SIZEOF(float)*4;
	len -= 4;
    }
    while (len-- > 0) {
	DGL_NTOH_FLOAT( *t, FLOAT(b));
	t++;
	b += SIZEOF(float);
    }
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Converts array of doubles in read buffer from network format and
//    puts in array.
//
// Use: private

void
SoInput::convertDoubleArray( char *from,
                             register double *to,
                             register int len)
//
////////////////////////////////////////////////////////////////////////
{
    register char *b = from;

    len >>= 3;			// convert bytes to doubles
    while (len > 4) {		// unroll the loop a bit
	DGL_NTOH_DOUBLE( to[0], DOUBLE(b));
	DGL_NTOH_DOUBLE( to[1], DOUBLE(b + SIZEOF(double)));
	DGL_NTOH_DOUBLE( to[2], DOUBLE(b + SIZEOF(double)*2));
	DGL_NTOH_DOUBLE( to[3], DOUBLE(b + SIZEOF(double)*3));
	to += 4;
	b  += SIZEOF(double)*4;
	len -= 4;
    }
    while (len-- > 0) {
	DGL_NTOH_DOUBLE( *to, DOUBLE(b));
	to++;
	b += SIZEOF(double);
    }
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if current file/buffer is at EOF.
//
// Use: public

SbBool
SoInput::eof() const
//
////////////////////////////////////////////////////////////////////////
{
    if (! fromBuffer())
	return feof(curFile->fp);
    else
	return (freeBytesInBuf() == 0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Fills in passed string to contain description of current
//    location in all open input files.
//
// Use: internal

void
SoInput::getLocationString(SbString &string) const
//
////////////////////////////////////////////////////////////////////////
{
    const struct SoInputFile	*f;
    int				i = files.getLength() - 1;
    char			buf[10000];

    string.makeEmpty();

    f = (const struct SoInputFile *) files[i];
    sprintf(buf, "\tOccurred at line %3d in %s",
	    f->lineNum, f->fullName.getString());
    string = buf;

    for (--i ; i >= 0; --i) {
	f = (const struct SoInputFile *) files[i];
	sprintf(buf, "\n\tIncluded at line %3d in %s",
		f->lineNum, f->fullName.getString());
	string += buf;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Puts a just-read character back in input stream
//
// Use: internal

void
SoInput::putBack(char c)
//
////////////////////////////////////////////////////////////////////////
{
    // Never put an EOF back in the stream
    if (c == (char) EOF)
	return;

    if (backBufIndex >= 0)
	--backBufIndex;
    else if (! fromBuffer())
	ungetc(c, curFile->fp);
    else if (isBinary())
	;				// Can't do anything???
    else
	curFile->curBuf--;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Puts a just-read character string back in input stream. It uses
//    backBuf to implement this.
//
// Use: internal

void
SoInput::putBack(const char *string)
//
////////////////////////////////////////////////////////////////////////
{
    backBuf = string;
    backBufIndex = 0;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Opens named file, Returns a file pointer to it. If the file name
//    is not absolute, it checks all of the current directories in the
//    search path. It returns the full name of the file it found in
//    the fullName parameter. Returns NULL on error.
//
// Use: public

FILE *
SoInput::findFile(const char *fileName, SbString &fullName) const
//
////////////////////////////////////////////////////////////////////////
{
    FILE	*fp;
    int		i;

    // If filename is absolute
    if (fileName[0] == '/') {
	fullName = fileName;
	fp = fopen(fileName, "r");
    }

    // For relative file names, try each of the directories in the search path
    else {
	fp = NULL;

	for (i = 0; i < directories->getLength(); i++) {
	    fullName = * (*directories)[i];
	    fullName += "/";
	    fullName += fileName;

	    fp = fopen(fullName.getString(), "r");

	    if (fp != NULL)
		break;
	}
    }	

    return fp;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes reading from file pointer. Just sets up some variables.
//
// Use: private

void
SoInput::initFile(FILE *newFP,		// New file pointer
		  const char *fileName,	// Name of new file to read
		  SbString *fullName,	// Full name of new file
		  SbBool openedHere,	// TRUE if SoInput opened file
		  SbDict *refDict)	// Dictionary of base references
					// (default is NULL: create new dict)
//
////////////////////////////////////////////////////////////////////////
{
    curFile->name	= fileName;
    if (fullName == NULL)
	curFile->fullName = fileName;
    else
	curFile->fullName = *fullName;
    curFile->fp		= newFP;
    curFile->buffer	= NULL;
    curFile->lineNum	= 1;
    curFile->openedHere	= openedHere;

    if (refDict == NULL) {
	// Start with a fresh dictionary
	if (curFile->refDict != NULL && ! curFile->borrowedDict)
	    curFile->refDict->clear();
	else
	    curFile->refDict = new SbDict;
	curFile->borrowedDict = FALSE;
    }
    else {
	if (curFile->refDict != NULL && ! curFile->borrowedDict)
	    delete curFile->refDict;
	curFile->refDict = refDict;
	curFile->borrowedDict = TRUE;
    }

    // Assume file is ASCII until header is checked
    curFile->binary	= FALSE;
    curFile->readHeader	= FALSE;
    curFile->headerOk	= TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Checks current file/buffer for header comment that determines whether
//    it is in ASCII or binary format. The header must be the very
//    next thing in the input for this to recognize it. This prints an
//    error message and returns FALSE if there is no header and we are
//    reading from a file.
//
// Use: private

SbBool
SoInput::checkHeader()
//
////////////////////////////////////////////////////////////////////////
{
    SoDBHeaderCB    *preCB;
    SoDBHeaderCB    *postCB;
    void	    *userData;
    SbBool	    isBinary;
    float	    versionNum;
    char	    c;
    
    // Don't need to do this again. This has to be set first here so
    // the subsequent reads don't try to do this again.
    curFile->readHeader = TRUE;

    // See if first character in file is a comment character. If so,
    // see if there is a Inventor file header at the beginning. If so,
    // determine whether it is ASCII or binary. If there is no valid
    // header, assume it is ASCII.

    if (get(c)) {
    	if (c == COMMENT_CHAR) {
	    char	buf[256];
	    int		i = 0;

	    // Read comment into buffer
	    buf[i++] = c;
	    while (get(c) && c != '\n')
		buf[i++] = c;
	    buf[i] = '\0';
	    if (c == '\n')
		curFile->lineNum++;

	    // Read the file if the header is a registered header, or
	    // if the file header is a superset of a registered header.
	    
	    if (SoDB::getHeaderData(buf, isBinary, versionNum,
					preCB, postCB, userData, TRUE)) {
	
		if (isBinary) {
		    curFile->binary = TRUE;
		    if (tmpBuffer == NULL) {
			tmpBuffer = malloc(64);
			tmpBufSize = 64;
			curTmpBuf = (char *)tmpBuffer;
		    }
		} else {		    
		    curFile->binary = FALSE;		    
		}

		// Set the Inventor file version associated with the header
		setIVVersion(versionNum);

		// Invoke the pre-callback associated with the header
		if (preCB)
		    (*preCB)(userData, this);
		
		// Store a pointer to the post-callback for later use
		curFile->postReadCB = postCB;
		curFile->CBData = userData;
		    
		curFile->headerOk = TRUE;
		curFile->headerString = buf;
		return TRUE;
	    }
	}

	// Put non-comment char back in case we are reading from a buffer.
	else
	    putBack(c);
    }

    // If it gets here, no valid header was found. If we are reading
    // from a buffer, we can just assume we are reading ASCII, in the
    // latest format:
    if (fromBuffer()) { 
	curFile->binary = FALSE;
	return TRUE;
    }

    // If we are reading from a file, it MUST have a header
    SoReadError::post(this, "File does not have a valid header string");
    curFile->headerOk = FALSE;
    return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Skips over white space (blanks, tabs, newlines) and Inventor
//    comments (from COMMENT_CHAR to end of line). Keeps current line
//    number up-to-date. Returns FALSE on error.
//
// Use: private

SbBool
SoInput::skipWhiteSpace()
//
////////////////////////////////////////////////////////////////////////
{
    char	c;
    SbBool	gotChar;

    // Check for ASCII/binary header if not already done. Since most
    // of the read methods call this first, it's a convenient place to
    // do it.
    if (! curFile->readHeader && ! checkHeader())
	return FALSE;

    // Don't skip space in binary input. In ASCII, keep going while
    // space and comments appear
    if (! curFile->binary) {
	if (fromBuffer()) {
	    while (TRUE) {
    
		// Skip over space characters
		while ((gotChar = getASCIIBuffer(c)) && isspace(c))
		    if (c == '\n')
			curFile->lineNum++;
    
		if (! gotChar)
		    break;
    
		// If next character is comment character, flush til end of line
		if (c == COMMENT_CHAR) {
		    while (getASCIIBuffer(c) && c != '\n')
			;
    
		    if (eof())
			SoReadError::post(this,
					  "EOF reached before end of comment");
		    else
			curFile->lineNum++;
		}
		else {
		    putBack(c);
		    break;		// EXIT: hit a non-comment, non-space
		}
	    }
	}
	else {
	    while (TRUE) {
    
		// Skip over space characters
		while ((gotChar = getASCIIFile(c)) && isspace(c))
		    if (c == '\n')
			curFile->lineNum++;
    
		if (! gotChar)
		    break;
    
		// If next character is comment character, flush til end of line
		if (c == COMMENT_CHAR) {
		    while (getASCIIFile(c) && c != '\n')
			;
    
		    if (eof())
			SoReadError::post(this,
					  "EOF reached before end of comment");
		    else
			curFile->lineNum++;
		}
		else {
		    putBack(c);
		    break;		// EXIT: hit a non-comment, non-space
		}
	    }
	}
    }

    // If EOF, pop to previous file and skip space again
    while (eof() && popFile())
	if (! skipWhiteSpace())
	    return FALSE;

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Pops current file from stack. This should be called when current
//    file is at EOF and more stuff is needed. This returns FALSE if
//    there ain't no more files on the stack.
//
// Use: private

SbBool
SoInput::popFile()
//
////////////////////////////////////////////////////////////////////////
{
    // Call the post callback associated with this file type (as determined
    // by the file header)
    if (curFile->postReadCB)
	(*curFile->postReadCB)(curFile->CBData, this);
	
    int depth = files.getLength();

    // Nothing to pop if we're already in last file on stack
    if (depth == 1)
	return FALSE;

    // Remove one file
    files.truncate(depth - 1);

    // Free up structure for current file and set to new one
    if (curFile->openedHere)
	fclose(curFile->fp);

    delete curFile->refDict;
    delete curFile;
    curFile = (struct SoInputFile *) files[depth - 2];

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads a hexadecimal number into a int32_t. Returns FALSE on EOF or
//    if no valid hexadecimal number was read.
//
// Use: public

SbBool
SoInput::readHex(uint32_t &l)
{
    int		i;
    char	str[32];	// Number can't be longer than this
    char	*s = str;
    SbBool	ret;

    // Read from backBuf if it is not empty
    if (backBufIndex >= 0) {
	ret = FALSE;
	if (backBufIndex >= 3) {
	    strcpy(str, backBuf.getString());
	    ret = TRUE;
	}

	// Clear the back buffer.
	backBuf.makeEmpty();
	backBufIndex = -1;
    }

    // Read from a memory buffer
    else if (fromBuffer()) {
	skipWhiteSpace();
	s = curFile->curBuf;
	ret = TRUE;
    }

    // Read from a file
    else {
	skipWhiteSpace();
	while ((i = getc(curFile->fp)) != EOF) {
	    *s = (char) i;
	    if (*s == ',' || *s == ']' || *s == '}' || isspace(*s)) {
		putBack(*s);
		*s = '\0';
		break;
	    }
	    s++;
	}

	ret = (s - str <= 0) ? FALSE : TRUE;
	s = str;
    }

    // Convert the hex string we just got into an uint32_teger
    if (ret) {
	int	i;
	int	minSize = 3;	// Must be at least this many bytes in str
	char	*save = s;

	if (*s++ == '0') {
	    if (*s == '\0' || *s == ',' || *s == ']' || *s == '}' ||
		isspace(*s))
	    {
		l = 0;
		curFile->curBuf++;
	    }
	    else if (*s == 'x' || *s == 'X') {
		s++;
		l = 0;
		while (*s != '\0' && *s != ',' && *s != ']' && *s != '}' &&
			! isspace(*s))
		{
		    i = (int)*s;
		    if (i >= '0' && i <= '9') {
			i -= '0';
			l = (l<<4) + i;
		    }
		    else if (i >= 'A' && i <= 'F') {
			i -= ('A' - 10);
			l = (l<<4) + i;
		    }
		    else if (i >= 'a' && i <= 'f') {
			i -= ('a' - 10);
			l = (l<<4) + i;
		    }
		    s++;
		}
    
		if (fromBuffer()) {
		    // Make sure we have at least 1 actual digit
		    if (s - curFile->curBuf < minSize) {
			if (fromBuffer())
			    curFile->curBuf = save;
			else
			    putBack(save);
			ret = FALSE;
		    }
		    else
			curFile->curBuf = s;
		}
		else if (s - str < minSize) {
		    if (fromBuffer())
			curFile->curBuf = save;
		    else
			putBack(save);
		    ret = FALSE;
		}
		  
	    }
	}
    }

    return ret;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads a int32_t signed integer. Returns FALSE on EOF or if no
//    valid integer was read.
//
// Use: private

SbBool
SoInput::readInteger(int32_t &l)
//
////////////////////////////////////////////////////////////////////////
{
    char	str[32];	// Number can't be longer than this
    char	*s = str;
    int		i;
    SbBool	ret;


    // Read from backBuf if it is not empty
    if (backBufIndex >= 0) {
	strcpy(str, backBuf.getString());

	// Clear the back buffer.
	backBuf.makeEmpty();
	backBufIndex = -1;

	s = str;
	ret = TRUE;
    }

    // Read from a memory buffer
    else if (fromBuffer()) {
	s = curFile->curBuf;
	ret = TRUE;
    }

    // Read from a file
    else {
	int i;

	while ((i = getc(curFile->fp)) != EOF) {
	    *s = (char) i;
	    if (*s == ',' || *s == ']' || *s == '}' || isspace(*s)) {
		putBack(*s);
		*s = '\0';
		break;
	    }
	    s++;
	}

	ret = (s - str <= 0) ? FALSE : TRUE;
	s = str;
    }
    
    // Convert the string we just got into a int32_t integer
    if (ret) {
	char	*ptr;
	char	*save = s;

	if (*s == '0') {
	    s++;

	    // The string just contains a single zero
	    if (*s == '\0' || *s == ',' || *s == ']' || *s == '}' ||
		isspace(*s))
	    {
		l = 0;
		ret = TRUE;
	    }

	    // A hexadecimal format number
	    else if (*s == 'x' || *s == 'X') {
		s++;
		l = 0;
		ptr = s;
		while (*s != '\0') {
		    i = (int)*s;
		    if (i >= '0' && i <= '9') {
			i -= '0';
			l = (l<<4) + i;
		    }
		    else if (i >= 'A' && i <= 'F') {
			i -= ('A' - 10);
			l = (l<<4) + i;
		    }
		    else if (i >= 'a' && i <= 'f') {
			i -= ('a' - 10);
			l = (l<<4) + i;
		    }
		    else {	// unrecognized character; stop processing
			break;
		    }
		    s++;
		}
		if (s == ptr) {
		    if (fromBuffer())
			s = curFile->curBuf = save;
		    else
			putBack(save);
		    ret = FALSE;
		}
	    }

	    // An octal format number
	    else {
		l = 0;
		ptr = s;
		while ((int)*s >= '0' && (int)*s <= '7') {
		    i = (int)*s - '0';
		    l = (l<<3) + i;
		    s++;
		}
		if (s == ptr) {
		    if (fromBuffer())
			s = curFile->curBuf = save;
		    else
			putBack(save);
		    ret = FALSE;
		}
	    }
	}

	// A decimal format number
	else {
	    int sign = 1;

	    l = 0;
	    if (*s == '-' || *s == '+') {
		s++;
		sign = -1;
	    }
	    ptr = s;
	    while ((int)*s >= '0' && (int)*s <= '9') {
		i = (int)*s - '0';
		l = l*10 + i;
		s++;
	    }
	    l *= sign;
	    if (s == ptr) {
		if (fromBuffer())
		    s = curFile->curBuf = save;
		else
		    putBack(save);
		ret = FALSE;
	    }
	}

	if (fromBuffer())
	    curFile->curBuf = s;
    }

    return ret;

}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads a int32_t unsigned integer. Returns FALSE on EOF or if no
//    valid integer was read.
//
// Use: private

SbBool
SoInput::readUnsignedInteger(uint32_t &l)
//
////////////////////////////////////////////////////////////////////////
{
    char	str[32];	// Number can't be longer than this
    char	*s = str;
    int		i;
    SbBool	ret;

    // Read from backBuf if it is not empty
    if (backBufIndex >= 0) {
	strcpy(str, backBuf.getString());

	// Clear the back buffer.
	backBuf.makeEmpty();
	backBufIndex = -1;

	s = str;
	ret = TRUE;
    }

    // Read from a memory buffer
    else if (fromBuffer()) {
	s = curFile->curBuf;
	ret = TRUE;
    }

    // Read from a file
    else {
	int i;

	while ((i = getc(curFile->fp)) != EOF) {
	    *s = (char) i;
	    if (*s == ',' || *s == ']' || *s == '}' || isspace(*s)) {
		putBack(*s);
		*s = '\0';
		break;
	    }
	    s++;
	}

	ret = (s - str <= 0) ? FALSE : TRUE;
	s = str;
    }

    // Convert the string we just got into an uint32_teger
    if (ret) {
	char	*ptr;
	char	*save = s;

	if (*s == '0') {
	    s++;

	    // The string just contains a single zero
	    if (*s == '\0' || *s == ',' || *s == ']' || *s == '}' ||
		isspace(*s))
	    {
		l = 0;
		ret = TRUE;
	    }

	    // A hexadecimal format number
	    else if (*s == 'x' || *s == 'X') {
		s++;
		l = 0;
		ptr = s;
		while (*s != '\0') {
		    i = (int)*s;
		    if (i >= '0' && i <= '9') {
			i -= '0';
			l = (l<<4) + i;
		    }
		    else if (i >= 'A' && i <= 'F') {
			i -= ('A' - 10);
			l = (l<<4) + i;
		    }
		    else if (i >= 'a' && i <= 'f') {
			i -= ('a' - 10);
			l = (l<<4) + i;
		    }
		    else {	// unrecognized character; stop processing
			break;
		    }
		    s++;
		}
		if (s == ptr) {
		    if (fromBuffer())
			s = curFile->curBuf = save;
		    else
			putBack(save);
		    ret = FALSE;
		}
	    }

	    // An octal format number
	    else {
		l = 0;
		ptr = s;
		while ((int)*s >= '0' && (int)*s <= '7') {
	            i = (int)*s - '0';
	            l = (l<<3) + i;
	            s++;
		}
		if (s == ptr) {
		    if (fromBuffer())
			s = curFile->curBuf = save;
		    else
			putBack(save);
		    ret = FALSE;
		}
	    }
	}

	// A decimal format number
	else {
	    l = 0;
	    ptr = s;
	    while ((int)*s >= '0' && (int)*s <= '9') {
		i = (int)*s - '0';
		l = l*10 + i;
		s++;
	    }
	    if (s == ptr) {
		if (fromBuffer())
		    s = curFile->curBuf = save;
		else
		    putBack(save);
		ret = FALSE;
	    }
	}

	if (fromBuffer())
	    curFile->curBuf = s;
    }

    return ret;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads a string containing an unsigned integer. Returns FALSE on
//    EOF or if no valid integer string was read.
//
// Use: private

SbBool
SoInput::readUnsignedIntegerString(char *str)
//
////////////////////////////////////////////////////////////////////////
{
    int		minSize = 1;	// Must be at least this many bytes in str
    char	*s = str;

    // If the integer begins with '0', it may be in octal or hex
    if (readChar(s, '0')) {

	// Check for '0x', signifying hex
	if (readChar(s + 1, 'x')) {
	    s += 2 + readHexDigits(s + 2);
	    minSize = 3;
	}

	// Read digits after zero. There don't have to be any, though
	else
	    s += 1 + readDigits(s + 1);
    }

    else
	s += readDigits(s);

    // Make sure we have at least 1 actual digit
    if (s - str < minSize)
	return FALSE;

    // End string with null byte
    *s = '\0';

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads a double floating-point real number. Returns FALSE on EOF
//    or if no valid real was read. This is used by all real-reading
//    methods.
//
// Use: private

SbBool
SoInput::readReal(double &d)
//
////////////////////////////////////////////////////////////////////////
{
    int		n;
    char	str[32];	// Number can't be longer than this
    char	*s = str;
    SbBool	ret;

    // Read from backBuf if it is not empty
    if (backBufIndex >= 0) {
	n = sscanf(backBuf.getString(), "%lf", &d);

	// Clear the back buffer.
	backBuf.makeEmpty();
	backBufIndex = -1;

	ret = (n == 0 || n == EOF) ? FALSE : TRUE;
    }

    else if (fromBuffer()) {
	SbBool	gotNum = FALSE;
    
	////////////////////////////////////////////
	//
	// Leading sign
    
	n = readChar(s, '-');
	if (n == 0)
	    n = readChar(s, '+');
	s += n;
    
	////////////////////////////////////////////
	//
	// Integer before decimal point
    
	if ((n = readDigits(s)) > 0) {
	    gotNum = TRUE;
	    s += n;
	}
    
	////////////////////////////////////////////
	//
	// Decimal point
    
	if (readChar(s, '.') > 0) {
	    s++;
    
	    ////////////////////////////////////////////
	    //
	    // Integer after decimal point (no sign)
    
	    if ((n = readDigits(s)) > 0) {
		gotNum = TRUE;
		s += n;
	    }
	}
    
	// If no number before or after decimal point, there's a problem
	if (! gotNum)
	    return FALSE;
    
	////////////////////////////////////////////
	//
	// 'e' or 'E' for exponent
    
	n = readChar(s, 'e');
	if (n == 0)
	    n = readChar(s, 'E');
    
	if (n > 0) {
	    s += n;
    
	    ////////////////////////////////////////////
	    //
	    // Sign for exponent
    
	    n = readChar(s, '-');
	    if (n == 0)
		n = readChar(s, '+');
	    s += n;
    
	    ////////////////////////////////////////////
	    //
	    // Exponent integer
    
	    if ((n = readDigits(s)) > 0)
		s += n;
    
	    else
		return FALSE;	// Invalid exponent
	}

	// Terminator
	*s = '\0';

	d = atof(str);

	ret = TRUE;
    }
    else {
	n = fscanf(curFile->fp, "%lf", &d);
	ret =  (n == 0 || n == EOF) ? FALSE : TRUE;
    }

    return ret;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads a series of decimal digits into string. Returns number of
//    bytes read.
//
// Use: private

int
SoInput::readDigits(char *string)
//
////////////////////////////////////////////////////////////////////////
{
    char c, *s = string;

    if (fromBuffer()) {
	while (getASCIIBuffer(c)) {

	    if (isdigit(c))
		*s++ = c;

	    else {
		putBack(c);
		break;
	    }
	}
    }
    else {
	while (getASCIIFile(c)) {

	    if (isdigit(c))
		*s++ = c;

	    else {
		putBack(c);
		break;
	    }
	}
    }

    return s - string;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads a series of hexadecimal digits into string. Returns number
//    of bytes read.
//
// Use: private

int
SoInput::readHexDigits(char *string)
//
////////////////////////////////////////////////////////////////////////
{
    char c, *s = string;

    if (fromBuffer()) {
	while (getASCIIBuffer(c)) {

	    if (isxdigit(c))
		*s++ = c;

	    else {
		putBack(c);
		break;
	    }
	}
    }
    else {
	while (getASCIIFile(c)) {

	    if (isxdigit(c))
		*s++ = c;

	    else {
		putBack(c);
		break;
	    }
	}
    }

    return s - string;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads the given character. Returns the number of bytes read (0 or 1).
//
// Use: private

int
SoInput::readChar(char *string, char charToRead)
//
////////////////////////////////////////////////////////////////////////
{
    char	c;
    int		ret;

    if (fromBuffer()) {
	if (! getASCIIBuffer(c))
	    ret = 0;

	else if (c == charToRead) {
	    *string = c;
	    ret = 1;
	}

	else {
	    putBack(c);
	    ret = 0;
	}
    }
    else {
	if (! getASCIIFile(c))
	    ret = 0;

	else if (c == charToRead) {
	    *string = c;
	    ret = 1;
	}

	else {
	    putBack(c);
	    ret = 0;
	}
    }

    return ret;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds a reference to dictionary in current file/buffer.
//    The name passed will be a full identifier-- the object's name
//    followed by a '+' and the object's reference ID.  This routine
//    takes care of stripping the name before the '+' and giving the
//    object that name.  It also notices if the name is an underscore
//    followed by nothing but digits (Inventor 1.0 format for
//    instances) and leaves the object unnamed in that case.
//    Called by SoBase.
//
// Use: private

void
SoInput::addReference(const SbName &name,	// Reference name
		      SoBase *base, SbBool addToGlobalDict)
//
////////////////////////////////////////////////////////////////////////
{
    // Enter in dictionary : generates a CC warning...
    curFile->refDict->enter((unsigned long) name.getString(), (void *) base);

    int length = name.getLength();
    if (length == 0) return;

    const char *n = name.getString();

    // If we're reading a 1.0 file and the name is an '_' followed by
    // all digits, don't name the node.
    if (n[0] == '_' &&  curFile->ivVersion == 1.0f) {
	for (int i = 1; i < length; i++) {
	    if (!isdigit(n[i])) break;
	}
	if (i == length) return;
    }

    if (addToGlobalDict) {
	// Look for the first '+':
	char *firstPlus = strchr(n, '+');

	if (firstPlus == NULL) {
	    base->setName(name);
	}
	else if (firstPlus != n) {
	    SbName instanceName(SbString(n, 0, firstPlus-n-1));
	    base->setName(instanceName);
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Removes a reference from the current file/buffer dictionary.
//
// Use: private

void
SoInput::removeReference(const SbName &name)
//
////////////////////////////////////////////////////////////////////////
{
    curFile->refDict->remove((unsigned long) name.getString());
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Finds a reference in dictionary, returning the base pointer or NULL.
//
// Use: private


SoBase *					 // Returns pointer to base
SoInput::findReference(const SbName &name) const // Reference name
//
////////////////////////////////////////////////////////////////////////
{
    void	*base;

    // Generates a CC warning. Ho hum.
    if (curFile->refDict->find((unsigned long) name.getString(), base))
	return (SoBase *) base;

    return NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Makes sure temp buffer can contain nBytes more bytes. Returns
//    FALSE if this is not possible.
//
// Use: private

SbBool
SoInput::makeRoomInBuf(size_t nBytes)
//
////////////////////////////////////////////////////////////////////////
{
    // If already had problems with buffer, stop
    if (tmpBuffer == NULL)
        return FALSE;

    // If buffer not big enough, realloc a bigger one
    if (nBytes >= tmpBufSize) {
	// While not enough room, double size of buffer
	while (nBytes >= tmpBufSize)
	    tmpBufSize *= 2;

	tmpBuffer = realloc(tmpBuffer, tmpBufSize);

	// Test for bad reallocation
	if (tmpBuffer == NULL)
	    return FALSE;
    }

    return TRUE;
}

