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
 * Copyright (C) 1990,91,92   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Classes:
 |	SoXtInputFocus
 |
 |   Author(s): David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <X11/X.h>
#include <Inventor/SbTime.h>
#include <Inventor/Xt/devices/SoXtInputFocus.h>

////////////////////////////////////////////////////////////////////////
//
// Description:
//   Constructor.
//
// public
//
SoXtInputFocus::SoXtInputFocus(EventMask mask)
//
////////////////////////////////////////////////////////////////////////
{
//??? for DEBUG, need to make sure the passed event mask is valid 
//??? for this device
    eventMask = mask;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   This selects input for input focus events which occur in w.
// The callback routine is proc, and the callback data is clientData.
//
// virtual public
//
void
SoXtInputFocus::enable(
    Widget w,
    XtEventHandler proc, 
    XtPointer clientData,
    Window)
//
////////////////////////////////////////////////////////////////////////
{
    XtAddEventHandler(w, eventMask, FALSE, proc, clientData);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   This unselects input focus events which occur in w.
//
// virtual public
//
void
SoXtInputFocus::disable(
    Widget w,
    XtEventHandler proc, 
    XtPointer clientData)
//
////////////////////////////////////////////////////////////////////////
{
    XtRemoveEventHandler(w, eventMask, FALSE, proc, clientData);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   NOTE: there are no SoEvents for X input focus events.
//
// static public
//
const SoEvent *
SoXtInputFocus::translateEvent(XAnyEvent *)
//
////////////////////////////////////////////////////////////////////////
{
    return NULL;
}

