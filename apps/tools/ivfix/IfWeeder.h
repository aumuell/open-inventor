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
// IfWeeder class: weeds unneeded items from the resulting fixed scene
// graph.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef  _IF_WEEDER_
#define  _IF_WEEDER_

class  SoNode;
struct IfWeederMaterialEntry;

class IfWeeder {

  public:
    IfWeeder();
    ~IfWeeder();

    void	weed(SoNode *root);

  private:
    SbPList	*materialList;		// Stores materials & dependent shapes

    // These are all used during removeDuplicateMaterials. They save
    // the components of an SoMaterial being weeded.
    SoMaterial		*curMaterial;
    const SbColor	*ambi;
    const SbColor	*diff;
    const SbColor	*spec;
    const SbColor	*emis;
    const float		*shin;
    const float		*tran;

    // This weeds out values in SoMaterial nodes that are not used by
    // any shape. It also removes duplicate material values.
    void	weedMaterials(SoNode *root);

    // Weeds the material in the given entry
    void	weedMaterial(SoNode *root, IfWeederMaterialEntry *entry);

    // Finds all materials in the given graph and the shapes that depend
    // on them
    void	findMaterialsAndShapes(SoNode *root);

    // Finds and removes duplicate materials, adjusting material
    // indices in all dependent shapes
    void	removeDuplicateMaterials(IfWeederMaterialEntry *entry);

    // Computes a hash key for the material value with the given index
    uint32_t	computeMaterialHashKey(int index);

    // Returns TRUE if the two material values with the given indices
    // are the same.
    SbBool	isSameMaterial(int i1, int i2);

    // Removes any unused material values and updates the indices in
    // all dependent shapes
    void	removeUnusedMaterials(IfWeederMaterialEntry *entry);
};

#endif /* _IF_WEEDER_ */
