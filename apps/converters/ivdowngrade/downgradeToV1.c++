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
 * Copyright (C) 1993-94   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.3 $
 |
 |   Author(s) : David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <stdio.h>

#include <assert.h>
#include <Inventor/SbDict.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoInteraction.h>
#include <Inventor/SoInput.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/errors/SoReadError.h>
#include <Inventor/manips/SoCenterballManip.h>
#include <Inventor/manips/SoDirectionalLightManip.h>
#include <Inventor/manips/SoHandleBoxManip.h>
#include <Inventor/manips/SoJackManip.h>
#include <Inventor/manips/SoPointLightManip.h>
#include <Inventor/manips/SoSpotLightManip.h>
#include <Inventor/manips/SoTabBoxManip.h>
#include <Inventor/manips/SoTrackballManip.h>
#include <Inventor/manips/SoTransformBoxManip.h>
#include <Inventor/manips/SoTransformManip.h>
#include <Inventor/nodes/SoAnnotation.h>
#include <Inventor/nodes/SoAntiSquish.h>
#include <Inventor/nodes/SoAsciiText.h>
#include <Inventor/nodes/SoBlinker.h>
#include <Inventor/nodes/SoClipPlane.h>
#include <Inventor/nodes/SoColorIndex.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoEnvironment.h>
#include <Inventor/nodes/SoFile.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoFontStyle.h>
#include <Inventor/nodes/SoIndexedTriangleStripSet.h>
#include <Inventor/nodes/SoLabel.h>
#include <Inventor/nodes/SoLevelOfDetail.h>
#include <Inventor/nodes/SoLOD.h>
#include <Inventor/nodes/SoPendulum.h>
#include <Inventor/nodes/SoPickStyle.h>
#include <Inventor/nodes/SoPointLight.h>
#include <Inventor/nodes/SoRotor.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoShuttle.h>
#include <Inventor/nodes/SoSpotLight.h>
#include <Inventor/nodes/SoSurroundScale.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoText3.h>
#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/nodes/SoTextureCoordinateDefault.h>
#include <Inventor/nodes/SoTextureCoordinatePlane.h>
#include <Inventor/nodes/SoTransformSeparator.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoWWWAnchor.h>
#include <Inventor/nodes/SoWWWInline.h>

#include "SoEnvironmentV1.h"
#include "SoPickStyleV1.h"
#include "SoShapeHintsV1.h"
#include "SoTextureCoordinatePlaneV1.h"
#include "SoIndexedTriangleMeshV1.h"
#include "Util.h"
#include "SoAsciiTextV2.h"
#include "SoVertexPropertyV2.h"

SbDict *version2Only = NULL;
SbDict *needsDowngrading = NULL;

#define DICT_ENTER(dict,nodeClass,dictData) \
    (dict->enter((unsigned long) nodeClass::getClassTypeId().getName().getString(), (void *) dictData))
    
#define DICT_FIND(dict,node,dictData) \
    (dict->find((unsigned long) node->getTypeId().getName().getString(), dictData))

typedef SoNode *DowngradeFunc(SoNode *n);

// Extern
extern SoNode *downgradeVertexShape(SoVertexShape *vs);

// Forward reference.
SoNode *downgradeCopy(SoNode *node);


// We do not want the fields or the children of the Separator (v1.0 did not
// have fields, and the children we'll copy later)
SoNode *
downgradeSep(SoNode *)
{
    return new SoSeparator;
}

// We do not want the fields or the children of the Selection (v1.0 did not
// have fields, and the children we'll copy later)
SoNode *
downgradeSelection(SoNode *)
{
    return new SoSelection;
}

SoNode *
downgradeInline(SoNode *node2)
{
    SoGroup *group = new SoGroup;
    SoWWWInline *winline = (SoWWWInline *) node2;
    
    if (winline->getChildData() != NULL)
	group->addChild(downgradeCopy(winline->getChildData()));
	
    return group;
}

SoNode *
downgradeFontStyle(SoNode *node2)
{
    // Downgrade to SoFont
    SoFontStyle *fs = (SoFontStyle *) node2;
    SoFont *f = new SoFont;
    
    f->name = fs->getFontName();
    COPY_FIELD(f, fs, size); // f->size = fs->size
	
    return f;
}

SoNode *
downgradeAsciiText(SoNode *node2)
{
    return SoAsciiTextV2::convert((SoAsciiText *) node2);
}

SoNode *
downgradeVertexProperty(SoNode *node2)
{
    // Have to downgrade the group that gets returned 
    // since SoPackedColor (a field of VertexProperty) must get downgraded
    // after it is convert to the PackedColor node.
    SoGroup *g = SoVertexPropertyV2::convert((SoVertexProperty *) node2);
    return downgradeCopy(g);
}

// LOD and LevelOfDetail can be written as SoSwitch.
// Set whichChild to 0 to show highest detail.
SoNode *
downgradeSwitch(SoNode *)
{
    SoSwitch *sw = new SoSwitch;
    sw->whichChild = 0;
    return sw;
}

SoNode *
downgradeDrawStyle(SoNode *node2)
{
    // v1.0 does not have a pointSize field. Hide it.
    SoDrawStyle *ds = (SoDrawStyle *) node2;
    ds->pointSize.setDefault(TRUE);
    return ds;
}

SoNode *
downgradeLight(SoNode *node2)
{
    // v1.0 does not have a pointSize field. Hide it.
    SoLight *light = (SoLight *) node2;
    light->on.setDefault(TRUE);
    return light;
}

SoNode *
downgradeEnv(SoNode *node2)
{
    return SoEnvironmentV1::downgrade((SoEnvironment *) node2);
}

SoNode *
downgradePickStyle(SoNode *node2)
{
    return SoPickStyleV1::downgrade((SoPickStyle *) node2);
}

SoNode *
downgradeText3(SoNode *node2)
{
    SoText3 *t = (SoText3 *) node2;
    if (t->parts.isDefault())
	t->parts.setValue(SoText3::FRONT);  // 2.0 default means FRONT in 1.0
    return t;
}

SoNode *
downgradeShapeHints(SoNode *node2)
{
    return SoShapeHintsV1::downgrade((SoShapeHints *) node2);
}

SoNode *
downgradeTexture2(SoNode *node2)
{
    // v1.0 does not have an image field. Hide it.
    SoTexture2 *t = (SoTexture2 *) node2;
    t->image.setDefault(TRUE);
    return t;
}

SoNode *
downgradeTexCoordPlane(SoNode *node2)
{
    return SoTextureCoordinatePlaneV1::downgrade((SoTextureCoordinatePlane *) node2);
}

SoNode *
downgradeIdxTriStripSet(SoNode *node2)
{
    return SoIndexedTriangleMeshV1::downgrade((SoIndexedTriangleStripSet *) node2);
}

//
// Traverse the scene graph, converting from 2.0 to 1.0.
// This is where the work is done.
//
SoNode *
downgradeCopy(SoNode *node)
{
    SoNode *nodeCopy = NULL;
    SoField *altRep;
    void *dictData;
    
    // If not an Inventor built in node, do not try to copy.
    // Simply return a label.
    if (! node->getIsBuiltIn()) {
	// If there is an alternate representation, use it
	if ((altRep = node->getField("alternateRep")) != NULL &&
		 (altRep->isOfType(SoSFNode::getClassTypeId()))) {
	    nodeCopy = downgradeCopy(((SoSFNode *) altRep)->getValue());
	    nodeCopy->setName("alternateRep");
	}
	else {
	    SoLabel *l = new SoLabel;
	    l->label = node->getTypeId().getName();
	    nodeCopy = l;
	}
    }
    // If not a group, "copy" and return
    else if (! node->isOfType(SoGroup::getClassTypeId())) {
	// Is this a version 2.0 only node?
	if (DICT_FIND(version2Only, node, dictData) ) {
	    SoLabel *l = new SoLabel;
	    l->label = node->getTypeId().getName();
	    nodeCopy = l;
	}

	// Is this a nodekit?
	else if ( node->isOfType(SoBaseKit::getClassTypeId())) {
	    // [1] Create a vanilla group instead: 
	    SoGroup *grp = new SoGroup;
	    // [2] Copy the children,
	    SoChildList *kids = ((SoBaseKit *) node)->getChildren();
	    if (kids != NULL) {
		for (int i = 0; i < kids->getLength(); i++) 
		    grp->addChild( downgradeCopy( (*kids)[i] ) );
	    }
	    // [3] Add a first child indicating that this used to be a kit.
	    SoLabel *l = new SoLabel;
	    l->label = node->getTypeId().getName();
	    grp->insertChild(l,0);
	    // [4] assign grp to be the nodeCopy
	    nodeCopy = grp;
	}
	
	// Or does it need downgrading?
	else if (DICT_FIND(needsDowngrading, node, dictData)) {
	    // downgrade! (dictData was set in DICT_FIND)
	    DowngradeFunc *f = (DowngradeFunc *) dictData;
	    nodeCopy = (*f)(node);
	}
	
	// Is it a vertex shape? Need to remove the vertexProperty field if set.
	else if ( node->isOfType(SoVertexShape::getClassTypeId())) {
	    nodeCopy = downgradeVertexShape((SoVertexShape *) node);
	    
	    // If the shape was downgraded to something else, we have to convert
	    // that to 1.0 format.
	    if (nodeCopy != node)
		nodeCopy = downgradeCopy(nodeCopy);
	}
	
	// If not a group, simply use this node, first disconnecting fields
	// (We could copy it, but that can get expensive with large data sets.)
	else {
	    SoFieldList list;
	    int num = node->getFields(list);
	    for (int i = 0; i < num; i++) {
		if (list[i]->isConnected())
		    list[i]->disconnect();
	    }
	    nodeCopy = node;
	}
    }
    
    // For a group, copy the node, then traverse the children
    else {
	SoGroup *group2 = (SoGroup *) node; // version 2.0
	SoGroup *group1;		    // version 1.0
	
	// Is this a version 2.0 only node?
	if (DICT_FIND(version2Only, node, dictData)) {
	    // Treat it like a group
	    group1 = new SoGroup;
	}
	
	// Or does it need downgrading?
	else if (DICT_FIND(needsDowngrading, node, dictData)) {
	    // downgrade! (dictData was set in DICT_FIND)
	    DowngradeFunc *f = (DowngradeFunc *) dictData;
	    group1 = (SoGroup *)(*f)(node);
	}
	
	// Else use the group. We have to downgrade its children.
	else {
	    nodeCopy = group2;
	    for (int i = 0; i < group2->getNumChildren(); i++) {
		SoNode *n = downgradeCopy(group2->getChild(i));
		if (n != group2->getChild(i))
		    group2->replaceChild(i, n);
	    }
	    
	    return nodeCopy;
	}
	
	// Traverse to copy the children from group2 to group1
	for (int i = 0; i < group2->getNumChildren(); i++) 
	    group1->addChild(downgradeCopy(group2->getChild(i)));
	    
	nodeCopy = group1;
    }

    return nodeCopy;
}

//
// This routine searches for and expands all SoFile nodes in the
// given scene graph.  It does this by making all the children of a
// SoFile node the children of its parent.
// (Code for this function taken from ivcat)
//
void
expandFileNodes(SoNode *&root)
{
    //
    // Special case: if root is a file node, replace it with a group.
    //
    if (root->isOfType(SoFile::getClassTypeId())) {
	SoFile *f = (SoFile *)root;
	SoGroup *g = f->copyChildren();
	root->unref();
	root = g;
	root->ref();
    }

    // Search for all file nodes
    SoSearchAction sa;
    sa.setType(SoFile::getClassTypeId());
    sa.setInterest(SoSearchAction::FIRST);
    sa.setSearchingAll(TRUE);
    
    sa.apply(root);
    
    // We'll keep on searching until there are no more file nodes
    // left.  We don't search for all file nodes at once, because we
    // need to modify the scene graph, and so the paths returned may
    // be truncated (if there are several file nodes under a group, if
    // there are files within files, etc).  Dealing properly with that
    // is complicated-- it is easier (but slower) to just reapply the
    // search until it fails.
    // We need an SoFullPath here because we're searching node kit
    // contents.
    SoFullPath *p = (SoFullPath *) sa.getPath();
    while (p != NULL) {
	SoGroup *parent = (SoGroup *)p->getNodeFromTail(1);
	assert(parent != NULL);

	SoFile *file = (SoFile *)p->getTail();

	// If the filename includes a directory path, add the directory name 
	// to the list of directories where to look for input files 
	const char* filename = file->name.getValue().getString();
	const char *slashPtr;
	char *searchPath = NULL;
        if ((slashPtr = strrchr(filename, '/')) != NULL) {
            searchPath = strdup(filename);
            searchPath[slashPtr - filename] = '\0';
            SoInput::addDirectoryFirst(searchPath);
	}

	int fileIndex = p->getIndexFromTail(0);
	assert(fileIndex != -1);
	
	// Now, add group of all children to file's parent's list of children,
	// right after the file node:
        SoGroup *fileGroup = file->copyChildren();
	fileGroup->ref();
	if (fileGroup != NULL) {
	    parent->insertChild(fileGroup, fileIndex+1);
	}
	else {
	    // So we can at least see where the file node contents were
	    // supposed to go.
	    parent->insertChild(new SoGroup, fileIndex+1);
	}
	
	// And expand the child node from the group.
	// Note that if the File node is multiply instanced,
	// the groups will not be instanced, but the children of the
	// groups will be.
	parent->removeChild(fileIndex);

	sa.apply(root);
	p = (SoFullPath *) sa.getPath();
    }
}


////////////////////////////////////////////////////////////////////////
//
// Down-grade the passed scene to 1.0 format.
//
SoNode *
downgradeToV1(SoNode *n)
//
////////////////////////////////////////////////////////////////////////
{
    // "Copy" the scene graph from 2.0 to 1.0 format.
    // That is, traverse the graph reconstrucing a new one.
    // When a node is the same format in 1.0 and 2.0, we simply copy it.
    // When the format has changed, we build the 1.0 counterpart.
    
    // First, set up a dictionary of nodes that exist in 2.0,
    // but have no 1.0 counterpart.
    version2Only = new SbDict;

#define VERSION_2_DICT_ENTER(nodeClass) \
    DICT_ENTER(version2Only, nodeClass, NULL)
    
    VERSION_2_DICT_ENTER(SoAnnotation);
    VERSION_2_DICT_ENTER(SoAntiSquish);
    VERSION_2_DICT_ENTER(SoBlinker);
    VERSION_2_DICT_ENTER(SoClipPlane);
    VERSION_2_DICT_ENTER(SoColorIndex);
    VERSION_2_DICT_ENTER(SoPendulum);
    VERSION_2_DICT_ENTER(SoRotor);
    VERSION_2_DICT_ENTER(SoShuttle);
    VERSION_2_DICT_ENTER(SoSurroundScale);
    VERSION_2_DICT_ENTER(SoTextureCoordinateDefault);
    VERSION_2_DICT_ENTER(SoTransformSeparator);
    VERSION_2_DICT_ENTER(SoCenterballManip);
    VERSION_2_DICT_ENTER(SoDirectionalLightManip);
    VERSION_2_DICT_ENTER(SoHandleBoxManip);
    VERSION_2_DICT_ENTER(SoJackManip);
    VERSION_2_DICT_ENTER(SoPointLightManip);
    VERSION_2_DICT_ENTER(SoSpotLightManip);
    VERSION_2_DICT_ENTER(SoTabBoxManip);
    VERSION_2_DICT_ENTER(SoTrackballManip);
    VERSION_2_DICT_ENTER(SoTransformBoxManip);
    VERSION_2_DICT_ENTER(SoTransformManip);
    
    // And a dictionary of nodes that need downgrading
    needsDowngrading = new SbDict;

#define DOWNGRADE_DICT_ENTER(nodeClass, func) \
    DICT_ENTER(needsDowngrading, nodeClass, func)
    
    DOWNGRADE_DICT_ENTER(SoSelection,		    downgradeSelection);
    DOWNGRADE_DICT_ENTER(SoSeparator,		    downgradeSep);
    DOWNGRADE_DICT_ENTER(SoWWWAnchor,		    downgradeSep);
    DOWNGRADE_DICT_ENTER(SoWWWInline,		    downgradeInline);
    DOWNGRADE_DICT_ENTER(SoLevelOfDetail,	    downgradeSwitch);
    DOWNGRADE_DICT_ENTER(SoLOD,			    downgradeSwitch);
    DOWNGRADE_DICT_ENTER(SoDirectionalLight,	    downgradeLight);
    DOWNGRADE_DICT_ENTER(SoPointLight,		    downgradeLight);
    DOWNGRADE_DICT_ENTER(SoSpotLight,		    downgradeLight);
    DOWNGRADE_DICT_ENTER(SoDrawStyle,		    downgradeDrawStyle);
    DOWNGRADE_DICT_ENTER(SoEnvironment,		    downgradeEnv);
    DOWNGRADE_DICT_ENTER(SoPickStyle,		    downgradePickStyle);
    DOWNGRADE_DICT_ENTER(SoText3,		    downgradeText3);
    DOWNGRADE_DICT_ENTER(SoShapeHints,		    downgradeShapeHints);
    DOWNGRADE_DICT_ENTER(SoTexture2,		    downgradeTexture2);
    DOWNGRADE_DICT_ENTER(SoTextureCoordinatePlane,  downgradeTexCoordPlane);
    DOWNGRADE_DICT_ENTER(SoIndexedTriangleStripSet, downgradeIdxTriStripSet);
    DOWNGRADE_DICT_ENTER(SoFontStyle,		    downgradeFontStyle);
    DOWNGRADE_DICT_ENTER(SoAsciiText,		    downgradeAsciiText);
    DOWNGRADE_DICT_ENTER(SoVertexProperty,	    downgradeVertexProperty);
    
    // Traverse the graph
    return downgradeCopy(n);
}  
