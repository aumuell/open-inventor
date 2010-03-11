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

//  -*- C++ -*-

/*
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Description:
 |	This file contains convenience routines which draws borders, and 
 |    elevated 3D looking regions. Those 3D looking UI regions are primarly
 |    used in SoSliders, SoColorPatch, SoColorWheel, ...
 |
 |   Author(s)	: Alain Dumesny
 |
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_UI_REGION_
#define  _SO_UI_REGION_

#include <Inventor/SbBasic.h>

/*
 * Defines
 */

//
// list of grey colors used when drawing regions
//
#define WHITE_UI_COLOR	    glColor3ub(255,255,255)
#define BLACK_UI_COLOR	    glColor3ub(0,0,0)
#define MAIN_UI_COLOR	    glColor3ub(170,170,170)
#define DARK1_UI_COLOR	    glColor3ub(128,128,128)
#define DARK2_UI_COLOR	    glColor3ub(85,85,85)
#define DARK3_UI_COLOR	    glColor3ub(50,50,50)
#define LIGHT1_UI_COLOR	    glColor3ub(215,215,215)

#define UI_THICK 3


#define SO_UI_REGION_GREY1	glColor3ub(240, 240, 240)
#define SO_UI_REGION_GREY2	glColor3ub(190, 190, 190)
#define SO_UI_REGION_GREY3	glColor3ub(150, 150, 150)
#define SO_UI_REGION_GREY4	glColor3ub(130, 130, 130)
#define SO_UI_REGION_GREY5	glColor3ub(110, 110, 110)
#define SO_UI_REGION_GREY6	glColor3ub(70, 70, 70)
#define SO_UI_REGION_GREY7	glColor3ub(30, 30, 30)


/*
 * Function prototypes
 */
 
extern void
SoDrawDownUIRegion(short x1, short y1, short x2, short y2);

extern void
SoDrawDownUIBorders(short x1, short y1, short x2, short y2, SbBool blackLast=FALSE);

extern void
SoDrawThumbUIRegion(short x1, short y1, short x2, short y2);

#endif  // _SO_UI_REGION_
