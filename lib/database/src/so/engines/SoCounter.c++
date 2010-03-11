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
 |      SoCounter
 |
 |   Author(s)          : Ronen Barzel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SoDB.h>
#include <Inventor/engines/SoCounter.h>

SO_ENGINE_SOURCE(SoCounter);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoCounter::SoCounter()
//
////////////////////////////////////////////////////////////////////////
{
    SO_ENGINE_CONSTRUCTOR(SoCounter);
    SO_ENGINE_ADD_INPUT(min, (0));
    SO_ENGINE_ADD_INPUT(max, (1));
    SO_ENGINE_ADD_INPUT(step, (1));
    SO_ENGINE_ADD_INPUT(trigger, ());
    SO_ENGINE_ADD_INPUT(reset, (0));
#ifdef DEVELOP
    SO_ENGINE_ADD_INPUT(debug, (FALSE));
#endif
    SO_ENGINE_ADD_OUTPUT(output, SoSFShort);
    SO_ENGINE_ADD_OUTPUT(syncOut, SoSFTrigger);

    todo		= RECALC;
    stages		= NULL;
    curStage		= 0;
    isBuiltIn           = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoCounter::~SoCounter()
//
////////////////////////////////////////////////////////////////////////
{
    delete [] stages;
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
SoCounter::inputChanged(SoField *whichInput)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool syncEnable = FALSE;

    if (whichInput == &min ||
	whichInput == &max ||
	whichInput == &step) {
	todo |= RECALC;
    }
    else if (whichInput == &reset) {
	todo |= RESET;
    }
    else if (whichInput == &trigger && stages != NULL) {
	todo |= STEP;
	if (curStage == nStages-1)
	    syncEnable = TRUE;
    }

    syncOut.enable(syncEnable);

#ifdef DEVELOP
    if (debug.getValue()) {
	printf("Counter::inputChanged %s syncEnable=%d todo=",
		whichInput==&min	? "min    " :
		whichInput==&max	? "max    " :
		whichInput==&step	? "step   " :
		whichInput==&reset	? "reset  " :
		whichInput==&trigger	? "trigger" :
					  " ...   ",
		syncEnable);
	if (todo) {
	    int t=todo;
	    if (todo&RECALC) printf("RECALC,"), t &= ~RECALC;
	    if (todo&RESET) printf("RESET,"), t &= ~RESET;
	    if (todo&STEP) printf("STEP,"), t &= ~STEP;
	    if (t) printf("????");
	}
	else printf("0");
	printf("\n");
    }
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

	// reallocate stage array
	delete [] stages;
	nStages	= 1+abs((mx-mn)/stp);
	stages	= new struct Stage[nStages];	// sentinnel on end

	// make sure curStage isn't out of range
	while (curStage >= nStages)	curStage -= nStages;

	// fill stage values and duties (non normalized)
	for (int i=0, val=stp>0?mn:mx; i<nStages; i++, val+=stp) {
	    stages[i].val = val;
	}

#ifdef DEVELOP
	// debugging output
	if (debug.getValue()) {
	    printf("Counter::evaluate RECALC min=%d max=%d step=%d nStages=%d\n",
			mn, mx, stp, nStages);
	    for (i=0; i<nStages; i++) {
		printf("\tstage %d:\tval=%d\n", i, stages[i].val);
	    }
	}
#endif
    }

    // adjust reset the cycle
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

#ifdef DEVELOP
	if (debug.getValue()) {
	    printf("Counter::evaluate RESET reset=%d curStage=%d .val=%d\n",
			r, curStage, stages[curStage].val);
	}
#endif
    }

    if (todo & STEP) {
	curStage++;
	if (curStage == nStages)
	    curStage = 0;
#ifdef DEVELOP
	if (debug.getValue()) {
	    printf("Counter::evaluate STEP  curStage=%d .val=%d\n",
			curStage, stages[curStage].val);
	}
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
SoCounter::evaluate()
//
////////////////////////////////////////////////////////////////////////
{
#ifdef DEVELOP
    if (debug.getValue()) {
	printf("Counter::evaluate output=%d\n",
		stages[curStage].val);
    }
#endif
    SO_ENGINE_OUTPUT(output, SoSFShort, setValue(stages[curStage].val));
}
