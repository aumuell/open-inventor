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
 |   $Revision: 1.1 $
 |
 |   Description:
 |	This file contains the implementation of the Counter engine
 |
 |   Classes:
 |	SoCounter
 |
 |   Author(s)		: Ronen Barzel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_COUNTER_
#define  _SO_COUNTER_

#include <Inventor/engines/SoSubEngine.h>
#include <Inventor/fields/SoSFShort.h>
#include <Inventor/fields/SoSFBool.h>
#include <Inventor/fields/SoSFTrigger.h>

// C-api: public=min,max,step,trigger,reset
// C-api: public=output,syncOut
class SoCounter : public SoEngine {

    SO_ENGINE_HEADER(SoCounter);

  public:
    // Inputs
    SoSFShort		min;		// min value for counter (default 0)
    SoSFShort		max;		// max value for counter (default 1)
    SoSFShort		step;		// value to step by (default 1)
    SoSFTrigger		trigger;	// go to next step
    SoSFShort		reset;		// reset cycle to value at setValue

    // Outputs
    SoEngineOutput	output;		// (SoSFShort) counts min...max
    SoEngineOutput	syncOut;	// (SoSFTrigger) triggers at cycle start

    // Constructor
    SoCounter();

  SoINTERNAL public:

    static void initClass();

  protected:
    virtual void	inputChanged(SoField *whichInput);

  private:
    enum Todo {
	RECALC	= (1<<0),
	RESET	= (1<<1),
	STEP	= (1<<2)
    };
    unsigned int	todo;
    int			nStages;
    int			curStage;
    struct Stage {
	int	val;		// counter value for stage
    }			*stages;

    // Destructor
    ~SoCounter();

    // Evaluation method
    virtual void	evaluate();
};

#endif  /* _SO_COUNTER_ */
