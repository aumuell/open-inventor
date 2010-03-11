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
 |   Author(s)	: Alain Dumesny
 |
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include "_SoXtUIRegion.h"
#include <GL/gl.h>

// ??? doing a GL_LINE_LOOP seems to be missing the top right
// ??? pixel due to subpixel == TRUE in openGL.
#define RECT(x1, y1, x2, y2) \
	glBegin(GL_LINE_STRIP); \
	glVertex2s(x2, y2); glVertex2s(x1, y2); \
	glVertex2s(x1, y1); glVertex2s(x2, y1); \
	glVertex2s(x2, y2+1); \
	glEnd();

void SoDrawDownUIBorders(short x1, short y1, short x2, short y2, SbBool blackLast)
{
    DARK1_UI_COLOR;
    glBegin(GL_LINE_STRIP);
    glVertex2s(x1, y1+1); glVertex2s(x1, y2); glVertex2s(x2+1, y2);
    glEnd();
    LIGHT1_UI_COLOR;
    glBegin(GL_LINE_STRIP);
    glVertex2s(x1, y1); glVertex2s(x2, y1); glVertex2s(x2, y2);
    glEnd();
    
    x1++; y1++; x2--; y2--;
    DARK2_UI_COLOR;
    glBegin(GL_LINE_STRIP);
    glVertex2s(x1, y1+1); glVertex2s(x1, y2); glVertex2s(x2+1, y2);
    glEnd();
    WHITE_UI_COLOR;
    glBegin(GL_LINE_STRIP);
    glVertex2s(x1, y1); glVertex2s(x2, y1); glVertex2s(x2, y2);
    glEnd();
    
    x1++; y1++; x2--; y2--;
    if (blackLast) {
	BLACK_UI_COLOR;
	RECT(x1, y1, x2, y2);
    }
    else {
	DARK3_UI_COLOR;
	glBegin(GL_LINE_STRIP);
	glVertex2s(x1, y1+1); glVertex2s(x1, y2); glVertex2s(x2+1, y2);
	glEnd();
	DARK2_UI_COLOR;
	glBegin(GL_LINE_STRIP);
	glVertex2s(x1, y1); glVertex2s(x2, y1); glVertex2s(x2, y2);
	glEnd();
    }
}


void SoDrawDownUIRegion(short x1, short y1, short x2, short y2)
{
    SoDrawDownUIBorders(x1, y1, x2, y2);
    
    MAIN_UI_COLOR;
    x1+=UI_THICK; y1+=UI_THICK; x2-=UI_THICK; y2-=UI_THICK;
    RECT(x1, y1, x2, y2);
    x1++; y1++; x2--; y2--;
    RECT(x1, y1, x2, y2);
    x1++; y1++; x2--; y2--;
    
    SoDrawDownUIBorders(x1, y1, x2, y2, TRUE);
}


void SoDrawThumbUIRegion(short x1, short y1, short x2, short y2)
{
    short v[3][2];
    short x = (x1 + x2) / 2;
    
    v[0][0] = x1;
    v[0][1] = v[2][1] = y1;
    v[1][0] = x;
    v[1][1] = y2;
    v[2][0] = x2;
    
    MAIN_UI_COLOR;
    glBegin(GL_POLYGON);
    glVertex2sv(v[0]);
    glVertex2sv(v[1]);
    glVertex2sv(v[2]);
    glEnd();
    
    glBegin(GL_LINES);
    
    BLACK_UI_COLOR;
    glVertex2s(x1, y1); glVertex2s(x, y2);
    DARK3_UI_COLOR;
    glVertex2s(x, y2); glVertex2s(x2, y1);
    glVertex2s(x2, y1); glVertex2s(x1+1, y1);
    
    x1++; y1++; x2--; y2--;
    WHITE_UI_COLOR;
    glVertex2s(x1, y1); glVertex2s(x, y2);
    DARK2_UI_COLOR;
    glVertex2s(x, y2); glVertex2s(x2, y1);
    DARK1_UI_COLOR; 
    glVertex2s(x2, y1); glVertex2s(x1+1, y1);
    
    x1++; y1++; x2--; y2--;
    WHITE_UI_COLOR;
    glVertex2s(x1, y1); glVertex2s(x, y2);
    DARK2_UI_COLOR;
    glVertex2s(x, y2); glVertex2s(x2, y1);
    
    glEnd();
}

