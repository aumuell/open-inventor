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
 |	SoSFImage
 |
 |   Author(s)		: Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_SF_IMAGE_
#define  _SO_SF_IMAGE_

#include <Inventor/fields/SoSubField.h>
#include <Inventor/SbLinear.h>

//////////////////////////////////////////////////////////////////////////////
//
//  SoSFImage subclass of SoSField.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoSImg
class SoSFImage : public SoSField {
    // Uses only some of the standard field stuff (because there is no
    // SbImage type):
    SO_SFIELD_REQUIRED_HEADER(SoSFImage);
    SO_SFIELD_CONSTRUCTOR_HEADER(SoSFImage);

  public:
    // getValue returns the size, number of components and a constant
    // pointer to the image.
    // C-api: name=get
    const unsigned char *	getValue(SbVec2s &size, int &nc) const;

    // setValue copies the image given to it into internal storage.
    // See startEditing() for a way of avoiding the copy if you are
    // doing a getValue() followed immediately by a setValue().
    // C-api: name=set
    void			setValue(const SbVec2s &size, int nc,
					 const unsigned char *bytes);
    
    // Equality/inequality tests
    int				operator ==(const SoSFImage &f) const;
    int				operator !=(const SoSFImage &f) const
	{ return ! ((*this) == f); }

    // Avoid copying the values in/out, if you are just changing the
    // bytes and not changing the dimensions of the image.  This is
    // equivalent to getValue, but returns a pointer you can change.
    // C-api: name=startEdit
    unsigned char *		startEditing(SbVec2s &size, int &nc);
    // C-api: name=finishEdit
    void			finishEditing();

  SoINTERNAL public:
    static void		initClass();

  private:
    SbVec2s		size;		// Width and height of image
    int			numComponents;	// Number of components per pixel
    unsigned char *	bytes;		// Array of pixels

    // Reading and writing
    virtual SbBool	readValue(SoInput *in);
    virtual void	writeValue(SoOutput *out) const;
};

#endif /* _SO_SF_IMAGE_ */
