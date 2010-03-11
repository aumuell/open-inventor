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
 * Copyright (C) 1990,91,92   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Classes:	SvManipList
 |
 |   Author(s):	David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SV_MANIP_LIST_
#define  _SV_MANIP_LIST_


// This class helps keep track of 
// selectionPath/manip/xfPath triplets.

class SbPList;
class SoPath;
class SoTransformManip;

// You can add a selectionPath/manip/xfPath triplet to the list.
// Methods let you find the index of this triplet based on any of the three
// things.  You can then use the index to get the selectionPath, manip, or
// xfPath, or remove the triplet from the list.

class SvManipList {
  public:
    SvManipList();
    ~SvManipList();
    
    int		    getLength() const;

    // append will ref() the paths and the manip
    void	    append(SoPath *selectionP, 
			   SoTransformManip *m, SoPath *xfP);
    
    // return the index of the triplet.
    // use this index in calls to:
    // remove(), getSelectionPath(), getManip(), getXfPath()
    int		    find(const SoPath *p) const;
    int		    find(const SoTransformManip *m) const;
    int		    findByXfPath(const SoPath *p) const;
    
    // remove will unref() the paths and the manip
    void	    remove(int which);
    
    // these return the paths or the manip.
    SoPath *	       getSelectionPath(int which) const;
    SoTransformManip * getManip(int which) const;
    SoPath *           getXfPath(int which) const;
  
  private:
    SbPList *	    list;
};

#endif // _SV_MANIP_LIST_
