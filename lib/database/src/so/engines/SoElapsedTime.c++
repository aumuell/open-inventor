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
 |   $Revision: 1.1.1.1 $
 |
 |   Classes:
 |      SoElapsedTime
 |
 |   Author(s)          : Gavin Bell, Paul Isaacs
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SoDB.h>
#include <Inventor/engines/SoElapsedTime.h>

SO_ENGINE_SOURCE(SoElapsedTime);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoElapsedTime::SoElapsedTime()
//
////////////////////////////////////////////////////////////////////////
{
    SO_ENGINE_CONSTRUCTOR(SoElapsedTime);
    SO_ENGINE_ADD_INPUT(timeIn,	  (SbTime::zero()));
    SO_ENGINE_ADD_INPUT(speed, (1.0));
    SO_ENGINE_ADD_INPUT(on, (TRUE));
    SO_ENGINE_ADD_INPUT(pause, (FALSE));
    SO_ENGINE_ADD_INPUT(reset, ());
    SO_ENGINE_ADD_OUTPUT(timeOut, SoSFTime);

    state	= RUNNING;
    todo	= RESET;
    isBuiltIn   = TRUE;

    // default time source connection
    timeIn.connectFrom(SoDB::getGlobalField("realTime"));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoElapsedTime::~SoElapsedTime()
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Writes instance to SoOutput. Takes care of not writing out
//    connection to realTime that is created in constructor.
//
// Use: private

void
SoElapsedTime::writeInstance(SoOutput *out)
//
////////////////////////////////////////////////////////////////////////
{
    // If connected to realTime, disable connection to prevent
    // connection from being written and mark timeIn as default to
    // prevent value from being written
    SbBool timeInWasDefault = timeIn.isDefault();
    SbBool timeInConnectionEnabled = timeIn.isConnectionEnabled();
    SoField *timeInConnection = NULL;
    timeIn.getConnectedField(timeInConnection);
    if (timeInConnection == SoDB::getGlobalField("realTime")) {
	timeIn.enableConnection(FALSE);
	timeIn.setDefault(TRUE);
    }
    SoEngine::writeInstance(out);

    timeIn.enableConnection(timeInConnectionEnabled);
    timeIn.setDefault(timeInWasDefault);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reads stuff into instance.  Works around a problem with old
//    files that contain explicit references to the default
//    connection to realTime.
//
// Use: private

SbBool
SoElapsedTime::readInstance(SoInput *in, unsigned short flags)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool result = SoEngine::readInstance(in, flags);
    todo |= RESET;
    SoField *timeInConnection = NULL;
    timeIn.getConnectedField(timeInConnection);
    if (timeInConnection == SoDB::getGlobalField("realTime")) {
	timeIn.disconnect();
	timeIn.connectFrom(timeInConnection);
    }

    return result;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Detects when the various inputs changed.
//
// Use: private

void
SoElapsedTime::inputChanged(SoField *whichInput)
//
////////////////////////////////////////////////////////////////////////
{
    if (whichInput == &on) {
	todo |= CHECK_ON;
    }
    if (whichInput == &pause) {
	todo |= CHECK_PAUSE;
    }
    if (whichInput == &reset) {
	todo |= RESET;
    }

    SbBool enable = todo || (state == RUNNING);
    timeOut.enable(enable);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Evaluation routine
//
// Use: private

void
SoElapsedTime::evaluate()
//
////////////////////////////////////////////////////////////////////////
{
    SbTime now = timeIn.getValue();
    State oldState = state;

    /**********
    **
    ** first do the things queue up in the "todo" bitmask
    */

    // reset output to 0
    if (todo&RESET) {
	reset.getValue();
	prevTimeOfDay = now;
	prevTimeOut = SbTime::zero();
	prevClockTime = prevTimeOut.getValue();
    }

    // Figure out which state we're in.  'on' takes precedence over
    // 'pause':
    if (todo & CHECK_ON || todo & CHECK_PAUSE) {
	if (on.getValue())
	    if (pause.getValue())
		state = PAUSED;
	    else
		state = RUNNING;
	else
	    state = STOPPED;
    }

    ////////////////////////////////////////////////////////////////////////
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
    ////////////////////////////////////////////////////////////////////////

    // Calculate clock time that has passed since last evaluation
    // This is difference in real time multiplied by speed.
    SbTime deltaClockT = speed.getValue() * (now - prevTimeOfDay);

    // We'll calculate the new internal clockTime.
    // If we're RUNNING, then we'll output clockTime.
    // Otherwise we'll output the same value as last time.
    SbTime clockTime;
    SbTime tOut;

    switch (state) {
      case RUNNING:
	// The first time running after a stop, we resume from the
	// previous time that was output.
	if (oldState == STOPPED)
	    clockTime = prevTimeOut;
	else
	    clockTime = SbTime(prevClockTime) + deltaClockT;
	tOut = clockTime;
	break;

      case STOPPED:
        clockTime = prevTimeOut;
	tOut = prevTimeOut;
	break;

      case PAUSED:
	clockTime = SbTime(prevClockTime) + deltaClockT;
	tOut = prevTimeOut;
	break;
    }

    // Update the state variables:
    prevTimeOfDay = now;
    prevTimeOut = tOut;
    prevClockTime = clockTime.getValue();

    // A value is always output.  Note that if the engine is paused or
    // stopped inputChanged() disables notification through timeOut,
    // so evaluate() will only be called if new connections are made.
    SO_ENGINE_OUTPUT(timeOut, SoSFTime, setValue(tOut));

    todo = 0;
}
