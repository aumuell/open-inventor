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
 |      Bunches of nurbs code
 |
 |   Author(s)          : Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include "../SoAddPrefix.h"
#include "arc.c++"
#include "arcsorter.c++"
#include "arctess.c++"
#include "backend.c++"
#include "basiccrveval.c++"
#include "basicsurfeval.c++"
#include "bin.c++"
#include "bufpool.c++"
#include "cachingeval.c++"
#include "ccw.c++"
#include "coveandtiler.c++"
#include "curve.c++"
#include "curvelist.c++"
#include "curvesub.c++"
#include "displaylist.c++"
#include "flist.c++"
#include "flistsorter.c++"
#include "hull.c++"
#include "intersect.c++"
#include "knotvector.c++"
#include "mapdesc.c++"
#include "mapdescv.c++"
#include "maplist.c++"
#include "mesher.c++"
#include "monotonizer.c++"
#include "mycode.c++"
#include "nurbsinterfac.c++"
#include "nurbstess.c++"
#include "patch.c++"
#include "patchlist.c++"
#include "quilt.c++"
#include "reader.c++"
#include "renderhints.c++"
#include "slicer.c++"
#include "sorter.c++"
#include "splitarcs.c++"
#include "subdivider.c++"
#include "tobezier.c++"
#include "trimline.c++"
#include "trimregion.c++"
#include "trimvertpool.c++"
#include "uarray.c++"
#include "varray.c++"
