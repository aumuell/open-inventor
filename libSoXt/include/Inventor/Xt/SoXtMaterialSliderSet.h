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
|      Defines SoXtMaterialSliderSet, a subclass of SoXtSliderSet
|      The SoXtMaterialSliderSet is used to manipulate the fields
|      of a SoXtMaterial node using an arrangement of motif sliders and 
|      buttons.
|
|   Author(s)          : Paul Isaacs
|
______________  S I L I C O N   G R A P H I C S   I N C .  ____________
_______________________________________________________________________
*/



#ifndef _SO_XT_MATERIAL_SLIDER_SET_
#define _SO_XT_MATERIAL_SLIDER_SET_

#include <Inventor/Xt/SoXtSliderSet.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoXtMaterialSliderSet
//
//  For editing material nodes.
//
//  The SoXtMaterialSliderSet allocates:
//     1 SoXtAmbientColorSliderModule     (for editing ambient color )
//     1 SoXtDiffuseColorSliderModule     (for editing diffues color )
//     1 SoXtSpecularColorSliderModule    (for editing specular color )
//     1 SoXtEmissiveColorSliderModule    (for editing emissive color )
//     1 SoXtShininessSliderModule        (for editing shininess )
//     1 SoXtTransparencySliderModule     (for editing transparency )
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoXtMtlSldrs
class SoXtMaterialSliderSet : public SoXtSliderSet {
  public:
    SoXtMaterialSliderSet(
	Widget parent = NULL,
	const char *name = NULL, 
	SbBool buildInsideParent = TRUE,
	SoNode *newEditNode = NULL );
    ~SoXtMaterialSliderSet();
    
  protected:

    // This constructor takes a boolean whether to build the widget now.
    // Subclasses can pass FALSE, then call the buildWidget() method on
    // this class when they are ready for it to be built.
    SoEXTENDER
    SoXtMaterialSliderSet(
	Widget parent,
	const char *name, 
	SbBool buildInsideParent, 
	SoNode *newEditNode,
	SbBool buildNow);
    
    Widget  	buildWidget( Widget parent);
  
    // this is called by both constructors
    void constructorCommon(SbBool buildNow);
};

#endif /* _SO_XT_MATERIAL_SLIDER_SET_ */
