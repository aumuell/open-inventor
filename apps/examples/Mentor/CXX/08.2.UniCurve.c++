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

/*--------------------------------------------------------
 *  This is an example from the Inventor Mentor,
 *  chapter 8, example 2.
 *
 *  This example creates and displays a Uniform B-Spline curve
 *  that passes through the end control points.
 *  The curve is order 4 with 13 control points and a knot
 *  vector of length 17.  The end knots have multiplicity
 *  4 to illustrate the curve passing through the endpoints.
 *--------------------------------------------------------*/

#include <stdlib.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoCamera.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoNurbsCurve.h>
#include <Inventor/nodes/SoRotation.h>
#include <Inventor/nodes/SoScale.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTranslation.h>

static char *floorData = 
   "#Inventor V2.0 ascii\n"
   "Separator {\n"
   "   SpotLight {\n"
   "      cutOffAngle 0.9\n"
   "      dropOffRate 0.2\n" 
   "      location 6 12 2\n" 
   "      direction 0 -1 0\n"
   "   }\n"
   "   ShapeHints {\n"
   "      faceType UNKNOWN_FACE_TYPE\n"
   "   }\n"
   "   Texture2Transform {\n"
   "      #rotation 1.57\n"
   "      scaleFactor 8 8\n"
   "   }\n"
   "   Texture2 {\n"
   "      filename /usr/share/src/Inventor/examples/data/oak.rgb\n"
   "   }\n"
   "   NormalBinding {\n"
   "        value  PER_PART\n"
   "   }\n"
   "   Material { diffuseColor 1 1 1 specularColor 1 1 1 shininess 0.4 }\n"
   "   DEF FloorPanel Separator {\n"
   "      DEF FloorStrip Separator {\n"
   "         DEF FloorBoard Separator {\n"
   "            Normal { vector 0 1 0 }\n"
   "            TextureCoordinate2 {\n"
   "               point [ 0 0, 0.5 0, 0.5 2, 0.5 4, 0.5 6,\n"
   "                       0.5 8, 0 8, 0 6, 0 4, 0 2 ] }\n"
   "            Coordinate3 {\n"
   "               point [ 0 0 0, .5 0 0, .5 0 -2, .5 0 -4, .5 0 -6,\n"
   "                       .5 0 -8, 0 0 -8, 0 0 -6, 0 0 -4, 0 0 -2, ]\n"
   "            }\n"
   "            FaceSet { numVertices 10 }\n"
   "            BaseColor { rgb 0.3 0.1 0.0 }\n"
   "            Translation { translation 0.125 0 -0.333 }\n"
   "            Cylinder { parts TOP radius 0.04167 height 0.002 }\n"
   "            Translation { translation 0.25 0 0 }\n"
   "            Cylinder { parts TOP radius 0.04167 height 0.002 }\n"
   "            Translation { translation 0 0 -7.333 }\n"
   "            Cylinder { parts TOP radius 0.04167 height 0.002 }\n"
   "            Translation { translation -0.25 0 0 }\n"
   "            Cylinder { parts TOP radius 0.04167 height 0.002 }\n"
   "         }\n"
   "         Translation { translation 0 0 8.03 }\n"
   "         USE FloorBoard\n"
   "         Translation { translation 0 0 8.04 }\n"
   "         USE FloorBoard\n"
   "      }\n"
   "      Translation { translation 0.53 0 -0.87 }\n"
   "      USE FloorStrip\n"
   "      Translation { translation 0.53 0 -2.3 }\n"
   "      USE FloorStrip\n"
   "      Translation { translation 0.53 0 1.3 }\n"
   "      USE FloorStrip\n"
   "      Translation { translation 0.53 0 1.1 }\n"
   "      USE FloorStrip\n"
   "      Translation { translation 0.53 0 -0.87 }\n"
   "      USE FloorStrip\n"
   "      Translation { translation 0.53 0 1.7 }\n"
   "      USE FloorStrip\n"
   "      Translation { translation 0.53 0 -0.5 }\n"
   "      USE FloorStrip\n"
   "   }\n"
   "   Translation { translation 4.24 0 0 }\n"
   "   USE FloorPanel\n"
   "   Translation { translation 4.24 0 0 }\n"
   "   USE FloorPanel\n"
   "}";

////////////////////////////////////////////////////////////
// CODE FOR The Inventor Mentor STARTS HERE

// The control points for this curve
float pts[13][3] = {
   { 6.0,  0.0,  6.0},
   {-5.5,  0.5,  5.5},
   {-5.0,  1.0, -5.0},
   { 4.5,  1.5, -4.5},
   { 4.0,  2.0,  4.0},
   {-3.5,  2.5,  3.5},
   {-3.0,  3.0, -3.0},
   { 2.5,  3.5, -2.5},
   { 2.0,  4.0,  2.0},
   {-1.5,  4.5,  1.5},
   {-1.0,  5.0, -1.0},
   { 0.5,  5.5, -0.5},
   { 0.0,  6.0,  0.0}};

// The knot vector
float knots[17] = {
   0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10};

// Create the nodes needed for the B-Spline curve.
SoSeparator *
makeCurve()
{
   SoSeparator *curveSep = new SoSeparator();
   curveSep->ref();

   // Set the draw style of the curve.
   SoDrawStyle *drawStyle  = new SoDrawStyle;
   drawStyle->lineWidth = 4;
   curveSep->addChild(drawStyle);

   // Define the NURBS curve including the control points
   // and a complexity.
   SoComplexity  *complexity = new SoComplexity;
   SoCoordinate3 *controlPts = new SoCoordinate3;
   SoNurbsCurve  *curve      = new SoNurbsCurve;
   complexity->value = 0.8;
   controlPts->point.setValues(0, 13, pts);
   curve->numControlPoints = 13;
   curve->knotVector.setValues(0, 17, knots);
   curveSep->addChild(complexity);
   curveSep->addChild(controlPts);
   curveSep->addChild(curve);

   curveSep->unrefNoDelete();
   return curveSep;
}

// CODE FOR The Inventor Mentor ENDS HERE
////////////////////////////////////////////////////////////

int
main(int, char **argv)
{
   // Initialize Inventor and Xt
   Widget appWindow = SoXt::init(argv[0]);
   if (appWindow == NULL)
      exit(1);

   SoSeparator *root  = new SoSeparator;
   root->ref();

   // Create the scene graph for the spiral
   SoSeparator  *spiral    = new SoSeparator;
   SoSeparator  *curveSep  = makeCurve();
   SoLightModel *lmodel    = new SoLightModel;
   SoBaseColor  *clr       = new SoBaseColor;

   lmodel->model = SoLightModel::BASE_COLOR;
   clr->rgb.setValue(SbColor(1.0, 0.0, 0.1));
   spiral->addChild(lmodel);
   spiral->addChild(clr);
   spiral->addChild(curveSep);
   root->addChild(spiral);

   // Create the scene graph for the floor
   SoSeparator   *floor = new SoSeparator;
   SoTranslation *xlate = new SoTranslation;
   SoRotation    *rot   = new SoRotation;
   SoScale       *scale = new SoScale;     
   SoInput       in;
   SoNode        *result;

   in.setBuffer(floorData, strlen(floorData));
   SoDB::read(&in, result);
   xlate->translation.setValue(SbVec3f(-12.0, -5.0, -5.0));
   scale->scaleFactor.setValue(SbVec3f(2.0, 1.0, 2.0));
   rot->rotation.setValue(SbRotation(SbVec3f(0.0, 1.0, 0.0), M_PI/2.0));
   floor->addChild(rot);
   floor->addChild(xlate);
   floor->addChild(scale);
   floor->addChild(result);
   root->addChild(floor);

   // Create the scene graph for the spiral's shadow
   SoSeparator  *shadow = new SoSeparator;
   SoLightModel *shmdl  = new SoLightModel;
   SoMaterial   *shmtl  = new SoMaterial;
   SoBaseColor  *shclr  = new SoBaseColor;
   SoTranslation *shxl  = new SoTranslation;
   SoScale      *shscl  = new SoScale;
   
   shmdl->model = SoLightModel::BASE_COLOR;
   shclr->rgb.setValue(SbColor(0.21, 0.15, 0.09));
   shmtl->transparency = 0.5;
   shxl->translation.setValue(SbVec3f(0.0, -4.9, 0.0));
   shscl->scaleFactor.setValue(SbVec3f(1.0, 0.0, 1.0));
   shadow->addChild(shmtl);
   shadow->addChild(shmdl);
   shadow->addChild(shclr);
   shadow->addChild(shxl);
   shadow->addChild(shscl);
   shadow->addChild(curveSep);
   root->addChild(shadow);

   // Initialize an Examiner Viewer
   SoXtExaminerViewer *viewer =
            new SoXtExaminerViewer(appWindow);
   viewer->setSceneGraph(root);
   viewer->setTitle("B-Spline Curve");
   SoCamera *cam = viewer->getCamera();
   cam->position.setValue(SbVec3f(-6.0, 8.0, 20.0));
   cam->pointAt(SbVec3f(0.0, -2.0, -4.0));
   viewer->show();

   SoXt::show(appWindow);
   SoXt::mainLoop();
}

