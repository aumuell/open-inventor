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
 * Copyright (C) 1990,91,92   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Class:
 |	Initializes all element classes
 |
 |   Author(s)		: Ronen Barzel, Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/elements/SoElements.h>

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes ALL Inventor element classes.
//
// Use: internal

void
SoElement::initElements()
//
////////////////////////////////////////////////////////////////////////
{
    // Initialize base classes first
    SoElement::initClass();
    SoAccumulatedElement::initClass();
    SoReplacedElement::initClass();
    SoInt32Element::initClass();
    SoFloatElement::initClass();

    // Initialize derived classes
    SoCacheElement::initClass();
    SoClipPlaneElement::initClass();
    SoComplexityElement::initClass();
    SoComplexityTypeElement::initClass();
    SoCoordinateElement::initClass();
    SoCreaseAngleElement::initClass();
    SoDrawStyleElement::initClass();
    SoFocalDistanceElement::initClass();
    SoFontNameElement::initClass();
    SoFontSizeElement::initClass();
    // We must put this before Lazy Element:
    SoShapeStyleElement::initClass();
    SoLazyElement::initClass();
    SoLightAttenuationElement::initClass();
    SoLinePatternElement::initClass();
    SoLineWidthElement::initClass();
    SoMaterialBindingElement::initClass();
    SoModelMatrixElement::initClass();
    SoNormalBindingElement::initClass();
    SoNormalElement::initClass();
    SoOverrideElement::initClass();
    SoPickRayElement::initClass();
    SoPickStyleElement::initClass();
    SoPointSizeElement::initClass();
    SoProfileCoordinateElement::initClass();
    SoProfileElement::initClass();
    SoProjectionMatrixElement::initClass();
    SoShapeHintsElement::initClass();
    SoSwitchElement::initClass();
    SoTextureCoordinateBindingElement::initClass();
    SoTextureCoordinateElement::initClass();
    SoTextureImageElement::initClass();
    SoTextureMatrixElement::initClass();
    SoTextureQualityElement::initClass();
    SoTextureOverrideElement::initClass();
    SoUnitsElement::initClass();
    SoViewVolumeElement::initClass();
    SoViewingMatrixElement::initClass();
    SoViewportRegionElement::initClass();

    // GL specific elements must be initialized after their more
    // generic counterparts

    SoGLCacheContextElement::initClass();
    SoGLClipPlaneElement::initClass();
    SoGLCoordinateElement::initClass();
    SoGLDrawStyleElement::initClass();
    SoGLLazyElement::initClass();
    SoGLLightIdElement::initClass();
    SoGLLinePatternElement::initClass();
    SoGLLineWidthElement::initClass();
    SoGLModelMatrixElement::initClass();
    SoGLNormalElement::initClass();
    SoGLPointSizeElement::initClass();
    SoGLProjectionMatrixElement::initClass();
    SoGLRenderPassElement::initClass();
    SoGLShapeHintsElement::initClass();
    SoGLTextureCoordinateElement::initClass();
    SoGLTextureEnabledElement::initClass();
    SoGLTextureImageElement::initClass();
    SoGLTextureMatrixElement::initClass();
    SoGLUpdateAreaElement::initClass();
    SoGLViewingMatrixElement::initClass();
    SoGLViewportRegionElement::initClass();

    // Other derived classes 
    SoBBoxModelMatrixElement::initClass();
    SoLocalBBoxMatrixElement::initClass();
    
    SoWindowElement::initClass();
}
////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoAccumulatedElement class.
//
// Use: internal

void
SoAccumulatedElement::initClass()
{
    // We can't use the SO_ELEMENT_INIT_CLASS() macro here, because we
    // don't want to set the stackIndex for this class to anything
    // real. So we'll just do the rest by hand.

    classTypeId = SoType::createType(SoElement::getClassTypeId(),
				     "AccumulatedElement", NULL);
    classStackIndex = -1;
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoBBoxModelMatrixElement class.
//
// Use: internal

void
SoBBoxModelMatrixElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoBBoxModelMatrixElement, SoModelMatrixElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoCacheElement class.
//
// Use: internal

void
SoCacheElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoCacheElement, SoElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoClipPlaneElement class.
//
// Use: internal

void
SoClipPlaneElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoClipPlaneElement, SoAccumulatedElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoComplexityElement class.
//
// Use: internal

void
SoComplexityElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoComplexityElement, SoFloatElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoComplexityTypeElement class.
//
// Use: internal

void
SoComplexityTypeElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoComplexityTypeElement, SoInt32Element);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoCoordinateElement class.
//
// Use: internal

void
SoCoordinateElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoCoordinateElement, SoReplacedElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoCreaseAngleElement class.
//
// Use: internal

void
SoCreaseAngleElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoCreaseAngleElement, SoFloatElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoDrawStyleElement class.
//
// Use: internal

void
SoDrawStyleElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoDrawStyleElement, SoInt32Element);
}

// This is the initial number of slots in the array of types that's
// indexed by stack index.  There is one slot per element class, so
// this number should be at least as large as the number of standard
// element classes in Inventor.
#define NUM_STACK_INDICES	100

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoElement class.
//
// Use: internal

void
SoElement::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    nextStackIndex = 0;

    // Initialize type id and unique id
    classTypeId = SoType::createType(SoType::badType(), "Element", NULL);

    // Initialize stack index to a bad value since this is an abstract
    // class that can't appear in stacks
    classStackIndex = -1;

#ifdef DEBUG
    // Create list that correlates stack indices to type id's
    stackToType = new SoTypeList(NUM_STACK_INDICES);
#endif
}
#undef NUM_STACK_INDICES


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoFloatElement class.
//
// Use: internal

void
SoFloatElement::initClass()
{
    // We can't use the SO_ELEMENT_INIT_CLASS() macro here, because we
    // don't want to set the stackIndex for this class to anything
    // real. So we'll just do the rest by hand.

    classTypeId = SoType::createType(SoElement::getClassTypeId(),
				     "SoFloatElement", NULL);
    classStackIndex = -1;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoFocalDistanceElement class.
//
// Use: internal

void
SoFocalDistanceElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoFocalDistanceElement, SoFloatElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoFontNameElement class.
//
// Use: internal

void
SoFontNameElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoFontNameElement, SoReplacedElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoFontSizeElement class.
//
// Use: internal

void
SoFontSizeElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoFontSizeElement, SoFloatElement);
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoGLCacheContextElement class.
//
// Use: internal

void
SoGLCacheContextElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoGLCacheContextElement, SoElement);
    waitingToBeFreed = new SbPList;
    extensionList = new SbPList;
    mipmapSupportList = new SbIntList;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoGLClipPlaneElement class.
//
// Use: internal

void
SoGLClipPlaneElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoGLClipPlaneElement, SoClipPlaneElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoGLCoordinateElement class.
//
// Use: internal

void
SoGLCoordinateElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoGLCoordinateElement, SoCoordinateElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoGLDrawStyleElement class.
//
// Use: internal

void
SoGLDrawStyleElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoGLDrawStyleElement, SoDrawStyleElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoGLLightIdElement class.
//
// Use: internal

void
SoGLLightIdElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoGLLightIdElement, SoInt32Element);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoGLLazyElement class.
//
// Use: internal

void
SoGLLazyElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoGLLazyElement, SoLazyElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoGLLinePatternElement class.
//
// Use: internal

void
SoGLLinePatternElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoGLLinePatternElement, SoLinePatternElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoGLLineWidthElement class.
//
// Use: internal

void
SoGLLineWidthElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoGLLineWidthElement, SoLineWidthElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoGLModelMatrixElement class.
//
// Use: internal

void
SoGLModelMatrixElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoGLModelMatrixElement, SoModelMatrixElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoGLNormalElement class.
//
// Use: internal

void
SoGLNormalElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoGLNormalElement, SoNormalElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoGLPointSizeElement class.
//
// Use: internal

void
SoGLPointSizeElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoGLPointSizeElement, SoPointSizeElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoGLProjectionMatrixElement class.
//
// Use: internal

void
SoGLProjectionMatrixElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoGLProjectionMatrixElement,
			  SoProjectionMatrixElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoGLRenderPassElement class.
//
// Use: internal

void
SoGLRenderPassElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoGLRenderPassElement, SoElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoGLShapeHintsElement class.
//
// Use: internal

void
SoGLShapeHintsElement::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO_ELEMENT_INIT_CLASS(SoGLShapeHintsElement, SoShapeHintsElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoGLTextureCoordinateElement class.
//
// Use: internal

void
SoGLTextureCoordinateElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoGLTextureCoordinateElement,
			  SoTextureCoordinateElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoGLTextureEnabledElement class.
//
// Use: internal

void
SoGLTextureEnabledElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoGLTextureEnabledElement, SoInt32Element);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoGLTextureImageElement class.
//
// Use: internal

void
SoGLTextureImageElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoGLTextureImageElement, SoTextureImageElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoGLTextureMatrixElement class.
//
// Use: internal

void
SoGLTextureMatrixElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoGLTextureMatrixElement, SoTextureMatrixElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoGLUpdateAreaElement class.
//
// Use: internal

void
SoGLUpdateAreaElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoGLUpdateAreaElement, SoElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoGLViewingMatrixElement class.
//
// Use: internal

void
SoGLViewingMatrixElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoGLViewingMatrixElement, SoViewingMatrixElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoGLViewportRegionElement class.
//
// Use: internal

void
SoGLViewportRegionElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoGLViewportRegionElement, SoViewportRegionElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoLazyElement class.
//
// Use: internal

void
SoLazyElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoLazyElement, SoElement);
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoLightAttenuationElement class.
//
// Use: internal

void
SoLightAttenuationElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoLightAttenuationElement, SoReplacedElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoLinePatternElement class.
//
// Use: internal

void
SoLinePatternElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoLinePatternElement, SoInt32Element);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoLineWidthElement class.
//
// Use: internal

void
SoLineWidthElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoLineWidthElement, SoFloatElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoLocalBBoxMatrixElement class.
//
// Use: internal

void
SoLocalBBoxMatrixElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoLocalBBoxMatrixElement, SoElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoInt32Element class.
//
// Use: internal

void
SoInt32Element::initClass()
{
    // We can't use the SO_ELEMENT_INIT_CLASS() macro here, because we
    // don't want to set the stackIndex for this class to anything
    // real. So we'll just do the rest by hand.

    classTypeId = SoType::createType(SoElement::getClassTypeId(),
				     "SoInt32Element", NULL);
    classStackIndex = -1;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoMaterialBindingElement class.
//
// Use: internal

void
SoMaterialBindingElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoMaterialBindingElement, SoInt32Element);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoModelMatrixElement class.
//
// Use: internal

void
SoModelMatrixElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoModelMatrixElement, SoAccumulatedElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoNormalBindingElement class.
//
// Use: internal

void
SoNormalBindingElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoNormalBindingElement, SoInt32Element);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoNormalElement class.
//
// Use: internal

void
SoNormalElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoNormalElement, SoReplacedElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoOverrideElement class.
//
// Use: internal

void
SoOverrideElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoOverrideElement, SoElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoPickRayElement class.
//
// Use: internal

void
SoPickRayElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoPickRayElement, SoElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoPickStyleElement class.
//
// Use: internal

void
SoPickStyleElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoPickStyleElement, SoInt32Element);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoPointSizeElement class.
//
// Use: internal

void
SoPointSizeElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoPointSizeElement, SoFloatElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoProfileCoordinateElement class.
//
// Use: internal

void
SoProfileCoordinateElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoProfileCoordinateElement, SoReplacedElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoProfileElement class.
//
// Use: internal

void
SoProfileElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoProfileElement, SoAccumulatedElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoProjectionMatrixElement class.
//
// Use: internal

void
SoProjectionMatrixElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoProjectionMatrixElement, SoReplacedElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoReplacedElement class.
//
// Use: internal

void
SoReplacedElement::initClass()
{
    // We can't use the SO_ELEMENT_INIT_CLASS() macro here, because we
    // don't want to set the stackIndex for this class to anything
    // real. So we'll just do the rest by hand.

    classTypeId = SoType::createType(SoElement::getClassTypeId(),
				     "SoReplacedElement", NULL);
    classStackIndex = -1;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoShapeHintsElement class.
//
// Use: internal

void
SoShapeHintsElement::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO_ELEMENT_INIT_CLASS(SoShapeHintsElement, SoElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoShapeStyleElement class.
//
// Use: internal

void
SoShapeStyleElement::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO_ELEMENT_INIT_CLASS(SoShapeStyleElement, SoElement);
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoSwitchElement class.
//
// Use: internal

void
SoSwitchElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoSwitchElement, SoInt32Element);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoTextureCoordinateBindingElement class.
//
// Use: internal

void
SoTextureCoordinateBindingElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoTextureCoordinateBindingElement,SoInt32Element);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoTextureCoordinateElement class.
//
// Use: internal

void
SoTextureCoordinateElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoTextureCoordinateElement, SoReplacedElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoTextureImageElement class.
//
// Use: internal

void
SoTextureImageElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoTextureImageElement, SoReplacedElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoTextureMatrixElement class.
//
// Use: internal

void
SoTextureMatrixElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoTextureMatrixElement, SoAccumulatedElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoTextureQualityElement class.
//
// Use: internal

void
SoTextureQualityElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoTextureQualityElement, SoFloatElement);
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoTextureQualityElement class.
//
// Use: internal

void
SoTextureOverrideElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoTextureOverrideElement, SoElement);
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoUnitsElement class.
//
// Use: internal

void
SoUnitsElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoUnitsElement, SoInt32Element);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoViewVolumeElement class.
//
// Use: internal

void
SoViewVolumeElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoViewVolumeElement, SoReplacedElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoViewingMatrixElement class.
//
// Use: internal

void
SoViewingMatrixElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoViewingMatrixElement, SoReplacedElement);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoViewportRegionElement class.
//
// Use: internal

void
SoViewportRegionElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoViewportRegionElement, SoElement);
    emptyViewportRegion = new SbViewportRegion(0, 0);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes SoWindowElement class.
//
// Use: internal

void
SoWindowElement::initClass()
{
    SO_ELEMENT_INIT_CLASS(SoWindowElement, SoElement);
}

