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
// Header file for "BarChart" node.
//

#ifndef BARCHART_H
#define BARCHART_H

#include <Inventor/SbLinear.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoMFFloat.h>
#include <Inventor/fields/SoSFUShort.h>
#include <Inventor/fields/SoSFVec3f.h>
#include <Inventor/fields/SoSFVec4f.h>
#include <Inventor/fields/SoMFVec4f.h>
#include <Inventor/fields/SoSFEnum.h>
#include <Inventor/fields/SoMFString.h>
#include <Inventor/nodes/SoShape.h>
#include <Inventor/misc/SoChildList.h>

class BarChart : public SoShape {

    SO_NODE_HEADER(BarChart);

  public:

    enum BarStyle {		// Bar drawing style
	BAR			// individual bars
    };

    // Fields
    SoSFVec4f	plateColor;	// color of base plate
    SoSFVec4f	poleColor;	// color of corner poles
    SoMFFloat	values;		// array of data points (Y major)
    SoMFVec4f	valueColors;	// SoMaterial index to use for each data point
    SoMFString	xLabels;	// array of x axis labels
    SoMFVec4f	xLabelColors;	// SoMaterial index to use for x labels
    SoSFVec3f	xLabelScale;	// scale for x labels (1.0 is 0.1 units tall)
    SoMFString	yLabels;	// array of y axis labels
    SoMFVec4f	yLabelColors;	// SoMaterial index to use for y labels
    SoSFVec3f	yLabelScale;	// scale for y labels (1.0 is 0.1 units tall)
    SoSFUShort	xDimension;	// number of cells in x direction
    SoSFUShort	yDimension;	// number of cells in y direction
    SoSFFloat	xBarProportion;	// relative x size of a bar
				// (1.0 gives no space between bars)
    SoSFFloat	yBarProportion;	// relative y size of a bar
				// (1.0 gives no space between bars)
    SoSFFloat	minValue;	// value shown at base of chart
    SoSFFloat	maxValue;	// value shown at top of chart
    SoSFVec3f	zLabelScale;	// scale for z labels (1.0 is 0.1 units tall)
    SoSFVec4f	zLabelColor;	// color of z labels
    SoSFVec4f	zMarkerColor;	// color of z marker spheres
    SoSFFloat	zLabelIncrement;// increment of z label markings
    SoSFEnum	barStyle;	// drawing style of bars
    

    // Initializes this class
    static void		initClass();

    // Constructor
    BarChart();

  protected:
    // These implement supported actions
    virtual void	GLRender(SoGLRenderAction *action);
    virtual void	generatePrimitives(SoAction *action);
    virtual void	computeBBox(SoAction *action,
				    SbBox3f &box, SbVec3f &center);
    virtual SoChildList *getChildren() const;

  private:
    // Destructor
    virtual ~BarChart();

    SbBox3f getBbox() const;
    void generateChildren();
    void generateBars(float xSpacing, float ySpacing, float width, float height);

    SoChildList *children;
};

#endif
