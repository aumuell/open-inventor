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
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Description:
 |	Defines the SoCallbackAction class
 |
 |   Author(s)		: Dave Immel, Thad Beier
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_CALLBACK_ACTION_
#define  _SO_CALLBACK_ACTION_

#include <Inventor/SbBox.h>
#include <Inventor/actions/SoSubAction.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoLightModel.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoNormalBinding.h>
#include <Inventor/nodes/SoPickStyle.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/nodes/SoTextureCoordinateBinding.h>
#include <Inventor/nodes/SoUnits.h>

class SoPrimitiveVertex;
class SoShape;

//
// Typedefs for callback routines used with the callbacks for
// generating primitives.
//

typedef void SoTriangleCB(void *userData,
			  SoCallbackAction *action,
			  const SoPrimitiveVertex *v1,
			  const SoPrimitiveVertex *v2,
			  const SoPrimitiveVertex *v3);

typedef void SoLineSegmentCB(void *userData, SoCallbackAction *action,
			     const SoPrimitiveVertex *v1,
			     const SoPrimitiveVertex *v2);

typedef void SoPointCB(void *userData, SoCallbackAction *action,
		       const SoPrimitiveVertex *v);

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoCallbackAction
//
//  State accumulation action. This action performs a generic
//  traversal of a scene graph. The caller can register callback
//  functions to be called when specific types of nodes are traversed.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoCBAct
class SoCallbackAction : public SoAction {

    SO_ACTION_HEADER(SoCallbackAction);

  public:

    // Possible responses from a pre or post callback
    enum Response {
	CONTINUE,		// Continue as usual
	ABORT,			// Stop traversing the rest of the graph
	PRUNE			// Do not traverse children of this node
    };

    // The SoCallbackActionCB typedef is defined within the class, since
    // it needs to refer to the Response enumerated type.
    // The first argument is the data pointer that the user supplied
    // when the callback was registered. The second argument is the
    // action, from which the state can be extracted. The third
    // argument is the node that the callback is called from.  
    typedef Response SoCallbackActionCB(void *userData,
					SoCallbackAction *action,
					const SoNode *node);

    SoCallbackAction();

    // Destructor
    virtual ~SoCallbackAction();

    //////////////////////////////////////////////////////////////////
    //
    // Adding callback functions:
    //

    // These add a callback function to call when a node of the given
    // type is encountered during traversal. As the node is entered, the
    // PreCallback is called; as the node is finished, the PostCallback is
    // called.
    // C-api: name=addPreCB
    void addPreCallback(SoType type, SoCallbackActionCB *cb, void *data);
    // C-api: name=addPostCB
    void addPostCallback(SoType type, SoCallbackActionCB *cb, void *data);

    // These can be used when the action is applied to a path. They
    // add functions to be called when the tail node of the path is
    // traversed.
    // C-api: name=addPreTailCB
    void addPreTailCallback(SoCallbackActionCB *cb, void *data);
    // C-api: name=addPostTailCB
    void addPostTailCallback(SoCallbackActionCB *cb, void *data);

    // Routines to add callbacks for generated primitives (triangles,
    // line segments, and points) of a shape of a given type.
    // Primitives will be generated and these callbacks will be
    // invoked for all shape nodes of that type.
    // C-api: name=addTriCB
    void addTriangleCallback(SoType type, SoTriangleCB *cb, void *data);
    // C-api: name=addLineSegCB
    void addLineSegmentCallback(SoType type, SoLineSegmentCB *cb, void *data);
    // C-api: name=addPntCB
    void addPointCallback(SoType type, SoPointCB *cb, void *data);
    

    //////////////////////////////////////////////////////////////////
    //
    // Accessing state information from callbacks. Non-obvious return
    // values are documented.
    //

    //////////////////////// Complexity:

    // C-api: name=getCmplx
    float			getComplexity() const;
    // C-api: name=getCmplxType    
    SoComplexity::Type		getComplexityType() const;

    //////////////////////// Coordinates:

    // C-api: name=getNumCoord
    int32_t			getNumCoordinates() const;
    // C-api: name=getCoord3
    const SbVec3f &		getCoordinate3(int index) const;
    // C-api: name=getCoord4
    const SbVec4f &		getCoordinate4(int index) const;

    //////////////////////// DrawStyle:

    SoDrawStyle::Style		getDrawStyle() const;
    u_short			getLinePattern() const;
    float			getLineWidth() const;
    float			getPointSize() const;

    //////////////////////// Font:

    const SbName &		getFontName() const;
    float			getFontSize() const;

    //////////////////////// Lighting:

    // C-api: name=getLModel
    SoLightModel::Model		getLightModel() const;
    // C-api: name=getLtAtten
    const SbVec3f &		getLightAttenuation() const;

    //////////////////////// Material:

    // Returns material with given index
    // C-api: name=getMtl
    void			getMaterial(SbColor &ambient,
					    SbColor &diffuse,
					    SbColor &specular,
					    SbColor &emission,
					    float   &shininess,
					    float   &transparency,
					    int     mtlIndex = 0) const;
    // C-api: name=getMtlBind
    SoMaterialBinding::Binding	getMaterialBinding() const;

    //////////////////////// Normals:

    // C-api: name=getNumNorms
    int32_t			getNumNormals() const;
    // C-api: name=getNorms
    const SbVec3f &		getNormal(int index) const;
    // C-api: name=getNormBind
    SoNormalBinding::Binding	getNormalBinding() const;

    //////////////////////// Profiles:

    // C-api: name=getNumProfCoord
    int32_t			getNumProfileCoordinates() const;
    // C-api: name=getProfCoord2
    const SbVec2f &		getProfileCoordinate2(int index) const;
    // C-api: name=getProfCoord3
    const SbVec3f &		getProfileCoordinate3(int index) const;
    // C-api: name=getProf
    const SoNodeList &		getProfile() const;

    //////////////////////// Shape Hints:

    // C-api:name=getVertOrder
    SoShapeHints::VertexOrdering	getVertexOrdering() const;
    SoShapeHints::ShapeType		getShapeType() const;
    SoShapeHints::FaceType		getFaceType() const;
    float				getCreaseAngle() const;

    //////////////////////// Textures:

    // Returns 0 if texture coordinates are generated by a function
    // C-api: name=getNumTexCoord
    int32_t				getNumTextureCoordinates() const;
    // C-api: name=getTexCoord2
    const SbVec2f &			getTextureCoordinate2(int index) const;
    // C-api: name=getTexCoord4
    const SbVec4f &			getTextureCoordinate4(int index) const;
    // C-api: name=getTexCoordBind
    SoTextureCoordinateBinding::Binding	getTextureCoordinateBinding() const;
    // C-api: name=getTexBlendCol
    const SbColor &			getTextureBlendColor() const;
    // Returns NULL if no texture is enabled
    // C-api: name=getTexImage
    const unsigned char *		getTextureImage(SbVec2s &size,
							int &numComps) const;
    // C-api: name=getTexMx
    const SbMatrix &			getTextureMatrix() const;
    // C-api: name=getTexModel
    SoTexture2::Model	getTextureModel() const;
    // C-api: name=getTexWrapS
    SoTexture2::Wrap			getTextureWrapS() const;
    // C-api: name=getTexWrapT
    SoTexture2::Wrap			getTextureWrapT() const;

    //////////////////////// Transformations:

    // C-api: name=getModelMx
    const SbMatrix &		getModelMatrix() const;
    SoUnits::Units		getUnits() const;

    //////////////////////// Viewing:
    
    // C-api: name=getFocDist
    float			getFocalDistance() const;
    // C-api: name=getProjMx
    const SbMatrix &		getProjectionMatrix() const;
    // C-api: name=getViewMx
    const SbMatrix &		getViewingMatrix() const;
    // C-api: name=getViewVol
    const SbViewVolume &	getViewVolume() const;

    //////////////////////// Miscellaneous:

    SoPickStyle::Style		getPickStyle() const;
    int32_t			getSwitch() const;

  SoEXTENDER public:
    // Returns the current response
    Response		getCurrentResponse() const { return response; }

    // Call the pre-callbacks and post-callbacks
    void		invokePreCallbacks(const SoNode *node);
    void		invokePostCallbacks(const SoNode *node);

    // Call the primitive callbacks
    void	invokeTriangleCallbacks(const SoShape *shape,
					const SoPrimitiveVertex *v1,
					const SoPrimitiveVertex *v2,
					const SoPrimitiveVertex *v3);
    void	invokeLineSegmentCallbacks(const SoShape *shape,
					   const SoPrimitiveVertex *v1,
					   const SoPrimitiveVertex *v2);
    void	invokePointCallbacks(const SoShape *shape,
				     const SoPrimitiveVertex *v);

    // Should primitives be generated for this shape?
    SbBool	shouldGeneratePrimitives(const SoShape *shape) const;    
 
  SoINTERNAL public:
    static void		initClass();
    
    // Returns the node that caused the callback:
    virtual SoNode *	getCurPathTail();
    
    // Set the current node during traversal:
    void		setCurrentNode(SoNode *node)
	{ currentNode = node;} 

  protected:
    // Initiates action on graph
    virtual void	beginTraversal(SoNode *node);

  private:
    // Callback lists
    SbPList		preCallbackList;
    SbPList		postCallbackList;

    SbPList		preTailCallbackList;
    SbPList		postTailCallbackList;

    SbPList		triangleCallbackList;
    SbPList		lineSegmentCallbackList;
    SbPList		pointCallbackList;

    // Response from last callback
    Response		response;
    
    // Node where the callback occurs:
    SoNode *		currentNode;
};

#endif /* _SO_CALLBACK_ACTION_ */
