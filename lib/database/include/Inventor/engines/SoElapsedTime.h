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
 |	This file contains the implementation of the ElapsedTime engine
 |
 |   Classes:
 |	SoElapsedTime
 |
 |   Author(s)		: Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_ELAPSED_TIME_
#define  _SO_ELAPSED_TIME_

#include <Inventor/engines/SoSubEngine.h>
#include <Inventor/fields/SoSFBool.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFTime.h>
#include <Inventor/fields/SoSFTrigger.h>

// C-api: public=timeIn,speed,on,pause,reset
// C-api: public=timeOut
class SoElapsedTime : public SoEngine {

    SO_ENGINE_HEADER(SoElapsedTime);

  public:
    // Inputs
    SoSFTime		timeIn;		// Running (clock) time
    SoSFFloat		speed;		// Scale factor for time
    SoSFBool		on;		// TRUE to run, FALSE to stop
    SoSFBool		pause;		// TRUE to freeze, FALSE to run
    SoSFTrigger		reset;

    // Output: 
    //
    // While the engine is on, it keeps track of 'clock' time.
    // This is the amount of real time that passes multiplied by the speed.
    // If the speed input varies while the engine is on, then 'clock' time 
    // advances non-uniformly.
    //
    // Pausing the engine will freeze the timeOut value, but internally the 
    // 'clock' time will continue to advance.
    // Unpause the engine and the timeOut will jump forward to display
    // 'clock' time.
    //
    // Stop the engine (by setting 'on' to FALSE) to freeze both the
    // timeOut value and the 'clock' time. Re-start (by setting 'on'
    // to TRUE) and both timeOut and 'clock' will continue from where
    // they were at the time the engine was stopped.
    //
    SoEngineOutput	timeOut;

    // Constructor
    SoElapsedTime();

  SoINTERNAL public:

    static void initClass();

  protected:

    // Detects when the running time input changed
    virtual void	inputChanged(SoField *whichInput);

    // Writes instance to SoOutput. Takes care of not writing out
    // connection to realTime that is created in constructor.
    virtual void	writeInstance(SoOutput *out);

    // Reads stuff into instance.  Works around a problem with old
    // files that contain explicit references to the default
    // connection to realTime.
    virtual SbBool	readInstance(SoInput *in, unsigned short flags);

  private:
    enum Todo {
	CHECK_ON	= (1<<0),
	CHECK_PAUSE	= (1<<1),
	RESET		= (1<<2)
    };
    enum State {
	STOPPED,	// Clock is not running
	RUNNING,	// Clock is running
	PAUSED		// Clock is running, but output is frozen
    };

    enum State		state;
    unsigned int	todo;
    SbTime		prevTimeOfDay;	 // Time of day when last
					 // evaluate was called.
    SbTime		prevTimeOut;	 // Last 'clock' time that 
					 // was output.
    float		prevClockTime;   // 'Clock' time when last 
					 // evaluate was called.

    // Destructor
    ~SoElapsedTime();

    // Evaluation method
    virtual void	evaluate();
};

#endif  /* _SO_ELAPSED_TIME_ */
