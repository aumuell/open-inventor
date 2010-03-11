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
 * renderhints.c++ - $Revision: 1.1.1.1 $
 * 	Derrick Burns - 1991
 */

#include "glimports.h"
#include "mystdio.h"
#include "renderhints.h"
#include "defines.h"
#include "nurbsconsts.h"


/*--------------------------------------------------------------------------
 * Renderhints::Renderhints - set all window specific options
 *--------------------------------------------------------------------------
 */
Renderhints::Renderhints()
{
    display_method 	= N_FILL;
    errorchecking 	= N_MSG;
    subdivisions 	= 6.0;
    tmp1 		= 0.0;
}

void
Renderhints::init( void )
{
    maxsubdivisions = (int) subdivisions;
    if( maxsubdivisions < 0 ) maxsubdivisions = 0;


    if( display_method == N_FILL ) {
	wiretris = 0;
	wirequads = 0;
    } else if( display_method == N_OUTLINE_TRI ) {
	wiretris = 1;
	wirequads = 0;
    } else if( display_method == N_OUTLINE_QUAD ) {
	wiretris = 0;
	wirequads = 1;
    } else {
	wiretris = 1;
	wirequads = 1;
    }
}

int
Renderhints::isProperty( long property )
{
    switch ( property ) {
	case N_DISPLAY:
	case N_ERRORCHECKING:
	case N_SUBDIVISIONS:
        case N_TMP1:
	    return 1;
	default:
	    return 0;
    }
}

REAL 
Renderhints::getProperty( long property )
{
    switch ( property ) {
	case N_DISPLAY:
	    return display_method;
	case N_ERRORCHECKING:
	    return errorchecking;
	case N_SUBDIVISIONS:
	    return subdivisions;
        case N_TMP1:
	    return tmp1;
	default:
	    abort();
	    return -1;  //not necessary, needed to shut up compiler
    }
}

void 
Renderhints::setProperty( long property, REAL value )
{
    switch ( property ) {
	case N_DISPLAY:
	    display_method = value;
	    break;
	case N_ERRORCHECKING:
	    errorchecking = value;
	    break;
	case N_SUBDIVISIONS:
	    subdivisions = value;
	    break;
	case N_TMP1: /* unused */
	    tmp1 = value;
	    break;
	default:
	    abort();
	    break;
    }
}
