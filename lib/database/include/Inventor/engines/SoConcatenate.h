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
 |   $Revision: 1.2 $
 |
 |   Description:
 |	This file defines the "concatenation" engine that joins
 |	together several (up to 10) fields.  Its inputs are several
 |      multi-valued fields (automatic field conversion will do the
 |      right thing, so you can attach single-valued fields) of the
 |      same type, output is a multi-value field of the same type.
 |
 |      If you need to concatenate more than 10 fields together,
 |      concatenate the outputs of several of these into yet another
 |      concatenate engine.
 |
 |   Classes:
 |	SoConcatenate
 |
 |   Author(s)		: Ronen Barzel, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_CONCATENATE_
#define  _SO_CONCATENATE_

#include <Inventor/engines/SoSubEngine.h>
#include <Inventor/fields/SoSFName.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoConcatenate
//
//  Concatenate several fields together into one big field.
//
//////////////////////////////////////////////////////////////////////////////

class SoMField;

// C-api: public=input,output
// C-api: prefix=SoConcat
class SoConcatenate : public SoEngine {

    SO_ENGINE_HEADER(SoConcatenate);

  public:
    // Constructor, takes a subclass of SoMField.
    SoConcatenate(SoType mfInputType);

    // Inputs; connect them like this:
    //    concatenate->input[0]->connectFrom(...)
    // The names of these inputs is input0 ... input9.
    // The default value for these fields is no values.
    SoMField *input[10];

    // Output.  Is of the same type as the inputs (note that it is a
    // pointer, unlike most engine outputs).
    SoEngineOutput *output;

  SoINTERNAL public:
    // Initializes engine class
    static void		initClass();

  protected:

    virtual SoFieldContainer *	copyThroughConnection() const;
  
  private:
    // Constructor/Destructor.  This constructor is used only when
    // reading these from file.
    SoConcatenate();
    ~SoConcatenate();

    // Concatenate engines must write out their input type before writing
    // their fields, so these are overridden:
    virtual SbBool	readInstance(SoInput *in, unsigned short flags);
    virtual void	writeInstance(SoOutput *out);

    void		setup(SoType);
    virtual void	evaluate();

    int conversionCase;

    SoFieldData *myInputData;
    SoEngineOutputData *myOutputData;

    SoSFName typeField; // Used when reading/writing
};

#endif /* _SO_CONCATENATE_ */
