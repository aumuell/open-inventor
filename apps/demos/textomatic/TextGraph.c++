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
// Routines for handling the text scene graph
//
// The scene graph consists of a Profile2, Font, and Translation node,
// and then a series of separators, each of which represents a
// paragraph.  The paragraphs consist of a Separator, under which
// there is a Translation, Material, and a Text3 node (underneath
// another separator which is used to cache the polygons).
//
// The whole thing is underneath a root Separator.
//

#include <stdio.h>
#include <string.h>
#include <math.h>

#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/SoXtClipboard.h>
#include <Inventor/Xt/SoXtMaterialEditor.h>
#include <Inventor/SoPath.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoLinearProfile.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoProfileCoordinate2.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoText3.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/sensors/SoAlarmSensor.h>
#include <Inventor/sensors/SoSensor.h>

#include <Xm/Xm.h>
#include <X11/Shell.h>
#include <Xm/Text.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/ToggleBG.h>
#include <Xm/LabelG.h>

#include "TextGraph.h"
#include "TextWrapper.h"
#include "labels.h"

//
// Global variables.  Evil, but I'm in a hurry.
//
SoProfileCoordinate2 *profileCoords;
SoLinearProfile *profile;

// Global for resources...
extern Labels labels;


//
// Variables local to this file.  Also evil (not as evil as globals),
// but I'm not going to bother to encapsulate all this into a class,
// since I don't really plan on reusing it.
//

static SoSeparator *root;

struct TextThing {
    SoSeparator *top;
    SoTranslation *translation;
    SoSeparator *cache;
    SoText3 *text;
};
    
static TextThing *paragraphs;
static SoFont *TheFont;
static SoTranslation *TheTranslation;
static SoMaterial *TheMaterial;
static int numParagraphs = 0;
static const float spacing = 1.0;	// Space between lines
static const float paraSpacing = 0.5;	// Extra space between paragraphs

// Which parts material editor is editing
static int whichMatEditing = SoText3::ALL;
// Which parts (front/sides/back) of the text are visible
static int whichPartsOn = SoText3::FRONT;

//
// These two callbacks are called when the text in the Motif text
// widget changes.  delayUpdateText is called while typing is
// happening; it, in turn, just schedules the delayCB to be called one
// second later.  While typing, that sensor will be constantly
// rescheduled, so delayCB is not called until one second after typing
// has ceased.  This is a pretty good compromise between the ideal
// immediate feedback and some sort of explicit gesture (like an OK
// button) that the user would have to use.
//
void
delayCB(void *textwidget, SoSensor *)
{
    updateText(NULL, (XtPointer)textwidget, NULL);
}
void
delayUpdateText(Widget, XtPointer textwidget, XtPointer)
{
    static SoAlarmSensor *alarmSensor = NULL;
    if (alarmSensor == NULL) {
	alarmSensor = new SoAlarmSensor;
	alarmSensor->setFunction(delayCB);
    }
    else {
	alarmSensor->unschedule();
    }
    alarmSensor->setData(textwidget);
    alarmSensor->setTimeFromNow(SbTime(1.0));	// Wait a second...
    alarmSensor->schedule();
}
//
// Actually change the scene graph based on the text entered into the
// Motif text widget
//
void
updateText(Widget, XtPointer textwidget, XtPointer)
{
    char *str = XmTextGetString((Widget)textwidget);
    //
    // The TextWrapper word-wraps the text string.  Here, we tell it
    // to format the string for 30 character lines.
    //
    TextWrapper twrap(str, 30);
    XtFree(str);

    //
    // Next, get rid of old text in scene graph and add the new text
    // (with appropriate translation to move the text apart)
    //
    int i;
    for (i = numParagraphs-1; i >= 0; i--) {
	root->removeChild(paragraphs[i].top);
    }
    if (numParagraphs != 0) {
	delete[] paragraphs;
    }

    numParagraphs = twrap.numParagraphs();

    if (numParagraphs != 0) {
	paragraphs = new TextThing[numParagraphs];
    }
    float currentY = 0.0;
    for (i = 0; i < numParagraphs; i++) {
	SoSeparator *top = new SoSeparator;
	paragraphs[i].top = top;
	root->addChild(top);
	
	paragraphs[i].translation = new SoTranslation;
	top->addChild(paragraphs[i].translation);
	paragraphs[i].translation->
	    translation.setValue(0.0, currentY, 0.0);

	paragraphs[i].cache = new SoSeparator;
	top->addChild(paragraphs[i].cache);
        paragraphs[i].cache->renderCaching = SoSeparator::ON;
        paragraphs[i].cache->boundingBoxCaching = SoSeparator::ON;

	SoText3 *tnode = new SoText3;
	paragraphs[i].text = tnode;
	paragraphs[i].cache->addChild(tnode);
	tnode->justification = SoText3::CENTER;
	tnode->parts = whichPartsOn;

	for (int j = 0; j < twrap.numLines(i); j++) {
	    tnode->string.set1Value(j, twrap.getLine(i, j));
	    currentY -= spacing;
	}
	currentY -= paraSpacing;
    }
    //
    // This centers the entire text vertically
    //
    float total_d = (-currentY - paraSpacing - spacing);
    TheTranslation->translation.setValue(0.0, total_d/2.0, 0.0);
}

//
// Copy the text scene graph to the clipboard
//
void
copyText(Widget, XtPointer w, XtPointer cbstruct)
{
    static SoXtClipboard *theClipboard = NULL;

    if (theClipboard == NULL) {
	theClipboard = new SoXtClipboard((Widget)w);
    }
    theClipboard->copy(root,
	((XmAnyCallbackStruct *)cbstruct)->event->xbutton.time);
}

//
// Turn the given part on or off.
//
static void
togglePart(Widget, XtPointer mydata, XtPointer cbstruct)
{
    int flag = ((XmToggleButtonCallbackStruct *)cbstruct)->set;
    int whichPart = (long)mydata;

    if (flag) {	// Turn part on
	whichPartsOn |= whichPart;
    }
    else {
	whichPartsOn &= ~whichPart;
    }
    for (int i = 0; i < numParagraphs; i++) {
	paragraphs[i].text->parts.setValue(whichPartsOn);
    }
}

//
// And cause the material editor to edit/not edit the given part.
//
static void
editToggle(Widget, XtPointer mydata, XtPointer cbstruct)
{
    int flag = ((XmToggleButtonCallbackStruct *)cbstruct)->set;
    int whichPart = (long)mydata;

    if (flag) {	// Edit the part
	whichMatEditing |= whichPart;
    }
    else {	// Don't edit it
	whichMatEditing &= (~whichPart);
    }
}

//
// The material editor calls this function whenever its material
// changes.  We have to copy its material into the right place in the
// scene graph.
//
static void
matEditCB(void *, const SoMaterial *mtl)
{
    if (whichMatEditing & SoText3::FRONT) {
	TheMaterial->ambientColor.set1Value(0, mtl->ambientColor[0]);
	TheMaterial->diffuseColor.set1Value(0, mtl->diffuseColor[0]);
	TheMaterial->specularColor.set1Value(0, mtl->specularColor[0]);
	TheMaterial->emissiveColor.set1Value(0, mtl->emissiveColor[0]);
	TheMaterial->shininess.set1Value(0, mtl->shininess[0]);
	TheMaterial->transparency.set1Value(0, mtl->transparency[0]);
    }
    if (whichMatEditing & SoText3::SIDES) {
	TheMaterial->ambientColor.set1Value(1, mtl->ambientColor[0]);
	TheMaterial->diffuseColor.set1Value(1, mtl->diffuseColor[0]);
	TheMaterial->specularColor.set1Value(1, mtl->specularColor[0]);
	TheMaterial->emissiveColor.set1Value(1, mtl->emissiveColor[0]);
	TheMaterial->shininess.set1Value(1, mtl->shininess[0]);
	TheMaterial->transparency.set1Value(1, mtl->transparency[0]);
    }
    if (whichMatEditing & SoText3::BACK) {
	TheMaterial->ambientColor.set1Value(2, mtl->ambientColor[0]);
	TheMaterial->diffuseColor.set1Value(2, mtl->diffuseColor[0]);
	TheMaterial->specularColor.set1Value(2, mtl->specularColor[0]);
	TheMaterial->emissiveColor.set1Value(2, mtl->emissiveColor[0]);
	TheMaterial->shininess.set1Value(2, mtl->shininess[0]);
	TheMaterial->transparency.set1Value(2, mtl->transparency[0]);
    }
}

//
// This is called when the user presses the 'Edit Parts' button.  It
// creates the parts editor (if necessary) and then manages it.
//
void
createPartEditor(Widget w, XtPointer, XtPointer)
{
    static Widget shell = NULL;

    if (shell == NULL) {
	// Create 3 labels and 6 togglebuttons, and one material
	// editor
	Arg resources[20];
	int n;
	
	// change the XmNdeleteResponse (when use closes the window) from
	// XmDESTROY to XmUNMAP since we don't want to delete the material
	// editor widgets (we would also have to delete the material editor
	// class since we can rebuild them independently). We could use
	// XmCreateFormDialog() but some some strange reason the material
	// editor has problems building inside one of them.
	n = 0;
	XtSetArg(resources[n], XmNdeleteResponse, XmUNMAP); ++n;
	shell = XtCreatePopupShell("editParts", topLevelShellWidgetClass, 
	    SoXt::getShellWidget(w), resources, n);
	
	Widget form = XmCreateForm(shell, NULL, NULL, 0);

	SoXtMaterialEditor *medit = new SoXtMaterialEditor(form);
	medit->addMaterialChangedCallback(matEditCB, NULL);
	XtSetArg(resources[n], XmNtopAttachment, XmATTACH_FORM); ++n;
	XtSetArg(resources[n], XmNleftAttachment, XmATTACH_FORM); ++n;
	XtSetArg(resources[n], XmNrightAttachment, XmATTACH_FORM); ++n;
	XtSetValues(medit->getWidget(), resources, n);

	// Create three columns (row/column widgets) containing 3
	// buttons each, for a total of 12 widgets/gadgets.
	Widget w[12]; // Used for XtManageChildren() call
	
	n = 0;
	XtSetArg(resources[n], XmNnumColumns, 1); ++n;
	XtSetArg(resources[n], XmNorientation, XmVERTICAL); ++n;
	XtSetArg(resources[n], XmNpacking, XmPACK_COLUMN); ++n;
	XtSetArg(resources[n], XmNisAligned, TRUE); ++n;
	XtSetArg(resources[n], XmNentryAlignment, XmALIGNMENT_END); ++n;
	XtSetArg(resources[n], XmNadjustLast, FALSE); ++n;
	XtSetArg(resources[n], XmNtopAttachment, XmATTACH_WIDGET); ++n;
	XtSetArg(resources[n], XmNtopWidget, medit->getWidget()); ++n;
	XtSetArg(resources[n], XmNbottomAttachment, XmATTACH_FORM); ++n;

	// All the previous resources are the same for the three
	// column widgets...
	int nn = n;
	XtSetArg(resources[n], XmNleftAttachment, XmATTACH_FORM); ++n;
	XtSetArg(resources[n], XmNrightAttachment, XmATTACH_POSITION); ++n;
	XtSetArg(resources[n], XmNrightPosition, 32); ++n;
	w[0] = XmCreateRowColumn(form, "PartsLabels", resources, n);

	n = nn;
	XtSetArg(resources[n], XmNleftAttachment, XmATTACH_POSITION); ++n;
	XtSetArg(resources[n], XmNleftPosition, 34); ++n;
	XtSetArg(resources[n], XmNrightAttachment, XmATTACH_POSITION); ++n;
	XtSetArg(resources[n], XmNrightPosition, 66); ++n;
	w[1] = XmCreateRowColumn(form, "PartsOnOff", resources, n);

	n = nn;
	XtSetArg(resources[n], XmNleftAttachment, XmATTACH_POSITION); ++n;
	XtSetArg(resources[n], XmNleftPosition, 67); ++n;
	XtSetArg(resources[n], XmNrightAttachment, XmATTACH_FORM); ++n;
	w[2] = XmCreateRowColumn(form, "PartsEdit", resources, n);
	n = 0;

#define STRING(a) XmStringCreateLocalized(a)

	XtSetArg(resources[n], XmNlabelString, STRING(labels.front)); ++n;
	w[3] = XmCreateLabelGadget(w[0], "frontLabel", resources, n); n = 0;
	XtSetArg(resources[n], XmNlabelString, STRING(labels.sides)); ++n;
	w[4] = XmCreateLabelGadget(w[0], "sidesLabel", resources, n); n = 0;
	XtSetArg(resources[n], XmNlabelString, STRING(labels.back)); ++n;
	w[5] = XmCreateLabelGadget(w[0], "backLabel",  resources, n); n = 0;

	// The three parts toggles
	XtSetArg(resources[n], XmNlabelString, STRING(labels.on)); ++n;
	XtSetArg(resources[n], XmNset, 1); ++n;
	w[6] = XmCreateToggleButtonGadget(w[1], "frontOnOff", resources, n);
	XtAddCallback(w[6], XmNvalueChangedCallback,
		      togglePart, (XtPointer)SoText3::FRONT);
	n = 0;
	
	XtSetArg(resources[n], XmNlabelString, STRING(labels.on)); ++n;
	if (whichPartsOn & SoText3::SIDES) {
	    XtSetArg(resources[n], XmNset, 1); ++n;
	} else {
	    XtSetArg(resources[n], XmNset, 0); ++n;
	}
	w[7] = XmCreateToggleButtonGadget(w[1], "sidesOnOff", resources, n);
	XtAddCallback(w[7], XmNvalueChangedCallback,
		      togglePart, (XtPointer)SoText3::SIDES);
	n = 0;
	
	XtSetArg(resources[n], XmNlabelString, STRING(labels.on)); ++n;
	if (whichPartsOn & SoText3::BACK) {
	    XtSetArg(resources[n], XmNset, 1); ++n;
	} else {
	    XtSetArg(resources[n], XmNset, 0); ++n;
	}
	w[8] = XmCreateToggleButtonGadget(w[1], "backOnOff", resources, n);
	XtAddCallback(w[8], XmNvalueChangedCallback,
		      togglePart, (XtPointer)SoText3::BACK);
	n = 0;

	// And the three material toggles
	XtSetArg(resources[n], XmNlabelString, STRING(labels.edit)); ++n;
	XtSetArg(resources[n], XmNset, 1); ++n;
	w[9] = XmCreateToggleButtonGadget(w[2], "frontEdit", resources, n);
	XtAddCallback(w[9], XmNvalueChangedCallback,
		      editToggle, (XtPointer)SoText3::FRONT);
	
	w[10] = XmCreateToggleButtonGadget(w[2], "sidesEdit", resources, n);
	XtAddCallback(w[10], XmNvalueChangedCallback,
		      editToggle, (XtPointer)SoText3::SIDES);
		      
	w[11] = XmCreateToggleButtonGadget(w[2], "backEdit",  resources, n);
	XtAddCallback(w[11], XmNvalueChangedCallback,
		      editToggle, (XtPointer)SoText3::BACK);
	
	XtManageChildren(w+3, 3); // labels/buttons
	XtManageChildren(w+6, 3);
	XtManageChildren(w+9, 3);
	medit->show();
	XtManageChildren(w, 3);	// row-columns..
	XtManageChild(form);
    }
    SoXt::show(shell);
}

//
// This function is called when the user goes into or out of the
// profile editor window.  I could give the line manipulator
// start/edit/finish callbacks, and have those start and finish
// callbacks turn off/on render caching, but I think it is neat to be
// able to demonstrate the change in speed of caching vs. no caching
// by moving the mouse between windows while the text spins.
//
void
setTextCaching(int flag)
{
    for (int i = 0; i < numParagraphs; i++) {
	if (flag) {
            paragraphs[i].cache->renderCaching = SoSeparator::ON;
	}
	else {        
            paragraphs[i].cache->renderCaching = SoSeparator::OFF;
        }
    }
}

//
// Create the main scene graph (the scene graph viewed by the
// Examiner).
//
SoSeparator *
createTextSceneGraph()
{
    SoSeparator *veryTop = new SoSeparator;
    veryTop->ref();

    SoPerspectiveCamera *camera = new SoPerspectiveCamera;
    veryTop->addChild(camera);
    camera->heightAngle = 28.0 * M_PI/180.0;
    camera->aspectRatio = 1.0;
    camera->position.setValue(0.0, 0.0, 25.0);
    camera->nearDistance = 8.0;
    camera->farDistance = 50.0;
    camera->focalDistance = 25.0;

    root = new SoSeparator;
    veryTop->addChild(root);

    TheFont = new SoFont;
    TheFont->name = labels.sofontlist;
    TheFont->size = 1;
    root->addChild(TheFont);

    TheMaterial = new SoMaterial;
    root->addChild(TheMaterial);
    // Make all three materials (for front/back/sides) the same:
    matEditCB(NULL, TheMaterial);

    SoMaterialBinding *mb = new SoMaterialBinding;
    root->addChild(mb);
    mb->value = SoMaterialBinding::PER_PART;

    TheTranslation = new SoTranslation;
    root->addChild(TheTranslation);

    profileCoords = new SoProfileCoordinate2;
    root->addChild(profileCoords);

    profile = new SoLinearProfile;
    root->addChild(profile);

    return veryTop;
}

