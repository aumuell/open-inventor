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
 |	This file contains X cursor bitmaps definitions (used by viewers
 |  right now). Those bitmaps were creadted using the 'bitmap' editor.
 |
 |   Author(s): Alain Dumesny
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_XT_CURSOR_
#define  _SO_XT_CURSOR_

// a generic viewing cursor (arrow with a small `v`)
#define so_xt_viewing_width 16
#define so_xt_viewing_height 16
#define so_xt_viewing_x_hot 0
#define so_xt_viewing_y_hot 0
extern char so_xt_viewing_bits[];

// a small curved hand (rolling a ball)
#define so_xt_curved_hand_width 24
#define so_xt_curved_hand_height 24
#define so_xt_curved_hand_x_hot 4
#define so_xt_curved_hand_y_hot 2
extern char so_xt_curved_hand_bits[];

// a small flat hand (translating in a plane)
#define so_xt_flat_hand_width 24
#define so_xt_flat_hand_height 28
#define so_xt_flat_hand_x_hot 13
#define so_xt_flat_hand_y_hot 4
extern char so_xt_flat_hand_bits[];

// a small pointing hand (dolling in/out)
#define so_xt_pointing_hand_width 24
#define so_xt_pointing_hand_height 24
#define so_xt_pointing_hand_x_hot 9
#define so_xt_pointing_hand_y_hot 4
extern char so_xt_pointing_hand_bits[];

// a circle with two arrows showing a roll
#define so_xt_roll_width 17
#define so_xt_roll_height 23
#define so_xt_roll_x_hot 8
#define so_xt_roll_y_hot 11
extern char so_xt_roll_bits[];

// circle with cross and center point (seek)
#define so_xt_target_width 21
#define so_xt_target_height 21
#define so_xt_target_x_hot 10
#define so_xt_target_y_hot 10
extern char so_xt_target_bits[];

// small polygon with it's normal (set up direction)
#define so_xt_normal_vec_width 24
#define so_xt_normal_vec_height 24
#define so_xt_normal_vec_x_hot 12
#define so_xt_normal_vec_y_hot 18
extern char so_xt_normal_vec_bits[];

//
// Walk viewer cursors
//

#define so_xt_walk_width 32
#define so_xt_walk_height 24
#define so_xt_walk_x_hot 15
#define so_xt_walk_y_hot 3
extern char so_xt_walk_bits[];
extern char so_xt_walk_mask_bits[];

#define so_xt_pan_width 20
#define so_xt_pan_height 24
#define so_xt_pan_x_hot 9
#define so_xt_pan_y_hot 1
extern char so_xt_pan_bits[];

#define so_xt_tilt_width 32
#define so_xt_tilt_height 32
#define so_xt_tilt_x_hot 7
#define so_xt_tilt_y_hot 10
extern char so_xt_tilt_bits[];
extern char so_xt_tilt_mask_bits[];


#endif /* _SO_XT_CURSOR_ */
