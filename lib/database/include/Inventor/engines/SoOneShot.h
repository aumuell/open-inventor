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
 |	This file contains the declaration of the OneShot engine
 |
 |   Classes:
 |	SoOneShot
 |
 |   Author(s)		: Ronen Barzel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_ONE_SHOT_
#define  _SO_ONE_SHOT_

#include <Inventor/engines/SoSubEngine.h>
#include <Inventor/fields/SoSFBool.h>
#include <Inventor/fields/SoSFTrigger.h>
#include <Inventor/fields/SoSFTime.h>
#include <Inventor/fields/SoSFBitMask.h>

// C-api: public=duration,trigger,flags,disable,timeIn
// C-api: public=timeOut,isActive,ramp
class SoOneShot : public SoEngine {

    SO_ENGINE_HEADER(SoOneShot);

  public:
    enum Flags	{
	RETRIGGERABLE	= (1<<0),	// can start over during cycle?
	HOLD_FINAL	= (1<<1)	// ramp & timeOut stay high after cycle?
    };

    // Inputs
    SoSFTime		duration;	// duration of active cycle
    SoSFTrigger		trigger;	// start the cycle
    SoSFBitMask		flags;
    SoSFBool		disable;	// if TRUE, timer is disabled, output=0
    SoSFTime		timeIn;

    // Outputs
    SoEngineOutput	timeOut;	// (SoSFTime) time counts from trigger
    SoEngineOutput	isActive;	// (SoSFBool)
    SoEngineOutput	ramp;		// (SoSFFloat) ramps linearly 0 to 1

    // Constructor
    SoOneShot();

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
	DISABLED,		// everything clamped at 0 (via disable input)
	OFF,			// normal idle state
	TRIGGERED,		// has been triggered, need to start running
	ON,			// running, watching the time
	PEAKED			// have reached the end, need to reset
    }			state;
    SbTime		baseTime;	// Starting time for elapsed time 
    SbTime		timeVal;	// holds output time value
    double		rampVal;	// holds ramp value
    SbBool		outputPending;	// keeps outputs enabled until data is read

    // Destructor
    ~SoOneShot();

    // Evaluation method
    virtual void	evaluate();
};

#endif  /* _SO_ONE_SHOT_ */
