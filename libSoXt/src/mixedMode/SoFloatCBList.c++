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
 * Copyright (C) 1991   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.2 $
 |
 |   Classes:
 |	SoFloatCallbackList
 |
 |   Author(s): David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include "SoFloatCallbackList.h"

typedef struct _SoCallbackStruct {
    SoFloatCallbackListCB   *func;
    void		    *userData;
} SoCallbackStruct;

//////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
SoFloatCallbackList::SoFloatCallbackList()
//
//////////////////////////////////////////////////////////////////////////////
{
}

//////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
SoFloatCallbackList::~SoFloatCallbackList()
//
//////////////////////////////////////////////////////////////////////////////
{
    int len = list.getLength();
    
    for (int i = 0; i < len; i++) {
    	delete (SoCallbackStruct *) list[i];
    }
}

//////////////////////////////////////////////////////////////////////////////
//
//  addCallback - adds the function f to the callback list, along with
//  userData. At invocation, f will be passed userData, along with callback
//  specific data.
//
void
SoFloatCallbackList::addCallback(SoFloatCallbackListCB *f, void *userData)
//
//////////////////////////////////////////////////////////////////////////////
{
    if (f == NULL)
    	return;
	
    SoCallbackStruct *cb = new SoCallbackStruct;
    cb->func = f;
    cb->userData = userData;
    
    list.append(cb);
}   

//////////////////////////////////////////////////////////////////////////////
//
//  removeCallback - removes the function f associated with userData from the.
//  callback list.
//
void
SoFloatCallbackList::removeCallback(SoFloatCallbackListCB *f, void *userData)
//
//////////////////////////////////////////////////////////////////////////////
{
    int len = list.getLength();
    SoCallbackStruct *cb;
    int found = 0;
    
    for (int i = 0; (i < len) && (! found); i++) {
	cb = (SoCallbackStruct *) list[i];
	if ((cb->func == f) && (cb->userData == userData)) {
	    list.remove(i);
	    delete cb;
	    found = 1;
	}
    }
}   

//////////////////////////////////////////////////////////////////////////////
//
//  invokeCallbacks - invokes each callback func in the list, passing.
//  userData, and callbackData as the parameters.
//
void
SoFloatCallbackList::invokeCallbacks(float callbackData)
//
//////////////////////////////////////////////////////////////////////////////
{
    int len = list.getLength();
    SoCallbackStruct *cb;
    
    for (int i = 0; i < len; i++) {
	cb = (SoCallbackStruct *) list[i];
	(*cb->func) (cb->userData, callbackData);
    }
}   
