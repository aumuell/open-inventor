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
* Copyright (C) 1990-93   Silicon Graphics, Inc.
*
_______________________________________________________________________
______________  S I L I C O N   G R A P H I C S   I N C .  ____________
|
|   $Revision: 1.1.1.1 $
|
|   Classes:
|      Extension of the Component base class used for all editing 
|      components.
|
|   Author(s)          : Paul Isaacs
|
______________  S I L I C O N   G R A P H I C S   I N C .  ____________
_______________________________________________________________________
*/

#include <inttypes.h>
#include <Inventor/Xt/SoXtSliderSetBase.h>

SoXtSliderSetBase::SoXtSliderSetBase(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent,
    SoNode *newEditNode)
	: SoXtComponent(
	    parent,
	    name, 
	    buildInsideParent)
{
    _layoutWidth = 0;
    _layoutHeight = 0;
    _editNode = newEditNode;
    if (_editNode != NULL) _editNode->ref();
    _numSubComponents = 0;
    _subComponentArray = NULL;
    widget = NULL;
}

SoXtSliderSetBase::~SoXtSliderSetBase()
{
    if (_editNode)
	_editNode->unref();

    for( int i = 0; i < _numSubComponents; i++ )
	delete _subComponentArray[i];

    if (_numSubComponents != 0)
	delete [] _subComponentArray;
}

void 
SoXtSliderSetBase::setNode( SoNode *newNode )
{
    if (newNode)            // ref before unref'ing the old
	newNode->ref();

    if (_editNode)          // out with the old
	_editNode->unref();

    _editNode = newNode;
    for( int i = 0; i < _numSubComponents; i++ )
	_subComponentArray[i]->setNode( newNode );
}

void 
SoXtSliderSetBase::show()
{
    setNode(_editNode); // insures that values are loaded into sliders
    SoXtComponent::show();
}

void 
SoXtSliderSetBase::getLayoutSize( int &w, int &h )
{ w = _layoutWidth; h = _layoutHeight; };
