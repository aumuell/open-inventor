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

#include <stdlib.h>
#include <ctype.h>
#include <X11/Intrinsic.h>

#include <Xm/Form.h>
#include <Xm/LabelG.h>
#include <Xm/PushB.h>
#include <Xm/Scale.h>
#include <Xm/SeparatoG.h>
#include <Xm/Text.h>
#include <Xm/ToggleBG.h>

#include <Inventor/errors/SoReadError.h>
#include <Inventor/fields/SoField.h>
#include <Inventor/fields/SoFieldData.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/sensors/SoNodeSensor.h>

#include "Error.h"
#include "FieldEditor.h"
#include "MotifHelp.h"

char	*FieldEditor::fieldBuf;		// Buffer for writing field values
int	FieldEditor::fieldBufSize = 0;	// Size of buffer in bytes

////////////////////////////////////////////////////////////////////////
//
// This class is used to hold info about each individual field widget.
// It contains the widget and other information accessible through
// callbacks in one neat package.
//
////////////////////////////////////////////////////////////////////////

struct FieldInfo {

    // These are set in initInfo():

    FieldEditor	*editor;	// Pointer to FieldEditor this is part of
    SoField	*field;		// Field being edited
    int		index;		// Index of field in node
    SbName	fieldName;	// Name of field
    SbBool	setToDefault;	// TRUE if user set value to default
    SbBool	isMultiple;	// TRUE if field is multiple value

    // These are set in buildFieldWidget():

    Widget	widget;		// Top-level field editing widget
    Widget	textWidget;	// Text editing widget
    Widget	ignoreButton;	// Ignore button widget
    Widget	defaultButton;	// Set-default button widget

    // These are used only for multiple-value fields:

    Widget	numberWidget;	// Text widget showing current scale value
    Widget	scrollWidget;	// Text widget scrollbar widget
};

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor. Takes pointer to node whose fields we are to edit.
//

FieldEditor::FieldEditor(SoNode *node, Widget parent, const char *name)
		: SoXtComponent(parent, name)
//
////////////////////////////////////////////////////////////////////////
{
    int	i;

    nodeToEdit = node;
    nodeToEdit->ref();

    defNode = (SoNode *) node->getTypeId().createInstance();
    defNode->ref();

    finishCB   = NULL;
    finishData = NULL;

    errorString = NULL;

    // Set up sensor. Make it priority 0 so we can be called
    // immediately when a field changes. This way we can update only
    // the fields that change in the sensor callback.
    nodeSensor = new SoNodeSensor(&FieldEditor::nodeSensorCB, (void *) this);
    nodeSensor->setPriority(0);
    nodeSensor->attach(node);

    // Allocate buffer to write field values into. We have to use
    // malloc, since we may use realloc on it later
    if (fieldBufSize == 0)
	fieldBuf = (char *) malloc((unsigned) (fieldBufSize = 1028));

    // Allocate and initialize field info structures
    numFields  = getNumFields(nodeToEdit);
    fieldInfos = new FieldInfo[numFields];
    for (i = 0; i < numFields; i++)
	initInfo(&fieldInfos[i], i);

    // Set up the class name
    setClassName("FieldEditor");

    // Build and set the widget
    setBaseWidget(buildWidget(getParentWidget()));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.
//

FieldEditor::~FieldEditor()
//
////////////////////////////////////////////////////////////////////////
{
    // Widget stuff is handled in SoXtComponent...

    nodeToEdit->unref();
    defNode->unref();
    delete nodeSensor;

    delete [] fieldInfos;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns number of fields in node being edited. This is static to
//    allow the check to be made before an editor is constructed.
//

int
FieldEditor::getNumFields(SoNode *node)
//
////////////////////////////////////////////////////////////////////////
{
    const SoFieldData	*fdata = node->getFieldData();

    if (fdata == NULL)
	return 0;

    return fdata->getNumFields();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets a callback to call when editing is finished.
//

void
FieldEditor::setFinishCallback(FieldEditorCB *cb, const void *userData)
//
////////////////////////////////////////////////////////////////////////
{
    finishCB   = cb;
    finishData = (void *) userData;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Builds and returns top-level editor widget.
//

Widget
FieldEditor::buildWidget(Widget parent)
//
////////////////////////////////////////////////////////////////////////
{
    Widget	topWidget, fieldForm, sep, buttonForm;
    ARG_VARS(20);

    // If no fields to edit, go away
    if (numFields == 0)
	return NULL;

    // The title is the class of the node whose fields we are editing
    setTitle(nodeToEdit->getTypeId().getName().getString());

    // Create the top level form widget and register it with a class name
    RESET_ARGS();
    topWidget = XmCreateForm(parent, (char *) getWidgetName(), ARGS);
    registerWidget(topWidget);

    // Create buttons at bottom
    buttonForm = buildButtonWidget(topWidget);
    RESET_ARGS();
    ADD_LEFT_FORM(4);
    ADD_RIGHT_FORM(4);
    ADD_BOTTOM_FORM(4);
    XtSetValues(buttonForm, ARGS);

    // Create separator between buttons and field editors
    RESET_ARGS();
    ADD_ARG(XmNorientation, XmHORIZONTAL);
    ADD_LEFT_FORM(0);
    ADD_RIGHT_FORM(0);
    ADD_BOTTOM_WIDGET(buttonForm, 4);
    sep = XmCreateSeparatorGadget(topWidget, "Separator", ARGS);

    // Create widget containing all field widgets
    fieldForm = buildFieldForm(topWidget);
    RESET_ARGS();
    ADD_TOP_FORM(4);
    ADD_LEFT_FORM(4);
    ADD_RIGHT_FORM(4);
    ADD_BOTTOM_WIDGET(sep, 4);
    XtSetValues(fieldForm, ARGS);

    // Manage the children
    XtManageChild(fieldForm);
    XtManageChild(sep);
    XtManageChild(buttonForm);

    return topWidget;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Creates and returns a widget containing all field-editing widgets.
//

Widget
FieldEditor::buildFieldForm(Widget parent)
//
////////////////////////////////////////////////////////////////////////
{
    Widget	form, sep;
    FieldInfo	*info;
    int		i;
    ARG_VARS(20);

    // Create form to hold all field widgets
    RESET_ARGS();
    form = XmCreateForm(parent, "Form", ARGS);

    // Create widget for each field and add them to form
    for (i = 0, info = fieldInfos; i < numFields; i++, info++) {

	// Build separator if necessary
	if (i > 0) {
	    RESET_ARGS();
	    ADD_ARG(XmNorientation,	XmHORIZONTAL);
	    ADD_LEFT_FORM(0);
	    ADD_RIGHT_FORM(0);
	    ADD_TOP_WIDGET((info - 1)->widget, 1);
	    sep = XmCreateSeparatorGadget(form, "Separator", ARGS);
	    XtManageChild(sep);
	}

	// Build field editor widget; sets widget field in info
	buildFieldWidget(form, info);

	// Set up widget contents from node
	updateInfo(info);

	RESET_ARGS();
	ADD_LEFT_FORM(0);
	ADD_RIGHT_FORM(0);
	if (i == 0) {
	    ADD_TOP_FORM(0);
	}
	else {
	    ADD_TOP_WIDGET(sep, 0);
	}
	if (i == numFields - 1) {
	    ADD_BOTTOM_FORM(0);
	}
	XtSetValues(info->widget, ARGS);
	XtManageChild(info->widget);
    }

    return form;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Creates a widget for editing the i'th field
//

void
FieldEditor::buildFieldWidget(Widget parent, FieldInfo *info)
//
////////////////////////////////////////////////////////////////////////
{
    Widget	form, label, sep1, valueWidget, sep2, butForm, ign, def;
    ARG_VARS(20);

    // Create a form widget
    RESET_ARGS();
    form = XmCreateForm(parent, "Field", ARGS);

    // Create a label widget with the name of the field
    RESET_ARGS();
    ADD_ARG(XmNlabelString,	STRING((char *) info->fieldName.getString()));
    ADD_ARG(XmNwidth,			140);
    ADD_LEFT_FORM(4);
    ADD_TOP_FORM(4);
    ADD_BOTTOM_FORM(4);
    label = XmCreateLabelGadget(form, "Label", ARGS);

    // Create a separator widget
    RESET_ARGS();
    ADD_ARG(XmNorientation,		XmVERTICAL);
    ADD_LEFT_WIDGET(label, 4);
    ADD_TOP_FORM(0);
    ADD_BOTTOM_FORM(0);
    sep1 = XmCreateSeparatorGadget(form, "Separator", ARGS);

    // Create a form widget to hold buttons
    RESET_ARGS();
    ADD_BOTTOM_FORM(4);
    ADD_RIGHT_FORM(4);
    butForm = XmCreateForm(form, "FieldButtons", ARGS);

    // Create a push button to set the field to default value.
    // (Disable it if the field is already the default value)
    RESET_ARGS();
    ADD_ARG(XmNlabelString,		STRING("Set To Default"));
    ADD_ARG(XmNheight,			28);
    ADD_ARG(XmNhighlightThickness,	0);
    ADD_RIGHT_FORM(0);
    ADD_TOP_FORM(0);
    ADD_BOTTOM_FORM(0);
    def = XmCreatePushButton(butForm, "defaultButton", ARGS);
    ADD_CB(def, XmNactivateCallback, &FieldEditor::defaultButtonCB, info);

    // Create a toggle button to set the ignore flag
    RESET_ARGS();
    ADD_ARG(XmNlabelString,		STRING("Ignore"));
    ADD_ARG(XmNhighlightThickness,	0);
    ADD_TOP_FORM(0);
    ADD_BOTTOM_FORM(0);
    ADD_LEFT_FORM(0);
    ADD_RIGHT_WIDGET(def, 4);
    ign = XmCreateToggleButtonGadget(butForm, "ignoreButton", ARGS);

    // Create a separator widget
    RESET_ARGS();
    ADD_ARG(XmNorientation,		XmVERTICAL);
    ADD_RIGHT_WIDGET(butForm, 4);
    ADD_TOP_FORM(0);
    ADD_BOTTOM_FORM(0);
    sep2 = XmCreateSeparatorGadget(form, "Separator", ARGS);

    // Field value editor widget
    if (info->isMultiple)
	valueWidget = buildMultipleValueWidget(form, info);
    else
	valueWidget = buildSingleValueWidget(form, info);
    RESET_ARGS();
    ADD_LEFT_WIDGET(sep1, 4);
    ADD_RIGHT_WIDGET(sep2, 4);
    ADD_TOP_FORM(0);
    ADD_BOTTOM_FORM(0);
    XtSetValues(valueWidget, ARGS);

    // Manage the children
    XtManageChild(label);
    XtManageChild(sep1);
    XtManageChild(valueWidget);
    XtManageChild(sep2);
    XtManageChild(ign);
    XtManageChild(def);
    XtManageChild(butForm);

    info->widget	= form;
    info->ignoreButton	= ign;
    info->defaultButton	= def;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Creates and returns a widget that does the main work of editing
//    a multiple-value field.
//

Widget
FieldEditor::buildMultipleValueWidget(Widget parent, FieldInfo *info)
//
////////////////////////////////////////////////////////////////////////
{
    Widget	form, text, number, scroll;
    char	buf[20];
    ARG_VARS(20);

    //////////////////////////////////////////////////////////////////
    //
    // Set up top level form widget
    //

    RESET_ARGS();
    form = XmCreateForm(parent, "Form", ARGS);

    //////////////////////////////////////////////////////////////////
    //
    // Set up the main scrolled text widget
    //

    RESET_ARGS();
    ADD_ARG(XmNeditMode,		XmMULTI_LINE_EDIT);
    ADD_ARG(XmNcolumns,			30);
    ADD_ARG(XmNrows,			5);
    ADD_RIGHT_FORM(4);
    ADD_TOP_FORM(4);
    ADD_BOTTOM_FORM(4);
    text = XmCreateScrolledText(form, "text", ARGS);

    // Set up a callback to check changes to text before allowing them
    ADD_CB(text, XmNmodifyVerifyCallback, &FieldEditor::textChangedCB, info);

    // Get the scroll bar resource from the scrolled window (the
    // parent widget of the scrolled text)
    XtVaGetValues(XtParent(text), XmNverticalScrollBar, &scroll, NULL);

    // Set up callbacks on the scroll bar so that the line number
    // widget scrolls with the other text. Note that we have to add
    // this to all of the scroll bar callbacks since they are already
    // set up for the scrolled window.
    ADD_CB(scroll, XmNvalueChangedCallback,	&FieldEditor::scrollCB, info);
    ADD_CB(scroll, XmNincrementCallback,	&FieldEditor::scrollCB, info);
    ADD_CB(scroll, XmNdecrementCallback,	&FieldEditor::scrollCB, info);
    ADD_CB(scroll, XmNpageIncrementCallback,	&FieldEditor::scrollCB, info);
    ADD_CB(scroll, XmNpageDecrementCallback,	&FieldEditor::scrollCB, info);
    ADD_CB(scroll, XmNtoTopCallback,		&FieldEditor::scrollCB, info);
    ADD_CB(scroll, XmNtoBottomCallback,		&FieldEditor::scrollCB, info);
    ADD_CB(scroll, XmNdragCallback,		&FieldEditor::scrollCB, info);

    //////////////////////////////////////////////////////////////////
    //
    // Set up the line number text widget
    //

    RESET_ARGS();
    ADD_ARG(XmNeditable,		False);
    ADD_ARG(XmNeditMode,		XmMULTI_LINE_EDIT);
    ADD_ARG(XmNvalue,			buf);
    ADD_ARG(XmNcolumns,			5);
    ADD_ARG(XmNrows,			5);
    ADD_ARG(XmNhighlightThickness,	0);
    ADD_RIGHT_WIDGET(text, 0);
    ADD_TOP_FORM(6);
    ADD_LEFT_FORM(4);
    ADD_BOTTOM_FORM(4);
    number = XmCreateText(form, "numbers", ARGS);
    XmTextSetString(number, "?");	// Bogus value

    // Set up a callback on the line number widget so that users
    // cannot scroll the text in there by moving the cursor
    ADD_CB(number, XmNmotionVerifyCallback, &FieldEditor::lineCB, info);

    XtManageChild(text);
    XtManageChild(number);

    info->textWidget	= text;
    info->numberWidget	= number;
    info->scrollWidget	= scroll;

    return form;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Creates and returns a widget that does the main work of editing
//    a single-value field.
//

Widget
FieldEditor::buildSingleValueWidget(Widget parent, FieldInfo *info)
//
////////////////////////////////////////////////////////////////////////
{
    Widget	form, text;
    ARG_VARS(20);

    RESET_ARGS();
    form = XmCreateForm(parent, "Form", ARGS);

    RESET_ARGS();
    ADD_ARG(XmNcolumns,		30);
    ADD_ARG(XmNeditMode,	XmSINGLE_LINE_EDIT);
    ADD_LEFT_FORM(4);
    ADD_RIGHT_FORM(4);
    ADD_TOP_FORM(4);
    ADD_BOTTOM_FORM(4);
    text = XmCreateText(form, "Text", ARGS);

    ADD_CB(text, XmNmodifyVerifyCallback, &FieldEditor::textChangedCB, info);

    XtManageChild(text);

    info->textWidget = text;

    return form;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Creates and returns a widget containing all buttons.
//

Widget
FieldEditor::buildButtonWidget(Widget parent)
//
////////////////////////////////////////////////////////////////////////
{
    Widget	form, acceptButton, revertButton, cancelButton;
    ARG_VARS(20);

    // Create a form widget to hold it all
    RESET_ARGS();
    form = XmCreateForm(parent, "Field", ARGS);

    // Create "Apply", "Accept", "Revert", and "Cancel" buttons
    RESET_ARGS();
    ADD_ARG(XmNlabelString,	STRING("Accept"));
    ADD_ARG(XmNhighlightThickness,	0);
    ADD_LEFT_FORM(0);
    ADD_TOP_FORM(0);
    ADD_BOTTOM_FORM(0);
    acceptButton = XmCreatePushButton(form, "acceptButton", ARGS);
    ADD_CB(acceptButton, XmNactivateCallback,
	   &FieldEditor::acceptButtonCB, this);

    RESET_ARGS();
    ADD_ARG(XmNlabelString,	STRING("Apply"));
    ADD_ARG(XmNhighlightThickness,	0);
    ADD_TOP_FORM(0);
    ADD_BOTTOM_FORM(0);
    ADD_LEFT_WIDGET(acceptButton, 4);
    applyButton = XmCreatePushButton(form, "applyButton", ARGS);
    ADD_CB(applyButton, XmNactivateCallback,
	   &FieldEditor::applyButtonCB, this);

    RESET_ARGS();
    ADD_ARG(XmNlabelString,	STRING("Revert"));
    ADD_ARG(XmNhighlightThickness,	0);
    ADD_TOP_FORM(0);
    ADD_BOTTOM_FORM(0);
    ADD_LEFT_WIDGET(applyButton, 4);
    revertButton = XmCreatePushButton(form, "revertButton", ARGS);
    ADD_CB(revertButton, XmNactivateCallback,
	   &FieldEditor::revertButtonCB, this);

    RESET_ARGS();
    ADD_ARG(XmNlabelString,	STRING("Cancel"));
    ADD_ARG(XmNhighlightThickness,	0);
    ADD_TOP_FORM(0);
    ADD_BOTTOM_FORM(0);
    ADD_LEFT_WIDGET(revertButton, 4);
    cancelButton = XmCreatePushButton(form, "cancelButton", ARGS);
    ADD_CB(cancelButton, XmNactivateCallback,
	   &FieldEditor::cancelButtonCB, this);

    // Create an "Override" toggle button to allow user to change
    // override flag for node
    RESET_ARGS();
    ADD_ARG(XmNset,			nodeToEdit->isOverride());
    ADD_ARG(XmNlabelString,		STRING("Override"));
    ADD_ARG(XmNhighlightThickness,	0);
    ADD_TOP_FORM(0);
    ADD_BOTTOM_FORM(0);
    ADD_RIGHT_FORM(0);
    overrideButton = XmCreateToggleButtonGadget(form, "overrideButton", ARGS);

    XtManageChild(acceptButton);
    XtManageChild(applyButton);
    XtManageChild(revertButton);
    XtManageChild(cancelButton);
    XtManageChild(overrideButton);

    // Hitting a carriage-return activates the "Apply" button
    RESET_ARGS();
    ADD_ARG(XmNdefaultButton,		applyButton);
    XtSetValues(parent, ARGS);

    return form;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Applies changes made in editor to node it is editing.
//

void
FieldEditor::apply()
//
////////////////////////////////////////////////////////////////////////
{
    FieldInfo	*info;
    int		i;
    SbBool	override;

    // Turn off sensor for a minute...
    nodeSensor->detach();

    // Get all field values and flags and store them back into the node
    for (i = 0, info = fieldInfos; i < numFields; i++, info++)
	updateNode(info);

    // Set the override flag if it has changed
    override = XmToggleButtonGadgetGetState(overrideButton);
    if (nodeToEdit->isOverride() != override)
	nodeToEdit->setOverride(override);

    // Make sure the text window reflects the exact state
    revert();

    // Reattach sensor
    nodeSensor->attach(nodeToEdit);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets field editor based on the values in the node being edited.
//    If the passed field index is not -1 (the default), only the
//    value for the specified field is reverted.
//

void
FieldEditor::revert(int fieldIndex)
//
////////////////////////////////////////////////////////////////////////
{
    FieldInfo	*info;
    int		i;

    if (fieldIndex >= 0)
	updateInfo(&fieldInfos[fieldIndex]);

    // Update all field info from node
    else
	for (i = 0, info = fieldInfos; i < numFields; i++, info++)
	    updateInfo(info);

    // Set the override button
    XmToggleButtonGadgetSetState(overrideButton,
				 nodeToEdit->isOverride(), FALSE);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Initializes given FieldInfo structure based on indexed field.
//

void
FieldEditor::initInfo(FieldInfo *info, int i)
//
////////////////////////////////////////////////////////////////////////
{
    info->editor	= this;
    info->index		= i;
    info->setToDefault	= FALSE;

    // Set the field name and pointer and determine whether it is a
    // single or multiple value field.
    const SoFieldData	*fdata = nodeToEdit->getFieldData();

    info->field      = fdata->getField(nodeToEdit, i);
    info->fieldName  = fdata->getFieldName(i);
    info->isMultiple = info->field->isOfType(SoMField::getClassTypeId());
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets up field editor from correct field in node being edited.
//

void
FieldEditor::updateInfo(FieldInfo *info)
//
////////////////////////////////////////////////////////////////////////
{
    SbString	fieldString;
    ARG_VARS(10);

    // Set text field from current field value(s)
    showFieldString(info);

    // Set ignore button state
    RESET_ARGS();
    ADD_ARG(XmNset, info->field->isIgnored());
    XtSetValues(info->ignoreButton, ARGS);

    // Turn set-default button on or off
    info->setToDefault = info->field->isDefault();
    RESET_ARGS();
    ADD_ARG(XmNsensitive, ! info->setToDefault);
    XtSetValues(info->defaultButton, ARGS);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Makes field editor show default value for given field. Does NOT
//    change the node at all!
//

void
FieldEditor::showDefaultValue(FieldInfo *info)
//
////////////////////////////////////////////////////////////////////////
{
    SoField		*defField, *saveField;
    ARG_VARS(10);

    // Make sure we know that this was done
    info->setToDefault = TRUE;

    // Get the appropriate field from a node instance with all default values
    defField = defNode->getFieldData()->getField(defNode, info->index);

    // Temporarily change the field to the default one
    saveField = info->field;
    info->field = defField;

    // Set the string in the text widget
    showFieldString(info);

    // Restore the real field value
    info->field = saveField;

    // Disable the default button
    RESET_ARGS();
    ADD_ARG(XmNsensitive, FALSE);
    XtSetValues(info->defaultButton, ARGS);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets string(s) in text widget to display field value(s).
//

void
FieldEditor::showFieldString(FieldInfo *info) const
//
////////////////////////////////////////////////////////////////////////
{
    SbString	valueString;

    REM_CB(info->textWidget, XmNmodifyVerifyCallback,
	   &FieldEditor::textChangedCB, info);

    // Get the field value(s) as a string
    getFieldString(info, valueString);

    // Set the text widget to show the string
    XmTextSetString(info->textWidget, (char *) valueString.getString());

    // Set up the line number widget for multiple-value fields
    if (info->isMultiple)
	updateLineNumbers(info, TRUE);

    // Reconnect the callback
    ADD_CB(info->textWidget, XmNmodifyVerifyCallback,
	   &FieldEditor::textChangedCB, info);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Updates field value and flags in node from current editor settings.
//

void
FieldEditor::updateNode(FieldInfo *info)
//
////////////////////////////////////////////////////////////////////////
{
    SbString	string;
    SbBool	ignore;

    getEditorString(info, string);

    // Set the read-error callback so that read errors caused by the
    // following call to SoField::set() can be trapped and displayed
    // in an error dialog box
    SoReadError::setHandlerCallback(&FieldEditor::readErrorCB, info);

    if (! nodeToEdit->set(string.getString()))
	displayReadError();

    // If value was set to default, set flag in the field
    if (info->setToDefault)
	info->field->setDefault(TRUE);

    // Make sure ignore flag is correct
    ignore = XmToggleButtonGadgetGetState(info->ignoreButton);
    if (info->field->isIgnored() != ignore)
	info->field->setIgnored(ignore);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Fills in SbString with string representing field value that can
//    be displayed in editor's text widget.
//

void
FieldEditor::getFieldString(FieldInfo *info, SbString &string) const
//
////////////////////////////////////////////////////////////////////////
{
    // Get the field value(s) as a string
    info->field->get(string);

    // If it's a multiple-value string, we have to some extra work
    if (info->isMultiple) {

	const char *s = string.getString();

	// If first character is an open bracket, we have work to do
	if (*s == '[') {

	    // Skip over the open brace
	    s++;

	    // Allocate working space for the string
	    char *buf = new char [strlen(s) + 1];

	    char	*b = buf;
	    SbBool	atNewline = TRUE;

	    while (*s != '\0') {

		// Skip over quoted strings - this will have a problem
		// with quotes nested inside quotes ???
		if (*s == '\"') {
		    *b++ = *s++;
		    while (*s != '\"')
			*b++ = *s++;
		    *b++ = *s++;	// Closing quote
		}

		// Change commas into newlines
		if (*s == ',') {
		    *b++ = '\n';
		    atNewline = TRUE;
		    s++;
		}

		// Skip over spaces (including extra newlines) after newlines
		else if (atNewline && isspace(*s))
		    s++;

		else {
		    *b++ = *s++;
		    atNewline = FALSE;
		}
	    }

	    // Remove closing brace and space before it
	    while (*b != ']')
		b--;
	    while (isspace(*(b - 1)))
		b--;

	    // Terminate string
	    *b = '\0';

	    string = buf;

	    delete [] buf;
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Fills in SbString with string containing current field value in
//    editor's text widget, to store back into node being edited.
//

void
FieldEditor::getEditorString(FieldInfo *info, SbString &string) const
//
////////////////////////////////////////////////////////////////////////
{
    char 	*str = XmTextGetString(info->textWidget);

    // Store name of string and a space
    string = info->fieldName.getString();
    string += " ";

    // If a single-value field, just append text string from widget
    if (! info->isMultiple)
	string += str;

    // If this is a multiple-value field, add extra characters
    else {
	char	*c;

	string += "[";

	for (c = str; *c != '\0'; c++) {

	    // Change all newlines to commas in string first
	    if (*c == '\n') {
		*c = ',';

		// Remove all whitespace after commas. This also gets
		// rid of blank lines.
		c++;
		while (isspace(*c))
		    c++;

		c--;
	    }
	}
	string += str;

	string += "]";
    }

    XtFree(str);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Redefines this method to clean up when the window is destroyed.
//

void
FieldEditor::windowCloseAction()
//
////////////////////////////////////////////////////////////////////////
{
    // Alert caller if necessary
    if (finishCB != NULL)
	finishCB(finishData, this);

    delete this;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Updates the line-number widget whenever the text area changes or
//    is scrolled. The textChanged flag indicates why the line number
//    may be changing.
//

void
FieldEditor::updateLineNumbers(FieldInfo *info, SbBool textChanged) const
//
////////////////////////////////////////////////////////////////////////
{
    if (textChanged) {

	// See if the number of lines in the text area has changed. If
	// so, insert or remove line numbers from the widget.
	
	// Count newlines in the text string
	char	*textString = XmTextGetString(info->textWidget);
	int	numLines = 0;
	char	*b;
	for (b = textString; *b != '\0'; b++)
	    if (*b == '\n')
		numLines++;
	// Last line may not end in a newline?
	if (b > textString && *(b-1) != '\n')
	    numLines++;
	XtFree(textString);
	
	// Make sure line number widget has the correct info
	char	*numberString = XmTextGetString(info->numberWidget);
	int	numNumbers = strlen(numberString) / 6;
	XtFree(numberString);
	
	// Too many line numbers? Remove some.
	if (numNumbers > numLines)
	    XmTextReplace(info->numberWidget,
			  numLines * 6, numNumbers * 6, NULL);
	
	// Too few line numbers? Add some.
	else if (numNumbers < numLines) {
	    int		numNeeded = numLines - numNumbers, i;
	    char	*buf = new char [numNeeded * 6 + 1], *b = buf;
	    for (i = 0; i < numNeeded; i++) {
		sprintf(b, "%5d\n", numNumbers + i);
		b += strlen(b);
	    }
	    if (numNumbers == 0)
		XmTextReplace(info->numberWidget, 0, 1, buf);
	    else
		XmTextInsert(info->numberWidget, numLines * 6, buf);
	    delete [] buf;
	}
    }

    // Find out the scroll bar value and set the line number position
    // based on it
    ARG_VARS(1);
    int	topLine;

    RESET_ARGS();
    ADD_ARG(XmNvalue, &topLine);
    XtGetValues(info->scrollWidget, ARGS);

    RESET_ARGS();
    ADD_ARG(XmNtopCharacter, topLine * 6);
    XtSetValues(info->numberWidget, ARGS);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback called when the node we are editing changes. The data
//    pointer points to a FieldEditor instance.
//

void
FieldEditor::nodeSensorCB(void *data, SoSensor *sensor)
//
////////////////////////////////////////////////////////////////////////
{
    // Try to update only the field that changed, if there is one

    FieldEditor		*editor     = (FieldEditor *) data;
    SoNodeSensor	*nodeSensor = (SoNodeSensor *) sensor;
    const SoField	*trigField  = nodeSensor->getTriggerField();
    int			fieldIndex = -1;

    if (trigField != NULL) {
	int	i;

	for (i = 0; i < editor->numFields; i++) {
	    if (editor->fieldInfos[i].field == trigField) {
		fieldIndex = i;
		break;
	    }
	}
    }

    editor->revert(fieldIndex);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback called when the "Accept" button in the editor is
//    activated. The clientData pointer points to a FieldEditor instance.
//

void
FieldEditor::acceptButtonCB(Widget, XtPointer clientData, XtPointer)
//
////////////////////////////////////////////////////////////////////////
{
    FieldEditor	*editor = (FieldEditor *) clientData;

    // Make sure latest changes were applied
    editor->apply();

    // Get rid of window
    editor->windowCloseAction();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback called when the "Apply" button in the editor is
//    activated. The clientData pointer points to a FieldEditor instance.
//

void
FieldEditor::applyButtonCB(Widget, XtPointer clientData, XtPointer)
//
////////////////////////////////////////////////////////////////////////
{
    ((FieldEditor *) clientData)->apply();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback called when the "Revert" button in the editor is
//    activated. The clientData pointer points to a FieldEditor instance.
//

void
FieldEditor::revertButtonCB(Widget, XtPointer clientData, XtPointer)
//
////////////////////////////////////////////////////////////////////////
{
    ((FieldEditor *) clientData)->revert();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback called when the "Cancel" button in the editor is
//    activated. The clientData pointer points to a FieldEditor instance.
//

void
FieldEditor::cancelButtonCB(Widget, XtPointer clientData, XtPointer)
//
////////////////////////////////////////////////////////////////////////
{
    FieldEditor	*editor = (FieldEditor *) clientData;

    // Get rid of window
    editor->windowCloseAction();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback called when the "Set Default" button in a field editor
//    is activated. The clientData pointer points to a FieldInfo
//    instance for the field to change.
//

void
FieldEditor::defaultButtonCB(Widget, XtPointer clientData, XtPointer)
//
////////////////////////////////////////////////////////////////////////
{
    FieldInfo	*info = (FieldInfo *) clientData;

    // Make sure the correct value is displayed in the text widget
    info->editor->showDefaultValue(info);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback called when the text in a field editor text widget
//    is changed. The clientData pointer points to a FieldInfo
//    instance.
//

void
FieldEditor::textChangedCB(Widget, XtPointer clientData, XtPointer callData)
//
////////////////////////////////////////////////////////////////////////
{
    FieldInfo		*info = (FieldInfo *) clientData;
    XmTextVerifyPtr	tvp = (XmTextVerifyPtr) callData;

    // If we are inserting a text character, see if it is a carriage return
    if (tvp->text->length > 0 && *tvp->text->ptr == '\n') {

	// We don't want the text to be inserted if it's a
	// single-value field - just act as if we hit the "Apply"
	// button
	if (! info->isMultiple) {
	    tvp->doit = FALSE;
	    XtCallActionProc(info->editor->applyButton, "ArmAndActivate",
			     tvp->event, NULL, 0);
	}
    }

    // Multiple-value fields need to scroll correctly when the text
    // changes
    if (info->isMultiple)
	info->editor->updateLineNumbers(info, TRUE);

    // Reset the setToDefault flag to FALSE if necessary
    if (info->setToDefault && tvp->doit) {
	XtSetSensitive(info->defaultButton, TRUE);
	info->setToDefault = FALSE;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback called when the value of the scrollbar in a
//    multiple-value field text scrolling area changes. The clientData
//    pointer points to a FieldInfo instance.
//

void
FieldEditor::scrollCB(Widget, XtPointer clientData, XtPointer)
//
////////////////////////////////////////////////////////////////////////
{
    FieldInfo	*info = (FieldInfo *) clientData;

    // Determine the new top line number from the scroll bar
    info->editor->updateLineNumbers(info, FALSE);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Callback used to keep the user from moving the insertion cursor
//    within the line-number widget. The clientData pointer points to
//    a FieldInfo instance.
//

void
FieldEditor::lineCB(Widget, XtPointer, XtPointer callData)
//
////////////////////////////////////////////////////////////////////////
{
    XmTextVerifyCallbackStruct *cbInfo =
	(XmTextVerifyCallbackStruct *) callData;

    // Indicate that no cursor motion should occur
    cbInfo->doit = False;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Reallocates fieldBuf and sets fieldBufSize.
//

void *
FieldEditor::reallocBuf(void *ptr, size_t newSize)
//
////////////////////////////////////////////////////////////////////////
{
    fieldBufSize = newSize;

    fieldBuf = (char *) realloc(ptr, newSize);

    return fieldBuf;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Handler for read errors that occur when setting values in the
//    field. Saves the error message to be displayed later.
//

void
FieldEditor::readErrorCB(const SoError *error, void *data)
//
////////////////////////////////////////////////////////////////////////
{
    FieldInfo	*info = (FieldInfo *) data;

    // Save only the first error that is found
    if (info->editor->errorString == NULL) {

	SbString *err = new SbString;

	(*err) = "There was an error reading the values for the field named ";
	(*err) += info->fieldName.getString();
	(*err) += ":\n\n";
	(*err) += error->getDebugString();

	info->editor->errorString = err;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Displays error message detected during reading.
//

void
FieldEditor::displayReadError()
//
////////////////////////////////////////////////////////////////////////
{
    if (errorString != NULL) {
	Error *err = new Error(XtParent(getWidget()),
			       errorString->getString());
	delete errorString;

	errorString = NULL;
    }
}
