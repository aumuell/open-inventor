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

/*--------------------------------------------------------------
 *  This is an example from the Inventor Toolmaker,
 *  chapter 7, examples 4 through 8.
 *
 *  Header file for "JumpingJackKit"
 *------------------------------------------------------------*/

#include <Inventor/nodekits/SoBaseKit.h>

//  Diagram of this kit's structure:
//                                   "this"
//        |----------------------------|
//     "callbackList"                  |
//                                     |
//      -------------------------------------------------
//      |       |           |       |         |          |
//  "body"      |      "leftArm"    |      "leftLeg"     |
//            "head"             "rightArm"          "rightLeg"

class SoEventCallback;

class JumpingJackKit : public SoBaseKit {

   SO_KIT_HEADER(JumpingJackKit);

   SO_KIT_CATALOG_ENTRY_HEADER(body);
   SO_KIT_CATALOG_ENTRY_HEADER(head);
   SO_KIT_CATALOG_ENTRY_HEADER(leftArm);
   SO_KIT_CATALOG_ENTRY_HEADER(rightArm);
   SO_KIT_CATALOG_ENTRY_HEADER(leftLeg);
   SO_KIT_CATALOG_ENTRY_HEADER(rightLeg);

  public:
      JumpingJackKit();

      // Overrides default method. All the parts are shapeKits,
      // so this node will not affect the state.
      virtual SbBool affectsState() const;

  SoINTERNAL public:
      static void initClass();

  private:

   // Constructor calls to build and set up parts.
   void createInitialJack();

   // An SoEventCallback will be inserted into the 
   // "callbackList" (inherited from SoBaseKit) as the part 
   // "callbackList[0]". This routine jumpJackJump() will be 
   // set as the callback function for that part. It is this 
   // routine which changes the angles in the joints.
   static void jumpJackJump(void *userData, 
      SoEventCallback *eventCB);

   virtual ~JumpingJackKit();
};
