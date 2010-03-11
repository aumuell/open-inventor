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
 |	SoDebugError class.
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/errors/SoDebugError.h>
#include <stdarg.h>

// Static variables declared in SoDebugError.h:
SoType		SoDebugError::classTypeId;
SoErrorCB	*SoDebugError::handlerCB;
void		*SoDebugError::cbData;

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns type id of instance.
//
// Use: public

SoType
SoDebugError::getTypeId() const
//
////////////////////////////////////////////////////////////////////////
{
    return classTypeId;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Posts an error.
//
// Use: extender

void
SoDebugError::post(const char *methodName, const char *formatString ...)
//
////////////////////////////////////////////////////////////////////////
{
    SoDebugError	error;
    SbString		str;

    // Same stuff as in base class
    char	buf[10000];
    va_list	ap;

    va_start(ap, formatString);
    vsprintf(buf, formatString, ap);
    va_end(ap);

    str  = "Inventor error in ";
    str += methodName;
    str += "(): ";
    str += buf;

    error.severity = ERROR;
    error.setDebugString(str.getString());
    error.handleError();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Posts a warning.
//
// Use: extender

void
SoDebugError::postWarning(const char *methodName, const char *formatString ...)
//
////////////////////////////////////////////////////////////////////////
{
    SoDebugError	error;
    SbString		str;

    // Same stuff as in base class
    char	buf[10000];
    va_list	ap;

    va_start(ap, formatString);
    vsprintf(buf, formatString, ap);
    va_end(ap);

    str  = "Inventor warning in ";
    str += methodName;
    str += "(): ";
    str += buf;

    error.severity = WARNING;
    error.setDebugString(str.getString());
    error.handleError();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Posts an informational (non-error) message.
//
// Use: extender

void
SoDebugError::postInfo(const char *methodName, const char *formatString ...)
//
////////////////////////////////////////////////////////////////////////
{
    SoDebugError	error;
    SbString		str;

    // Same stuff as in base class
    char	buf[10000];
    va_list	ap;

    va_start(ap, formatString);
    vsprintf(buf, formatString, ap);
    va_end(ap);

    str  = "Inventor: in ";
    str += methodName;
    str += "(): ";
    str += buf;

    error.severity = INFO;
    error.setDebugString(str.getString());
    error.handleError();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns handler callback (and data) to use for a given instance.
//
// Use: protected, virtual

SoErrorCB *
SoDebugError::getHandler(void *&data) const
//
////////////////////////////////////////////////////////////////////////
{
    data = cbData;
    return handlerCB;
}
