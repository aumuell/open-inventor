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
 |	This file defines the abstract SoFieldConverter base class.
 |
 |   Author(s)		: Paul S. Strauss, Ronen Barzel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_FIELD_CONVERTER_
#define  _SO_FIELD_CONVERTER_

#include <Inventor/SoDB.h>
#include <Inventor/engines/SoSubEngine.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoFieldConverter
//
//  Abstract base class for all field converter engines. A field
//  converter engine is used to convert the value of a field of one
//  type to another. These engines are registered with the global
//  database so connections between fields of different types can be
//  made by automatically inserting a converter instance.
//  A single class of converter engine may be registered for several
//  different conversions.
//  Field converters are never written to file
//  (SoField::writeConnection takes care of that).
//
//////////////////////////////////////////////////////////////////////////////

// C-api: abstract

SoEXTENDER class SoFieldConverter : public SoEngine {

    SO_ENGINE_ABSTRACT_HEADER(SoFieldConverter);

  public:
    // These must be defined in each subclass. They return the input
    // and output connections of the given types.
    virtual SoField *		getInput(SoType type)  = 0;
    virtual SoEngineOutput *	getOutput(SoType type) = 0;

    // Returns the input that is connected.  By default, this searches
    // through the field data for the field that is connected;
    // you can redefine to make more efficient.
    virtual SoField *		getConnectedInput();

  SoINTERNAL public:
    static void		initClass();
    static void		initClasses();	// init all builtin converters

    // Adds all fields connected from the output of the converter to
    // the given list
    int			getForwardConnections(SoFieldList &list) const;

  protected:
    SoFieldConverter();

    ~SoFieldConverter();
};

#endif /* _SO_FIELD_CONVERTER_ */
