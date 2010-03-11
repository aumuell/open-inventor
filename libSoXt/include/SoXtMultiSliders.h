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
|      Defines the various subclasses of multiple slider components
|
|   Author(s)          : Paul Isaacs
|
______________  S I L I C O N   G R A P H I C S   I N C .  ____________
_______________________________________________________________________
*/


#ifndef SO_XT_MULTI_SLIDERS_
#define SO_XT_MULTI_SLIDERS_

#include "SoXtMultiSlider.h"

#define SO_TRANSLATE_SLIDER_MINIMUM -10000
#define SO_TRANSLATE_SLIDER_MAXIMUM  10000
#define SO_SCALE_SLIDER_MINIMUM          0
#define SO_SCALE_SLIDER_MAXIMUM       1000
#define SO_ROTATE_SLIDER_MINIMUM    -36000
#define SO_ROTATE_SLIDER_MAXIMUM     36000
#define SO_UNIT_SLIDER_MINIMUM           0
#define SO_UNIT_SLIDER_MAXIMUM         100
#define SO_FOV_SLIDER_MINIMUM           0
#define SO_FOV_SLIDER_MAXIMUM        18000

class SoXtTransMultiSlider : public SoXtMultiSlider {
  public:
    SoXtTransMultiSlider(
	Widget parent = NULL,
	const char *name = NULL, 
	SbBool buildInsideParent = TRUE);
    ~SoXtTransMultiSlider();
    virtual void    exportValuesToInventor();
    virtual void    importValuesFromInventor();

  protected:

    virtual SbBool  validNodeType(SoNode *node);
};

class SoXtScaleMultiSlider : public SoXtMultiSlider {
  public:
    SoXtScaleMultiSlider(
	Widget parent = NULL,
	const char *name = NULL, 
	SbBool buildInsideParent = TRUE);
    ~SoXtScaleMultiSlider();
    virtual void    exportValuesToInventor();
    virtual void    importValuesFromInventor();
  protected:
    virtual SbBool  validNodeType(SoNode *node);
};

class SoXtRotateMultiSlider : public SoXtMultiSlider
{
  public:
    SoXtRotateMultiSlider(
	Widget parent = NULL,
	const char *name = NULL, 
	SbBool buildInsideParent = TRUE);
    ~SoXtRotateMultiSlider();
    virtual void    exportValuesToInventor();
    virtual void    importValuesFromInventor();
  protected:
    virtual SbBool  validNodeType(SoNode *node);
};

class SoXtScaleOrientationMultiSlider : public SoXtMultiSlider {
  public:
    SoXtScaleOrientationMultiSlider(
	Widget parent = NULL,
	const char *name = NULL, 
	SbBool buildInsideParent = TRUE);
    ~SoXtScaleOrientationMultiSlider();
    virtual void    exportValuesToInventor();
    virtual void    importValuesFromInventor();
  protected:
    virtual SbBool  validNodeType(SoNode *node);
};

// SoXtCenterMultiSlider - for transform center
class SoXtCenterMultiSlider : public SoXtMultiSlider {
  public:
    SoXtCenterMultiSlider(
	Widget parent = NULL,
	const char *name = NULL, 
	SbBool buildInsideParent = TRUE);
    ~SoXtCenterMultiSlider();
    virtual void    exportValuesToInventor();
    virtual void    importValuesFromInventor();
  protected:
    virtual SbBool  validNodeType(SoNode *node);
};

class SoXtAmbientColorMultiSlider : public SoXtMultiSlider {
  public:
    SoXtAmbientColorMultiSlider(
	Widget parent = NULL,
	const char *name = NULL, 
	SbBool buildInsideParent = TRUE);
    ~SoXtAmbientColorMultiSlider();
    virtual void    exportValuesToInventor();
    virtual void    importValuesFromInventor();
  protected:
    virtual SbBool  validNodeType(SoNode *node);
};

class SoXtDiffuseColorMultiSlider : public SoXtMultiSlider {
  public:
    SoXtDiffuseColorMultiSlider(
	Widget parent = NULL,
	const char *name = NULL, 
	SbBool buildInsideParent = TRUE);
    ~SoXtDiffuseColorMultiSlider();
    virtual void    exportValuesToInventor();
    virtual void    importValuesFromInventor();
  protected:
    virtual SbBool  validNodeType(SoNode *node);
};

class SoXtSpecularColorMultiSlider : public SoXtMultiSlider {
  public:
    SoXtSpecularColorMultiSlider(
	Widget parent = NULL,
	const char *name = NULL, 
	SbBool buildInsideParent = TRUE);
    ~SoXtSpecularColorMultiSlider();
    virtual void    exportValuesToInventor();
    virtual void    importValuesFromInventor();
  protected:
    virtual SbBool  validNodeType(SoNode *node);
};

class SoXtEmissiveColorMultiSlider : public SoXtMultiSlider {
  public:
    SoXtEmissiveColorMultiSlider(
	Widget parent = NULL,
	const char *name = NULL, 
	SbBool buildInsideParent = TRUE);
    ~SoXtEmissiveColorMultiSlider();
    virtual void    exportValuesToInventor();
    virtual void    importValuesFromInventor();
  protected:
    virtual SbBool  validNodeType(SoNode *node);
};

class SoXtShininessMultiSlider : public SoXtMultiSlider {
  public:
    SoXtShininessMultiSlider(
	Widget parent = NULL,
	const char *name = NULL, 
	SbBool buildInsideParent = TRUE);
    ~SoXtShininessMultiSlider();
    virtual void    exportValuesToInventor();
    virtual void    importValuesFromInventor();
  protected:
    virtual SbBool  validNodeType(SoNode *node);
};

class SoXtTransparencyMultiSlider : public SoXtMultiSlider {
  public:
    SoXtTransparencyMultiSlider(
	Widget parent = NULL,
	const char *name = NULL, 
	SbBool buildInsideParent = TRUE);
    ~SoXtTransparencyMultiSlider();
    virtual void    exportValuesToInventor();
    virtual void    importValuesFromInventor();
  protected:
    virtual SbBool  validNodeType(SoNode *node);
};

class SoXtLightIntensityMultiSlider : public SoXtMultiSlider {
  public:
    SoXtLightIntensityMultiSlider(
	Widget parent = NULL,
	const char *name = NULL, 
	SbBool buildInsideParent = TRUE);
    ~SoXtLightIntensityMultiSlider();
    virtual void    exportValuesToInventor();
    virtual void    importValuesFromInventor();
  protected:
    virtual SbBool  validNodeType(SoNode *node);
};

class SoXtLightColorMultiSlider : public SoXtMultiSlider {
  public:
    SoXtLightColorMultiSlider(
	Widget parent = NULL,
	const char *name = NULL, 
	SbBool buildInsideParent = TRUE);
    ~SoXtLightColorMultiSlider();
    virtual void    exportValuesToInventor();
    virtual void    importValuesFromInventor();
  protected:
    virtual SbBool  validNodeType(SoNode *node);
};

class SoXtFovMultiSlider : public SoXtMultiSlider {
  public:
    SoXtFovMultiSlider(
	Widget parent = NULL,
	const char *name = NULL, 
	SbBool buildInsideParent = TRUE);
    ~SoXtFovMultiSlider();
    virtual void    exportValuesToInventor();
    virtual void    importValuesFromInventor();
  protected:
    virtual SbBool  validNodeType(SoNode *node);
};

#endif /*  SO_XT_MULTI_SLIDERS_ */
