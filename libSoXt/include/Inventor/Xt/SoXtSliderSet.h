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
* Copyright (C) 1990-93   Silicon Graphics, Inc.
*
_______________________________________________________________________
______________  S I L I C O N   G R A P H I C S   I N C .  ____________
|
|   $Revision: 1.1.1.1 $
|
|   Description:
|      Defines the SoXtSliderSet Component class, a base class for the
|          SoXtTransformSliderSet
|          SoXtMaterialSliderSet
|      and SoXtLightSliderSet
|
|   Author(s)          : Paul Isaacs
|
______________  S I L I C O N   G R A P H I C S   I N C .  ____________
_______________________________________________________________________
*/


#ifndef _SO_XT_SLIDER_SET_
#define _SO_XT_SLIDER_SET_

#include <Inventor/Xt/SoXtSliderSetBase.h>

#define SLIDER_SET_DEFAULT_BORDER_WIDTH 3

///////////////////////////////////////////////////////////////////////
//
// class: SoXtSliderSet
//
///////////////////////////////////////////////////////////////////////

// C-api: abstract
// C-api: prefix=SoXtSldrs
class SoXtSliderSet : public SoXtSliderSetBase {
  public:
  // internal:
      void updateLayout();        // Called after button presses that may
				  // cause the size or layout of the SoXtSliderSet 
				  // to be altered.
				  
  protected:
    SoXtSliderSet(
	Widget parent,
	const char *name, 
	SbBool buildInsideParent,
	SoNode *newEditNode);
      ~SoXtSliderSet();
      Widget _parentShellWidget;  // since this class can resize itself when
				  // buttons are pressed, it needs to know
				  // about its parent shell, so that it
				  // query and change the size.
};

#endif // _SO_XT_SLIDER_SET_
