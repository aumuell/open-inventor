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
 * gl4surfeval.h
 *
 * $Revision: 1.1.1.1 $
 */

#ifndef SO_NURBS_GL4SURFEVAL
#define SO_NURBS_GL4SURFEVAL

#include <inttypes.h>   /* defines long and brethren */

#include "basicsurfeval.h"

class _SoNurbsGL4SurfaceEval : public _SoNurbsBasicSurfaceEvaluator
{
public:
    void		range2f( long, REAL *, REAL * );
    void		domain2f( REAL, REAL, REAL, REAL );

    void		enable( long );
    void		disable( long );
    void		bgnmap2f( long );
    void		map2f( long, REAL, REAL, long, long,
				     REAL, REAL, long, long,
			       REAL * );
    void		mapgrid2f( long, REAL, REAL, long, REAL, REAL );
    void		mapmesh2f( long, long, long, long, long );
    void		evalcoord2f( long, REAL, REAL );
    void		evalpoint2i( long, long );
    void		endmap2f( void );

    void 		bgnline( void );
    void 		endline( void );
    void 		bgnclosedline( void );
    void 		endclosedline( void );
    void 		bgntmesh( void );
    void 		swaptmesh( void );
    void 		endtmesh( void );
    void 		bgnqstrip( void );
    void 		endqstrip( void );

    int			canRecord( void ) ;
    int			canPlayAndRecord( void ) ;
    int			createHandle( int handle );
    void		beginOutput( ServiceMode, int handle );
    void		endOutput( void ); 
    void		discardRecording( int handle );
    void		playRecording( int handle );

private:
    enum primitive { tmesh, qstrip, closedline, openline, none };
    primitive		curtype;
    int			numverts;
    float		psave[2];
    long		gsave[2];
    int			grid;
};

#endif /* SO_NURBS_GL4SURFEVAL */
