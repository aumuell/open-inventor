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
 |	This file defines the SoSeparator node class.
 |
 |   Author(s)		: Paul S. Strauss, Nick Thompson
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_SEPARATOR_
#define  _SO_SEPARATOR_

#include <Inventor/nodes/SoGroup.h>
#include <Inventor/fields/SoSFEnum.h>

class SoBoundingBoxCache;
class SoGLCacheList;

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoSeparator
//
//  Separator group node: state is saved before traversing children
//  and restored afterwards.
//
//////////////////////////////////////////////////////////////////////////////

#include <Inventor/fields/SoSFBitMask.h>

// C-api: prefix=SoSep
// C-api: public=renderCaching, boundingBoxCaching, renderCulling, pickCulling
class SoSeparator : public SoGroup {

    SO_NODE_HEADER(SoSeparator);

  public:

    enum CacheEnabled {		// Possible values for caching
	OFF,			// Never build or use a cache
	ON,			// Always try to build a cache
	AUTO			// Decide based on some heuristic
    };

    // Fields
    SoSFEnum renderCaching;	// OFF/ON/AUTO
    SoSFEnum boundingBoxCaching;// OFF/ON/AUTO
    SoSFEnum renderCulling;	// OFF/ON/AUTO
    SoSFEnum pickCulling;	// OFF/ON/AUTO

    // Constructor
    SoSeparator();

    // Constructor that takes approximate number of children
    // C-api: name=CreateN
    SoSeparator(int nChildren);

    // Overrides default method on SoNode
    virtual SbBool	affectsState() const;

    // Set/get the number of render caches each separator will have
    // (2, by default).  The more render caches each separator is
    // allowed to have, the more memory used.  setNumRenderCaches only
    // affects separators that are created after it is called, not
    // separators that were created before.  Setting zero render
    // caches globally turns off render caching.
    static void		setNumRenderCaches(int howMany);
    static int		getNumRenderCaches();

  SoEXTENDER public:
    // Implement actions
    virtual void	doAction(SoAction *action);
    virtual void	callback(SoCallbackAction *action);
    virtual void	getBoundingBox(SoGetBoundingBoxAction *action);
    virtual void	getMatrix(SoGetMatrixAction *action);
    virtual void	GLRender(SoGLRenderAction *action);
    virtual void	handleEvent(SoHandleEventAction *action);
    virtual void	rayPick(SoRayPickAction *action);
    virtual void	search(SoSearchAction *action);

    // These methods make render traversal faster by implementing
    // different rendering paths corresponding to different action
    // path codes.
    virtual void	GLRenderBelowPath(SoGLRenderAction *action);
    virtual void	GLRenderInPath(SoGLRenderAction *action);
    virtual void	GLRenderOffPath(SoGLRenderAction *action);

  SoINTERNAL public:
    static void		initClass();

    virtual void	notify(SoNotList *list);

  protected:
    // Returns TRUE if this separator can be culled because it is
    // outside the cull volume.  The cullResults flags come from the
    // GLRenderAction->getCullTestResults() method, and track the
    // results of previous cull tests to save work when we know we're
    // completely inside the view volume.
    virtual SbBool	cullTest(SoGLRenderAction *action, int &cullResults);

    virtual SbBool	readInstance(SoInput *in, unsigned short flags);

    virtual ~SoSeparator();

  private:
    // Each separator that is created will contain up to this many
    // render caches.
    static int		numRenderCaches;

    SoBoundingBoxCache	*bboxCache;	// Cache for bounding boxes
    SoGLCacheList	*cacheList;	// Caches for GL rendering
};

#endif /* _SO_SEPARATOR_ */
