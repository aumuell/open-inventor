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
 |	This file contains the implementation of the TimeCounter engine
 |
 |   Classes:
 |	SoTimeCounter
 |
 |   Author(s)		: Ronen Barzel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_TIME_COUNTER_
#define  _SO_TIME_COUNTER_

#include <Inventor/engines/SoSubEngine.h>
#include <Inventor/fields/SoSFShort.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoMFFloat.h>
#include <Inventor/fields/SoSFTime.h>
#include <Inventor/fields/SoSFBool.h>
#include <Inventor/fields/SoSFTrigger.h>

// C-api: public=min,max,step,on,frequency,duty,timeIn,syncIn,reset
// C-api: public=output,syncOut
class SoTimeCounter : public SoEngine {

    SO_ENGINE_HEADER(SoTimeCounter);

  public:
    // Inputs
    SoSFShort		min;		// min value for counter (default 0)
    SoSFShort		max;		// max value for counter (default 1)
    SoSFShort		step;		// value to step by (default 1)
    SoSFBool		on;		// counter pauses while this is FALSE
    SoSFFloat		frequency;	// cycles-per-second
    SoMFFloat		duty;		// duty-cycle array
    SoSFTime		timeIn;		// time source (else internal is used)
    SoSFTrigger		syncIn;		// restart cycle
    SoSFShort		reset;		// reset cycle to value at setValue

    // Outputs
    SoEngineOutput	output;		// (SoSFShort) counts min...max
    SoEngineOutput	syncOut;	// (SoSFTrigger) triggers at cycle start

    // Constructor
    SoTimeCounter();

  SoINTERNAL public:

    static void initClass();

  protected:
    virtual void	inputChanged(SoField *whichInput);

    // Writes instance to SoOutput. Takes care of not writing out
    // connection to realTime that is created in constructor.
    virtual void	writeInstance(SoOutput *out);

    // Reads stuff into instance.  Works around a problem with old
    // files that contain explicit references to the default
    // connection to realTime.
    virtual SbBool	readInstance(SoInput *in, unsigned short flags);

  private:
    enum State {
	ON,			// counting
	PAUSED			// holding
    }			state;
    enum Todo {
	RECALC	= (1<<0),
	RESET	= (1<<1),
	SYNC	= (1<<2),
	PAUSE	= (1<<3),
	UNPAUSE	= (1<<4)
    };
    unsigned int	todo;
    SbTime		period;		// total cycle time
    int			nStages;
    int			curStage;
    int			prevStage;
    struct Stage {
	int	val;		// counter value for stage
	float	duty;		// normalized fraction of cycle
	SbTime	offset;		// time from start of cycle
    }			*stages;
    SbTime		baseTime;	// Starting time for cycle
    SbTime		pauseOffset;	// Starting time for cycle

    // Destructor
    ~SoTimeCounter();

    // Evaluation method
    virtual void	evaluate();
};

#endif  /* _SO_TIME_COUNTER_ */
