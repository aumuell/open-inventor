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

/////////////////////////////////////////////////////////////////////////////
//
// IfShape class: this holds the state relevant to a particular shape
// found in a scene graph.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef  _IF_SHAPE_
#define  _IF_SHAPE_

#include <Inventor/SbColor.h>
#include <Inventor/SbLinear.h>

class SoCamera;
class SoComplexity;
class SoCoordinate3;
class SoDrawStyle;
class SoEnvironment;
class SoFont;
class SoGroup;
class SoLightModel;
class SoMaterial;
class SoMaterialBinding;
class SoNode;
class SoNodeList;
class SoNormal;
class SoNormalBinding;
class SoProfileCoordinate2;
class SoShapeHints;
class SoTexture2;
class SoTextureCoordinate2;
class SoTextureCoordinateBinding;

class IfShape {

  public:
    IfShape();
    ~IfShape();

    //////////////////////////////////////////////////////////////////
    //
    // These codes indicate the highest-level difference found when
    // comparing 2 shapes.
    //

    enum DifferenceCode {
	// No difference:
	NONE,

	// Level 1:
	CAMERA,

	// Level 2:
	LIGHTS,
	CLIP_PLANES,
	ENVIRONMENT,
	LIGHT_MODEL,

	// Level 3:
	TEXTURE,

	// Level 4:
	DRAW_STYLE,
	SHAPE_HINTS,
	OTHER,
	MATERIAL,

	// Level 5:
	COMPLEXITY,
	COORDS,
	FONT,
	MATERIAL_BINDING,
	NORMALS,
	NORMAL_BINDING,
	PROFILE_COORDS,
	PROFILES,
	TEX_COORDS,
	TEX_COORD_BINDING,

	// IfShape:
	SHAPE
    };

    //////////////////////////////////////////////////////////////////
    //
    // This is the shape itself. (Since we have to treat some types of
    // groups as if they were shapes, we can't assume it's derived
    // from SoShape.)
    //

    SoNode		*shape;

    //////////////////////////////////////////////////////////////////
    //
    // Level 1 nodes: This level contains only the camera, which is
    // handled specially because it is usually moving around when
    // viewing a scene. Separating the camera from the rest of the
    // global properties allows caching to be more efficient.
    //

    SoCamera		*camera;

    //////////////////////////////////////////////////////////////////
    //
    // Level 2 nodes: These nodes are used to partition the scene
    // graph at the top level. They are not always found in input
    // scenes, so this level may not be used. When they are used, they
    // typically apply to most or all of the scene graph, so using
    // them as a partition basis is not too expensive.
    //

    SoNodeList		*lights;	// List of pointers to SoLights
    SoNodeList		*clipPlanes;	// List of pointers to SoClipPlane
    SoEnvironment	*environment;
    SoLightModel	*lightModel;

    //////////////////////////////////////////////////////////////////
    //
    // Level 3 nodes: These nodes are used as a second partitioning
    // level because of the relatively high expense of switching
    // between them during rendering.
    //

    SoTexture2		*texture;

    //////////////////////////////////////////////////////////////////
    //
    // Level 4 nodes: These nodes all have an effect during rendering
    // of shapes. Therefore, they need to be preserved for attaching to
    // shapes after they have been flattened. Unlike level 2 and 3
    // nodes, these do not cause any new scene graph levels to be
    // created during the partitioning, but they are used as sorting
    // factors that affect the partitioning.
    //

    SoDrawStyle		*drawStyle;
    SoShapeHints	*shapeHints;
    SoMaterial		*material;
    SoNodeList		*other;		// Any other nodes go here

    //////////////////////////////////////////////////////////////////
    //
    // Level 5 nodes: These nodes are all used during rendering, but
    // can be ignored after flattening: either flattening produces its
    // own version of them or they are no longer relevant afterwards.
    //

    SoComplexity		*complexity;
    SoCoordinate3		*coords;
    SoFont			*font;
    SoMaterialBinding		*materialBinding;
    SoNormal			*normals;
    SoNormalBinding		*normalBinding;
    SoProfileCoordinate2	*profileCoords;
    SoNodeList			*profiles;	// List of ptrs to SoProfiles
    SoTextureCoordinate2	*texCoords;
    SoTextureCoordinateBinding	*texCoordBinding;

    //////////////////////////////////////////////////////////////////
    //
    // Since transformations accumulate, it is easier to just store
    // them as a matrix. However, there is no way to store the texture
    // matrix as a matrix, so we just save a list of transforms.
    // NOTE: If we are producing texture coordinates, the
    // textureTransforms are used to multiply out those coordinates
    // and are therefore not preserved. If we are NOT producing
    // texture coordinates, we need to save and output these
    // transforms, so they are added to the Level 4 "other" nodes in
    // that case.
    //

    SbMatrix		transform;
    SoNodeList		*textureTransforms;

    //////////////////////////////////////////////////////////////////
    //
    // These are used in a list of shapes to indicate how 2 adjacent
    // shapes differ. They contain the level at which the shapes
    // differ (or 0 if there are no significant differences) and a
    // specific difference code.

    int			differenceLevel;
    DifferenceCode	differenceCode;

    // This is used to indicate that the shape should not be flattened
    // because it really is an SoFile or SoLevelOfDetail or something
    // like that
    SbBool		dontFlatten;

    //////////////////////////////////////////////////////////////////

    // This compares two IfShape instances and returns the first level
    // (1-5) at which they differ and a comparison code (-1, 0, or +1)
    // to indicate the result, and a more specific difference code.
    static int		compare(const IfShape *s1, const IfShape *s2,
				int &level, DifferenceCode &diffCode);

    // Compares two node lists for equality
    static int		compareLists(const SoNodeList *l1,
				     const SoNodeList *l2);

    // Compares two floating point numbers for equality
    static int		compareFloats(float f1, float f2);

    // Compares two colors for equality
    static int		compareColors(const SbColor &c1, const SbColor &c2);

    // Adds the appropriate nodes for the given level to the given
    // group node
    void		addNodesForLevel(SoGroup *group, int level);

    // Unref's non-NULL node pointers and sets them all to NULL
    void		clearNodes();
};

#endif /* _IF_SHAPE_ */
