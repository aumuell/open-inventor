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
 |	SoNotRec, SoNotList
 |
 |   Author(s)		: Paul S. Strauss, Nick Thompson
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/misc/SoBase.h>
#include <Inventor/misc/SoNotification.h>
#include <Inventor/fields/SoField.h>
#include <Inventor/nodes/SoNode.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoNotRec (internal)
//
//////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Prints a notification record for debugging.
//
// Use: public

void
SoNotRec::print(FILE *fp) const
{
    const char *typeName;

    switch (type) {
      case CONTAINER:
	typeName = "CONTAINER";
	break;
      case PARENT:
	typeName = "PARENT";
	break;
      case SENSOR:
	typeName = "SENSOR";
	break;
      case FIELD:
	typeName = "FIELD";
	break;
      case ENGINE:
	typeName = "ENGINE";
	break;
      default:
	typeName = "UNSET";
	break;
    }

    if (fp == NULL)
	fp = stdout;

    fprintf(fp, "\tSoNotRec @%#x: type %s, base %#x", this, typeName, base);

    if (base != NULL) {
	const char *baseTypeName = base->getTypeId().getName().getString();
	fprintf(fp, " (type %s, \"%s\")",
		baseTypeName, base->getName().getString());
    }

    fprintf(fp, "\n");
}

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoNotList (internal)
//
//  These methods are not inline like because that would create an
//  #include loop with SoBase.h.
//
//////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor.
//
// Use: public

SoNotList::SoNotList()
{
    first = last = firstAtNode = NULL;
    lastField = NULL;
    timeStamp = SoNode::getNextNodeId();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Appends given non-field record to end of list.
//
// Use: public

void
SoNotList::append(SoNotRec *rec)
{
    rec->setPrevious(last);
    last = rec;
    if (first == NULL)
	first = rec;

    if (firstAtNode == NULL && 
	rec->getBase()->isOfType(SoNode::getClassTypeId()))
	firstAtNode = rec;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Appends given (container) field record to end of list.
//
// Use: public

void
SoNotList::append(SoNotRec *rec, SoField *field)
{
    append(rec);

    lastField   = field;
    firstAtNode = rec;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Prints a notification list for debugging.
//
// Use: public

void
SoNotList::print(FILE *fp) const
{
    const SoNotRec	*rec = last;

    if (fp == NULL)
	fp = stdout;

    if (rec == NULL) {
	fprintf(fp, "SoNotList: NULL list\n");
	return;
    }

    fprintf(fp, "SoNotList:\n");

    do {
	rec->print(fp);

	if (rec != first)
	    fprintf(fp, "    ... Was notified from:\n");

	rec = rec->getPrevious();
    } while (rec != NULL);

    fprintf(fp, "\tfirstAtNode = %#x, lastField = %#x\n",
	    firstAtNode, lastField);
}
