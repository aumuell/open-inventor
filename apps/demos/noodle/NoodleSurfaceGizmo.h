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
|   Description:
|      A component class that creates a gizmo for editing 
|      orientation of normals, crease angle, and the 
|      minimum number of rows/columns in a noodle.
|
|   Author(s)          : Paul Isaacs
|
*/


#ifndef _NOODLE_SURFACE_GIZMO_
#define _NOODLE_SURFACE_GIZMO_

#include <Xm/Xm.h>
#include <Inventor/Xt/SoXtComponent.h>

class GeneralizedCylinder;
class NoodleSlider;

class NoodleSurfaceGizmo : public SoXtComponent {
  public:
    NoodleSurfaceGizmo( 
	Widget parent = NULL,
	const char *name = NULL, 
	SbBool buildInsideParent = TRUE);
    ~NoodleSurfaceGizmo();

     void setNoodle( GeneralizedCylinder *newNoodle );

  // internal:
      Widget buildWidget( Widget parent );

  private:
      Widget         _rowCol;
      Widget         flipNormButton;
      NoodleSlider  *creaseSlider;
      Widget         minNumRowsEdit;
      Widget         minNumColsEdit;

      GeneralizedCylinder *myNoodle;

      static void flipNormCallback( Widget, XtPointer, XtPointer);
      static void creaseCallback( void *, void *);
      static void minNumRowsCallback( Widget, XtPointer, XtPointer);
      static void minNumColsCallback( Widget, XtPointer, XtPointer);
};

#endif /* _NOODLE_SURFACE_GIZMO_ */
