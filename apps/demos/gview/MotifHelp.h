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

//////////////////////////////////////////////////////////////////
//
// Dealing with Xt/Xm arguments
//

#define ARG_VARS(num)							      \
    int		argN = 0;						      \
    Arg         args[num]

#define ADD_ARG(resource, value)					      \
		XtSetArg(args[argN], resource, value); argN++

#define RESET_ARGS()    argN = 0
#define ARGS            args, argN

//////////////////////////////////////////////////////////////////
//
// Dealing with form arguments
//

#define ADD_LEFT_FORM(offset)						      \
    ADD_ARG(XmNleftAttachment,		XmATTACH_FORM);			      \
    ADD_ARG(XmNleftOffset,		offset)

#define ADD_LEFT_WIDGET(widget, offset)					      \
    ADD_ARG(XmNleftAttachment,		XmATTACH_WIDGET);		      \
    ADD_ARG(XmNleftWidget,		widget);			      \
    ADD_ARG(XmNleftOffset,		offset)

#define ADD_RIGHT_FORM(offset)						      \
    ADD_ARG(XmNrightAttachment,		XmATTACH_FORM);			      \
    ADD_ARG(XmNrightOffset,		offset)

#define ADD_RIGHT_WIDGET(widget, offset)				      \
    ADD_ARG(XmNrightAttachment,		XmATTACH_WIDGET);		      \
    ADD_ARG(XmNrightWidget,		widget);			      \
    ADD_ARG(XmNrightOffset,		offset)

#define ADD_TOP_FORM(offset)						      \
    ADD_ARG(XmNtopAttachment,		XmATTACH_FORM);			      \
    ADD_ARG(XmNtopOffset,		offset)

#define ADD_TOP_WIDGET(widget, offset)					      \
    ADD_ARG(XmNtopAttachment,		XmATTACH_WIDGET);		      \
    ADD_ARG(XmNtopWidget,		widget);			      \
    ADD_ARG(XmNtopOffset,		offset)

#define ADD_BOTTOM_FORM(offset)						      \
    ADD_ARG(XmNbottomAttachment,	XmATTACH_FORM);			      \
    ADD_ARG(XmNbottomOffset,		offset)

#define ADD_BOTTOM_WIDGET(widget, offset)				      \
    ADD_ARG(XmNbottomAttachment,	XmATTACH_WIDGET);		      \
    ADD_ARG(XmNbottomWidget,		widget);			      \
    ADD_ARG(XmNbottomOffset,		offset)

//////////////////////////////////////////////////////////////////
//
// Dealing with Xm strings
//

#define STRING(a) XmStringCreateSimple(a)


#if JUST_FOR_CUT_AND_PASTE
    ADD_LEFT_FORM(0);
    ADD_LEFT_WIDGET(w, 0);

    ADD_RIGHT_FORM(0);
    ADD_RIGHT_WIDGET(w, 0);

    ADD_TOP_FORM(0);
    ADD_TOP_WIDGET(w, 0);

    ADD_BOTTOM_FORM(0);
    ADD_BOTTOM_WIDGET(w, 0);
#endif

//////////////////////////////////////////////////////////////////
//
// Adding/removing a callback to/from a widget
//

#define ADD_CB(widget, resource, func, data)				      \
    XtAddCallback(widget, resource, (XtCallbackProc) func, (XtPointer) data)

#define REM_CB(widget, resource, func, data)				      \
    XtRemoveCallback(widget, resource, (XtCallbackProc) func, (XtPointer) data)

