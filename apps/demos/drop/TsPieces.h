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
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Description:
 |	This file describes the pieces used in the drop game.
 |      All pieces are derived from the base class TsPiece.
 |
 |   Classes:
 |      TsEll, TsTee, TsZee, TsLTri, TsMTri, TsRtri
 |
 |   Author: Dave Immel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef _TS_PIECES_
#define _TS_PIECES_

#include "TsPiece.h"

//////////////////////////////////////////////////////////////////////////////
//
//  Class: TsEll
//
//  This piece is an 'L' shape composed of four blocks, three in a row, and
//  the fourth to one side at the end.
//
//                                     ***
//                                       *
//
//////////////////////////////////////////////////////////////////////////////

class TsEll : public TsPiece {
public:
    TsEll();
   ~TsEll();
};

//////////////////////////////////////////////////////////////////////////////
//
//  Class: TsTee
//
//  This piece is an 'T' shape composed of four blocks, three in a row, and
//  the fourth to one side in the middle of the other three.
//
//                                     ***
//                                      *
//
//////////////////////////////////////////////////////////////////////////////

class TsTee : public TsPiece {
public:
    TsTee();
   ~TsTee();
};

//////////////////////////////////////////////////////////////////////////////
//
//  Class: TsZee
//
//  This piece is an 'Z' shape composed of four blocks, two in a row, and
//  two more to one side and shifted over one space.
//
//                                     **
//                                      **
//
//////////////////////////////////////////////////////////////////////////////

class TsZee : public TsPiece {
public:
    TsZee();
   ~TsZee();
};

//////////////////////////////////////////////////////////////////////////////
//
//  Class: TsLTri
//
//  This piece is a triangle shape with an extra block over the left
//  block of the triangle.
//
//////////////////////////////////////////////////////////////////////////////

class TsLTri : public TsPiece {
public:
    TsLTri();
   ~TsLTri();
};

//////////////////////////////////////////////////////////////////////////////
//
//  Class: TsMTri
//
//  This piece is a triangle shape with an extra block over the middle
//  block of the triangle.
//
//////////////////////////////////////////////////////////////////////////////

class TsMTri : public TsPiece {
public:
    TsMTri();
   ~TsMTri();
};

//////////////////////////////////////////////////////////////////////////////
//
//  Class: TsRTri
//
//  This piece is a triangle shape with an extra block over the right
//  block of the triangle.
//
//////////////////////////////////////////////////////////////////////////////

class TsRTri : public TsPiece {
public:
    TsRTri();
   ~TsRTri();
};

#endif /* _TS_PIECES_ */


