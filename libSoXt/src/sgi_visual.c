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

#include <sys/param.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/StringDefs.h>

#ifdef _SG_IS_STATIC
/*
 * This hideous hack is to allow XmRoxy to compile and use this code without
 * actually adding any symbols to the Xm library.
 *       #ifdef _SG_IS_STATIC
 *       static
 *       #endif
 * is placed at the beginning of each function definition; XmRoxy will
 * define _SG_IS_STATIC.
 *
 * This is the same as using "_SG_STORAGE", which is defined in sgi_visual.h
 *
 * Now that more than one subsystem (window manager, XmRoxy) is using this
 * code, perhaps it should be put in a more central place (libSgt?).  At that
 * time this hack can be removed.
 */
#endif

#include <ctype.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include "sgi_visual.h"


/*
 *  Some of the less obvious choices are made because many of the entry points
 *  actually expect DWIM (Do What I Mean) code.  In actual practice, a NULL
 *  generally means DWIM, not "use a literal default value" or "don't care".
 *
 *  We could probably do more/better defaulting in some routines;  if the
 *  need arises, be sure not to introduce any circular logic.  Many of these
 *  routines call each other.
 */

/*  THINGS TO DO:
 *	* NOTE: scr uses 0 as a default value, when in fact 0 is a valid
 *	  value.  scr should be made a pointer.  Unfortunately, most X
 *	  convention passes acr directly.  Probably better to leave as is.
 *
 *	* For parameters that must be passed, not defaulted, return an
 *	  error status for out-of-range (e.g defaulted).
 */

/*  KNOWN BUGS:
 *	* _SG_Warning core dumps with non-Xt programs.  Fix it to work right
 *	  (i.e. not core dump).  See the remarks with that routine.
 */


/*
 * Variables for this module
 */

/*
 * used to return a value if and only if the caller passed a pointer.
 */
#define setNonNull(a,b) if (a) *(a) = (b)


/*
 *  We need to sort out visual information for ourselves because:
 *
 *	* X11 does not give us the necessary facilities, so we provide our
 *	  own heuristics.
 *
 *	* This provides a form of caching -- in particular, every place
 *	  that uses the same visual uses the same colormap.
 *
 * Some values (the arrays) contain a default value for each visual type.
 * Defaults for the dpy/scr are indexed by defaultVisualType
 *
 *  The following visual data structure is used throughout this module.
 */
struct overlayData {
	long  overlay_visual;
	long  transparent_type;
	long  value;
	long  layer;
};

typedef struct _VisInfo {
	struct _VisInfo	   *next;	/* ==> data for next screen/visual */
	Display		   *dpy;	/* display this visual info is for */
	int		    scr;	/* screen this visual info is for */
	struct overlayData *pOverlay;	/* SERVER_OVERLAY_VISUALS property */
	unsigned long	    nOverlay;	/* number of visuals in overlay prop */
	XVisualInfo	   *visualList;	/* ==> fetched visuals list */
	int		    num;	/* # of visuals for this scr & dpy */
	Colormap	   *colormap;	/* colormap for each visual */
	int		   *flag;	/* Attribute flags for each visual */
	int		    defaultVisualType;
					/* for this display/screen */
	XVisualInfo	   *defaultVisualP[SG_MAX_TYPES];
					/* ==> dflt visual record */
	int		    defaultClass[SG_MAX_TYPES];
					/* default visual class */
	int		    defaultDepth[SG_MAX_TYPES];
					/* default visual depth */
	char		   *defaultVisualTypeS[SG_MAX_TYPES];
					/* default visual type string */
} VisInfo, *VisInfoPtr;


/* Internal forward references */
#ifdef _NO_PROTO
	_SG_STORAGE VisInfoPtr _SG_getVisualList();
	_SG_STORAGE Visual *_SG_getVisual();
	_SG_STORAGE int _SG_getMatchingClass();
	_SG_STORAGE int _SG_getMatchingDepth();
	_SG_STORAGE int _SG_getMatchingType();
	_SG_STORAGE void _SG_warning();
#else/* _NO_PROTO */
	_SG_STORAGE VisInfoPtr _SG_getVisualList
		(Display *dpy, int scr);
	_SG_STORAGE Visual *_SG_getVisual
		(Display *dpy, int scr, int depth, int *v_class, int v_type);
	_SG_STORAGE int _SG_getMatchingClass
		(Display *dpy, int scr, int type, int depth, int *class);
	_SG_STORAGE int _SG_getMatchingDepth
		(Display *dpy, int scr, int *class, int type, int depth);
	_SG_STORAGE int _SG_getMatchingType
		(Display *dpy, int scr, int *class, int type, char **typeP);
	_SG_STORAGE void _SG_warning
		(Display *dpy, char *format, char *p1, char *p2, char *p3, char *p4);
#endif /* _NO_PROTO */

/* Global storage */
	static int PColor = PseudoColor;



/*
 *  _SG_getDefaultColormap -- Routine to get a default ("best") colormap.
 *
 *  Since the default colormap for each visual does not yet exist, we don't
 *  know with certainty what arguments will be needed in the long run.
 *
 *  I have been promised that before the Cypress release there will be a
 *  default color map for each visual.  At such time we should adopt it.
 *
 *  In the meantime, we just get any suitable one.  We do record the one we got,
 *  and re-use it for later requests where possible.  This at least leaves a
 *  single application coordinated.
 *
 *  Arguments:		Default Values
 *  =================	=====================
 *	dpy		none -- must be passed
 *	scr		none -- must be passed
 *	vsl		NULL	DefaultVisual(dpy, scr)
 *
 *  Errors returned:	SG_BAD_DISPLAY	no dpy argument supplied (one must be)
 *
 *  Known Bugs:		NONE
 *
 *  Things To Do Later:
 *	* When X11 supports a default colormap for each visual, we should
 *	  fetch it.  That would considerably shorten this routine -- we
 *	  would no longer need to call _SG_getVisualList and search the
 *	  data it returns.
 *
 */

_SG_STORAGE Colormap
_SG_getDefaultColormap (dpy, scr, vsl)
	Display *dpy;
	int      scr;
	Visual  *vsl;
{
	VisInfoPtr vp;
	register int i;

    /*
     *  Screen and visual can both be defaulted.
     *  If either has been, then set up the true value.
     */
	if (!dpy) return ((Colormap)SG_BAD_DISPLAY);	/* Cannot default dpy */
	if (!vsl) vsl = DefaultVisual(dpy, scr);

    /*  See whether we are looking for the one-and-only default colormap. */
	if ( vsl == DefaultVisual(dpy,scr) )
		{ return DefaultColormap (dpy, scr); }

    /*
     *  Search the list of visuals for a matching one.
     *  Then get its colormap.
     */
	vp = _SG_getVisualList (dpy, scr);	/* list of visuals */
	for ( i=0 ; i < vp->num; i++ ) {
		if ( (vp->visualList)[i].visual != vsl)
			continue;
		if ( ! (vp->colormap)[i] ) {
			(vp->colormap)[i]  = XCreateColormap(
				dpy, RootWindow (dpy, scr), vsl, AllocNone);
		}
		return (vp->colormap)[i];
	}

    /*
     *  No matching visual -- just get the caller a colormap of its own.
     */
	return XCreateColormap( dpy, RootWindow (dpy, scr), vsl, AllocNone);
} /* END OF FUNCTION _SG_getDefaultColormap */


/*
 *  Get the default depth for the given visual class and type.
 *
 *  Arguments:		Default Values
 *  =================	=====================
 *	dpy		none -- must be passed
 *	scr		none -- must be passed
 *	class		NULL	Not checked
 *	type		NULL	Normal (main) planes
 *
 *                              | explicit  |     normal      |  other
 *  type ==> ||     NULL        | default   |     planes      |  value
 *  class    ||                 |           |                 |
 *  =====================================================================
 *           ||                 |           | if (default     |
 *  NULL     || default         | default   | class ==        | maximum
 *           || depth           | depth     | PseudoColor)    | depth
 *           ||                 |           | then 8 else max |
 *  =========||=================|===========|=================|==========
 *           ||                 |           |                 |
 *  explicit || default         | default   |                 | maximum
 *  default  || depth           | depth     |  ^^ ditto ^^    | depth
 *           ||                 |           |                 |
 *  =========||=================|===========|=================|==========
 *           ||                 |           |                 |
 *  Pseudo-  || if (default     |           |                 | maximum
 *  Color    || type == NORMAL) | <== ditto |      8          | depth
 *           || then 8 else max |           |                 |
 *           ||                 |           |                 |
 *  =========||=================|===========|=================|==========
 *           ||                 |           |                 |
 *  other    || maximum         | maximum   | maximum         | maximum
 *  value    || depth           | depth     | depth           | depth
 *           ||                 |           |                 |
 *  =====================================================================
 *
 *
 *  Normal planes (class PseudoColor) prefer to return depth eight because
 *  that will minimize colormap flashing once Jeff puts in his code to
 *  provide 16 8-bit normal plane colormaps.
 *
 *  Errors returned:	SG_BAD_DISPLAY	no dpy argument supplied (one must be)
 *
 *  Known Bugs:		None
 *
 *  Things To Do Later:	Nothing
 *
 */

_SG_STORAGE int
_SG_getDefaultDepth (dpy, scr, class, type)
	Display  *dpy;
	int       scr, *class, type;
{
	VisInfoPtr vp;

    /* Process the calling parameters.  Set up any needed defaults. */
 	if (!dpy) return ((int)SG_BAD_DISPLAY);		/* Cannot default dpy*/

    /*
     * If both type and class are (implicitly or explicitly) default,
     * so is the depth.
     */
	vp = _SG_getVisualList (dpy, scr);	     /* list of visuals */
	if (   (!class || (*class == vp->defaultClass[vp->defaultVisualType]))
	    && (!type  || (type == vp->defaultVisualType))
	   ) {
		return DefaultDepth(dpy, scr);
	}

    /*
     * Normal planes && PseudoColor prefers 8 bits.  This is because of
     * plans to make 16 8-bit PseudoColor colormaps available.
     */
	if (   (   (   !class
		    && (vp->defaultClass[vp->defaultVisualType] == PseudoColor))
		 ||(class && (*class == PseudoColor)))
	    && (   (  !type && (vp->defaultVisualType == SG_NORMAL_PLANES))
		 ||(type == SG_NORMAL_PLANES))
	   ) {
		if (   (vp->defaultClass[vp->defaultVisualType] == PseudoColor)
		    && (vp->defaultVisualType == SG_NORMAL_PLANES)
		   ) return 8;
	}

	return _SG_getMaxDepth(dpy, scr, class, type);

} /* END OF FUNCTION _SG_getDefaultDepth */


/*
 *  Check that the passed class actually exists, for this type and depth.
 *  If the passed one does not exist, try to get one that does.
 *
 *  Arguments:		Default Values
 *  =================	=====================
 *	dpy		none -- must be passed
 *	scr		none -- must be passed
 *	type		none -- must be passed (underlay=1 ... overlay=4)
 *	depth		none -- depth to be matched
 *	class		NULL	Don't care -- returns some suitable one
 *
 *  Errors returned:	SG_BAD_DISPLAY	no dpy argument supplied (one must be)
 *
 *  Known Bugs:		None
 *
 *  Things To Do Later:
 *	* Should type and depth use default values if passed a NULL?
 *
 */

_SG_STORAGE int
_SG_getMatchingClass (dpy, scr, type, depth, class)
	Display  *dpy;
	int       scr, type, depth, *class;
{
	VisInfoPtr vp;
	register int i;
	int foundClass = 0;

    /* Process the calling parameters.  Set up any needed defaults. */
 	if (!dpy)   return ((int)SG_BAD_DISPLAY); /* Cannot default dpy*/

	vp = _SG_getVisualList (dpy, scr);	  /* list of visuals */

    /* Search the list for the matching Class */
	for ( i=0 ; i < vp->num; i++ ) {
		if ( (vp->flag)[i] != type ) continue;
		if ( (vp->visualList)[i].depth != depth ) continue;
		if ( class && ((vp->visualList)[i].class == *class) )
			return *class;
	    /*
	     *  Bias towards returning:
	     *		default class (if present)
	     *		PseudoColor (if cannot find the default class)
	     *  Otherwise return whatever we can find.
	     */
		if (foundClass == vp->defaultClass[vp->defaultVisualType]) {
			;	/* Already found the default class */
		} else if (foundClass == PseudoColor) {
			if ( (vp->visualList)[i].class
				== vp->defaultClass[vp->defaultVisualType])
				{ foundClass = (vp->visualList)[i].class; }
		} else {
			foundClass = (vp->visualList)[i].class;
		}
	}

    /* Else get what we can. */
	return foundClass;

} /* END OF FUNCTION _SG_getMatchingClass */


/*
 *  Check that the passed depth actually exists.
 *  If not, get the default depth.
 *
 *  Arguments:		Default Values
 *  =================	=====================
 *	dpy		none -- must be passed
 *	scr		none -- must be passed
 *	class		NULL	Don't care
 *	type		none -- must be passed (underlay=1 ... overlay=4)
 *	depth			depth to be matched or defaulted
 *
 *  Errors returned:	SG_BAD_DISPLAY	no dpy argument supplied
 *
 *  Known Bugs:
 *	* class = 0 should not be used for "don't care",
 *	  because it could mean StaticGray
 *
 *  Things To Do Later:	Nothing
 *
 */

_SG_STORAGE int
_SG_getMatchingDepth (dpy, scr, class, type, depth)
	Display  *dpy;
	int       *class, scr, type, depth;
{
	VisInfoPtr vp;
	register int i;

    /* Process the calling parameters.  Set up any needed defaults. */
 	if (!dpy)   return ((int)SG_BAD_DISPLAY); /* Cannot default dpy*/

	vp = _SG_getVisualList (dpy, scr);	  /* list of visuals */

    /* Search the list for the matching depth & class & type */
	for ( i=0 ; i < vp->num; i++ ) {
		if ( (vp->flag)[i] != type ) continue;
		if ( class && ((vp->visualList)[i].class != *class) ) continue;
		if ( (vp->visualList)[i].depth == depth )
			return depth;
	}

    /* Else get what we can.  This returns 0 if none is found. */
	return _SG_getDefaultDepth (dpy, scr, class, type);

} /* END OF FUNCTION _SG_getMatchingDepth */


/*
 *  Check that the passed type actually exists.
 *  If not, return the default type.
 *
 *  Arguments:		Default Values
 *  =================	=====================
 *	dpy		none -- must be passed
 *	scr		none -- must be passed
 *	class		NULL	Don't care
 *	type		NULL	Normal (main) planes
 *      typeP           none -- must point to a valid address for return value
 *
 *  Errors returned:	SG_BAD_DISPLAY	no dpy argument supplied
 *
 *  Known Bugs:		None
 *
 *  Things To Do Later:	Nothing
 *
 */

_SG_STORAGE int
_SG_getMatchingType (dpy, scr, class, type, typeP)
	Display  *dpy;
	int	  *class, scr, type;
	char	**typeP;
{
	VisInfoPtr vp;
	register int i;
	int underlay=0, normal=0, overlay=0, popup=0;

    /* Process the calling parameters.  Set up any needed defaults. */
 	if (!dpy)   return ((int)SG_BAD_DISPLAY); /* Cannot default dpy*/
	vp = _SG_getVisualList (dpy, scr);	  /* list of visuals */

    /* You won't EVER get a match on type==0, so default is all we can do */
	if (!type) {
		*typeP = vp->defaultVisualTypeS[vp->defaultVisualType];
		return vp->defaultVisualType;
	}

    /* Search the list for the matching type & class */
	for ( i=0 ; i < vp->num; i++ ) {
		if (vp->flag[i] == type)
			if ( !class || ((vp->visualList)[i].class == *class)) {
				return type;
		}
		switch (vp->flag[i]) {
			case SG_UNDERLAY_PLANES:
				underlay++;
				break;
			case SG_NORMAL_PLANES:
				normal++;
				break;
			case SG_OVERLAY_PLANES:
				overlay++;
				break;
			case SG_POPUP_PLANES:
				popup++;
				break;
		}
	}

    /* Else return the default */
	switch (type) {
		case SG_UNDERLAY_PLANES:
			*typeP = "NORMAL";
			return SG_NORMAL_PLANES;
			/* NOTREACHED */
			break;
		case SG_NORMAL_PLANES:		/* This shouldn't ever happen */
			*typeP = vp->defaultVisualTypeS[vp->defaultVisualType];
			return vp->defaultVisualType;
			/* NOTREACHED */
			break;
		case SG_OVERLAY_PLANES:
			if (popup) {
				*typeP = "POPUP";
				return SG_POPUP_PLANES;
			} else {
				*typeP = "NORMAL";
				return SG_NORMAL_PLANES;
			}
			/* NOTREACHED */
			break;
		case SG_POPUP_PLANES:
			*typeP = "NORMAL";
			return SG_NORMAL_PLANES;
			/* NOTREACHED */
			break;
		default:
			*typeP = vp->defaultVisualTypeS[vp->defaultVisualType];
			return vp->defaultVisualType;
			/* NOTREACHED */
			break;
	}
} /* END OF FUNCTION _SG_getMatchingType */



/*
 *  Get the visual information for the specified visual.
 *  This is a basic low-level routine so that the caller can get to ALL of the
 *  info.
 *
 *  Arguments:		Default Values
 *  =================	=====================
 *	dpy		none -- must be passed
 *	scr		none -- must be passed
 *	vsl		0	don't care
 *	class		NULL	don't care
 *	type		0	don't care
 *	depth		0	don't care
 *
 *  Errors returned:	SG_BAD_DISPLAY	   no dpy argument supplied
 *			SG_NO_SUCH_VISUAL  couldn't find a matching visual
 *
 *  Known Bugs:		None
 *
 *  Things To Do Later:	Nothing
 *
 */

_SG_STORAGE XVisualInfo *
_SG_getMatchingVisual (dpy, scr, vsl, class, type, depth)
	Display  *dpy;
	int	  *class, depth, scr, type;
	VisualID  vsl;

{
	VisInfoPtr vp;
	register int i;

    /* Process the calling parameters.  Set up any needed defaults. */
 	if (!dpy)   return ((XVisualInfo *)SG_BAD_DISPLAY); /*Can't dflt dpy*/
	vp = _SG_getVisualList (dpy, scr);	  /* list of visuals */

    /* Search the list for a matching visual */
	for ( i=0 ; i < vp->num; i++ ) {
		if (  ( !vsl   || (vp->visualList)[i].visualid == vsl )
		   && ( !class || (vp->visualList)[i].class == *class )
		   && ( !depth || (vp->visualList)[i].depth == depth )
		   && ( !type  || (vp->flag[i] == type ) )
		   ) {
			return &(vp->visualList[i]);
		}
	}

    /* No match was found */
	return (XVisualInfo *) SG_NO_SUCH_VISUAL;
} /* END OF FUNCTION _SG_getMatchingVisual */



/*
 *  Get the maximum available depth for the given visual class and type.
 *
 *  Arguments:		Default Values
 *  =================	=====================
 *	dpy		none -- must be passed
 *	scr		none -- must be passed
 *	class		NULL	Not checked
 *	type		NULL	Default planes
 *
 *  Errors returned:	SG_BAD_DISPLAY	no dpy argument supplied
 *
 *  Known Bugs:		None
 *
 *  Things To Do Later:	Nothing
 *
 */

_SG_STORAGE int
_SG_getMaxDepth (dpy, scr, class, type)
	Display  *dpy;
	int       scr, *class, type;
{
	VisInfoPtr vp;
	register int i, depth=0;

    /* Process the calling parameters.  Set up any needed defaults. */
 	if (!dpy)   return ((int)SG_BAD_DISPLAY); /* Cannot default dpy*/

	vp = _SG_getVisualList (dpy, scr);	  /* list of visuals */
	if (!type) type = vp->defaultVisualType;

    /* Search the list for the greatest depth */
	for ( i=0 ; i < vp->num; i++ ) {
		if ( class && ((vp->visualList)[i].class  != *class) )
			continue;
		if ((vp->flag)[i] != type )
			continue;
		if ( (vp->visualList)[i].depth > depth )
			depth = (vp->visualList)[i].depth;
	}
	return depth;
} /* END OF FUNCTION _SG_getMaxDepth */


/*  Convenience routine to get visual args for the normal planes.
 *
 *  NOTE: we get the deepest normal visual available, just to demonstrate the
 *	 capability.  In fact, we suggest you stick to an 8-bit visual if you
 *	 are using normal planes (i.e. neither popup nor overlay), because
 *	 that will help to minimize colormap flashing.
 */

_SG_STORAGE int 
_SG_getNormalArgs(dpy, scr, args, n)
	Display   *dpy;
	int       scr;
	ArgList   args;
	int       *n;
{
	register Colormap  colormap;
	register int       depth;
	register Visual   *visual;

	depth = _SG_getDefaultDepth (dpy, scr, &PColor, SG_NORMAL_PLANES);
	visual = _SG_getVisual(dpy,scr, depth, &PColor, SG_NORMAL_PLANES);
	if ((ptrdiff_t)visual<0)
		return ((int)(ptrdiff_t)visual);
	colormap = _SG_getDefaultColormap (dpy, scr, visual);

	XtSetArg(args[*n], XtNcolormap, colormap); (*n)++;
	XtSetArg(args[*n], XtNdepth, depth); (*n)++;
	XtSetArg(args[*n], XtNvisual, visual); (*n)++;

	return (depth);	/* something more meaningful later? */
} /* END OF FUNCTION _SG_getNormalArgs */


/*
 *  Convenience routine to get visual args for the overlay
 *  planes (default depth).
 */
_SG_STORAGE int 
_SG_getOverlayArgs(dpy, scr, args, n)
	Display   *dpy;
	int       scr;
	ArgList   args;
	int       *n;
{
	register Colormap  colormap;
	register int        depth;
	register Visual   *visual;

	depth = _SG_getDefaultDepth (dpy, scr, &PColor, SG_OVERLAY_PLANES);
	visual = _SG_getVisual(dpy,scr, depth, &PColor, SG_OVERLAY_PLANES);
/*	if ((ptrdiff_t)visual<0) return (_SG_getPopupArgs(dpy, scr, args, n));
 */
	if ((ptrdiff_t)visual<0) return (_SG_getNormalArgs(dpy, scr, args, n));
	colormap = _SG_getDefaultColormap (dpy, scr, visual);

	XtSetArg(args[*n], XtNcolormap, colormap); (*n)++;
	XtSetArg(args[*n], XtNdepth, depth); (*n)++;
	XtSetArg(args[*n], XtNvisual, visual); (*n)++;

	return (depth);		/* something more meaningful later? */
} /* END OF FUNCTION _SG_getOverlayArgs */



/*  
 * Convenience routine to get visual args for the popup planes for
 * Inventor pulldown menus. This will try to get the POPUP
 * first then try the OVERLAY second.
 */
_SG_STORAGE int 
iv_SG_getPopupArgs(dpy, scr, args, n)
	ArgList   args;
	int       scr;
	Display   *dpy;
	int       *n;
{
    register Colormap  colormap;
    register int       depth;
    register Visual   *visual;
    
    /* POPUP first */
    depth = _SG_getDefaultDepth(dpy, scr, &PColor, SG_POPUP_PLANES);
    visual = _SG_getVisual(dpy, scr, depth, &PColor, SG_POPUP_PLANES);
    
    /* OVERLAY if it fails */
    if ((ptrdiff_t)visual < 0) {
	depth = _SG_getDefaultDepth(dpy, scr, &PColor, SG_OVERLAY_PLANES);
	visual = _SG_getVisual(dpy, scr, depth, &PColor, SG_OVERLAY_PLANES);
    }
    
    /* else REGULAR */
    if ((ptrdiff_t)visual < 0)
	return(_SG_getNormalArgs(dpy, scr, args, n));
    
    colormap = _SG_getDefaultColormap(dpy, scr, visual);
    
    XtSetArg(args[*n], XtNcolormap, colormap); (*n)++;
    XtSetArg(args[*n], XtNdepth, depth); (*n)++;
    XtSetArg(args[*n], XtNvisual, visual); (*n)++;
    
    return (depth);		/* something more meaningful later? */
} /* END OF FUNCTION iv_SG_getPopupArgs */


/*
 *  Convenience routine to get visual args for the underlay planes.
 *
 * NOTE: THE CURENT SGI X SERVER DOES NOT SUPPORT UNDERLAY PLANES
 */
_SG_STORAGE int 
_SG_getUnderlayArgs(dpy, scr, args, n)
	Display   *dpy;
	int       scr;
	ArgList   args;
	int       *n;
{
	register Colormap  colormap;
	register int        depth;
	register Visual   *visual;

	depth = _SG_getDefaultDepth (dpy, scr, &PColor, SG_UNDERLAY_PLANES);
	visual = _SG_getVisual(dpy,scr,depth, &PColor, SG_UNDERLAY_PLANES);
	if ((ptrdiff_t)visual<0) return (_SG_getNormalArgs(dpy, scr, args, n));
	colormap = _SG_getDefaultColormap (dpy, scr, visual);

	XtSetArg(args[*n], XtNcolormap, colormap); (*n)++;
	XtSetArg(args[*n], XtNdepth, depth); (*n)++;
	XtSetArg(args[*n], XtNvisual, visual); (*n)++;

	return (depth);		/* something more meaningful later? */
} /* END OF FUNCTION _SG_getUnderlayArgs */


/*
 *  _SG_getVisual -- Routine to find the precise visual asked for.
 *
 *  Arguments:		Default Values
 *  =================	=====================
 *	dpy		none -- must be passed
 *	scr		none -- must be passed
 *	depth		NULL	DefaultDepth(display, scr) or MaxDepth
 *	v_class		NULL	Don't care
 *	v_type		NULL	Default planes
 *
 *  Errors returned:	SG_BAD_DISPLAY	no dpy argument supplied
 *
 *  Known Bugs:		None
 *
 *  Things To Do Later:	Nothing
 */

_SG_STORAGE Visual *
_SG_getVisual (dpy, scr, depth, v_class, v_type)
    Display	*dpy;
    int		 scr, depth, *v_class, v_type;
{
	VisInfoPtr vp;
	register int i;

  /* Process the calling parameters.  Set up any needed defaults. */
 	if (!dpy) return ((Visual *)SG_BAD_DISPLAY); /* Cannot default dpy*/

	vp = _SG_getVisualList (dpy, scr);	     /* list of visuals */
	if (!v_type) v_type = vp->defaultVisualType;
	if (!depth) {
		if ((    !v_class
		     || (*v_class == vp->defaultClass[vp->defaultVisualType])
		    ) && (v_type == vp->defaultVisualType)
		   ) { depth = DefaultDepth(dpy, scr); }
		else
			{ depth = _SG_getMaxDepth (dpy, scr, v_class, v_type);}
	}

    /*
     * Get the list of visuals & search it for a matching one.
     */
	for ( i=0 ; i < vp->num; i++ ) {
		if ((vp->visualList)[i].depth  != depth)
			continue;
		if (v_class && ((vp->visualList)[i].class != *v_class))
			continue;
		if ( (vp->flag)[i] != v_type ) {
		    continue;
		}
		return (vp->visualList)[i].visual;
	}
/*	_SG_warning (dpy, "matching visual NOT found\n", "", "", "", "" );
*/
	return ((Visual *)SG_NO_VISUAL);
} /* END OF FUNCTION _SG_getVisual */


/*
 *  Routine to supply the arguments for a toolkit arglist visual environment.
 *  Simple for now -- will be extended as other capabilities become available.
 *
 *  All SGI systems have PUP planes.  To keep things simple for now, just look
 *  for the PUP visual.
 *
 *  Arguments:
 *	dpy	The display is used to find the root window.
 *
 *	args	Must have room for the arguments to be added by this routine.
 *		Currently that means three more arguments.  Possibly more in
 *		the future.
 *
 *	n	Index into the arguments list.  It will be incremented for each
 *		argument added.
 *
 *  Errors returned:
 *	NO_VISUAL	could not get a suitable visual
 *
 *  Known Bugs:
 *
 *  Things To Do Later:
 *
 *	* Extend the calling arguments to specify just what visual is wanted.
 *
 *	* Perhaps we should pass a max number of arguments in, and then check
 *	  it.  That way if we add more arguments later, the application will
 *	  be able to diagnose the problem.
 *
 *	* Meaningful return status?
 */

_SG_STORAGE int 
_SG_getVisualArgs(dpy, scr, depth, class, type, args, n)
    Display           *dpy;
    int		       scr, depth, *class, type;
    ArgList            args;
    int               *n;
{
    register Colormap  colormap;
    register Visual   *visual;

	visual   = _SG_getVisual (dpy, scr, depth, class, type);
	colormap = _SG_getDefaultColormap (dpy, scr, visual);

	XtSetArg(args[*n], XtNcolormap, colormap); (*n)++;
	XtSetArg(args[*n], XtNdepth, depth); (*n)++;
	XtSetArg(args[*n], XtNvisual, visual); (*n)++;

	return (depth);	/* something more meaningful later? */
} /* END OF FUNCTION _SG_getVisualArgs */


/*
 *  Get, and cache, the list of visuals for this display & screen.
 *
 *  vPtr points to a linked list.  Each item on the list contains details of
 *  of the visual information for a particular display/screen combination.
 *
 *  Arguments:		Default Values
 *  =================	=====================
 *	dpy		none -- must be passed
 *	scr		none -- must be passed
 */

_SG_STORAGE VisInfoPtr
_SG_getVisualList(dpy, scr)
	Display *dpy;
	int scr;
{

	static VisInfoPtr	vPtr=NULL;	/* ==> cache of visuals */
	VisInfoPtr		vp, vtmp;
	XVisualInfo		vTemplate;	/* Visual values we need */

	Atom actualType;	    /* (Returned) actual property type */
	int actualFmt;		    /* (Returned) actual data type */
	unsigned long bytesAfter;   /* (Returned) number of unread bytes */
	int rValue;		    /* return value */

    /*  "dpy" cannot be defaulted. */
	if (!dpy) return ((VisInfoPtr)SG_BAD_DISPLAY);	/* Cannot default dpy */


    /*  If there is a list, try to find a matching block */
	if (vPtr) {
		for (vp=vPtr; vp; vp=vp->next) {
			if ( (vp->dpy == dpy) && (vp->scr == scr) ) {
				return vp;
			}
			if (!vp->next) break;
		}
	}

    /*
     *  Either an empty list, or else no match.  In either case, malloc a
     *  new block, fill it in, and add it to the front of the list.
     */
	vtmp = (VisInfoPtr)XtCalloc(sizeof(VisInfo),1);	/* Get a new block */
	vtmp->dpy = dpy;
	vtmp->scr = scr;

    /*
     * Get the SERVER_OVERLAY_VISUALS property from the root window.
     * This property is the only way we have to know which visuals are
     * overlay visuals.
     */

	rValue = XGetWindowProperty(dpy, RootWindow(dpy, scr),
			XInternAtom(dpy, "SERVER_OVERLAY_VISUALS", FALSE), 0L,
	        	(long)1000000, FALSE, AnyPropertyType, &actualType,
			&actualFmt, &vtmp->nOverlay, &bytesAfter,
			(unsigned char **)&vtmp->pOverlay);

    /*
     * Ensure that we really got the property.
     * If no property, we don't know how to tell what is an overlay.
     */
        if (   (rValue != Success)
            || (actualType == None)
            || (actualFmt != 32)
            || (vtmp->nOverlay < 4)
           ) {

    /* SERVER_OVERLAY_VISUALS does not exist or it is an invalid type */
		vtmp->nOverlay = 0;
		vtmp->pOverlay = 0;
        } else {
                vtmp->nOverlay /= (sizeof (struct overlayData)/4);
	}

	vTemplate.screen = scr;
    /* Get list of visuals */
	vtmp->visualList =
		XGetVisualInfo(dpy, VisualScreenMask, &vTemplate, &vtmp->num);

    /* Get array to hold colormaps as allocated (one per visual) */
	vtmp->colormap = (Colormap *)XtCalloc((Cardinal)sizeof(Colormap)*vtmp->num,1);

    /* Get array to hold bit-coded flags (one per visual) */
	vtmp->flag = (int *)XtCalloc((Cardinal)sizeof(int)*vtmp->num,1);

    /* Fill in the type strings and values */
	vtmp->defaultVisualTypeS[SG_UNDERLAY_PLANES] = "UNDERLAY";
	vtmp->defaultVisualTypeS[SG_NORMAL_PLANES] = "NORMAL";
	vtmp->defaultVisualTypeS[SG_OVERLAY_PLANES] = "OVERLAY";
	vtmp->defaultVisualTypeS[SG_POPUP_PLANES] = "POPUP";
	
    /*
     *  Derive the flags for each visual.
     *  For now, this is just the visual type (e.g. SG_NORMAL_PLANES).
     */

	{
		register int i, j;
		int vis_depth_type = 0;
		int depth, type;

		for (i=0; i<vtmp->num; i++) {		/* For each visual */

		    /*
		     * Decide the visual type for each visual.
		     *
		     * NOTE: even though hardware supports them, X11 doesn't
		     * deal in underlays.  If it ever does, this code needs to
		     * be augmented to recognize them.
		     */
			if ( !vtmp->nOverlay ) {
				/* No overlays at all */
				vtmp->flag[i] = SG_NORMAL_PLANES;
			} else {
				for (j=0; j<vtmp->nOverlay; j++) {
				    /* If visual is on overlay visuals list */
				    if ( (vtmp->visualList)[i].visualid ==
					  (vtmp->pOverlay)[j].overlay_visual ) {
					/* If HEURISTIC is popup visual */
					if (
				   ((vtmp->visualList)[i].depth == 2)
				&& ((vtmp->visualList)[i].class == PseudoColor)
				&& !vis_depth_type
					   ) {
						vtmp->flag[i]=SG_POPUP_PLANES;
						vis_depth_type ++;
					 } else {
						vtmp->flag[i]=SG_OVERLAY_PLANES;
					 }
					 break;
				    }
				}
				if (j == vtmp->nOverlay) {
				    /* Not on visuals list -- must be normal */
				    vtmp->flag[i] = SG_NORMAL_PLANES;
				}
			}

		    /*
		     * Keep track of the default visual information as we go.
		     * This will be used for handling the caller's defaults.
		     */
			type = (vtmp->flag)[i];
			if (    DefaultVisual(dpy,scr)
			    == (vtmp->visualList)[i].visual
			   ) {

				vtmp->defaultVisualType = type;
				vtmp->defaultVisualP[type] =
					&(vtmp->visualList)[i];
				vtmp->defaultClass[type] =
					(vtmp->visualList)[i].class;
				vtmp->defaultDepth[type] =
					DefaultDepth(dpy, scr);

			/*
			 * If this is not the known default, assume that the
			 * default for this type corresponds to maximum depth.
			 */
/** DEBUG: Is this right?  Perhaps we should assume the deepest PseudoColor, **/
/** DEBUG: or else the deepest visual type that matches the default one.  **/
			} else if (vtmp->defaultVisualType!=(vtmp->flag)[i]) {
				depth = (vtmp->visualList)[i].depth;
				if (depth > vtmp->defaultDepth[type]) {
					vtmp->defaultDepth[type] = depth;
					vtmp->defaultVisualP[type] =
						&(vtmp->visualList)[i];
					vtmp->defaultClass[type] =
						(vtmp->visualList)[i].class;
				}
			}
		}
	}


    /*
     *  Add the block to the front of our list.  This is based on the
     *  assumption that it is the most likely to be needed again soon.
     */
	vtmp->next = vPtr ? vPtr : (VisInfoPtr)NULL;
	vPtr = vtmp;
	return vPtr;
} /* END OF FUNCTION _SG_getVisualList */



/*
 *  Print a warning message to stderr.  This is used the same way as the
 *  mwm function MWarning.  We provide it here for other programs to use.
 *
 *  Arguments:		Meaning
 *  =================	=====================
 *	format		fprintf format string
 *	dpy		display (used to find the application name and class)
 *	p{1,2,3,4}	fprintf parameters/values
 *
 *  BUGS:
 *	* This routine core dumps in XtGetApplicationNameAndClass() if the
 *	  program it is linked to is not an Xt program (e.g. Joel's improved
 *	  xshowcmap).  Some possible solutions are:
 *
 *		* Behave differently for Xt and non-Xt programs:
 *			* If it is Xt program, QBoolean and XtQAtom should
 *			  be non-zero
 *			* Add a call (or external char* variables SG_progName
 *			  and SG_progClass), allowing non-Xt programs to set
 *			  program's name and class.
 *			* only call the Xt stuff if it is an Xt progrma.
 *			  Otherwise, if name not yet set, use a lesser message.
 *		* ifdef to never call Xt (we default it to call Xt, though)
 */

_SG_STORAGE void
#ifdef _NO_PROTO
_SG_warning (dpy, format, p1, p2, p3, p4)
    Display *dpy;
    char *format;
    char *p1, *p2, *p3, *p4;
#else /* _NO_PROTO */
_SG_warning (Display *dpy, char *format, char *p1, char *p2, char *p3, char *p4)
#endif /* _NO_PROTO */
{

    char pch[MAXPATHLEN];
    String name="UNKNOWN PROGRAM", class="UNKNOWN CLASS";

    XtGetApplicationNameAndClass(dpy, &name, &class);

    sprintf (pch, format, p1, p2, p3, p4);
    fprintf (stderr, "%s (%s): %s\n", name, class, pch);
    fflush (stderr);

} /* END OF FUNCTION _SG_warning */

/***** SGI INTERNAL ONLY *****
 **
 **  Following code is not included in the overlay demos.
 **  This code is for internal use, only (e.g. mwm, Toolchest, 4Dwm).
 **/


/*******************************************************************************
 * Routine to:
 *
 *	* check the "SG_visualType" and "SG_visualDepth" resources.
 *	* Do the sensible thing for default cases.
 *	* get associated consistent visual attributes.
 *
 * This routine handles any default values.
 * It also handles value conflicts, and resource settings that don't exist
 * on the server in use.
 *
 *  Arguments:		Legal Values
 *  =================	=====================
 *  dpy			must be passed
 *  scr			must be passed
 *  requestedClass	==> PseudoColor (etc)
 *			(NULL pointer == default)
 *			(return) set to class actually found
 *  requestedType	"underlay", "normal", "overlay", "popup", "default".
 *				"" == default, 0 == default
 *  requestedDepth	==> integer bitplane depth wanted.
 *			(NULL pointer == default)
 *			(return) set to depth actually found
 *
 *  requestedTypeV	(return) type value --  e.g. SG_OVERLAY_PLANES
 *  requestedVisual	(return) corresponding visual
 *  requestedColormap	(return) corresponding colormap
 *  requestedDrawable	(return) corresponding drawable
 *
 * Any pointer may be NULL.  If a return value pointer is null, the
 * corresponding value will not be returned.
 *
 * To do:		Nothing
 *
 */

_SG_STORAGE int
#ifdef _NO_PROTO
_SG_defaultDepthAndTypeResources
    (dpy, scr, requestedClass, requestedType, requestedTypeV,
     requestedDepth, requestedVisual, requestedColormap, requestedDrawable)
	Display		*dpy;
	int		 scr;
	int		*requestedClass;
	char		*requestedType;
	int		*requestedTypeV;
	int		*requestedDepth;
	Visual		**requestedVisual;
	Colormap	*requestedColormap;
	Drawable	*requestedDrawable;

#else /* _NO_PROTO */
_SG_defaultDepthAndTypeResources (
	Display		*dpy,
	int		 scr,
	int		*requestedClass,
	char		*requestedType,
	int		*requestedTypeV,
	int		*requestedDepth,
	Visual		**requestedVisual,
	Colormap	*requestedColormap,
	Drawable	*requestedDrawable)
#endif /* _NO_PROTO */
{
	int depth, typeV;
	int userClass = (-1);	/* zero is a valid value, not a flag */
	int userDepth=0, userTypeV=0;/* dummies for null ptrs */
	int returnStatus = 0;
	char *actualType="";
	Boolean requestedTypeWasNull = FALSE;
	Boolean requestedClassWasNull = FALSE;
	Boolean requestedDepthWasNull = FALSE;

	Boolean forceDefaultValues = FALSE;
	Boolean forceWarningMessage = FALSE;
	VisInfoPtr vp;

    /*
     * Set up default values and flags.
     *
     * Allow for any null pointers the caller could have passed.
     * Saves work (and bugs?) later.
     */
	if (!dpy) return SG_BAD_DISPLAY;	/* Cannot default dpy */

	if (!requestedClass) {
		requestedClassWasNull = TRUE;
		requestedClass = &userClass;
	}
	if (!requestedType) {
		requestedTypeWasNull  = TRUE;
		requestedType="";
	}
	if (!requestedDepth) {
		requestedDepthWasNull = TRUE;
		requestedDepth = &userDepth;
	}
	if (!requestedTypeV) requestedTypeV = &userTypeV;

    /*
     * If requestedVisual is null (i.e. we are not returning a visual),
     * then requestedColormap and requestedDrawable must also be.
     * We cannot return them without generating a visual.
     */
	if (!requestedVisual) {
		requestedColormap = requestedDrawable = 0;
	}

    /*
     * Find out about the visuals available for this display and screen.
     * All of the ensuing code depends on this list.
     */
	vp = _SG_getVisualList (dpy, scr);	  /* list of visuals */

    /*
     * If all resources are defaulted (presumably a common case), it is a
     * no-brainer.  Just return default everything.
     */
	if (    (   requestedTypeWasNull
	         || (*requestedType == vp->defaultVisualType)
	   ) && (   requestedClassWasNull
	         ||(*requestedClass == vp->defaultClass[vp->defaultVisualType])
	   ) && (   requestedDepthWasNull
	         ||(*requestedDepth == vp->defaultDepth[vp->defaultVisualType])
	   ) ) {
		*requestedDepth = DefaultDepth (dpy, scr);
		setNonNull (requestedTypeV, SG_NORMAL_PLANES);
		setNonNull (requestedDrawable, RootWindow (dpy, scr));
		setNonNull (requestedVisual, DefaultVisual (dpy, scr));
		setNonNull (requestedColormap, DefaultColormap (dpy, scr));
		return SG_VISUAL_DEFAULT;
	}
	
    /*
     * Ensure that the caller's type is all alphabetic.  This checks for
     * trailing garbage.  It catches the most misleading type of user
     * error -- trailing white space.
     */
	if (*requestedType){
		char *cp;
		for (cp=requestedType; *cp; cp++) {
			if (!isalpha(*cp)) {
				_SG_warning (dpy,
"SG_visualType \"%s\" contains a non-alphabetic character \"%c\".",
					requestedType, (char *)(unsigned long)*cp, "","");
				*cp = '\0';
				_SG_warning (dpy, "Using \"%s\" instead.",
						requestedType, "", "", "");
				break;
			}
		}
	}


    /*
     * Check to see that the type string is one we can deal with.
     */
	if (!*requestedType) requestedType = "DEFAULT";
	actualType = requestedType;		/* Initialization */

	if (!strcasecmp ("DEFAULT", requestedType)) {
		actualType = (vp->defaultVisualTypeS)[vp->defaultVisualType];
		*requestedTypeV = vp->defaultVisualType;

	} else if (!strcasecmp ("UNDERLAY", requestedType)) {
		actualType = (vp->defaultVisualTypeS)[vp->defaultVisualType];
		*requestedTypeV = vp->defaultVisualType;
		forceWarningMessage = TRUE;
		_SG_warning (dpy, "underlay planes not supported from X11",
			"", "", "", "");

	} else if (!strcasecmp ("NORMAL", requestedType)) {
		*requestedTypeV = SG_NORMAL_PLANES;

	} else if (!strcasecmp ("OVERLAY", requestedType)) {
		*requestedTypeV = SG_OVERLAY_PLANES;

	} else if (!strcasecmp ("POPUP", requestedType)) {
		*requestedTypeV = SG_POPUP_PLANES;

	} else {
		actualType = (vp->defaultVisualTypeS)[vp->defaultVisualType];
		*requestedTypeV = vp->defaultVisualType;
		forceWarningMessage = TRUE;
	}

    /*
     * Type is the first thing known -- for our purposes, we need to do
     * something intelligent about a defaulted class.  Defaulting the
     * class can be done more intelligently if we get a better handle on DWIM
     * later on.  The fundamental problem is whether we want a bias towards
     * class, or towards preserving the requested depth if at all possible.
     */

	if (requestedClassWasNull && (*requestedTypeV == vp->defaultVisualType))
	{
		*requestedClass = vp->defaultClass[vp->defaultVisualType];
	}

    /*
     * We now know the visual type the user wants (possibly "DEFAULT").
     * Check that we can get it -- and fall back if it isn't available.
     *
     * NOTE: Visual type is the "top priority" and all following code
     * assumes that it is already set to a valid value.
     *
     *		requestedType		user's string
     *		actualType		string we are giving the user
     *		*requestedTypeV		(int) corresponding to actualType
     */
	{
		int i, underlay=0, normal=0, overlay=0, popup=0, typeFound=0;
	
	    /* Search the visuals list for the matching type & class */
		for ( i=0 ; i < vp->num; i++ ) {
			if (vp->flag[i] == *requestedTypeV) {
				if (*requestedClass<0) break;
				if ((vp->visualList)[i].class== *requestedClass)
					break;
				typeFound++;
			}
			switch (vp->flag[i]) {
				case SG_UNDERLAY_PLANES:
					underlay++; break;
				case SG_NORMAL_PLANES:
					normal++; break;
				case SG_OVERLAY_PLANES:
					overlay++; break;
				case SG_POPUP_PLANES:
					popup++; break;
			}
		}
		if (i == vp->num) {  /* If there is no matching type & class */
		    /* If class specified, we have an error */
			if (typeFound) return SG_NO_TYPE_AND_CLASS;

		    /*
		     * Class not specified, and type not found.
		     * OK to use a fallback type.
		     */
			switch (*requestedTypeV) {
				case SG_UNDERLAY_PLANES:
					actualType = "NORMAL";
					*requestedTypeV = SG_NORMAL_PLANES;
					forceWarningMessage = TRUE;
					break;
				case SG_NORMAL_PLANES:	/* Shouldn't happen */
					actualType = vp->defaultVisualTypeS[vp->defaultVisualType];
					*requestedTypeV = vp->defaultVisualType;
					forceWarningMessage = TRUE;
					break;
				case SG_OVERLAY_PLANES:
					if (popup) {
						actualType = "POPUP";
						*requestedTypeV =
							SG_POPUP_PLANES;
					} else {
						actualType = "NORMAL";
						*requestedTypeV =
							SG_NORMAL_PLANES;
						forceWarningMessage = TRUE;
					}
					break;
				case SG_POPUP_PLANES:
					actualType = "NORMAL";
					*requestedTypeV = SG_NORMAL_PLANES;
					forceWarningMessage = TRUE;
					break;
				default:
					actualType = vp->defaultVisualTypeS[vp->defaultVisualType];
					*requestedTypeV = vp->defaultVisualType;
					forceWarningMessage = TRUE;
					break;
			}
		}
	}

    /*
     * At this point:
     *	actualType     => string: "NORMAL", etc.
     *  requestedTypeV => (int) type value (1-4)
     *  requestedClass => (int) class value -1, (0-5)
     *  requestedDepth => either 0 (default) or else not yet verified.
     *
     *  Verify we have a valid combination, and fall back if it is not.
     */

	depth = *requestedDepth;
	typeV = *requestedTypeV;
	do {
		depth = _SG_getMatchingDepth
			(dpy, scr,
			 requestedClassWasNull ? NULL : requestedClass,
			 typeV, *requestedDepth);
		if (!depth) {
			switch (typeV) {
				case SG_DEFAULT_PLANES: /* Nothing left */
					_SG_warning (dpy,
	"Unable to get requested visualType (%s) with visualDepth (%d).",
						requestedType,
						(char *)(unsigned long)*requestedDepth, "","");
					_SG_warning (dpy,
	"Setting visual resources to server default values.", "", "", "", "");
					*requestedDepth = DefaultDepth
						(dpy, scr);
					*requestedClass =
					vp->defaultClass[vp->defaultVisualType];
					setNonNull (requestedTypeV,
						SG_NORMAL_PLANES);
					setNonNull (requestedDrawable,
						RootWindow (dpy, scr));
					setNonNull (requestedVisual,
						DefaultVisual (dpy, scr));
					setNonNull (requestedColormap,
						DefaultColormap (dpy, scr));
					return (SG_NO_VISUAL);
					/* NOTREACHED */
					break;
				case SG_UNDERLAY_PLANES:
					typeV = SG_NORMAL_PLANES;
					actualType = "NORMAL";
					break;
				case SG_NORMAL_PLANES:
					typeV = vp->defaultVisualType;
					actualType = vp->defaultVisualTypeS[vp->defaultVisualType];
					break;
				case SG_OVERLAY_PLANES:
					typeV = SG_POPUP_PLANES;
					actualType = "POPUP";
					break;
				case SG_POPUP_PLANES:
					typeV = SG_NORMAL_PLANES;
					actualType = "NORMAL";
					break;
			}
		}
	} while (!depth);

    /*
     * We now have a corresponding (and valid) type and depth.
     * Print a warning message if we had to change any non-default thing.
     */


	if (    (   !requestedDepthWasNull
		 && (depth != *requestedDepth)
		 && (*requestedDepth != 0) )
	     || ( !requestedTypeWasNull  && (typeV !=  *requestedTypeV) )
	     || ( forceWarningMessage )
	   ) {
		if (!(   requestedDepthWasNull
		      &&(*requestedTypeV == SG_OVERLAY_PLANES)
		      &&(typeV == SG_POPUP_PLANES)
		   )) {
			_SG_warning (dpy,
"Unable to get requested visualType (%s) with visualDepth (%d).",
				requestedType, (char *)(unsigned long)*requestedDepth, "", "");
			_SG_warning (dpy,
			"Using visualType \"%s\" & visualDepth \"%d\" instead.",
				actualType, (char *)(unsigned long)depth, "", "");
		}
	}

    /*
     * Return the type and depth we actually found.
     * This must be after thw above error checking.
     */
	*requestedDepth = depth;
	setNonNull (requestedTypeV, typeV);

    /*
     * Now get the visual, colormap and a drawable to pass around.
     */
	*requestedClass = _SG_getMatchingClass
			(dpy, scr, typeV, depth,
			 requestedClassWasNull ? NULL : requestedClass);
	setNonNull(requestedVisual, _SG_getVisual
			(dpy, scr, depth, requestedClass, typeV));

    /* If got visual successfully, set corresponding colormap & drawable */
	if (requestedVisual && (ptrdiff_t)*requestedVisual >= 0) {
		if (*requestedVisual == DefaultVisual(dpy, scr)) {
			setNonNull (requestedColormap,
				DefaultColormap (dpy, scr));
			returnStatus = SG_VISUAL_DEFAULT;
		} else {
			setNonNull (requestedColormap,
				_SG_getDefaultColormap
					(dpy, scr, *requestedVisual));
		}

		setNonNull (requestedDrawable, XCreatePixmap
				(dpy, RootWindow(dpy, scr), 1, 1, depth));
	}

    /*
     * If we couldn't successfully get a full set of visual information...
     */
	if ( requestedVisual && ((ptrdiff_t)*requestedVisual<=0)) {
		_SG_warning (dpy, "Couldn't get suitable visual",
			"", "", "", "");
		forceDefaultValues++;
	}
	if ( requestedColormap && ((int)*requestedColormap<=0)) {
		_SG_warning (dpy, "Couldn't get a matching colormap",
			"", "", "", "");
		forceDefaultValues++;
	}
	if ( requestedDrawable && ((int)*requestedDrawable<=0)) {
		_SG_warning (dpy, "Couldn't get a matching drawable",
			"", "", "", "");
		forceDefaultValues++;
	}
	if (forceDefaultValues) {
		_SG_warning (dpy,"Setting visual resources to default values.",
			"", "", "", "");
		*requestedDepth     = DefaultDepth (dpy, scr);
		setNonNull (requestedTypeV, SG_NORMAL_PLANES);
		setNonNull (requestedDrawable, RootWindow (dpy, scr));
		setNonNull (requestedVisual, DefaultVisual (dpy, scr));
		setNonNull (requestedColormap, DefaultColormap (dpy, scr));
		return (SG_NO_VISUAL);
	}

	return returnStatus ? returnStatus : SG_VISUAL_SUCCESS;

} /* END OF FUNCTION _SG_defaultDepthAndTypeResources */
