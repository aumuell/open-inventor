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
|      Defines the SoXtSliderSetBase class
|      This is the base class for the SoXtSliderSet and for the
|      smaller components of which it is composed. (Such as the compound
|      sliderTool and the combined groups of sliderTools that make up 
|      each subPanel of an editor
|      
|
|   Author(s)          : Paul Isaacs
|
______________  S I L I C O N   G R A P H I C S   I N C .  ____________
_______________________________________________________________________
*/


#ifndef _SO_XT_SLIDER_SET_BASE_
#define _SO_XT_SLIDER_SET_BASE_

#include <Inventor/Xt/SoXtComponent.h>
#include <Inventor/nodes/SoNode.h>

///////////////////////////////////////////////////////////////////////
//
// class: SoXtSliderSetBase
//
///////////////////////////////////////////////////////////////////////

// C-api: abstract
// C-api: name=SoXtSldrsBase
class SoXtSliderSetBase : public SoXtComponent {
  public:
    // C-api: expose
      virtual void setNode( SoNode *newNode );

      SoNode * getNode() const { return _editNode; }
  // internal:
      virtual void getLayoutSize( int &w, int &h );

      virtual void show();
      
  protected:
      Widget	      widget;//??? huh? why not SoXtComponent::widget?

      int             _layoutWidth;
      int             _layoutHeight;
      SoNode          *_editNode;
      int             _numSubComponents;
      SoXtSliderSetBase **_subComponentArray;
      SoXtSliderSetBase(
	Widget parent = NULL,
	const char *name = NULL, 
	SbBool buildInsideParent = TRUE,
	SoNode *newEditNode = NULL);
      ~SoXtSliderSetBase();
};

#endif // _SO_XT_SLIDER_SET_BASE_
