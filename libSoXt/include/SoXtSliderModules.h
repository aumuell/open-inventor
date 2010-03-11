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
|      Defines the various subclasses of sliders modules
|
|   Author(s)          : Paul Isaacs
|
______________  S I L I C O N   G R A P H I C S   I N C .  ____________
_______________________________________________________________________
*/


#ifndef SO_XT_SLIDER_MODULES_
#define SO_XT_SLIDER_MODULES_

#include "SoXtSliderModule.h"

#define SO_TRANSLATE_MODULE_LABEL "TRANSLATIONS"
#define SO_TRANSLATE_MODULE_SLIDER1_LABEL "X Translation"
#define SO_TRANSLATE_MODULE_SLIDER2_LABEL "Y Translation"
#define SO_TRANSLATE_MODULE_SLIDER3_LABEL "Z Translation"

#define SO_SCALE_MODULE_LABEL "SCALES"
#define SO_SCALE_MODULE_SLIDER1_LABEL "X Scale"
#define SO_SCALE_MODULE_SLIDER2_LABEL "Y Scale"
#define SO_SCALE_MODULE_SLIDER3_LABEL "Z Scale"

#define SO_ROTATE_MODULE_LABEL "ROTATIONS"
#define SO_ROTATE_MODULE_SLIDER1_LABEL "X Rotate"
#define SO_ROTATE_MODULE_SLIDER2_LABEL "Y Rotate"
#define SO_ROTATE_MODULE_SLIDER3_LABEL "Z Rotate"

#define SO_SCALE_ORIENTATION_MODULE_LABEL "SCALE ORIENTATION"
#define SO_SCALE_ORIENTATION_MODULE_SLIDER1_LABEL "X Rotate"
#define SO_SCALE_ORIENTATION_MODULE_SLIDER2_LABEL "Y Rotate"
#define SO_SCALE_ORIENTATION_MODULE_SLIDER3_LABEL "Z Rotate"

#define SO_CENTER_MODULE_LABEL "CENTER"
#define SO_CENTER_MODULE_SLIDER1_LABEL "X Center"
#define SO_CENTER_MODULE_SLIDER2_LABEL "Y Center"
#define SO_CENTER_MODULE_SLIDER3_LABEL "Z Center"

#define SO_AMBIENT_COLOR_MODULE_LABEL "AMBIENT"
#define SO_AMBIENT_COLOR_MODULE_SLIDER1_LABEL "Ambient Red"
#define SO_AMBIENT_COLOR_MODULE_SLIDER2_LABEL "Ambient Green"
#define SO_AMBIENT_COLOR_MODULE_SLIDER3_LABEL "Ambient Blue"

#define SO_DIFFUSE_COLOR_MODULE_LABEL "DIFFUSE"
#define SO_DIFFUSE_COLOR_MODULE_SLIDER1_LABEL "Diffuse Red"
#define SO_DIFFUSE_COLOR_MODULE_SLIDER2_LABEL "Diffuse Green"
#define SO_DIFFUSE_COLOR_MODULE_SLIDER3_LABEL "Diffuse Blue"

#define SO_SPECULAR_COLOR_MODULE_LABEL "SPECULAR"
#define SO_SPECULAR_COLOR_MODULE_SLIDER1_LABEL "Specular Red"
#define SO_SPECULAR_COLOR_MODULE_SLIDER2_LABEL "Specular Green"
#define SO_SPECULAR_COLOR_MODULE_SLIDER3_LABEL "Specular Blue"

#define SO_EMISSIVE_COLOR_MODULE_LABEL "EMISSIVE"
#define SO_EMISSIVE_COLOR_MODULE_SLIDER1_LABEL "Emissive Red"
#define SO_EMISSIVE_COLOR_MODULE_SLIDER2_LABEL "Emissive Green"
#define SO_EMISSIVE_COLOR_MODULE_SLIDER3_LABEL "Emissive Blue"

#define SO_SHININESS_MODULE_LABEL "SHININESS"
#define SO_SHININESS_MODULE_SLIDER1_LABEL "Shininess"

#define SO_TRANSPARENCY_MODULE_LABEL "TRANSPARENCY"
#define SO_TRANSPARENCY_MODULE_SLIDER1_LABEL "Transparency"

#define SO_LIGHT_INTENSITY_MODULE_LABEL "INTENSITY"
#define SO_LIGHT_INTENSITY_MODULE_SLIDER1_LABEL "Intensity"

#define SO_LIGHT_COLOR_MODULE_LABEL "COLOR"
#define SO_LIGHT_COLOR_MODULE_SLIDER1_LABEL "Red"
#define SO_LIGHT_COLOR_MODULE_SLIDER2_LABEL "Green"
#define SO_LIGHT_COLOR_MODULE_SLIDER3_LABEL "Blue"

class SoXtTransSliderModule : public SoXtSliderModule {
  public:
      SoXtTransSliderModule(
	Widget parent = NULL,
	const char *name = NULL, 
	SbBool buildInsideParent = TRUE);
     ~SoXtTransSliderModule();
  private:
      void initLabels();
};

class SoXtScaleSliderModule : public SoXtSliderModule {
  public:
      SoXtScaleSliderModule(
	Widget parent = NULL,
	const char *name = NULL, 
	SbBool buildInsideParent = TRUE);
     ~SoXtScaleSliderModule();
  private:
      void initLabels();
};

class SoXtRotateSliderModule : public SoXtSliderModule {
  public:
      SoXtRotateSliderModule(
	Widget parent = NULL,
	const char *name = NULL, 
	SbBool buildInsideParent = TRUE);
     ~SoXtRotateSliderModule();
  private:
      void initLabels();
};

class SoXtScaleOrientationSliderModule : public SoXtSliderModule {
  public:
      SoXtScaleOrientationSliderModule(
	Widget parent = NULL,
	const char *name = NULL, 
	SbBool buildInsideParent = TRUE);
     ~SoXtScaleOrientationSliderModule();
  private:
      void initLabels();
};

class SoXtCenterSliderModule : public SoXtSliderModule {
  public:
      SoXtCenterSliderModule(
	Widget parent = NULL,
	const char *name = NULL, 
	SbBool buildInsideParent = TRUE);
     ~SoXtCenterSliderModule();
  private:
      void initLabels();
};

class SoXtAmbientColorSliderModule : public SoXtSliderModule {
  public:
      SoXtAmbientColorSliderModule(
	Widget parent = NULL,
	const char *name = NULL, 
	SbBool buildInsideParent = TRUE);
     ~SoXtAmbientColorSliderModule();
  private:
      void initLabels();
};

class SoXtDiffuseColorSliderModule : public SoXtSliderModule {
  public:
      SoXtDiffuseColorSliderModule(
	Widget parent = NULL,
	const char *name = NULL, 
	SbBool buildInsideParent = TRUE);
     ~SoXtDiffuseColorSliderModule();
  private:
      void initLabels();
};

class SoXtSpecularColorSliderModule : public SoXtSliderModule {
  public:
      SoXtSpecularColorSliderModule(
	Widget parent = NULL,
	const char *name = NULL, 
	SbBool buildInsideParent = TRUE);
     ~SoXtSpecularColorSliderModule();
  private:
      void initLabels();
};

class SoXtEmissiveColorSliderModule : public SoXtSliderModule {
  public:
      SoXtEmissiveColorSliderModule(
	Widget parent = NULL,
	const char *name = NULL, 
	SbBool buildInsideParent = TRUE);
     ~SoXtEmissiveColorSliderModule();
  private:
      void initLabels();
};

class SoXtShininessSliderModule : public SoXtSliderModule {
  public:
      SoXtShininessSliderModule(
	Widget parent = NULL,
	const char *name = NULL, 
	SbBool buildInsideParent = TRUE);
     ~SoXtShininessSliderModule();
  private:
      void initLabels();
};

class SoXtTransparencySliderModule : public SoXtSliderModule {
  public:
      SoXtTransparencySliderModule(
	Widget parent = NULL,
	const char *name = NULL, 
	SbBool buildInsideParent = TRUE);
     ~SoXtTransparencySliderModule();
  private:
      void initLabels();
};

class SoXtLightIntensitySliderModule : public SoXtSliderModule {
  public:
      SoXtLightIntensitySliderModule(
	Widget parent = NULL,
	const char *name = NULL, 
	SbBool buildInsideParent = TRUE);
     ~SoXtLightIntensitySliderModule();
  private:
      void initLabels();
};

class SoXtLightColorSliderModule : public SoXtSliderModule {
  public:
      SoXtLightColorSliderModule(
	Widget parent = NULL,
	const char *name = NULL, 
	SbBool buildInsideParent = TRUE);
     ~SoXtLightColorSliderModule();
  private:
      void initLabels();
};

#endif /* SO_XT_SLIDER_MODULES_ */


