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
 |   $Revision: 1.1 $
 |
 |   Classes:
 |      SoNurbsSurface
 |
 |   Author(s)          : Dave Immel, Thad Beier
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
#include <Inventor/SbBox.h>
#include <Inventor/SoDB.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/bundles/SoMaterialBundle.h>
#include <Inventor/bundles/SoTextureCoordinateBundle.h>
#include <Inventor/elements/SoCacheElement.h>
#include <Inventor/elements/SoComplexityElement.h>
#include <Inventor/elements/SoComplexityTypeElement.h>
#include <Inventor/elements/SoCoordinateElement.h>
#include <Inventor/elements/SoDrawStyleElement.h>
#include <Inventor/elements/SoGLTextureEnabledElement.h>
#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/elements/SoProjectionMatrixElement.h>
#include <Inventor/elements/SoProfileElement.h>
#include <Inventor/elements/SoShapeHintsElement.h>
#include <Inventor/elements/SoTextureCoordinateElement.h>
#include <Inventor/elements/SoViewingMatrixElement.h>
#include <Inventor/elements/SoViewportRegionElement.h>
#include <Inventor/nodes/SoNurbsSurface.h>
#include <Inventor/nodes/SoProfile.h>
#include "nurbs/SoAddPrefix.h"
#include "nurbs/SoPickRender.h"
#include "nurbs/SoGLRender.h"
#include "nurbs/SoPrimRender.h"

SO_NODE_SOURCE(SoNurbsSurface);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoNurbsSurface::SoNurbsSurface()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoNurbsSurface);
    SO_NODE_ADD_FIELD(numUControlPoints, (0));
    SO_NODE_ADD_FIELD(numVControlPoints, (0));
    SO_NODE_ADD_FIELD(numSControlPoints, (0));
    SO_NODE_ADD_FIELD(numTControlPoints, (0));
    SO_NODE_ADD_FIELD(uKnotVector, (0.0));
    SO_NODE_ADD_FIELD(vKnotVector, (0.0));
    SO_NODE_ADD_FIELD(sKnotVector, (0.0));
    SO_NODE_ADD_FIELD(tKnotVector, (0.0));

    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoNurbsSurface::~SoNurbsSurface()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Performs GL rendering of a NurbsSurface.
//
// Use: protected

void
SoNurbsSurface::GLRender(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // First see if the object is visible and should be rendered now
    if (! shouldGLRender(action))
	return;

    const SoCoordinateElement   *ce =
            SoCoordinateElement::getInstance(action->getState());
    GLfloat			*sKnots, *tKnots, *dstCoords;
    GLenum                      type;
    float                       *fKnots;
    int32_t                        nCoords, uOffset, vOffset;
    int32_t			nsKnots, ntKnots, nsCoords, ntCoords;
    int32_t			nDstCoords;
    int32_t                        sOffset, tOffset;
    int                         i, j;

    // Check for 0 control points
    nCoords = ce->getNum();
    if (nCoords == 0)
        return;
        
    // Make sure the first current material is sent to GL
    SoMaterialBundle	mb(action);
    mb.sendFirst();

    //
    // Find the number of steps required for object space tessellation and
    // the pixel tolerance used for screen space tessellation.
    //
    float val = SoComplexityElement::get(action->getState());
    if (val < 0.0) val = 0.0;
    if (val > 1.0) val = 1.0;
    int   steps;

    if      (val < 0.10) steps = 2;
    else if (val < 0.25) steps = 3;
    else if (val < 0.40) steps = 4;
    else if (val < 0.55) steps = 5;
    else                 steps = (int)(powf(val, 3.32)*28) + 2;

    float pixTolerance = 104.0*val*val - 252.0*val + 150;

    //
    // If the surface is being cached, or if the tessellation is in object
    // space, use the software NURBS library.  Create a software NURBS
    // rendering class and use it to make nurbs rendering calls.  Since
    // the software NURBS library generates triangles, texture mapping
    // will happen automatically without having to render a separate
    // texture surface.
    //
    if (SoComplexityTypeElement::get(action->getState()) ==
        SoComplexityTypeElement::OBJECT_SPACE)
    {
        _SoNurbsGLRender *GLRender = new _SoNurbsGLRender();

        //
        // Set the sampling to be constant across the surface with the
        // tessellation to be 'steps' across the S and T parameters
        //
        GLRender->setnurbsproperty( N_T2D,  N_SAMPLINGMETHOD,
                N_FIXEDRATE );
        GLRender->setnurbsproperty( N_V3D,  N_SAMPLINGMETHOD,
                N_FIXEDRATE );
        GLRender->setnurbsproperty( N_V3DR, N_SAMPLINGMETHOD,
                N_FIXEDRATE );
        GLRender->setnurbsproperty( N_T2D,  N_S_STEPS, steps);
        GLRender->setnurbsproperty( N_T2D,  N_T_STEPS, steps);
        GLRender->setnurbsproperty( N_V3D,  N_S_STEPS, steps);
        GLRender->setnurbsproperty( N_V3D,  N_T_STEPS, steps);
        GLRender->setnurbsproperty( N_V3DR, N_S_STEPS, steps);
        GLRender->setnurbsproperty( N_V3DR, N_T_STEPS, steps);

        // Determine whether a texture coordinate surface must be generated
        SbBool doTextures = SoGLTextureEnabledElement::get(action->getState());

        // Draw the surface
        drawNURBS (GLRender, action->getState(), doTextures);
        delete GLRender;

        return;
    }
    
    if (SoDrawStyleElement::get(action->getState()) ==
        SoDrawStyleElement::POINTS) {
        //
        // Render the control points of the surface.  Rendering the points
        // of the surface would be very slow, as the Software NURBS library
        // would have to be used, and because of the view dependent 
        // tessellation, points would not necessarily remain visible.
        //
        glBegin(GL_POINTS);
        if (ce->is3D()) {
            for (i=0; i<nCoords; i++) {
                const SbVec3f & coords3 = ce->get3((int)i);
                glVertex3f ((GLfloat)(coords3[0]),
                            (GLfloat)(coords3[1]),
                            (GLfloat)(coords3[2]));
            }
        }
        else {
            for (i=0; i<nCoords; i++) {
                const SbVec4f & coords4 = ce->get4((int)i);
                glVertex4f ((GLfloat)(coords4[0]),
                            (GLfloat)(coords4[1]),
                            (GLfloat)(coords4[2]),
                            (GLfloat)(coords4[3]));
            }
        }
        glEnd();
        return;
    }


    //
    // Render the NURBS surface using the GLU.
    //
    GLUnurbsObj *nurbsObj = gluNewNurbsRenderer();

    switch (SoDrawStyleElement::get(action->getState())) {
        case SoDrawStyleElement::FILLED:
            gluNurbsProperty (nurbsObj, (GLenum)GLU_DISPLAY_MODE, GLU_FILL);
            break;
        case SoDrawStyleElement::LINES:
            gluNurbsProperty (nurbsObj, (GLenum)GLU_DISPLAY_MODE, GLU_OUTLINE_POLYGON);
            break;
    }

    gluNurbsProperty (nurbsObj, (GLenum)GLU_SAMPLING_TOLERANCE, (GLfloat)pixTolerance);

    //
    // Collect the control points and knot vectors into an array suitable
    // for sending to the GL.  The control points and knot vectors must be
    // converted to double precision so that they can be passed to the
    // GL NURBS routines.
    //
    GLfloat *dCoords, *duKnots, *dvKnots;
    if (ce->is3D()) {
        dCoords = (GLfloat *)new GLfloat[3*nCoords];
        for (i=0; i<nCoords; i++) {
            const SbVec3f &c3 = ce->get3((int)i);
            dCoords[3*i]   = (GLfloat)c3[0];
            dCoords[3*i+1] = (GLfloat)c3[1];
            dCoords[3*i+2] = (GLfloat)c3[2];
        }
        uOffset = 3;
        type = GL_MAP2_VERTEX_3;
    }
    else {
        dCoords = (GLfloat *)new GLfloat[4*nCoords];
        for (i=0; i<nCoords; i++) {
            const SbVec4f &c4 = ce->get4((int)i);
            dCoords[4*i]   = (GLfloat)c4[0];
            dCoords[4*i+1] = (GLfloat)c4[1];
            dCoords[4*i+2] = (GLfloat)c4[2];
            dCoords[4*i+3] = (GLfloat)c4[3];
        }
        uOffset = 4;
        type = GL_MAP2_VERTEX_4;
    }
    vOffset = uOffset * numUControlPoints.getValue();

    fKnots = (float *)uKnotVector.getValues(0);
    duKnots = (GLfloat *)new GLfloat[uKnotVector.getNum()];
    for (i=0; i<uKnotVector.getNum(); i++)
        duKnots[i] = (GLfloat)fKnots[i];
    fKnots = (GLfloat *)vKnotVector.getValues(0);
    dvKnots = (GLfloat *)new GLfloat[vKnotVector.getNum()];
    for (i=0; i<vKnotVector.getNum(); i++)
        dvKnots[i] = (GLfloat)fKnots[i];

    // Texture mapping.  If doTextures == TRUE
    // we are drawing textures.  If the textureCoordinateBinding is
    // DEFAULT, we have to build a default NURBS surface for the texture
    // coordinates, otherwise we use the texture coordinates in the texture
    // element.
    // If there is a software texture function defined, then we have to
    // create a texture nurb surface with the same number of points and
    // knots as the original surface, and call the texture coordinate function
    // at each vertex.
    SbBool doTextures = SoGLTextureEnabledElement::get(action->getState());
    if(doTextures) {

	switch (SoTextureCoordinateElement::getType(action->getState())) {

          // software texture functions
	  case SoTextureCoordinateElement::FUNCTION:
          {
	    // generate S and T coords from U and V coords
	    SbVec3f	coord;
	    SbVec2f	stCoord;
	    int		offset;
            SoTextureCoordinateBundle tb(action, TRUE);

	    nsCoords = numUControlPoints.getValue();
	    ntCoords = numVControlPoints.getValue();

	    sKnots = duKnots;
	    tKnots = dvKnots;
	    nsKnots = uKnotVector.getNum();
	    ntKnots = vKnotVector.getNum();

	    nDstCoords = nsCoords * ntCoords;
	    dstCoords = (GLfloat *)new GLfloat[nDstCoords * 2];

	    for(int v = 0; v < ntCoords; v++) {
		for(int u = 0; u < nsCoords; u++) {
		    if (ce->is3D()) {
			offset = 3 * (v * (int)nsCoords + u);
			coord[0] = dCoords[offset + 0];
			coord[1] = dCoords[offset + 1];
			coord[2] = dCoords[offset + 2];
		    }
		    else {
			offset = 4 * (v * (int)nsCoords + u);
			coord[0] = dCoords[offset + 0] / dCoords[offset + 3];
			coord[1] = dCoords[offset + 1] / dCoords[offset + 3];
			coord[2] = dCoords[offset + 2] / dCoords[offset + 3];
		    }
                    const SbVec4f &tc = tb.get(coord, SbVec3f(0.0, 1.0, 0.0));
		    dstCoords[(v * (int)nsCoords  + u) * 2 + 0] = tc[0];
		    dstCoords[(v * (int)nsCoords  + u) * 2 + 1] = tc[1];
		}
	    }
            break;
  	  }

	  // texture coordinates defined from texture node
	case SoTextureCoordinateElement::EXPLICIT:
	  // get texture coordinates from texture node
	  const SoTextureCoordinateElement *te = 
	      SoTextureCoordinateElement::getInstance(action->getState());
	  int32_t		nstCoords = te->getNum();
	  if (nstCoords < 1) {
	      // Default texture coordinates are computed by defining
	      // a bezier surface that is defined in the same valid
	      // parameter space as the geometric surface.  The valid
	      // parameter space is defined based on the order and knot
	      // vector.  The coordinates go from 0 to one and the knot
	      // vectors span the valid range of the geometric surface.
	      // The knot vectors default to 0 and 1 in the event of bogus
	      // input data.
	      int uOrder, vOrder;
	      GLfloat sKnotVal1, sKnotVal2, tKnotVal1, tKnotVal2;

	      uOrder = uKnotVector.getNum() - numUControlPoints.getValue();
	      vOrder = vKnotVector.getNum() - numVControlPoints.getValue();
	      if ((uOrder > 0) && (uOrder < uKnotVector.getNum()))
		  sKnotVal1 = duKnots[uOrder-1];
	      else
		  sKnotVal1 = 0;
	      if ((uOrder > 0) && (uOrder < uKnotVector.getNum()))
		  sKnotVal2 = duKnots[uKnotVector.getNum()-uOrder];
	      else
		  sKnotVal2 = 1;
	      if ((vOrder > 0) && (vOrder < vKnotVector.getNum()))
		  tKnotVal1 = dvKnots[vOrder-1];
	      else
		  tKnotVal1 = 0;
	      if ((vOrder > 0) && (vOrder < vKnotVector.getNum()))
		  tKnotVal2 = dvKnots[vKnotVector.getNum()-vOrder];
	      else
		  tKnotVal2 = 1;

	      // do a linear 2x2 array
	      nsKnots = 4;
	      ntKnots = 4;

	      sKnots = (GLfloat *)new GLfloat[4];
	      tKnots = (GLfloat *)new GLfloat[4];
		
	      sKnots[0] = sKnots[1] = sKnotVal1;
	      tKnots[0] = tKnots[1] = tKnotVal1;
	      sKnots[2] = sKnots[3] = sKnotVal2;
	      tKnots[2] = tKnots[3] = tKnotVal2;

	      // allocate a 2 x 2 array of GLfloat[2]'s
	      nsCoords = 2;
	      ntCoords = 2;
	      nDstCoords = nsCoords * ntCoords * 2;
	      dstCoords = (GLfloat *)new GLfloat[nDstCoords];
	      for(i = 0; i < 2; i++) {
		  for(j = 0; j < 2; j++) {
		      dstCoords[(i * 2 + j) * 2 + 0] = j;
		      dstCoords[(i * 2 + j) * 2 + 1] = i;
		  }
	      } 
	  }
	  else {
	      // get knot vectors from this node
	      nsKnots = sKnotVector.getNum();
	      fKnots = (float *)sKnotVector.getValues(0);
	      sKnots = (GLfloat *)new GLfloat[nsKnots];
	      for (i=0; i < nsKnots; i++)
		  sKnots[i] = (GLfloat)fKnots[i];

	      ntKnots = tKnotVector.getNum();
	      fKnots = (float *)tKnotVector.getValues(0);
	      tKnots = (GLfloat *)new GLfloat[ntKnots];
	      for (i=0; i < ntKnots; i++)
		  tKnots[i] = (GLfloat)fKnots[i];

	      nsCoords = numSControlPoints.getValue();
	      ntCoords = numTControlPoints.getValue();

	      nDstCoords = 2 * nstCoords;
	      dstCoords = (GLfloat *)new GLfloat[nDstCoords];
	      for(i = 0; i < nstCoords; i++) {
		  const SbVec2f &tc2 = te->get2(i);
		  dstCoords[2*i]   = (GLfloat)tc2[0];
		  dstCoords[2*i+1] = (GLfloat)tc2[1];
	      }
	  }
	  break;
      }
	sOffset = 2;
	tOffset = sOffset * nsCoords;
    }


    //
    // Draw the NURBS surface.  Begin the surface.  Then load the texture
    // map as a nurbs surface.  Then, draw the geometric surface followed
    // by all of its trim curves.  Then, end the surface.
    //
    glEnable(GL_AUTO_NORMAL);

    // Get one camera based element so that this node will be registered
    // with the cache.  If the camera changes, this element will cause
    // the cache to be blown for this node and the nurbs surface will be
    // regenerated.
    SbMatrix vMat = SoViewingMatrixElement::get (action->getState());
    SbMatrix mMat = SoModelMatrixElement::get (action->getState());

    // Begin the surface.
    gluBeginSurface(nurbsObj);

    // Draw the texture surface
    if(doTextures) {
	// send down nurbs surface, then free memory
	gluNurbsSurface(nurbsObj, (GLint)nsKnots, sKnots,
                     (GLint)ntKnots, tKnots,
		     (GLint)sOffset, (GLint)tOffset, dstCoords,
		     (GLint)(nsKnots - nsCoords),
                     (GLint)(ntKnots - ntCoords),
                     GL_MAP2_TEXTURE_COORD_2);

	// delete knots if not sharing them with the surface description
	// (in the case of software texture coordinates only)
	if(sKnots != duKnots) {
	    delete [] sKnots;
	    delete [] tKnots;
	}
	delete [] dstCoords;
    }

    gluNurbsSurface (nurbsObj, (GLint)(uKnotVector.getNum()), duKnots,
                    (GLint)(vKnotVector.getNum()), dvKnots,
                    (GLint)uOffset, (GLint)vOffset, dCoords,
                    (GLint)(uKnotVector.getNum() -
                        numUControlPoints.getValue()),
                    (GLint)(vKnotVector.getNum() -
                        numVControlPoints.getValue()),
                    type);

    //
    // Get all of the trim curves and use them to trim the surface.
    //
    SoProfile        *profile;
    const SoNodeList &trimNodes = SoProfileElement::get(action->getState());
    SbBool           haveTrim = FALSE;
    float            *trimCoords, *trimKnots;
    int32_t             numTrimCoords, numKnots, offset;
    int              numTrims = trimNodes.getLength();
    int              floatsPerVec;

    //
    // For each trim curve, check its linkage to find out if it should be
    // continued on to the previous trim curve or if it should begin a
    // new trim curve.  Then, send the trim to the NURBS library.
    //
    for (i=0; i<numTrims; i++)
    {
        GLfloat *dTrimCoords;
        GLfloat *dtmp;
        float   *ftmp;

        // Get the trim curve.
        profile = (SoProfile *)trimNodes[(int) i];
    	profile->getTrimCurve (action->getState(), numTrimCoords,
                               trimCoords, floatsPerVec,
                               numKnots, trimKnots);

        // Check for degenerate trim curves
        if (numTrimCoords == 0)
            continue;

        // Check the linkage.
        if ((profile->linkage.getValue() == SoProfileElement::START_FIRST) ||
            (profile->linkage.getValue() == SoProfileElement::START_NEW))
        {
            if (haveTrim)
               gluEndTrim(nurbsObj);
            gluBeginTrim(nurbsObj);
            haveTrim = TRUE;
        }

        // Set the data type of the control points to non-rational or rational
        if (floatsPerVec == 2)
            type = (GLenum)GLU_MAP1_TRIM_2;
        else
            type = (GLenum)GLU_MAP1_TRIM_3;
        offset = floatsPerVec;

        dTrimCoords = new GLfloat[numTrimCoords*floatsPerVec];
        dtmp = dTrimCoords;
        ftmp = trimCoords;
        for (j=0; j<floatsPerVec*numTrimCoords; j++)
            *dtmp++ = (GLfloat)(*ftmp++);
            
        if (numKnots == 0)
        {
            // Send down a Piecewise Linear Trim Curve
            gluPwlCurve (nurbsObj, (GLint)numTrimCoords, dTrimCoords,
                    (GLint)offset, type);
         }
        else
        {
            // Send down a NURBS Trim Curve
            GLfloat *dTrimKnots = new GLfloat[numKnots];

            dtmp = dTrimKnots;
            ftmp = trimKnots;
            for (j=0; j<numKnots; j++)
                *dtmp++ = (GLfloat)(*ftmp++);
            
            gluNurbsCurve (nurbsObj, (GLint)numKnots, dTrimKnots,
                    (GLint)offset, dTrimCoords,
                    (GLint)(numKnots - numTrimCoords), type);
            delete[] dTrimKnots;
            delete[] trimKnots;
        }
        delete[] dTrimCoords;
        delete[] trimCoords;
    }
    if (haveTrim)
        gluEndTrim(nurbsObj);

    gluEndSurface(nurbsObj);
    gluDeleteNurbsRenderer(nurbsObj);
    glDisable(GL_AUTO_NORMAL);

    delete[] dvKnots;
    delete[] duKnots;
    delete[] dCoords;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements ray picking
//
// Use: protected

void
SoNurbsSurface::rayPick(SoRayPickAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // First see if the object is pickable
    if (! shouldRayPick(action))
	return;

    // Tell the action about our current object space
    action->setObjectSpace();

    //
    // Create an SoPickRender class which performs picking with the
    // software NURBS library.
    //
    _SoNurbsPickRender pickRender(action);

    //
    // Set NURBS properties telling the NURBS library to pick filled
    // triangles, use screen space tessellation with the pixel tolerance very
    // low.  Notify the library that the sampling and culling matrices will be
    // passed in.
    //
    pickRender.setnurbsproperty (N_DISPLAY, N_FILL);
    pickRender.setnurbsproperty (N_T2D,  N_PIXEL_TOLERANCE, 2.0 );
    pickRender.setnurbsproperty (N_V3D,  N_PIXEL_TOLERANCE, 2.0 );
    pickRender.setnurbsproperty (N_V3DR, N_PIXEL_TOLERANCE, 2.0 );
    pickRender.setnurbsproperty (N_T2D,N_SAMPLINGMETHOD, N_PARAMETRICDISTANCE);
    pickRender.setnurbsproperty (N_V3D,N_SAMPLINGMETHOD, N_PARAMETRICDISTANCE);
    pickRender.setnurbsproperty (N_V3DR,N_SAMPLINGMETHOD,N_PARAMETRICDISTANCE);
    pickRender.setnurbsproperty (N_V3D,  N_CULLING, N_CULLINGON);
    pickRender.setnurbsproperty (N_V3DR, N_CULLING, N_CULLINGON);

    //
    // Calculate the total viewing matrix by concatenating the modeling
    // matrix, the camera's viewing matrix, and the projection matrix.
    // Pass the resulting matrix to the NURBS library for use in determining
    // sampling and culling of the surface.
    //
    const SbViewportRegion & vpRegion =
            SoViewportRegionElement::get(action->getState());
    const SbVec2s & vpSize = vpRegion.getViewportSizePixels();
    SbMatrix totalMat;
    calcTotalMatrix(action->getState(), totalMat);
    pickRender.loadMatrices(totalMat, vpSize);

    // Determine whether a texture coordinate surface must be generated
    SbBool generateTexCoords = TRUE;
    if (SoTextureCoordinateElement::getType(action->getState()) ==
        SoTextureCoordinateElement::FUNCTION)
        generateTexCoords = FALSE;

    //
    // Draw the NURBS surface.  The SoPickRender class will receive primitive
    // drawn by the NURBS library and test them for intersection.
    //
    drawNURBS(&pickRender, action->getState(), generateTexCoords);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Generates triangles representing the NURBS surface.
//
// Use: protected

void
SoNurbsSurface::generatePrimitives(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    _SoNurbsPrimRender   primRender(action, primCB, (void *)this);  

    //
    // Find the number of steps required for tessellation and the pixel
    // tolerance used for tessellation.
    //
    float val = SoComplexityElement::get(action->getState());
    if (val < 0.0) val = 0.0;
    if (val > 1.0) val = 1.0;

    float pixTolerance = 104.0*val*val - 252.0*val + 150;
    int   steps;

    if      (val < 0.10) steps = 2;
    else if (val < 0.25) steps = 3;
    else if (val < 0.40) steps = 4;
    else if (val < 0.55) steps = 5;
    else                 steps = (int)(powf(val, 3.32)*28) + 2;

    if (SoComplexityTypeElement::get(action->getState()) ==
        SoComplexityTypeElement::OBJECT_SPACE)
    {
        //
        // Set the sampling to be constant across the surface with the
        // tessellation to be 'steps' across the S and T parameters
        //
        primRender.setnurbsproperty( N_T2D,  N_SAMPLINGMETHOD,
                N_FIXEDRATE );
        primRender.setnurbsproperty( N_V3D,  N_SAMPLINGMETHOD,
                N_FIXEDRATE );
        primRender.setnurbsproperty( N_V3DR, N_SAMPLINGMETHOD,
                N_FIXEDRATE );
        primRender.setnurbsproperty( N_T2D,  N_S_STEPS, steps);
        primRender.setnurbsproperty( N_T2D,  N_T_STEPS, steps);
        primRender.setnurbsproperty( N_V3D,  N_S_STEPS, steps);
        primRender.setnurbsproperty( N_V3D,  N_T_STEPS, steps);
        primRender.setnurbsproperty( N_V3DR, N_S_STEPS, steps);
        primRender.setnurbsproperty( N_V3DR, N_T_STEPS, steps);
    }
    else
    {
        //
        // Set the sampling to be view dependent with the given
        // pixel tolerance.
        //
        primRender.setnurbsproperty( N_T2D,  N_SAMPLINGMETHOD,
                N_PARAMETRICDISTANCE );
        primRender.setnurbsproperty( N_V3D,  N_SAMPLINGMETHOD,
                N_PARAMETRICDISTANCE );
        primRender.setnurbsproperty( N_V3DR, N_SAMPLINGMETHOD,
                N_PARAMETRICDISTANCE );
        primRender.setnurbsproperty( N_T2D,  N_PIXEL_TOLERANCE,
                pixTolerance );
        primRender.setnurbsproperty( N_V3D,  N_PIXEL_TOLERANCE,
                pixTolerance );
        primRender.setnurbsproperty( N_V3DR, N_PIXEL_TOLERANCE,
                pixTolerance );
    }

    //
    // Calculate the total viewing matrix by concatenating the modeling
    // matrix, the camera's viewing matrix, and the projection matrix.
    // Pass the resulting matrix to the NURBS library for use in
    // determining sampling and culling of the surface.
    //
    const SbViewportRegion & vpRegion =
            SoViewportRegionElement::get(action->getState());
    const SbVec2s & vpSize = vpRegion.getViewportSizePixels();
    SbMatrix totalMat;
    calcTotalMatrix (action->getState(), totalMat);
    primRender.loadMatrices (totalMat, vpSize);

    // Determine whether a texture coordinate surface must be generated
    SbBool generateTexCoords = TRUE;
    if (SoTextureCoordinateElement::getType(action->getState()) ==
        SoTextureCoordinateElement::FUNCTION)
        generateTexCoords = FALSE;

    //
    // Draw the NURBS surface.  The SoPrimRender class will receive primitive
    // drawn by the NURBS library, triangulate them, and send them to the
    // callback.
    drawNURBS(&primRender, action->getState(), generateTexCoords);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Gets details back from the NURBS library and sends them to the
//    generate primitive callbacks.
//
// Use: private, static

void
SoNurbsSurface::primCB(
    SoAction *act,
    SoPrimitiveVertex *vertices,
    void *userData)

//
////////////////////////////////////////////////////////////////////////
{
    SoNurbsSurface *surf = (SoNurbsSurface *)userData;
    surf->sendPrimitive(act, vertices);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sends primitives to the generate primitive callbacks.
//
// Use: internal, public

void
SoNurbsSurface::sendPrimitive(
    SoAction *act,
    SoPrimitiveVertex *vertices )

//
////////////////////////////////////////////////////////////////////////
{
    invokeTriangleCallbacks(act, &vertices[0], &vertices[1], &vertices[2]);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Computes bounding box for the NURBS surface. The center of the
//    surface is defined as the average of all control point
//    coordinates used for the surface.
//
// Use: extender

void
SoNurbsSurface::computeBBox(SoAction *action, SbBox3f &box, SbVec3f &center)
//
////////////////////////////////////////////////////////////////////////
{
    const SoCoordinateElement   *ce =
            SoCoordinateElement::getInstance(action->getState());
    int32_t			nCoords, numSurfCoords;
    int				j, curCoord;
    SbVec3f			tmpCoord;

    //
    // Loop through coordinates, keeping max bounding box and sum of coords
    // If the coordinates are rational, divide the first three values by
    // the fourth value before extending the bounding box.
    //
    numSurfCoords = numUControlPoints.getValue() *
                    numVControlPoints.getValue();
    nCoords = ce->getNum();

    // Check for a degenerate surface
    if ((numSurfCoords == 0) || (nCoords == 0))
        return;

    curCoord = 0;
    center.setValue(0.0, 0.0, 0.0);
    if (ce->is3D()) {
        for (j = 0; j < numSurfCoords; j++) {
            //
            // Wrap around if necessary
            //
            if (curCoord >= nCoords)
	        curCoord = 0;
            const SbVec3f &coord = ce->get3(curCoord);
            box.extendBy(coord);
            center += coord;
            curCoord++;
        }
    }
    else {
        for (j = 0; j < numSurfCoords; j++) {
            //
            // Wrap around if necessary
            //
            if (curCoord >= nCoords)
	        curCoord = 0;
            const SbVec4f &coord = ce->get4(curCoord);
            coord.getReal (tmpCoord);
            box.extendBy (tmpCoord);
            center += tmpCoord;
            curCoord++;
        }
    }

    center /= (float) numSurfCoords;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Draws the NURBS surface.
//
// Use: protected

void
SoNurbsSurface::drawNURBS(
    _SoNurbsNurbsTessellator *render,
    SoState *state,
    SbBool doTextures )

//
////////////////////////////////////////////////////////////////////////
{
    const SoCoordinateElement   *ce =
            SoCoordinateElement::getInstance(state);
    const SoTextureCoordinateElement *te =
            SoTextureCoordinateElement::getInstance(state);
    int32_t                        uOffset, vOffset, nsKnots, ntKnots;
    int32_t			nstCoords, nsCoords, ntCoords;
    int32_t                        type, nCoords;
    float                       *coords;
    float			*sKnots = NULL, *tKnots = NULL, *stCoords;
    SbBool			newKnots = FALSE, newSTCoords = FALSE;

    nCoords = ce->getNum();

    // Check for a degenerate surface
    if (nCoords == 0)
        return;

    //
    // Draw the NURBS surface.  Then, draw each trim curve.
    //

    if (ce->is3D()) {
        coords = (float *)new float[3*nCoords];
        for (int i=0; i<nCoords; i++) {
            const SbVec3f &c3 = ce->get3((int)i);
            coords[3*i]   = c3[0];
            coords[3*i+1] = c3[1];
            coords[3*i+2] = c3[2];
        }
        uOffset = 3 * sizeof(float);
        type = N_V3D;
    }
    else {
        coords = (float *)new float[4*nCoords];
        for (int i=0; i<nCoords; i++) {
            const SbVec4f &c4 = ce->get4((int)i);
            coords[4*i]   = c4[0];
            coords[4*i+1] = c4[1];
            coords[4*i+2] = c4[2];
            coords[4*i+3] = c4[3];
        }
        uOffset = 4 * sizeof(float);
        type = N_V3DR;
    }
    vOffset = uOffset * numUControlPoints.getValue();

    // Texture mapping.  If doTextures == TRUE
    // we are drawing textures.  If the textureCoordinateBinding is
    // DEFAULT, we have to build a default NURBS surface for the texture
    // coordinates, otherwise we use the texture coordinates in the texture
    // element.
    // If there is a software texture function defined, then we have to
    // create a texture nurb surface with the same number of points and
    // knots as the original surface, and call the texture coordinate function
    // at each vertex.
     if(doTextures) {

	switch (SoTextureCoordinateElement::getType(state)) {

          // software texture functions
	  case SoTextureCoordinateElement::FUNCTION:
          {
	    // generate S and T coords from U and V coords
	    SbVec3f	coord;
	    SbVec2f	stCoord;
	    int		offset;

	    nsCoords = numUControlPoints.getValue();
	    ntCoords = numVControlPoints.getValue();

	    sKnots = (float *) uKnotVector.getValues(0),
	    tKnots = (float *) vKnotVector.getValues(0),
	    nsKnots = uKnotVector.getNum();
	    ntKnots = vKnotVector.getNum();

	    nstCoords = nsCoords * ntCoords;
	    stCoords = (float *)new float[nstCoords * 2];
	    newSTCoords = TRUE;

	    for(int v = 0; v < ntCoords; v++) {
		for(int u = 0; u < nsCoords; u++) {
		    if (ce->is3D()) {
			offset = 3 * (v * (int)nsCoords + u);
			coord[0] = coords[offset + 0];
			coord[1] = coords[offset + 1];
			coord[2] = coords[offset + 2];
		    }
		    else {
			offset = 4 * (v * (int)nsCoords + u);
			coord[0] = coords[offset + 0] / coords[offset + 3];
			coord[1] = coords[offset + 1] / coords[offset + 3];
			coord[2] = coords[offset + 2] / coords[offset + 3];
		    }
                    const SbVec4f &tc = te->get(coord, SbVec3f(0.0, 1.0, 0.0));
		    stCoords[(v * (int)nsCoords  + u) * 2 + 0] = tc[0];
		    stCoords[(v * (int)nsCoords  + u) * 2 + 1] = tc[1];
		}
	    }
            break;
	  }

  	  // texture coordinates defined from texture node
	case SoTextureCoordinateElement::EXPLICIT:
	  nstCoords = te->getNum();
	  if (nstCoords < 1) {
            // Default texture coordinates are computed by defining
            // a bezier surface that is defined in the same valid
            // parameter space as the geometric surface.  The valid
            // parameter space is defined based on the order and knot
            // vector.  The coordinates go from 0 to one and the knot
            // vectors span the valid range of the geometric surface.
            // The knot vectors default to 0 and 1 in the event of bogus
            // input data.
            int uOrder, vOrder;
            float sKnotVal1, sKnotVal2, tKnotVal1, tKnotVal2;

            uOrder = uKnotVector.getNum() - numUControlPoints.getValue();
            vOrder = vKnotVector.getNum() - numVControlPoints.getValue();
            if ((uOrder > 0) && (uOrder < uKnotVector.getNum()))
                sKnotVal1 = uKnotVector[uOrder-1];
            else
                sKnotVal1 = 0;
            if ((uOrder > 0) && (uOrder < uKnotVector.getNum()))
                sKnotVal2 = uKnotVector[uKnotVector.getNum()-uOrder];
            else
                sKnotVal2 = 1;
            if ((vOrder > 0) && (vOrder < vKnotVector.getNum()))
                tKnotVal1 = vKnotVector[vOrder-1];
            else
                tKnotVal1 = 0;
            if ((vOrder > 0) && (vOrder < vKnotVector.getNum()))
                tKnotVal2 = vKnotVector[vKnotVector.getNum()-vOrder];
            else
                tKnotVal2 = 1;

	    // do a linear 2x2 array
	    nsKnots = 4;
	    ntKnots = 4;

	    sKnots = (float *)new float[4];
	    tKnots = (float *)new float[4];
	    newKnots = TRUE;

	    sKnots[0] = sKnots[1] = sKnotVal1;
	    tKnots[0] = tKnots[1] = tKnotVal1;
	    sKnots[2] = sKnots[3] = sKnotVal2;
	    tKnots[2] = tKnots[3] = tKnotVal2;

	    // allocate a 2 x 2 array of float[2]'s
	    nsCoords = 2;
	    ntCoords = 2;
	    nstCoords = nsCoords * ntCoords;
	    stCoords = (float *)new float[nstCoords * 2];
	    newSTCoords = TRUE;
	    for(int i = 0; i < 2; i++) {
		for(int j = 0; j < 2; j++) {
		    stCoords[(i * 2 + j) * 2 + 0] = j;
		    stCoords[(i * 2 + j) * 2 + 1] = i;
		}
	    }
	  } else {
	    // get texture coordinates from texture node
	    // get knot vectors from this node
	    stCoords = (float *)new float[nstCoords * 2];
	    newSTCoords = TRUE;
	    for(int i = 0; i < nstCoords; i++) {
                const SbVec2f &tc2 = te->get2(i);
	        stCoords[2*i]   = tc2[0];
	        stCoords[2*i+1] = tc2[1];
            }
	    nstCoords *= 2;

	    nsKnots = sKnotVector.getNum();
	    ntKnots = tKnotVector.getNum();
	    sKnots = (float *) sKnotVector.getValues(0),
	    tKnots = (float *) tKnotVector.getValues(0),

	    nsCoords = numSControlPoints.getValue();
	    ntCoords = numTControlPoints.getValue();
	  }
	  break;
      }
    }

    //
    // Render the NURBS surface as if to the GL, but use the given
    // NurbsTessellator instead which will make calls to the
    // software NURBS library.
    //
    render->bgnsurface(0);

    // Draw the texture surface
    if(doTextures) {

        // send down nurbs surface, then free memory
        render->nurbssurface(nsKnots, (INREAL *)sKnots, ntKnots,
                             (INREAL *)tKnots,
                     	     2 * sizeof(float), 2 * sizeof(float) * nsCoords,
			     (INREAL *)stCoords, nsKnots - nsCoords,
			     ntKnots - ntCoords, N_T2D);

        // delete knots if not sharing them with the surface description
        // (in the case of software texture coordinates only)
        if(newKnots) {
            delete [] sKnots;
            delete [] tKnots;
        }
	if(newSTCoords)
	    delete [] stCoords;
    }

    render->nurbssurface (uKnotVector.getNum(),
                    (INREAL *) uKnotVector.getValues(0),
                    vKnotVector.getNum(),
                    (INREAL *) vKnotVector.getValues(0),
                    uOffset, vOffset, (INREAL *)coords,
                    uKnotVector.getNum() - numUControlPoints.getValue(),
                    vKnotVector.getNum() - numVControlPoints.getValue(),
                    type);

    //
    // Get all of the trim curves and use them to trim the surface.
    //
    const SoNodeList &trimNodes = SoProfileElement::get(state);
    SoProfile       *profile;
    SbBool          haveTrim = FALSE;
    float           *trimCoords, *trimKnots;
    int32_t            numTrimCoords, numKnots, offset;
    int             numTrims = trimNodes.getLength();
    int             floatsPerVec, i;

    //
    // For each trim curve, check its linkage to find out if it should be
    // continued on to the previous trim curve or if it should begin a
    // new trim curve.  Then, send the trim to the NURBS library.
    //
    for (i=0; i<numTrims; i++)
    {
        // Get the trim curve.
        profile = (SoProfile *)trimNodes[i];
    	profile->getTrimCurve (state, numTrimCoords,
                               trimCoords, floatsPerVec,
                               numKnots, trimKnots);

        // Ignore degenerate trim curves
        if (numTrimCoords == 0)
            continue;

        // Check the linkage.
        if ((profile->linkage.getValue() == SoProfileElement::START_FIRST) ||
            (profile->linkage.getValue() == SoProfileElement::START_NEW))
        {
            // Begin a new trim curve.
            if (haveTrim) {
               render->endtrim();
            }
            render->bgntrim();
            haveTrim = TRUE;
        }

        // Set the data type of the control points to non-rational or rational
        if (floatsPerVec == 2)
            type = N_P2D;
        else
            type = N_P2DR;
        offset = floatsPerVec * sizeof(float);

        if (numKnots == 0)
            // Send down a Piecewise Linear Trim Curve
            render->pwlcurve (numTrimCoords, (INREAL *)trimCoords,
                              offset, type);
        else
            // Send down a NURBS Trim Curve
            render->nurbscurve (numKnots, (INREAL *)trimKnots, offset,
                                (INREAL *)trimCoords,
                                numKnots - numTrimCoords, type);
        delete[] trimCoords;
        if (numKnots != 0)
            delete[] trimKnots;
    }
    if (haveTrim)
        render->endtrim();

    render->endsurface();
    delete coords;
}



////////////////////////////////////////////////////////////////////////
//
// Description:
//    Calculate the total transformation matrix by contatenating the
//    modeling matrix, the camera's viewing matrix, and the projection
//    matrix all together.
//
// Use: protected

void
SoNurbsSurface::calcTotalMatrix(
    SoState *state,
    SbMatrix &totalMat )

//
////////////////////////////////////////////////////////////////////////
{
    SbMatrix mMat, pMat, vMat;
    SbMatrix viewMat;

    mMat = SoModelMatrixElement::get (state);
    pMat = SoProjectionMatrixElement::get (state);
    vMat = SoViewingMatrixElement::get (state);
    multMatrix4d (viewMat, mMat, vMat);
    multMatrix4d (totalMat, viewMat, pMat);
}



////////////////////////////////////////////////////////////////////////
//
// Description:
//    Multiply two matrices together.
//    new = [left][right]
//
// Use: protected

void
SoNurbsSurface::multMatrix4d (
   SbMatrix &n,
   SbMatrix left,
   SbMatrix right )

//
////////////////////////////////////////////////////////////////////////
{
    int i;

    for (i=0; i<4; i++)
    {
        n[i][0] = left[i][0]*right[0][0] + left[i][1]*right[1][0] +
                  left[i][2]*right[2][0] + left[i][3]*right[3][0];
        n[i][1] = left[i][0]*right[0][1] + left[i][1]*right[1][1] +
                  left[i][2]*right[2][1] + left[i][3]*right[3][1];
        n[i][2] = left[i][0]*right[0][2] + left[i][1]*right[1][2] +
                  left[i][2]*right[2][2] + left[i][3]*right[3][2];
        n[i][3] = left[i][0]*right[0][3] + left[i][1]*right[1][3] +
                  left[i][2]*right[2][3] + left[i][3]*right[3][3];
    }
}
