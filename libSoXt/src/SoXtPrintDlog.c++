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
 |   $Revision: 1.4 $
 |
 |   Classes:
 |      SoXtPrintDialog
 |
 |   Author(s): Dave Immel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */
 
#include <Inventor/SbPList.h>
#include <Inventor/SoOffscreenRenderer.h>
#include <Inventor/SoOutput.h>
#include <Inventor/SoPath.h>
#include <Inventor/Xt/SoXt.h>	
#include <Inventor/Xt/SoXtPrintDialog.h>	
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/Xt/SoXtResource.h>

#include <assert.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

#include <X11/StringDefs.h>
#include <Xm/BulletinB.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/List.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/SeparatoG.h>
#include <Xm/Text.h>
#include <Xm/ToggleB.h>
#include <GL/gl.h>


// Definitions for PostScript Output
#define INITIALXRES      288
#define INITIALYRES      378
#define FULLPAGEXSIZE	(8.5)
#define FULLPAGEYSIZE	(11.0)
#define PNTS		(72.27)
#define PAGEXSIZE	(8.0)
#define PAGEYSIZE	(10.5)
#define PAGEMARGIN	(0.25)
#define SO_PRINTER_STRLEN   32
#define INCHES_PER_MM       3.937008e-02
#define POINTS_PER_INCH     72.27


#define PRINTER_MENU_HEIGHT         330
#define RGB_FILE_MENU_HEIGHT        220
#define POSTSCRIPT_FILE_MENU_HEIGHT 275

#define PRINTER_NAME(INDEX) ((char *) ((*printers)[INDEX]))
#if 0
#define ADD_STYLE_BUTTON(KONST) \
    sButtons[KONST] = XmCreatePushButtonGadget (stylePopup, \
                    styleLabels[KONST], wargs, 1); \
    XtAddCallback (sButtons[KONST], XmNactivateCallback, \
                   (XtCallbackProc) SoXtPrintDialog::stylePopupCB, \
                   (XtPointer)(KONST));
#endif


// Resources for labels. +++
typedef struct {
	char *pageOutput;
	char *toPrinter;
	char *toFile;
	char *fileFormat;
	char *postScript;
	char *rgb;
	char *resolution;
	char *fileName;
	char *printQuality;
	char *high;
	char *draft;
	char *pageFormat;   
	char *portrait;	
	char *landscape;
	char *printSize;
	char *printer;
	char *dpi;
	char *message;
	char *quitB;
	char *printB;
	char *holzn1;
	char *holzn2;
	char *vertc1;
	char *by;
} RES_LABELS;
static RES_LABELS rl;
static char *defaults[]={ 
	"Page Output:",
	"To Printer",
	"To File",
	"File Format:",
	"PostScript",
	"RGB",
	"Resolution:",
	"File Name:",
	"Print Quality:",
	"High",
	"Draft",
        "Page Format:",
	"Portrait",
	"Landscape",
	"Print Size (inches):",
	"Printer:",
	"DPI:",
	"Message:",
	"Quit",
	"Print",
	"7",
	"5",
	"7",
	"by"
};


// ??? COMPAT 2.0 
//
// This addition is being made to enable the programmer to pass in an
// SoGLRenderAction to be used when rendering.  To insure binary
// compatibility, a static list of structures will be kept to record
// the instances of this class with the print dialogs that are
// set with them.
//

class _SoPrintDialogItem {
  public:
    _SoPrintDialogItem(SoGLRenderAction *glAct, SoXtPrintDialog *p)
        { act = glAct; printDialog = p;}
    SoGLRenderAction *act;       // The render action used with this instance
    SoXtPrintDialog *printDialog;// The instance of the print dialog
};

// The list of print dialogs and their render actions
static SbPList *printDialogList = NULL;

////////////////////////////////////////////////////////////////////////
//
// Description:
// ??? COMPAT 2.0
//    Find the instance of the _SoPrintDialogItem class that corresponds
//    to this instance of the SoXtPrintDialog class.
//
// Use: internal

static _SoPrintDialogItem *_SoFindPrintDialog(const SoXtPrintDialog *dialog)

//
////////////////////////////////////////////////////////////////////////
{
    // If no class instances have been stored, return NULL
    if (printDialogList == NULL)
        return NULL;

    for (int i=0; i<printDialogList->getLength(); i++) {

        _SoPrintDialogItem *item = (_SoPrintDialogItem *)(*printDialogList)[i];
        if (item->printDialog == dialog)
            return item;
    }

    // A render action has not been set with this instance.  Return NULL.
    return NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Public constructor - build the widget right now
//
SoXtPrintDialog::SoXtPrintDialog(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent)
	: SoXtComponent(
	    parent,
	    name, 
	    buildInsideParent)
//
////////////////////////////////////////////////////////////////////////
{
    // In this case, print dialog is what the app wants, so buildNow = TRUE
    constructorCommon(TRUE);
}

////////////////////////////////////////////////////////////////////////
//
// SoEXTENDER constructor - the subclass tells us whether to build or not
//
SoXtPrintDialog::SoXtPrintDialog(
    Widget parent,
    const char *name, 
    SbBool buildInsideParent, 
    SbBool buildNow)
	: SoXtComponent(
	    parent,
	    name, 
	    buildInsideParent)
//
////////////////////////////////////////////////////////////////////////
{
    // In this case, print dialog may be what the app wants, 
    // or it may want a subclass of print dialog. Pass along buildNow
    // as it was passed to us.
    constructorCommon(buildNow);
}

////////////////////////////////////////////////////////////////////////
//
// Called by the constructors
//
// private
//
void
SoXtPrintDialog::constructorCommon(SbBool buildNow)
//
//////////////////////////////////////////////////////////////////////
{
    printDone      = TRUE;
    highQuality    = TRUE;
    portraitFormat = TRUE;
    printerOutput  = TRUE;
    postScriptOutput    = TRUE;
    nodeMostRecent = TRUE;
    rootNode       = NULL;
    rootPath       = NULL;
    defaultPrinter = NULL;
    printerHorizSize    = NULL;
    printerVertSize     = NULL;
    postScriptHorizSize = NULL;
    postScriptVertSize  = NULL;
    rgbHorizSize   = NULL;
    rgbVertSize    = NULL;
    alreadyUpdated = FALSE;
    printers       = new SbPList;
    printRes.setValue(INITIALXRES, INITIALYRES);
    printSize.setValue(printRes[0]/SoOffscreenRenderer::getScreenPixelsPerInch(),
                       printRes[1]/SoOffscreenRenderer::getScreenPixelsPerInch());

    //
    // Create the list of available printers.
    //
    getPrinterList();
    
    // Build the widget tree, and let SoXtComponent know about our base widget.
    if (buildNow) {
	Widget w = buildWidget(getParentWidget());
	setBaseWidget(w);
    }
}

///////////////////////////////////////////////////////////////////////
//
// Description:
//	Destructor.
//
// Use: public

SoXtPrintDialog::~SoXtPrintDialog()

//
//////////////////////////////////////////////////////////////////////
{
    // ??? COMPAT 2.0
    //
    // Get the print dialog item from the list and delete the render action.
    // Then, remove the item from the list and delete it.
    //
    _SoPrintDialogItem *item = _SoFindPrintDialog(this);
    if (item != NULL) {
        delete item->act;
        printDialogList->remove(printDialogList->find(item));
        delete item;
    }

    for (int i = 0; i < printers->getLength(); i++) {
	if (PRINTER_NAME(i) != NULL) free(PRINTER_NAME(i));
    }
    delete printers;
    if (defaultPrinter != NULL) free(defaultPrinter);
    
    if (rootNode != NULL)
    	rootNode->unref();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Get the render action.
//
// Use: public

SoGLRenderAction *
SoXtPrintDialog::getGLRenderAction()


//
////////////////////////////////////////////////////////////////////////
{
    // ??? COMPAT 2.0
    //
    // Find the list item and return the action associated with it.
    // If there is none, allocate one and put it on the list.  Return
    // the new action.
    //
    _SoPrintDialogItem *printItem = _SoFindPrintDialog(this);
    if (printItem == NULL) {
        SbViewportRegion tmpRegion(100, 100);
        SoGLRenderAction *tmpAct = new SoGLRenderAction(tmpRegion);
        printItem = new _SoPrintDialogItem(tmpAct, this);

        // If the printDialogList is NULL, allocate one and add the new
        // item to it.
        if (printDialogList == NULL) {
            printDialogList = new SbPList(4);
        }
        printDialogList->append(printItem);
    }

    return (printItem->act);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Set the render action.
//
// Use: public

void
SoXtPrintDialog::setGLRenderAction(
    const SoGLRenderAction *act )


//
////////////////////////////////////////////////////////////////////////
{
    // ??? COMPAT 2.0
    //
    // Find the list item and load the new render action into it.
    // If there is currently no render action associated with this
    // instance, create one.  Store the new action in the item.
    //
    _SoPrintDialogItem *item = _SoFindPrintDialog(this);

    if (item == NULL) {
        item = new _SoPrintDialogItem((SoGLRenderAction *)act, this);

        // If the printDialogList is NULL, allocate one and add the new
        // item to it.
        if (printDialogList == NULL) {
            printDialogList = new SbPList(4);
        }
        printDialogList->append(item);
    }

    item->act = (SoGLRenderAction *)act;
}

///////////////////////////////////////////////////////////////////////
//
// Description:
//	Set the root of the scene graph to print.
//
// Use: public
//
void
SoXtPrintDialog::setSceneGraph( SoNode *root )
//
//////////////////////////////////////////////////////////////////////
{
    if ( root != NULL )
	root->ref();

    if (rootNode != NULL)
    	rootNode->unref();
	
    rootNode = root;
    
    if (rootNode != NULL)
        nodeMostRecent = TRUE;
}

///////////////////////////////////////////////////////////////////////
//
// Description:
//	Set the root path of the scene graph to print.
//
// Use: public
//
void
SoXtPrintDialog::setSceneGraph( SoPath *root )
//
//////////////////////////////////////////////////////////////////////
{
    if ( root != NULL )
	root->ref();

    if (rootPath != NULL)
    	rootPath->unref();
	
    rootPath = root;
    
    if (rootPath != NULL)
        nodeMostRecent = FALSE;
}

///////////////////////////////////////////////////////////////////////
//
// Description:
//	Set the size of the print
//
// Use: public
//
void
SoXtPrintDialog::setPrintSize( const SbVec2f &psize )
//
//////////////////////////////////////////////////////////////////////
{
    printSize = psize;
    printRes.setValue(
            (short)(printSize[0]*SoOffscreenRenderer::getScreenPixelsPerInch()),
            (short)(printSize[1]*SoOffscreenRenderer::getScreenPixelsPerInch()));

    updateTextports();
}

///////////////////////////////////////////////////////////////////////
//
// Description:
//	Set the size of the print in pixels
//
// Use: public
//
void
SoXtPrintDialog::setPrintSize( const SbVec2s &psize )
//
//////////////////////////////////////////////////////////////////////
{
    printRes = psize;
    printSize.setValue(
            printRes[0]/SoOffscreenRenderer::getScreenPixelsPerInch(),
            printRes[1]/SoOffscreenRenderer::getScreenPixelsPerInch());

    updateTextports();
}

///////////////////////////////////////////////////////////////////////
//
// Description:
//	This builds the widget.
//
// Use: public

Widget
SoXtPrintDialog::buildWidget(Widget parent)

//
//////////////////////////////////////////////////////////////////////
{
    XmString printMsg;
    Arg      wargs[16];
    int      n;

    //
    // "widget" is the main widget for the print widget.  labels and their
    // associated radio buttons, a label and scrollList widget for selecting
    // a printer, a label and text widget for typing a filename, quit and
    // print buttons, and a message text widget.
    //
    n = 0;
    Widget widget = XtCreateWidget(getWidgetName(), xmBulletinBoardWidgetClass, 
                                        parent, NULL, 0);
    XtSetArg( wargs[n], XmNmarginWidth,  10 ); n++;
    XtSetValues( widget, wargs, n );


    // get resources...
       SoXtResource xr(widget);
       if (!xr.getResource( "pageOutput", "PageOutput", rl.pageOutput ))
	   rl.pageOutput = defaults[0];
       if (!xr.getResource( "toPrinter", "ToPrinter", rl.toPrinter ))
	   rl.toPrinter = defaults[1];
       if (!xr.getResource( "toFile", "ToFile", rl.toFile ))
	   rl.toFile = defaults[2];
       if (!xr.getResource( "fileFormat", "FileFormat", rl.fileFormat ))
	   rl.fileFormat = defaults[3];
       if (!xr.getResource( "postScript", "PostScript", rl.postScript ))
	   rl.postScript = defaults[4];
       if (!xr.getResource( "rgb", "RGB", rl.rgb ))
	   rl.rgb = defaults[5];
       if (!xr.getResource( "resolution", "Resolution", rl.resolution ))
	   rl.resolution = defaults[6];
       if (!xr.getResource( "fileName", "FileName", rl.fileName ))
	   rl.fileName = defaults[7];
       if (!xr.getResource( "printQuality", "PrintQuality", rl.printQuality ))
	   rl.printQuality = defaults[8];
       if (!xr.getResource( "high", "High", rl.high ))
	   rl.high = defaults[9];
       if (!xr.getResource( "draft", "Draft", rl.draft ))
	   rl.draft = defaults[10];
       if (!xr.getResource( "pageFormat", "PageFormat", rl.pageFormat ))
	   rl.pageFormat = defaults[11];
       if (!xr.getResource( "portrait", "Portrait", rl.portrait ))
	   rl.portrait = defaults[12];
       if (!xr.getResource( "landscape", "Landscape", rl.landscape ))
	   rl.landscape = defaults[13];
       if (!xr.getResource( "printSize", "PrintSize", rl.printSize ))
	   rl.printSize = defaults[14];
       if (!xr.getResource( "printer", "Printer", rl.printer ))
	   rl.printer = defaults[15];
       if (!xr.getResource( "dpi", "DPI", rl.dpi ))
	   rl.dpi = defaults[16];
       if (!xr.getResource( "message", "Message", rl.message ))
	   rl.message = defaults[17];
       if (!xr.getResource( "quitB", "QuitB", rl.quitB ))
	   rl.quitB = defaults[18];
       if (!xr.getResource( "printB", "PrintB", rl.printB ))
	   rl.printB = defaults[19];
       if (!xr.getResource( "holznColumn1", "HolznColumn1", rl.holzn1 ))
	   rl.holzn1 = defaults[20];
       if (!xr.getResource( "holznColumn2", "HolznColumn2", rl.holzn2 ))
	   rl.holzn2 = defaults[21];
       if (!xr.getResource( "vertcColumn1", "VertcColumn1", rl.vertc1 ))
	   rl.vertc1 = defaults[22];
       if (!xr.getResource( "byLabel", "ByLabel", rl.by ))
	   rl.by = defaults[23];



    // Build the radio button for the Page Output.
    buildRadioButton( rl.pageOutput, rl.toPrinter, rl.toFile, 120, 5,
            widget, (XtCallbackProc) SoXtPrintDialog::outputCB);

    //
    // Create the Quit and Print buttons, and the message textport.  These
    // will change position as the menu changes.
    //
    n = 0;
    printMsg = XmStringCreate ( rl.quitB, XmSTRING_DEFAULT_CHARSET);
    XtSetArg( wargs[n], XmNx,            180 ); n++;
    XtSetArg( wargs[n], XmNy,            170 ); n++;
    XtSetArg( wargs[n], XmNwidth,        70 ); n++;
    XtSetArg( wargs[n], XmNheight,       30 ); n++;
    XtSetArg( wargs[n], XmNhighlightThickness, 0 ); n++;
    XtSetArg( wargs[n], XmNlabelString,  printMsg ); n++;
    quitButton = XtCreateManagedWidget (NULL, xmPushButtonWidgetClass,
                                        widget, wargs, n);
    XtAddCallback (quitButton, XmNactivateCallback, 
        (XtCallbackProc) SoXtPrintDialog::quitCB, (XtPointer)(this));
    XmStringFree(printMsg);

    n = 0;
    printMsg = XmStringCreate (rl.printB, XmSTRING_DEFAULT_CHARSET);
    XtSetArg( wargs[n], XmNx,            270 ); n++;
    XtSetArg( wargs[n], XmNy,            170 ); n++;
    XtSetArg( wargs[n], XmNwidth,        70 ); n++;
    XtSetArg( wargs[n], XmNheight,       30 ); n++;
    XtSetArg( wargs[n], XmNhighlightThickness, 0 ); n++;
    XtSetArg( wargs[n], XmNlabelString,  printMsg ); n++;
    printButton = XtCreateManagedWidget (NULL, xmPushButtonWidgetClass,
                                         widget, wargs, n);
    XtAddCallback (printButton, XmNactivateCallback, 
        (XtCallbackProc) SoXtPrintDialog::printCB, (XtPointer)(this));
    XmStringFree(printMsg);

    n = 0;
    printMsg = XmStringCreate( rl.message, XmSTRING_DEFAULT_CHARSET);
    XtSetArg( wargs[n], XmNy,            180 ); n++;
    XtSetArg( wargs[n], XmNlabelString,  printMsg ); n++;
    messageLabelWidget  = XtCreateManagedWidget (NULL, xmLabelWidgetClass,
                                        widget, wargs, n);
    n = 0;
    XtSetArg( wargs[n], XmNy,            210 ); n++;
    XtSetArg( wargs[n], XmNwidth,        330 ); n++;
    XtSetArg( wargs[n], XmNhighlightThickness, 0 ); n++;
    XtSetArg( wargs[n], XmNeditable,     FALSE ); n++;
    messageWidget = XtCreateManagedWidget (NULL, xmTextWidgetClass,
                                        widget, wargs, n);
    XmStringFree(printMsg);

    // Create the widget storing the file format radio buttons
    n = 0;
    fileFormatWidget = XtCreateWidget (NULL, xmBulletinBoardWidgetClass, 
                                   widget, NULL, 0);
    XtSetArg( wargs[n], XmNmarginHeight, 0 ); n++;
    XtSetArg( wargs[n], XmNmarginWidth,  0 ); n++;
    XtSetArg( wargs[n], XmNx, 0 ); n++;
    XtSetArg( wargs[n], XmNy, 40 ); n++;
    XtSetValues( fileFormatWidget, wargs, n );
    buildRadioButton( rl.fileFormat, rl.postScript, rl.rgb,
            110, 0, fileFormatWidget,
            (XtCallbackProc) SoXtPrintDialog::fileFormatCB);

    //
    //  Three subwidgets will contain file specific and printer specific UI.
    //  Each will be managed and unmanaged depending on the printerMenu flag.
    //
    buildToPrinterWidget( widget );
    buildToPostScriptFileWidget( widget );
    buildToRGBFileWidget( widget );
    setPrintSize(printSize);
    setPrintSize(printRes);
    XtSetArg( wargs[0], XmNx, 0 );
    XtSetArg( wargs[1], XmNy, 40 );
    XtSetValues( toPrinterWidget, wargs, 2 );
    XtSetArg( wargs[0], XmNx, 0 );
    XtSetArg( wargs[1], XmNy, 40 );
    XtSetValues( toPostScriptFileWidget, wargs, 2 );
    XtSetValues( toRGBFileWidget, wargs, 2 );
    if (printerOutput)
        XtManageChild( toPrinterWidget );
    else if (postScriptOutput)
        XtManageChild( toPostScriptFileWidget );
    else
        XtManageChild( toRGBFileWidget );

    //
    // Place the quit, print, and message textports
    //
    placeBottomOfDialog( this );

    return widget;
}



///////////////////////////////////////////////////////////////////////
//
// Description:
//	This builds the toPrinterWidget widget.
//
// Use: protected

void
SoXtPrintDialog::buildToPrinterWidget( Widget parent )

//
//////////////////////////////////////////////////////////////////////
{
    XmString labelMsg;
    Widget   list, label;
    Arg      wargs[16];
    int      i, n;

    toPrinterWidget = XtCreateManagedWidget (getWidgetName(),
            xmBulletinBoardWidgetClass, parent, NULL, 0);
    n = 0;
    XtSetArg( wargs[n], XmNmarginHeight, 0 ); n++;
    XtSetArg( wargs[n], XmNmarginWidth,  0 ); n++;
    XtSetValues( toPrinterWidget, wargs, n );

    // Build radio buttons for the Print Quality and Page Format
    buildRadioButton( rl.printQuality, rl.high, rl.draft, 110, 0,
            toPrinterWidget, (XtCallbackProc) SoXtPrintDialog::qualityCB);
    buildRadioButton( rl.pageFormat, rl.portrait, rl.landscape, 110, 30,
            toPrinterWidget, (XtCallbackProc) SoXtPrintDialog::pageFormatCB);

    // Build the fields for entering the Print Size
    buildSizeFields( rl.printSize, 60, toPrinterWidget,
            printerHorizSize, printerVertSize,
            (XtCallbackProc) SoXtPrintDialog::printerHorizSizeCB,
            (XtCallbackProc) SoXtPrintDialog::printerVertSizeCB );

    //
    // Create the list of printer names
    //
    n = 0;
    labelMsg = XmStringCreate ( rl.printer, XmSTRING_DEFAULT_CHARSET);
    XtSetArg( wargs[n], XmNy,            90 ); n++;
    XtSetArg( wargs[n], XmNlabelString,  labelMsg ); n++;
    label  = XtCreateManagedWidget (NULL, xmLabelWidgetClass,
                                 toPrinterWidget, wargs, n);
    XmStringFree(labelMsg);
    
    n = 0;
    XtSetArg( wargs[n], XmNx,            110 ); n++;
    XtSetArg( wargs[n], XmNy,            90 ); n++;
    XtSetArg( wargs[n], XmNwidth,        220 ); n++;
    XtSetArg( wargs[n], XmNheight,       100 ); n++;
    XtSetArg( wargs[n], XmNhighlightThickness, 0 ); n++;
    XtSetArg( wargs[n], XmNselectionPolicy, XmSINGLE_SELECT); n++;
    list = XmCreateScrolledList(toPrinterWidget, "printerList", wargs, n);
    XtAddCallback(list,
    	XmNsingleSelectionCallback,
	(XtCallbackProc) SoXtPrintDialog::listPick,
	(XtPointer) this);
    XtManageChild (list);
    
    // fill the list with printer names
    whichPrinter = -1;
    for (i = 0; i < printers->getLength(); i++)
    {
        XmString listItem = XmStringCreate (PRINTER_NAME(i),
                                            XmSTRING_DEFAULT_CHARSET);
        XmListAddItemUnselected (list, listItem, 0);
	XmStringFree(listItem); // XmList made a copy
	
	// if this is the default, then select it. (list count starts at 1)
        if (strcmp(PRINTER_NAME(i), defaultPrinter) == 0) {
            XmListSelectPos (list, i+1, FALSE);
	    whichPrinter = i;
	}
    }

    // Build the field for entering the DPI
    buildDPIField( 120, toPrinterWidget, printerDPIField,
            (XtCallbackProc) SoXtPrintDialog::printerDPICB );
}



///////////////////////////////////////////////////////////////////////
//
// Description:
//	This builds the toPostScriptFileWidget widget.
//
// Use: protected

void
SoXtPrintDialog::buildToPostScriptFileWidget( Widget parent )

//
//////////////////////////////////////////////////////////////////////
{
    XmString labelMsg;
    Widget   label;
    Arg      wargs[4];
    int      n;

    n = 0;
    toPostScriptFileWidget = XtCreateWidget (NULL, xmBulletinBoardWidgetClass, 
                                   parent, NULL, 0);
    XtSetArg( wargs[n], XmNmarginHeight, 0 ); n++;
    XtSetArg( wargs[n], XmNmarginWidth,  0 ); n++;
    XtSetValues( toPostScriptFileWidget, wargs, n );

    // Build radio buttons for the Print Quality
    buildRadioButton( rl.printQuality, rl.high, rl.draft, 110, 30,
            toPostScriptFileWidget,
            (XtCallbackProc) SoXtPrintDialog::qualityCB);

    // Build the fields for entering the Print Size
    buildSizeFields( rl.printSize, 60, toPostScriptFileWidget,
            postScriptHorizSize, postScriptVertSize,
            (XtCallbackProc) SoXtPrintDialog::postScriptHorizSizeCB,
            (XtCallbackProc) SoXtPrintDialog::postScriptVertSizeCB );

    //
    // Create the FileName label and textport.
    //
    n = 0;
    labelMsg = XmStringCreate (rl.fileName, XmSTRING_DEFAULT_CHARSET);
    XtSetArg( wargs[n], XmNy,            90 ); n++;
    XtSetArg( wargs[n], XmNlabelString,  labelMsg ); n++;
    label  = XtCreateManagedWidget (NULL, xmLabelWidgetClass,
                                        toPostScriptFileWidget, wargs, n);
    XmStringFree(labelMsg);

    n = 0;
    XtSetArg( wargs[n], XmNx,            100 ); n++;
    XtSetArg( wargs[n], XmNy,            90 ); n++;
    XtSetArg( wargs[n], XmNwidth,        230 ); n++;
    XtSetArg( wargs[n], XmNhighlightThickness, 0 ); n++;
    postScriptFilenameWidget = XtCreateManagedWidget (NULL, xmTextWidgetClass,
                                        toPostScriptFileWidget, wargs, n);
    // Build the field for entering the DPI
    buildDPIField( 130, toPostScriptFileWidget, postScriptDPIField,
            (XtCallbackProc) SoXtPrintDialog::postScriptDPICB );
}



///////////////////////////////////////////////////////////////////////
//
// Description:
//	This builds the toRGBFileWidget widget.
//
// Use: protected

void
SoXtPrintDialog::buildToRGBFileWidget( Widget parent )

//
//////////////////////////////////////////////////////////////////////
{
    XmString labelMsg;
    Widget   label;
    Arg      wargs[4];
    int      n;

    n = 0;
    toRGBFileWidget = XtCreateWidget (NULL, xmBulletinBoardWidgetClass, 
                                   parent, NULL, 0);
    XtSetArg( wargs[n], XmNmarginHeight, 0 ); n++;
    XtSetArg( wargs[n], XmNmarginWidth,  0 ); n++;
    XtSetValues( toRGBFileWidget, wargs, n );

    // Build radio buttons for the Print Quality
    buildRadioButton( rl.fileFormat, rl.postScript, rl.rgb, 110, 0,
            toRGBFileWidget,
            (XtCallbackProc) SoXtPrintDialog::fileFormatCB);

    // Build the fields for entering the Print Size
    buildSizeFields( rl.resolution, 30, toRGBFileWidget,
            rgbHorizSize, rgbVertSize,
            (XtCallbackProc) SoXtPrintDialog::rgbHorizSizeCB,
            (XtCallbackProc) SoXtPrintDialog::rgbVertSizeCB );

    // Initialize the horizontal and vertical size textports
    char tc[8];
    sprintf(tc, "%d", printRes[0]);
    XmTextSetString (rgbHorizSize, tc);
    sprintf(tc, "%d", printRes[1]);
    XmTextSetString (rgbVertSize, tc);

    // Create the FileName label and textport.
    n = 0;
    labelMsg = XmStringCreate (rl.fileName, XmSTRING_DEFAULT_CHARSET);
    XtSetArg( wargs[n], XmNy,            60 ); n++;
    XtSetArg( wargs[n], XmNlabelString,  labelMsg ); n++;
    label  = XtCreateManagedWidget (NULL, xmLabelWidgetClass,
                                        toRGBFileWidget, wargs, n);
    XmStringFree(labelMsg);

    n = 0;
    XtSetArg( wargs[n], XmNx,            100 ); n++;
    XtSetArg( wargs[n], XmNy,            60 ); n++;
    XtSetArg( wargs[n], XmNwidth,        230 ); n++;
    XtSetArg( wargs[n], XmNhighlightThickness, 0 ); n++;
    rgbFilenameWidget = XtCreateManagedWidget (NULL, xmTextWidgetClass,
                                        toRGBFileWidget, wargs, n);
}

///////////////////////////////////////////////////////////////////////
//
// Description:
//	This builds a Radio Button.
//
// Use: protected

void
SoXtPrintDialog::buildRadioButton(
    char *title, 
    char *label1,
    char *label2,
    int  horizontalPlacement,
    int  verticalPlacement,
    Widget parent,
    XtCallbackProc cb )

//
//////////////////////////////////////////////////////////////////////
{
    XmString labelMsg;
    Widget   buttons, b, label;
    Arg      wargs[4];
    int      n;

    //
    // Create the Radio Button.  It is a horizontal RowColumn widget
    // composed of a label widget and a RadioBox widget.
    //
    n = 0;
    labelMsg = XmStringCreate( title, XmSTRING_DEFAULT_CHARSET);
    XtSetArg( wargs[n], XmNx,            0 ); n++;
    XtSetArg( wargs[n], XmNy,            verticalPlacement ); n++;
    XtSetArg( wargs[n], XmNlabelString,  labelMsg ); n++;
    label  = XtCreateManagedWidget (NULL, xmLabelWidgetClass,
                                        parent, wargs, n);
    XmStringFree(labelMsg);

    n = 0;
    XtSetArg( wargs[n], XmNx,            horizontalPlacement ); n++;
    XtSetArg( wargs[n], XmNy,            verticalPlacement ); n++;
    XtSetArg( wargs[n], XmNorientation,  XmHORIZONTAL ); n++;
    XtSetArg( wargs[n], XmNpacking,      XmPACK_NONE ); n++;
    buttons = XmCreateRadioBox(parent, "radioToggles", wargs, n);
    XtManageChild (buttons);

    n = 0;
    labelMsg = XmStringCreate (label1, XmSTRING_DEFAULT_CHARSET);
    XtSetArg( wargs[n], XmNset,  TRUE ); n++;
    XtSetArg( wargs[n], XmNx,            10 ); n++;
    XtSetArg( wargs[n], XmNlabelString,  labelMsg ); n++;
    XtSetArg( wargs[n], XmNhighlightThickness, 0 ); n++;
    b = XtCreateManagedWidget("radioButton1", xmToggleButtonWidgetClass,
                              buttons, wargs, n);
    XtAddCallback (b, XmNvalueChangedCallback, cb, (XtPointer)(this));
    XmStringFree(labelMsg);

    n = 0;
    labelMsg = XmStringCreate (label2, XmSTRING_DEFAULT_CHARSET);
    XtSetArg( wargs[n], XmNx,            100 ); n++;
    XtSetArg( wargs[n], XmNlabelString,  labelMsg ); n++;
    XtSetArg( wargs[n], XmNhighlightThickness, 0 ); n++;
    b = XtCreateManagedWidget("radioButton2", xmToggleButtonWidgetClass,
                       buttons, wargs, n);
    XmStringFree(labelMsg);
}
 
///////////////////////////////////////////////////////////////////////
//
// Description:
//	This builds a set of two text fields for entering horizontal
//      and vertical sizes.
//
// Use: protected

void
SoXtPrintDialog::buildSizeFields(
    char *title, 
    int  verticalPlacement,
    Widget parent,
    Widget &horizField,
    Widget &vertField,
    XtCallbackProc cb1,
    XtCallbackProc cb2 )

//
//////////////////////////////////////////////////////////////////////
{
    XmString labelMsg;
    Widget   label;
    Arg      wargs[4];
    int      n;

    //
    // Create the Print Size row.  It is a label with two text widgets
    // for the horizontal and vertical sizes of the print.
    //
    n = 0;
    labelMsg = XmStringCreate (title, XmSTRING_DEFAULT_CHARSET);
    XtSetArg( wargs[n], XmNy,            verticalPlacement ); n++;
    XtSetArg( wargs[n], XmNlabelString,  labelMsg ); n++;
    label  = XtCreateManagedWidget (NULL, xmLabelWidgetClass,
            parent, wargs, n);
    XmStringFree(labelMsg);

    n = 0;
    XtSetArg( wargs[n], XmNx,            150 ); n++;
    XtSetArg( wargs[n], XmNy,            verticalPlacement-3 ); n++;
    XtSetArg( wargs[n], XmNhighlightThickness, 1); n++;
    XtSetArg( wargs[n], XmNcolumns, atoi(rl.holzn1)); n++;
    horizField = XtCreateManagedWidget("horizontalSize", xmTextWidgetClass,
            parent, wargs, n);
    if (cb1 != NULL)
        XtAddCallback(horizField, XmNactivateCallback, cb1, (XtPointer) this);

    n = 0;
    labelMsg = XmStringCreate (rl.by, XmSTRING_DEFAULT_CHARSET);
    XtSetArg( wargs[n], XmNx,            226 ); n++;
    XtSetArg( wargs[n], XmNy,            verticalPlacement ); n++;
    XtSetArg( wargs[n], XmNlabelString,  labelMsg ); n++;
    label  = XtCreateManagedWidget (NULL, xmLabelWidgetClass,
                                 parent, wargs, n);
    XmStringFree(labelMsg);

    n = 0;
    XtSetArg( wargs[n], XmNx,            258 ); n++;
    XtSetArg( wargs[n], XmNy,            verticalPlacement-3 ); n++;
    XtSetArg( wargs[n], XmNhighlightThickness, 1); n++;
    XtSetArg( wargs[n], XmNcolumns, atoi(rl.vertc1)); n++;
    vertField = XtCreateManagedWidget("verticalSize", xmTextWidgetClass,
            parent, wargs, n);
    if (cb2 != NULL)
        XtAddCallback(vertField, XmNactivateCallback, cb2, (XtPointer) this);
}    
 
///////////////////////////////////////////////////////////////////////
//
// Description:
//	This builds a field containing the DPI of the desired print.
//
// Use: protected

void
SoXtPrintDialog::buildDPIField(
    int  verticalPlacement,
    Widget parent,
    Widget &dpiField,
    XtCallbackProc cb )

//
///////////////////////////////////////////////////////////////////////
{
    XmString labelMsg;
    Widget   label;
    Arg      wargs[4];
    int      n;

    n = 0;
    labelMsg = XmStringCreate ( rl.dpi, XmSTRING_DEFAULT_CHARSET);
    XtSetArg( wargs[n], XmNy,            verticalPlacement ); n++;
    XtSetArg( wargs[n], XmNlabelString,  labelMsg ); n++;
    label  = XtCreateManagedWidget (NULL, xmLabelWidgetClass,
            parent, wargs, n);
    XmStringFree(labelMsg);

    n = 0;
    XtSetArg( wargs[n], XmNx,            40 ); n++;
    XtSetArg( wargs[n], XmNy,            verticalPlacement-3 ); n++;
    XtSetArg( wargs[n], XmNhighlightThickness, 1); n++;
    XtSetArg( wargs[n], XmNcolumns, atoi(rl.holzn2)); n++;
    dpiField = XtCreateManagedWidget("horizontalSize", xmTextWidgetClass,
            parent, wargs, n);
    XtAddCallback(dpiField, XmNactivateCallback, cb, (XtPointer) this);
    XmTextSetString (dpiField, "100");
}

///////////////////////////////////////////////////////////////////////
//
// Description:
//	This places the quit and text buttons and the message textport
//      depending on whether the toFileWidget or toPrinterWidget is
//      currently being displayed.
//
// Use: protected

void
SoXtPrintDialog::placeBottomOfDialog(
    SoXtPrintDialog *ptr )

//
//////////////////////////////////////////////////////////////////////
{
    Arg      wargs[1];

    if (ptr->printerOutput)
    {
        XtSetArg( wargs[0], XmNy,        240 );
        XtSetValues( ptr->quitButton, wargs, 1 );
        XtSetValues( ptr->printButton, wargs, 1 );

        XtSetArg( wargs[0], XmNy,        260 );
        XtSetValues( ptr->messageLabelWidget, wargs, 1 );
        XtSetArg( wargs[0], XmNy,        280 );
        XtSetValues( ptr->messageWidget, wargs, 1 );
    }
    else if (ptr->postScriptOutput)
    {
        XtSetArg( wargs[0], XmNy,        205 );
        XtSetValues( ptr->messageLabelWidget, wargs, 1 );
        XtSetArg( wargs[0], XmNy,        225 );
        XtSetValues( ptr->messageWidget, wargs, 1 );
        XtSetArg( wargs[0], XmNy,        175 );
        XtSetValues( ptr->quitButton, wargs, 1 );
        XtSetValues( ptr->printButton, wargs, 1 );
    }
    else
    {
        XtSetArg( wargs[0], XmNy,        150 );
        XtSetValues( ptr->messageLabelWidget, wargs, 1 );
        XtSetArg( wargs[0], XmNy,        170 );
        XtSetValues( ptr->messageWidget, wargs, 1 );
        XtSetArg( wargs[0], XmNy,        135 );
        XtSetValues( ptr->quitButton, wargs, 1 );
        XtSetValues( ptr->printButton, wargs, 1 );
    }
}

///////////////////////////////////////////////////////////////////////
//
// Description:
//	This routine is called when a Print Quality radio button is pressed.
//
// Use: private

void
SoXtPrintDialog::qualityCB(
    Widget,
    SoXtPrintDialog *ptr,
    XmAnyCallbackStruct *)
//
//////////////////////////////////////////////////////////////////////
{
    ptr->highQuality = (ptr->highQuality) ? FALSE : TRUE;
}

///////////////////////////////////////////////////////////////////////
//
// Description:
//	This routine is called when a Page Format radio button is pressed.
//
// Use: private

void
SoXtPrintDialog::pageFormatCB(
    Widget,
    SoXtPrintDialog *ptr,
    XmAnyCallbackStruct *)
//
//////////////////////////////////////////////////////////////////////
{
    ptr->portraitFormat = (ptr->portraitFormat) ? FALSE : TRUE;
}

///////////////////////////////////////////////////////////////////////
//
// Description:
//	This routine is called when the rgb horizontal size field
//      is modified.
//
// Use: private

void
SoXtPrintDialog::rgbHorizSizeCB(
    Widget,
    SoXtPrintDialog *ptr,
    XmAnyCallbackStruct *)
//
//////////////////////////////////////////////////////////////////////
{
    if (ptr->alreadyUpdated)
        return;

    char *text = XmTextGetString (ptr->rgbHorizSize);
    short x = (short)atoi(text);
    XtFree(text);
    text = XmTextGetString (ptr->rgbVertSize);
    short y = (short)atoi(text);
    XtFree(text);
    ptr->setPrintSize(SbVec2s(x, y));

    // make the text field loose the focus
    XmProcessTraversal(SoXt::getShellWidget(ptr->getWidget()),
            XmTRAVERSE_CURRENT);
}

///////////////////////////////////////////////////////////////////////
//
// Description:
//	This routine is called when the printer vertical size field
//      is modified.
//
// Use: private

void
SoXtPrintDialog::rgbVertSizeCB(
    Widget,
    SoXtPrintDialog *ptr,
    XmAnyCallbackStruct *)
//
//////////////////////////////////////////////////////////////////////
{
    if (ptr->alreadyUpdated)
        return;

    char *text = XmTextGetString (ptr->rgbHorizSize);
    short x = (short)atoi(text);
    XtFree(text);
    text = XmTextGetString (ptr->rgbVertSize);
    short y = (short)atoi(text);
    XtFree(text);
    ptr->setPrintSize(SbVec2s(x, y));

    // make the text field loose the focus
    XmProcessTraversal(SoXt::getShellWidget(ptr->getWidget()),
            XmTRAVERSE_CURRENT);
}

///////////////////////////////////////////////////////////////////////
//
// Description:
//	This routine is called when the printer horizontal size field
//      is modified.
//
// Use: private

void
SoXtPrintDialog::printerHorizSizeCB(
    Widget,
    SoXtPrintDialog *ptr,
    XmAnyCallbackStruct *)
//
//////////////////////////////////////////////////////////////////////
{
    if (ptr->alreadyUpdated)
        return;

    char *text = XmTextGetString (ptr->printerHorizSize);
    float x = atof(text);
    XtFree(text);
    text = XmTextGetString (ptr->printerVertSize);
    float y = atof(text);
    XtFree(text);
    ptr->setPrintSize(SbVec2f(x, y));

    // make the text field loose the focus
    XmProcessTraversal(SoXt::getShellWidget(ptr->getWidget()),
            XmTRAVERSE_CURRENT);
}

///////////////////////////////////////////////////////////////////////
//
// Description:
//	This routine is called when the printer vertical size field
//      is modified.
//
// Use: private

void
SoXtPrintDialog::printerVertSizeCB(
    Widget,
    SoXtPrintDialog *ptr,
    XmAnyCallbackStruct *)
//
//////////////////////////////////////////////////////////////////////
{
    if (ptr->alreadyUpdated)
        return;

    char *text = XmTextGetString (ptr->printerHorizSize);
    float x = atof(text);
    XtFree(text);
    text = XmTextGetString (ptr->printerVertSize);
    float y = atof(text);
    XtFree(text);
    ptr->setPrintSize(SbVec2f(x, y));

    // make the text field loose the focus
    XmProcessTraversal(SoXt::getShellWidget(ptr->getWidget()),
            XmTRAVERSE_CURRENT);
}

///////////////////////////////////////////////////////////////////////
//
// Description:
//	This routine is called when the PostScript horizontal size field
//      is modified.
//
// Use: private

void
SoXtPrintDialog::postScriptHorizSizeCB(
    Widget,
    SoXtPrintDialog *ptr,
    XmAnyCallbackStruct *)
//
//////////////////////////////////////////////////////////////////////
{
    if (ptr->alreadyUpdated)
        return;

    char *text = XmTextGetString (ptr->postScriptHorizSize);
    float x = atof(text);
    XtFree(text);
    text = XmTextGetString (ptr->postScriptVertSize);
    float y = atof(text);
    XtFree(text);
    ptr->setPrintSize(SbVec2f(x, y));

    // make the text field loose the focus
    XmProcessTraversal(SoXt::getShellWidget(ptr->getWidget()),
            XmTRAVERSE_CURRENT);
}

///////////////////////////////////////////////////////////////////////
//
// Description:
//	This routine is called when the PostScript vertical size field
//      is modified.
//
// Use: private

void
SoXtPrintDialog::postScriptVertSizeCB(
    Widget,
    SoXtPrintDialog *ptr,
    XmAnyCallbackStruct *)
//
//////////////////////////////////////////////////////////////////////
{
    if (ptr->alreadyUpdated)
        return;

    char *text = XmTextGetString (ptr->postScriptHorizSize);
    float x = atof(text);
    XtFree(text);
    text = XmTextGetString (ptr->postScriptVertSize);
    float y = atof(text);
    XtFree(text);
    ptr->setPrintSize(SbVec2f(x, y));

    // make the text field loose the focus
    XmProcessTraversal(SoXt::getShellWidget(ptr->getWidget()),
            XmTRAVERSE_CURRENT);
}

///////////////////////////////////////////////////////////////////////
//
// Description:
//	This routine is called when the printer DPI field
//      is modified.
//
// Use: private

void
SoXtPrintDialog::printerDPICB(
    Widget,
    SoXtPrintDialog *ptr,
    XmAnyCallbackStruct *)
//
//////////////////////////////////////////////////////////////////////
{
    if (ptr->alreadyUpdated) {
        ptr->alreadyUpdated = FALSE;
        return;
    }
    ptr->alreadyUpdated = TRUE;
    char *text = XmTextGetString (ptr->printerDPIField);
    XmTextSetString (ptr->postScriptDPIField, text);
    XtFree(text);

    // make the text field loose the focus
    XmProcessTraversal(SoXt::getShellWidget(ptr->getWidget()),
            XmTRAVERSE_CURRENT);
}

///////////////////////////////////////////////////////////////////////
//
// Description:
//	This routine is called when the PostScript DPI field
//      is modified.
//
// Use: private

void
SoXtPrintDialog::postScriptDPICB(
    Widget,
    SoXtPrintDialog *ptr,
    XmAnyCallbackStruct *)
//
//////////////////////////////////////////////////////////////////////
{
    if (ptr->alreadyUpdated) {
        ptr->alreadyUpdated = FALSE;
        return;
    }
    ptr->alreadyUpdated = TRUE;
    char *text = XmTextGetString (ptr->postScriptDPIField);
    XmTextSetString (ptr->printerDPIField, text);
    XtFree(text);

    // make the text field loose the focus
    XmProcessTraversal(SoXt::getShellWidget(ptr->getWidget()),
            XmTRAVERSE_CURRENT);
}

///////////////////////////////////////////////////////////////////////
//
// Description:
//	This routine is called when an Output radio button is pressed.
//
// Use: private

void
SoXtPrintDialog::outputCB(
    Widget,
    SoXtPrintDialog *ptr,
    XmAnyCallbackStruct *)
//
//////////////////////////////////////////////////////////////////////
{
    short height;

    ptr->printerOutput = (ptr->printerOutput) ? FALSE : TRUE;

    //
    // Display the correct portion of the dialog and adjust the bottom
    // of the dialog.
    //
    if (ptr->printerOutput)
    {
        XtUnmanageChild( ptr->fileFormatWidget );
        if (ptr->postScriptOutput)
            XtUnmanageChild( ptr->toRGBFileWidget );
        else
            XtUnmanageChild( ptr->toPostScriptFileWidget );
        XtManageChild( ptr->toPrinterWidget );
        height = PRINTER_MENU_HEIGHT;
    }
    else if (ptr->postScriptOutput)
    {
        XtUnmanageChild( ptr->toPrinterWidget );
        XtManageChild( ptr->fileFormatWidget );
        XtManageChild( ptr->toPostScriptFileWidget );
        height = POSTSCRIPT_FILE_MENU_HEIGHT;
    }
    else
    {
        XtUnmanageChild( ptr->toPrinterWidget );
        XtManageChild( ptr->fileFormatWidget );
        XtManageChild( ptr->toRGBFileWidget );
        height = RGB_FILE_MENU_HEIGHT;
    }

    //
    // check if window needs to be resized
    //
    if (ptr->isTopLevelShell()) {

        // get current window height and set the new height
        SbVec2s size = ptr->getSize();
        size[1] = height;
        ptr->setSize(size);
    }

    ptr->placeBottomOfDialog( ptr );
}

///////////////////////////////////////////////////////////////////////
//
// Description:
//	This routine is called when an Output radio button is pressed.
//
// Use: private

void
SoXtPrintDialog::fileFormatCB(
    Widget,
    SoXtPrintDialog *ptr,
    XmAnyCallbackStruct *)
//
//////////////////////////////////////////////////////////////////////
{
    short height;

    ptr->postScriptOutput = (ptr->postScriptOutput) ? FALSE : TRUE;

    //
    // Display the correct portion of the dialog and adjust the bottom
    // of the dialog.
    //
    if (ptr->postScriptOutput)
    {
        XtUnmanageChild( ptr->toRGBFileWidget );
        XtManageChild( ptr->toPostScriptFileWidget );
        height = POSTSCRIPT_FILE_MENU_HEIGHT;
    }
    else
    {
        XtUnmanageChild( ptr->toPostScriptFileWidget );
        XtManageChild( ptr->toRGBFileWidget );
        height = RGB_FILE_MENU_HEIGHT;
    }

    //
    // check if window needs to be resized
    //
    if (ptr->isTopLevelShell()) {

        // get current window height and set the new height
        SbVec2s size = ptr->getSize();
        size[1] = height;
        ptr->setSize(size);
    }

    ptr->placeBottomOfDialog( ptr );
}

///////////////////////////////////////////////////////////////////////
//
// Description:
//	This callback is invoked when the Quit button is pressed.
//
// Use: private

void
SoXtPrintDialog::quitCB(
    Widget,
    SoXtPrintDialog *ptr,
    XmAnyCallbackStruct *)
//
//////////////////////////////////////////////////////////////////////
{
    ptr->hide();
}

////////////////////////////////////////////////////////////////////////
//
//  Called by Xt when a list item is picked.
//
//  Use: static private
//
void
SoXtPrintDialog::listPick(
    Widget,
    SoXtPrintDialog *ptr,
    XmAnyCallbackStruct *list)
//
////////////////////////////////////////////////////////////////////////
{
    // our array is indexed from 0, the list is indexed from 1
    ptr->whichPrinter = ((XmListCallbackStruct *) list)->item_position - 1;
}

///////////////////////////////////////////////////////////////////////
//
// Description:
//	This routine is called when the Print button of the SoXtPrintDialog 
//      widget is pressed.  It gets the current text string from the 
//      textWidget and uses it as the filename into which to write
//      the PostScript output data. (This is called by the printCallback())
//
// Use: private

void
SoXtPrintDialog::print()
//
//////////////////////////////////////////////////////////////////////
{
    SoOutput      outFile;
    FILE          *fileP = NULL;
    char          *printerName;
    char          tempPSFileName[64];
    char	  *text;
    char          *fileName = NULL;
    SbColor       bgColor(0.0, 0.0, 0.0);
    SoOffscreenRenderer *renderer;

    XmTextSetString (messageWidget, "Printing in progress...");
    XmUpdateDisplay(messageWidget);

    //
    // Invoke the beforeList callbacks
    //
    beforeList.invokeCallbacks ((void *)this);

    // Get Xt Strings.  Update the print size.
    char *dpiString = XmTextGetString(printerDPIField);
    if (postScriptOutput) {
        float fx, fy;
        if (printerOutput) {
	    text = XmTextGetString (printerHorizSize);
            fx = atof(text);
	    XtFree(text);
	    text = XmTextGetString (printerVertSize);
            fy = atof(text);
	    XtFree(text);
            setPrintSize(SbVec2f(fx, fy));
        }
        else {
	    text = XmTextGetString (postScriptHorizSize);
            fx = atof(text);
	    XtFree(text);
	    text = XmTextGetString (postScriptVertSize);
            fy = atof(text);
	    XtFree(text);
            setPrintSize(SbVec2f(fx, fy));
        }
    }
    else {
	text = XmTextGetString (rgbHorizSize);
        short sx = (short)atoi(text);
	XtFree(text);
	text = XmTextGetString (rgbVertSize);
        short sy = (short)atoi(text);
	XtFree(text);
        setPrintSize(SbVec2s(sx, sy));
    }
    // make the text fields loose focus
    XmProcessTraversal(SoXt::getShellWidget(getWidget()),
            XmTRAVERSE_CURRENT);
 
    if (rootNode == NULL)
    {
        XmTextSetString (messageWidget, "ERROR:  Empty database.");
        XmUpdateDisplay(messageWidget);
        afterList.invokeCallbacks ((void *)this);
        return;
    }

    // If Draft Quality, halve the resolution
    SbVec2s renderRes = printRes;
    if (!highQuality)
        renderRes /= 2;

    // If rendering to PostScript, adjust the rendering resolution to account
    // for the specified DPI.
    if (printerOutput || postScriptOutput) {
        float   renderDPI = atof(dpiString);
        renderRes *= 
                (renderDPI / SoOffscreenRenderer::getScreenPixelsPerInch());

        // For postScript Printing, set the backgroundColor to white.  This
        // should probably be changed to get the color from a color editor.
        bgColor.setValue(1.0, 1.0, 1.0);
    }

    // Check the viewport size to make sure the image can be rendered
    // on the current DISPLAY
    SbVec2s maxRes = SoOffscreenRenderer::getMaximumResolution();

    if ((renderRes[0] > maxRes[0]) || (renderRes[1] > maxRes[1])) {
        char tmpStr[64];

        sprintf(tmpStr, "ERROR:  Exceeds %d by %d.",
            maxRes[0], maxRes[1] );
        XmTextSetString (messageWidget, tmpStr);
        XmUpdateDisplay(messageWidget);
        afterList.invokeCallbacks ((void *)this);
        return;
    }

    SbViewportRegion vpRegion(renderRes);
    vpRegion.setPixelsPerInch(atof(dpiString));

    // ??? COMPAT 2.0
    //
    // Create a render action and fill it in with the viewport region 
    // defined by the dialog, and other attributes from the stored 
    // render action.  Use this new render action with the offscreen
    // renderer.
    SoGLRenderAction act(vpRegion);
    _SoPrintDialogItem *item = _SoFindPrintDialog(this);

    if (item != NULL) {
        act.setTransparencyType(item->act->getTransparencyType());
        act.setSmoothing(item->act->isSmoothing());
        act.setNumPasses(item->act->getNumPasses());
    }

    renderer = new SoOffscreenRenderer(&act);
    renderer->setBackgroundColor(bgColor);

    //
    // Render the database!
    //
    if (nodeMostRecent) {
        if (!renderer->render(rootNode)) {
            XmTextSetString (messageWidget, "ERROR:  Could not execute print.");
            XmUpdateDisplay(messageWidget);
            afterList.invokeCallbacks ((void *)this);
            delete renderer;   
            return;
        }
    }
    else
    {
        if (!renderer->render(rootPath)) {
            XmTextSetString (messageWidget, "ERROR:  Could not execute print.");
            XmUpdateDisplay(messageWidget);
            afterList.invokeCallbacks ((void *)this);
            delete renderer;   
            return;
        }
    }

    //
    // Are we printing to a printer or a file?
    //
    if (printerOutput) {
        if (whichPrinter < 0)
        {
            XmTextSetString (messageWidget, "ERROR:  No printer.");
            XmUpdateDisplay(messageWidget);
            afterList.invokeCallbacks ((void *)this);
            delete renderer;   
            return;
        }

        printerName = PRINTER_NAME(whichPrinter);

        //
        // Open a temporary filename to write the data into.  Look at the
        // environment variable TMPDIR and write the file there.
        //
        char *tmpdir = getenv("TMPDIR");

        if (tmpdir == NULL)
            (void)sprintf (tempPSFileName, "/tmp/.sop%d.ps", getpid());
        else
            (void)sprintf (tempPSFileName, "%s/.sop%d.ps", tmpdir, getpid());
        if ((fileP = fopen (tempPSFileName, "w")) == NULL)
        {
            char str[64];
            sprintf(str, "ERROR:  Couldn't open %s", tempPSFileName);
            XmTextSetString (messageWidget, str);
            XmUpdateDisplay(messageWidget);
            afterList.invokeCallbacks ((void *)this);
            delete renderer;   
            return;
        }

        //
        // If the print is to be landscape format, rotate the print
        // 90 degrees and translate it up by a margin.  Write the appropriate
        // PostScript commands to the temporary PostScript file.
        //
        fprintf(fileP, "%%!PS-Adobe-2.0\n");
        if (!portraitFormat)
        {
            fprintf(fileP, "%f %f translate\n",
                    PNTS*(PAGEMARGIN),
                    PNTS*(FULLPAGEYSIZE-PAGEMARGIN));
            fprintf(fileP,"-90.0 rotate\n");
        }
        else
        {
            fprintf(fileP, "%f %f translate\n",
                    PNTS*(PAGEMARGIN),
                    PNTS*(PAGEMARGIN));
        }
        
        renderer->writeToPostScript( fileP, printSize );
        fflush (fileP);
        fclose (fileP);

        // Send the file to the printer

#ifdef __sgi
        union wait waitStatus;
#else
	int waitStatus;
        typedef void (*SIG_PF)(int);
#endif // __sgi
        SIG_PF     childstat;
        pid_t      wpid;

        //
        // Save old function and restore default behavior.
        //
        childstat = signal (SIGCHLD, SIG_DFL);
        if (!(wpid=fork())) {
            if (*printerName == 0)
                execlp("lp", "lp", "-c", "-s", tempPSFileName, 0);
            else {
                char popt[SO_PRINTER_STRLEN];
                sprintf(popt, "-d%s", printerName);
                execlp("lp", "lp", "-c", popt, "-s", tempPSFileName, 0);
            }
        }
        (void)waitpid(wpid, (int *)(&waitStatus), 0);
        //
        // Restore catching dead babies.
        //
        (void) signal(SIGCHLD, childstat);
#ifdef __sgi
        if (!(WIFEXITED(waitStatus) && waitStatus.w_retcode == 0))
#else
	if (!(WIFEXITED(waitStatus) && WEXITSTATUS(waitStatus) == 0))
#endif // __sgi
            SoDebugError::post("SoXtPrintDialog::print",
                    "Print Error. Diagnose with Print Manager.");
        unlink(tempPSFileName);
    }
    else
    {
	//
        // Send to file.  Get the file name from the textWidget and open 
        // the file.
        //
        if (postScriptOutput)
	    fileName = XmTextGetString (postScriptFilenameWidget);
        else
	    fileName = XmTextGetString (rgbFilenameWidget);
	if (strlen(fileName) == 0)
        {
            XmTextSetString (messageWidget, "ERROR:  No file name.");
            XmUpdateDisplay(messageWidget);
            afterList.invokeCallbacks ((void *)this);
            XtFree (fileName);
            delete renderer;   
	    return;
	}

        if (!outFile.openFile (fileName))
        {
            XmTextSetString (messageWidget, "ERROR:  Could not open file.");
            XmUpdateDisplay(messageWidget);
            afterList.invokeCallbacks ((void *)this);
            XtFree (fileName);
            delete renderer;   
	    return;
	}
        if (postScriptOutput) {
            if (!renderer->writeToPostScript( outFile.getFilePointer(),
                    printSize ))
            {
                XmTextSetString (messageWidget, "ERROR:  Could not get data.");
                XmUpdateDisplay(messageWidget);
                outFile.closeFile();
                afterList.invokeCallbacks ((void *)this);
                XtFree (fileName);
                delete renderer;   
	        return;
	    }
        }
        else if (!renderer->writeToRGB( outFile.getFilePointer() ))
        {
            XmTextSetString (messageWidget, "ERROR:  Could not get data.");
            XmUpdateDisplay(messageWidget);
            outFile.closeFile();
            XtFree (fileName);
            afterList.invokeCallbacks ((void *)this);
            delete renderer;   
	    return;
        }
        
        outFile.closeFile();
        XtFree (fileName);
    }

    //
    // Invoke the afterList callbacks
    //
    afterList.invokeCallbacks ((void *)this);

    XmTextSetString (messageWidget, "Printing completed.");
    XmUpdateDisplay(messageWidget);
    delete renderer; 
    XtFree (dpiString);
}

///////////////////////////////////////////////////////////////////////
//
// Description:
//	This routine is called when the Print button of the SoXtPrintDialog 
//      widget is pressed.  It calls print().
//
// Use: private

void
SoXtPrintDialog::printCB(
    Widget,
    SoXtPrintDialog *ptr,
    XmAnyCallbackStruct *)
//
//////////////////////////////////////////////////////////////////////
{
    ptr->print();
}

///////////////////////////////////////////////////////////////////////
//
// Description:
//	Update the textports with new sizes
//
// Use: protected
//
void
SoXtPrintDialog::updateTextports()
//
//////////////////////////////////////////////////////////////////////
{
    alreadyUpdated = TRUE;
    // Send the new size to the widgets
    char tc[12];
    sprintf(tc, "%.3f", printSize[0]);
    XmTextSetString (printerHorizSize, tc);
    XmTextSetString (postScriptHorizSize, tc);
    sprintf(tc, "%.3f", printSize[1]);
    XmTextSetString (printerVertSize, tc);
    XmTextSetString (postScriptVertSize, tc);
    sprintf(tc, "%d", printRes[0]);
    XmTextSetString (rgbHorizSize, tc);
    sprintf(tc, "%d", printRes[1]);
    XmTextSetString (rgbVertSize, tc);

    // If the textports are currently managed, update them
    if (XtIsManaged(printerHorizSize)) {
        XmUpdateDisplay(printerHorizSize);
        XmUpdateDisplay(printerVertSize);
    }
    else if (XtIsManaged(postScriptHorizSize)) {
        XmUpdateDisplay(postScriptHorizSize);
        XmUpdateDisplay(postScriptVertSize);
    }
    if (XtIsManaged(rgbHorizSize)) {
        XmUpdateDisplay(rgbHorizSize);
        XmUpdateDisplay(rgbVertSize);
    }
    alreadyUpdated = FALSE;
}


///////////////////////////////////////////////////////////////////////
//
// Description:
//	This routine gets the list of available printers for printing.
//
// Use: private

void
SoXtPrintDialog::getPrinterList()

//
//////////////////////////////////////////////////////////////////////
{
    FILE        *fp;
    char        line[80];
    SbBool  	firstTime = TRUE;

    fp = popen("lpstat -d -a | awk '/accepting/ {if ($2 != \"not\") print $1}; /^system default destination:/ { print $4 }; /^no system default destination/ { print \"none\"} '", "r");
    while (fgets(line, 80, fp) != NULL) {
        line[strlen(line)-1] = 0;
        if (firstTime) {
            defaultPrinter = strdup(line);
	    firstTime = FALSE;
	}
        else printers->append(strdup(line));
    }
    pclose(fp);
}

//
// redefine those generic virtual functions
//
const char *
SoXtPrintDialog::getDefaultWidgetName() const
{ return "SoXtPrintDialog"; }

const char *
SoXtPrintDialog::getDefaultTitle() const
{ return "Print Dialog"; }

const char *
SoXtPrintDialog::getDefaultIconTitle() const
{ return "Print Dialog"; }

