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
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Description:
 |	Defines the SoWWWInline class
 |
 |   Author(s): Gavin Bell, David Mott, Jim Kent
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_WWWINLINE_
#define  _SO_WWWINLINE_

#include <Inventor/misc/SoChildList.h>
#include <Inventor/nodes/SoSubNode.h>
#include <Inventor/fields/SoSFNode.h>
#include <Inventor/fields/SoSFString.h>
#include <Inventor/fields/SoSFVec3f.h>

#include <Inventor/actions/SoCallbackAction.h>

class SoWWWInline;
class SoGroup;
class SoWriteAction;

// This is the format for callbacks that will get URL data for us.
// Once they get the data, they should call the setChildData() method.
typedef void SoWWWInlineFetchURLCB(
    const SbString &url, void *userData, SoWWWInline *node);
		

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoWWWInline
//
//////////////////////////////////////////////////////////////////////////////

// C-api: public=name,bboxCenter,bboxSize
class SoWWWInline : public SoNode {

    SO_NODE_HEADER(SoWWWInline);

  public:
    // Constructor
    SoWWWInline();

    enum BboxVisibility {
	NEVER,             // Do not show bounding box
	UNTIL_LOADED,      // Show bounding box (if specified) until data is loaded
	ALWAYS            // Show bounding box along with data
    };

    SoSFVec3f	bboxCenter;	// Bounding box center
    SoSFVec3f	bboxSize;	// Bounding box size
    SoSFString	name;		// URL specifying children (might be relative)
    SoSFNode	alternateRep;	// Child data if app cannot fetch URL
    
    // If the name field contains a relative URL (e.g. "foo.wrl" 
    // instead of "http://bogus.com/foo.wrl"), the anchor cannot 
    // resolve the URL reference. This method allows the application 
    // to tell the anchor what it's full URL should be. 
    // getFullURLName returns the fullURL set here, or if not set, returns 
    // the contents of the name field.
    void		setFullURLName(const SbString &url) { fullURL = url; }
    const SbString &	getFullURLName();

    // Return (hidden) children as Group
    SoGroup             *copyChildren() const;
    
    // Request that URL data be fetched, and tell whether the URL data is here
    // C-api: name=reqURLData
    void		requestURLData() 
			    { if (! kidsRequested) requestChildrenFromURL(); }
    // C-api: name=isURLDataReq
    SbBool		isURLDataRequested() const { return kidsRequested; }
    SbBool		isURLDataHere() const { return kidsAreHere; }
    // C-api: name=cancelURLDataReq
    void		cancelURLDataRequest() 
			    { if (!kidsAreHere) kidsRequested = FALSE; }

    // Set/get the child data the inline should display. Application should call
    // this after it has fetched data for the inline node.
    void		setChildData(SoNode *urlData);
    SoNode *		getChildData() const;

    // Allow the viewer to fetch URLs when needed.
    // C-api: name=setFetchCB
    static void setFetchURLCallBack(SoWWWInlineFetchURLCB *f, void *userData);

    // Allow the user to specify how bounding boxes are displayed
    // C-api: name=setBboxVis
    static void setBoundingBoxVisibility(BboxVisibility b) { bboxVisibility = b; } 
    // C-api: name=getBboxVis
    static BboxVisibility getBoundingBoxVisibility() { return bboxVisibility; }

    // Allow the user to specify the bounding box color
    // C-api: name=setBboxCol
    static void setBoundingBoxColor(SbColor &c) { bboxColor = c; }
    // C-api: name=getBboxCol
    static const SbColor &getBoundingBoxColor() { return bboxColor; }


  SoEXTENDER public:
    // Traversal methods for all the actions:
    virtual void	doAction(SoAction *action);
    virtual void	doActionOnKidsOrBox(SoAction *action);
    virtual void	callback(SoCallbackAction *action);
    virtual void	GLRender(SoGLRenderAction *action);
    virtual void	getBoundingBox(SoGetBoundingBoxAction *action);
    virtual void	getMatrix(SoGetMatrixAction *action);
    virtual void	handleEvent(SoHandleEventAction *action);
    virtual void	search(SoSearchAction *action);
    virtual void	pick(SoPickAction *action);

  SoINTERNAL public:
    static void		initClass();

    // Returns pointer to children, or NULL if none
    virtual SoChildList *getChildren() const;

  protected:
    virtual ~SoWWWInline();

    virtual void        addBoundingBoxChild(SbVec3f center, SbVec3f size);

    // Reads into instance of SoWWWInline. Returns FALSE on error.
    virtual SbBool	readInstance(SoInput *in, unsigned short flags);

    // Copies the child data as well as the rest of the node (if
    // the child data has been set)
    virtual void        copyContents(const SoFieldContainer *fromFC,
				     SbBool copyConnections);

  private:
    void		requestChildrenFromURL();

    SoChildList		*children;
    SbBool		kidsRequested;
    SbBool		kidsAreHere;
    SbString		fullURL;

    static SbColor            bboxColor;
    static BboxVisibility     bboxVisibility;

    // Static methods
    static SoWWWInlineFetchURLCB	*fetchURLcb;
    static void				*fetchURLdata;
};

#endif /* _SO_WWWINLINE_ */

