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
 |	SoSFTrigger
 |
 |   Author(s)		: Ronen Barzel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_SF_TRIGGER_
#define  _SO_SF_TRIGGER_

#include <Inventor/fields/SoSubField.h>

//////////////////////////////////////////////////////////////////////////////
//
//  SoSFTrigger subclass of SoSField.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoSTrigger
class SoSFTrigger : public SoSField {

    // Use standard field stuff
    SO_SFIELD_REQUIRED_HEADER(SoSFTrigger);
    SO_SFIELD_CONSTRUCTOR_HEADER(SoSFTrigger);

  public:
    // equivalent to SoField::touch().  Kept for consistency
    void                setValue()		{ touch(); }

    // getValue is a no-op:
    void                getValue() const	{ }

    // Start notification.
    virtual void	touch();

    // Equality/inequality tests. Since there are no values, trigger
    // fields are all the same
    int			operator ==(const SoSFTrigger &t) const;
    int			operator !=(const SoSFTrigger &t) const;

  SoINTERNAL public:
    static void		initClass();

    // Notification can flow through a trigger, but can't start at a
    // trigger.  The only way to start notification at a trigger is to
    // call touch().
    virtual void	startNotify();

    // Override notify to always evaluate (after passing on
    // notification) to clear upstream dirty bits
    virtual void	notify(SoNotList *list);

  private:
    // Reading and writing. These are no-ops.
    virtual SbBool	readValue(SoInput *in);
    virtual void	writeValue(SoOutput *out) const;
};

#endif /* _SO_SF_TRIGGER_ */
