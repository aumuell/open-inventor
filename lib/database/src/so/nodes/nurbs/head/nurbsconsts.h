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

#ifndef __glunurbsconsts_h_
#define __glunurbsconsts_h_

#include "../SoAddPrefix.h"
/*
 * nurbsconsts.h - $Revision: 1.1.1.1 $
 */

/* NURBS Properties - one set per map, 
   each takes a single INREAL arg */
#define N_SAMPLING_TOLERANCE  	1
#define N_S_RATE		6
#define N_T_RATE		7
#define N_CLAMPFACTOR		13
#define 	N_NOCLAMPING		0.0
#define N_MINSAVINGS		14
#define 	N_NOSAVINGSSUBDIVISION	0.0

/* NURBS Properties - one set per map, 
   each takes an enumerated value */
#define N_CULLING		2
#define 	N_NOCULLING		0.0
#define 	N_CULLINGON		1.0
#define N_SAMPLINGMETHOD	10
#define 	N_NOSAMPLING		0.0
#define 	N_FIXEDRATE		3.0
#define 	N_DOMAINDISTANCE	2.0
#define 	N_PARAMETRICDISTANCE	5.0
#define 	N_PATHLENGTH		6.0
#define 	N_SURFACEAREA		7.0
#define N_BBOX_SUBDIVIDING	17
#define 	N_NOBBOXSUBDIVISION	0.0
#define 	N_BBOXTIGHT		1.0
#define 	N_BBOXROUND		2.0

/* NURBS Rendering Properties - one set per renderer
   each takes an enumerated value */
#define N_DISPLAY		3
#define 	N_FILL			1.0 	
#define 	N_OUTLINE_POLY		2.0
#define 	N_OUTLINE_TRI		3.0
#define 	N_OUTLINE_QUAD	 	4.0
#define 	N_OUTLINE_PATCH		5.0
#define 	N_OUTLINE_PARAM		6.0
#define 	N_OUTLINE_PARAM_S	7.0
#define 	N_OUTLINE_PARAM_ST 	8.0
#define 	N_OUTLINE_SUBDIV 	9.0
#define 	N_OUTLINE_SUBDIV_S 	10.0
#define 	N_OUTLINE_SUBDIV_ST 	11.0
#define 	N_ISOLINE_S		12.0
#define N_ERRORCHECKING		4
#define 	N_NOMSG			0.0
#define 	N_MSG			1.0

/* GL 4.0 propeties not defined above */
#ifndef N_PIXEL_TOLERANCE
#define N_PIXEL_TOLERANCE	N_SAMPLING_TOLERANCE
#define N_ERROR_TOLERANCE	20
#define N_SUBDIVISIONS		5
#define N_TILES			8
#define N_TMP1			9
#define N_TMP2			N_SAMPLINGMETHOD
#define N_TMP3			11
#define N_TMP4			12
#define N_TMP5			N_CLAMPFACTOR
#define N_TMP6			N_MINSAVINGS
#define N_S_STEPS		N_S_RATE
#define N_T_STEPS		N_T_RATE
#endif

/* NURBS Rendering Properties - one set per map,
   each takes an INREAL matrix argument */
#define N_CULLINGMATRIX		1
#define N_SAMPLINGMATRIX	2
#define N_BBOXMATRIX		3


/* NURBS Rendering Properties - one set per map,
   each takes an INREAL vector argument */
#define	N_BBOXSIZE		4

/* type argument for trimming curves */
#ifndef N_P2D
#define N_P2D 	 		0x8	
#define N_P2DR 	 		0xd
#endif	

#endif /* __glunurbsconsts_h_ */
