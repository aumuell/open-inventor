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
// Source file for "BarChart" shape node.
//

#include <assert.h>

#include <GL/gl.h>
#include <Inventor/SoDB.h>

#include <Inventor/SbBox.h>

#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>

#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoScale.h>
#include <Inventor/nodes/SoRotationXYZ.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoText3.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoMaterial.h>

#include "BarChart.h"

SO_NODE_SOURCE(BarChart);

#define MAX(x,y) ((x) > (y) ? (x) : (y))
#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define TWOPI (2.0*M_PI)

//
// This initializes the BarChart class.
//

void
BarChart::initClass()
{
    SO_NODE_INIT_CLASS(BarChart, SoShape, "Shape");
}

//
// Constructor
//

BarChart::BarChart()
{
    children = new SoChildList(this);
    
    SO_NODE_CONSTRUCTOR(BarChart);
    SO_NODE_ADD_FIELD(plateColor,	(0.3, 0.3, 0.3, 0));
    SO_NODE_ADD_FIELD(poleColor,	(0, 0.3, 0.5, 0));
    SO_NODE_ADD_FIELD(values,		(1.0));
    SO_NODE_ADD_FIELD(valueColors,	(0.8, 0, 0, 0));
    SO_NODE_ADD_FIELD(xLabels,		("X Label"));
    SO_NODE_ADD_FIELD(xLabelColors,	(0.8, 0, 0, 0));
    SO_NODE_ADD_FIELD(xLabelScale,	(1, 1, 1));
    SO_NODE_ADD_FIELD(yLabels,		("Y Label"));
    SO_NODE_ADD_FIELD(yLabelColors,	(0.8, 0.8, 0, 0));
    SO_NODE_ADD_FIELD(yLabelScale,	(1, 1, 1));
    SO_NODE_ADD_FIELD(xDimension,	(1));
    SO_NODE_ADD_FIELD(yDimension,	(1));
    SO_NODE_ADD_FIELD(xBarProportion,	(0.7));
    SO_NODE_ADD_FIELD(yBarProportion,	(0.7));
    SO_NODE_ADD_FIELD(minValue,		(0));
    SO_NODE_ADD_FIELD(maxValue,		(1.0));
    SO_NODE_ADD_FIELD(zLabelColor,	(0.8, 0.8, 0, 0));
    SO_NODE_ADD_FIELD(zMarkerColor,	(0.8, 0, 0, 0));
    SO_NODE_ADD_FIELD(zLabelScale,	(1, 1, 1));
    SO_NODE_ADD_FIELD(zLabelIncrement,	(0.2));
    SO_NODE_ADD_FIELD(barStyle,		(BAR));
}

//
// Destructor
//

BarChart::~BarChart()
{
}

//
// Implements the SoGetBoundingBoxAction for the BarChart node.
//

SbBox3f
BarChart::getBbox() const
{
    // everything inside BarChart normalizes to a unit volume
    SbVec3f max( 1, 1, 1 );
    return SbBox3f(-max, max);
}

void
BarChart::computeBBox(SoAction *, SbBox3f &box, SbVec3f &center)
{
    box = getBbox();
    center.setValue(0.0, 0.0, 0.0);
}

//
// Implements the SoGLRenderAction for the BarChart node.
//

void
BarChart::GLRender(SoGLRenderAction *action)
{
    if (! shouldGLRender(action))
	return;

    if (children->getLength() == 0) generateChildren();
    children->traverse(action);
}


void
BarChart::generatePrimitives(SoAction *action)
{
    if (children->getLength() == 0) generateChildren();
    children->traverse(action);
}

SoChildList 
*BarChart::getChildren() const
{
    return children;
}

void
BarChart::generateChildren()
{
    SoTranslation   *tr;
    SoScale	    *sc;
    SoRotationXYZ   *rot;
    SoMaterial	    *mtl;
    SoSeparator	    *sep;
    
    // this should get called only once so there should be no children yet.
    assert(children->getLength() == 0);
    
    // for testing, a wireframe cube surrounding the volume
    /*
    sep = new SoSeparator();
    children->append(sep);
    SoDrawStyle *ds = new SoDrawStyle();
    ds->style = SoDrawStyle::LINES;
    SoLightModel *lm = new SoLightModel();
    lm->model = SoLightModel::BASE_COLOR;
    sep->addChild(ds);
    sep->addChild(lm);
    sep->addChild(new SoCube);
    */
    
    // base plate
    sep = new SoSeparator();
    children->append(sep);
    tr = new SoTranslation();
    tr->translation.setValue(0, -0.95, 0);
    sep->addChild(tr);
    mtl = new SoMaterial();
    mtl->diffuseColor.setValue( plateColor.getValue()[0], 
				plateColor.getValue()[1], 
				plateColor.getValue()[2] );
    mtl->transparency.setValue( plateColor.getValue()[3] );
    sep->addChild(mtl);
    SoCube *plate = new SoCube();
    plate->height = 0.1;
    plate->width = 1.9;
    plate->depth = 1.9;
    sep->addChild(plate);
    
    // boundary poles
    sep = new SoSeparator();
    children->append(sep);
    mtl = new SoMaterial();
    mtl->diffuseColor.setValue( poleColor.getValue()[0], 
				poleColor.getValue()[1], 
				poleColor.getValue()[2] );
    mtl->transparency.setValue( poleColor.getValue()[3] );
    sep->addChild(mtl);
    SoCylinder *pole = new SoCylinder();
    pole->radius = 0.03;
    tr = new SoTranslation();
    tr->translation.setValue(-0.95, 0, -0.95);
    sep->addChild(tr);
    sep->addChild(pole);
    tr = new SoTranslation();
    tr->translation.setValue(0, 0, 1.9);
    sep->addChild(tr);
    sep->addChild(pole);
    tr = new SoTranslation();
    tr->translation.setValue(1.9, 0, 0);
    sep->addChild(tr);
    sep->addChild(pole);
    tr = new SoTranslation();
    tr->translation.setValue(0, 0, -1.9);
    sep->addChild(tr);
    sep->addChild(pole);

    // generate appropriate bars
    float barXSpace = 1.8/MAX(xDimension.getValue(), 1);
    float barYSpace = 1.8/MAX(yDimension.getValue(), 1);
    float barWidth = barXSpace*MIN(xBarProportion.getValue(), 1);
    float barDepth = barYSpace*MIN(yBarProportion.getValue(), 1);
    
    switch (barStyle.getValue())
    {
	case BAR:   generateBars(barXSpace, barYSpace, barWidth, barDepth);
		    break;
    }

    // add X labels
    sep = new SoSeparator();
    children->append(sep);
    tr = new SoTranslation();
    tr->translation.setValue(-0.9+barXSpace/2, -0.95, 0.98);
    sep->addChild(tr);
    sc = new SoScale();
    sc->scaleFactor.setValue(   xLabelScale.getValue()[0]*0.01, 
				xLabelScale.getValue()[1]*0.01, 
				xLabelScale.getValue()[2]*0.01 );
    sep->addChild(sc);
    
    int i;
    
    for (i = 0; i < xLabels.getNum() && i < xDimension.getValue(); ++i)
    {
	SoMaterial *mtl = new SoMaterial();
	int c = i % xLabelColors.getNum();
	mtl->diffuseColor.setValue( xLabelColors[c][0], 
				    xLabelColors[c][1], 
				    xLabelColors[c][2] );
	mtl->transparency.setValue( xLabelColors[c][3] );
	sep->addChild(mtl);

	SoText3 *text = new SoText3();
	text->string = xLabels[i];
	text->justification = SoText3::CENTER;
	text->parts = SoText3::ALL;
	sep->addChild(text);
	
	tr = new SoTranslation();
	tr->translation.setValue(barXSpace/(xLabelScale.getValue()[0]*0.01), 
				 0, 0);
	sep->addChild(tr);
    }
    
    // add Y labels (just rotate -90 degrees about XZ and do same as xlabels
    sep = new SoSeparator();
    children->append(sep);
    rot = new SoRotationXYZ();
    rot->axis = SoRotationXYZ::Y;
    rot->angle = TWOPI/4;
    sep->addChild(rot);
    tr = new SoTranslation();
    tr->translation.setValue(-0.9+barYSpace/2, -0.95, 0.98);
    sep->addChild(tr);
    sc = new SoScale();
    sc->scaleFactor.setValue(   yLabelScale.getValue()[0]*0.01, 
				yLabelScale.getValue()[1]*0.01, 
				yLabelScale.getValue()[2]*0.01 );
    sep->addChild(sc);
    
    for (i = 0; i < yLabels.getNum() && i < yDimension.getValue(); ++i)
    {
	SoMaterial *mtl = new SoMaterial();
	int c = i % yLabelColors.getNum();
	mtl->diffuseColor.setValue( yLabelColors[c][0], 
				    yLabelColors[c][1], 
				    yLabelColors[c][2] );
	mtl->transparency.setValue( yLabelColors[c][3] );
	sep->addChild(mtl);

	SoText3 *text = new SoText3();
	text->string = yLabels[i];
	text->justification = SoText3::CENTER;
	text->parts = SoText3::ALL;
	sep->addChild(text);
	
	tr = new SoTranslation();
	tr->translation.setValue(barYSpace/(yLabelScale.getValue()[0]*0.01), 
				 0, 0);
	sep->addChild(tr);
    }
    
    // add Z markings
    SoSeparator *zsep = new SoSeparator();
    children->append(zsep);
    tr = new SoTranslation();
    tr->translation.setValue(-0.95, -0.95, 0.95);
    zsep->addChild(tr);
    sep = new SoSeparator();
    zsep->addChild(sep);
    
    float nzLabels = (maxValue.getValue() - minValue.getValue()) / 
						    zLabelIncrement.getValue();
    
    // first add the spheres for markers
    mtl = new SoMaterial();
    mtl->diffuseColor.setValue( zMarkerColor.getValue()[0], 
				zMarkerColor.getValue()[1], 
				zMarkerColor.getValue()[2] );
    mtl->transparency.setValue( zMarkerColor.getValue()[3] );
    sep->addChild(mtl);
	
    float val;
    SoSphere *sph = new SoSphere(); // sphere for instancing
    sph->radius = 0.04;
    
    for (val = minValue.getValue() + zLabelIncrement.getValue(); 
	 val <= maxValue.getValue(); 
	 val += zLabelIncrement.getValue())
    {
	tr = new SoTranslation();
	tr->translation.setValue(0, (1.8/nzLabels), 0);
	sep->addChild(tr);
	sep->addChild(sph);
    }
    
    // now do the labels
    sep = new SoSeparator();
    zsep->addChild(sep);
    SoTransform *xform = new SoTransform();
    xform->scaleFactor.setValue(zLabelScale.getValue()[0]*0.01, 
				zLabelScale.getValue()[1]*0.01, 
				zLabelScale.getValue()[2]*0.01 );
    xform->center.setValue(0, 0, 0.05);
    xform->rotation.setValue(SbVec3f(0, 1, 0), TWOPI/10);
    sep->addChild(xform);
    
    mtl = new SoMaterial();
    mtl->diffuseColor.setValue( zLabelColor.getValue()[0], 
				zLabelColor.getValue()[1], 
				zLabelColor.getValue()[2] );
    mtl->transparency.setValue( zLabelColor.getValue()[3] );
    sep->addChild(mtl);

    for (val = minValue.getValue(); 
	 val <= maxValue.getValue(); 
	 val += zLabelIncrement.getValue())
    {
	SoText3 *text = new SoText3();
	char s[40];
	sprintf(s, "%hg", val);
	text->string = s;
	text->justification = SoText3::CENTER;
	text->parts = SoText3::ALL;
	sep->addChild(text);
	
	tr = new SoTranslation();
	tr->translation.setValue(0, (1.8/nzLabels) /
				    (zLabelScale.getValue()[0]*0.01), 0);
	sep->addChild(tr);
    }
}

void BarChart::generateBars(float barXSpace, float barYSpace, 
			    float barWidth, float barDepth)
{
    // first, figure out the tallest bar for scaling
    float maxbar = values[0];
    float minbar = minValue.getValue();
    int i = values.getNum();
    while (--i > 0)
	if (values[i] > maxbar) maxbar = values[i];

    if (maxbar > maxValue.getValue()) maxbar = maxValue.getValue();
    
    // bars
    SoSeparator *sep = new SoSeparator();
    children->append(sep);
    SoTranslation *tr = new SoTranslation();
    tr->translation.setValue(-0.9+barXSpace/2, 0.09, 0.9-barYSpace/2);
    sep->addChild(tr);
    
    int n = 0;
    int j = 0;
    
    // traversal in both directions will go 1.8 units, so
    // each movement will be 1.8 / ({x,y}Dim - 1)
    
    for ( ; ; )
    {
	SoSeparator *ysep = new SoSeparator();
	sep->addChild(ysep);
	
	if (n >= values.getNum()) break;
	float	curPlant = 0;
	int	i = 0;
	
	for ( ; ; )
	{ 
	    if (n >= values.getNum()) break;
	    
	    // translate in Y to plant on base plate
	    float curZ = (values[n]-minbar) / maxbar * 1.8;
	    
	    tr = new SoTranslation();
	    tr->translation.setValue(0, curPlant-(1-curZ/2), 0);
	    curPlant = (1-curZ/2);
	    ysep->addChild(tr);
	    SoMaterial *mtl = new SoMaterial();
	    int c = n % valueColors.getNum();
	    mtl->diffuseColor.setValue( valueColors[c][0], 
					valueColors[c][1], 
					valueColors[c][2] );
	    mtl->transparency.setValue( valueColors[c][3] );
	    ysep->addChild(mtl);

	    SoCube *bar = new SoCube();
	    bar->width = barWidth;
	    bar->depth = barDepth;
	    bar->height = curZ;
	    ysep->addChild(bar);
	    n++;
	    
	    if (++i >= xDimension.getValue()) break;
	    tr = new SoTranslation();
	    tr->translation.setValue(barXSpace, 0, 0);
	    ysep->addChild(tr);
	}
	if (++j >= yDimension.getValue()) break;
	tr = new SoTranslation();
	tr->translation.setValue(0, 0, -barYSpace);
	sep->addChild(tr);
    }
}
