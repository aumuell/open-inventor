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

/*--------------------------------------------------------------
 *  This is an example from the Inventor Toolmaker,
 *  chapter 6, example 1.
 *
 *  Header file for "MultFloatVec3f" engine.
 *------------------------------------------------------------*/

#include <Inventor/engines/SoSubEngine.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFVec3f.h>

class MultFloatVec3f : public SoEngine {

   SO_ENGINE_HEADER(MultFloatVec3f);

 public:

   // Input fields: a scalar (float) and a vector
   SoSFFloat       scalar;
   SoSFVec3f       vector;

   // The output is a vector
   SoEngineOutput  product;  // (SoSFVec3f) product

   // Initializes this class for use in scene graphs. This
   // should be called after database initialization and before
   // any instance of this engine is constructed.
   static void initClass();

   // Constructor
   MultFloatVec3f();

 private:
   // Destructor. Since engines are never deleted explicitly,
   // this can be private.
   virtual ~MultFloatVec3f();

   // Evaluation method
   virtual void evaluate();
};
