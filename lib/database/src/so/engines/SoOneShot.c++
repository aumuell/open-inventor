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
 |      SoOneShot
 |
 |   Author(s)          : Ronen Barzel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SoDB.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFShort.h>
#include <Inventor/engines/SoOneShot.h>

SO_ENGINE_SOURCE(SoOneShot);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoOneShot::SoOneShot()
//
////////////////////////////////////////////////////////////////////////
{
    SO_ENGINE_CONSTRUCTOR(SoOneShot);
    SO_ENGINE_ADD_INPUT(duration, (1.0));
    SO_ENGINE_ADD_INPUT(trigger, ());
    SO_ENGINE_ADD_INPUT(flags, (0));
    SO_ENGINE_ADD_INPUT(disable, (0));
    SO_ENGINE_ADD_INPUT(timeIn,	  (SbTime::zero()));
    SO_ENGINE_ADD_OUTPUT(timeOut, SoSFTime);
    SO_ENGINE_ADD_OUTPUT(isActive, SoSFBool);
    SO_ENGINE_ADD_OUTPUT(ramp, SoSFFloat);

    SO_ENGINE_DEFINE_ENUM_VALUE(Flags, RETRIGGERABLE);
    SO_ENGINE_DEFINE_ENUM_VALUE(Flags, HOLD_FINAL);

    SO_ENGINE_SET_SF_ENUM_TYPE(flags, Flags);

    state	= OFF;
    rampVal	= 0.0;
    timeVal	= 0.0;
    outputPending = FALSE;
    isBuiltIn   = TRUE;

    // default time source connection
    timeIn.connectFrom(SoDB::getGlobalField("realTime"));

    ramp.enable(FALSE);
    isActive.enable(FALSE);
    timeOut.enable(FALSE);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoOneShot::~SoOneShot()
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
SoOneShot::writeInstance(SoOutput *out)
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
SoOneShot::readInstance(SoInput *in, unsigned short flags)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool result = SoEngine::readInstance(in, flags);
    state = OFF;
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
//    Detects when the inputs changed.
//
// Use: private

void
SoOneShot::inputChanged(SoField *whichInput)
//
////////////////////////////////////////////////////////////////////////
{
    const enum State oldstate = state;
    if (whichInput == &disable) {
	if (disable.getValue())
	    state = DISABLED;
	else if (state == DISABLED)
	    state = OFF;
    }
    else if (whichInput == &trigger) {
	switch (state)  {
	    case DISABLED:
	    case TRIGGERED:
		break;
	    case OFF:
		state = TRIGGERED;
		break;
	    case ON:
		if (flags.getValue()&RETRIGGERABLE)
		    state = TRIGGERED;
		break;
	    case PEAKED:
		state = TRIGGERED;
		break;
	}
    }
    if (oldstate != state) outputPending = TRUE;
    int enable = outputPending ||
	state==ON || state==TRIGGERED || state == PEAKED;
    timeOut.enable(enable);
    isActive.enable(enable);
    ramp.enable(enable);
#ifdef DEVELOP
    static char *enames[] = { "DISABLED", "OFF", "TRIGGERED", "ON", "PEAKED" };
    printf("OneShot::inputChanged %s oldstate=%s state=%s disable=%d pending=%d enable=%d\n",
	   whichInput==&disable ? "disable" :
	   whichInput==&trigger ? "trigger" :
	   whichInput==&timeIn  ? "----   " :
	   "  ...  ",
	   enames[oldstate], enames[state],
	   disable.getValue(),
	   outputPending,
	   enable);
    fflush(stdout);
#endif
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Evaluation routine
//
// Use: private

void
SoOneShot::evaluate()
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEVELOP
    const enum State oldstate = state;
#endif
    SbBool activeVal;

    switch (state) {
	case DISABLED:
	    activeVal = FALSE;
	    rampVal = 0.0;
	    timeVal = 0.0;
	    break;

	case OFF:
	    activeVal = FALSE;
	    break;

	case TRIGGERED:
	    trigger.getValue();
	    baseTime = timeIn.getValue();
	    activeVal = TRUE;
	    rampVal = 0.0;
	    timeVal = 0.0;
	    state = ON;
	    break;

	case ON:
	    timeVal = timeIn.getValue() - baseTime;
	    rampVal = timeVal/duration.getValue();
	    if (rampVal >= 1.0) {
		rampVal = 1.0;
		timeVal = duration.getValue();
		state = PEAKED;
	    }
	    activeVal = TRUE;
	    break;

	case PEAKED:	// gone past the end, waiting to reset.
	    if (!(flags.getValue()&HOLD_FINAL)) {
		rampVal = 0.0;
		timeVal = 0.0;
	    }
	    activeVal = FALSE;
	    state = OFF;
	    break;
    }
#ifdef DEVELOP
    static char *enames[] = { "DISABLED", "OFF", "TRIGGERED", "ON", "PEAKED" };
    printf("OneShot::evaluate state=%s oldstate=%s active=%d ramp=%f time=%s pending=%d\n",
	   enames[state], enames[oldstate], activeVal, rampVal, timeVal.format("%M:%s.%u").getString(),
	   outputPending);
    fflush(stdout);
#endif
    SO_ENGINE_OUTPUT(timeOut, SoSFTime, setValue(timeVal));
    SO_ENGINE_OUTPUT(isActive, SoSFBool, setValue(activeVal));
    SO_ENGINE_OUTPUT(ramp, SoSFFloat, setValue(rampVal));
    outputPending = FALSE;
}
