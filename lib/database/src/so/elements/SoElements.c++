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
 |   $Revision $
 |
 |   Classes:
 |	All elements in one neat little file.
 |
 |   Author(s)		: Paul S. Strauss
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


// Initialization file
#include "SoElementInit.c++"

// Base classes
#include "SoAccumulatedElement.c++"
#include "SoReplacedElement.c++"

// Derived classes

       
#include "SoBBoxModelMatrixElement.c++"
#include "SoCacheElement.c++"
#include "SoClipPlaneElement.c++" 
#include "SoComplexityElement.c++"
#include "SoComplexityTypeElement.c++"  
#include "SoCoordinateElement.c++"
#include "SoCreaseAngleElement.c++"  
#include "SoDrawStyleElement.c++"
#include "SoFloatElement.c++"  
#include "SoFocalDistanceElement.c++"
#include "SoFontNameElement.c++"  
#include "SoFontSizeElement.c++"  
#include "SoGLCacheContextElement.c++"
#include "SoGLClipPlaneElement.c++"  
#include "SoGLCoordinateElement.c++"
#include "SoGLDrawStyleElement.c++"  
#include "SoGLLazyElement.c++"
#include "SoGLLightIdElement.c++"  
#include "SoGLLinePatternElement.c++"
#include "SoGLLineWidthElement.c++"  
#include "SoGLModelMatrixElement.c++"
#include "SoGLNormalElement.c++"  
#include "SoGLPointSizeElement.c++"  
#include "SoGLProjectionMatrixElement.c++"
#include "SoGLRenderPassElement.c++"  
#include "SoGLShapeHintsElement.c++"  
#include "SoGLTextureCoordinateElement.c++"
#include "SoGLTextureEnabledElement.c++"  
#include "SoGLTextureImageElement.c++"
#include "SoGLTextureMatrixElement.c++"  
#include "SoGLUpdateAreaElement.c++"
#include "SoGLViewingMatrixElement.c++"  
#include "SoGLViewportRegionElement.c++"
#include "SoLazyElement.c++"  
#include "SoLightAttenuationElement.c++"
#include "SoLinePatternElement.c++"  
#include "SoLineWidthElement.c++"
#include "SoLocalBBoxMatrixElement.c++"  
#include "SoInt32Element.c++"  
#include "SoMaterialBindingElement.c++"  
#include "SoModelMatrixElement.c++"
#include "SoNormalBindingElement.c++"  
#include "SoNormalElement.c++"
#include "SoOverrideElement.c++"
#include "SoPickRayElement.c++"  
#include "SoPickStyleElement.c++"
#include "SoPointSizeElement.c++"  
#include "SoProfileCoordinateElement.c++"
#include "SoProfileElement.c++"  
#include "SoProjectionMatrixElement.c++"
#include "SoShapeHintsElement.c++"  
#include "SoShapeStyleElement.c++"  
#include "SoSwitchElement.c++"  
#include "SoTextureCoordinateBindingElement.c++"
#include "SoTextureCoordinateElement.c++"  
#include "SoTextureImageElement.c++"
#include "SoTextureMatrixElement.c++"  
#include "SoTextureQualityElement.c++"  
#include "SoUnitsElement.c++"
#include "SoViewVolumeElement.c++"  
#include "SoViewingMatrixElement.c++"
#include "SoViewportRegionElement.c++"  
#include "SoWindowElement.c++"
