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

#include <GL/gl.h>
#include "softcurvemaps.h"

_SoNurbsColor4CurveMap::_SoNurbsColor4CurveMap() : _SoNurbsCurveMap( N_C4D, 0, 4 ) {}
_SoNurbsColor4CurveMap::~_SoNurbsColor4CurveMap() {}
void _SoNurbsColor4CurveMap::domain( float * ) {}
void _SoNurbsColor4CurveMap::point( float *p ) { ::glColor4fv( p ); }
void _SoNurbsColor4CurveMap::uderiv( float * ) {}
void _SoNurbsColor4CurveMap::uuderiv( float * ) {}

_SoNurbsTexture2CurveMap::_SoNurbsTexture2CurveMap() : _SoNurbsCurveMap( N_T2D, 0, 2 ) {}
_SoNurbsTexture2CurveMap::~_SoNurbsTexture2CurveMap() {}
void _SoNurbsTexture2CurveMap::domain( float * ) {}
void _SoNurbsTexture2CurveMap::point( float *p ) { ::glTexCoord2fv( p ); }
void _SoNurbsTexture2CurveMap::uderiv( float * ) {}
void _SoNurbsTexture2CurveMap::uuderiv( float * ) {}

_SoNurbsVertex3CurveMap::_SoNurbsVertex3CurveMap() : _SoNurbsCurveMap( N_V3D, 0, 3 ) {}
_SoNurbsVertex3CurveMap::~_SoNurbsVertex3CurveMap() {}
void _SoNurbsVertex3CurveMap::domain( float * ) {}
void _SoNurbsVertex3CurveMap::point( float *p ) { ::glVertex3fv( p ); }
void _SoNurbsVertex3CurveMap::uderiv( float * ) {}
void _SoNurbsVertex3CurveMap::uuderiv( float * ) {}

_SoNurbsVertex4CurveMap::_SoNurbsVertex4CurveMap() : _SoNurbsCurveMap( N_V3DR, 1, 4 ) {}
_SoNurbsVertex4CurveMap::~_SoNurbsVertex4CurveMap() {}
void _SoNurbsVertex4CurveMap::domain( float * ) {}
void _SoNurbsVertex4CurveMap::point( float *p ) { ::glVertex4fv( p ); }
void _SoNurbsVertex4CurveMap::uderiv( float * ) {}
void _SoNurbsVertex4CurveMap::uuderiv( float * ) {}
