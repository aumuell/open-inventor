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
 * Copyright (C) 1990,91,92   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Description:
 |	This file contains the implementation of the
 |      various composition and decomposition engines
 |
 |   Classes:
 |       SoComposeVec2f 
 |       SoComposeVec3f 
 |       SoComposeVec4f 
 |       SoComposeRotation 
 |       SoComposeRotationFromTo
 |       SoDecomposeVec2f 
 |       SoDecomposeVec3f 
 |       SoDecomposeVec4f 
 |       SoDecomposeRotation 
 |
 |   Author(s)		: Ronen Barzel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_COMPOSE_
#define  _SO_COMPOSE_

#include <Inventor/engines/SoSubEngine.h>
#include <Inventor/fields/SoMFFloat.h>
#include <Inventor/fields/SoMFMatrix.h>
#include <Inventor/fields/SoMFRotation.h>
#include <Inventor/fields/SoMFVec2f.h>
#include <Inventor/fields/SoMFVec3f.h>
#include <Inventor/fields/SoMFVec4f.h>

// C-api: end
// This internal macro defines the standard methods for the composition
// engines.  These are just basic engine declarations, nothing fancy. 
// The macro leaves the class header in "public:" state so it can be
// followed immediately with the composition input and output declarations.
#define SO_COMPOSE__HEADER(Name)		\
	    SO_ENGINE_HEADER(Name);		\
	  private:				\
	    ~Name();				\
	    virtual void evaluate();		\
	  public:				\
	    Name();				\
	    static void initClass()				
// C-api: begin

///////////////////////////////////////////////////////////
// 
//  Vector composition engines
// 
// C-api: prefix=SoCmpsV2f
// C-api: public=x,y,vector
class SoComposeVec2f : public SoEngine {
    SO_COMPOSE__HEADER(SoComposeVec2f);
    SoMFFloat		x;	// Input
    SoMFFloat		y;	// Input
    SoEngineOutput	vector;	// Output (SoMFVec2f)
};
// C-api: prefix=SoCmpsV3f
// C-api: public=x,y,z,vector
class SoComposeVec3f : public SoEngine {
    SO_COMPOSE__HEADER(SoComposeVec3f);
    SoMFFloat		x;	// Input
    SoMFFloat		y;	// Input
    SoMFFloat		z;	// Input
    SoEngineOutput	vector;	// Output (SoMFVec3f)
};
// C-api: prefix=SoCmpsV4f
// C-api: public=x,y,z,w,vector
class SoComposeVec4f : public SoEngine {
    SO_COMPOSE__HEADER(SoComposeVec4f);
    SoMFFloat		x;	// Input
    SoMFFloat		y;	// Input
    SoMFFloat		z;	// Input
    SoMFFloat		w;	// Input
    SoEngineOutput	vector;	// Output (SoMFVec4f)
};

///////////////////////////////////////////////////////////
// 
//  Vector decomposition engines
// 
// C-api: prefix=SoDecmpsV2f
// C-api: public=vector,x,y
class SoDecomposeVec2f : public SoEngine {
    SO_COMPOSE__HEADER(SoDecomposeVec2f);
    SoMFVec2f		vector;	// Input
    SoEngineOutput	x;	// Output (SoMFFloat)
    SoEngineOutput	y;	// Output (SoMFFloat)
};
// C-api: prefix=SoDecmpsV3f
// C-api: public=vector,x,y,z
class SoDecomposeVec3f : public SoEngine {
    SO_COMPOSE__HEADER(SoDecomposeVec3f);
    SoMFVec3f		vector;	// Input
    SoEngineOutput	x;	// Output (SoMFFloat)
    SoEngineOutput	y;	// Output (SoMFFloat)
    SoEngineOutput	z;	// Output (SoMFFloat)
};
// C-api: prefix=SoDecmpsV4f
// C-api: public=vector,x,y,z,w
class SoDecomposeVec4f : public SoEngine {
    SO_COMPOSE__HEADER(SoDecomposeVec4f);
    SoMFVec4f		vector;	// Input
    SoEngineOutput	x;	// Output (SoMFFloat)
    SoEngineOutput	y;	// Output (SoMFFloat)
    SoEngineOutput	z;	// Output (SoMFFloat)
    SoEngineOutput	w;	// Output (SoMFFloat)
};

///////////////////////////////////////////////////////////
// 
//  Rotation composition engines
// 
// C-api: prefix=SoCmpsRot
// C-api: public=axis,angle,rotation
class SoComposeRotation : public SoEngine {
    SO_COMPOSE__HEADER(SoComposeRotation);
    SoMFVec3f		axis;		// Input
    SoMFFloat		angle;		// Input
    SoEngineOutput	rotation;	// Output (SoMFRotation)
};
// C-api: prefix=SoCmpsRotFromTo
// C-api: public=from,to,rotation
class SoComposeRotationFromTo : public SoEngine {
    SO_COMPOSE__HEADER(SoComposeRotationFromTo);
    SoMFVec3f		from;		// Input
    SoMFVec3f		to;		// Input
    SoEngineOutput	rotation;	// Output (SoMFRotation)
};


///////////////////////////////////////////////////////////
// 
//  Rotation decomposition engine
// 
// C-api: prefix=SoDecmpsRot
// C-api: public=rotation,axis,angle
class SoDecomposeRotation : public SoEngine {
    SO_COMPOSE__HEADER(SoDecomposeRotation);
    SoMFRotation	rotation;	// Input
    SoEngineOutput	axis;		// Output (SoMFVec3f)
    SoEngineOutput	angle;		// Output (SoMFFloat)
};


///////////////////////////////////////////////////////////
// 
//  Matrix composition engine
// 
// C-api: prefix=SoCmpsMx
// C-api: public=translation,rotation,scaleFactor
// C-api: public=scaleOrientation,center,matrix
class SoComposeMatrix : public SoEngine {
    SO_COMPOSE__HEADER(SoComposeMatrix);
    SoMFVec3f		translation;	// Input
    SoMFRotation	rotation;	// Input
    SoMFVec3f		scaleFactor;	// Input
    SoMFRotation	scaleOrientation;//Input
    SoMFVec3f		center;		// Input
    SoEngineOutput	matrix;		// Output (SoMFMatrix)
};


///////////////////////////////////////////////////////////
// 
//  Matrix decomposition engine
//
//  Note: a matrix decomposition depends on the choice of
//  center. So a "center" input is provided (with default 
//  value 0,0,0) and there is no "center" output.
// 
// C-api: prefix=SoDecmpsMx
// C-api: public=marix,center,translation,rotation,scaleFactor
// C-api: public=scaleOrientation
class SoDecomposeMatrix : public SoEngine {
    SO_COMPOSE__HEADER(SoDecomposeMatrix);
    SoMFMatrix		matrix;		// Input
    SoMFVec3f		center;		// Input
    SoEngineOutput	translation;	// Output (SoMFVec3f)
    SoEngineOutput	rotation;	// Output (SoMFRotation)
    SoEngineOutput	scaleFactor;	// Output (SoMFVec3f)
    SoEngineOutput	scaleOrientation;//Output (SoMFRotation)
};

#endif  /* _SO_COMPOSE_ */
