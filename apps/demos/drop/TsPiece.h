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
 |   $Source: /oss/CVS/cvs/inventor/apps/demos/drop/TsPiece.h,v $
 |   $Revision: 1.1.1.1 $
 |   $Date: 2000/08/15 12:55:54 $
 |
 |   Description:
 |	This class describes a base class for constructing pieces used in
 |      the drop demo program.  Pieces are composed of blocks arranged
 |      in some order which defines the particular piece.  The pieces are
 |      stored as indices into a three dimensional array (the playing field)
 |      for each block that is part of the piece.  Operations on the piece
 |      move it to different locations and orientations on the playing field
 |      by modifying the indices into the playing field.
 |
 |   Classes:
 |      TsPiece
 |
 |   Author: Dave Immel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef _TS_PIECE_
#define _TS_PIECE_

//////////////////////////////////////////////////////////////////////////////
//
//  Class: TsPiece
//
//  The base class for the drop pieces.  Methods transform the piece and
//  store its position and orientation.
//
//////////////////////////////////////////////////////////////////////////////

class TsPiece {
public:
    TsPiece();
   ~TsPiece();

    // Sets the orientation of the piece
    void  	        setOrientation( int orient ) {orientation = orient;};

    // Get the positions of the blocks of the piece in its current
    // orientation.  Return the number of blocks in the piece.
    int 	        getPosition( int pos[] );

    // Get the positions of the blocks of the piece given an orientation.
    // Return the number of blocks in the piece.
    int                 getPosition( int orientation, int pos[] );

protected:
    int                 orientation;
    int                 numBlocks;
    int                 piecePositionTable[24][4];

};

#endif /* _TS_PIECE_ */




