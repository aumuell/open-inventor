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
 |   $Revision: 1.1.1.1 $
 |
 |   Classes:
 |	SoSFTrigger
 |
 |   Author(s)		: Ronen Barzel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/fields/SoSFTrigger.h>
#include <Inventor/misc/SoNotification.h>

//////////////////////////////////////////////////////////////////////////////
//
// SoSFTrigger class
//
//////////////////////////////////////////////////////////////////////////////

// Use most of the standard stuff:
SO__FIELD_ID_SOURCE(SoSFTrigger);
SO__FIELD_EQ_SAME_SOURCE(SoSFTrigger);
SO_SFIELD_CONSTRUCTOR_SOURCE(SoSFTrigger);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Comparison.  Always returns TRUE.
//
// Use: internal

int
SoSFTrigger::operator ==(const SoSFTrigger &) const
//
////////////////////////////////////////////////////////////////////////
{
    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Comparison.  Always returns FALSE.
//
// Use: internal

int
SoSFTrigger::operator !=(const SoSFTrigger &) const
//
////////////////////////////////////////////////////////////////////////
{
    return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Method required to copy a field.  For triggers, just touches the
//    destination, ignoring the source
//
// Use: internal

const SoSFTrigger &
SoSFTrigger::operator =(const SoSFTrigger &)
//
////////////////////////////////////////////////////////////////////////
{
    touch();
    return *this;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    "Reads value from file." Doesn't bother reading anything, since
// write method is a no-op.  Note that trigger fields might get
// written/read for field connections to be written/read, so it's not
// an error for this routine to be called.
//
// Use: private

SbBool
SoSFTrigger::readValue(SoInput *)
//
////////////////////////////////////////////////////////////////////////
{
    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    "Writes value of field to file."  Doesn't bother writing anything.
// Note that trigger fields might get written/read for field connections
// to be written/read, so it's not an error for this routine to be
// called.
//
// Use: private

void
SoSFTrigger::writeValue(SoOutput *) const
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Override startNotify to prevent notification from going any
//    further than just this field.
//    Temporarily sets container to NULL, which prevents notification
//    from going to downstream connections.  Relies on the dirty bit
//    being set and evaluate getting called to clear it (in notify()).
//
// Use: internal

void
SoSFTrigger::startNotify()
//
////////////////////////////////////////////////////////////////////////
{
    SoFieldContainer *container = getContainer();
    setContainer(NULL);

    SoField::startNotify();

    setContainer(container);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Override notification to always evaluate, clearing upstream dirty
//    bits.
//
// Use: internal

void
SoSFTrigger::notify(SoNotList *list)
//
////////////////////////////////////////////////////////////////////////
{
    SoField::notify(list);
    evaluate();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Override touch to notify.
//
// Use: internal

void
SoSFTrigger::touch()
//
////////////////////////////////////////////////////////////////////////
{
    SoField::startNotify();  // Use SoField method explicitly
}

