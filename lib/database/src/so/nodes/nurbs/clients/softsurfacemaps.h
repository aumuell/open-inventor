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

#ifndef SO_NURBS_SOFTSURFACEMAPS
#define SO_NURBS_SOFTSURFACEMAPS

#include <inttypes.h>   /* defines long and brethren */

#include <GL/gl.h>
#include "softsurfmap.h"
#include "gl4types.h"

class _SoNurbsSurfPartial {
public:
	float	p[4];
	float	du[4];
	float	dv[4];
	float	norm[3];
        float   uv[4];
};

class _SoNurbsColor4SurfaceMap : public _SoNurbsSurfaceMap
{
public:
			_SoNurbsColor4SurfaceMap() : _SoNurbsSurfaceMap( N_C4D, 0, 4 )
				{isUsed = 0; ptIndex = 0;}
			~_SoNurbsColor4SurfaceMap();
    virtual void	domain( float * );
    virtual void	point( float *p );
    virtual void	uderiv( float * );
    virtual void	vderiv( float * );
    virtual void	uuderiv( float * );
    virtual void	uvderiv( float * );
    virtual void	vvderiv( float * );

    virtual void	startNewTriStrip();
    virtual void	swapTriStripVertices();

protected:
    char           	isUsed;
    float		cachePt[2][4];
    int			ptIndex;
    int			swapCalled;
};

class _SoNurbsTexture2SurfaceMap : public _SoNurbsSurfaceMap
{
public:
			_SoNurbsTexture2SurfaceMap() : _SoNurbsSurfaceMap( N_T2D, 0, 2 ) {}
			~_SoNurbsTexture2SurfaceMap();
    virtual void	domain( float * );
    virtual void	point( float *p );
    virtual void	uderiv( float * );
    virtual void	vderiv( float * );
    virtual void	uuderiv( float * );
    virtual void	uvderiv( float * );
    virtual void	vvderiv( float * );
};

class _SoNurbsVertex3SurfaceMap : public _SoNurbsSurfaceMap
{

public:
			_SoNurbsVertex3SurfaceMap() : _SoNurbsSurfaceMap( N_V3D, 0, 3 )
				{isUsed = 0; ptIndex = 0;}
			~_SoNurbsVertex3SurfaceMap();
    virtual void	domain( float *v );
    virtual void	point( float *p );
    virtual void	uderiv( float * );
    virtual void	vderiv( float * );
    virtual void	uuderiv( float * );
    virtual void	uvderiv( float * );
    virtual void	vvderiv( float * );

    virtual void	startNewTriStrip();
    virtual void	swapTriStripVertices();

protected:
    void		computeNormal( void );

    _SoNurbsSurfPartial       P;
    char           	isUsed;
    _SoNurbsSurfPartial	cachePt[2];
    int			ptIndex;
    int			swapCalled;
};

class _SoNurbsVertex4SurfaceMap : public _SoNurbsSurfaceMap
{
public:
			_SoNurbsVertex4SurfaceMap() : _SoNurbsSurfaceMap( N_V3DR, 1, 4 )
				{isUsed = 0; ptIndex = 0;}
			~_SoNurbsVertex4SurfaceMap();
    virtual void	domain( float * );
    virtual void	point( float *p );
    virtual void	uderiv( float * );
    virtual void	vderiv( float * );
    virtual void	uuderiv( float * );
    virtual void	uvderiv( float * );
    virtual void	vvderiv( float * );

    virtual void	startNewTriStrip();
    virtual void	swapTriStripVertices();

protected:
    void		computeFirstPartials( void );
    void		computeNormal( void );

    _SoNurbsSurfPartial       P;
    char           	isUsed;
    _SoNurbsSurfPartial	cachePt[2];
    int			ptIndex;
    int			swapCalled;
};

#endif /* SO_NURBS_SOFTSURFACEMAPS */



