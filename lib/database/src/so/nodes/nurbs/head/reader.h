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

#ifndef __glureader_h_
#define __glureader_h_

#include "../SoAddPrefix.h"
/*
 * reader.h - $Revision: 1.1 $
 */

#include "bufpool.h"
#include "types.h"

enum Curvetype { ct_nurbscurve, ct_pwlcurve, ct_none };
    
struct Property;
struct O_surface;
struct O_nurbssurface;
struct O_trim;
struct O_pwlcurve;
struct O_nurbscurve;
struct O_curve;
class  Quilt;
class TrimVertex;


struct O_curve : public PooledObj {
    union {
        O_nurbscurve	*o_nurbscurve;
        O_pwlcurve	*o_pwlcurve;
    } curve;
    Curvetype		curvetype;	/* arc type: pwl or nurbs	*/
    O_curve *		next;		/* next arc in loop		*/
    O_surface *		owner;		/* owning surface		*/
    int			used;		/* curve called in cur surf	*/
    int			save;		/* 1 if in display list		*/
    long		nuid;
    			O_curve() { next = 0; used = 0; owner = 0; 
				    curve.o_pwlcurve = 0; }
    };

struct O_nurbscurve : public PooledObj {
    Quilt		*bezier_curves;	/* array of bezier curves	*/
    long		type;		/* range descriptor		*/
    REAL		tesselation;	/* tesselation tolerance 	*/
    int			method;		/* tesselation method 		*/
    O_nurbscurve *	next;		/* next curve in list		*/
    int			used;		/* curve called in cur surf	*/
    int			save;		/* 1 if in display list		*/
    O_curve *		owner;		/* owning curve 		*/
			O_nurbscurve( long _type ) 
			   { type = _type; owner = 0; next = 0; used = 0; }
    };
 
class O_pwlcurve : public PooledObj {
public:
    TrimVertex		*pts;		/* array of trim vertices	*/
    int			npts;		/* number of trim vertices	*/
    O_pwlcurve *	next;		/* next curve in list		*/
    int			used;		/* curve called in cur surf	*/
    int			save;		/* 1 if in display list		*/
    O_curve *		owner;		/* owning curve 		*/
			O_pwlcurve( long, long, INREAL *, long, TrimVertex * );
    };

struct O_trim : public PooledObj {
    O_curve		*o_curve;	/* closed trim loop	 	*/
    O_trim *		next;		/* next loop along trim 	*/
    int			save;		/* 1 if in display list		*/
			O_trim() { next = 0; o_curve = 0; }
    };

struct O_nurbssurface : public PooledObj {
    Quilt *		bezier_patches;/* array of bezier patches	*/
    long		type;		/* range descriptor		*/
    O_surface *		owner;		/* owning surface		*/
    O_nurbssurface *	next;		/* next surface in chain	*/
    int			save;		/* 1 if in display list		*/
    int			used;		/* 1 if prev called in block	*/
			O_nurbssurface( long _type ) 
			   { type = _type; owner = 0; next = 0; used = 0; }
    };

struct O_surface : public PooledObj {
    O_nurbssurface *	o_nurbssurface;	/* linked list of surfaces	*/
    O_trim *		o_trim;		/* list of trim loops		*/
    int			save;		/* 1 if in display list		*/
    long		nuid;
			O_surface() { o_trim = 0; o_nurbssurface = 0; }
    };

struct Property : public PooledObj {
    long		type;
    long		tag;
    REAL		value;
    int			save;		/* 1 if in display list		*/
			Property( long _type, long _tag, INREAL _value )
			{ type = _type; tag = _tag; value = (REAL) _value; }
			Property( long _tag, INREAL _value )
			{ type = 0; tag = _tag; value = (REAL) _value; }
    };

class NurbsTessellator;
#endif /* __glureader_h_ */
