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
 |	SoError base class.
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <stdarg.h>

#include <Inventor/SoPath.h>
#include <Inventor/engines/SoEngine.h>
#include <Inventor/errors/SoError.h>
#include <Inventor/nodes/SoNode.h>

// Static variables declared in SoError.h:
SoType		 SoError::classTypeId;
SoErrorCB	*SoError::handlerCB;
void		*SoError::cbData;
SbBool		 SoError::wasInitted;

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns type id of instance.
//
// Use: public

SoType
SoError::getTypeId() const
//
////////////////////////////////////////////////////////////////////////
{
    return classTypeId;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if error is an instance of the given type or a
//    subclass of it.
//
// Use: public

SbBool
SoError::isOfType(SoType type) const
//
////////////////////////////////////////////////////////////////////////
{
    return getTypeId().isDerivedFrom(type);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Posts an error. The debugString will be created from the given
//    arguments, which are in printf() format.
//
// Use: extender

void
SoError::post(const char *formatString ...)
//
////////////////////////////////////////////////////////////////////////
{
    SoError	error;

    // Set up the debugString. This just passes everything to sprintf,
    // using the variable arguments stuff in stdarg.h. I got this from
    // the C++ Reference Manual, page 148.

    char	buf[10000];
    va_list	ap;

    va_start(ap, formatString);
    vsprintf(buf, formatString, ap);
    va_end(ap);

    error.setDebugString("Inventor error: ");
    error.appendToDebugString(buf);
    error.handleError();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns printable string representing a node.
//
// Use: extender

SbString
SoError::getString(const SoNode *node)
//
////////////////////////////////////////////////////////////////////////
{
    return getBaseString(node, "node");
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns printable string representing a path.
//
// Use: extender

SbString
SoError::getString(const SoPath *path)
//
////////////////////////////////////////////////////////////////////////
{
    return getBaseString(path, "path");
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns printable string representing an engine.
//
// Use: extender

SbString
SoError::getString(const SoEngine *engine)
//
////////////////////////////////////////////////////////////////////////
{
    return getBaseString(engine, "engine");
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    The default error handler callback - it just prints to stderr.
//
// Use: protected, static

void
SoError::defaultHandlerCB(const SoError *error, void *)
//
////////////////////////////////////////////////////////////////////////
{
    fprintf(stderr, "%s\n", error->getDebugString().getString());
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns handler callback (and data) to use for a given instance.
//
// Use: protected, virtual

SoErrorCB *
SoError::getHandler(void *&data) const
//
////////////////////////////////////////////////////////////////////////
{
    data = cbData;
    return handlerCB;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Calls appropriate handler for an error instance. Application
//    writers can set breakpoints at this when debugging.
//
// Use: protected

void
SoError::handleError()
//
////////////////////////////////////////////////////////////////////////
{
    // Determine the handler to call and call it
    void	*data;
    SoErrorCB	*handler = getHandler(data);

    // If the handler is NULL, this can mean one of two things. Either
    // the SoError class was never initialized, or someone purposely
    // set the handler to NULL. We can use the wasInitted flag to
    // tell these cases apart.

    if (handler == NULL) {

	// If the class was initialized, someone set the handler to
	// NULL. So don't do anything.
	if (wasInitted)
	    ;

	// The class was never initialized. Use the default handler
	else
	    defaultHandlerCB(this, data);
    }

    else
	(*handler)(this, data);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Used by all getString() methods.
//
// Use: private

SbString
SoError::getBaseString(const SoBase *base, const char *what)
//
////////////////////////////////////////////////////////////////////////
{
    const SbName	&baseName = base->getName();
    SbString		str;
    char		addrBuf[32];

    str  = what;

    if (! (! baseName)) {
	str += " named \"";
	str += baseName.getString();
	str += "\"";
    }

    sprintf(addrBuf, "%#x", base);
    str += " at address ";
    str += addrBuf;

    return str;
}
