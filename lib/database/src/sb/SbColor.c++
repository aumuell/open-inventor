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

/*
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.3 $
 |
 |   Classes:
 |	SbColor
 |
 |   Author(s)	: Alain Dumesny
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <math.h>
#include <Inventor/SbColor.h>

//
// Default constructor. Defined here to reduce warnings.
//

SbColor::SbColor()
{
}

//
// Sets value of vector from 3 individual hsv components
//
SbColor &
SbColor::setHSVValue(float hue, float sat, float val)
{
    float f,q,t,p;
    int i;
    
    if (hue == 1.0)
	hue = 0.0;
    else
	hue *= 6.0;
    i = (int)(floor(hue));
    f = hue-i;
    p = val*(1.0-sat);
    q = val*(1.0-(sat*f));
    t = val*(1.0-(sat*(1.0-f)));
    switch (i) {
	case 0: vec[0] = val; vec[1] = t; vec[2] = p; break;
	case 1: vec[0] = q; vec[1] = val; vec[2] = p; break;
	case 2: vec[0] = p; vec[1] = val; vec[2] = t; break;
	case 3: vec[0] = p; vec[1] = q; vec[2] = val; break;
	case 4: vec[0] = t; vec[1] = p; vec[2] = val; break;
	case 5: vec[0] = val; vec[1] = p; vec[2] = q; break;
    }

    return (*this);
}

//
// Returns 3 individual hsv components
//
void
SbColor::getHSVValue(float &hue, float &sat, float &val) const
{
    float max,min;
    
    max = (vec[0] > vec[1]) ? 
	((vec[0] > vec[2]) ? vec[0] : vec[2]) : 
	((vec[1] > vec[2]) ? vec[1] : vec[2]);
    min = (vec[0] < vec[1]) ? 
	((vec[0] < vec[2]) ? vec[0] : vec[2]) : 
	((vec[1] < vec[2]) ? vec[1] : vec[2]);
    
    // brightness
    val = max;
    
    // saturation
    if (max != 0.0) 
	sat = (max-min)/max;
    else
	sat = 0.0;
    
    // finally the hue
    if (sat  !=  0.0) {
    	float h;
	
	if (vec[0]  ==  max) 
	    h = (vec[1] - vec[2]) / (max-min);
	else if (vec[1]  ==  max)
	    h = 2.0 + (vec[2] - vec[0]) / (max-min);
	else
	    h = 4.0 + (vec[0] - vec[1]) / (max-min);
	if (h < 0.0)
	    h += 6.0;
	hue = h/6.0;
    }
    else
    	hue = 0.0;
}

//
// Set value of vector from rgba color
//
SbColor &
SbColor::setPackedValue(uint32_t orderedRGBA, float& transparency)
{
    float f = 1.0 / 255.0;
    vec[0] = ((orderedRGBA & 0xFF000000)>>24) * f;
    vec[1] = ((orderedRGBA & 0xFF0000) >> 16) * f;
    vec[2] = ((orderedRGBA & 0xFF00) >> 8) * f;
    transparency = 1.0 - (orderedRGBA & 0xFF) * f;
    
    return (*this);
}

//
// Returns orderedRGBA packed color format
//
uint32_t
SbColor::getPackedValue(float transparency) const
{
    return (
    	(((uint32_t) (vec[0] * 255)) << 24) +
	(((uint32_t) (vec[1] * 255)) << 16) +
	(((uint32_t) (vec[2] * 255)) << 8) +
	((uint32_t) ((1.0 - transparency) * 255)));
}
