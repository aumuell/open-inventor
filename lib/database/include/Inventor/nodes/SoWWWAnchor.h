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
 |	Defines the SoWWWAnchor class
 |
 |   Author(s): David Mott, Jim Kent, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_WWWANCHOR_
#define  _SO_WWWANCHOR_

#include <Inventor/fields/SoSFString.h>
#include <Inventor/nodes/SoLocateHighlight.h>

class SoWWWAnchor;

// This is the format for callbacks during anchor activation (left mouse click)
// and highlight (move over the anchor).
// The URL for the anchor is passed, with additional information as described by the
// map field tagged onto the URL.
typedef void SoWWWAnchorCB(
    const SbString &url, void *userData, SoWWWAnchor *node);


//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoWWWAnchor
//
//////////////////////////////////////////////////////////////////////////////

// C-api: public=name,description,map
class SoWWWAnchor : public SoLocateHighlight {

    SO_NODE_HEADER(SoWWWAnchor);

  public:
    // Constructor
    SoWWWAnchor();

    enum Mapping {
	NONE,		    // Do no add information to the URL
	POINT		    // Add object-space coordinates to URL
    };

    // Fields
    SoSFString	name;	     // URL to fetch when activated by mouse click
    SoSFString	description; // Description of the URL that will be grabbed
    SoSFEnum	map;	     // What additional info to add to the URL
    
    // If the name field contains a relative URL (e.g. "foo.wrl" 
    // instead of "http://bogus.com/foo.wrl"), the anchor cannot 
    // resolve the URL reference. This method allows the application 
    // to tell the anchor what it's full URL should be. 
    // getFullURLName returns the fullURL set here, or if not set, returns 
    // the contents of the name field.
    void		setFullURLName(const SbString &url) { fullURL = url; }
    const SbString &	getFullURLName();

    // Application callbacks invoked when the mouse is clicked on an 
    // anchor (activate) so that the app can fetch the URL or call
    // netscape/mosaic to fetch the URL.
    // C-api: name=setFetchCB
    static void setFetchURLCallBack( 
        SoWWWAnchorCB *f, void *userData); 

    // Application callbacks invoked when the mouse is moved over an
    // anchor so that the app can provide feedback such as changing the
    // cursor, or displaying the anchor description or URL.
    // C-api: name=setHLCB
    static void setHighlightURLCallBack(
	SoWWWAnchorCB *f, void *userData);

  SoEXTENDER public:
    // Override handleEvent to look for left-mouse clicks, to do a
    // pick:
    virtual void	handleEvent(SoHandleEventAction *action);

  SoINTERNAL public:
    static void		initClass();

  protected:
    virtual ~SoWWWAnchor();
    
    // redefine this to also invoke the app callbacks...
    // This is called when the locate highlight status of this node changes.
    virtual void	redrawHighlighted(SoAction *act, SbBool isNowHighlighting);

    static SoWWWAnchorCB	    *fetchURLcb;
    static void			    *fetchURLdata;
    static SoWWWAnchorCB	    *highlightURLcb;
    static void			    *highlightURLdata;
    static SbString		    emptyString;
  
  private:  
    SbString			    fullURL;
};

#endif /* _SO_WWWANCHOR_ */
