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
 |      SoBaseKit
 |
 |   Author(s)          : Paul Isaacs and Thad Beier
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


#include <Inventor/SbBox.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoNodeKitPath.h>
#include <Inventor/SoPath.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoGetMatrixAction.h>
#include <Inventor/actions/SoHandleEventAction.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/details/SoNodeKitDetail.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/errors/SoReadError.h>
#include <Inventor/misc/SoAuditorList.h>
#include <Inventor/nodekits/SoBaseKit.h>
#include <Inventor/nodekits/SoNodeKitListPart.h>
#include <Inventor/nodes/SoCallback.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoResetTransform.h>
#include <Inventor/nodes/SoSeparator.h>

#include <SoDebug.h>

#include <ctype.h>
#ifdef _POSIX_SOURCE
extern "C" char *strdup(const char *);
#endif

// Defines for printing out file data for nodekits
#define SO_BASEKIT_FILEDATA_HEADER "partName\/childNum pairs [ "
#define SO_BASEKIT_FILEDATA_FOOTER "]"

SO_KIT_SOURCE(SoBaseKit);

SbBool SoBaseKit::searchingChildren = FALSE;

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This prints a diagram of the parts in this class
//
// Use: static, internal

void SoBaseKit::printDiagram()
{
    const SoNodekitCatalog *cat = getNodekitCatalog();
    fprintf( stdout, "CLASS So%s\n", getTypeId().getName().getString() );
    printSubDiagram( cat->getName(0), 0 );
}

void SoBaseKit::printSubDiagram( const SbName &rootName, int level )
{
    const SoNodekitCatalog *cat = getNodekitCatalog();

    // If part is new for this class, or if part is different in this
    // class than in parent class, print "-->" at the beginning of
    // the line. Else, 3 spaces
    SbBool sameAsInParentClass = TRUE;
    if ( getTypeId() == SoBaseKit::getClassTypeId() )
	sameAsInParentClass = FALSE;
    else {
	SoType parentClassType = getTypeId().getParent();
	SoBaseKit *parentClassInst = 
		(SoBaseKit *)parentClassType.createInstance();
	const  SoNodekitCatalog *parentClassCat;
	parentClassCat = parentClassInst->getNodekitCatalog();
	parentClassInst->ref();
	parentClassInst->unref();

	int parentClassPartNum;
	parentClassPartNum = parentClassCat->getPartNumber( rootName );
	if ( parentClassPartNum == SO_CATALOG_NAME_NOT_FOUND )  {
	    sameAsInParentClass = FALSE;
	}
	else {
	    if ( cat->getType( rootName ) != 
		 parentClassCat->getType( rootName ) )
		sameAsInParentClass = FALSE;
	    if ( cat->getDefaultType( rootName ) != 
		 parentClassCat->getDefaultType( rootName ) )
		sameAsInParentClass = FALSE;
	} 
    }

    if ( sameAsInParentClass )
	fprintf( stdout, "   ");
    else
	fprintf( stdout, "-->");

    // indent
    int i;
    for ( i = 0; i < level; i++ )
	fprintf( stdout, "   ");

    fprintf( stdout, "\"%s\"\n", rootName.getString());

    level++;

    int howMany = cat->getNumEntries();
    int *inds = new int[howMany];

    // Find the children parts, going from right to left, and load
    // their indices into the inds array.
    SbName rightSib = "";
    int    curInd;
    SbBool foundIt;
    for ( curInd = 0, foundIt = TRUE; foundIt == TRUE; ) {
	// Find the part with right sibling named RightSib
	foundIt = FALSE;
	for ( i = 0; i < cat->getNumEntries(); i++ ) {
	    if ( foundIt == FALSE && cat->getParentName(i) == rootName ) {
		if (cat->getRightSiblingName(i) == rightSib ) {
		    inds[curInd] = i;
		    rightSib = cat->getName(i);
		    foundIt = TRUE;
		    curInd++;
		}
	    }
	}
    }


    for (i = curInd - 1; i >= 0; i-- ) {
	printSubDiagram( cat->getName(inds[i]), level );
    }

    delete [ /*howMany*/ ] inds;
}

void SoBaseKit::printTable()
{
    const SoNodekitCatalog *cat = getNodekitCatalog();
    fprintf( stdout, "CLASS So%s\n", getTypeId().getName().getString() );
    for (int i = 0; i < cat->getNumEntries(); i++ ) {


	if ( cat->isPublic(i) == FALSE )
	    fprintf(stdout, "PVT ");
	else 
	    fprintf(stdout, "    ");

	const char *catName = cat->getName(i).getString();
	fprintf( stdout, "  \"%s\",  So%s", catName,
		cat->getType(i).getName().getString());

	if ( cat->isList(i) == TRUE ) {
	    const SoTypeList l = cat->getListItemTypes(i);
	    fprintf(stdout, " [");
	    for (int j = 0; j < l.getLength(); j++ ) {
		if ( j > 0 )
		    fprintf(stdout, ",");
		fprintf(stdout, " So%s", l[j].getName().getString() );
	    }
	    fprintf(stdout, " ] ");
	}
	else {
	    fprintf(stdout, "  --- ");
	}

	if ( cat->getType(i) != cat->getDefaultType(i) ) {
	    fprintf(stdout, ", (default type = So%s)", 
				cat->getDefaultType(i).getName().getString());
	}

	fprintf(stdout, "\n");


    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoBaseKit::SoBaseKit()
//
////////////////////////////////////////////////////////////////////////
{
    children = new SoChildList(this);
    fieldDataForWriting = NULL;

    // We use the NODE  constructor 
    SO_KIT_CONSTRUCTOR(SoBaseKit);

    isBuiltIn = TRUE;

    // Create the Catalog entry for "this"
    // Don't use the macro, because we don't create a field for this part.

    if (firstInstance && !nodekitCatalog->addEntry("this",
	    SoBaseKit::getClassTypeId(), SoBaseKit::getClassTypeId(),
	    TRUE, "", "", FALSE, SoType::badType(), SoType::badType(), FALSE ))
	catalogError();

    SO_KIT_ADD_CATALOG_LIST_ENTRY(callbackList, SoSeparator, TRUE,
				       this, , SoCallback, TRUE );
    SO_KIT_ADD_LIST_ITEM_TYPE(callbackList, SoEventCallback );


    if ( getNodekitCatalog() != NULL )
        nodekitPartsList = new SoNodekitParts( this );

    connectionsSetUp = FALSE;
    setUpConnections( TRUE, TRUE );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor (necessary since inline destructor is too complex)
//
// Use: public

SoBaseKit::~SoBaseKit()
//
////////////////////////////////////////////////////////////////////////
{
    if (fieldDataForWriting != NULL)
	delete fieldDataForWriting;
    if ( nodekitPartsList != NULL )
	delete nodekitPartsList;
    delete children;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//     Returns the containerNode within the SoNodeKitListPart given
//     by listName.
//
// Use: protected
SoGroup *
SoBaseKit::getContainerNode( const SbName &listName, SbBool makeIfNeeded )
{
    SoNodeKitListPart *l 
	= (SoNodeKitListPart *) getAnyPart( listName, makeIfNeeded );
    if ( l == NULL )
	return NULL;
    return ( l->getContainerNode() );
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Return the part with the given name
//
// Use: protected

SoNode *
SoBaseKit::getAnyPart( const SbName &partName, SbBool makeIfNeeded, 
			SbBool leafCheck, SbBool publicCheck )
//
////////////////////////////////////////////////////////////////////////
{
    return (nodekitPartsList->getAnyPart( partName, makeIfNeeded, 
						leafCheck, publicCheck ));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Return the part with the given name
//
// Use: public

SoNode *
SoBaseKit::getPart( const SbName &partName, SbBool makeIfNeeded )
//
////////////////////////////////////////////////////////////////////////
{
    // the fourth argument, leafCheck and publicCheck are TRUE, because we 
    // don't ordinarily return parts unless they are public leaves.
    return ( getAnyPart( partName, makeIfNeeded, TRUE, TRUE ) );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   Given a node or a path to a node, sees if it's a part in the 
//   nodekit, a part in a nested nodekit, or an element of a list part.
//   If so, returns the name. If not, returns the empty string.
//
// Use: public

SbString
SoBaseKit::getPartString( const SoBase *part )
//
////////////////////////////////////////////////////////////////////////
{
    const SoNodekitParts   *partsList = getNodekitPartsList();
    const SoNodekitCatalog *cat = getNodekitCatalog();

    if ( part == NULL)
	return "";

    if ( part->isOfType( SoNode::getClassTypeId() )) {
	// trivial case
	if ( part == this )
	    return "this";

	// Look through the parts list and see if you can find a match.
	// Remember to skip entry 0, which is 'this'.
	for ( int i = 1; i < partsList->numEntries; i++ ) {

	    SoNode *iPart = partsList->fieldList[i]->getValue();
	    if (iPart != NULL) {

		// Simple match
		if ( iPart == part )
		    return cat->getName(i).getString();

		if (iPart->isOfType( SoBaseKit::getClassTypeId())) {

		    // Try to look inside the nodekit part.
		    SoBaseKit *kit = (SoBaseKit *) iPart;
		    SbString subString = kit->getPartString(part);
		    if (subString != "") {
			SbString answer( cat->getName(i).getString() );
			answer += ".";
			answer += subString.getString();
			return answer;
		    }
		}
		else if ( cat->isList(i) ) {

		    // Within a leaf that's a list part. Try to look inside.
		    SoNodeKitListPart *lst = (SoNodeKitListPart *) iPart;

		    for (int indx = 0; 
			 indx < lst->getNumChildren(); indx++ ) {
			SoNode *kid = lst->getChild(indx);
			if (kid == part) {
			    char indxString[30];
			    sprintf(indxString, "[%d]", indx );
			    SbString answer( cat->getName(i).getString() );
			    answer += indxString;
			    return answer;
			}
			else if (kid->isOfType(SoBaseKit::getClassTypeId())) {
			    // Try to look inside the nodekit part.
			    SbString subString 
				= ((SoBaseKit *)kid)->getPartString(part);
			    if (subString != "") {
				char indxString[30];
				sprintf(indxString, "[%d]", indx );
				SbString answer( cat->getName(i).getString() );
				answer += indxString;
				answer += ".";
				answer += subString.getString();
				return answer;
			    }
			}
		    }
		}
	    }
	}
    }
    else if (part->isOfType( SoPath::getClassTypeId() )) {
	const SoFullPath *fullP = (const SoFullPath *) part;
	int pathIndex;

	// First, find 'this' on the path. 
	for (pathIndex = 0; pathIndex < fullP->getLength(); pathIndex++ ) {
	    if ( fullP->getNode(pathIndex) == this ) 
		break;
	}

	// If 'this' is not on path...
	if ( pathIndex >= fullP->getLength() )
	    return "";

	// If 'this' is at end of path...
	if ( pathIndex == fullP->getLength() - 1 )
	    return "this";

	pathIndex++;


	// This node appears on the path, but is not the tail.
	// See if the tail is named within this node. Remember to skip
	// entry 0, which is 'this'.
	for ( int i = 1; i < partsList->numEntries; i++ ) {

	    SoNode *iPart = partsList->fieldList[i]->getValue();

	    // If this part lies on the path, then we have work to do...
	    if (iPart == fullP->getNode(pathIndex)) {

		// Is this part at the end of the path?
		if ( pathIndex == fullP->getLength() - 1 )
		    return cat->getName(i).getString();


		// Is this part a leaf in the catalog?
		if ( cat->isLeaf(i) ) {
		    if (iPart->isOfType( SoBaseKit::getClassTypeId())) {

			// Try to look inside the nodekit part.
			SoBaseKit *kit = (SoBaseKit *) iPart;
			SbString subString = kit->getPartString(part);
			if (subString != "") {
			    SbString answer( cat->getName(i).getString() );
			    answer += ".";
			    answer += subString.getString();
			    return answer;
			}
		    }
		    else if ( cat->isList(i) ) {

			SoNodeKitListPart *lst = (SoNodeKitListPart *) iPart;

			for (int indx = 0; 
			     indx < lst->getNumChildren(); indx++ ) {
			    SoNode *kid = lst->getChild(indx);
			    if (kid == fullP->getTail()) {
				char indxString[30];
				sprintf(indxString, "[%d]", indx );
				SbString answer( cat->getName(i).getString() );
				answer += indxString;
				return answer;
			    }
			    else if (kid->isOfType(SoBaseKit::getClassTypeId())) {
				// Try to look inside the nodekit part.
				SbString subString 
				    = ((SoBaseKit *)kid)->getPartString(part);
				if (subString != "") {
				    char indxString[30];
				    sprintf(indxString, "[%d]", indx );
				    SbString answer( cat->getName(i).getString() );
				    answer += indxString;
				    answer += ".";
				    answer += subString.getString();
				    return answer;
				}
			    }
			}
		    }
		}

		// Otherwise, increment the pathIndex and.
		// Keep on looking through the catalog...
		// We can keep using the same loop, since
		// successive entries in a path should always proceed to 
		// increasing indices in the parts list. This is because 
		// when classes define catalogs, entries must be added as 
		// children of pre-existing parts. 
		pathIndex++;
	    }

	}
    }

    // we didn't find a match...
    return "";
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//     Returns a path that begins with 'this', and ends at 
//     the part named.  
//
//     If 'pathToExtend' is not NULL:
//         the returned path will be a copy of 'pathToExtend' with the new 
//         path appended to it. In order to append, however, the following 
//         condition must be met:
//             'this' must lie on the pathToExtend, otherwise NULL is returned.
//         If 'this' lies on pathToExtend, then a copy is made, and the
//             copy is truncated to end at 'this.' Finally, the path from 'this'
//             down to the part will be appended.
//     
//
// Use: public

SoNodeKitPath *
SoBaseKit::createPathToAnyPart(const SbName &partName, 
			    SbBool makeIfNeeded, SbBool leafCheck,
			    SbBool publicCheck, const SoPath *pathToExtend )
//
////////////////////////////////////////////////////////////////////////
{
    // Return if pathToExtend is non-NULL but doesn't contain 'this'
    if (   pathToExtend != NULL &&
	 ((const SoFullPath *)pathToExtend)->containsNode(this) == FALSE ) {
#ifdef DEBUG
	    SoDebugError::post("SoBaseKit::createPathToAnyPart",
	    "The given pathToExtend does not contain this node.Returning NULL");
#endif
	return NULL;
    }

    SoFullPath *thePath = nodekitPartsList->createPathToAnyPart( partName, 
					makeIfNeeded, leafCheck, publicCheck );
    if ( thePath == NULL )
	return NULL;

    if ( pathToExtend == NULL )
	return (SoNodeKitPath *) thePath;

    const SoFullPath *fullPathToExtend = (const SoFullPath *) pathToExtend;

    thePath->ref();
    fullPathToExtend->ref();

    // Create a copy of 'fullPathToExtend' with 'thePath' tacked onto it

    // First, copy fullPathToExtend into longPath
    SoFullPath *longPath = (SoFullPath *) fullPathToExtend->copy();
    longPath->ref();

    // Now, truncate longPath to end at 'this'
    while ( longPath->getTail() != this )
	longPath->pop();

    // Finally, append 'thePath' after 'longPath'.  Leave out thePath->head(), 
    // since it's already at the tail of longPath...
    for( int i = 1; i < thePath->getLength(); i++ )
	longPath->append( thePath->getIndex( i ) );

    thePath->unref();
    fullPathToExtend->unref();
    longPath->unrefNoDelete();

    return (SoNodeKitPath *)longPath;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//     Returns a path that begins with 'this', and ends at 
//     the part named.  
//
//     If 'pathToExtend' is not NULL:
//         the returned path will be a copy
//         of 'pathToExtend' with the new path appended to it. In order to 
//         append, however, the following condition must be met:
//             pathToExtend->getTail() must equal 'this', 
//             otherwise NULL will be returned.
//     
//
// Use: public

SoNodeKitPath *
SoBaseKit::createPathToPart(const SbName &partName, 
			 SbBool makeIfNeeded, const SoPath *pathToExtend )
//
////////////////////////////////////////////////////////////////////////
{
    // the fourth and fifth arguments, leafCheck and publicCheck, are TRUE, 
    // because we don't ordinarily return parts unless they are public leaves.
    return ( (SoNodeKitPath *) createPathToAnyPart( partName, makeIfNeeded, 
				TRUE, TRUE, pathToExtend ) );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Return the part with the given name
//
// Use: public

SbBool
SoBaseKit::setPart( const SbName &partName, SoNode *from )
//
////////////////////////////////////////////////////////////////////////
{
    // the third argument, anyPart, is FALSE, because we don't ordinarily
    // return parts unless they are public leaves.
    return ( setAnyPart( partName, from, FALSE ) );
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Return the part with the given name
//
// Use: protected

SbBool
SoBaseKit::setAnyPart( const SbName &partName, SoNode *from, SbBool anyPart )
//
////////////////////////////////////////////////////////////////////////
{
    if ( from )
        from->ref();
    SbBool answer = nodekitPartsList->setAnyPart( partName, from, anyPart );
    if ( from )
        from->unref();
    return answer;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets field values in the desired parts of the nodeKit...
//    The argument string contains a list of  desiredNodeName/fieldData
//    string pairs.  The string is parsed, and subsequent calls are made
//    to:     theNode = getPart( desiredNodeName);
//    and:    theNode->set( fieldData );
//
// Use: public

SbBool
SoBaseKit::set(char *nameValuePairListString) 
		            // the string to use in setting the values
//
////////////////////////////////////////////////////////////////////////
{
    char *string = strdup( nameValuePairListString );
    char *c;
    SbBool	success = TRUE;
    SoNode     *node;

    while(*string) {
	skipWhiteSpace(string);

	if(!*string)
	    break;

	// look for the node name
	c = string;
	while(*c && !isspace(*c) && *c != '{')
	    c++;

	if(!*c) 		// no more data, return
	    break;

	int length = c - string + 1;
	char *desiredNodeName = new char [length];
	strncpy(desiredNodeName, string, c - string);
	desiredNodeName[c - string] = 0;
	string = c;

	// find that node
	node = getPart(desiredNodeName, TRUE);

	delete [ /*length*/ ] desiredNodeName;

	if (node == NULL)      // no node found, return
	    break;

	// get the field data
	skipWhiteSpace(string);
	if(*string != '{') {
#ifdef DEBUG
	    SoDebugError::post("SoBaseKit::set",
		"Found char '%c' in string \"%s\", expected '{'",
		*string, nameValuePairListString);
#endif
	    success = FALSE;
	    break;
	}

	string++;
	c = string;
	while(*c && *c != '}')
	    c++;

	length = c - string + 1;
	char *fielddata = new char [length];
	strncpy(fielddata, string, c - string);
	fielddata[c - string] = 0;
	string = c + 1;

	// call the SoNode::set method to set the fields
	success &= node->set(fielddata);

	delete [ /*length*/ ] fielddata;
    }
    return success;
}

#define __SO_COMMENT_CHAR	'#'

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets field values in the desired parts of the nodeKit...
//    The partNameString contains the name of a part to set values for.
//    It may be of the form 'part1.part2.part3', so that the kit will
//    search within it's template for the node.
//    Next, calls the node's set, using parameterString as the input... 
//
// Use: public

SbBool
SoBaseKit::set(char *partNameString,     // name of the part
	       char *parameterString )   // values for the part
//
////////////////////////////////////////////////////////////////////////
{
    SoNode *node = getPart(partNameString, TRUE);

    // call the SoNode::set method to set the fields
    if ( node == NULL )
	return FALSE;

    if ( node->set(parameterString) )
	return TRUE;
    else
	return FALSE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Creates the nodekitPartsList list.
//    Should be called once during the constructor for each instance.
//
//
//
// Use: protected

void
SoBaseKit::createNodekitPartsList()
//
////////////////////////////////////////////////////////////////////////
{
    if ( getNodekitCatalog() != NULL ) {
	if (nodekitPartsList)
	    delete nodekitPartsList;
        nodekitPartsList = new SoNodekitParts( this );
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Creates the parts in this nodekit for which
//    nullByDefault is FALSE in the nodekitCatalog
//
// Use: protected

void
SoBaseKit::createDefaultParts()
//
////////////////////////////////////////////////////////////////////////
{
    const SoNodekitCatalog *cat = getNodekitCatalog();
    SoSFNode *theField;
    SoNode *oldPart, *myPart;

    // Start at index 1. Don't want to do anything to 'this', index 0
    for ( int i = 1; i < cat->getNumEntries(); i++ ) {
	const SbName pName = cat->getName(i);
	theField = nodekitPartsList->fieldList[i];
	// If part is created by default and either 
	//    [a] hasn't been created yet
	// or [b] has been created, but the default type has changed...
	if (cat->isNullByDefault(i) == FALSE ) {
	    oldPart = theField->getValue();
	    if (   oldPart == NULL 
		|| oldPart->isOfType( cat->getDefaultType(i) ) == FALSE ) {
		myPart = (SoNode *) cat->getDefaultType(i).createInstance();
		setAnyPart( cat->getName(i) , myPart );
	    }
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Skips white space in strings, used by the set method.
//
// Use: private

void
SoBaseKit::skipWhiteSpace(char *&string)
//
////////////////////////////////////////////////////////////////////////
{
    // Keep going while space and comments appear
    // Skip over space characters
    while (*string && isspace(*string))
	string++;

    // If next character is comment character, flush until end of line
    if (*string == __SO_COMMENT_CHAR) {
	while (*string)
	 string++;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Skips white space in strings, used by the set method.
//
// Use: protected

void
SoBaseKit::catalogError()
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEBUG
    SoDebugError::post("SoBaseKit::catalogError",
	"ERROR creating nodekit catalog for class %s Did you remember to call initClass?",
	     getTypeId().getName().getString() );
#endif
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    detach/attach any sensors, callbacks, and/or field connections.
//    Called by:            start/end of SoBaseKit::readInstance
//    and on new copy by:   start/end of SoBaseKit::copy.
//    Classes that redefine must call setUpConnections(TRUE,TRUE) 
//    at end of constructor.
//    Returns the state of the node when this was called.
//
SbBool 
SoBaseKit::setUpConnections( SbBool onOff, SbBool doItAlways )
{
    if ( !doItAlways && connectionsSetUp == onOff)
	return onOff;
    return !(connectionsSetUp = onOff);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads the fields for the nodekit.
//    Then it sets the parts in the kit based on the field values.
//    Also, is careful to keep any pre-defined nodes that are not 
//    given values in the file.
//
// Use: protected

SbBool
SoBaseKit::readInstance( SoInput *in, unsigned short /*flags*/ )
//
////////////////////////////////////////////////////////////////////////
{
    SbBool readOK = TRUE;

    // [0] First, turn off notification and connections for this node
	SbBool saveNotify = enableNotify(FALSE);
	SbBool wasSetUp = setUpConnections( FALSE );

    // [1] make a temporary list of all the part fields.
	int      numParts       = nodekitPartsList->numEntries;
	// firstPartField is 1. Part 0 is 'this' and is not treated here.
	int      firstPartField = 1;
	SoSFNode **realPartFldLst = nodekitPartsList->fieldList;
	SoSFNode **tempPartFldLst = new ( SoSFNode *[numParts] );

	int i;
	for ( i = firstPartField; i < numParts; i++ ) {
	    tempPartFldLst[i] = new SoSFNode;
	    tempPartFldLst[i]->setValue(   realPartFldLst[i]->getValue()  );
	    tempPartFldLst[i]->setDefault( realPartFldLst[i]->isDefault() );
	}

    // [2] clear out the child list (remove all hidden children)
	for ( i = getNumChildren() - 1; i >= 0; i-- )
	    removeChild( i );

    // [3] clear out the real part fields (remember, we've made a copy!)
	for ( i = firstPartField; i < numParts; i++ ) {
	    realPartFldLst[i]->setValue( NULL );
	    realPartFldLst[i]->setDefault( TRUE );
	}

    // [4] Try to read the fields.  The unknownFieldData will contain
    //     any fields that are not listed in this catalog. We will try
    //     to use them later, with setPart().  For example, the
    //     field      
    //          appearance.material Material { diffuseColor 1 0 0 }
    //     or simply
    //          material Material { diffuseColor 1 0 0 }
    //     will be readable by an SoSeparatorKit, even though neither
    //     "material" nor "appearance.material" appears as a name in our
    //     fieldData.
	SoFieldData *unknownFieldData = new SoFieldData;
	if ( !readMyFields(in, unknownFieldData ) )
	    readOK = FALSE;

    // [5] copy any values that were read from file into
    //     the tempPartFldLst. These new values should replace old ones,
    //     since things in the file take precedence. However, if nothing
    //     was said about the part in file, we keep the old value in
    //     tempPartFldLst.
	if ( readOK == TRUE ) {
	    for ( i = firstPartField; i < numParts; i++ ) {
		if ( realPartFldLst[i]->isDefault() == FALSE )
	          tempPartFldLst[i]->setValue(  realPartFldLst[i]->getValue() );
	    }
	}

    // [6] At this point, all the parts we want to keep are stored in the 
    //     tempPartFldLst.
    //     Clear out the child list and the realPartFldLst. This readies
    //     us to set values from every part listed in tempPartFldLst.
	if ( readOK == TRUE ) {
	    for ( i = getNumChildren() - 1; i >= 0; i-- )
		removeChild( i );

	    for ( i = firstPartField; i < numParts; i++ ) {
		realPartFldLst[i]->setValue(NULL);
		realPartFldLst[i]->setDefault(TRUE);
	    }
	}

    // [7] call setPart() for every non-NULL or non-default part in the saved 
    //     part list.  (must call if value == NULL but isDefault() == FALSE,
    //     because we may get rid of some ancestors that way...)
    //     Then, our hidden children and part fields will be consistent.
    // [7a] Skip part 0, because part 0 is "this"
    // [7c] For non-leaves, remove all children.
    //      They will be added back when (and if) setPart() is 
    //      called on the child.
    // [7d] Set the part, using setPartFromThisCatalog()
    // [7e] Copy EVERY default flag from tempPartFldLst to 
    //      realPartFldLst. Sometimes, a NULL part has 
    //      (default == FALSE).
	if ( readOK == TRUE ) {

	    const SoNodekitCatalog *cat = getNodekitCatalog();
    	    // [7a] Skip part 0, because part 0 is "this"
	    for ( i = firstPartField; i < numParts; i++ ) {
		if ( tempPartFldLst[i]->getValue() != NULL ||
		     tempPartFldLst[i]->isDefault() == FALSE) {
		    SoNode *n = tempPartFldLst[i]->getValue();
		    if (n)
			n->ref();

		    // [7c] For non-leaves, remove all children.
		    //      They will be added back when (and if) setPart() is 
		    //      called on the child.
		    if (n && cat->isLeaf(i) == FALSE ) {
			SoGroup *g = (SoGroup *) n;
			for (int j = g->getNumChildren()-1; j>=0; j-- )
			    g->removeChild(j);
		    }

    		    // [7d] Set the part, using setPartFromThisCatalog()
		    nodekitPartsList->setPartFromThisCatalog( i, n, TRUE );

		    if (n)
			n->unref();
		}
		// [7e] Copy EVERY default flag from tempPartFldLst to 
		//      realPartFldLst. Sometimes, a NULL part has 
		//      (default == FALSE).
		realPartFldLst[i]->setDefault( tempPartFldLst[i]->isDefault());
	    }
	}

    // [8] Now attempt to put the unknown fields as parts, using setPart.
	if (readOK == TRUE ) {

	    // Now attempt to put the unknown fields as parts, using setAnyPart.
	    SbName partName;
	    SoNode *pNode;
	    SoSFNode *pNodeField;
	    for ( i = 0; i < unknownFieldData->getNumFields(); i++ ) {

		partName = unknownFieldData->getFieldName(i);
		pNodeField = (SoSFNode *) unknownFieldData->getField(this,i);
		pNode = pNodeField->getValue();

		setAnyPart( partName, pNode );
	    }
	}

    // [9] delete the temporary storage.
	// Each entry in unknownFieldData contains an SoSFField that we
	// created on the fly to point to a part that was then unknown.
	// We need to delete these fields.  Note that, usually, one does
	// not go inside fieldData and delete the fields. This is because the
	// 'real owner' of the field is usually a node, and the fieldData just
	// stores an extra pointer. In this case, unknownFieldData is the
	// owner of the field.
	for ( i = 0; i < unknownFieldData->getNumFields(); i++ ) {
	    SoSFNode *byebye = (SoSFNode *) unknownFieldData->getField(this,i);
	    delete byebye;
	}
	delete unknownFieldData;

	for ( i = firstPartField; i < numParts; i++ ) {
	    delete tempPartFldLst[i];
	}
	delete [] tempPartFldLst;

    // Turn connections and notification back on for node
	setUpConnections( wasSetUp );
	enableNotify(saveNotify); 

    return readOK;
}

SbBool 
SoBaseKit::readMyFields(SoInput *in, SoFieldData *&unknownFieldData )
{
    const SoFieldData *fieldData = getFieldData();

    // If the file is binary, then no special stuff...
    if ( in->isBinary() ) {
	SbBool notBuiltIn; // Not used
	return fieldData->read(in, this, TRUE, notBuiltIn);
    }

    // Otherwise, we have special code to read fields.
    // It allows for 'parts within parts.' If the fieldName is not
    // recognizable, then we assume it's the name of an embedded part and 
    // go ahead reading the node which follows. Later, we will call setPart
    // to insert it down below.

    // The code below is ripped off and modified from SoFieldData::read()
    // But instead of looking only for names that are valid identifiers,
    // we allow any string not beginning with '}'

    // Only check for "fields" or "inputs" the first time:
    SbBool firstTime = TRUE;
    SbName fieldName;

    // Keep reading fields until done
    while (TRUE) {

	// Read first character - if none, EOF
	char nextChar;
	if (!in->read( nextChar ))
	    return FALSE;

	// See if its a close bracket, then put the character back.
	SbBool closeBracket = (nextChar == '}');
	in->putBack(nextChar);

	// If it was a closeBracket, we are done here.
	if (closeBracket)
	    return TRUE;

	// Otherwise, read the next name, but don't require an
	// identifier. That way, we can get partNames like
	// duck.foot.toe[4] to read in.
	if (! in->read(fieldName, FALSE) || !fieldName)
	    return TRUE;

	// Read field descriptions.  Field descriptions may be
	// given for built-in nodes, and do NOT have to be given
	// for non-built-in nodes as long as their code can be
	// DSO-loaded.
	if (firstTime) {
	    firstTime = FALSE;
	    if (fieldName == "fields") {
		if (!fieldData->readFieldDescriptions(in, this, 1<<20))
		    return TRUE;
		continue;
	    }
	}

	SbBool foundName;
	if (! fieldData->read(in, this, fieldName, foundName))
	    return FALSE;

	// No match with any valid field name
	// Assume it's gonna be an embedded part, of type SoSFNode
	if (!foundName) {

	    // Add a node pointer field with this name to
	    // the unknownFieldData.
	    unknownFieldData->addField(this,fieldName.getString(),
				       new SoSFNode);

	    // Tell the unknownFieldData to read in this field...
	    if ( !unknownFieldData->read(in,this, fieldName, foundName))
		return FALSE;
	}
    }
}


#ifdef DEBUG
SoNode *
SoBaseKit::typeCheck( const SbName &partName, const SoType &partType,
		      SoNode *node )
#else  /* DEBUG */
SoNode *
SoBaseKit::typeCheck( const SbName &, const SoType &partType,
		      SoNode *node )
#endif /* DEBUG */
{
    if (     node != NULL && !node->isOfType( partType ) ) {
#ifdef DEBUG
	const char *typeName = partType.getName().getString();
	SoDebugError::post("SoBaseKit::typeCheck",
			   "ERROR getting part! The part you asked for, "
			   "\"%s\", is not of the type you asked for, %s", 
			   partName.getString(), typeName);
#endif
	return NULL;
    }
    else
	return node;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Redefines this to add this node and all part nodes to the dictionary.
//
// Use: protected, virtual

SoNode *
SoBaseKit::addToCopyDict() const
//
////////////////////////////////////////////////////////////////////////
{
    // If this node is already in the dictionary, nothing else to do
    SoNode *copy = (SoNode *) checkCopy(this);
    if (copy == NULL) {

	// Create and add a new instance to the dictionary
	copy = (SoNode *) getTypeId().createInstance();
	copy->ref();
	addCopy(this, copy);		// Adds a ref()
	copy->unrefNoDelete();

	// Recurse on all non-NULL parts. Skip part 0, which is "this".
	for (int i = 1; i < nodekitPartsList->numEntries; i++) {
	    SoNode *partNode = nodekitPartsList->fieldList[i]->getValue();
	    if (partNode != NULL)
		partNode->addToCopyDict();
	}
    }

    return copy;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Copies the contents of the given nodekit into this instance.
//
// Use: protected, virtual

void
SoBaseKit::copyContents(const SoFieldContainer *fromFC, SbBool copyConnections)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool wasSetUp = setUpConnections( FALSE );

    // Copy the standard node stuff (fields and names)
    SoNode::copyContents(fromFC, copyConnections);

    const SoBaseKit *origKit = (const SoBaseKit *) fromFC;

    // We've got to do extra work now, for two reasons.
    // First, we've got to make copies of all the parts. This is because the
    //        parts are stored in SoSFNode fields, which only copy the pointer 
    //        and then call ref().  We need brand new copies.
    // Second, nodekits have hidden children.  Some may have been 
    //        created during the constructor. But these pointers will differ
    //        from the part pointers in the field, so we want them deleted.
    //        The original also has hidden children that were created
    //        by setPart(), which we are missing. So we'll need to remove the
    //        hidden children, then insert them by calling setPart() repeatedly
    //        on all the parts.
    //
    // So, we need to:
    //  [1] create a temporary list to store copies of all part fields.
    //  [2]  copy each part and store it in the temporary list.
    //    [2a] If NULL, just use NULL.
    //    [2b] else if parent part is 'this' use the copy of oldPart
    //    [2c] else we made this part already when we copied the 
    //         ancestor part whose parent was 'this.' Find the parent.
    //         Then figure out which child to use for the part and
    //         set the part to be that node.
    //    [2d] Copy the default flag.
    //  [3] clear out the child list (remove all hidden children)
    //  [4] clear out the real part fields (remember, we've made copies!)
    //  [5] call setPart() for every non-NULL part in the saved part list.
    //      After this, our hidden children and part fields will be consistent.
    //  [6] delete the temporary copy of the part fields.
    //  [7] undo temporary ref of the copy.

    //  [1] create a temporary list to store copies of all part fields.
	int numParts = origKit->nodekitPartsList->numEntries;
	// firstPartField is 1. Part 0 is 'this' and is not treated here.
	int      firstPartField = 1;
	SoSFNode **realPartFldLst = nodekitPartsList->fieldList;
	SoSFNode **tempPartFldLst = new SoSFNode *[numParts];

	int i;
	for ( i = firstPartField; i < numParts; i++ )
	    tempPartFldLst[i] = new SoSFNode;

    //  [2]  copy each part and store it in the temporary list.
    //  [2a] If NULL, just use NULL.
    //  [2b] else if parent part is 'this' use the copy of oldPart
    //  [2c] else we made this part already when we copied the 
    //       ancestor part whose parent was 'this.' Find the parent.
    //       Then figure out which child to use for the part and
    //       set the part to be that node.
    //  [2d] Copy the default flag.
	const SoNodekitCatalog *cat = origKit->getNodekitCatalog();
	SoSFNode **origPartFldLst   = origKit->nodekitPartsList->fieldList;
	SoNode  *copyPart,            *origPart;
	SoNode  *copyParentPartNode,  *origParentPartNode;
	SoGroup *copyParentPartGroup, *origParentPartGroup;
	int      parentPNum;
	SoType  grpType = SoGroup::getClassTypeId();

	for ( i = firstPartField; i < numParts; i++ ) {
	    if (realPartFldLst[i]->getValue() == NULL ) {
    	        //  [2a] If NULL, just use NULL.
		copyPart = NULL;
	    }
	    else if ((parentPNum = cat->getParentPartNumber(i)) 
						== SO_CATALOG_THIS_PART_NUM) { 
		//  [2b] else if parent part is 'this' use the copy of
		//  oldPart. The copy has already been created, but we
		//  need to fill in its contents.
		copyPart = realPartFldLst[i]->getValue();
		copyPart->copyContents(origPartFldLst[i]->getValue(),
				       copyConnections);
	    }
	    else {
		//  [2c] else we made this part already when we copied the 
		//       ancestor part whose parent was 'this.' Find the parent.
		//       Then figure out which child to use for the part and
		//       set the part to be that node.
		copyParentPartNode = tempPartFldLst[ parentPNum ]->getValue();
		origParentPartNode = origPartFldLst[ parentPNum ]->getValue();
#ifdef DEBUG
		if ( !copyParentPartNode || !origParentPartNode )
		    SoDebugError::post("SoBaseKit::copyContents",
			"copy or original of parent part is NULL");
		if ( !copyParentPartNode->isOfType(grpType) ||
		     !origParentPartNode->isOfType(grpType) )
		    SoDebugError::post("SoBaseKit::copyContents",
			"copy or original of parent part not an SoGroup");
#endif

		copyParentPartGroup = (SoGroup *) copyParentPartNode;
		origParentPartGroup = (SoGroup *) origParentPartNode;

		origPart = origPartFldLst[i]->getValue();
		int kidIndex = origParentPartGroup->findChild( origPart ); 

#ifdef DEBUG
		if ( kidIndex < 0 )
		    SoDebugError::post( "SoBaseKit::copyContents",
		       "original part not found under original part parent");
		if ( copyParentPartGroup->getNumChildren() < (kidIndex + 1) )
		    SoDebugError::post( "SoBaseKit::copyContents",
		       "copy of parent part does not have enough children");
#endif

		copyPart = copyParentPartGroup->getChild(kidIndex);
	    }

	    tempPartFldLst[i]->setValue(  copyPart );

    	    //  [2d] Copy the default flag.
	    tempPartFldLst[i]->setDefault(realPartFldLst[i]->isDefault() );
	}

    //  [3] clear out the child list (remove all hidden children)
	for ( i = getNumChildren() - 1; i >= 0; i-- )
	    removeChild( i );

    //  [4] clear out the real part fields (remember, we've made copies!)
	for ( i = firstPartField; i < numParts; i++ ) {
	    realPartFldLst[i]->setValue( NULL );
	    realPartFldLst[i]->setDefault( TRUE );
	}

    // [5]  Call setPart() for every non-NULL part in the saved part list.
    //      After this, our hidden children and part fields will be consistent.
    // [5a] Skip part 0, because part 0 is "this"
    // [5c] For non-leaves, remove all children.
    //      They will be added back when (and if) setPart() is 
    //      called on the child.
    // [5d] Set the part, using setPartFromThisCatalog()
    // [5e] Copy EVERY default flag from tempPartFldLst to 
    //      realPartFldLst. Sometimes, a NULL part has 
    //      (default == FALSE).
	// [5a] Skip part 0, because part 0 is "this"
	for ( i = firstPartField; i < numParts; i++ ) {
	    if ( tempPartFldLst[i]->getValue() != NULL ) {
		SoNode *n = tempPartFldLst[i]->getValue();
		n->ref();

		// [5c] For non-leaves, remove all children.
		//      They will be added back when (and if) setPart() is 
		//      called on the child.
		if (cat->isLeaf(i) == FALSE ) {
		    SoGroup *g = (SoGroup *) n;
		    for (int j = g->getNumChildren()-1; j>=0; j-- )
			g->removeChild(j);
		}

		// [5d] Set the part, using setPartFromThisCatalog()
		nodekitPartsList->setPartFromThisCatalog( i, n, TRUE );

		n->unref();
	    }
	    // [5e] Copy EVERY default flag from tempPartFldLst to 
	    //      realPartFldLst. Sometimes, a NULL part has 
	    //      (default == FALSE).
	    realPartFldLst[i]->setDefault( tempPartFldLst[i]->isDefault() );
	}

    //  [6] delete the temporary copy of the part fields.
	for ( i = firstPartField; i < numParts; i++ ) {
	    delete tempPartFldLst[i];
	}
	delete [] tempPartFldLst;


    //  [7] re-instate connections of the copy.
	setUpConnections( wasSetUp );
}

SoChildList *
SoBaseKit::getChildren() const
{
   return children; 
}

void 
SoBaseKit::doAction( SoAction *action )
{
    int         numIndices;
    const int   *indices;

    if (action->getPathCode(numIndices, indices) == SoAction::IN_PATH)
	children->traverse(action, 0, indices[numIndices - 1]);
    else
	children->traverse(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements get matrix action.
//
// Use: protected

void
SoBaseKit::getMatrix(SoGetMatrixAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    int         numIndices;
    const int   *indices;

    // Only need to compute matrix if group is a node in middle of
    // current path chain or is off path chain (since the only way
    // this could be called if it is off the chain is if the group is
    // under a group that affects the chain).
    switch (action->getPathCode(numIndices, indices)) {

	case SoAction::NO_PATH:
	    break;

	case SoAction::IN_PATH:
	    children->traverse(action, 0, indices[numIndices - 1]);
	    break;

	case SoAction::BELOW_PATH:
	    break;

	case SoAction::OFF_PATH:
	    children->traverse(action);
	    break;
    }
}


// These functions implement all actions for nodekits.
void 
SoBaseKit::callback( SoCallbackAction *action )
{ 
    SoBaseKit::doAction( action );
}

void 
SoBaseKit::GLRender( SoGLRenderAction *action )
{ SoBaseKit::doAction( action ); }

void 
SoBaseKit::getBoundingBox( SoGetBoundingBoxAction *action )
{ 
    SbVec3f totalCenter(0,0,0);
    int     numCenters = 0;
    int     numIndices;
    const int *indices;
    int     lastChild;

    if (action->getPathCode(numIndices, indices) == SoAction::IN_PATH)
	lastChild = indices[numIndices - 1];
    else
	lastChild = getNumChildren() - 1;

    for (int i = 0; i <= lastChild; i++) {
	children->traverse(action, i, i);
	if (action->isCenterSet()) {
	    totalCenter += action->getCenter();
	    numCenters++;
	    action->resetCenter();
	}
    }
    // Now, set the center to be the average:
    if (numCenters != 0)
	action->setCenter(totalCenter / numCenters, FALSE);
}

void 
SoBaseKit::handleEvent( SoHandleEventAction *action )
{ SoBaseKit::doAction( action ); }

void 
SoBaseKit::rayPick( SoRayPickAction *action )
{ 

    SoBaseKit::doAction( action ); 

    // Look at the picked point. If no detail has been constructed yet for 
    // this node in the pickedPoint's path list, then do so. (In the case
    // of instancing, the detail might already exist.)
    const SoPickedPointList &pointList 
		    = (const SoPickedPointList &) action->getPickedPointList();
    SoPickedPoint *kidPoint;
    SoFullPath    *pointPath;

    for (int i = 0; i < pointList.getLength(); i++ ) {

	kidPoint = pointList[i]; 
	pointPath = (SoFullPath *) kidPoint->getPath(); 

	// See if there is a detail for this node.
	if (   pointPath->containsNode( this ) 
	    && kidPoint->getDetail(this) == NULL ) {

	    // Set up the nodekit detail.
	    SoNodeKitDetail *newDetail = new SoNodeKitDetail;

	    newDetail->setNodeKit( this );

	    // Walk down the path until you find a node that is a leaf 
	    // in this kit's catalog. That will be the 'part' in the 
	    // detail
		// First, find index of 'this' in path
		int startIndex;
		for ( int j = 0; j < pointPath->getLength(); j++ ) {
		    startIndex = j;
		    if ( pointPath->getNode(j) == this )
			break;
		}
		// Next, walk down path starting at startIndex
		const SoNodekitCatalog *cat = getNodekitCatalog();
		SoNode *thePart;
		int    thePartNum, partPathIndex;
		for ( int k = startIndex; k < pointPath->getLength(); k++ ) {
		    thePart = pointPath->getNode(k);
		    partPathIndex = k;
		    // Find the part number of this part in the catalog
		    for (int l = 0; l < nodekitPartsList->numEntries; l++ ){
			thePartNum = l;
			if (l==0) {
			    if (nodekitPartsList->rootPointer == thePart)
				break;
			}
			else {
			    if ( nodekitPartsList->fieldList[l]->getValue()
				 == thePart )
				break;
			}
		    }

		    if (cat->isLeaf(thePartNum) == TRUE )
			break;
		}

	    // If the part was a nodeKitListPart, then see if one
	    // of it's children is also in the path. If so, we'll
	    // give back a part name like "childList[0]" instead of 
	    // just childList.
		int childNumber = -1;
		if (thePart->isOfType(SoNodeKitListPart::getClassTypeId())){
		    if ( pointPath->getLength() >= (partPathIndex + 2) ) {
			// The path contains the grandchild of the list 
			// part. (The child is the container, the 
			// grandchild is the actual 'child' that we expose)
			childNumber = pointPath->getIndex(partPathIndex+2);
		    }
		}

	    newDetail->setPart( thePart );
	    SbName thePartName;
	    if ( childNumber == -1 )
		thePartName = cat->getName(thePartNum);
	    else {
		char fullStr[100];
		sprintf(fullStr, "%s[%d]", 
			cat->getName(thePartNum).getString(), childNumber );
		thePartName = fullStr;
	    }

	    newDetail->setPartName( thePartName );

	    kidPoint->setDetail( newDetail, this );
	}
    }
}

void 
SoBaseKit::search( SoSearchAction *action )
{ 
    SoNode::search(action);
    if (isSearchingChildren())
        SoBaseKit::doAction( action );
}

void 
SoBaseKit::write( SoWriteAction *action )
{ 
    SoOutput *out = action->getOutput();

    if (out->getStage() == SoOutput::COUNT_REFS) {

	// Increment the write reference count. We have our own version
	// that treats fields for our parts differently.
	// If a part field is set to default, we try not to write it.
	// So we give it a field-style reference.  If, in addition, the
	// part is of type SoBaseKit, we'll count the fields within that
	// part as well. This insures that if a part nested beneath our part 
	// must write, then our part will write as well.
	addWriteReference(out);

	// If this environment variable is set, we are going to write 
	// out the children the same way that SoGroup does. This is ONLY for 
	// debugging purposes.
#ifdef DEBUG
	if (!out->isBinary() &&
	    SoDebug::GetEnv("IV_DEBUG_WRITE_KIT_CHILDREN")) {
	    if ( !hasMultipleWriteRefs())
		SoBaseKit::doAction(action);
	}
#endif
    }

    else if (! writeHeader(out, FALSE, FALSE)) {

	// We are in the WRITE stage.  We can assume that if we got here, 
	// this instance has at least one write reference.  
	// Write out fields (including part fields!) using
	// 'fieldDataForWriting. Then delete 'fieldDataForWriting'.
	if (fieldDataForWriting != NULL) {

	    const SoNodekitCatalog *cat      = getNodekitCatalog();
	    SoSFNode               **pFields = nodekitPartsList->fieldList;
	    int                    numParts  = nodekitPartsList->numEntries;

	    // Determine if we must force any default parts to write and
	    // call setDefault(FALSE) on them. This way, they'll write.
	    // The simple test is the regular shouldWrite().  If this
	    // fails on part of type nodekit, we must also check all of its 
	    // nested parts before being certain.
	    SoNode    *partNode;
	    for ( int i = 1; i < numParts; i++ ) {
		if ( ! pFields[i]->isDefault() )
		    continue;
		partNode = pFields[i]->getValue();
		if ( !partNode )
		    continue;
		if ( partNode->shouldWrite()) {
		    pFields[i]->setDefault(FALSE);
		    continue;
		}
		if ( partNode->isOfType( SoBaseKit::getClassTypeId()) &&
		    ((SoBaseKit *)partNode)->forceChildDrivenWriteRefs(out)) {
		    // If the kit has changed its mind and upped its
		    // write ref count because a descended child must write,
		    // then:
		    pFields[i]->setDefault(FALSE);
		    continue;
		}
	    }

	    // Now, do the usual writing bit.
	    fieldDataForWriting->write(out, this);

	    // Get rid of fieldDataForWriting.
	    delete fieldDataForWriting;
	    fieldDataForWriting = NULL;
	}

#ifdef DEBUG
	if ( !out->isBinary() &&
	    SoDebug::GetEnv("IV_DEBUG_WRITE_KIT_CHILDREN")) {

	    // If this env variable is set, write children like an SoGroup.
	    // This is ONLY for debugging purposes, to check for consistency
	    // of childList and part fields.

	    fprintf(stderr,"NODEKIT DEBUG: Writing Children of this NodeKit\n");
	    fprintf(stderr,"    to file. Do not try to read in the result\n");

	    // In the writing stage, we can't use standard traversal,
	    // because if we are writing out one path of a path list, we
	    // might need to write out children that are not relevant to
	    // the current path (but are relevant to another path in the
	    // list). Therefore, we implement a different traversal that
	    // writes out each child that is referenced.
	    for (int i = 0; i <= getNumChildren(); i++)
		if (getChild(i)->shouldWrite())
		    children->traverse(action, i);
	}
#endif

	writeFooter(out);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Overrides this method to use countMyFields instead of
//    the regular SoFieldData writing mechanism.
//
// Use: internal, virtual

void
SoBaseKit::addWriteReference(SoOutput *out, SbBool isFromField)
//
////////////////////////////////////////////////////////////////////////
{
    // Do standard stuff
    SoBase::addWriteReference(out, isFromField);

    // If this is the first reference to this instance and we are not
    // being referenced from a field-to-field connection, give fields
    // a chance to do their thing
    // We can not use the regular fieldData write method, because
    // we do special stuff with our part fields.
    if (! isFromField && ! hasMultipleWriteRefs() ) {

	countMyFields( out );
    }
}

// This returns TRUE if the nodekit intends to write out.
//   [a] call shouldWrite(). If TRUE, trivial return.
//   [b] If the kit thinks it shouldn't write, it first does a recursive
//       call to its children.  If any children must write, then so must
//       the kit.
//   [c] If kit has changed its mind because of [b], then add a writeRef.
//
//   [d] If kit should not write, it will delete the fieldDataForWriting, 
//       since there will no writing pass applied to take care of this.
SbBool 
SoBaseKit::forceChildDrivenWriteRefs( SoOutput *out )
{
    //   [a] call shouldWrite(). If TRUE, trivial return.
    if ( shouldWrite() ) 
	return TRUE;

    // If NULL, we've already failed this test once (and therefore deleted it), 
    // or never even attempted to count references.
    if ( fieldDataForWriting == NULL )
	return FALSE;

    //   [b] If the kit thinks it shouldn't write, it first does a recursive
    //       call to its children.  If any children must write, then so must
    //       the kit.
    SbBool writeMe = FALSE;

    const SoNodekitCatalog *cat = getNodekitCatalog();
    int       partNumber;
    SoField	 *field;
    SoSFNode *partField;
    SoNode   *partNode;

    for (int i = 0; i < fieldDataForWriting->getNumFields(); i++) {

	field       = fieldDataForWriting->getField(this, i);

	// If field is not default, we write...
	if ( ! field->isDefault() ) {
	    writeMe = TRUE;
	    break;
	}

	partNumber  = cat->getPartNumber(fieldDataForWriting->getFieldName(i));

	if ( partNumber == SO_CATALOG_NAME_NOT_FOUND ) {
	    // non-parts that are ignored cause us to write.
	    if ( field->isIgnored()) {
		writeMe = TRUE;
		break;
	    }
	}
	else {
	    // The field is a part field set to default.
	    partField = (SoSFNode *) field;

	    // If part is NULL, move onto the next field.
	    if (partField->getValue() == NULL) 
		continue;

	    // Else, get the partNode...
	    partNode = partField->getValue();

	    // If partNode should write, so should we...
	    if ( partNode->shouldWrite() ) {
		partField->setDefault(FALSE);
		writeMe = TRUE;
		break;
	    }

	    // If partNode is a nodekit, recurse.
	    if ( partNode->isOfType(SoBaseKit::getClassTypeId()) ) {
		if (((SoBaseKit *)partNode)->forceChildDrivenWriteRefs(out) == TRUE )
		    partField->setDefault(FALSE);
		    writeMe = TRUE;
		    break;
	    }
	}
    }

    //   [c] If kit has changed its mind because of [b], then add a writeRef.
    //       Then  return.
    if (writeMe) {
	SoBase::addWriteReference(out);	
	return TRUE;
    }

    //   [d] If kit should not write, it will delete the fieldDataForWriting, 
    //       since there will no writing pass applied to take care of this.
    delete fieldDataForWriting;
    fieldDataForWriting = NULL;

    return FALSE;
}



////////////////////////////////////////////////////////////////////////
//
// Description:
//
// This is called during addWriteReference() on parts of the nodekit that 
// are in turn nodekits. Under normal circumstances, it is not used.
// It is only called if the kit-part has a non-NULL value, but has been 
// set to default. (This indicates that we would rather not write it).
// Even though it is default, it will be forced to write if any of its
// fields or ancestor parts are required to write.
// This method adds a field-connection style write reference to the 
// kit-part itself, then allows the kit-part to add write references to 
// its fields and part-fields.
// Later, in the WRITE stage, the kit-part will be written if at least
// one of its fields or ancestor parts has shouldWrite() ==TRUE.
// Otherwise, it won't.
// Example: 
//   Parent draggers attempt not to write out child draggers.
//   But the parentDragger must at least traverse the childDragger to see 
//   if any of the part geometry has been changed from its default. Such 
//   changes must be written to file.
//
// Use: protected.
//
////////////////////////////////////////////////////////////////////////
void
SoBaseKit::countMyFields(SoOutput *out )
{
    if (out->getStage() == SoOutput::COUNT_REFS) {

	// In first pass, 'fieldDataForWriting' should always 
	// be NULL, since we delete it at the end of the WRITE stage.

	if (fieldDataForWriting != NULL) {
	    // We've already been here once during this write action.
	    // Just return.
	    return;
	}

	// Make version of fieldData that reorders fields so they write prettier
	createFieldDataForWriting();

	// This virtual function will call setDefault(TRUE) on those 
	// fields which we do not wish to write out.
	setDefaultOnNonWritingFields();

	// This (non-virtual) function will undo the setDefault on
	// those parts which simply MUST write out, even though 
	// they have been set to default.  This prevents errors in subclassing.
	undoSetDefaultOnFieldsThatMustWrite();

	// Now, count the write refs caused by our fields.

        const SoNodekitCatalog *cat = getNodekitCatalog();
	int       partNumber;
	SoField	 *field;
	SoSFNode *partField;
	SoNode   *partNode;

	for (int i = 0; i < fieldDataForWriting->getNumFields(); i++) {

	    const SbName fieldName = fieldDataForWriting->getFieldName(i);

	    field       = fieldDataForWriting->getField(this, i);
	    partNumber  = cat->getPartNumber( fieldName );

	    if ( partNumber == SO_CATALOG_NAME_NOT_FOUND ) {

		// If the field is not a part, do things the same way 
		// that SoFieldData does:
		if ( ! field->isDefault() || field->isIgnored())
		    field->write( out, fieldName );
	    }
	    else {
		// The field is for a part. 

		if ( !field->isDefault() ) {

		    // If it's not default, just write it out...
		    field->write(out, fieldName );
		}
		else {

		    // The field is a part field set to default.
		    // First, cast field to an SoSFNode:
		    partField = (SoSFNode *) field;

		    // The regular SoSFNode::countWriteRefs() looks like this:
		    // We call SoField::countWriteRefs() to count any 
		    // fields/engines we are connected from.
		    // But we don't want to write an instance of the part. 
		    // We're hoping it will not have to write.
		    // SoField::countWriteRefs(out);
		    // if (value != NULL)
		    //     value->writeInstance(out);

		    // Just like we said we'd do...
		    partField->SoField::countWriteRefs(out);

		    // If the part is NULL, we're done with this field.
		    if (partField->getValue() == NULL)
			continue;

		    // Else, get the partNode...
		    partNode = partField->getValue();

		    // Put a field-connection style write reference on partNode.
		    // This only makes partNode write if another instance is
		    // forcing a write anyway.
		    // Since it is a field-connection, the method will not
		    // traverse the fields of the node.
		    partNode->addWriteReference(out, TRUE);

		    if ( partNode->isOfType(SoBaseKit::getClassTypeId()) ) {

			// If partNode is a nodekit, write the fields as well.
			((SoBaseKit *)partNode)->countMyFields(out );
		    }
		}
	    }
	}
    }
}

/////////////////////////////////////////////////////////////////////////
//
// Copy the fields in fieldData into a new structure.
// We make the following changes to order in which fields are written:
// [1] Write out fields that are not named in the catalog first.
// [2] Write out leaf nodes from the catalog.
// [3] Write out non-leaf nodes from the catalog.
//
/////////////////////////////////////////////////////////////////////////

void
SoBaseKit::createFieldDataForWriting()
{
    const SoNodekitCatalog *cat = getNodekitCatalog();
    const SoFieldData *fd = getFieldData();

    fieldDataForWriting = new SoFieldData;
    int i, pNum;

    // [1] Write out fields that are not named in the catalog first.
    for ( i = 0; i < fd->getNumFields(); i++ ) {
	pNum = cat->getPartNumber( fd->getFieldName(i) );
	if ( pNum == SO_CATALOG_NAME_NOT_FOUND )
	    fieldDataForWriting->addField(this, 
		fd->getFieldName(i).getString(), fd->getField(this,i) );
    }

    // [2] Write out leaf nodes before non-leaf nodes.
    for ( i = 0; i < fd->getNumFields(); i++ ) {
	pNum = cat->getPartNumber( fd->getFieldName(i) );
	if ( pNum != SO_CATALOG_NAME_NOT_FOUND && cat->isLeaf(pNum) == TRUE )
	    fieldDataForWriting->addField(this, 
		fd->getFieldName(i).getString(), fd->getField(this,i) );
    }

    // [3] Write out non-leaf nodes from the catalog.
    for ( i = 0; i < fd->getNumFields(); i++ ) {
	pNum = cat->getPartNumber( fd->getFieldName(i) );
	if ( pNum != SO_CATALOG_NAME_NOT_FOUND && cat->isLeaf(pNum) == FALSE )
	    fieldDataForWriting->addField(this, 
		fd->getFieldName(i).getString(), fd->getField(this,i) );
    }
}

/////////////////////////////////////////////////////////////////////////
//
// call setDefault(TRUE) on the part fields (of type SoSFNode) for
// those parts which we do not wish to write out.
//
// We use the following test:
//
// First of all, we skip all the work if part already has isDefault() == TRUE
//
// Don't have to write out if:
//      [NULL Test]: value is NULL and nullByDefault is TRUE
// Otherwise, it must either be:
//      [Leaf SoGroup with no children]
// Or:
//      [Leaf SoSeparator with no children]
// Or:
//      [Leaf ListPart with no children and a container that's a group or sep]
// Or all three of the following:
//      [NonLeaf Test]:    part is not a leaf part          AND
//      [Group Test]:      node is non-NULL of type SoGroup AND
//      [FieldValue Test]: (node->isNodeFieldValuesImportant() == FALSE)
//
/////////////////////////////////////////////////////////////////////////

void
SoBaseKit::setDefaultOnNonWritingFields()
{
    const SoNodekitCatalog *cat      = getNodekitCatalog();
    SoSFNode               **pFields = nodekitPartsList->fieldList;
    int                    numP      = nodekitPartsList->numEntries;

    SoNode *node;

    for ( int partNum = 1; partNum < numP; partNum++ ) {

	// Skip all the work if part already has isDefault() == TRUE
	if ( pFields[partNum]->isDefault() )
	    continue;

	// [NULL Test]: value is NULL and nullByDefault is TRUE
	node = pFields[partNum]->getValue();
	if ( node == NULL && cat->isNullByDefault(partNum) ) {
	    pFields[partNum]->setDefault(TRUE);
	    continue;
	}

	SoType grpType = SoGroup::getClassTypeId();
	SoType sepType = SoSeparator::getClassTypeId();

	if ( cat->isLeaf(partNum) && node != NULL ) {

	    // [Leaf SoGroup with no children]
	    if ( node->getTypeId() == grpType 
		 && ((SoGroup *)node)->getNumChildren() == 0) {
		pFields[partNum]->setDefault(TRUE);
		continue;
	    }

	    // Or:
	    // [Leaf SoSeparator with no children]
	    if ( node->getTypeId() == sepType 
		 && ((SoGroup *)node)->getNumChildren() == 0) {
		pFields[partNum]->setDefault(TRUE);
		continue;
	    }

	    // Or:
	    // [Leaf ListPart with no kids and a group or sep container]
	    if ( node->getTypeId() == SoNodeKitListPart::getClassTypeId() 
		 && ((SoNodeKitListPart *)node)->getNumChildren() == 0 ) {
		 SoGroup *cn = ((SoNodeKitListPart *)node)->getContainerNode();
		 if (cn->getTypeId() == sepType || cn->getTypeId() == grpType) {
		    pFields[partNum]->setDefault(TRUE);
		    continue;
	        }
	    }
	}

	// Otherwise, must pass all three of the following:
	// [NonLeaf Test]:    part is not a leaf part          AND
	if (cat->isLeaf(partNum) == TRUE )
	    continue;

	// [Group Test]:      node is non-NULL of type SoGroup AND
	if (      node == NULL || 
	    (   ! node->isOfType(SoGroup::getClassTypeId()))) 
	    continue;

	// [FieldValue Test]: (node->isNodeFieldValuesImportant() == FALSE)
	if ( isNodeFieldValuesImportant( node ))
	    continue;

	// passed all three tests...
	pFields[partNum]->setDefault(TRUE);
    }
}

// Called by write() after the (virtual) setDefaultOnNonWritingNodes() 
// method.  This method looks at the part fields which have isDefault()
// set to TRUE.    It calls setDefault(FALSE) on any part fields
// that MUST write.   
//     This happens when the part-field is for a part whose parent 
//     is going to write out anyway. Therefore, it will appear in file as 
//     a node within this kit, so we better write out the part field to 
//     explain where the node belongs in the kit.
void 
SoBaseKit::undoSetDefaultOnFieldsThatMustWrite()
{
    const SoNodekitCatalog *cat      = getNodekitCatalog();
    SoSFNode               **pFields = nodekitPartsList->fieldList;
    int                    numP      = nodekitPartsList->numEntries;

    SoNode *node;

    for ( int partNum = 1; partNum < numP; partNum++ ) {

	// Skip if already not default.
	if ( ! pFields[partNum]->isDefault() )
	    continue;

	// NULL can always be written as default.
	node = pFields[partNum]->getValue();
	if ( node == NULL )
	    continue;

	// Parent must either be 'this' or it must also be
	// set to default. Otherwise this part will get written
	// out as a child of the parent anyway.
	int    prntPartNum = cat->getParentPartNumber( partNum ); 
	if (prntPartNum != SO_CATALOG_THIS_PART_NUM &&
	    pFields[prntPartNum]->isDefault() == FALSE ) {
	    pFields[partNum]->setDefault(FALSE);
	    continue;
	}
    }
}


SbBool
SoBaseKit::isNodeFieldValuesImportant( SoNode *node )
{
    // Create an instance of the same type - it will have default values
    // Use this for comparing in [Test1 ]
    SoFieldContainer *def 
	= (SoFieldContainer *) node->getTypeId().createInstance();
    def->ref();

    const SoFieldData *fd = node->getFieldData();
    for ( int i = 0; i < fd->getNumFields(); i++ ) { 

	// A field is important if:
	// [Test 1]: field has (isDefault() != TRUE) &&
	// [Test 2]: field does not have default value
	if (  ! fd->getField(node, i)->isDefault() &&
	     (! fd->getField(node, i)->isSame(*fd->getField(def, i)))) {

	    def->unref();
	    return TRUE;
	}

    }

    // Get rid of default instance
    def->unref();

    return FALSE;
}

void
SoBaseKit::replaceChild( int index, SoNode *newChild )
{
    // copied from SoGroup...
#ifdef DEBUG
    if (index < 0 || index >= getNumChildren()) {
	SoDebugError::post( "SoBaseKit::replaceChild",
			   "Index %d is out of range %d - %d", index, 0, getNumChildren() - 1);
	return;
    }
#endif				/* DEBUG */

    // Play it safe anyway...
    if (index >= 0)
	children->set(index, newChild);
}

void
SoBaseKit::removeChild( int index )
{
    // copied from SoGroup
#ifdef DEBUG
    if (index < 0 || index >= getNumChildren()) {
	SoDebugError::post( "SoBaseKit::removeChild",
			   "Index %d is out of range %d - %d", index, 0, getNumChildren() - 1);
	return;
    }
#endif				/* DEBUG */
    // Play it safe anyway...
    if (index >= 0) {
	children->remove(index);
    }

}

void
SoBaseKit::addChild( SoNode *child )
{
#ifdef DEBUG
    if (child == NULL) {
	SoDebugError::post( "SoBaseKit::addChild", "NULL child node");
	return;
    }
#endif				/* DEBUG */

    children->append(child);
}

int
SoBaseKit::findChild( const SoNode *child) const
{
    int i, num;

    num = getNumChildren();

    for (i = 0; i < num; i++)
	if (getChild(i) == child) return(i);

    return(-1);
}


void
SoBaseKit::insertChild(SoNode *child, int newChildIndex)
{
#ifdef DEBUG
    if (child == NULL) {
	SoDebugError::post( "SoBaseKit::insertChild", "NULL child node");
	return;
    }

    // Make sure index is reasonable
    if (newChildIndex < 0 || newChildIndex > getNumChildren()) {
	SoDebugError::post( "SoBaseKit::insertChild",
			   "Index %d is out of range %d - %d",newChildIndex, 0, getNumChildren());
	return;
    }
#endif				/* DEBUG */

    // See if adding at end
    if (newChildIndex >= getNumChildren())
	children->append(child);
    else
	children->insert(child, newChildIndex);
}

void
SoBaseKit::setSearchingChildren( SbBool newVal )
{
    searchingChildren = newVal;
}
