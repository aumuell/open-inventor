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
 |	SoInteractionKit
 |
 |   Author(s): Paul Isaacs, David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


#include <stdio.h>
#include <stdlib.h>
#include <Inventor/SbDict.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/SoNodeKitPath.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/nodekits/SoInteractionKit.h>
#include <Inventor/nodekits/SoNodeKitListPart.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/nodes/SoLabel.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>

SO_KIT_SOURCE(SoInteractionKit);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor

SoInteractionKit::SoInteractionKit()
//
////////////////////////////////////////////////////////////////////////
{
    SO_KIT_CONSTRUCTOR(SoInteractionKit);

    isBuiltIn = TRUE;

    // Add a separator to the catalog.
    SO_KIT_ADD_CATALOG_ENTRY(topSeparator, SoSeparator, TRUE, this, ,FALSE);
    SO_KIT_ADD_CATALOG_ENTRY(geomSeparator,SoSeparator, 
				TRUE, topSeparator, ,FALSE);

    SO_KIT_ADD_FIELD(renderCaching,            (AUTO));
    SO_KIT_ADD_FIELD(boundingBoxCaching,       (AUTO));
    SO_KIT_ADD_FIELD(renderCulling,            (AUTO));
    SO_KIT_ADD_FIELD(pickCulling,              (AUTO));

    // Set up static info for enum fields
    SO_KIT_DEFINE_ENUM_VALUE(CacheEnabled, OFF);
    SO_KIT_DEFINE_ENUM_VALUE(CacheEnabled, ON);
    SO_KIT_DEFINE_ENUM_VALUE(CacheEnabled, AUTO);

    // Set up info in enumerated type fields
    SO_KIT_SET_SF_ENUM_TYPE(renderCaching,     CacheEnabled);
    SO_KIT_SET_SF_ENUM_TYPE(boundingBoxCaching,CacheEnabled);
    SO_KIT_SET_SF_ENUM_TYPE(renderCulling,     CacheEnabled);
    SO_KIT_SET_SF_ENUM_TYPE(pickCulling,       CacheEnabled);

    SO_KIT_INIT_INSTANCE();

    surrogatePartPathList = new SoPathList;
    surrogatePartNameList = new SbPList;

    // This sensor will watch the topSeparator part.  If the part changes to a 
    // new node,  then the fields of the old part will be disconnected and
    // the fields of the new part will be connected.
    // Connections are made from/to the renderCaching, boundingBoxCaching,
    // renderCulling and pickCulling fields. This way, the SoInteractionKit
    // can be treated from the outside just like a regular SoSeparator node.
    // Setting the fields will affect caching and culling, even though the
    // topSeparator takes care of it.
    // oldTopSep keeps track of the part for comparison.
    fieldSensor = new SoFieldSensor( &SoInteractionKit::fieldSensorCB, this );
    fieldSensor->setPriority(0);
    oldTopSep = NULL;
    setUpConnections( TRUE, TRUE );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//

SoInteractionKit::~SoInteractionKit()
//
////////////////////////////////////////////////////////////////////////
{
    // truncate takes care of unref'ing all the entries
    surrogatePartPathList->truncate(0);
    delete surrogatePartPathList;

    surrogatePartNameList->truncate(0);
    delete surrogatePartNameList;

    if (oldTopSep) {
	oldTopSep->unref();
	oldTopSep = NULL;
    }
    if (fieldSensor)
	delete fieldSensor;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Copies the contents of the given instance into this
//    instance. This copies the surrogatePartPathList and the
//    surrogatePartNameList.
//
// Use: protected, virtual

void
SoInteractionKit::copyContents(const SoFieldContainer *fromFC,
			       SbBool copyConnections)
//
////////////////////////////////////////////////////////////////////////
{
    // Copy the regular node kit stuff
    SoBaseKit::copyContents(fromFC, copyConnections);

    const SoInteractionKit *fromKit = (const SoInteractionKit *) fromFC;

    // calls to setPartAsDefault() will result in some non-NULL parts
    // receiving calls of setDefault(TRUE).   These nodes should not
    // be copied. Rather, they should be instances of the default nodes
    // use in the original.
	const SoNodekitCatalog *cat = getNodekitCatalog();
	SoType prtFldType = SoSFNode::getClassTypeId();

	for (int i = 1; i < cat->getNumEntries(); i++ ) {

	    // Is this part a leaf node?
	    if ( cat->isLeaf(i) ) {

	        // Is the field for this part set to default and also a
		// node-pointer field?
		const SbName partName = cat->getName(i);
		SoField *fld = getField( partName );
		if ( fld->isDefault() && fld->isOfType( prtFldType )) { 

		    // Is it non-NULL? 
		    SoNode *n = ((SoSFNode *) fld)->getValue();
		    if (n != NULL ) {
			// Set part in this to be an instance of the
			// original part. Use setPartAsDefault(), so
			// it also sets the field to default in this.
			setAnyPartAsDefault(partName, n, TRUE, FALSE);
		    }
		}
	    }
	}

    // Now, copy the two surrogate lists...
	(*surrogatePartPathList).copy( (*fromKit->surrogatePartPathList) );
        (*surrogatePartNameList).copy( (*fromKit->surrogatePartNameList) );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Creates dictionary of part names corresponding to different nodes.
//
//    The 'priority' of entries is as follows:
//
//    Anything in 'filename' will definitely be used.
//    Anything in 'defaultBuffer' will be used, if it's not found in
//    'filename'.
//
// Use: protected
//
void
SoInteractionKit::readDefaultParts(const char   *fileName, 
				const char   defaultBuffer[], 
				int defBufSize )
//
////////////////////////////////////////////////////////////////////////
{
    // Next, read the contents of 'defaultBuffer'. 
    // Add the parts found within this to the dictionary.
    SoGroup *bufStuff = readFromBuffer( defaultBuffer, defBufSize );
    if (bufStuff != NULL)
	bufStuff->ref();

    // Next, read the contents of 'fileName'
    // Add the parts found within this to the dictionary.
    SoGroup *fileStuff = readFromFile( fileName );
    if (fileStuff != NULL)
	fileStuff->ref();
}

////////////////////////////////////////////////////////////////////////
//
// Use: public
//
// Description:
//     Sets the part, then sets the field for that part to default.
//     using partField.setDefault(TRUE).
//     Hence 'newNode' becomes the new default value, and the part 
//     only writes if [a] the part is set to a different node. 
//     [b] values are changes in the fields of newNode.
//     [c] any child below newNode changes.
//
// setPartAsDefault will only set parts if the are public.
// Returns FALSE on failure, TRUE otherwise. 
//
// This version sets the part to the given node.
//
////////////////////////////////////////////////////////////////////////
SbBool
SoInteractionKit::setPartAsDefault(const SbName &partName, SoNode *newNode,
				    SbBool onlyIfAlready )
{
    return( setAnyPartAsDefault( partName, newNode, FALSE, onlyIfAlready ) );
}

////////////////////////////////////////////////////////////////////////
//
// Use: public
//
// Description:
//     This overloaded version of setPartAsDefault takes the name
//     of the node as an argument.  It looks the node up in the global
//      dictionary.
//
////////////////////////////////////////////////////////////////////////
SbBool
SoInteractionKit::setPartAsDefault(const SbName &partName, 
			       const SbName &newNodeName, SbBool onlyIfAlready )
{
    return( setAnyPartAsDefault( partName, newNodeName, FALSE, onlyIfAlready ));
}

////////////////////////////////////////////////////////////////////////
//
// Use: protected
//
// Description:
//     Protected versions of these methods that allow you to set non-leaf
//     and/or private parts.
////////////////////////////////////////////////////////////////////////
SbBool 
SoInteractionKit::setAnyPartAsDefault(const SbName &partName, 
			const SbName &newPartName, SbBool anyPart,
			SbBool onlyIfAlready )
{
    // Try to find the given node in the global dictionary.
    SoNode *newNode = SoNode::getByName(newPartName);
    return( setAnyPartAsDefault( partName, newNode, anyPart, onlyIfAlready ));
}
SbBool 
SoInteractionKit::setAnyPartAsDefault(const SbName &partName, 
			SoNode *newNode, SbBool anyPart, SbBool onlyIfAlready )
{
    // Ref ourself while in this routine.
    ref();

    // Find the 'owner' kit. This is the kit (which could be a child of this 
    // one) which is the closest parent above the part.
    SoField   *f;
    SoBaseKit *owner = NULL;
    SbBool happyDone = FALSE;
    SbBool reply = TRUE;

    // If the partName is in our catalog, we are the owner.
    if (getNodekitCatalog()->getPartNumber(partName) 
	    != SO_CATALOG_NAME_NOT_FOUND)
	owner = this;

    if ( onlyIfAlready ) {
	// We have to determine if the part already isDefault()

	// If we're the owner, it's pretty easy to check...
	if ( owner == this ) {
	    f = getField( partName );
	    if ( ! f ||  ! f->isDefault() )
		reply = FALSE;
	}
	else {
	    // Create a path to the part, but use makeIfNeeded of FALSE.
	    // Don't check leaf or public. We just want information.
	    SoNodeKitPath *testP 
		= createPathToAnyPart(partName, FALSE,FALSE,FALSE);
	    if (testP != NULL) {
		testP->ref();

		// Cast to a full path:
		    SoFullPath *fp = (SoFullPath *) testP;
		// The part is the tail of the full path:
		    SoNode *part = fp->getTail();
		    SoNode *tester;
		// Owner is first nodekit above the tail:
		    for (int ind = 1; ind < fp->getLength(); ind ++ ) {
			tester = fp->getNodeFromTail(ind);
			if ( tester->isOfType(SoBaseKit::getClassTypeId()) ) {
			    owner = (SoBaseKit *) tester;
			    break;
			}
		    }

		if (owner == NULL)     // This should never happen...
		    reply = FALSE;
		else {

		    // Determine name of part in the owner's catalog.
		    // If contains '.' character (e.g., "duck.foot.toe")
		    // take LAST word.
		    const char *partStringInOwner;
		    const char *lastDot = strrchr(partName.getString(), '.' );
		    if (lastDot)
			partStringInOwner = lastDot+1;
		    else
			partStringInOwner = partName.getString();

		    // Find out if the part isDefault()
		    f = owner->getField(partStringInOwner);
		    if (! f || ! f->isDefault() )
			reply = FALSE;
		    else if ( ((SoSFNode *)f)->getValue() == newNode ) {
			// We can return successfully!  The part is default,
			// and already equals newNode.
			happyDone = TRUE;
		    }
		}

		testP->unref();
	    }
	    // If path was NULL, the part has not been created, so 
	    // we will allow ourselves to set the part as default. We may
	    // continue...
	}
    }

    // Did we pass the onlyIfAlready test (if we were required to)??
    if ( reply == FALSE ) {
	unrefNoDelete();
	return FALSE;
    }

    // Was the part already there and already default?
    if ( happyDone == TRUE ) {
	unrefNoDelete();
	return FALSE;
    }

    // If we can't set the part, then bag it and return.
    if ( !setAnyPart(partName,   newNode, anyPart )) {
	unrefNoDelete();
	return FALSE;
    }

    // If we still don't know the owner, we've got to find out now...
    if (owner == NULL) {

	// use makeIfNeed of TRUE. We'll set the part back to NULL if 
	// newNode is NULL.
	SoNodeKitPath *nkp = createPathToAnyPart( partName, TRUE,
						 !anyPart, !anyPart );
	if (nkp == NULL)
	    reply = FALSE;
	else {
	    nkp->ref();

	    SoBaseKit *tail = (SoBaseKit *) nkp->getTail();

	    if (tail != newNode)
		owner = (SoBaseKit *) tail;
	    else
		owner = (SoBaseKit *) nkp->getNodeFromTail( 1 );

	    nkp->unref();
	}
    }
    if (owner == NULL)
	reply = FALSE;

    // We got the owner.
    if ( reply ) {
        owner->ref();

	// Determine name of part in the owner's catalog.
	// If contains '.' character (e.g., "duck.foot.toe")
	// take LAST word.
	const char *partStringInOwner;
	const char *lastDot = strrchr(partName.getString(), '.' );
	if (lastDot)
	    partStringInOwner = lastDot+1;
	else
	    partStringInOwner = partName.getString();

	// If newNode is NULL, set it back to NULL now.
	// (remember, we forced part creation when we
	// made the path to find the owner).
	if (newNode == NULL)
	    setAnyPart( partName,NULL,anyPart);

	// call setDefault(TRUE) on the part.
	f = owner->getField(partStringInOwner);
	if (f != NULL)
	    f->setDefault(TRUE);
	else
	    reply = FALSE;

	owner->unref();
    }

    unrefNoDelete();
    return reply;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Creates shared dragger geometry 
//
// Use: protected
//
SoGroup *
SoInteractionKit::readFromFile(const char *fileName)
//
////////////////////////////////////////////////////////////////////////
{
    SoInput	in;

    // Only look for the file if the environment variable is set.
    if ( getenv("SO_DRAGGER_DIR") == NULL )
	return NULL;

    SoGroup *answer = NULL;

#ifdef DRAGGER_DIR_TEST
    fprintf(stderr, "STARTING DIRECTORY LIST\n");
    const SbStringList &startList = SoInput::getDirectories();
    for (int sli = 0; sli < startList.getLength(); sli++ ) {
	fprintf(stderr,"  %s\n", startList[sli]->getString() );
    }
#endif

    // There is a static directory list on the class SoInput.
    // We ONLY want to look in the ones in our SO_DRAGGER_DIR, so
    // we save the list, do our stuff, then restore the list.
	SbStringList dirStrings = SoInput::getDirectories();
	SbStringList savedDirStrings;
	int i;
	for (i = 0; i < dirStrings.getLength(); i++ ) {
	    SbString *newOne = new SbString( dirStrings[i]->getString() );
	    savedDirStrings.append( newOne );
	}

    // Set up the directory list based on the enviroment variable only
	SoInput::clearDirectories();
	in.addEnvDirectoriesLast( "SO_DRAGGER_DIR");

    if ( in.openFile( fileName, TRUE )) {

	// read it on in...
	answer = SoDB::readAll( &in );

	in.closeFile();
    }

    // Restore the old directory list
	SoInput::clearDirectories();
	for ( i = 0; i < savedDirStrings.getLength(); i++ )
	    SoInput::addDirectoryLast( savedDirStrings[i]->getString() );

    // Delete the strings we allocated and truncate the saved list
	for ( i = 0; i < savedDirStrings.getLength(); i++ )
	    delete savedDirStrings[i];
	savedDirStrings.truncate(0);

#ifdef DRAGGER_DIR_TEST
    fprintf(stderr, "ENDING DIRECTORY LIST\n");
    const SbStringList &endList = SoInput::getDirectories();
    for (int eli = 0; eli < endList.getLength(); eli++ ) {
	fprintf(stderr,"  %s\n", endList[eli]->getString() );
    }
#endif

    return answer;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads geometry from a set of buffers.
//    Assumes that the last buffer is empty.
//    If the buffer contains numerous scene graphs, puts them all under
//    a common separator.
//
// Use: protected
//
SoGroup *
SoInteractionKit::readFromBuffer(const char defaultBuffer[], int defBufSize )
//
////////////////////////////////////////////////////////////////////////
{
    SoInput	in;

    if ( defaultBuffer == NULL )
	return NULL;

    in.setBuffer((void *) defaultBuffer, (size_t) defBufSize );

    // read it on in...
    SoSeparator  *root = SoDB::readAll( &in );

    return root;
}

////////////////////////////////////////////////////////////////////////
//
// Use: public
//
SbBool
SoInteractionKit::setPartAsPath( const SbName &partName, SoPath *surrogatePath )
//
////////////////////////////////////////////////////////////////////////
{
    // The last 2 arguments indicate that we can only do this for 
    // leafparts that are also public.
    return( setAnyPartAsPath( partName, surrogatePath, TRUE, TRUE ) );
}

////////////////////////////////////////////////////////////////////////
//
// Use: public
//
SbBool
SoInteractionKit::setAnyPartAsPath( const SbName &partName, 
				    SoPath *surrogatePath,
				    SbBool leafCheck, SbBool publicCheck )
//
////////////////////////////////////////////////////////////////////////
{
    // Try to set the surrogate path to 'surrogatePath'
    // You might not be able to, for example if the part does not exist
    // or if it is not public or not a leaf.
    if ( !setAnySurrogatePath(partName, surrogatePath, leafCheck, publicCheck)){
#ifdef DEBUG
	SoDebugError::post("SoInteractionKit::setPartAsPath",
		"can not set surrogate path for part %s", partName.getString());
#endif
	return FALSE;
    }

    // If you were successful, then 
    // see if you can empty out the part.
    // You might not be able to, for example if the part does not exist
    // or if it is not public or not a leaf.

    // Strategy:
    //1-Get pathToPart, with makeIfNeeded FALSE.
    //2-If (pathToPart == NULL) the part is already NULL, so just return.
    //3-Get the 'partParent' and 'partNode' from pathToPart, then discard path.
    //4-Determine if we can set any part by examining leafCheck & publicCheck
    //5-If 'partParent' is NOT an SoSwitch:
    //  Set the part to NULL and return.
    //6-If 'partParent' IS an SoSwitch:
    //  Setting the part to NULL could screw up the whichChild
    //  numbering. So we won't set part to NULL if we can avoid it.
	//A-If the 'partNode' is exactly of type SoGroup or 
	//  SoSeparator, setPart(partName, freshlyCreatedEmptyOne ); and return.
	//B-If the 'partNode' is a different type, we have to set 
	//  the part to NULL, but to be safe we will change the setting 
	//  of whichChild.
	    //1-If (whichChild > partIndex) decrement by one.
	    //2-Else if (whichChild == partIndex) set to NONE.
	    //3-Else (whichChild < partIndex) so leave alone.
	    //4-Set the part to NULL and return.

    //1-Get pathToPart, with makeIfNeeded FALSE.
	SoNodeKitPath *pathToPart 
	    = SoBaseKit::createPathToAnyPart( partName, FALSE, 
					      leafCheck, publicCheck );

    //2-If (pathToPart == NULL) the part is already NULL, so just return.
	if (pathToPart == NULL)
	    return TRUE;

    //3-Get the 'partParent' and 'partNode' from pathToPart, then discard path.
	pathToPart->ref();
	SoNode *partParent = ((SoFullPath *)pathToPart)->getNodeFromTail(1);
	SoNode *partNode   = ((SoFullPath *)pathToPart)->getTail();
	pathToPart->unref();

    //4-Determine if we can set any part by examining leafCheck & publicCheck
	SbBool anyPart = ( !leafCheck && !publicCheck );

    //5-If 'partParent' is NOT an SoSwitch:
    //  Set the part to NULL and return.
        if ( ! partParent->isOfType(SoSwitch::getClassTypeId() ) )
	    return ( SoBaseKit::setAnyPart( partName, NULL, anyPart ) );

    //6-If 'partParent' IS an SoSwitch:
    //  Setting the part to NULL could screw up the whichChild
    //  numbering. So we won't set part to NULL if we can avoid it.

	//A-If the 'partNode' is exactly of type SoGroup or 
	//  SoSeparator, setPart(partName, freshlyCreatedEmptyOne ); and return.
	    if (partNode->getTypeId() == SoGroup::getClassTypeId())
	        return(SoBaseKit::setAnyPart(partName, new SoGroup, anyPart));
	    if (partNode->getTypeId() == SoSeparator::getClassTypeId())
	        return(SoBaseKit::setAnyPart(partName,new SoSeparator,anyPart));

	//B-If the 'partNode' is a different type, we have to set 
	//  the part to NULL, but to be safe we will change the setting 
	//  of whichChild.

	    SoSwitch *parentSwitch = (SoSwitch *) partParent;
	    int whichChild = parentSwitch->whichChild.getValue();
	    int partIndex = parentSwitch->findChild( partNode );

	    //1-If (whichChild > partIndex) decrement by one.
		if (whichChild > partIndex)
		    parentSwitch->whichChild.setValue( whichChild - 1 );
	    //2-Else if (whichChild == partIndex) set to NONE.
		else if (whichChild == partIndex)
		    parentSwitch->whichChild.setValue( SO_SWITCH_NONE );
	    //3-Else (whichChild < partIndex) so leave alone.
		else {
		}

	    //4-Set the part to NULL and return.
    		return SoBaseKit::setAnyPart( partName, NULL, anyPart );
}

////////////////////////////////////////////////////////////////////////
//
// Use: protected
//
SoNode *
SoInteractionKit::getAnyPart( const SbName &partName, SbBool makeIfNeeded,
			      SbBool leafCheck, SbBool publicCheck )
//
////////////////////////////////////////////////////////////////////////
{
    // Try to get the part:
    SoNode *n;
    n = SoBaseKit::getAnyPart(partName, makeIfNeeded, leafCheck, publicCheck);

    // You might not be able to, for example:
    //    [a] makeIfNeeded == FALSE
    //    [b] part does not exist
    //    [c] we ask it to leafCheck or publicCheck and it fails.
    if ( n == NULL )
	return NULL;

    // If you were successful, then try to set the surrogate path to NULL
    n->ref();
    if ( !setAnySurrogatePath( partName, NULL, leafCheck, publicCheck )) {
#ifdef DEBUG
	SoDebugError::post("SoInteractionKit::getAnyPart",
		"can not set surrogate path to NULL for part %s",
		partName.getString() );
#endif
    }
    n->unref();

    return n;
}

////////////////////////////////////////////////////////////////////////
//
// Use: protected
//
SoNodeKitPath *
SoInteractionKit::createPathToAnyPart( const SbName &partName, 
			SbBool makeIfNeeded, SbBool leafCheck, 
			SbBool publicCheck, const SoPath *pathToExtend )
//
////////////////////////////////////////////////////////////////////////
{
    // Try to create a path to the part:
    SoNodeKitPath  *answerPath = SoBaseKit::createPathToAnyPart(
		    		    partName, makeIfNeeded, leafCheck, 
				    publicCheck, pathToExtend );

    // You might not be able to, for example:
    //    [a] makeIfNeeded == FALSE
    //    [b] part does not exist
    //    [c] we ask it to leafCheck or publicCheck and it fails.
    if ( answerPath == NULL )
	return NULL;

    // If you were successful, then try to set the surrogate path to NULL
    answerPath->ref();
    if ( !setAnySurrogatePath( partName, NULL, leafCheck, publicCheck )) {
#ifdef DEBUG
	SoDebugError::post("SoInteractionKit::createPathToAnyPart",
		"can not set surrogate path to NULL for part %s",
		partName.getString() );
#endif
    }
    answerPath->unrefNoDelete();

    return answerPath;
}

////////////////////////////////////////////////////////////////////////
//
// Use: protected
//
SbBool
SoInteractionKit::setAnyPart( const SbName &partName, SoNode *from,
			      SbBool anyPart )
//
////////////////////////////////////////////////////////////////////////
{
    // Try to create set the part:
    // You might not be able to, for example:
    //    [a] part does not exist
    //    [b] anyPart is FALSE and the part is a leaf or non-public.
    if ( !SoBaseKit::setAnyPart( partName, from, anyPart ))
	return FALSE;

    // Temporary ref
    if (from)
	from->ref();

    // If you were successful, then try to set the surrogate path to NULL
    SbBool success = TRUE;
    if ( !setAnySurrogatePath( partName, NULL, !anyPart, !anyPart )) {
#ifdef DEBUG
	SoDebugError::post("SoInteractionKit::setAnyPart",
		"can not set surrogate path to NULL for part %s",
		partName.getString() );
#endif
	success = FALSE;
    }

    // Undo temporary ref
    if (from)
	from->unref();
    return success;
}

////////////////////////////////////////////////////////////////////////
//
// Use: protected
//
SbBool
SoInteractionKit::setAnySurrogatePath( const SbName &partName, 
			SoPath *from, SbBool leafCheck, SbBool publicCheck )
//
////////////////////////////////////////////////////////////////////////
{
    // Strategy:
    //   [-2] If 'partName' is in this catalog, just call 
    //        setMySurrogatePath and return.
    //   [-1] If 'from' is NULL, and partName is not directly in this catalog,
    //        determine which of our leaf nodes is on the way to the part.
    //        This will be either a nodekit or a list part.
    //        If this 'intermediary' has not been created yet, then we
    //        can just return.  This is because if the intermediary is NULL,
    //        then the part below it can have no value as of yet. So we don't
    //        need to bother removing it's surrogate path (which is what 
    //        we do when 'from' is NULL.
    //   [0] Temporarily ref 'from' and 'this'
    //       We need to ref 'this' because creating a path refs this,
    //       and this can get called from within constructors since
    //       it's called from within setPart(), getPart(), etc.
    //   [1] get partPath, which leads down to the part.
    //       First time, use 'makeIfNeeded' of FALSE.
    //       That's how we'll find out if there was something there to start.
    //
    //   [2] If (partPath == NULL), call it again with 'makeIfNeeded' of TRUE,
    //       but remember that we must NULL out the part when we are finished.
    //
    //   [3] Now we've got a path from 'this' down to the part.
    //       Find 'owner', the first InteractionKit above the part in the 
    //       partPath. 
    //       Note:   'owner' might not == this.
    //   [4] Find the 'nameInOwner' the name of the part within 'owner'
    //   [5] Tell 'owner' to use the given path 'from' as its surrogate 
    //       path for 'nameInOwner'
    //   [6] If you need to, remember to set the node-part back to NULL
    //   [8] Undo temporary ref of 'from' and 'this'

    //   [-2] If 'partName' is in this catalog, just call 
    //        setMySurrogatePath and return.
	const SoNodekitCatalog *cat = getNodekitCatalog();
	int partNum = cat->getPartNumber( partName );
	if ( partNum != SO_CATALOG_NAME_NOT_FOUND ) {
	    if ( leafCheck && (cat->isLeaf(partNum) == FALSE) )
		return FALSE;
	    if ( publicCheck && (cat->isPublic(partNum) == FALSE) )
		return FALSE;
	    setMySurrogatePath(partName, from);
	    return TRUE;
	}

    //   [-1] If 'from' is NULL, and partName is not directly in this catalog,
    //        determine which of our leaf nodes is on the way to the part.
    //        This will be either a nodekit or a list part.
    //        If this 'intermediary' has not been created yet, then we
    //        can just return.  This is because if the intermediary is NULL,
    //        then the part below it can have no value as of yet. So we don't
    //        need to bother removing its surrogate path (which is what 
    //        we do when 'from' is NULL.
	if (from == NULL) {

	    // See if there's a '.' and/or a '[' in the partName.
	    // (as in "childList[0].appearance")
	    // If so, get the string up to whichever came first.
	    // This will be the 'intermediary' we look for.
	    char *dotPtr   = strchr( partName.getString(), '.' );
	    char *brackPtr = strchr( partName.getString(), '[' );

	    if ( dotPtr != NULL || brackPtr != NULL ) {
		char *nameCopy = strdup( partName.getString() );
		char *firstName;
		if (dotPtr == NULL)
		    firstName = strtok( nameCopy, "[");
		else if (brackPtr == NULL || dotPtr < brackPtr)
		    firstName = strtok( nameCopy, ".");
		else 
		    firstName = strtok( nameCopy, "[");

		// Okay, look for the part, then free the string copy.
		int firstPartNum = cat->getPartNumber( firstName );

		SoNode *firstPartNode = NULL; 
		if ( firstPartNum != SO_CATALOG_NAME_NOT_FOUND ) {
		    // Check if the part is there.
		    // 2nd arg is FALSE, 'cause we don't want to create part.
		    // 3rd arg is TRUE 'cause this better be a leaf.
		    firstPartNode = SoBaseKit::getAnyPart( firstName, 
						FALSE, TRUE, publicCheck );
		}
		free (nameCopy);

		// If the intermediary doesn't exist, return TRUE
		if (firstPartNode == NULL)
		    return TRUE;
	    }
	}


    //   [0] Temporarily ref 'from' and 'this'
	if (from) from->ref();
	ref();

    //   [1] get partPath, which leads down to the part.
    //       First time, use 'makeIfNeeded' of FALSE.
    //       That's how we'll find out if there was something there to start.
	SbBool        makeIfNeeded = FALSE;
	SoNodeKitPath *partPath    = NULL;

	partPath = SoBaseKit::createPathToAnyPart( partName, makeIfNeeded, 
					           leafCheck, publicCheck );

    //   [2] If (partPath == NULL), call it again with 'makeIfNeeded' of TRUE,
    //       but remember that we must NULL out the part when we are finished.
	if (partPath == NULL) {
	    // Try again, this time with 'makeIfNeeded' TRUE
	    makeIfNeeded = TRUE;
	    partPath = SoBaseKit::createPathToAnyPart( partName, makeIfNeeded, 
					           leafCheck, publicCheck );
	}
	if (partPath == NULL) {
	    // This would happen if leafCheck or publicCheck were TRUE
	    // and the check failed.
#ifdef DEBUG
	    SoDebugError::post("SoInteractionKit::setAnySurrogatePath",
		"can not get a part path for part %s", partName.getString());
#endif
	    // Undo temporary ref of 'from' and 'this'
	    if (from) from->unref();
	    unrefNoDelete();
	    return FALSE;
	}
	else
	    partPath->ref();

    //   [3] Now we've got a path from 'this' down to the part.
    //       Find 'owner', the first InteractionKit above the part in the 
    //       partPath. 
    //       Note:   'owner' might not == this.
	SoInteractionKit *owner = NULL;
	for (int i = partPath->getLength() - 1;  i >= 0; i-- ) {
	    SoNode *n = partPath->getNode(i);
	    if ( n != ((SoFullPath *)partPath)->getTail() &&
		 n->isOfType( SoInteractionKit::getClassTypeId() ) ) {
		    owner = (SoInteractionKit *) n;
		    owner->ref();
		    break;
	    }
	}
	if ( owner == NULL ) {
	    partPath->unref();
	    // Undo temporary ref of 'from' and this.
	    if (from) from->unref();
	    unrefNoDelete();
	    return FALSE;
	}

    //   [4] Find the 'nameInOwner' the name of the part within 'owner'
	SbName nameInOwner = owner->getPartString( partPath );
	
    //   [5] Tell 'owner' to use the given path 'from' as its surrogate 
    //       path for 'nameInOwner'
    //       Use setMySurrogatePath for this...
	owner->setMySurrogatePath(nameInOwner,from);

    //   [6] If you need to, remember to set the node-part back to NULL
	SbBool success = TRUE;
	if (makeIfNeeded == TRUE) {
	    SbBool anyPart = ( !leafCheck && !publicCheck );
	    if ( !SoBaseKit::setAnyPart( partName, NULL, anyPart ) )
		success = FALSE;
	}

	owner->unref();
	partPath->unref();

    //   [8] Undo temporary ref of 'from' and 'this'
	if (from) from->unref();
	unrefNoDelete();

	return success;
}

////////////////////////////////////////////////////////////////////////
//
// Assumes that this node is the 'owner' of the surrogate part.
// That is, it is the first InteractionKit above the part.
//
// This means that 'name' is registered in this node's catalog,
// or if the name is 'listName[#]', then listName is in the catalog.
//
// Passing a value of NULL for newPath has the effect of removing 
// this part from the surrogate path lists.
//
// Use: private
//
void
SoInteractionKit::setMySurrogatePath( const SbName &name, SoPath *newPath )
//
////////////////////////////////////////////////////////////////////////
{
    int index = surrogatePartNameList->find( (void *) name.getString() );
    if ( index != -1 ) {
	// an entry already exists for this part name. So we need to remove
	// the old entry before adding the new one.
	surrogatePartNameList->remove( index );
	surrogatePartPathList->remove( index );
    }

    // Now append the new entry.
    if ( newPath != NULL ) {
	surrogatePartNameList->append( (void *) name.getString() );
	surrogatePartPathList->append( newPath );
    }
}

////////////////////////////////////////////////////////////////////////
//
// Use: EXTENDER public
//
SbBool
SoInteractionKit::isPathSurrogateInMySubgraph( const SoPath *pathToCheck )
//
////////////////////////////////////////////////////////////////////////
{
    SoPath *pathToOwner, *pathInOwner;
    SbName  nameInOwner;

    return( isPathSurrogateInMySubgraph( pathToCheck, pathToOwner,
				       nameInOwner, pathInOwner, FALSE ));
}

////////////////////////////////////////////////////////////////////////
//
// Use: EXTENDER public
//
SbBool
SoInteractionKit::isPathSurrogateInMySubgraph( const SoPath *pathToCheck,
		       SoPath *&pathToOwner, SbName  &surrogateNameInOwner,
		       SoPath *&surrogatePathInOwner, SbBool fillArgs )
//
////////////////////////////////////////////////////////////////////////
{
    // Search inside this kit and each InteractionKit below.
    // See if they have a surrogate path that is wholely contained within
    // pathToCheck.
    // The first time you find one, fill in the info and return.

    // Get the paths to the InteractionKits
	static SoSearchAction *searchAction = NULL;
	if (searchAction == NULL)
	    searchAction = new SoSearchAction;
	else
	    searchAction->reset();
	searchAction->setType( SoInteractionKit::getClassTypeId() );
	searchAction->setInterest( SoSearchAction::ALL );
	SbBool oldNkVal = SoBaseKit::isSearchingChildren();
	SoBaseKit::setSearchingChildren( TRUE );
	searchAction->apply(this);
	SoBaseKit::setSearchingChildren( oldNkVal );

	SoPathList ownerPaths = searchAction->getPaths();

    // Start at the end and work backwards.
    // More likely to find a closer match this way...
    // For each path:
	for (int i = ownerPaths.getLength() - 1; i >= 0; i-- ) {

	    // Get the potential owner:
		SoPath *ownerPath = ownerPaths[i];
		SoInteractionKit *owner 
		    = (SoInteractionKit *) ((SoFullPath *)ownerPath)->getTail();

	    // See if the potential owner is in fact the owner...
		int surrogateNum = owner->indexOfSurrogateInMyList(pathToCheck);

	    // If it's the owner, we've succeeded and should return.
		if ( surrogateNum != -1 ) {
		    if (fillArgs == TRUE) {
			pathToOwner = ownerPath->copy();
			surrogateNameInOwner = (char *) 
			      (*(owner->surrogatePartNameList))[surrogateNum];
			surrogatePathInOwner =
			      (*(owner->surrogatePartPathList))[surrogateNum];
		    }
		    return TRUE;
		}
	}

    // Return FALSE if you could not find an owner.
	return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Use: private
//
int
SoInteractionKit::indexOfSurrogateInMyList( const SoPath *pathToCheck )
//
////////////////////////////////////////////////////////////////////////
{
    if (pathToCheck == NULL)
	return -1;

    // Check each of the surrogate paths...
    for ( int i = 0; i < surrogatePartPathList->getLength(); i++ ) {

	// If the pathToCheck contains the surrogate path, we're okay!
	if ( pathToCheck->containsPath( (*surrogatePartPathList)[i] ) )
	    return i;
    }
    return -1; // no match found
}

////////////////////////////////////////////////////////////////////////
//
// Use: EXTENDER public, static
//
void
SoInteractionKit::setSwitchValue( SoNode *n, int newVal )
//
////////////////////////////////////////////////////////////////////////
{
    if ( n != NULL ) {
	if  (n->getTypeId() == SoSwitch::getClassTypeId()) {
	    SoSwitch *sw = (SoSwitch *) n;
	    if (sw->whichChild.getValue() != newVal && 
		sw->getNumChildren() > newVal )
		sw->whichChild.setValue( newVal );
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    overrides method in SoNode to return FALSE.
//    Since there is a separator as the top child of the dragger.
//
// Use: public
//
SbBool
SoInteractionKit::affectsState() const
{
    return FALSE;
}

//    detach/attach any sensors, callbacks, and/or field connections.
//    Called by:            start/end of SoBaseKit::readInstance
//    and on new copy by:   start/end of SoBaseKit::copy.
//    Classes that redefine must call setUpConnections(TRUE,TRUE) 
//    at end of constructor.
//    Returns the state of the node when this was called.
SbBool
SoInteractionKit::setUpConnections( SbBool onOff, SbBool doItAlways )
{
    if ( !doItAlways && connectionsSetUp == onOff)
	return onOff;

    if ( onOff ) {

	// We connect AFTER base class.
	SoBaseKit::setUpConnections( onOff, FALSE );

	// Hookup the field-to-field connections on topSeparator.
	connectSeparatorFields( oldTopSep, TRUE );

	// Call sensor CBs to make sure oldTopSep is up-to-date
	fieldSensorCB( this, NULL );

	// Connect the field sensors
	if (fieldSensor->getAttachedField() != &topSeparator)
	    fieldSensor->attach( &topSeparator );
    }
    else {

	// We disconnect BEFORE base class.

	// Disconnect the field sensors.
	if (fieldSensor->getAttachedField())
	    fieldSensor->detach();

	// Undo the field-to-field connections on topSeparator.
        connectSeparatorFields( oldTopSep, FALSE );

	SoBaseKit::setUpConnections( onOff, FALSE );
    }

    return !(connectionsSetUp = onOff);
}

SbBool
SoInteractionKit::readInstance( SoInput *in, unsigned short flags )
{
    // Start with base kit functionality...
	SbBool answer = SoBaseKit::readInstance(in, flags);

    // Any part that is set to a non-NULL value should not have a corresponding
    // entry in the surrogate part list.
    // The exception would be a part that is an empty group or an empty
    // separator node.  These can stay.

    // Run through every name in the surrogatePartNameList.
	char   *theName;
	SoNode *thePart;
	for ( int i = 0; i < surrogatePartNameList->getLength(); i++ ) {

	    theName = (char *) (*surrogatePartNameList)[i];

	    // Check if a part exists for it. 
		thePart = SO_CHECK_ANY_PART( this, theName, SoNode );

	    // If the part is NULL or an empty group or an empty separator, 
	    // the surrogate path can stay. Otherwise, remove it.
		SbBool removeSurrogate = TRUE;
		if (thePart == NULL)
		    removeSurrogate = FALSE;
		else if ( thePart->getTypeId() == SoGroup::getClassTypeId() ) {
		    if ( ((SoGroup *)thePart)->getNumChildren() == 0 )
			removeSurrogate = FALSE;
		}
		else if(thePart->getTypeId() == SoSeparator::getClassTypeId() ){
		    if ( ((SoSeparator *)thePart)->getNumChildren() == 0 )
			removeSurrogate = FALSE;
		}

	    // Remove the name and path if necessary.
		if (removeSurrogate == TRUE) {
		    surrogatePartNameList->remove( i );
		    surrogatePartPathList->remove( i );
		}
	}

    return answer;
}

void
SoInteractionKit::connectSeparatorFields( SoSeparator *dest, SbBool onOff )
{
    if (dest == NULL)
	return;
    if (onOff) {
	SoField *f;
	if ( ! dest->renderCaching.getConnectedField(f) ||
	       f != &renderCaching )
	    dest->renderCaching.connectFrom( &renderCaching );
	if ( ! dest->boundingBoxCaching.getConnectedField(f) ||
	       f != &boundingBoxCaching )
	    dest->boundingBoxCaching.connectFrom( &boundingBoxCaching );
	if ( ! dest->renderCulling.getConnectedField(f) ||
	       f != &renderCulling )
	    dest->renderCulling.connectFrom( &renderCulling );
	if ( ! dest->pickCulling.getConnectedField(f) ||
	       f != &pickCulling )
	    dest->pickCulling.connectFrom( &pickCulling );
    }
    else {
	dest->renderCaching.disconnect();
	dest->boundingBoxCaching.disconnect();
	dest->renderCulling.disconnect();
	dest->pickCulling.disconnect();
    }
}

void
SoInteractionKit::fieldSensorCB( void *inKit, SoSensor *)
{
    SoInteractionKit *k  = (SoInteractionKit *) inKit;
    if ( k->oldTopSep == k->topSeparator.getValue())
	return;

    k->connectSeparatorFields( k->oldTopSep, FALSE );

    SoNode *newTopSep = k->topSeparator.getValue();
    if (newTopSep)
	newTopSep->ref();

    if (k->oldTopSep)
	k->oldTopSep->unref();

    k->oldTopSep = (SoSeparator *) newTopSep;
    k->connectSeparatorFields( k->oldTopSep, TRUE );
}

/////////////////////////////////////////////////////////////////////////
//
// Called by the SoBaseKit::write() method. 
//
// InteractionKits don't want to write fields if they have default vals.
//
// Calls setDefault(TRUE) on caching fields if default and not 
// connected.
//
// Calls setDefault(TRUE) on the topSeparator. 
// Calls setDefault(TRUE) on the geomSeparator. 
// 
// Calls setDefault(TRUE) on any non-leaf part of type exactly == SoSwitch.
// Subclasses can override this if they like, since the function is virtual.
// But interactionKits use non-leaf switches to turn parts on and off, and 
// this does not need to be written to file.
//
// Lastly, calls same method on SoBaseKit.
//
// NOTE: Parts which are set to default may still wind up writing to file 
//       if, for example, they lie on a path.
/////////////////////////////////////////////////////////////////////////
void
SoInteractionKit::setDefaultOnNonWritingFields()
{

// Here's a little macro to test the field value against another node and
// also make sure there's no active connection.
#define __SO_FCHECK( f, v )                                 \
	  ( ! (f.isConnected() && f.isConnectionEnabled())  \
	    && f.getValue() == v )

    // Calls setDefault(TRUE) on caching fields if default and not 
    // connected.
	if ( __SO_FCHECK( renderCaching, SoInteractionKit::AUTO ) )
			  renderCaching.setDefault(TRUE);
	if ( __SO_FCHECK( boundingBoxCaching, SoInteractionKit::AUTO ) )
			  boundingBoxCaching.setDefault(TRUE);
	if ( __SO_FCHECK( renderCulling, SoInteractionKit::AUTO ) )
			  renderCulling.setDefault(TRUE);
	if ( __SO_FCHECK( pickCulling, SoInteractionKit::AUTO ) )
			  pickCulling.setDefault(TRUE);

#undef __SO_FCHECK

    // Calls setDefault(TRUE) on the topSeparator. 
	topSeparator.setDefault(TRUE);
	geomSeparator.setDefault(TRUE);


    // Calls setDefault(TRUE) on any non-leaf part of type exactly == SoSwitch.
	const SoNodekitCatalog *cat = getNodekitCatalog();
	int   numParts = cat->getNumEntries();

	for (int i = 1; i < numParts; i++ ) {

	    // The part should not be a leaf.
		if ( cat->isLeaf(i) )
		    continue;

		SoSFNode *fld = (SoSFNode *) getField( cat->getName(i) );
		SoNode   *n   = fld->getValue();

	    // set to default if node is NULL
		if (n == NULL ) {
		    fld->setDefault(TRUE);
		    continue;
		}

	    // set to default if node is exactly an SoSwitch.
		if ( n->getTypeId() == SoSwitch::getClassTypeId()) {
		    fld->setDefault(TRUE);
		    continue;
		}
	}

    // Call the base class...
        SoBaseKit::setDefaultOnNonWritingFields();
}
