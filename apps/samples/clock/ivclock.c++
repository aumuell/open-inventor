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

//
//  ivclock - this views and operates a clock, whose geometry is
//  specified in an input file.
//
//  See the README file in this directory for a complete explanation.
//

#include <stdlib.h>
#include <X11/Intrinsic.h>
#include <math.h>

#include <Inventor/SoDB.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>
#include <Inventor/engines/SoTimeCounter.h>
#include <Inventor/engines/SoCounter.h>
#include <Inventor/engines/SoCalculator.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoRotationXYZ.h>

class ClockHands {
  public:
    SoRotationXYZ   *hour;
    SoRotationXYZ   *minute;
    SoRotationXYZ   *second;
};

static
void getCurrentTime( int &h, int &m, int &s )
{
    time_t clock;
    struct tm *timeofday;

    time(&clock);
    timeofday = (struct tm *)localtime(&clock);
    h = timeofday->tm_hour;
    if (h >= 12) h -= 12;
    m = timeofday->tm_min;
    s = timeofday->tm_sec;
}


// Find the clock hand separators.
// If the geometry does not include a minute hand and an hour hand
// return FALSE.
// Attach the engines to make the hands rotate.
static SbBool
setupHands(SoGroup *root)
{
    SbBool foundSecond = FALSE;
    int currentHour, currentMinute, currentSecond;

    // Engines to time the ticks of the clock hands
    SoTimeCounter *minuteTimer, *secondTimer;
    SoCounter *minuteCounter, *hourCounter;

    // Engines to convert the clock ticks into radian angles of rotation
    SoCalculator *secondRadians = new SoCalculator; 
    SoCalculator *minuteRadians = new SoCalculator; 
    SoCalculator *hourRadians = new SoCalculator; 

    // Rotation of the hands
    ClockHands	*rotation = new ClockHands;
    rotation->hour = NULL;
    rotation->minute = NULL;
    rotation->second = NULL;

    // Get current time of day to set the initial time
    getCurrentTime(currentHour, currentMinute, currentSecond);
    
    // Search for Names which denote the hands of the clock,
    // then insert these rotations into the clock geometry and
    // attach engines to update the rotations.
    // Also, since the hands will be constantly moving, turn 
    // render caching off for these separators. 

    SoSeparator *s;

    // Second hand (optional)
    s = (SoSeparator *)root->getByName("SecondHand");
    if (s != NULL && s->isOfType(SoSeparator::getClassTypeId())) {

	rotation->second = new SoRotationXYZ;
	rotation->second->axis = SoRotationXYZ::Z;
	s->insertChild(rotation->second, 0);

        // Second hand ticks from 0 to 59 in one minute (60 seconds)
	secondTimer = new SoTimeCounter;
        secondTimer->min = 0;
        secondTimer->max = 59;
        secondTimer->frequency = 1./60.;
        secondTimer->reset = currentSecond;
        secondRadians->a.connectFrom(&secondTimer->output);
        secondRadians->expression = "oa=-a*M_PI/30.0";
        rotation->second->angle.connectFrom(&secondRadians->oa);

	s->renderCaching = SoSeparator::OFF;
	foundSecond = TRUE;
    }

    // Hour hand 
    s = (SoSeparator *)root->getByName("HourHand");
    if (s != NULL && s->isOfType(SoSeparator::getClassTypeId())) {
	s->renderCaching = SoSeparator::OFF;
	rotation->hour = new SoRotationXYZ;
	rotation->hour->axis = SoRotationXYZ::Z;
	s->insertChild(rotation->hour, 0);

	hourCounter = new SoCounter;
	hourCounter->min = 0;
	hourCounter->max = 11;
        hourCounter->reset = currentHour;
        hourRadians->a.connectFrom(&hourCounter->output);
        hourRadians->expression = "oa=-((a+b/60.)*M_PI/6.0)";
        rotation->hour->angle.connectFrom(&hourRadians->oa);
    } else {
	return (FALSE); // The hour hand is required
    }

    // Minute hand 
    s = (SoSeparator *)root->getByName("MinuteHand");
    if (s != NULL && s->isOfType(SoSeparator::getClassTypeId())) {
	s->renderCaching = SoSeparator::OFF;
	rotation->minute = new SoRotationXYZ;
	rotation->minute->axis = SoRotationXYZ::Z;
	s->insertChild(rotation->minute, 0);

 	// If the clock has a second hand:
	// use the second timer to figure out when to tick the minutes
  	if (foundSecond) {
	    minuteCounter = new SoCounter;
	    minuteCounter->min = 0;
	    minuteCounter->max = 59;
            minuteCounter->reset = currentMinute;
	    minuteCounter->trigger.connectFrom(&secondTimer->syncOut);
            minuteRadians->a.connectFrom(&minuteCounter->output);

	    hourCounter->trigger.connectFrom(&minuteCounter->syncOut);
            hourRadians->b.connectFrom(&minuteCounter->output);
	} 
	// If the clock doesn't have a second hand:
        // use a minute timer
	else {
	    minuteTimer = new SoTimeCounter;
            minuteTimer->min = 0;
            minuteTimer->max = 59;
            minuteTimer->frequency = 1./3600.;
            minuteTimer->reset = currentMinute;
            minuteRadians->a.connectFrom(&minuteTimer->output);

	    hourCounter->trigger.connectFrom(&minuteTimer->syncOut);
            hourRadians->b.connectFrom(&minuteTimer->output);
	}

        minuteRadians->expression = "oa=-a*M_PI/30.0";
        rotation->minute->angle.connectFrom(&minuteRadians->oa);
    } else {
	return (FALSE);  // The minute hand is required
    }

    return (TRUE);
}

void
main(int argc, char **argv)
{
    char *filename = "clockData.iv";

    if (argc != 2) {
	fprintf(stderr, "NOTE: You can specify your own geometry file.\n");
	fprintf(stderr, "Run: %s inputFile\n",  argv[0]);
	fprintf(stderr, "Running with the default geometry, %s\n", filename);
    }
    else filename = argv[1];
    
    printf("\nUse left mouse to rotate the view.\n");
    printf("Middle mouse to pan the view.\n");
    printf("Left and middle together to zoom the view\n");
    printf("Right mouse for a popup menu showing more viewing features\n");
    
    Widget mainWindow = SoXt::init("Time");
    if (mainWindow == NULL)
	exit (1);

    // Read in the scene graph
    SoInput in;
    if (!in.openFile(filename)) {
        fprintf(stderr, "Error - could not open %s.\n", filename);
	exit(1);
    }

    SoSeparator *root = SoDB::readAll(&in);
    if (root == NULL) {
        fprintf(stderr, "Error - could not read %s.\n", filename);
	exit(1);
    }
    root->ref();
	
    if (! setupHands(root)) {
        fprintf(stderr, "Error - cannot use %s for the clock.\n", 
			filename);
        exit(1);
    }
	
    // Build and initialize the Inventor render area widget
    SoXtExaminerViewer *examiner = new SoXtExaminerViewer(mainWindow);
    examiner->setSize(SbVec2s(100, 100));
    examiner->setDecoration(FALSE);
    examiner->setFeedbackVisibility(FALSE);
    examiner->setSceneGraph(root);
    examiner->setTitle("Inventor Time");
    examiner->show();
    examiner->viewAll();

    SoXt::show(mainWindow);
    SoXt::mainLoop();
}

