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
// Implementation of cool backgrounds from Drew Olbrich
// Modified by Gavin Bell
//

#include <math.h>
#include <unistd.h>
#include <stdlib.h>

#include <Inventor/SbLinear.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoPickStyle.h>
#include <Inventor/nodes/SoQuadMesh.h>
#include <Inventor/nodes/SoOrthographicCamera.h>

#include "Background.h"

#define ABS(a) ((a) < 0.0 ? -(a) : (a))
#define CLAMP(a, b, c) ((b) > (a) ? (b) : ((a) > (c) ? (c) : (a)))
#define SMOOTH(a) ((a)*(a)*(3.0 - 2.0*(a)))

static const int HASH_SIZE=751; // two of my favorite prime numbers
static const int HASH_VALUE=911;

static const int32_t BIG_NOISE=100000; // noise repeats this often

//
// Constructor.  The background is always the same number of rows and
// columns (that number is the size argument that gets passed).
//
FlashBackground::FlashBackground(int size)
{
    //
    // Must create hash table...
    //
    hash_table = new double[HASH_SIZE];

    srand48(getpid());
    int i;
    for (i = 0; i < HASH_SIZE; i++)
	hash_table[i] = drand48();

    //
    // And must create the scene graph
    //
    meshSize = size;
    int nverts = (size+1)*(size+1);

    // Set up a 0.0 to 1.0 view
    SoOrthographicCamera *oc = new SoOrthographicCamera;
    //
    // The LEAVE_ALONE viewport mapping is used because we don't
    // really care if the background is squished/stretched, but it
    // MUST always fill up the entire viewport.
    //
    oc->viewportMapping = SoCamera::LEAVE_ALONE;
    oc->position.setValue(0.5, 0.5, 1);
    oc->height = 0.9;
    oc->aspectRatio = 1.0;
    oc->farDistance = 1001.0;
    oc->focalDistance = 999.0;
    // Defaults for other stuff are OK.
    sceneGraph->addChild(oc);

    coordinates = new SoCoordinate3;
    //
    // Pre-allocate enough room in the coordinates node for the
    // vertices
    //
    coordinates->point.insertSpace(0, nverts);
    sceneGraph->addChild(coordinates);

    colors = new SoBaseColor;
    colors->rgb.insertSpace(0, nverts);
    sceneGraph->addChild(colors);

    SoMaterialBinding *mb = new SoMaterialBinding;
    mb->value = SoMaterialBinding::PER_VERTEX_INDEXED;
    sceneGraph->addChild(mb);

    SoLightModel *lm = new SoLightModel;
    lm->model = SoLightModel::BASE_COLOR;
    sceneGraph->addChild(lm);

    SoPickStyle *ps = new SoPickStyle;
    ps->style = SoPickStyle::UNPICKABLE;
    sceneGraph->addChild(ps);

    SoQuadMesh *qm = new SoQuadMesh;
    qm->verticesPerRow = meshSize+1;
    qm->verticesPerColumn = meshSize+1;
    sceneGraph->addChild(qm);

    //
    // Pre-calculate some different shapes and colors.  These are
    // linearly interpolated in a random order to get the nice
    // animation.  It takes a significant time to calculate some of
    // the shape/color functions; pre-calculating them avoids annoying
    // jumps in the animation.
    //
    shapeTable = new SbVec3f*[NUM_SHAPES];
    for (i = 0; i < NUM_SHAPES; i++)
    {
	shapeTable[i] = new SbVec3f[nverts];
	calculateShape((Shape)i, shapeTable[i]);
    }
    schemeTable = new SbColor*[NUM_SHAPES];
    for (i = 0; i < NUM_SCHEMES; i++)
    {
	schemeTable[i] = new SbColor[nverts];
	calculateColor((Shade)(lrand48()%NUM_SHADES), (Scheme)i,
		       schemeTable[i]);
    }
    //
    // Start out with a random shape and color
    //
    currentShape = (Shape)(lrand48()%NUM_SHAPES);
    currentScheme = (Scheme)(lrand48()%NUM_SCHEMES);
    nextShape = (Shape)(lrand48()%NUM_SHAPES);
    nextScheme = (Scheme)(lrand48()%NUM_SCHEMES);
    //
    // And start at time 0.0
    //
    updateSceneShape(0.0);
    updateSceneColor(0.0);
}

FlashBackground::~FlashBackground()
{
    //
    // Must get rid of all the stuff allocated or ref'ed in the
    // constructor.  Note: the sceneGraph member variable is allocated
    // and freed in the Background base class.
    //
    delete[] hash_table;

    int i;
    for (i = 0; i < NUM_SHAPES; i++)
    {
	delete[] shapeTable[i];
    }
    delete[] shapeTable;
    for (i = 0; i < NUM_SCHEMES; i++)
    {
	delete[] schemeTable[i];
    }
    delete[] schemeTable;
}

//
//      Noise() - returns noise at integer boundaries
//
//      Warning!  This only works for positive values!
//
double
FlashBackground::Noise(uint32_t x, uint32_t y, uint32_t z)
{
    uint32_t index;
    double result;

    index = x;
    index = index*HASH_VALUE + y;
    index = index*HASH_VALUE + z;
    result = hash_table[index % HASH_SIZE];

    // add a little high order noise to make it look more convincing
    index = HASH_VALUE + x*1000;
    index = index*HASH_VALUE + y*1000;
    index = index*HASH_VALUE + z*1000;
    result += hash_table[index % HASH_SIZE];

    if (result >= 1.0)
	result -= 1.0;

    return result;
}

//
//      Noise() - the noise function
//
double
FlashBackground::Noise(SbVec3f &p)
{
    uint32_t x0, y0, z0;
    uint32_t x1, y1, z1;
    double sx, sy, sz;
    double tx, ty, tz;
    double n;
    double result;

    SbVec3f q = p;
    if (q[0] < 0.0)
	q[0] += BIG_NOISE;
    q[0] = fmod(q[0], (double) BIG_NOISE);
    if (q[1] < 0.0)
	q[1] += BIG_NOISE;
    q[1] = fmod(q[1], (double) BIG_NOISE);
    if (q[2] < 0.0)
	q[2] += BIG_NOISE;
    q[2] = fmod(q[2], (double) BIG_NOISE);

    x0 = (uint32_t) q[0];
    y0 = (uint32_t) q[1];
    z0 = (uint32_t) q[2];

    x1 = (x0 + 1) % BIG_NOISE;
    y1 = (y0 + 1) % BIG_NOISE;
    z1 = (z0 + 1) % BIG_NOISE;

    sx = SMOOTH(q[0] - x0);
    sy = SMOOTH(q[1] - y0);
    sz = SMOOTH(q[2] - z0);

    tx  = 1.0 - sx;
    ty = 1.0 - sy;
    tz = 1.0 - sz;

    n = Noise(x0, y0, z0);
    result = tx*ty*tz*n;
    n = Noise(x0, y0, z1);
    result += tx*ty*sz*n;
    n = Noise(x0, y1, z0);
    result += tx*sy*tz*n;
    n = Noise(x0, y1, z1);
    result += tx*sy*sz*n;
    n = Noise(x1, y0, z0);
    result += sx*ty*tz*n;
    n = Noise(x1, y0, z1);
    result += sx*ty*sz*n;
    n = Noise(x1, y1, z0);
    result += sx*sy*tz*n;
    n = Noise(x1, y1, z1);
    result += sx*sy*sz*n;

    return result;
}

//
//      Turbulence() - the turbulence function
//
//      The variable 'octaves' holds the number of octaves of noise
//      we want to make the turbulence out of.
//
double
FlashBackground::Turbulence(SbVec3f &p, int octaves)
{
    double scale = 1.0;
    double inv_scale = 1.0;
    double result = 0.0;
    double max_total = 0.0;
    int i;
    SbVec3f q;

    for (i = 0; i < octaves; i++)
    {
	max_total += inv_scale;
	q = p * scale;
	result += Noise(q)*inv_scale;
	if (i != octaves - 1)
	{
	    scale *= 2.0;
	    inv_scale *= 0.5;
	}
    }

    result /= max_total;          /* scale it to [0..1] */

    return result;
}

void
FlashBackground::shapeFunction(Shape shape, SbVec3f &xy)
{
    double dx, dy;
    double l;
    double theta;

    switch (shape) {
      case SHAPE_CLOUDS :
	xy[1] = (0.5 - xy[1])*2.0;
	xy[1] = xy[1]*xy[1]*xy[1];
	xy[1] = 1.0 - (xy[1]/2.0 + 0.5);
	break;
      case SHAPE_FLAT :
	break;
      case SHAPE_SWIRL :
	dx = 0.5 - xy[0];
	dy = 0.5 - xy[1];
	l = sqrt(dx*dx + dy*dy);
	if (dx != 0.0 || dy != 0.0)
	{
	    theta = atan2(dy, dx);
	    xy[0] = 0.5 + cos(theta + l*5.0)*l*1.5;
	    xy[1] = 0.5 + sin(theta + l*5.0)*l*1.5;
	}
	break;
      case SHAPE_PINCH :
	dx = xy[0] - 0.5;
	dy = xy[1] - 0.5;
	l = sqrt(dx*dx + dy*dy);
	xy[0] = 0.5 + dx*l*l*4.0;
	xy[1] = 0.5 + dy*l*l*4.0;
	break;
      case SHAPE_WAVY :
	xy[1] += sin(xy[0]*15.0)*0.06;
	xy[1] = 0.5 + (xy[1] - 0.5)*1.3;
	break;
      case SHAPE_SCRUNCH :
	xy[1] = xy[1]*xy[1]*xy[1];
	break;
      case SHAPE_WALLS :
	dx = (1.0 - ABS(xy[0] - 0.5)*2.0);
	dx = 1.0 - exp(dx);
	if (xy[1] != 1.0)
	    xy[1] += xy[1]*dx*0.8;
	break;
      case SHAPE_PULL :
	dx = (xy[0] - 0.5)*2.0;
	dx *= 4.0;
	dx = 1.0/(1.0 + dx*dx);
	xy[1] += xy[1]*dx;
	break;
      case NUM_SHAPES:
	fprintf(stderr, "BLEUCH: NUM_SHAPES CASE REACHED\n");
	break;
    }
}

double
FlashBackground::shadeFunction(Shade shade, double i, double j)
{
    double gray;
    double result;
    SbVec3f v;

    switch (shade) {
      case SHADE_BARS :
	j *= meshSize;
	gray = (meshSize - j)*120/meshSize + ((int) (j*4129) % 30);
	result = gray/255.0;
	break;
      case SHADE_STATIC :
	v.setValue(i*20.0, j*20.0, 0.0);
	result = drand48()*0.5 + 0.25;
	result *= (1.0 - j)*0.8;
	break;
      case SHADE_FOG :
	v.setValue(i*6.0, j*6.0, 0.0);
	result = Turbulence(v, 5);
	result *= (1.0 - j)*0.8;
	break;
      case SHADE_MARBLE :
	v.setValue(i*5.0, j*5.0, 0.0);
	result = sin(i*30.0 + 15.0*Turbulence(v, 5))*0.5 + 0.5;
	result = result*0.5 + 0.25;
	result *= (1.0 - j)*0.8;
	break;
      case SHADE_HAZY :
	j *= meshSize;
	gray = (meshSize - j)*120/meshSize + ((int) (j*4129) % 30);
	result = gray/255.0;
	v.setValue(i*4.5, j*6.0, 0.0);
	v[0] += v[1]/3.1;
	result *= Turbulence(v, 4);
	break;
      case NUM_SHADES:
	fprintf(stderr, "BLEUCH: NUM_SHADES CASE REACHED\n");
	break;
    }
    return result;
}

void
FlashBackground::colorFunction(Scheme cscheme, SbColor &c, double y)
{
    switch (cscheme) {
      case COLOR_GRAY :
	break;
      case COLOR_NOISE :
	c[0] += (drand48()*2.0 - 1.0)*0.03;
	c[1] += (drand48()*2.0 - 1.0)*0.03;
	c[2] += (drand48()*2.0 - 1.0)*0.03;
	break;
      case COLOR_PURPLE :
	c[0] = c[0];
	c[1] = SMOOTH(c[1]);
	c[2] = c[2];
	break;
      case COLOR_SUNSET :
	c[0] = c[0]*3.0 - 0.5;
	c[1] = c[1]*c[1]*c[1]*2.5;
	c[2] = c[2]*(1.0 - y);
	break;
      case COLOR_ABYSS :
	c[0] = c[0]*c[0]*c[0]*3.0 - 0.5;
	c[1] = c[1]*c[1]*3.0 - 0.5;
	c[2] = c[2]*3.0 - 0.5;
	break;
      case NUM_SCHEMES:
	fprintf(stderr, "BLEUCH: NUM_SCHEMES CASE REACHED\n");
	break;
    }
    c[0] = CLAMP(c[0], 0.0, 1.0);
    c[1] = CLAMP(c[1], 0.0, 1.0);
    c[2] = CLAMP(c[2], 0.0, 1.0);
}

void
FlashBackground::calculateShape(Shape shape,
				SbVec3f *coords)
{
    for (int i = 0; i < meshSize+1; i++)
    {
	double y = (double)i / (double)(meshSize);

	for (int j = 0; j < meshSize+1; j++)
	{
	    int index = i*(meshSize+1)+j;
	    double x = (double)j / (double)(meshSize);

	    coords[index].setValue(x, y, -999.0);

	    shapeFunction(shape, coords[index]);

	    if (j > 0 && j < meshSize)
		coords[index][1] += drand48()*x*0.015;
	}
    }
}

void
FlashBackground::calculateColor(Shade shade, 
				Scheme cscheme,
				SbColor *clrs)
{
    for (int i = 0; i < meshSize+1; i++)
    {
	double y = (double)i / (double)(meshSize);

	for (int j = 0; j < meshSize+1; j++)
	{
	    int index = i*(meshSize+1)+j;
	    double x = (double)j / (double)(meshSize);

	    double gray = shadeFunction(shade, x, y);
	    clrs[index][0] = gray;
	    clrs[index][1] = gray;
	    clrs[index][2] = gray;
	    
	    colorFunction(cscheme, clrs[index], 1.0 - x);
	}
    }
}

//
// Once we figure out which two shapes we are interpolating
// (currentShape and the shape after currentShape) these two routines
// actually do the interpolation.  If we wanted to speed this up,
// these are the best candidates for optimization effort.
//
void
FlashBackground::updateSceneShape(double t)
{
    SbVec3f *coords = coordinates->point.startEditing();

    for (int i = 0; i < meshSize+1; i++)
    {
	for (int j = 0; j < meshSize+1; j++)
	{
	    int index = i*(meshSize+1)+j;

	    coords[index] = (1.0 - t) * shapeTable[currentShape][index] +
		t * shapeTable[nextShape][index];
	}
    }
    coordinates->point.finishEditing();
}
void
FlashBackground::updateSceneColor(double t)
{
    SbColor *clrs = colors->rgb.startEditing();

    for (int i = 0; i < meshSize+1; i++)
    {
	for (int j = 0; j < meshSize+1; j++)
	{
	    int index = i*(meshSize+1)+j;

	    SbVec3f workAround = 
		(1.0 - t) * schemeTable[currentScheme][index] +
		    t * schemeTable[nextScheme][index];
	    clrs[index] = workAround.getValue();
	}
    }
    colors->rgb.finishEditing();
}

//
// This is called repeatedly to animate between the various shape and
// color schemes.  It is assumed that time does not go backwards!  If
// real-world time is fed directly to this function, shapes will
// change at a rate of one per second.  You can multiply or divide to
// get faster or slower changes (I find a change every 8 seconds to be
// pretty nice).
//
void
FlashBackground::animateShape(double time)
{
    static double lastTime = (-1.0);

    if (lastTime == (-1.0))
    {
	lastTime = time;
    }

    double t = time - lastTime;

    while (t > 1.0)
    {
	// Choose a new next shape
	currentShape = nextShape;
	nextShape = (Shape)(lrand48()%NUM_SHAPES);

	t -= 1.0;
	lastTime += 1.0;
    }
    updateSceneShape(t);
}
void
FlashBackground::animateColor(double time)
{
    static double lastTime = (-1.0);

    if (lastTime == (-1.0))
    {
	lastTime = time;
    }

    double t = time - lastTime;

    while (t > 1.0)
    {
	// Choose a new next color scheme
	currentScheme = nextScheme;
	nextScheme = (Scheme)(lrand48()%NUM_SCHEMES);

	t -= 1.0;
	lastTime += 1.0;
    }
    updateSceneColor(t);
}
