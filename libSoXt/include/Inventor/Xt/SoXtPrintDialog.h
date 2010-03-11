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
 * Copyright (C) 1990-93   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Description:
 |      This class defines a component for starting print processes.
 |
 |   Author(s): Dave Immel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef SO_XT_PRINT_DIALOG_
#define SO_XT_PRINT_DIALOG_

#include <X11/Intrinsic.h>
#include <Xm/Xm.h>

#include <Inventor/SbBasic.h>
#include <Inventor/Xt/SoXtComponent.h>
#include <Inventor/misc/SoCallbackList.h>

class SbPList;
class SoNode;
class SoPath;
class SoGLRenderAction;
class SoXtPrintDialog;


// callback function prototypes
typedef void SoXtPrintDialogCB(void *userData, SoXtPrintDialog *dialog);

///////////////////////////////////////////////////////////////////////
//
// class: SoXtPrintDialog
//
///////////////////////////////////////////////////////////////////////

// C-api: prefix=SoXtPrintDlog
class SoXtPrintDialog : public SoXtComponent {
  public:
    // Constructors and destructor.
    SoXtPrintDialog(
	Widget parent = NULL,
	const char *name = NULL, 
	SbBool buildInsideParent = TRUE);
    // C-api: name=CreateAct
    SoXtPrintDialog(
        SoGLRenderAction *act,
	Widget parent = NULL,
	const char *name = NULL, 
	SbBool buildInsideParent = TRUE);
   ~SoXtPrintDialog();

    // Sets/gets the node to be used for printing
    // C-api: name=setScenePath
    void        setSceneGraph( SoPath *path );
    // C-api: name=setScene
    void        setSceneGraph( SoNode *root );

    // C-api: name=getScene
    SoNode      *getSceneGraph()   	{ return rootNode; }
    // C-api: name=getScenePath
    SoPath      *getSceneGraphPath()   	{ return rootPath; }

    // C-api: name=setAct
    void 	setGLRenderAction(const SoGLRenderAction *act);
    // C-api: name=getAct
    SoGLRenderAction *getGLRenderAction();

    // Sets the size of the printing
    // C-api: name=setPrintSizeInches
    void        setPrintSize( const SbVec2f &s );
    void        setPrintSize( const SbVec2s &s );

    // These two callbacks are used before and after a print
    // action is performed.
    // C-api: name=setBeforePrintCB
    inline void	setBeforePrintCallback(
			SoXtPrintDialogCB *f,
			void *userData = NULL);
    // C-api: name=setAfterPrintCB
    inline void	setAfterPrintCallback(
			SoXtPrintDialogCB *f,
			void *userData = NULL);

  protected:

    // This constructor takes a boolean whether to build the widget now.
    // Subclasses can pass FALSE, then call SoXtPrintDialog::buildWidget()
    // when they are ready for it to be built.
    SoXtPrintDialog(
	Widget parent,
	const char *name, 
	SbBool buildInsideParent, 
	SbBool buildNow);

    // redefine these
    virtual const char *    getDefaultWidgetName() const;
    virtual const char *    getDefaultTitle() const;
    virtual const char *    getDefaultIconTitle() const;
    
    SoNode          *rootNode;
    SoPath          *rootPath;
    SbBool          printDone;
    SbBool	    highQuality;
    SbBool	    portraitFormat;
    SbBool	    printerOutput, postScriptOutput;
    SbBool          nodeMostRecent;
    SbBool          WYSIWYGflag;
    Widget          messageWidget, printButton, quitButton;
    Widget	    messageLabelWidget, fileFormatWidget;
    Widget          toPrinterWidget, toPostScriptFileWidget, toRGBFileWidget;
    Widget          printerHorizSize, printerVertSize;
    Widget          postScriptHorizSize, postScriptVertSize;
    Widget          rgbHorizSize, rgbVertSize;
    Widget          printerDPIField, postScriptDPIField;
    Widget 	    rgbFilenameWidget, postScriptFilenameWidget;
    SbPList 	    *printers;
    char            *defaultPrinter;
    int	    	    whichPrinter;   	// index into printers list
    SbVec2f         printSize;
    SbVec2s         printRes;
    SoCallbackList  beforeList, afterList;
    SbBool          alreadyUpdated;

    void    	    print();	    	// called by printCallback
    void            getPrinterList();
    void            printToPostScript( SoNode *, char *, int, int );
    
    // Methods used to build the print dialog.
    void            buildToPrinterWidget( Widget parent );
    void            buildToPostScriptFileWidget( Widget parent );
    void            buildToRGBFileWidget( Widget parent );
    void            placeBottomOfDialog( SoXtPrintDialog * );
    void            buildRadioButton( char *,  char *, char *, int, int,
                            Widget, XtCallbackProc );
    void            buildSizeFields( char *, int, Widget, Widget &, Widget &,
                            XtCallbackProc, XtCallbackProc );
    void            buildDPIField( int, Widget, Widget &, XtCallbackProc );
    void            updateTextports();

    //
    // Callback routines.
    //
    static void     qualityCB(  Widget, SoXtPrintDialog *,
                              XmAnyCallbackStruct * );
    static void     pageFormatCB(  Widget, SoXtPrintDialog *,
                              XmAnyCallbackStruct * );
    static void     fileFormatCB(  Widget, SoXtPrintDialog *,
                              XmAnyCallbackStruct * );
    static void     printerHorizSizeCB(  Widget, SoXtPrintDialog *,
                              XmAnyCallbackStruct * );
    static void     printerVertSizeCB(  Widget, SoXtPrintDialog *,
                              XmAnyCallbackStruct * );
    static void     postScriptHorizSizeCB(  Widget, SoXtPrintDialog *,
                              XmAnyCallbackStruct * );
    static void     postScriptVertSizeCB(  Widget, SoXtPrintDialog *,
                              XmAnyCallbackStruct * );
    static void     rgbHorizSizeCB( Widget, SoXtPrintDialog *,
                              XmAnyCallbackStruct * );
    static void     rgbVertSizeCB(  Widget, SoXtPrintDialog *,
                              XmAnyCallbackStruct * );
    static void     printerDPICB(   Widget, SoXtPrintDialog *,
                              XmAnyCallbackStruct * );
    static void     postScriptDPICB( Widget, SoXtPrintDialog *,
                              XmAnyCallbackStruct * );
    static void     outputCB( Widget, SoXtPrintDialog *,
                              XmAnyCallbackStruct * );
    static void     printCB(  Widget, SoXtPrintDialog *,
                              XmAnyCallbackStruct * );
    static void     quitCB(   Widget, SoXtPrintDialog *,
                              XmAnyCallbackStruct * );
    static void	    listPick( Widget, SoXtPrintDialog *ml,
    	    	    	      XmAnyCallbackStruct * );

    // Widget for controlling the Print Style
    Widget          styleButton;
    int             currentStyle;

  private:
    // Builds the widget.
    Widget	    buildWidget(Widget parent);

    // this is called by both constructors
    void constructorCommon(SbBool buildNow);
};

// Inline methods
void
SoXtPrintDialog::setBeforePrintCallback(
    SoXtPrintDialogCB *f,
    void *userData )
{ 
    beforeList.clearCallbacks();
    beforeList.addCallback((SoCallbackListCB *) f, userData);
}

void
SoXtPrintDialog::setAfterPrintCallback(
    SoXtPrintDialogCB *f,
    void *userData )
{
    afterList.clearCallbacks();
    afterList.addCallback((SoCallbackListCB *) f, userData);
}


#endif /* SO_XT_PRINT_DIALOG_ */

