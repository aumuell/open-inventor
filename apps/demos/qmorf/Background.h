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

//
// Inventor base class for scene graphs that can be used as
// Backgrounds (static and animated).
//
// Backgrounds should typically be the first child of a LayerGroup.
// They have their own cameras, sensors, etc.  Note that when being
// used with a Viewer, the program should do the following:
//
// Create a LayerGroup.
// Create the Background, and add it as the first child of the
// LayerGroup.
// Create the 'main' scene graph.
// Add a camera to the 'main' scene graph if it doesn't have one.
// Create the Viewer.
// Pass the LayerGroup to the setSceneGraph method of the viewer
// Pass the camera created or found to the setCamera method of the
// viewer (or the viewer will incorrectly use the Background's
// camera).
//

#include <Inventor/SbBasic.h>

class SoNode;
class SoGroup;
class SoCoordinate3;
class SoBaseColor;
class SbColor;
class SbVec3f;
class SoSensor;
class SoTimerSensor;

//
// Base class; this also functions as a blank background.
//
class Background
{
  public:
    Background();
    virtual ~Background();

    // Returns scene graph rep of the background
    virtual SoNode *getSceneGraph() { return sceneGraph; }

  protected:
    SoGroup *sceneGraph;
};

//
// Class derived from code written by Drew Olbrich
//
class FlashBackground : public Background
{
  public:
    FlashBackground(int size = 12);
    ~FlashBackground();
    
    void animateShape(double t);
    void animateColor(double t);

    enum Shape {
	SHAPE_CLOUDS, SHAPE_FLAT, SHAPE_SWIRL, SHAPE_PINCH,
	SHAPE_WAVY, SHAPE_SCRUNCH, SHAPE_WALLS, SHAPE_PULL,
	NUM_SHAPES
    };

    enum Shade {
	SHADE_HAZY, SHADE_BARS, SHADE_STATIC, SHADE_FOG, SHADE_MARBLE,
	NUM_SHADES
    };

    enum Scheme {
	COLOR_GRAY, COLOR_NOISE, COLOR_PURPLE, COLOR_SUNSET, COLOR_ABYSS,
	NUM_SCHEMES
    };

  private:
    int meshSize;
    double *hash_table;
    SoCoordinate3 *coordinates;
    SoBaseColor *colors;
    SbVec3f **shapeTable;
    SbColor **schemeTable;
    Shape currentShape;
    Scheme currentScheme;
    Shape nextShape;
    Scheme nextScheme;

    double Noise(uint32_t, uint32_t, uint32_t);
    double Noise(SbVec3f &);
    double Turbulence(SbVec3f &, int);
    void shapeFunction(Shape, SbVec3f &);
    double shadeFunction(Shade, double, double);
    void colorFunction(Scheme, SbColor &, double);
    void calculateShape(Shape, SbVec3f *);
    void calculateColor(Shade, Scheme, SbColor *);
    void updateSceneShape(double);
    void updateSceneColor(double);
};

