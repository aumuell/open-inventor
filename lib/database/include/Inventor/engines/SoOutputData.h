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
 |	Definition of the SoEngineOutputData class, which is used by
 |	engines to store information about their outputs.  Based on
 |	SoFieldData.
 |
 |   Author(s)		: Paul S. Strauss, Nick Thompson, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

// C-api: end

#ifndef  _SO_OUTPUT_DATA_
#define  _SO_OUTPUT_DATA_

#include <Inventor/misc/SoBasic.h>
#include <Inventor/SbPList.h>
#include <Inventor/SbString.h>
#include <Inventor/SoType.h>

class SoEngine;
class SoFieldContainer;
class SoInput;
class SoOutput;
class SoEngineOutput;

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoEngineOutputData
//
//  The SoEngineOutputData class holds data about engine outputs: the
//  number of outputs the engine has, the names of the outputs.
//
//  All engines of a given class share the same output data. Therefore,
//  to get information about a particular output in a particular engine
//  instance, it is necessary to pass that instance to the appropriate
//  method.
//
//////////////////////////////////////////////////////////////////////////////

SoINTERNAL class SoEngineOutputData {
  public:
    // Default constructor
    SoEngineOutputData() { }

    SoEngineOutputData(const SoEngineOutputData *);

    // Constructor that takes number of fields as a hint
    SoEngineOutputData(int numOutputs) : outputs(numOutputs)
	{ }

    // Destructor
    ~SoEngineOutputData();

    // Adds an output to current data, given name of output,
    // a pointer to field within the engine, and the type of output.
    void		addOutput(const SoEngine *defEngine,
				  const char *outputName,
				  const SoEngineOutput *output,
				  SoType type);

    // Returns number of outputs
    int			getNumOutputs() const	{ return outputs.getLength(); }

    // Returns name of output with given index
    const SbName &	getOutputName(int index) const;

    // Returns pointer to output with given index within given object instance
    SoEngineOutput *	getOutput(const SoEngine *engine,
				  int index) const;

    // Returns index of output, given the output and the engine it
    // is in.
    int			getIndex(const SoEngine *engine,
				 const SoEngineOutput *output) const;

    // Returns type of output with given index
    const SoType &	getType(int index) const;

    // Methods to read and write the output descriptions
    SbBool		readDescriptions(SoInput *in,
					 SoEngine *engine) const;
    void		writeDescriptions(SoOutput *out,
					 SoEngine *engine) const;
  private:
    SbPList		outputs;	// List of outputs (SoOutputEntry)
};    

#endif /* _SO_OUTPUT_DATA_ */
