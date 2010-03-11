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
 |	call initClasses for all field classes
 |
 |   Author(s)		: Ronen Barzel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/fields/SoFields.h>

////////////////////////////////////////////////////////////////////////
//
// Description:
//    This initializes ALL Inventor field classes.
//
// Use: internal

void
SoField::initClasses()
//
////////////////////////////////////////////////////////////////////////
{
    SoField::initClass();
    SoSField::initClass();
    SoMField::initClass();

    SoSFBool::initClass();
    SoSFColor::initClass();
    SoSFEngine::initClass();
    SoSFEnum::initClass();
    SoSFBitMask::initClass();		// Note: derived from SoSFEnum!
    SoSFFloat::initClass();
    SoSFImage::initClass();
    SoSFInt32::initClass();
    SoSFMatrix::initClass();
    SoSFName::initClass();
    SoSFNode::initClass();
    SoSFPath::initClass();
    SoSFPlane::initClass();
    SoSFRotation::initClass();
    SoSFShort::initClass();
    SoSFString::initClass();
    SoSFTime::initClass();
    SoSFTrigger::initClass();
    SoSFUInt32::initClass();
    SoSFUShort::initClass();
    SoSFVec2f::initClass();
    SoSFVec3f::initClass();
    SoSFVec4f::initClass();

    SoMFBool::initClass();
    SoMFColor::initClass();
    SoMFEngine::initClass();
    SoMFEnum::initClass();
    SoMFBitMask::initClass();		// Note: derived from SoMFEnum!
    SoMFFloat::initClass();
    SoMFInt32::initClass();
    SoMFMatrix::initClass();
    SoMFName::initClass();
    SoMFNode::initClass();
    SoMFPath::initClass();
    SoMFPlane::initClass();
    SoMFRotation::initClass();
    SoMFShort::initClass();
    SoMFString::initClass();
    SoMFTime::initClass();
    SoMFUInt32::initClass();
    SoMFUShort::initClass();
    SoMFVec2f::initClass();
    SoMFVec3f::initClass();
    SoMFVec4f::initClass();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoField class.
//
// Use: internal

void
SoField::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    // Allocate a new field type id. There's no real parent id, so we
    // can't use the regular macro.
    classTypeId = SoType::createType(SoType::badType(), "Field");
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoMFBitMask class.
//
// Use: internal

void
SoMFBitMask::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__FIELD_INIT_CLASS(SoMFBitMask, "MFBitMask", SoMFEnum);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoMFBool class.
//
// Use: internal

void
SoMFBool::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__FIELD_INIT_CLASS(SoMFBool, "MFBool", SoMField);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoMFColor class.
//
// Use: internal

void
SoMFColor::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__FIELD_INIT_CLASS(SoMFColor, "MFColor", SoMField);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoMFEngine class.
//
// Use: internal

void
SoMFEngine::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__FIELD_INIT_CLASS(SoMFEngine, "MFEngine", SoMField);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoMFEnum class.
//
// Use: internal

void
SoMFEnum::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__FIELD_INIT_CLASS(SoMFEnum, "MFEnum", SoMField);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoMFFloat class.
//
// Use: internal

void
SoMFFloat::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__FIELD_INIT_CLASS(SoMFFloat, "MFFloat", SoMField);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoMFInt32 class.
//    Enables the synonym MFLong.
//       Note: last one registered is used in output.
//
// Use: internal

void
SoMFInt32::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__FIELD_INIT_CLASS(SoMFInt32, "MFLong", SoMField);
    SO__FIELD_INIT_CLASS(SoMFInt32, "MFInt32", SoMField);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoMFMatrix class.
//
// Use: internal

void
SoMFMatrix::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__FIELD_INIT_CLASS(SoMFMatrix, "MFMatrix", SoMField);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoMFName class.
//
// Use: internal

void
SoMFName::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__FIELD_INIT_CLASS(SoMFName, "MFName", SoMField);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoMFNode class.
//
// Use: internal

void
SoMFNode::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__FIELD_INIT_CLASS(SoMFNode, "MFNode", SoMField);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoMFPath class.
//
// Use: internal

void
SoMFPath::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__FIELD_INIT_CLASS(SoMFPath, "MFPath", SoMField);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoMFPlane class.
//
// Use: internal

void
SoMFPlane::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__FIELD_INIT_CLASS(SoMFPlane, "MFPlane", SoMField);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoMFRotation class.
//
// Use: internal

void
SoMFRotation::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__FIELD_INIT_CLASS(SoMFRotation, "MFRotation", SoMField);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoMFShort class.
//
// Use: internal

void
SoMFShort::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__FIELD_INIT_CLASS(SoMFShort, "MFShort", SoMField);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoMFString class.
//
// Use: internal

void
SoMFString::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__FIELD_INIT_CLASS(SoMFString, "MFString", SoMField);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoMFTime class.
//
// Use: internal

void
SoMFTime::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__FIELD_INIT_CLASS(SoMFTime, "MFTime", SoMField);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoMFUInt32 class.
//    Enables the synonym MFULong.
//       Note: last one registered is used in output.
//
// Use: internal

void
SoMFUInt32::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__FIELD_INIT_CLASS(SoMFUInt32, "MFULong", SoMField);
    SO__FIELD_INIT_CLASS(SoMFUInt32, "MFUInt32", SoMField);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoMFUShort class.
//
// Use: internal

void
SoMFUShort::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__FIELD_INIT_CLASS(SoMFUShort, "MFUShort", SoMField);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoMFVec2f class.
//
// Use: internal

void
SoMFVec2f::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__FIELD_INIT_CLASS(SoMFVec2f, "MFVec2f", SoMField);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoMFVec3f class.
//
// Use: internal

void
SoMFVec3f::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__FIELD_INIT_CLASS(SoMFVec3f, "MFVec3f", SoMField);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoMFVec4f class.
//
// Use: internal

void
SoMFVec4f::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__FIELD_INIT_CLASS(SoMFVec4f, "MFVec4f", SoMField);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoSFBitMask class.
//
// Use: internal

void
SoSFBitMask::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__FIELD_INIT_CLASS(SoSFBitMask, "SFBitMask", SoSFEnum);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoSFBool class.
//
// Use: internal

void
SoSFBool::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__FIELD_INIT_CLASS(SoSFBool, "SFBool", SoSField);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoSFColor class.
//
// Use: internal

void
SoSFColor::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__FIELD_INIT_CLASS(SoSFColor, "SFColor", SoSField);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoSFEngine class.
//
// Use: internal

void
SoSFEngine::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__FIELD_INIT_CLASS(SoSFEngine, "SFEngine", SoSField);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoSFEnum class.
//
// Use: internal

void
SoSFEnum::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__FIELD_INIT_CLASS(SoSFEnum, "SFEnum", SoSField);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoSFFloat class.
//
// Use: internal

void
SoSFFloat::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__FIELD_INIT_CLASS(SoSFFloat, "SFFloat", SoSField);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoSFImage class.
//
// Use: internal

void
SoSFImage::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__FIELD_INIT_CLASS(SoSFImage, "SFImage", SoSField);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoSFInt32 class.
//    Enables the synonym SFLong.
//       Note: last one registered is used in output.
//
// Use: internal

void
SoSFInt32::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__FIELD_INIT_CLASS(SoSFInt32, "SFLong", SoSField);
    SO__FIELD_INIT_CLASS(SoSFInt32, "SFInt32", SoSField);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoSFMatrix class.
//
// Use: internal

void
SoSFMatrix::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__FIELD_INIT_CLASS(SoSFMatrix, "SFMatrix", SoSField);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoSFName class.
//
// Use: internal

void
SoSFName::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__FIELD_INIT_CLASS(SoSFName, "SFName", SoSField);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoSFNode class.
//
// Use: internal

void
SoSFNode::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__FIELD_INIT_CLASS(SoSFNode, "SFNode", SoSField);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoSFPath class.
//
// Use: internal

void
SoSFPath::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__FIELD_INIT_CLASS(SoSFPath, "SFPath", SoSField);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoSFPlane class.
//
// Use: internal

void
SoSFPlane::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__FIELD_INIT_CLASS(SoSFPlane, "SFPlane", SoSField);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoSFRotation class.
//
// Use: internal

void
SoSFRotation::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__FIELD_INIT_CLASS(SoSFRotation, "SFRotation", SoSField);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoSFShort class.
//
// Use: internal

void
SoSFShort::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__FIELD_INIT_CLASS(SoSFShort, "SFShort", SoSField);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoSFString class.
//
// Use: internal

void
SoSFString::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__FIELD_INIT_CLASS(SoSFString, "SFString", SoSField);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoSFTime class.
//
// Use: internal

void
SoSFTime::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__FIELD_INIT_CLASS(SoSFTime, "SFTime", SoSField);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoSFTrigger class.
//
// Use: internal

void
SoSFTrigger::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__FIELD_INIT_CLASS(SoSFTrigger, "SFTrigger", SoSField);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoSFUInt32 class.
//    Enables the synonym SFULong.
//       Note: last one registered is used in output.
//
// Use: internal

void
SoSFUInt32::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__FIELD_INIT_CLASS(SoSFUInt32, "SFULong", SoSField);
    SO__FIELD_INIT_CLASS(SoSFUInt32, "SFUInt32", SoSField);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoSFUShort class.
//
// Use: internal

void
SoSFUShort::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__FIELD_INIT_CLASS(SoSFUShort, "SFUShort", SoSField);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoSFVec2f class.
//
// Use: internal

void
SoSFVec2f::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__FIELD_INIT_CLASS(SoSFVec2f, "SFVec2f", SoSField);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoSFVec3f class.
//
// Use: internal

void
SoSFVec3f::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__FIELD_INIT_CLASS(SoSFVec3f, "SFVec3f", SoSField);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes the SoSFVec4f class.
//
// Use: internal

void
SoSFVec4f::initClass()
//
////////////////////////////////////////////////////////////////////////
{
    SO__FIELD_INIT_CLASS(SoSFVec4f, "SFVec4f", SoSField);
}

