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
 * Copyright (C) 1990-93   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.4 $
 |
 |   Classes:
 |	SoOutput
 |
 |   Notes: This file includes any machine-dependent `dgl' (data goo lib)
 |   code for each machine.  See dgl.h.
 |
 |   Author(s)		: Paul S. Strauss, Dave Immel (dgl/cray)
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <stdio.h>
#include <stdlib.h>
#include <Inventor/errors/SoDebugError.h>

#include <machine.h>		// Inventor data goo lib

#include <Inventor/SoDB.h>
#include <Inventor/SoOutput.h>

/////////////////////////////////////////////////////////////////////////
//
// Note: The following dgl code is for machine specific implemenations.
//       The SGI implementation is defined in dgl.h and does not
//       require any functions (macros only).
//
////////////////////// BEGIN CRAY-DEPENDENT CODE ////////////////////////
//
// Following are functions that are used to convert from the
// host format to the network-neutral format, for CRAY machines:
//
//	DGL_HTON_SHORT(t,f)	host to network - short
//	DGL_HTON_INT32(t,f)	host to network - int32_t
//	DGL_HTON_FLOAT(PC,vc)	host to network - float
//	DGL_HTON_DOUBLE(PC,vc)	host to network - double
//

#ifdef _CRAY

void DGL_HTON_SHORT( char *t, short f )
{
    t[0] = f >> 8;
    t[1] = f;
}
void DGL_HTON_INT32( char *t, int32_t f )
{
    t[0] = f >> 24;
    t[1] = f >> 16;
    t[2] = f >> 8;
    t[3] = f;
}

//
// BIG_IEEE: no conversion necessary (FLOAT)
//
// (Not portable. This routine works on Crays.)
// IEEE single precision floating point for a Cray.
//

struct	ieee_single {
    unsigned int	zero	: 32;	/* Upper 32 bits are junk */
    unsigned int	sign	: 1;
    unsigned int	exp	: 8;
    unsigned int	mantissa: 23;	/* 24-bit mantissa with 1 hidden bit */
};

/* Cray floating point, partitioned for easy conversion to IEEE single */
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

/* Cray exponent limits for conversion to IEEE single */
#define	MAX_CRAY_SNG	(0x100 + CRAY_BIAS - IEEE_SNG_BIAS)
#define	MIN_CRAY_SNG	(0x00 + CRAY_BIAS - IEEE_SNG_BIAS)

static struct ieee_single min_sng_ieee = { 0, 0, 0x00, 0 };
static struct ieee_single max_sng_ieee = { 0, 0, 0xff, 0 };

static struct cray_single max_sng_cray = { 0, 0x6000, 0, 0 } ;
static struct cray_double max_dbl_cray = { 0, 0x6000, 0 } ;

#define IEEE_SNG_BIAS   0x7f

static void DGL_HTON_FLOAT( char *PC, struct cray_single vc ) {
    struct ieee_single ais;
    union {
	struct ieee_single is;
	unsigned iis;
    } ieee;

    if (vc.exp >= MAX_CRAY_SNG) ieee.is = max_sng_ieee;
    else if ( vc.exp < MIN_CRAY_SNG ||
	    ( vc.mantissa == 0 && vc.mantissa2 == 0  ) )
    {
	//  On the Cray, there is no hidden mantissa bit.
	//  So, if the mantissa is zero, the number is zero.
	ieee.is = min_sng_ieee ;
    }
    else {
	ieee.is.exp = vc.exp - CRAY_BIAS + IEEE_SNG_BIAS;
	ieee.is.mantissa = vc.mantissa;
	/* Hidden bit removed by truncation */
    }
    ieee.is.sign = vc.sign;
    DGL_HTON_INT32(PC,ieee.iis);
}

// IEEE double precision floating point for a Cray,
// (the first word, anyway - second word is pure mantissa2).

struct  ieee_double {
    unsigned int    sign      : 1;
    unsigned int    exp       : 11;
    unsigned int    mantissa  : 52;
};

static struct ieee_double min_dbl_ieee = { 0, 0x000, 0 };
static struct ieee_double max_dbl_ieee = { 0, 0x7ff, 0 };

#define IEEE_DBL_BIAS   0x3ff

static void DGL_HTON_DOUBLE( char *PC, struct cray_double vc ) {
    union {
	struct ieee_double is;
	char iis[M_SIZEOF(double)];
    } ieee;

    if(vc.exp >= MAX_CRAY_SNG) ieee.is = max_dbl_ieee;
    else if ( vc.exp < MIN_CRAY_SNG || vc.mantissa == 0 ) {
	//  On the Cray, there is no hidden mantissa bit.
	//  So, if the mantissa is zero, the number is zero.
	ieee.is = min_dbl_ieee ;
    }
    else {
	ieee.is.exp = vc.exp - CRAY_BIAS + IEEE_DBL_BIAS;
	ieee.is.mantissa = vc.mantissa << 5;
	/* Hidden bit removed by truncation */
    }
    ieee.is.sign = vc.sign;
    memcpy(PC, ieee.iis, M_SIZEOF(double));
}

#endif // _CRAY
//////////////////////// END CRAY-DEPENDENT CODE ////////////////////////

static const char *defaultASCIIHeader =  "#Inventor V2.1 ascii";
static const char *defaultBinaryHeader = "#Inventor V2.1 binary";
    
////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor - sets output to stdout by default.
//
// Use: public

SoOutput::SoOutput()
//
////////////////////////////////////////////////////////////////////////
{
    fp		= stdout;
    buffer	= NULL;
    toBuffer	= FALSE;
    openedHere	= FALSE;
    anyRef	= FALSE;
    binary	= FALSE;
    compact	= FALSE;
    wroteHeader	= FALSE;
    tmpBuffer   = NULL;
    tmpBufSize  = 0;
    refDict	= new SbDict;
    borrowedDict= FALSE;
    annotation	= 0;
    headerString = SbString("");
    fmtString	= SbString("%g");

    reset();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor that gets reference dictionary from another SoOutput.
//
// Use: internal

SoOutput::SoOutput(SoOutput *dictOut)
//
////////////////////////////////////////////////////////////////////////
{
    fp		= stdout;
    buffer	= NULL;
    toBuffer	= FALSE;
    openedHere	= FALSE;
    anyRef	= FALSE;
    binary	= FALSE;
    compact	= FALSE;
    wroteHeader	= FALSE;
    tmpBuffer   = NULL;
    tmpBufSize  = 0;
    annotation	= 0;
    fmtString	= SbString("%g");

    if (dictOut == NULL) {
	borrowedDict = FALSE;
	refDict = new SbDict;
    }
    else {
	borrowedDict = TRUE;
	refDict	= dictOut->refDict;
    }

    reset();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor - closes output file if it was opened by SoOutput.
//
// Use: public

SoOutput::~SoOutput()
//
////////////////////////////////////////////////////////////////////////
{
    closeFile();

    if (! borrowedDict)
	delete refDict;

    if (tmpBuffer != NULL)
        free((void *)tmpBuffer);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets file pointer to write to; resets indentation level.
//
// Use: public

void
SoOutput::setFilePointer(FILE *newFP)		// New file pointer
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    if (newFP == NULL)
	SoDebugError::postWarning("SoOutput::setFilePointer",
				  "Setting file pointer to NULL - "
				  "may cause problems");
#endif /* DEBUG */

    // Close open file, if any
    closeFile();

    fp		= newFP;
    openedHere	= FALSE;
    wroteHeader = FALSE;
    toBuffer	= FALSE;

    if (isBinary() && (tmpBuffer == NULL)) {
        tmpBuffer = (char *)malloc(64);
        tmpBufSize = 64;
    }

    reset();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns file pointer writing to, or NULL if writing to buffer.
//
// Use: public

FILE *
SoOutput::getFilePointer() const
//
////////////////////////////////////////////////////////////////////////
{
    return isToBuffer() ? NULL : fp;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Opens named file, sets file pointer to result. If it can't open
//    the file, it prints an error message, leaves the file pointer
//    alone, and returns FALSE.
//
// Use: public

SbBool
SoOutput::openFile(const char *fileName)	// Name of file
//
////////////////////////////////////////////////////////////////////////
{
    FILE *newFP = fopen(fileName, "w");

    // Close open file, if any
    closeFile();

    if (newFP == NULL) {
	SoDebugError::post("SoOutput::openFile",
			   "Can't open file \"%s\" for writing", fileName);
	return FALSE;
    }

    fp		= newFP;
    openedHere	= TRUE;
    wroteHeader = FALSE;
    toBuffer	= FALSE;

    reset();

    if (isBinary() && (tmpBuffer == NULL)) {
        tmpBuffer = (char *)malloc(64);
        tmpBufSize = 64;
    }

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Closes current file if it was opened with openFile(). If not,
//    this does nothing, so it's always safe to call.
//
// Use: public

void
SoOutput::closeFile()
//
////////////////////////////////////////////////////////////////////////
{
    if (openedHere) {
	fclose(fp);
	openedHere = FALSE;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets up buffer to write to, initial size, reallocation function,
// and offset at which to start writing.
//
// Use: public

void
SoOutput::setBuffer(void *bufPointer, size_t initSize,
		    SoOutputReallocCB *f, int32_t offset)
//
////////////////////////////////////////////////////////////////////////
{
    buffer  = bufPointer;
    curBuf  = (char *) bufPointer;
    bufSize = initSize;
    reallocFunc = f;
    
    // make sure there are enough bytes in buffer to start writing at offset
    if (offset > 0) {
	makeRoomInBuf((int) (offset + 1));
	curBuf = (char *) buffer + (int) offset;
    }
    
    if (tmpBuffer != NULL) {
        free( (void *)tmpBuffer );
        tmpBuffer = NULL;
        tmpBufSize = 0;
    }

    wroteHeader = FALSE;

    toBuffer = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns pointer to buffer and the buffer's new size.
//    Returns FALSE if not writing into buffer.
//
// Use: public

SbBool
SoOutput::getBuffer(void *&bufPointer, size_t &nBytes) const
//
////////////////////////////////////////////////////////////////////////
{
    if (isToBuffer()) {
	bufPointer = buffer;
	nBytes     = bytesInBuf();

	return TRUE;
    }

    return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Resets buffer for output again; output starts over at beginning.
//
// Use: public

void
SoOutput::resetBuffer()
//
////////////////////////////////////////////////////////////////////////
{
    curBuf = (char *) buffer;

    wroteHeader = FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Indicates whether output should be ASCII (default) or binary.
//
// Use: public

void
SoOutput::setBinary(SbBool flag)
//
////////////////////////////////////////////////////////////////////////
{
    binary = flag;

    // If writing to file, initialize the temporary output for buffering
    // data before writing.
    if (!isToBuffer())
    {
        tmpBuffer = (char *)malloc(64);
        tmpBufSize = 64;
    }

}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the precision for outputing real numbers
//
// Use: public

void
SoOutput::setFloatPrecision(int precision)
//
////////////////////////////////////////////////////////////////////////
{
    char tmp[8];

    // Invalid precision specified; use default format string
    if (precision < 0 || precision > 8)
    {
#ifdef DEBUG
	SoDebugError::postWarning("SoOutput::setFloatPrecision",
		"Precision (significant digits) must be between 0 "
		"and 8 for %.xg format");
#endif /* DEBUG */
	fmtString = SbString("%g");
    }

    // Build the output format string from the input parameters
    else
    {
	sprintf(tmp, "%%.%dg", precision);
	fmtString = SbString(tmp);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets the header to be used for writing the file to be a 
//    user-specified string.
//
// Use: public

void
SoOutput::setHeaderString(const SbString &str)
//
////////////////////////////////////////////////////////////////////////
{
    headerString = str;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reset the user-specified header string, so that the default headers
//    will be used
//
// Use: public

void
SoOutput::resetHeaderString()
//
////////////////////////////////////////////////////////////////////////
{
    headerString.makeEmpty();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Return the default ASCII header string (ie the latest version
//    of the standard Inventor ascii header)
//
// Use: public, static

SbString
SoOutput::getDefaultASCIIHeader()
//
////////////////////////////////////////////////////////////////////////
{
    return (SbString(defaultASCIIHeader));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Return the default Binary header string (ie the latest version
//    of the standard Inventor binary header).  Note binary headers
//    must always be padded for correct alignment in binary files.
//
// Use: public, static

SbString
SoOutput::getDefaultBinaryHeader()
//
////////////////////////////////////////////////////////////////////////
{
    return (SoOutput::padHeader(SbString(defaultBinaryHeader)));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Pad a string to a valid length for binary headers (ie one less
//    than a multiple of 4)
//
// Use: private, static

SbString
SoOutput::padHeader(const SbString &str)
//
////////////////////////////////////////////////////////////////////////
{
    SbString paddedStr(str);
    
    int pad = 3 - (str.getLength()%4);    
    for (int i = 0; i < pad; i++)
	paddedStr += " ";
	
    return (paddedStr);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Writes a character to current file/buffer.
//
// Use: public

void
SoOutput::write(char c)
//
////////////////////////////////////////////////////////////////////////
{
    if (! wroteHeader)
	writeHeader();

    if (isToBuffer() && ! makeRoomInBuf(4))
	return;

    if (isBinary()) {
        if (isToBuffer()) {
   	    *curBuf++ = c;
            *curBuf++ = 0;
            *curBuf++ = 0;
            *curBuf++ = 0;
        }
        else {
            *tmpBuffer = c;
            tmpBuffer[1] = 0;
            tmpBuffer[2] = 0;
            tmpBuffer[3] = 0;
            fwrite((void *)tmpBuffer, M_SIZEOF(char), 4, fp);
            fflush(fp);
        }
    }

    else if (! isToBuffer())
	putc(c, fp);

    else
	*curBuf++ = c;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Writes a NULL-terminated character string to current file/buffer.
//
// Use: public

void
SoOutput::write(const char *s)
//
////////////////////////////////////////////////////////////////////////
{
    int n = strlen(s);
    int nsize = (n + 3) & ~0003;

    if (! wroteHeader)
	writeHeader();


    if (isToBuffer() && ! makeRoomInBuf(nsize))	// Leave room for end NUL char
	return;

    if (isBinary())
    {
        //
        // Writing a binary string consists of writing a word representing
        // the string length without the NULL character, followed by the
        // string, followed by padding out to the word boundary.
        //
        if (isToBuffer()) {
            int m = n;
            DGL_HTON_INT32(m, n);
            *((int *)curBuf) = m;
            curBuf += 4;
	    memcpy((void *)curBuf, (const void *)s, n);
	    curBuf += n;
            for (int i=0; i<(nsize-n); i++) 
                *curBuf++ = 0;
        }
        else {
            if (!makeRoomInTmpBuf(nsize))
                return;
            int m = n;
            DGL_HTON_INT32(m, n);
            fwrite((void *)&m, sizeof(int), 1, fp);
	    memcpy(tmpBuffer, (const void *)s, n);
            for (int i=0; i<(nsize-n); i++) 
                tmpBuffer[n+i] = 0;
            fwrite((void *)tmpBuffer, sizeof(char), nsize, fp);
            fflush(fp);
        }
    }

    else if (! isToBuffer())
        fputs(s, fp);

    else {
	strcpy(curBuf, s);
	curBuf += n;		// Don't increment over NUL char
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Writes an SbString to current file/buffer. This will always write out
//    double quotes around the string and will make sure that
//    backslashes are properly escaped.
//
// Use: public

void
SoOutput::write(const SbString &s)
//
////////////////////////////////////////////////////////////////////////
{
    if (isBinary())
	write(s.getString());

    else {
	const char *c;

	write('\"');

	for (c = s.getString(); *c != '\0'; c++) {
	    if (*c == '\"')
		write('\\');
	    write(*c);
	}

	write('\"');
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Writes an SbName to current file/buffer. This will always write out
//    double quotes around the string and will make sure that
//    backslashes are properly escaped.
//
// Use: public

void
SoOutput::write(const SbName &s)
//
////////////////////////////////////////////////////////////////////////
{
    if (isBinary())
	write(s.getString());

    else {
	const char *c;

	write('\"');

	for (c = s.getString(); *c != '\0'; c++) {
	    if (*c == '\"')
		write('\\');
	    write(*c);
	}

	write('\"');
    }
}

////////////////////////////////////////////////////////////////////////
//
// This macro is used by all the functions that write numbers.
//
////////////////////////////////////////////////////////////////////////

#define WRITE_NUM(num, formatString, dglFunc, dglType)   		      \
    if (! wroteHeader)							      \
	writeHeader();							      \
    if (isBinary()) {							      \
	if (isToBuffer() && ! makeRoomInBuf(M_SIZEOF(dglType)))		      \
	    return;							      \
        if (isToBuffer()) {                                                   \
            dglFunc(num, curBuf);	  				      \
            curBuf += M_SIZEOF(dglType);                                      \
        }                                                                     \
        else {                                                                \
            if (!makeRoomInTmpBuf(M_SIZEOF(dglType)))			      \
                return;							      \
            dglFunc(num, tmpBuffer);                                          \
            fwrite((void *)tmpBuffer, M_SIZEOF(dglType), 1, fp);              \
            fflush(fp);                                                       \
        }                                                                     \
    }									      \
    else if (! isToBuffer())						      \
	fprintf(fp, formatString, num);					      \
    else {								      \
	char	str[20];						      \
	sprintf(str, formatString, num);				      \
	write(str);							      \
    }

#define WRITE_BIN_ARRAY(type, array, length, dglFunc)  			      \
    if (! wroteHeader)							      \
	writeHeader();							      \
    if (isToBuffer() && ! makeRoomInBuf(length*M_SIZEOF(type)))	              \
        return;							              \
    if (isToBuffer()) {                                                       \
        dglFunc(array, curBuf, length);                                       \
        curBuf += length * M_SIZEOF(type);                                    \
    }									      \
    else {                                                                    \
        if (!makeRoomInTmpBuf(length*M_SIZEOF(type))) 			      \
            return;							      \
        dglFunc(array, tmpBuffer, length);                                    \
        fwrite((void *)tmpBuffer, M_SIZEOF(type), length, fp);                \
        fflush(fp); 							      \
    }                                                                         \


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Writes an integer to current file/buffer.
//
// Use: public

void
SoOutput::write(int i)
//
////////////////////////////////////////////////////////////////////////
{
    WRITE_NUM(i, "%d", convertInt32, int32_t);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Writes an unsigned integer to current file/buffer.
//
// Use: public

void
SoOutput::write(unsigned int i)
//
////////////////////////////////////////////////////////////////////////
{
    WRITE_NUM(i, "%#x", convertInt32, int32_t);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Writes a short integer to current file/buffer.
//
// Use: public

void
SoOutput::write(short s)
//
////////////////////////////////////////////////////////////////////////
{
    int32_t l = (int32_t)s;
    WRITE_NUM(l, "%ld", convertInt32, int32_t);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Writes an unsigned short integer to current file/buffer.
//
// Use: public

void
SoOutput::write(unsigned short s)
//
////////////////////////////////////////////////////////////////////////
{
    uint32_t l = (uint32_t)s;
    WRITE_NUM(l, "%#lx", convertInt32, int32_t);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Writes a int32_t integer to current file/buffer.
//
// Use: public

//     made redundant by typedef of int32_t
//void
//SoOutput::write(int32_t l)
////
//////////////////////////////////////////////////////////////////////////
//{
//    WRITE_NUM(l, "%ld", convertInt32, int32_t);
//}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Writes an uint32_teger to current file/buffer.
//
// Use: public

//     made redundant by typedef of uint32_t
//void
//SoOutput::write(uint32_t l)
////
//////////////////////////////////////////////////////////////////////////
//{
//    WRITE_NUM(l, "%#lx", convertInt32, int32_t);
//}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Writes a float to current file/buffer.
//
// Use: public

void
SoOutput::write(float f)
//
////////////////////////////////////////////////////////////////////////
{
    WRITE_NUM(f, fmtString.getString(), convertFloat, float);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Writes a double to current file/buffer.
//
// Use: public

void
SoOutput::write(double d)
//
////////////////////////////////////////////////////////////////////////
{
    WRITE_NUM(d, "%.16lg", convertDouble, double);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Writes an array of unsigned chars to current file/buffer.
//
// Use: public

void
SoOutput::writeBinaryArray(unsigned char *c, int length)
//
////////////////////////////////////////////////////////////////////////
{
    if (! wroteHeader)
	writeHeader();
    if (isToBuffer() && ! makeRoomInBuf(length*M_SIZEOF(unsigned char)))
	return;
    if (isToBuffer()) {
	memcpy(curBuf, c, length);
	curBuf += length * M_SIZEOF(unsigned char);
    }
    else {
	fwrite((void *)c, M_SIZEOF(unsigned char), length, fp);
	fflush(fp);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Writes an array of int32_ts to current file/buffer.
//
// Use: public

void
SoOutput::writeBinaryArray(int32_t *l, int length)
//
////////////////////////////////////////////////////////////////////////
{
    WRITE_BIN_ARRAY(int32_t, l, length, convertInt32Array);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Writes an array of floats to current file/buffer.
//
// Use: public

void
SoOutput::writeBinaryArray(float *f, int length)
//
////////////////////////////////////////////////////////////////////////
{
    WRITE_BIN_ARRAY(float, f, length, convertFloatArray);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Writes an array of doubles to current file/buffer.
//
// Use: public

void
SoOutput::writeBinaryArray(double *d, int length)
//
////////////////////////////////////////////////////////////////////////
{
    WRITE_BIN_ARRAY(double, d, length, convertDoubleArray);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Converts short to network format and puts in buffer.
//
// Use: private

void
SoOutput::convertShort(short s, char *to)
//
////////////////////////////////////////////////////////////////////////
{
    int32_t l = (int32_t)s;
    char jjj[16];
    int i;

    DGL_HTON_INT32( INT32(jjj), l );
    for (i=0; i<M_SIZEOF(int32_t); i++)  to[i] = jjj[i];
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Converts long to network format and puts in buffer.
//
// Use: private

void
SoOutput::convertInt32(int32_t l, char *to)
//
////////////////////////////////////////////////////////////////////////
{
    char jjj[16];
    int i;

    DGL_HTON_INT32( INT32(jjj), l );

    for (i=0; i<M_SIZEOF(int32_t); i++)  to[i] = jjj[i];
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Converts float to network format and puts in buffer.
//
// Use: private

void
SoOutput::convertFloat(float f, char *to)
//
////////////////////////////////////////////////////////////////////////
{
    char jjj[64];
    int i;

    DGL_HTON_FLOAT( FLOAT(jjj), f );

    for (i=0; i<M_SIZEOF(float); i++)  to[i] = jjj[i];
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Converts double to network format and puts in buffer.
//
// Use: private

void
SoOutput::convertDouble(double d, char *to)
//
////////////////////////////////////////////////////////////////////////
{
    DGL_HTON_DOUBLE( DOUBLE(to), d );
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Converts array of shorts to network format and puts in buffer.
//
// Use: private

void
SoOutput::convertShortArray( register short *from,
                             char *to,
                             register int len)
//
////////////////////////////////////////////////////////////////////////
{
    register char *b = to;

    while (len > 4) {		// unroll the loop a bit
	DGL_HTON_SHORT( SHORT(b), from[0]);
	DGL_HTON_SHORT( SHORT(b + M_SIZEOF(short)),   from[1]);
	DGL_HTON_SHORT( SHORT(b + M_SIZEOF(short)*2), from[2]);
	DGL_HTON_SHORT( SHORT(b + M_SIZEOF(short)*3), from[3]);
	b += M_SIZEOF(short)*4;
	from += 4;
	len -= 4;
    }
    while (len-- > 0) {
	DGL_HTON_SHORT( SHORT(b),*from);
	b += M_SIZEOF(short);
	from++;
    }
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Converts array of int32_ts to network format and puts in buffer.
//
// Use: private

void
SoOutput::convertInt32Array( int32_t *from,
                            char *to,
                            register int len)
//
////////////////////////////////////////////////////////////////////////
{
    register char  *b = to;
    register int32_t  *f = from;

    while (len > 4) {		// unroll the loop a bit
	DGL_HTON_INT32( INT32(b), f[0]);
	DGL_HTON_INT32( INT32(b + M_SIZEOF(int32_t)),   f[1]);
	DGL_HTON_INT32( INT32(b + M_SIZEOF(int32_t)*2), f[2]);
	DGL_HTON_INT32( INT32(b + M_SIZEOF(int32_t)*3), f[3]);
	b += M_SIZEOF(int32_t)*4;
	f += 4;
	len -= 4;
    }
    while (len-- > 0) {
	DGL_HTON_INT32( INT32(b),*f);
	b += M_SIZEOF(int32_t);
	f++;
    }
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Converts array of floats to network format and puts in buffer.
//
// Use: private

void
SoOutput::convertFloatArray( float *from,
                             char *to,
                             register int len)
//
////////////////////////////////////////////////////////////////////////
{
    register char  *b = to;
    register float *f = from;

    while (len > 4) {		// unroll the loop a bit
	DGL_HTON_FLOAT( FLOAT(b), f[0]);
	DGL_HTON_FLOAT( FLOAT(b + M_SIZEOF(float)),   f[1]);
	DGL_HTON_FLOAT( FLOAT(b + M_SIZEOF(float)*2), f[2]);
	DGL_HTON_FLOAT( FLOAT(b + M_SIZEOF(float)*3), f[3]);
	b += M_SIZEOF(float)*4;
	f += 4;
	len -= 4;
    }
    while (len-- > 0) {
	DGL_HTON_FLOAT( FLOAT(b),*f);
	b += M_SIZEOF(float);
	f++;
    }
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Converts array of doubles to network format and puts in buffer.
//
// Use: private

void
SoOutput::convertDoubleArray( register double *from,
                              char *to,
                              register int len )
//
////////////////////////////////////////////////////////////////////////
{
    register char *b = to;

    while (len > 4) {		// unroll the loop a bit
	DGL_HTON_DOUBLE( DOUBLE(b), from[0]);
	DGL_HTON_DOUBLE( DOUBLE(b + M_SIZEOF(double)),   from[1]);
	DGL_HTON_DOUBLE( DOUBLE(b + M_SIZEOF(double)*2), from[2]);
	DGL_HTON_DOUBLE( DOUBLE(b + M_SIZEOF(double)*3), from[3]);
	b += M_SIZEOF(double)*4;
	from += 4;
	len -= 4;
    }
    while (len-- > 0) {
	DGL_HTON_DOUBLE( DOUBLE(b),*from);
	b += M_SIZEOF(double);
	from++;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Writes indentation to file based on current indentation level.
//
// Use: public

void
SoOutput::indent()
//
////////////////////////////////////////////////////////////////////////
{
    int		i;

    for (i = indentLevel / 2; i > 0; --i)
	write('\t');

    if (indentLevel & 1) {
	write(' ');
	write(' ');
	write(' ');
	write(' ');
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Resets things for writing to a new file or changing files
//
// Use: internal

void
SoOutput::reset()
//
////////////////////////////////////////////////////////////////////////
{
    // If writing ASCII into buffer in memory, make sure it's
    // NULL-byte-terminated
    if (isToBuffer() && ! isBinary()) {
	write('\0');

	// However, subsequent writes (if any) should overwrite it
	curBuf--;
    }

    indentLevel  = 0;
    refIdCount = 0;

    if (anyRef) {

	// Clear dictionary
	refDict->clear();

	anyRef = FALSE;
	refIdCount = 0;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Writes correct file header string to current file or buffer. The
//    header must be written in normal ASCII form since it is read in
//    ASCII to determine whether the data is ASCII or binary. So if we
//    are writing in binary, we have to stop temporarily.
//
// Use: private

void
SoOutput::writeHeader()
//
////////////////////////////////////////////////////////////////////////
{
    wroteHeader = TRUE;

    // Don't write header if writing in compact form
    if (compact)
	return;

    if (isBinary()) {
	binary = FALSE;
	if (headerString == "") {
	    SbString defaultHeader = SoOutput::padHeader(defaultBinaryHeader);
	    write(defaultHeader.getString());
	} else {
	    // Make sure the string is padded for correct alignment, in case
	    // it's used in a binary file
	    SbString paddedString = SoOutput::padHeader(headerString);
	    write(paddedString.getString());
	}  
	write('\n');
	binary = TRUE;
    }

    else {
	if (headerString == "")
	    write(defaultASCIIHeader);
	else
	    write(headerString.getString());
	write('\n');
	write('\n');
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Makes sure current buffer can contain nBytes more bytes. Returns
//    FALSE if this is not possible.
//
// Use: private

SbBool
SoOutput::makeRoomInBuf(size_t nBytes)
//
////////////////////////////////////////////////////////////////////////
{
    size_t	bytesUsed  = bytesInBuf();
    size_t	roomNeeded = bytesUsed + nBytes;

    // If already had problems with buffer, stop
    if (buffer == NULL)
	return FALSE;

    // If not enough room in buffer for nBytes more, realloc
    if (roomNeeded >= bufSize) {
	// While not enough room, double size of buffer
	while (roomNeeded >= bufSize)
	    bufSize *= 2;
    
	// Now realloc a new buffer that is big enough
	buffer = (*reallocFunc)(buffer, bufSize);
    }
    
    // Test for bad reallocation
    if (buffer == NULL)
	return FALSE;
	    
    // Readjust current buffer pointer
    curBuf = (char *) buffer + bytesUsed;

    return TRUE;
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Makes sure temporary buffer can contain nBytes more bytes. Returns
//    FALSE if this is not possible.
//
// Use: private

SbBool
SoOutput::makeRoomInTmpBuf(size_t nBytes)
//
////////////////////////////////////////////////////////////////////////
{
    if (tmpBuffer == NULL)
        return FALSE;
	    
    // If not enough room in tmpBuffer for nBytes more, realloc
    if (nBytes >= tmpBufSize) {
	// While not enough room, double size of buffer
	while (nBytes >= tmpBufSize)
	    tmpBufSize *= 2;
	    
	// Now realloc a new buffer that is big enough
	tmpBuffer = (char *)::realloc((void *)tmpBuffer, tmpBufSize);
    }
    
    // Test for bad reallocation
    if (tmpBuffer == NULL)
	return FALSE;

    return TRUE;
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Adds a reference to dictionary, returning the name
//    created for the reference.  Called by SoBase::writeHeader.
//
// Use: private

int					// Returns reference id
SoOutput::addReference(const SoBase *base)	// Thing to enter
//
////////////////////////////////////////////////////////////////////////
{
    int referenceId = refIdCount++;

    // Enter in dictionary : generates a CC warning...
    refDict->enter((unsigned long) base, (void *) referenceId);

    // Dictionary is now "dirty"
    anyRef = TRUE;

    return referenceId;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Finds a reference in dictionary, returning the name of the
//    reference. Returns NULL if not found.
//
// Use: private

int					// Returns reference ID
SoOutput::findReference(const SoBase *base	// Thing to look for
			) const
//
////////////////////////////////////////////////////////////////////////
{
    int referenceId = -1;
    void *ref;

    // Generates a CC warning. Ho hum.
    if (refDict->find((unsigned long) base, ref))
#if (_MIPS_SZPTR == 64)
        referenceId = (int) ((unsigned long) ref);
#else
	referenceId = (int)ref;
#endif

    return referenceId;
}
