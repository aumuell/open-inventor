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
 * nurbserror.c++ - $Revision: 1.1.1.1 $
 * 	Derrick Burns - 1992
 */

#include "myglimports.h"
#include "nurbstess.h"

const char *Tessellator::nurbserror[] = {
/* 0 */	" ",				
	"spline order un-supported",
	"too few knots",
	"valid knot range is empty",
	"decreasing knot sequence knot",
/* 5 */	"knot multiplicity greater than order of spline",
	"endcurve() must follow bgncurve()", 
	"bgncurve() must precede endcurve()", 
	"missing or extra geometric data", 
	"can't draw pwlcurves", 
/*10 */	"missing or extra domain data", 
	"missing or extra domain data", 
	"endtrim() must precede endsurface()", 
	"bgnsurface() must precede endsurface()", 
	"curve of improper type passed as trim curve", 
/*15 */	"bgnsurface() must precede bgntrim()", 
	"endtrim() must follow bgntrim()", 
	"bgntrim() must precede endtrim()", 
	"invalid or missing trim curve", 
	"bgntrim() must precede pwlcurve()", 
/*20 */	"pwlcurve referenced twice", 
	"pwlcurve and nurbscurve mixed", 
	"improper usage of trim data type", 
	"nurbscurve referenced twice", 
	"nurbscurve and pwlcurve mixed", 
/*25 */	"nurbssurface referenced twice", 
	"invalid property",
	"endsurface() must follow bgnsurface()",
        "intersecting or misoriented trim curves",
	"intersecting trim curves",
/*30 */ "UNUSED",
	"unconnected trim curves", 
	"unknown knot error",
	"negative vertex count encountered", 
	"negative byte-stride encounteed",
/*35 */ "unknown type descriptor",
	"null control point reference",
	"duplicate point on pwlcurve"
};

