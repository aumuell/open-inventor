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
 |   Classes:
 |	WorldInfo
 |
 |   Author(s): Paul Isaacs
 |
 */


#include <stdio.h>
#include <Inventor/SbLinear.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoLists.h>
#include <Inventor/SbViewportRegion.h>
#include <Inventor/SoPickedPoint.h>

#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/manips/SoHandleBoxManip.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/errors/SoDebugError.h>

#include "WorldInfo.h"
#include "GeneralizedCylinder.h"

WorldInfo::WorldInfo()
{
    // Make worldRoot
	worldRoot = new SoSeparator;
	worldRoot->ref();

    // Add selection node.

	selector = new SoSelection;
	worldRoot->addChild(selector);
	selector->addSelectionCallback( &WorldInfo::selectionCB, this );
	selector->addDeselectionCallback( &WorldInfo::deselectionCB, this );
	selector->setPickFilterCallback( &WorldInfo::pickFilterCB, this );

    // Put sceneRoot under selector
	sceneRoot = new SoSeparator;
	selector->addChild( sceneRoot );

    // Create the deletedNoodles list. We store them so they can be un-deleted.
	deletedNoodles = new SoNodeList;
	
    // This piece will be dragged in z whenever the activePlane moves.
        currentNoodle = NULL;

    // Initialize other variables.
	fileName = NULL;

	manipType = SoTransform::getClassTypeId();
}

WorldInfo::~WorldInfo()
{
    if ( worldRoot ) worldRoot->unref();
    if ( deletedNoodles ) delete deletedNoodles;
}

/////////////////////////////////////////////////////////////////////
// Set the type of manipulator being used to move selected noodles.
// Then install one in each selected noodle.
//
void    
WorldInfo::setManipType( SoType newType )
{ 
    if ( newType == manipType)
	return;

    if ( ! newType.isDerivedFrom( SoTransform::getClassTypeId() ))
	return;

    manipType = newType; 

    // Get all selections; replace their transforms with the new type.
    const SoPathList *pl = selector->getList();
    for (int i = 0; i < selector->getNumSelected(); i++ ) {

	SoNode *t = (*pl)[i]->getTail();
	if (t->isOfType(GeneralizedCylinder::getClassTypeId()))
	    ((GeneralizedCylinder *)t)->changeTransformType(newType);
    }
}

/////////////////////////////////////////////////////////////////////
// Returns a copy of the scene with all noodles replaced by subgraphs
// of standard inventor nodes.
SoSeparator *
WorldInfo::getVanillaSceneCopy()
{
    if (sceneRoot == NULL)
	return NULL;

    SoSeparator *myCopy = (SoSeparator *) sceneRoot->copy();
    myCopy->ref();

    // Replace every GeneralizedCylinder  in the copy with just the 
    // data that draws, removing the nodekit.
    SoSearchAction *sa = new SoSearchAction;
    sa->setType( GeneralizedCylinder::getClassTypeId() );
    sa->setInterest( SoSearchAction::ALL );
    sa->apply( myCopy );
    SoPathList *genCylPaths = &(sa->getPaths());
    SoFullPath *myPath;
    SoNode              *myParent;
    GeneralizedCylinder *gc;
    SoGroup             *parentGroup;

    for ( int i = 0; i < genCylPaths->getLength(); i++ ) {
	myPath = (SoFullPath *) (*genCylPaths)[i];
	myParent = myPath->getNode( myPath->getLength() - 2 );
	gc = (GeneralizedCylinder *) myPath->getTail();
	if ( myParent->isOfType( SoGroup::getClassTypeId() ) == FALSE ) {
	    fprintf(stderr,"Error converting scene to vanilla. Found a \n");
	    fprintf(stderr,"non-group parent of the generalized cyliinder\n");
	    break;
	}
	parentGroup = (SoGroup *) myParent;
	SoSeparator *vanillaObject = gc->makeVanillaVersion();
	parentGroup->replaceChild( gc, vanillaObject );
    }

    // Delete the search action BEFORE unrefNodelete. This takes
    // count down to 1.
    delete sa;

    // Take count from 1 to 0
    myCopy->unrefNoDelete();

    return myCopy;
}

void 
WorldInfo::setScene( SoSeparator *newScene )
{
    if (newScene == sceneRoot)
	return;

    // Deselect everything.
    selector->deselectAll();

    // Always have at least an empty separator in the scene.
    if (newScene == NULL)
	newScene = new SoSeparator;

    if (sceneRoot == NULL) {
#ifdef DEBUG
	SoDebugError::post("WorldInfo::setScene", "old sceneRoot is NULL");
#endif
	selector->insertChild( newScene, 0 );
    }
    else
        selector->replaceChild( sceneRoot, newScene );

    sceneRoot = newScene;

    // Now, select the first noodle in the scene
    setFirstNoodleCurrent();
}

SbBool
WorldInfo::isSceneEmpty()
{
    if (sceneRoot == NULL) {
#ifdef DEBUG
	SoDebugError::post("WorldInfo::setScene", "old sceneRoot is NULL");
#endif
	return TRUE;
    }
    else 
	return ( sceneRoot->getNumChildren() == 0);
}

void 
WorldInfo::setFileName( char *newFileName )
{
    if (newFileName == NULL)
	fileName = NULL;
    else
	fileName = strdup( newFileName );
}

void 
WorldInfo::addNoodle( GeneralizedCylinder *newNoodle )
{
    if (newNoodle == NULL)
	return;
    sceneRoot->addChild( newNoodle );
}

void 
WorldInfo::deleteNoodle( GeneralizedCylinder *victim )
{
    // See if victim is in the selection list...
    // We'd prefer to delete based on selection path, if possible, since we
    // can then deselect.
    SoFullPath *victimPath = NULL;
    for (int i = 0; i < selector->getNumSelected(); i++ ) {
	if ( ((SoNodeKitPath *)selector->getPath(i))->getTail() == victim ) {
	    victimPath = (SoFullPath *) selector->getPath(i);
	    victimPath->ref();
	    break;
	}
    }
    if (victimPath) {
        // deselect old one if it is selected...
	selector->deselect(victimPath);
    }
    else {
        // If it wasn't a selection, get a path to it...
	SoSearchAction sa;
	sa.setNode( victim );
	sa.setInterest( SoSearchAction::FIRST );
        SbBool wasSearchingKits = SoBaseKit::isSearchingChildren();
        SoBaseKit::setSearchingChildren(TRUE);
	sa.apply( selector );
        SoBaseKit::setSearchingChildren(wasSearchingKits);
	victimPath = (SoFullPath *) sa.getPath();
	if (victimPath)
	    victimPath->ref();
    }

    if ( ! victimPath )
	return;

    // Remove piece from parent.
    if (victimPath != NULL) {
	// Get index of victim in the path...
	int objInd;
	for (objInd = victimPath->getLength() - 1; objInd >= 0; objInd --) {
	    if ( victimPath->getNode(objInd) == victim )
		break;
	}
	SoNode *parent = NULL;
	SoType bkt = SoBaseKit::getClassTypeId();
	// Remove victim from its parent.
	// For parent, if there's a nodekit above victim, use that first kit.
	    for (int pInd = objInd - 1; pInd >= 0; pInd --) {
		if (victimPath->getNode(pInd)->isOfType( bkt ) ) {
		    parent = victimPath->getNode(pInd);
		    SoBaseKit *k = (SoBaseKit *)parent;
		    k->setPart( k->getPartString(victim), NULL );
		    break;
		}
	    }
	// Otherwise, use the first node above victim.
	    if ( parent == NULL ) {
		parent = victimPath->getNodeFromTail( 1 );
		if (parent->isOfType( SoGroup::getClassTypeId() ))
		    ((SoGroup *)parent)->removeChild( victim );
	    }

    }
    victimPath->unref();
}


void 
WorldInfo::deleteCurrentNoodle()
{
    if (currentNoodle == NULL)
	return;

    currentNoodle->ref();
    deleteNoodle( currentNoodle );
    deletedNoodles->append( currentNoodle );
    currentNoodle->unref();
}

GeneralizedCylinder * 
WorldInfo::undeleteNoodle()
{
    int num = deletedNoodles->getLength();
    if ( num <= 0)
	return NULL;

    GeneralizedCylinder *theNoodle 
	= (GeneralizedCylinder *) (*deletedNoodles)[num - 1];

    theNoodle->ref();

    addNoodle( theNoodle );

    // Since we're undeleting the noodle, make it the only selection.
    selector->deselectAll();
    selector->select( theNoodle );

    setCurrentNoodle( theNoodle );

    theNoodle->unrefNoDelete();

    deletedNoodles->remove( num - 1);

    return theNoodle;
}

GeneralizedCylinder *
WorldInfo::setFirstNoodleCurrent()
{
    if (sceneRoot == NULL)
	return NULL;

    SoSearchAction sa;
    sa.setType( GeneralizedCylinder::getClassTypeId() );
    sa.apply( sceneRoot );
    SoPath *pathToFirst = sa.getPath();
    if (pathToFirst == NULL)
	return NULL;

    GeneralizedCylinder *first = (GeneralizedCylinder *) pathToFirst->getTail();

    // Since we're undeleting the noodle, make it the only selection.
    selector->deselectAll();
    selector->select( first );

    setCurrentNoodle( first );

    return first;
}

GeneralizedCylinder * 
WorldInfo::addNewNoodle()
{
    GeneralizedCylinder *newOne = new GeneralizedCylinder;
    newOne->ref();

    addNoodle( newOne );

    // Since we're adding a new noodle, make it the only selection.
    selector->deselectAll();
    selector->select( newOne );

    setCurrentNoodle( newOne );
    newOne->unrefNoDelete();

    return newOne;
}

void
WorldInfo::setCurrentNoodle( GeneralizedCylinder *newNoodle )
{
    currentNoodle = newNoodle;

    // Add a manipulator if necessary...
    if (currentNoodle)
	currentNoodle->changeTransformType( getManipType() );
}

void
WorldInfo::selectionCB(void *userData, SoPath *selectPath)
{
    // Return if pickFilter truncated path down to nothing.
    if (selectPath->getLength() == 0)
	return;

    WorldInfo *myself = (WorldInfo *) userData;

    SoNodeKitPath *nkPath = (SoNodeKitPath *) selectPath;
    GeneralizedCylinder *g = (GeneralizedCylinder *) nkPath->getTail();

    myself->setCurrentNoodle( g );
}

void
WorldInfo::deselectionCB(void *, SoPath *deselectPath)
{
    if (deselectPath->getLength() == 0)
	return;

    SoNodeKitPath *nkPath = (SoNodeKitPath *) deselectPath;
    GeneralizedCylinder *g = (GeneralizedCylinder *) nkPath->getTail();

    g->changeTransformType( SoTransform::getClassTypeId() );
}

SoPath *
WorldInfo::pickFilterCB(void *, const SoPickedPoint *pick )
{
    SoFullPath *fullP = (SoFullPath *) pick->getPath();

    // Go up the path, and keep popping nodes until the tail is a
    // Generalized Cylinder kit...
    // If we hit the selection node first, then just stop right there...
    while ( fullP->getLength() > 1 ) {

	// If we're at a noodle, stop:
	if ( fullP->getTail()->isOfType(
			GeneralizedCylinder::getClassTypeId()))
	    return fullP;

	// If we're at the selection node, stop.
	if ( fullP->getTail()->isOfType( SoSelection::getClassTypeId()))
	    return fullP;

	// Otherwise, pop off the end of the path.
	fullP->pop();
    }

    // If we got here, the path emptied out.
    return NULL;
}
