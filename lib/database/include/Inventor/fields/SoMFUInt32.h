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
 |   Classes:
 |	SoMFUInt32
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_MF_UINT32_
#define  _SO_MF_UINT32_

#include <Inventor/fields/SoSubField.h>

//////////////////////////////////////////////////////////////////////////////
//
//  SoMFUInt32 subclass of SoMField.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoMUInt32
class SoMFUInt32 : public SoMField {

    // Use standard field stuff
    SO_MFIELD_HEADER(SoMFUInt32, uint32_t, uint32_t);

  SoINTERNAL public:
#ifndef IV_STRICT
#if (_MIPS_SZLONG == 32)
//
//  If the system long has exactly 32 bits, support methods that have "long" 
//  in the argument list.
//
    int   find(unsigned long targetValue,			  // System long
				          SbBool addIfNotFound = FALSE)
		{ return find((uint32_t)targetValue, addIfNotFound);}
    void  setValues(int start, int num, 
			const unsigned long *newValues)		  // System long
	    	{ setValues(start, num, (const uint32_t *)newValues);}
    void  set1Value(int index, unsigned long newValue)		  // System long
		{ set1Value(index, (uint32_t)newValue); }
    unsigned long  operator =(unsigned long newValue)		  // System long
		{ setValue((uint32_t)newValue); return (newValue); }
//
//  Problems in SoFieldConverters.c++ (more than one instance of ...)
//    void  setValue(unsigned long newValue)			  // System long
//		{ setValue((uint32_t)newValue); }
#endif
#endif

    static void		initClass();

  private:
    // Returns number of ASCII values to write per output line
    virtual int		getNumValuesPerLine() const;

    // Write/Read the values as a block of data
    virtual void	writeBinaryValues(SoOutput *out) const;
    virtual SbBool	readBinaryValues(SoInput *in, int numToRead);
};

#endif /* _SO_MF_UINT32_ */
