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

/*-------------------------------------------------------------
 *  This is an example from the Inventor Toolmaker,
 *  chapter 8, example 16
 *
 *  Header file for "RotTransManip"
 *
 *  This is the include file for the RotTransManip.
 *  The RotTransManip is an SoTransformManip that utilizes a  
 *  RotTransDragger to do the interactive part of its job.
 *
 *----------------------------------------------------------*/

#ifndef  _ROT_TRANS_MANIP_
#define  _ROT_TRANS_MANIP_

#include <Inventor/manips/SoTransformManip.h>

class RotTransManip : public SoTransformManip
{
   SO_NODE_HEADER(RotTransManip);

  public:
   // Constructor
   RotTransManip();

   // Initialize the class. This should be called once
   // after SoInteraction::init(),
   // TranslateRadialDragger::init().
   // and RotTransDragger::init().
   static void initClass();

  private:

   // Destructor
   ~RotTransManip();
};    
#endif  /* _ROT_TRANS_MANIP_ */
