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
 |      SoTimeCounter
 |
 |   Author(s)          : Ronen Barzel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SoDB.h>
#include <Inventor/engines/SoTimeCounter.h>

SO_ENGINE_SOURCE(SoTimeCounter);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoTimeCounter::SoTimeCounter()
//
////////////////////////////////////////////////////////////////////////
{
    SO_ENGINE_CONSTRUCTOR(SoTimeCounter);
    SO_ENGINE_ADD_INPUT(min, (0));
    SO_ENGINE_ADD_INPUT(max, (1));
    SO_ENGINE_ADD_INPUT(step, (1));
    SO_ENGINE_ADD_INPUT(on, (TRUE));
    SO_ENGINE_ADD_INPUT(frequency, (1));
    SO_ENGINE_ADD_INPUT(duty, (1.0));
    SO_ENGINE_ADD_INPUT(timeIn,	  (SbTime::zero()));
    SO_ENGINE_ADD_INPUT(syncIn, ());
    SO_ENGINE_ADD_INPUT(reset, (0));
    SO_ENGINE_ADD_OUTPUT(output, SoSFShort);
    SO_ENGINE_ADD_OUTPUT(syncOut, SoSFTrigger);

    state		= ON;
    todo		= RECALC|SYNC;
    stages		= NULL;
    curStage		= -1;
    prevStage		= -1;
    isBuiltIn           = TRUE;

    // default connection for time source
    timeIn.connectFrom(SoDB::getGlobalField("realTime"));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoTimeCounter::~SoTimeCounter()
//
////////////////////////////////////////////////////////////////////////
{
    delete [] stages;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Writes instance to SoOutput. Takes care of not writing out
//    connection to realTime that is created in constructor.
//
// Use: private

void
SoTimeCounter::writeInstance(SoOutput *out)
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
SoTimeCounter::readInstance(SoInput *in, unsigned short flags)
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
//    Detects when the inputs changed.  Notifications are only propogated
// when the output is going to change; periodic clock-ticks are suppressed
// while waiting for the next stage
//
// Use: private

void
SoTimeCounter::inputChanged(SoField *whichInput)
//
////////////////////////////////////////////////////////////////////////
{
    // get current time value
    SbTime now = timeIn.getValue();

    SbBool enable = FALSE;
    SbBool syncEnable = !!(todo&SYNC);
    if (whichInput == &min ||
	whichInput == &max ||
	whichInput == &step ||
	whichInput == &frequency ||
	whichInput == &duty) {
	todo |= RECALC;
    }
    else if (whichInput == &reset) {
	todo |= RESET;
    }
    else if (whichInput == &syncIn) {
	todo |= SYNC;
	syncEnable = TRUE;
    }
    else if (whichInput == &on) {
	if (!on.getValue() && state != PAUSED) {
	    todo |= PAUSE;
	    todo &= ~UNPAUSE;
	}
	else if (on.getValue() && state == PAUSED) {
	    todo |= UNPAUSE;
	    todo &= ~PAUSE;
	}
    }
    else if (whichInput == &timeIn && on.getValue() && stages != NULL) {
	SbTime off = now-baseTime;
	if (off >= stages[curStage+1].offset || off < stages[curStage].offset) {
	    enable = TRUE;
	    if (off > stages[nStages].offset)
		syncEnable = TRUE;
	}
    }
    if (todo) enable = TRUE;

    output.enable(enable);
    syncOut.enable(syncEnable);

#ifdef DEVELOP
    printf("TimeCounter::inputChanged %s state=%s on=%d reset=%d enable=%d syncEnable=%d todo=",
	   whichInput==&min	? "min   " :
	   whichInput==&max	? "max   " :
	   whichInput==&step	? "step  " :
	   whichInput==&reset	? "reset " :
	   whichInput==&syncIn	? "syncIn" :
	   whichInput==&on		? "on    " :
	   whichInput==&timeIn	? "------" :
	   " ...  ",
	   state==ON		? "ON    " :
	   state==PAUSED		? "PAUSED" :
	   "??????",
	   on.getValue(),
	   reset.getValue(),
	   enable,
	   syncEnable);
    if (todo) {
	int t=todo;
	if (todo&RECALC) printf("RECALC,"), t &= ~RECALC;
	if (todo&RESET) printf("RESET,"), t &= ~RESET;
	if (todo&SYNC) printf("SYNC,"), t &= ~SYNC;
	if (todo&PAUSE) printf("PAUSE,"), t &= ~PAUSE;
	if (todo&UNPAUSE) printf("UNPAUSE,"), t &= ~UNPAUSE;
	if (t) printf("????");
    }
    else printf("0");
    printf("\n");
#endif

    /***********
    **  
    ** first do the things queued up in the "todo" bitmask
    */

    // recalculate timing/counting info
    if (todo&RECALC) {

	// get values
	int mn = min.getValue();
	int mx = max.getValue();
	int stp = step.getValue();
	if (stp == 0) stp = 1;
	if (mn > mx) { int t = mn; mn=mx; mx=t; }

	int nDuty = duty.getNum();
	period	= 1/frequency.getValue();

	// reallocate stage array
	delete [] stages;
	nStages	= 1+abs((mx-mn)/stp);
	stages	= new struct Stage[nStages + 1];	// sentinnel on end

	// make sure curStage isn't out of range
	while (curStage >= nStages)	curStage -= nStages;

	// fill stage values and duties (non normalized)
        int i, val;
	for (i=0, val=stp>0?mn:mx; i<nStages; i++, val+=stp) {
	    stages[i].val = val;
	    stages[i].duty = (i < nDuty) ? duty[i] : 1.0;
	}

	// normalize duties
	double total;
	for (total=0, i=0; i<nStages; i++) total += stages[i].duty;
	for (i=0; i<nStages; i++) stages[i].duty /= total;

	// set offsets
	stages[0].offset = 0.0;
	for (i=1; i<nStages; i++) {
	    stages[i].offset = stages[i-1].offset + (stages[i-1].duty * period);
	}
	stages[nStages].offset = period;	// sentinnel value

#ifdef DEVELOP
	// debugging output
	printf("TimeCounter::evaluate RECALC min=%d max=%d step=%d "
	       "nStages=%d freq=%g period=%g\n",
	       mn, mx, stp, nStages, frequency.getValue(), period);
	for (i=0; i<nStages; i++) {
	    printf("\tstage %d:\tval=%d\tduty=%g\toffset=%g\n",
		   i, stages[i].val, stages[i].duty, stages[i].offset.getValue());
	}
#endif
    }

    // adjust the base time to reset the cycle
    if (todo&SYNC) {

	curStage = 0;
	baseTime = now;
	pauseOffset = 0.0;	// in case we're currently paused

#ifdef DEVELOP
	printf("TimeCounter::evaluate SYNC now=%s baseTime=%s\n",
	       now.format("%s.%i").getString(),
	       baseTime.format("%s.%i").getString());
#endif
    }

    // adjust the base time to reset the cycle
    if (todo&RESET) {
	// get inputs
	const int r = reset.getValue();
	int stp = step.getValue();
	if (stp==0) stp = 1;

	// convert input value to stage index
	curStage = (r-stages[0].val)/stp;
        if (curStage < 0)
            curStage = 0;
        else if (curStage >= nStages)
            curStage = nStages-1;

	// adjust base time so that requested stage is starting now.
	baseTime = now - stages[curStage].offset;
	pauseOffset = stages[curStage].offset;	// in case we're currently paused

#ifdef DEVELOP
	printf("TimeCounter::evaluate RESET reset=%d curStage=%d "
	       "val=%d offset=%s now=%s baseTime=%s\n",
	       r, curStage, stages[curStage].val,
	       stages[curStage].offset.format("%s.%i").getString(),
	       now.format("%s.%i").getString(),
	       baseTime.format("%s.%i").getString());
#endif
    }

    // when initiating a pause, remember the offset into the cycle
    if (todo&PAUSE) {
	pauseOffset = now - baseTime;
	state = PAUSED;
#ifdef DEVELOP
	printf("TimeCounter::evaluate PAUSE now=%s baseTime=%s pauseOffset=%s\n",
	       now.format("%s.%i").getString(),
	       baseTime.format("%s.%i").getString(),
	       pauseOffset.format("%s.%i").getString());
#endif
    }

    // to stop a pause, adjust the base time so that the saved offset
    // is maintained
    if (todo&UNPAUSE) {
	baseTime = now - pauseOffset;
	state = ON;
#ifdef DEVELOP
	printf("TimeCounter::evaluate UNPAUSE now=%s pauseOffset=%s "
	       "baseTime=%s\n",
	       now.format("%s.%i").getString(),
	       pauseOffset.format("%s.%i").getString(),
	       baseTime.format("%s.%i").getString());
#endif
    }

    todo = 0;
    /*
    ** done with the things queued up in the "todo" bitmask
    **  
    ************/
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Evaluation routine
//
// Use: private

void
SoTimeCounter::evaluate()
//
////////////////////////////////////////////////////////////////////////
{
    int i;
    // get current time value
    SbTime now = timeIn.getValue();

    // find out the current segment
    if (state==ON) {

	// move baseTime to the start of the current cycle
	while (now - baseTime >= period) baseTime += period;
	while (baseTime - now > period) baseTime -= period;

	// search for the offset
	SbTime offset = now - baseTime;
	for (i=nStages-1; i; i--)
	    if (offset >= stages[i].offset)
		break;

	curStage = i;
    }

#ifdef DEVELOP
printf("TimeCounter::evaluate state=%s now=%s baseTime=%s offset=%s "
       "curStage=%d output=%d\n",
       state==ON		? "ON    " :
       state==PAUSED		? "PAUSED" : "??????",
       now.format("%s.%i").getString(),
       baseTime.format("%s.%i").getString(),
       (now-baseTime).format("%s.%i").getString(),
       curStage,
       stages[curStage].val);
#endif
    SO_ENGINE_OUTPUT(output, SoSFShort, setValue(stages[curStage].val));
    prevStage = curStage;
}
