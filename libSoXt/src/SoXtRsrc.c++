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
 |   $Revision: 1.2 $
 |
 |   Classes:
 |	SoXtResource
 |
 |   Author(s): David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SbPList.h>
#include <Inventor/Xt/SoXtComponent.h>
#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/SoXtResource.h>
#include <Inventor/errors/SoDebugError.h>

#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#include <X11/Xresource.h>

// ShellP.h has a variable named 'class' - not good for c++!
#define class CLASS
#include <X11/ShellP.h>
#undef class


#define GET_RESOURCE(DISPLAY,STRNAME,STRCLASS,RETTYPE,RETVAL) \
	XrmGetResource(XrmGetDatabase(DISPLAY),STRNAME,STRCLASS,RETTYPE,RETVAL)

#define GET_QRESOURCE(DISPLAY,QNAME,QCLASS,RETTYPE,RETVAL) \
	XrmQGetResource(XrmGetDatabase(DISPLAY),QNAME,QCLASS,RETTYPE,RETVAL)


////////////////////////////////////////////////////////////////////////
//
// Constructor - this builds a quark list representing the widget
// hierarchy leading down to w.
//
SoXtResource::SoXtResource(Widget widget)
//
////////////////////////////////////////////////////////////////////////
{
    if (widget == NULL)
    	return;
	
    SbPList 	nameplist, classplist;
    SoXtComponent	*comp;
    Widget  	w = widget;
    XrmQuark	n,c;
    
    display = XtDisplay(widget);
    
    // Traverse up the widget tree gather widget names and class names.
    // If the widget is a Inventor component, we do not get these names
    // from the widget itself; rather, we get them from the component.
    // This is so that we can fool the X resource manager into looking
    // up resource values based on our class names (e.g. SoColorPicker)
    // rather than the 'real' class names (e.g. XmForm).
    while (w != NULL) {
    	if (comp = SoXtComponent::getComponent(w)) {
	    // get the widget name and class from SoXtComponent.
	    // we do this so that we can use Inventor class names
	    // like "SoMaterialEditor" instead of the Motif names
	    // which the components are really built from (e.g.XmForm).
	    const char *widgetName = comp->getWidgetName();
	    if (widgetName != NULL)
		 n = XrmStringToQuark(widgetName);
	    else n = XrmStringToQuark("");

	    const char *className = comp->getClassName();
	    if (className != NULL)
		 c = XrmStringToQuark(className);
	    else c = XrmStringToQuark("");
    	}
	else if ((XtParent(w) == NULL) && XtIsApplicationShell(w)) {
	    // ??? KLUDGE from Xt src code (Xt/Resources.c)
	    // We have to get the application class differently
	    n = w->core.xrm_name;
	    c = ((ApplicationShellWidget) w)->application.xrm_class;
	    // ??? end of kludge
	}
	else {
	    // get the widget name and class from the widget
	    n = w->core.xrm_name;
	    c = XtClass(w)->core_class.xrm_class;
	}
	nameplist.append((void *) (unsigned long) n);
	classplist.append((void *) (unsigned long) c);
	w = XtParent(w);
    }

    // Allocate the quark list, and reverse the order of names so that
    // the list specifies the hierarchy from the root down to this widget.
    // The size of our list is 2 greater:
    //   1 for the resource which will be specified later
    //   1 for a NULL end-of-list sentinel
    
    int q,s;
    int len = nameplist.getLength(); // classplist.getLength() is the same
    listSize = len + 2;
    nameList = new XrmQuark[listSize];
    classList = new XrmQuark[listSize];
    for (q = 0, s = len - 1;
    	 s >= 0; q++, s--) {
#if (_MIPS_SZPTR == 64 || __ia64)
	 nameList[q]  = (XrmQuark) ((long) nameplist[s]);
	 classList[q] = (XrmQuark) ((long) classplist[s]);
#else
	 nameList[q]  = (XrmQuark) nameplist[s];
	 classList[q] = (XrmQuark) classplist[s];
#endif
    }

    // make the last entry the NULL sentinel
    nameList[listSize - 1]  = NULLQUARK;
    classList[listSize - 1] = NULLQUARK;
}

////////////////////////////////////////////////////////////////////////
//
// Denstructor - nuke the quark lists.
//
SoXtResource::~SoXtResource()
//
////////////////////////////////////////////////////////////////////////
{
    delete [ /*listSize*/ ] nameList;
    delete [ /*listSize*/ ] classList;
}

////////////////////////////////////////////////////////////////////////
//
// This gets the 'SbColor' resource value, returning TRUE if successful.
//
SbBool
SoXtResource::getResource(char *resName, char *resClass, SbColor &c)
//
////////////////////////////////////////////////////////////////////////
{
    nameList[listSize - 2]  = XrmStringToQuark(resName);
    classList[listSize - 2] = XrmStringToQuark(resClass);
    return getResource(display, nameList, classList, c);
}

////////////////////////////////////////////////////////////////////////
//
// This gets the 'short' resource value, returning TRUE if successful.
//
SbBool
SoXtResource::getResource(char *resName, char *resClass, short &i)
//
////////////////////////////////////////////////////////////////////////
{
    nameList[listSize - 2]  = XrmStringToQuark(resName);
    classList[listSize - 2] = XrmStringToQuark(resClass);
    return getResource(display, nameList, classList, i);
}

////////////////////////////////////////////////////////////////////////
//
// This gets the 'u_short' resource value, returning TRUE if successful.
//
SbBool
SoXtResource::getResource(char *resName, char *resClass, unsigned short &u)
//
////////////////////////////////////////////////////////////////////////
{
    nameList[listSize - 2]  = XrmStringToQuark(resName);
    classList[listSize - 2] = XrmStringToQuark(resClass);
    return getResource(display, nameList, classList, u);
}

////////////////////////////////////////////////////////////////////////
//
// This gets the value for the resource, returning TRUE if successful.
//
SbBool
SoXtResource::getResource(char *resName, char *resClass, char *&s)
//
////////////////////////////////////////////////////////////////////////
{
    nameList[listSize - 2]  = XrmStringToQuark(resName);
    classList[listSize - 2] = XrmStringToQuark(resClass);
    return getResource(display, nameList, classList, s);
}

////////////////////////////////////////////////////////////////////////
//
// This gets the 'SbBool' resource value, returning TRUE if successful.
//
SbBool
SoXtResource::getResource(char *resName, char *resClass, SbBool &b)
//
////////////////////////////////////////////////////////////////////////
{
    nameList[listSize - 2]  = XrmStringToQuark(resName);
    classList[listSize - 2] = XrmStringToQuark(resClass);
    return getResource(display, nameList, classList, b);
}

////////////////////////////////////////////////////////////////////////
//
// This gets the 'SbBool' resource value, returning TRUE if successful.
//
SbBool
SoXtResource::getResource(char *resName, char *resClass, float &f)
//
////////////////////////////////////////////////////////////////////////
{
    nameList[listSize - 2]  = XrmStringToQuark(resName);
    classList[listSize - 2] = XrmStringToQuark(resClass);
    return getResource(display, nameList, classList, f);
}


//
// Some convenience routines
//

static
SbBool getColor(Display *d, char *val, SbColor &c)
{
    if (val != NULL) {
	XColor rgb;
	XParseColor(d,
	    XDefaultColormap(d,DefaultScreen(d)),
	    val,
	    &rgb);
	
	short r,g,b;
	r = rgb.red >> 8;
	g = rgb.green >> 8;
	b = rgb.blue >> 8;
	c.setValue(float(r/255.),float(g/255.),float(b/255.));
	
	return TRUE;
    }
    
    return FALSE;
}

static
SbBool getShort(char *val, short &s)
{
    SbBool ok = FALSE;
    
    if (val != NULL) {
	// the value may or may not be hex (begin with '#')
	int i;
	if (sscanf(val, "%d", &i)) {
	    s = i;
	    ok = TRUE;
	}
	else if (sscanf(val, "#%x", &i)) {
	    s = i;
	    ok = TRUE;
	}
    }
    
    return ok;
}

static
SbBool getUShort(char *val, unsigned short &u)
{
    SbBool ok = FALSE;
    
    if (val != NULL) {
	// the value may or may not be hex (begin with '#')
	int i;
	if (sscanf(val, "%d", &i)) {
	    u = i;
	    ok = TRUE;
	}
	else if (sscanf(val, "#%x", &i)) {
	    u = i;
	    ok = TRUE;
	}
    }
    
    return ok;
}

static
SbBool getBool(char *val, SbBool &b)
{
    SbBool ok = TRUE;
    
    if (val != NULL) {
	if      (strcmp(val, "True") == 0)
	    b = TRUE;
	else if (strcmp(val, "False") == 0)
	    b = FALSE;
	else if (strcmp(val, "On") == 0)
	    b = TRUE;
	else if (strcmp(val, "Off") == 0)
	    b = FALSE;
	else if (strcmp(val, "true") == 0)
	    b = TRUE;
	else if (strcmp(val, "false") == 0)
	    b = FALSE;
	else if (strcmp(val, "on") == 0)
	    b = TRUE;
	else if (strcmp(val, "off") == 0)
	    b = FALSE;
	else ok = FALSE;
    }
    else ok = FALSE;
    
    return ok;
}

static
SbBool getFloat(char *val, float &f)
{
    SbBool ok = FALSE;
    
    if (val != NULL) {
	float g;
	if (sscanf(val, "%f", &g)) {
	    f = g;
	    ok = TRUE;
	}
    }
    
    return ok;
}


//
// Get resource from X
//

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Find the color resource for strName,strClass and return it in c.
//
// Use: static, private
//
SbBool
SoXtResource::getResource(
    Display *d,
    char *strName,
    char *strClass,
    SbColor &c)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool ok = FALSE;
    XrmString typeStr;
    XrmValue result;
    
#ifdef DEBUG
    // make sure Display is valid
    if (d == NULL) {
	SoDebugError::post("SoXtResource::getResource",
			 "ERROR SoXtResource::getResource - Display is NULL");
	return FALSE;
    }
#endif


    if (GET_RESOURCE(d, strName, strClass, &typeStr, &result)) {
	ok = getColor(d, result.addr, c);
    }
    
    return ok;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Find the color resource for qName,qClass and return it in c.
//
// Use: static, public
//
SbBool
SoXtResource::getResource(
    Display *d,
    XrmQuarkList qName,
    XrmQuarkList qClass,
    SbColor &c)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool ok = FALSE;
    XrmRepresentation rep;
    XrmValue result;
    
#ifdef DEBUG
    // make sure Display is valid
    if (d == NULL) {
	SoDebugError::post("SoXtResource::getResource",
			 "ERROR SoXtResource::getResource - Display is NULL");
	return FALSE;
    }
#endif

    if (GET_QRESOURCE(d, qName, qClass, &rep, &result)) {
	ok = getColor(d, result.addr, c);
    }
    
    return ok;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Find the 'short' resource for strName,strClass and return it in s.
//
// Use: static, public
//
SbBool
SoXtResource::getResource(
    Display *d,
    char *strName,
    char *strClass,
    short &s)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool ok = FALSE;
    XrmString typeStr;
    XrmValue result;
    
#ifdef DEBUG
    // make sure Display is valid
    if (d == NULL) {
	SoDebugError::post("SoXtResource::getResource",
			 "ERROR SoXtResource::getResource - Display is NULL");
	return FALSE;
    }
#endif

    if (GET_RESOURCE(d, strName, strClass, &typeStr, &result)) {
	ok = getShort(result.addr, s);
    }
    
    return ok;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Find the 'short' resource for qName,qClass and return it in s.
//
// Use: static, public
//
SbBool
SoXtResource::getResource(
    Display *d,
    XrmQuarkList qName,
    XrmQuarkList qClass,
    short &s)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool ok = FALSE;
    XrmRepresentation rep;
    XrmValue result;
    
#ifdef DEBUG
    // make sure Display is valid
    if (d == NULL) {
	SoDebugError::post("SoXtResource::getResource",
			 "ERROR SoXtResource::getResource - Display is NULL");
	return FALSE;
    }
#endif

    if (GET_QRESOURCE(d, qName, qClass, &rep, &result)) {
	ok = getShort(result.addr, s);
    }
    
    return ok;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Find the 'unsigned short' resource for strName,strClass
// and return it in u.
//
// Use: static, public
//
SbBool
SoXtResource::getResource(
    Display *d,
    char *strName,
    char *strClass,
    unsigned short &u)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool ok = FALSE;
    XrmString typeStr;
    XrmValue result;
    
#ifdef DEBUG
    // make sure Display is valid
    if (d == NULL) {
	SoDebugError::post("SoXtResource::getResource",
			 "ERROR SoXtResource::getResource - Display is NULL");
	return FALSE;
    }
#endif

    if (GET_RESOURCE(d, strName, strClass, &typeStr, &result)) {
	ok = getUShort(result.addr, u);
    }
    
    return ok;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Find the 'unsigned short' resource for qName,qClass
// and return it in u.
//
// Use: static, public
//
SbBool
SoXtResource::getResource(
    Display *d,
    XrmQuarkList qName,
    XrmQuarkList qClass,
    unsigned short &u)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool ok = FALSE;
    XrmRepresentation rep;
    XrmValue result;
    
#ifdef DEBUG
    // make sure Display is valid
    if (d == NULL) {
	SoDebugError::post("SoXtResource::getResource",
			 "ERROR SoXtResource::getResource - Display is NULL");
	return FALSE;
    }
#endif

    if (GET_QRESOURCE(d, qName, qClass, &rep, &result)) {
	ok = getUShort(result.addr, u);
    }
    
    return ok;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Find the string resource for strName,strClass and return it in s.
//
// Use: static, public
//
SbBool
SoXtResource::getResource(
    Display *d,
    char *strName,
    char *strClass,
    char *&s)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool ok = FALSE;
    XrmString typeStr;
    XrmValue result;
    
#ifdef DEBUG
    // make sure Display is valid
    if (d == NULL) {
	SoDebugError::post("SoXtResource::getResource",
			 "ERROR SoXtResource::getResource - Display is NULL");
	return FALSE;
    }
#endif

    if (GET_RESOURCE(d, strName, strClass, &typeStr, &result)) {
	if (result.addr != NULL) {
	    s = result.addr;
	    ok = TRUE;
	}
    }
    
    return ok;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Find the string resource for qName,qClass and return it in s.
//
// Use: static, public
//
SbBool
SoXtResource::getResource(
    Display *d,
    XrmQuarkList qName,
    XrmQuarkList qClass,
    char *&s)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool ok = FALSE;
    XrmRepresentation rep;
    XrmValue result;
    
#ifdef DEBUG
    // make sure Display is valid
    if (d == NULL) {
	SoDebugError::post("SoXtResource::getResource",
			 "ERROR SoXtResource::getResource - Display is NULL");
	return FALSE;
    }
#endif

    if (GET_QRESOURCE(d, qName, qClass, &rep, &result)) {
	if (result.addr != NULL) {
	    s = result.addr;
	    ok = TRUE;
	}
    }
    
    return ok;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Find the 'SbBool' resource for strName,strClass and return it in b.
// Valid strings are "True", "False", "On", "Off".
//
// Use: static, public
//
SbBool
SoXtResource::getResource(
    Display *d,
    char *strName,
    char *strClass,
    SbBool &b)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool ok = FALSE;
    XrmString typeStr;
    XrmValue result;
    
#ifdef DEBUG
    // make sure Display is valid
    if (d == NULL) {
	SoDebugError::post("SoXtResource::getResource",
			 "ERROR SoXtResource::getResource - Display is NULL");
	return FALSE;
    }
#endif

    if (GET_RESOURCE(d, strName, strClass, &typeStr, &result)) {
	ok = getBool(result.addr, b);
    }
    
    return ok;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Find the 'short' resource for qName,qClass and return it in s.
// Valid strings are "True", "False", "On", "Off".
//
// Use: static, public
//
SbBool
SoXtResource::getResource(
    Display *d,
    XrmQuarkList qName,
    XrmQuarkList qClass,
    SbBool &b)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool ok = FALSE;
    XrmRepresentation rep;
    XrmValue result;
    
#ifdef DEBUG
    // make sure Display is valid
    if (d == NULL) {
	SoDebugError::post("SoXtResource::getResource",
			 "ERROR SoXtResource::getResource - Display is NULL");
	return FALSE;
    }
#endif

    if (GET_QRESOURCE(d, qName, qClass, &rep, &result)) {
	ok = getBool(result.addr, b);
    }
    
    return ok;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Get a float value.
//
// Use: static, private
//
SbBool
SoXtResource::getResource(
    Display *d,
    char *strName,
    char *strClass,
    float &f)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool ok = FALSE;
    XrmString typeStr;
    XrmValue result;
    
#ifdef DEBUG
    // make sure Display is valid
    if (d == NULL) {
	SoDebugError::post("SoXtResource::getResource",
			 "ERROR SoXtResource::getResource - Display is NULL");
	return FALSE;
    }
#endif

    if (GET_RESOURCE(d, strName, strClass, &typeStr, &result)) {
	ok = getFloat(result.addr, f);
    }
    
    return ok;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Get a float value.
//
// Use: static, private
//
SbBool
SoXtResource::getResource(
    Display *d,
    XrmQuarkList qName,
    XrmQuarkList qClass,
    float &f)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool ok = FALSE;
    XrmRepresentation rep;
    XrmValue result;
    
#ifdef DEBUG
    // make sure Display is valid
    if (d == NULL) {
	SoDebugError::post("SoXtResource::getResource",
			 "ERROR SoXtResource::getResource - Display is NULL");
	return FALSE;
    }
#endif

    if (GET_QRESOURCE(d, qName, qClass, &rep, &result)) {
	ok = getFloat(result.addr, f);
    }
    
    return ok;
}
