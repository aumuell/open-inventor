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
 |   $Revision: 1.6 $
 |
 |   Classes:
 |      SoText2
 |
 |   Author(s)          : Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */
// Irix 6 implements i18n, so a different version of the source is used here:
#ifdef IRIX_6

#include <GL/gl.h>
#include <Inventor/SbBox.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/bundles/SoMaterialBundle.h>
#include <Inventor/caches/SoCache.h>
#include <Inventor/details/SoTextDetail.h>
#include <Inventor/elements/SoCacheElement.h>
#include <Inventor/elements/SoFontNameElement.h>
#include <Inventor/elements/SoFontSizeElement.h>
#include <Inventor/elements/SoGLCacheContextElement.h>
#include <Inventor/elements/SoGLTextureEnabledElement.h>
#include <Inventor/elements/SoLazyElement.h>
#include <Inventor/elements/SoMaterialBindingElement.h>
#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/elements/SoProjectionMatrixElement.h>
#include <Inventor/elements/SoViewingMatrixElement.h>
#include <Inventor/elements/SoViewportRegionElement.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/misc/SoState.h>
#include <Inventor/nodes/SoText2.h>

// 6.2 requires flclient.h, iconv.h for UTF->UCS conversion
#include <flclient.h>
#include <math.h>
#include <iconv.h>
#include <machine.h>


// An internal class that makes life easier:

// This very specialized cache class is used to cache bitmaps and GL
// display lists containing bitmaps.  It is strange because it doesn't
// use the normal list of elements used to determine validity, etc,
// and knows exactly which elements it depends on.

class SoBitmapFontCache : public SoCache
{
  public:
    // Return a font (either a new one or an old one) that is valid
    // for the given state.
    static SoBitmapFontCache *	getFont(SoState *state, SbBool forRender);

    // Checks to see if this font is valid
    SbBool		isValid(const SoState *state) const;

    // Use this when rendering to decide if this cache is valid (it
    // checks the GL cache context in addition to other elements)
    SbBool		isRenderValid(SoState *state) const;
    
    // Set up for GL rendering:
    void	setupToRender(SoState *state);

    // Returns the amount the current raster position will be advanced
    // after drawing the given unicode character.
    SbVec3f		getCharOffset(char* c);

    // Get the pixel-space bounding box of a given unicode character.
    void		getCharBbox(char* c, SbBox3f &box);

    // Gets the width (in pixels) of specified UCS-2 string 
    float		getWidth(int line);

    // Gets the height of the font, in pixels
    float		getHeight();

    // Draws the current UCS-2 string
    void		drawString(int line);

    // Draws the given unicode character (using GL)
    void		drawCharacter(const char* c);
	
    //Convert string to UCS-2 format, keep a copy in this cache.
    //Use nodeid to know when to reconvert.    
    SbBool	convertToUCS(uint32_t nodeid, const SoMFString& string);
    
    //Returns line of UCS-2 text
    char *	getUCSString(int line)
	{ return (char*)UCSStrings[line];}
	
    int		getNumUCSChars(int line)
	{ return (int)(long)UCSNumChars[line];}
	
  protected:
    // Free up display lists before being deleted
    virtual void	destroy(SoState *state);

  private:
    // Constructor.
    SoBitmapFontCache(SoState *state);

    // Destructor
    virtual ~SoBitmapFontCache();

    // Returns TRUE if this font cache has a display list for the
    // given unicode UCS-2 character.  It will try to build a display list, if it
    // can.
    SbBool	hasDisplayList(const char* c);

    // Renders an entire UCS-2 string by using the GL callList() function.
    void	callLists(const char *string, int length);

    const FLbitmap *getBitmap(unsigned char* c);

    // Static list of all fonts.  OPTIMIZATION:  If there turn out to
    // be applications that use lots of fonts, we could change this
    // list into a dictionary keyed off the font name.
    static SbPList	*fonts;

    int		numChars;  // Number of characters in this font
     
     

    SoGLDisplayList *list;
    
    // Dictionary to point to unicode-character display lists;
    // Keyed by unicode value
    SbDict	*displayListDict;
    
    // Dictionary to point to bitmap; keyed by unicode value.
    SbDict	*bitmapDict;

    // This flag will be true if there is another cache open (if
    // building GL display lists for render caching, that means we
    // can't also build display lists).
    SbBool	otherOpen;
    
    // This indicates the nodeId of the last created cache, so we can
    // know when a new UCS-2 translation is required
    uint32_t	currentNodeId;

    // And some font library stuff:
    static FLcontext	flContext;

//  instead of font number, we use a comma-separated list of font numbers.
    GLubyte* createUniFontList(const char* fontNameList, float fontsize);
    GLubyte* fontNumList;
    SbPList* fontNums;
    
    // char* pointers of UCS-2 strings:
    SbPList	UCSStrings;
    // size of these strings, in UCS-2 characters:
    SbPList	UCSNumChars;

    static iconv_t conversionCode;  // Used for UCS conversion 
    //Routine to release storage of an FLBitmap
    static void freeBitmap(unsigned long, void* value);
    
};


    
SO_NODE_SOURCE(SoText2);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoText2::SoText2()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoText2);

    SO_NODE_ADD_FIELD(string,	(""));
    SO_NODE_ADD_FIELD(spacing,	(1.));
    SO_NODE_ADD_FIELD(justification,	(LEFT));

    // Set up static info for enumerated type field
    SO_NODE_DEFINE_ENUM_VALUE(Justification, LEFT);
    SO_NODE_DEFINE_ENUM_VALUE(Justification, RIGHT);
    SO_NODE_DEFINE_ENUM_VALUE(Justification, CENTER);

    // Set up info in enumerated type field
    SO_NODE_SET_SF_ENUM_TYPE(justification, Justification);

    isBuiltIn = TRUE;
    myFont = NULL;
  
    
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoText2::~SoText2()
//
////////////////////////////////////////////////////////////////////////
{
    if (myFont != NULL) myFont->unref();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Static helper routine; given a vector in object space, a
//    transformation matrix to screen (normalized-device coordinate)
//    space, and a viewportRegion, returns the 2D pixel coordinates of
//    the vector (relative to the origin of the viewport).  This
//    actually returns a 3D vector; the z value is just the NDC z value.
//
// Use: internal, static

static SbVec3f
fromObjectSpace(const SbVec3f &vector, const SbMatrix &matrix,
		const SbViewportRegion &vpr)
//
////////////////////////////////////////////////////////////////////////
{
    // First, transform to NDC (-1 to 1 in viewport)
    SbVec3f ndc;
    matrix.multVecMatrix(vector, ndc);

    // And do the viewport transformation:
    SbVec2s vpSize = vpr.getViewportSizePixels();
    SbVec3f result;
    result[0] = (ndc[0]+1.0)*vpSize[0]/2.0;
    result[1] = (ndc[1]+1.0)*vpSize[1]/2.0;
    // Leave the z coordinate alone
    result[2] = ndc[2];

    return result;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Static helper routine; given 2D pixel coordinates, a
//    transformation matrix to object space, and a viewportRegion,
//    return the object-space point (assuming the 2D pixel is at z
//    coordinate 0).
//
// Use: internal, static

static SbVec3f
toObjectSpace(const SbVec3f &pixel, const SbMatrix &matrix,
	      const SbViewportRegion &vpr)
//
////////////////////////////////////////////////////////////////////////
{
    // Viewport transformation, to normalized device coordinates:
    SbVec2s vpSize = vpr.getViewportSizePixels();
    SbVec3f ndc;
    ndc[0] = pixel[0]*2.0/vpSize[0] - 1.0;
    ndc[1] = pixel[1]*2.0/vpSize[1] - 1.0;
    ndc[2] = pixel[2];

    SbVec3f result;
    matrix.multVecMatrix(ndc, result);

    return result;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    render the text with the current font
//
// Use: protected

void
SoText2::GLRender(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    if (!shouldGLRender(action)) return;

    SoState *state = action->getState();

    state->push();

    // Get a font cache we can pull stuff out of
    if (myFont != NULL) {
	if (!myFont->isRenderValid(state)) {
	    myFont->unref(state);
	    myFont = NULL;
	}
    }
    if (myFont == NULL) {
	myFont = SoBitmapFontCache::getFont(state, TRUE);
	if (myFont == NULL) {
	    state->pop();
	    return;
	}
    }

    //The current text must be translated to UCS, unless this
    //translation has already been done.    
    if (myFont) myFont->convertToUCS(getNodeId(), string);
    
    // Turn off lighting
    SoLazyElement::setLightModel(state,
			     SoLazyElement::BASE_COLOR);
    // Turn off texturing
    SoGLTextureEnabledElement::set(state, FALSE);
    
    myFont->setupToRender(state);

    // Send first color
    SoMaterialBundle mb(action);
    mb.sendFirst();
    
    // Special-case left-justified, single-line text, which we know
    // starts at (0,0,0) in object space, so we can help caching by
    // avoiding getting the projection/view/model matrices:
    if (string.getNum() == 1 && justification.getValue() == LEFT) {
	glRasterPos3f(0,0,0);	    

	myFont->drawString(0);
    }
    // General case:
    else {
	SbMatrix objToScreen;
	objToScreen = SoProjectionMatrixElement::get(state);
	objToScreen =
	    objToScreen.multLeft(SoViewingMatrixElement::get(state));
	objToScreen =
	    objToScreen.multLeft(SoModelMatrixElement::get(state));

	SbMatrix screenToObj = objToScreen.inverse();
	
	SbViewportRegion vpr = SoViewportRegionElement::get(state);

	// The origin of the text on the screen is the object-space point
	// 0,0,0:
	SbVec3f screenOrigin =
	    fromObjectSpace(SbVec3f(0,0,0), objToScreen, vpr);
    
	for (int line = 0; line < string.getNum(); line++) {
	
	    // Starting position of string, based on justification:
	    SbVec3f charPosition = getPixelStringOffset(line) +
		screenOrigin;	    

	    // Transform the screen-space starting position into object
	    // space, and feed that back to the glRasterPos command (which
	    // will turn around and transform it back into screen-space,
	    // but oh well).
	    SbVec3f lineOrigin = toObjectSpace(charPosition, screenToObj,
					       vpr);
	    glRasterPos3fv(&lineOrigin[0]);
	    
	    myFont->drawString(line);
	}
	// Don't auto-cache above, since dependent on camera:
	SoGLCacheContextElement::shouldAutoCache(state,
		SoGLCacheContextElement::DONT_AUTO_CACHE);
    }
    state->pop();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements ray picking
//
// Use: protected

void
SoText2::rayPick(SoRayPickAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // First see if the object is pickable
    if (! shouldRayPick(action))
	return;

    SoState *state = action->getState();

    state->push();

    // Get a font cache we can pull stuff out of
    if (myFont != NULL) {
	if (!myFont->isValid(state)) {
	    myFont->unref();
	    myFont = NULL;
	}
    }
    if (myFont == NULL) {
	myFont = SoBitmapFontCache::getFont(state, FALSE);
	if (myFont == NULL) {
	    state->pop();
	    return;
	}
    }
    
    //The current text must be translated to UCS, unless this
    //translation has already been done.    
    if (myFont) myFont->convertToUCS(getNodeId(), string);
 
    // Tell the action about our current object space
    action->setObjectSpace();

    SbMatrix objToScreen;
    objToScreen = SoProjectionMatrixElement::get(state);
    objToScreen =
	objToScreen.multLeft(SoViewingMatrixElement::get(state));
    objToScreen =
	objToScreen.multLeft(SoModelMatrixElement::get(state));

    SbMatrix screenToObj = objToScreen.inverse();

    SbViewportRegion vpr = SoViewportRegionElement::get(state);
    // font size in pixels
    float fontSize   = SoFontSizeElement::get(state) *
	vpr.getPixelsPerPoint();

    // The origin of the text on the screen is the object-space point
    // 0,0,0:
    SbVec3f screenOrigin =
	fromObjectSpace(SbVec3f(0,0,0), objToScreen, vpr);

    for (int line = 0; line < string.getNum(); line++) {
    	
	int len = myFont->getNumUCSChars(line);

	char* str = myFont->getUCSString(line);
	
	// Intersect against each line of text's bounding box:
	SbBox3f lineBbox, charBbox;

	// Starting position of string, based on justification:
	SbVec3f charPosition = getPixelStringOffset(line) +
	    screenOrigin;

	SbVec3f p0, p1, p2, p3;
	int chr;
	for (chr = 0; chr < len; chr++) {
	    myFont->getCharBbox(str + 2*chr, charBbox);

	    if (!charBbox.isEmpty()) {

		SbVec3f charMin = charBbox.getMin() + charPosition;
		SbVec3f charMax = charBbox.getMax() + charPosition;

		// Extend the line's bounding box by this character's
		// bounding box (both boxes are in screen-space):
		lineBbox.extendBy(charMin);
		lineBbox.extendBy(charMax);

		// Advance to next character...
		charPosition += myFont->getCharOffset(str + 2*chr);
	    }
	}
	// And transform line's box into object space:
	SbVec3f min = lineBbox.getMin();
	SbVec3f max = lineBbox.getMax();

	SbVec3f t;
	t.setValue(min[0], min[1], screenOrigin[2]);
	p0 = toObjectSpace(t, screenToObj, vpr);
	t.setValue(max[0], min[1], screenOrigin[2]);
	p1 = toObjectSpace(t, screenToObj, vpr);
	t.setValue(min[0], max[1], screenOrigin[2]);
	p2 = toObjectSpace(t, screenToObj, vpr);
	t.setValue(max[0], max[1], screenOrigin[2]);
	p3 = toObjectSpace(t, screenToObj, vpr);

	// intersect the two triangles:
	SbVec3f point;
	// Info we get back from the pick that we don't need:
	SbVec3f junk1; SbBool junk2;
	if (action->intersect(p0, p1, p2, point, junk1, junk2) ||
	    action->intersect(p2, p1, p3, point, junk1, junk2)) {

	    SoPickedPoint *pp = action->addIntersection(point);
	    if (pp) {
		SoTextDetail *detail = new SoTextDetail();
		detail->setStringIndex(line);

		// Figure out which character was hit:
		// Transform picked point into screen space:
		SbVec3f screenPoint =
		    fromObjectSpace(pp->getObjectPoint(), objToScreen,
				    vpr);
		// Figure out which character that corresponds to, by
		// adding on the x-offset of each character until we
		// go past the picked point:
		charPosition = getPixelStringOffset(line) +
		    screenOrigin;
		for (chr = 0; chr < len; chr++) {
		    charPosition += 
			myFont->getCharOffset(str + 2*chr);
		    // Assuming left-to-right drawing of characters:
		    if (charPosition[0] >= screenPoint[0]) break;
		}
		    
		detail->setCharacterIndex(chr);

		pp->setDetail(detail, this);
		pp->setMaterialIndex(0);

		// We'll define normal to be object-space 0,0,1:
		pp->setObjectNormal(SbVec3f(0,0,1));
		// And texture coordinates to be zero:
		pp->setObjectTextureCoords(SbVec4f(0,0,0,0));
	    }
	}
    }
    state->pop();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Generates primitives - no-op.
//
// Use: protected

void
SoText2::generatePrimitives(SoAction *)
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Computes bounding box of text. Since 2D text is screen-aligned,
//    we need to look at the current view volume and viewport region.
//
// Use: protected

void
SoText2::computeBBox(SoAction *action, SbBox3f &box, SbVec3f &center)
//
////////////////////////////////////////////////////////////////////////
{
    SoState *state = action->getState();

    state->push();

    // Get a font cache we can pull stuff out of
    if (myFont != NULL) {
	if (!myFont->isValid(state)) {
	    myFont->unref();
	    myFont = NULL;
	}
    }
    if (myFont == NULL) {
	myFont = SoBitmapFontCache::getFont(state, FALSE);
	if (myFont == NULL) {
	    state->pop();
	    return;
	}
    }

    //The current text must be translated to UCS, unless this
    //translation has already been done.    
    if (myFont) myFont->convertToUCS(getNodeId(), string);
    
    SbMatrix objToScreen;
    objToScreen = SoProjectionMatrixElement::get(state);
    objToScreen =
	objToScreen.multLeft(SoViewingMatrixElement::get(state));
    objToScreen =
	objToScreen.multLeft(SoModelMatrixElement::get(state));

    SbMatrix screenToObj = objToScreen.inverse();

    SbViewportRegion vpr = SoViewportRegionElement::get(state);

    // font size in pixels
    float fontSize   = SoFontSizeElement::get(state) *
	vpr.getPixelsPerPoint();

    // The origin of the text on the screen is the object-space point
    // 0,0,0:
    SbVec3f screenOrigin =
	fromObjectSpace(SbVec3f(0,0,0), objToScreen, vpr);

    // Figure out the screen-space bounding box of the characters:
    SbBox3f screenBbox, charBbox;

    for (int line = 0; line < string.getNum(); line++) {
    
	int len = myFont->getNumUCSChars(line);
	char* str = myFont->getUCSString(line);
       
	// Starting position of string, based on justification:
	SbVec3f charPosition = getPixelStringOffset(line) +
	    screenOrigin;	    

	for (int chr = 0; chr < len; chr++) {
	    myFont->getCharBbox(str+2*chr, charBbox);
	    if (!charBbox.isEmpty()) {
		SbVec3f min = charBbox.getMin() + charPosition;
		SbVec3f max = charBbox.getMax() + charPosition;
		screenBbox.extendBy(min);
		screenBbox.extendBy(max);
	    }

	    // And advance...
	    charPosition += myFont->getCharOffset(str+2*chr);
	}
    }
    // Ok, screenBbox now contains the pixel-space extent of the
    // characters.  We'll transform the bounds of that box back into
    // object space and extend the object-space bounding box:
    
    if (!screenBbox.isEmpty()) {
	// Do each of the 4 corners of the screen-space box:
	const SbVec3f &min = screenBbox.getMin();
	const SbVec3f &max = screenBbox.getMax();
	SbVec3f objectPoint, temp;

	temp.setValue(min[0], min[1], screenOrigin[2]);
	objectPoint = toObjectSpace(temp, screenToObj, vpr);
	box.extendBy(objectPoint);

	temp.setValue(max[0], max[1], screenOrigin[2]);
	objectPoint = toObjectSpace(temp, screenToObj, vpr);
	box.extendBy(objectPoint);

	temp.setValue(min[0], max[1], screenOrigin[2]);
	objectPoint = toObjectSpace(temp, screenToObj, vpr);
	box.extendBy(objectPoint);

	temp.setValue(max[0], min[1], screenOrigin[2]);
	objectPoint = toObjectSpace(temp, screenToObj, vpr);
	box.extendBy(objectPoint);

	// Set the center to be the origin, which is the natural "center"
	// of the text, regardless of justification
	center.setValue(0.0, 0.0, 0.0);
    }

    state->pop();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the number of pixels the current text string is offset from the
//    text origin.  Uses latest UCS-string in myFont. 
//
// Use: private

SbVec3f
SoText2::getPixelStringOffset(int line)
//
////////////////////////////////////////////////////////////////////////
{
    SbVec3f result(0,0,0);

    if (justification.getValue() == RIGHT) {
	float width = myFont->getWidth(line);
	result[0] = -width;
    }
    if (justification.getValue() == CENTER) {
	float width = myFont->getWidth(line);
	result[0] = -width/2.0;
    }
    result[1] = -line*myFont->getHeight()*spacing.getValue()*2;

    return result;
}
//////////////////////////////////////////////////////////////////////////////
/////////////////////    SoBitmapFontCache  //////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// Static variables for SoBitmapFontCache
SbPList *SoBitmapFontCache::fonts = NULL;
FLcontext SoBitmapFontCache::flContext;

//The conversion code (from UTF-8 to UCS-2) is static, set only once.
iconv_t SoBitmapFontCache::conversionCode = NULL;


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Convert MFString to UCS string, if necessary.
//
// Use: internal

SbBool
SoBitmapFontCache::convertToUCS(uint32_t nodeid,
			       const SoMFString& strings)			       
//
////////////////////////////////////////////////////////////////////////
{
    if (nodeid == currentNodeId) return TRUE;
    currentNodeId = nodeid;
    
    //delete previously converted UCS string
    int i;
    for (i = 0; i< UCSStrings.getLength(); i++){
	delete [] (char*)UCSStrings[i];
    }
    UCSStrings.truncate(0);
    UCSNumChars.truncate(0);
 
    //make sure conversion code already set:
    if (conversionCode == NULL){
	conversionCode = iconv_open("UCS-2", "UTF-8");	
    }
    
    if ( conversionCode == (iconv_t)-1 ){
#ifdef DEBUG
	SoDebugError::post("SoBitmapFontCache::convertToUCS", 
	    "Invalid UTF-8 to UCS-2 conversion");
#endif /*DEBUG*/
	return FALSE;
    }
        
    //for each line of text, allocate a sufficiently large buffer:
    //An extra two bytes allocated, since glCallLists likes to trample it.
    for (i = 0; i< strings.getNum(); i++){
	UCSStrings[i] = new char[2*strings[i].getLength()+2];

	char* input = (char *)strings[i].getString();
	size_t inbytes = strings[i].getLength();
	size_t outbytes = 2*inbytes+2;
	char* output = (char*)UCSStrings[i];
    
	if ((iconv(conversionCode, &input, &inbytes, &output, &outbytes) == (size_t)-1)){
#ifdef DEBUG
	    SoDebugError::post("SoBitmapFontCache::convertToUCS", 
		"Error converting text to UCS-2");
#endif /*DEBUG*/ 

	}
    
	if (inbytes){
#ifdef DEBUG
	    SoDebugError::post("SoBitmapFontCache::convertToUCS", 
		"Incomplete conversion to UCS-2");
#endif /*DEBUG*/
	    return FALSE;
	}
    
	UCSNumChars[i] = (void*)((2*strings[i].getLength()+2 - outbytes)>>1);

        int j;
        for (j = 0; j < getNumUCSChars(i); j++) {
            char* c = (char*)UCSStrings[i]+j*2;
            DGL_HTON_SHORT(SHORT(c), SHORT(c));
        }
    }
 
    return TRUE;
    
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Get a font cache suitable for using with the given state.
//
// Use: static, internal, public

SoBitmapFontCache *
SoBitmapFontCache::getFont(SoState *state, SbBool forRender)
//
////////////////////////////////////////////////////////////////////////
{
    if (fonts == NULL) {
	// One-time font library initialization
	fonts = new SbPList;
	flContext = flCreateContext(NULL, FL_FONTNAME, NULL,
				  1.0, 1.0);
	if (flContext == NULL) {
#ifdef DEBUG
	    SoDebugError::post("SoText2::getFont",
			       "flCreateContext returned NULL");
#endif
	    return NULL;
	}
	if (flGetCurrentContext() != flContext)
	    flMakeCurrentContext(flContext);
	flSetHint(FL_HINT_MINOUTLINESIZE, 24.0);
    }
    else if (flContext == NULL) return NULL;
    else {
	if (flGetCurrentContext() != flContext)
	    flMakeCurrentContext(flContext);
    }
    
    SoBitmapFontCache *result = NULL;
    for (int i = 0; i < fonts->getLength() && result == NULL; i++) {
	SoBitmapFontCache *fc = (SoBitmapFontCache *)(*fonts)[i];
	if (!fc->fontNumList) continue;
	if (forRender ? fc->isRenderValid(state) : fc->isValid(state)) {
	    result = fc;
	    result->ref();

	}	    
    }
    if (result == NULL) {
	result = new SoBitmapFontCache(state);

    }    
    return result;
}
////////////////////////////////////////////////////////////////////////
//
// Description:
//    Create a list of font numbers from a list of font names
//
// Use: private
GLubyte *
SoBitmapFontCache::createUniFontList(const char* fontNameList, float size) 
//
////////////////////////////////////////////////////////////////////////
{
    char *s, *s1, *ends;
    FLfontNumber fn;
    float mat[2][2];

    mat[0][0] = mat[1][1] = size;
    mat[0][1] = mat[1][0] = 0.0;
    
    //Make a copy of fontNameList so we don't disturb the one we are passed.    
    //Find \n at end of namelist:
    char * nameCopy = new char[strlen(fontNameList)+1];
    strcpy(nameCopy, fontNameList);

    //find the last null in nameCopy.    
    s = ends = (char *)strrchr(nameCopy, '\0');        
    *s = ';';  /* put a guard in the end of string */
    

    s = (char*)nameCopy;
    fontNums = new SbPList;
      
    while (s1 = (char *)strchr(s, ';')) {
       *s1 = (char)NULL;  /* font name is pointed to s */

       if ((fn = flCreateFont((const GLubyte*)s, mat, 0, NULL)) == (FLfontNumber)0) {
#ifdef DEBUG
	    SoDebugError::post("SoBitmapFontCache::createUniFontList", 
		"Cannot create font %s", s);         
#endif          
       }
       else {
	 fontNums->append((void*)(unsigned long)fn);       
       }
       if(s1 == ends) break;
       s = (s1 + 1);  /* move to next font name */
    }
  
    if (fontNums->getLength() == 0 ) return NULL;
    
    // create a comma-separated list of font numbers:
    char *fontList = new char[10*fontNums->getLength()];
    fontList[0] = '\0';
    for (int i = 0; i< fontNums->getLength(); i++ ){
	fn = (FLfontNumber)(long)(*fontNums)[i];
	sprintf(&fontList[strlen(fontList)], "%d,", fn);	
    }
    fontList[strlen(fontList) - 1] = '\0'; // the last ',' is replaced with NULL
    
    delete [] nameCopy;
                                                    
    return (GLubyte *)fontList;
   
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sees if this font is valid.  If it is valid, it also makes it
//    current.
//
// Use: public

SbBool
SoBitmapFontCache::isValid(const SoState *state) const
//
////////////////////////////////////////////////////////////////////////
{
    SbBool result = SoCache::isValid(state);
    
    if (result) {
	if (flGetCurrentContext() != flContext) {
	    flMakeCurrentContext(flContext);
	}
    }
    return result;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Construct a bitmap font cache, given the state and a dummy
//    (empty) list of overridden elements (needed only to pass to the
//    SoCache constructor).
//
// Use: internal, private

SoBitmapFontCache::SoBitmapFontCache(SoState *state) : SoCache(state)
//
////////////////////////////////////////////////////////////////////////
{
    ref();

    list = NULL;

    // Grab all the stuff we'll need to determine our validity from
    // the state.
    SbName fontName = SoFontNameElement::get(state);
    addElement(state->getConstElement(
	SoFontNameElement::getClassStackIndex()));
    if (fontName == SoFontNameElement::getDefault()) {
	fontName = SbName("Utopia-Regular");
    }
    const SbViewportRegion &vpr = SoViewportRegionElement::get(state);
    addElement(state->getConstElement(
	SoViewportRegionElement::getClassStackIndex()));
    float fontSize = SoFontSizeElement::get(state) * vpr.getPixelsPerPoint();
    addElement(state->getConstElement(
	SoFontSizeElement::getClassStackIndex()));

    // Initialize font list
    fontNumList = createUniFontList(fontName.getString(), fontSize);
 

    if (fontNumList == NULL) {
	// Try Utopia-Regular, unless we just did!
	if (fontName != SbName("Utopia-Regular")) {
#ifdef DEBUG
	    SoDebugError::post("SoText2::getFont",
		      "Couldn't find font %s, replacing with Utopia-Regular",
		       fontName.getString());
#endif
	    fontNumList = createUniFontList("Utopia-Regular", fontSize);
	
	}
	if (fontNumList == NULL) {
#ifdef DEBUG
	    SoDebugError::post("SoText3::getFont",
			       "Couldn't find font Utopia-Regular!");
#endif
	    numChars = 0;
	}
    }

    
    numChars = 65536;  // ??? Just do UCS-2
    currentNodeId = 0;
    displayListDict = new SbDict;
    bitmapDict = new SbDict;
    
    fonts->append(this);
}
    
////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.  Clean up-- GL stuff, font library stuff, and
//    internally generated stuff.
//
// Use: internal, private

SoBitmapFontCache::~SoBitmapFontCache()
//
////////////////////////////////////////////////////////////////////////
{
    if (fontNumList) {
	if (flGetCurrentContext() != flContext) {
	    flMakeCurrentContext(flContext);	
	}
	
	//Must free every bitmap in dictionary:
	//this will just apply flFreeBitmap(entry->value)
	
	bitmapDict->applyToAll(freeBitmap);

	// Only destroy the font library font if no other font caches
	// are using the same font identifier:
	// Must go through fontlist and destroy every font that isn't used
	// by any other cache.
	
	SbBool otherFonts = (fonts->getLength() > 1);
	SbDict *otherFontDict;
	if (otherFonts){
	    otherFontDict = new SbDict;
	    //Enter all the other fontnums into the dictionary:
	    for (int i = 0; i< fonts->getLength(); i++) {
		SoBitmapFontCache *t = (SoBitmapFontCache *)(*fonts)[i];
		if ( t == this) continue;	
		for (int j = 0; j< (t->fontNums->getLength()); j++){
		    unsigned long key = (unsigned long)(*(t->fontNums))[j];	       
		    otherFontDict->enter(key, NULL);
		}	
	    }
	}
	// Now destroy any fonts that don't appear in otherFontDict
	for (int i = 0; i < fontNums->getLength(); i++){
	    void *value;
	    if ( !otherFonts || 
		    !otherFontDict->find((unsigned long)(*fontNums)[i], value)){
		flDestroyFont((FLfontNumber)(long)(*fontNums)[i]);
	    }
	}
	if (otherFonts) delete otherFontDict;	
	delete displayListDict;
	delete bitmapDict;
	
	if (fontNumList)	delete [] fontNumList;
	if (fontNums)		delete fontNums;

	fonts->remove(fonts->find(this));
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destroy this cache.  Called by unref(); frees up OpenGL display
//    lists.
//
// Use: protected, virtual

void
SoBitmapFontCache::destroy(SoState *)
//
////////////////////////////////////////////////////////////////////////
{
    // Pass in NULL to unref because this cache may be destroyed
    // from an action _other_ than GLRender:
    if (list) {
	list->unref(NULL);
	list = NULL;
    }
    SoCache::destroy(NULL);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if this cache is valid
//
// Use: internal, public

SbBool
SoBitmapFontCache::isRenderValid(SoState *state) const
//
////////////////////////////////////////////////////////////////////////
{
    if (!list) return isValid(state);
    else
	return (list->getContext() == SoGLCacheContextElement::get(state)
		 && isValid(state));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets up for GL rendering.
//
// Use: internal

void
SoBitmapFontCache::setupToRender(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
    otherOpen = SoCacheElement::anyOpen(state);

    if (!otherOpen && !list) {
	list = new SoGLDisplayList(state,
				   SoGLDisplayList::DISPLAY_LIST,
				   numChars);
	list->ref();
    }
    if (list) {
	// Set correct list base
	glListBase(list->getFirstIndex());
	list->addDependency(state);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if a display lists exists for given character.
//    Tries to build a display list, if it can.
//
// Use: internal

SbBool
SoBitmapFontCache::hasDisplayList(const char* c)
//
////////////////////////////////////////////////////////////////////////
{
    unsigned char *uc = (unsigned char*)c;
    unsigned long key = (uc[0]<<8)|uc[1];
    // If we have one, return TRUE
    void *value = NULL;
    if (displayListDict->find(key, value)) return TRUE;

    // If we don't and we can't build one, return FALSE.
    if (otherOpen) return FALSE;
    
    // Build one:
    glNewList(list->getFirstIndex()+key, GL_COMPILE);
    drawCharacter(c);
    glEndList();
    
    displayListDict->enter(key, value);

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Assuming that there are display lists built for all the
//    UCS-2 characters in given string, render them.
//    string should have already been converted from UTF8 form.
//
// Use: internal

void
SoBitmapFontCache::callLists(const char *string, int len)
//
////////////////////////////////////////////////////////////////////////
{

    glCallLists(len, GL_2_BYTES, (unsigned char*)string);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the pixel-space bounding box of given UCS-2 character.
//
// Use: internal, public

void
SoBitmapFontCache::getCharBbox(char* c, SbBox3f &box)
//
////////////////////////////////////////////////////////////////////////
{
    box.makeEmpty();

    const FLbitmap *bmap = getBitmap((unsigned char*)c);
    if (bmap == NULL) return;
    
    box.extendBy(SbVec3f(-bmap->xorig, -bmap->yorig, 0));
    box.extendBy(SbVec3f(bmap->width - bmap->xorig,
			 bmap->height - bmap->yorig, 0));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the amount the current raster position will be advanced
//    after drawing the given UCS-2 character.
//
// Use: internal, public

SbVec3f
SoBitmapFontCache::getCharOffset(char* c)
//
////////////////////////////////////////////////////////////////////////
{
    const FLbitmap *bmap = getBitmap((unsigned char*)c);
    if (bmap != NULL)
	return SbVec3f(bmap->xmove, bmap->ymove, 0);
    else return SbVec3f(0,0,0);
}
    
////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the width of specified UCS2 string.
//
// Use: internal, public

float
SoBitmapFontCache::getWidth(int line)
//
////////////////////////////////////////////////////////////////////////
{
    float result = 0.0;

    char *str = getUCSString(line);
    for (int i = 0; i < getNumUCSChars(line); i++) {
	const FLbitmap *bmap = getBitmap((unsigned char*)(str+2*i));
	if (bmap != NULL)
	    result += bmap->xmove;
    }
    return result;
}
    
////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the height of given string.
//
// Use: internal, public

float
SoBitmapFontCache::getHeight()
//
////////////////////////////////////////////////////////////////////////
{
    //take height from UCS-2 code for "M"
    const FLbitmap *bmap = getBitmap((unsigned char*)"\000M");
    if (bmap != NULL)
	return bmap->height - bmap->yorig;
    else return 0;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Draws a bitmap, using UCS-2 character
//
// Use: internal public

void
SoBitmapFontCache::drawCharacter(const char* c)
//
////////////////////////////////////////////////////////////////////////
{
    unsigned char *uc = (unsigned char*)c;
    const FLbitmap *bmap = getBitmap(uc);
    
    if (bmap != NULL)
	glBitmap(bmap->width, bmap->height, bmap->xorig, bmap->yorig,
	     bmap->xmove, bmap->ymove, bmap->bitmap);
#ifdef DEBUG
    else SoDebugError::post("SoBitmapFontCache::drawCharacter", 
	"no bitmap for character %d ", uc[0]*256+uc[1]);
#endif	
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Draws a whole string.  Tries to build display lists, if it can.
//    Assumes string is in UCS-2 format.
//
// Use: internal public

void
SoBitmapFontCache::drawString(int line)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool useCallLists = TRUE;
    
    char *str = getUCSString(line);
    unsigned char *ustr = (unsigned char*)str;

    // If there aren't any other caches open, build display lists for
    // the characters we can:
    for (int i = 0; i < getNumUCSChars(line); i++) {
	// See if the font cache already has (or can build) a display
	// list for this character:
	if (!hasDisplayList(str + 2*i)) {
	    useCallLists = FALSE;
	    break;
	}
    }
	
    // if we have display lists for all of the characters, use
    // glCallLists:
    if (useCallLists) {
	callLists(str, getNumUCSChars(line));
    } else {
	// if we don't, draw the string character-by-character, using the
	// display lists we do have:
	for (int i = 0; i < getNumUCSChars(line); i++) {
	    if (!hasDisplayList(str + 2*i)) {
		drawCharacter(str + 2*i);
	    }
	    else glCallList(list->getFirstIndex()+
		((ustr[2*i]<<8) | ustr[2*i+1]));
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns a bitmap.
//
// Use: private

const FLbitmap *
SoBitmapFontCache::getBitmap(unsigned char* c)
//
////////////////////////////////////////////////////////////////////////
{
    if (!fontNumList) return NULL;

    unsigned long key = (unsigned long)(c[0]<<8 | c[1]);
    void* value;
    if(!bitmapDict->find(key, value)){
	value = (void*)flUniGetBitmap(fontNumList, c);
	
#ifdef DEBUG	
	if(value == NULL){
	    SoDebugError::post("SoBitmapFontCache::getBitmap", 
		"Invalid Unicode bitmap for character %d", key);
	}
#endif /*DEBUG*/

	bitmapDict->enter(key, value);
    }

    return (FLbitmap*)value;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    free up a bitmap.
//
// Use: static, private
void 
SoBitmapFontCache::freeBitmap(unsigned long, void* value)
//////////////////////////////////////////////////////////////////////// 
{
    flFreeBitmap((FLbitmap*)value);    
} 

/////////////////////////////////////////////////////////////////////////
#else /*IRIX_6*/
#include <GL/gl.h>
#include <Inventor/SbBox.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/bundles/SoMaterialBundle.h>
#include <Inventor/caches/SoCache.h>
#include <Inventor/details/SoTextDetail.h>
#include <Inventor/elements/SoCacheElement.h>
#include <Inventor/elements/SoFontNameElement.h>
#include <Inventor/elements/SoFontSizeElement.h>
#include <Inventor/elements/SoGLCacheContextElement.h>
#include <Inventor/elements/SoGLTextureEnabledElement.h>
#include <Inventor/elements/SoLightModelElement.h>
#include <Inventor/elements/SoMaterialBindingElement.h>
#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/elements/SoProjectionMatrixElement.h>
#include <Inventor/elements/SoViewingMatrixElement.h>
#include <Inventor/elements/SoViewportRegionElement.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/misc/SoState.h>
#include <Inventor/nodes/SoText2.h>

#include <flclient.h>
#include <math.h>

// An internal class that makes life easier:

// This very specialized cache class is used to cache bitmaps and GL
// display lists containing bitmaps.  It is strange because it doesn't
// use the normal list of elements used to determine validity, etc,
// and knows exactly which elements it depends on.

class SoBitmapFontCache : public SoCache
{
  public:
    // Return a font (either a new one or an old one) that is valid
    // for the given state.
    static SoBitmapFontCache *	getFont(SoState *state, SbBool forRender);

    // Checks to see if this font is valid
    SbBool		isValid(SoState *state) const;

    // Use this when rendering to decide if this cache is valid (it
    // checks the GL cache context in addition to other elements)
    SbBool		isRenderValid(SoState *state) const;
    
    // Set up for GL rendering:
    void	setupToRender(SoState *state);

    // Returns the amount the current raster position will be advanced
    // after drawing the given character.
    SbVec3f		getCharOffset(char c);

    // Get the pixel-space bounding box of a given character.
    void		getCharBbox(char c, SbBox3f &box);

    // Gets the width (in pixels) of the given string
    float		getWidth(const SbString &str);

    // Gets the height of the font, in pixels
    float		getHeight();

    // Draws the given string
    void		drawString(const SbString &string);

    // Draws the given character (using GL)
    void		drawCharacter(char c);

  protected:
    // Free up display lists before being deleted
    virtual void	destroy(SoState *state);

  private:
    // Constructor.
    SoBitmapFontCache(SoState *state);

    // Destructor
    virtual ~SoBitmapFontCache();

    // Returns TRUE if this font cache has a display list for the
    // given character.  It will try to build a display list, if it
    // can.
    SbBool	hasDisplayList(const char c);

    // Renders an entire string by using the GL callList() function.
    void	callLists(const SbString &string);

    const FLbitmap *getBitmap(unsigned char c);

    // Static list of all fonts.  OPTIMIZATION:  If there turn out to
    // be applications that use lots of fonts, we could change this
    // list into a dictionary keyed off the font name.
    static SbPList	*fonts;

    int		numChars;  // Number of characters in this font

    SoGLDisplayList *list;
    SbBool	*listFlags;// Flag for each character-- have we built
			   // GL display list yet?
    FLbitmap	**bitmaps; // Cached bitmaps for each character.  NULL
			   // if bitmap hasn't been fetched yet.
    
    // This flag will be true if there is another cache open (if
    // building GL display lists for render caching, that means we
    // can't also build display lists).
    SbBool	otherOpen;

    // And some font library stuff:
    static FLcontext	flContext;
    FLfontNumber	fontId;
};
    
SO_NODE_SOURCE(SoText2);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoText2::SoText2()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoText2);

    SO_NODE_ADD_FIELD(string,	(""));
    SO_NODE_ADD_FIELD(spacing,	(1.));
    SO_NODE_ADD_FIELD(justification,	(LEFT));

    // Set up static info for enumerated type field
    SO_NODE_DEFINE_ENUM_VALUE(Justification, LEFT);
    SO_NODE_DEFINE_ENUM_VALUE(Justification, RIGHT);
    SO_NODE_DEFINE_ENUM_VALUE(Justification, CENTER);

    // Set up info in enumerated type field
    SO_NODE_SET_SF_ENUM_TYPE(justification, Justification);

    isBuiltIn = TRUE;
    myFont = NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoText2::~SoText2()
//
////////////////////////////////////////////////////////////////////////
{
    if (myFont != NULL) myFont->unref();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Static helper routine; given a vector in object space, a
//    transformation matrix to screen (normalized-device coordinate)
//    space, and a viewportRegion, returns the 2D pixel coordinates of
//    the vector (relative to the origin of the viewport).  This
//    actually returns a 3D vector; the z value is just the NDC z value.
//
// Use: internal, static

static SbVec3f
fromObjectSpace(const SbVec3f &vector, const SbMatrix &matrix,
		const SbViewportRegion &vpr)
//
////////////////////////////////////////////////////////////////////////
{
    // First, transform to NDC (-1 to 1 in viewport)
    SbVec3f ndc;
    matrix.multVecMatrix(vector, ndc);

    // And do the viewport transformation:
    SbVec2s vpSize = vpr.getViewportSizePixels();
    SbVec3f result;
    result[0] = (ndc[0]+1.0)*vpSize[0]/2.0;
    result[1] = (ndc[1]+1.0)*vpSize[1]/2.0;
    // Leave the z coordinate alone
    result[2] = ndc[2];

    return result;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Static helper routine; given 2D pixel coordinates, a
//    transformation matrix to object space, and a viewportRegion,
//    return the object-space point (assuming the 2D pixel is at z
//    coordinate 0).
//
// Use: internal, static

static SbVec3f
toObjectSpace(const SbVec3f &pixel, const SbMatrix &matrix,
	      const SbViewportRegion &vpr)
//
////////////////////////////////////////////////////////////////////////
{
    // Viewport transformation, to normalized device coordinates:
    SbVec2s vpSize = vpr.getViewportSizePixels();
    SbVec3f ndc;
    ndc[0] = pixel[0]*2.0/vpSize[0] - 1.0;
    ndc[1] = pixel[1]*2.0/vpSize[1] - 1.0;
    ndc[2] = pixel[2];

    SbVec3f result;
    matrix.multVecMatrix(ndc, result);

    return result;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    render the text with the current font
//
// Use: protected

void
SoText2::GLRender(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    if (!shouldGLRender(action)) return;

    SoState *state = action->getState();

    state->push();

    // Get a font cache we can pull stuff out of
    if (myFont != NULL) {
	if (!myFont->isRenderValid(state)) {
	    myFont->unref(state);
	    myFont = NULL;
	}
    }
    if (myFont == NULL) {
	myFont = SoBitmapFontCache::getFont(state, TRUE);
	if (myFont == NULL) {
	    state->pop();
	    return;
	}
    }

    // Turn off lighting
    SoLightModelElement::set(state,
			     SoLightModelElement::BASE_COLOR);
    // Turn off texturing
    SoGLTextureEnabledElement::set(state, FALSE);
    
    myFont->setupToRender(state);

    // Send first color
    SoMaterialBundle mb(action);
    mb.sendFirst();
    
    // Special-case left-justified, single-line text, which we know
    // starts at (0,0,0) in object space, so we can help caching by
    // avoiding getting the projection/view/model matrices:
    if (string.getNum() == 1 && justification.getValue() == LEFT) {
	glRasterPos3f(0,0,0);
	myFont->drawString(string[0]);
    }
    // General case:
    else {
	SbMatrix objToScreen;
	objToScreen = SoProjectionMatrixElement::get(state);
	objToScreen =
	    objToScreen.multLeft(SoViewingMatrixElement::get(state));
	objToScreen =
	    objToScreen.multLeft(SoModelMatrixElement::get(state));

	SbMatrix screenToObj = objToScreen.inverse();
	
	SbViewportRegion vpr = SoViewportRegionElement::get(state);

	// The origin of the text on the screen is the object-space point
	// 0,0,0:
	SbVec3f screenOrigin =
	    fromObjectSpace(SbVec3f(0,0,0), objToScreen, vpr);
    
	for (int line = 0; line < string.getNum(); line++) {
	    // Starting position of string, based on justification:
	    SbVec3f charPosition = getPixelStringOffset(line) +
		screenOrigin;

	    const SbString &str = string[line];

	    // Transform the screen-space starting position into object
	    // space, and feed that back to the glRasterPos command (which
	    // will turn around and transform it back into screen-space,
	    // but oh well).
	    SbVec3f lineOrigin = toObjectSpace(charPosition, screenToObj,
					       vpr);
	    glRasterPos3fv(&lineOrigin[0]);
	    
	    myFont->drawString(str);
	}
	// Don't auto-cache above, since dependent on camera:
	SoGLCacheContextElement::shouldAutoCache(state,
		SoGLCacheContextElement::DONT_AUTO_CACHE);
    }
    state->pop();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements ray picking
//
// Use: protected

void
SoText2::rayPick(SoRayPickAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // First see if the object is pickable
    if (! shouldRayPick(action))
	return;

    SoState *state = action->getState();

    state->push();

    // Get a font cache we can pull stuff out of
    if (myFont != NULL) {
	if (!myFont->isValid(state)) {
	    myFont->unref();
	    myFont = NULL;
	}
    }
    if (myFont == NULL) {
	myFont = SoBitmapFontCache::getFont(state, FALSE);
	if (myFont == NULL) {
	    state->pop();
	    return;
	}
    }

    // Tell the action about our current object space
    action->setObjectSpace();

    SbMatrix objToScreen;
    objToScreen = SoProjectionMatrixElement::get(state);
    objToScreen =
	objToScreen.multLeft(SoViewingMatrixElement::get(state));
    objToScreen =
	objToScreen.multLeft(SoModelMatrixElement::get(state));

    SbMatrix screenToObj = objToScreen.inverse();

    SbViewportRegion vpr = SoViewportRegionElement::get(state);
    // font size in pixels
    float fontSize   = SoFontSizeElement::get(state) *
	vpr.getPixelsPerPoint();

    // The origin of the text on the screen is the object-space point
    // 0,0,0:
    SbVec3f screenOrigin =
	fromObjectSpace(SbVec3f(0,0,0), objToScreen, vpr);

    for (int line = 0; line < string.getNum(); line++) {

	// Intersect against each line of text's bounding box:
	SbBox3f lineBbox, charBbox;

	// Starting position of string, based on justification:
	SbVec3f charPosition = getPixelStringOffset(line) +
	    screenOrigin;

	const SbString &str = string[line];
	const char *chars = str.getString();

	SbVec3f p0, p1, p2, p3;
	int chr;
	for (chr = 0; chr < str.getLength(); chr++) {
	    myFont->getCharBbox(chars[chr], charBbox);

	    if (!charBbox.isEmpty()) {

		SbVec3f charMin = charBbox.getMin() + charPosition;
		SbVec3f charMax = charBbox.getMax() + charPosition;

		// Extend the line's bounding box by this character's
		// bounding box (both boxes are in screen-space):
		lineBbox.extendBy(charMin);
		lineBbox.extendBy(charMax);

		// Advance to next character...
		charPosition += myFont->getCharOffset(chars[chr]);
	    }
	}
	// And transform line's box into object space:
	SbVec3f min = lineBbox.getMin();
	SbVec3f max = lineBbox.getMax();

	SbVec3f t;
	t.setValue(min[0], min[1], screenOrigin[2]);
	p0 = toObjectSpace(t, screenToObj, vpr);
	t.setValue(max[0], min[1], screenOrigin[2]);
	p1 = toObjectSpace(t, screenToObj, vpr);
	t.setValue(min[0], max[1], screenOrigin[2]);
	p2 = toObjectSpace(t, screenToObj, vpr);
	t.setValue(max[0], max[1], screenOrigin[2]);
	p3 = toObjectSpace(t, screenToObj, vpr);

	// intersect the two triangles:
	SbVec3f point;
	// Info we get back from the pick that we don't need:
	SbVec3f junk1; SbBool junk2;
	if (action->intersect(p0, p1, p2, point, junk1, junk2) ||
	    action->intersect(p2, p1, p3, point, junk1, junk2)) {

	    SoPickedPoint *pp = action->addIntersection(point);
	    if (pp) {
		SoTextDetail *detail = new SoTextDetail();
		detail->setStringIndex(line);

		// Figure out which character was hit:
		// Transform picked point into screen space:
		SbVec3f screenPoint =
		    fromObjectSpace(pp->getObjectPoint(), objToScreen,
				    vpr);
		// Figure out which character that corresponds to, by
		// adding on the x-offset of each character until we
		// go past the picked point:
		charPosition = getPixelStringOffset(line) +
		    screenOrigin;
		for (chr = 0; chr < str.getLength(); chr++) {
		    charPosition += myFont->getCharOffset(chars[chr]);
		    // Assuming left-to-right drawing of characters:
		    if (charPosition[0] >= screenPoint[0]) break;
		}
		    
		detail->setCharacterIndex(chr);

		pp->setDetail(detail, this);
		pp->setMaterialIndex(0);

		// We'll define normal to be object-space 0,0,1:
		pp->setObjectNormal(SbVec3f(0,0,1));
		// And texture coordinates to be zero:
		pp->setObjectTextureCoords(SbVec4f(0,0,0,0));
	    }
	}
    }
    state->pop();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Generates primitives - no-op.
//
// Use: protected

void
SoText2::generatePrimitives(SoAction *)
//
////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Computes bounding box of text. Since 2D text is screen-aligned,
//    we need to look at the current view volume and viewport region.
//
// Use: protected

void
SoText2::computeBBox(SoAction *action, SbBox3f &box, SbVec3f &center)
//
////////////////////////////////////////////////////////////////////////
{
    SoState *state = action->getState();

    state->push();

    // Get a font cache we can pull stuff out of
    if (myFont != NULL) {
	if (!myFont->isValid(state)) {
	    myFont->unref();
	    myFont = NULL;
	}
    }
    if (myFont == NULL) {
	myFont = SoBitmapFontCache::getFont(state, FALSE);
	if (myFont == NULL) {
	    state->pop();
	    return;
	}
    }

    SbMatrix objToScreen;
    objToScreen = SoProjectionMatrixElement::get(state);
    objToScreen =
	objToScreen.multLeft(SoViewingMatrixElement::get(state));
    objToScreen =
	objToScreen.multLeft(SoModelMatrixElement::get(state));

    SbMatrix screenToObj = objToScreen.inverse();

    SbViewportRegion vpr = SoViewportRegionElement::get(state);

    // font size in pixels
    float fontSize   = SoFontSizeElement::get(state) *
	vpr.getPixelsPerPoint();

    // The origin of the text on the screen is the object-space point
    // 0,0,0:
    SbVec3f screenOrigin =
	fromObjectSpace(SbVec3f(0,0,0), objToScreen, vpr);

    // Figure out the screen-space bounding box of the characters:
    SbBox3f screenBbox, charBbox;

    for (int line = 0; line < string.getNum(); line++) {
	// Starting position of string, based on justification:
	SbVec3f charPosition = getPixelStringOffset(line) +
	    screenOrigin;

	const SbString &str = string[line];
	const char *chars = str.getString();

	for (int chr = 0; chr < str.getLength(); chr++) {
	    myFont->getCharBbox(chars[chr], charBbox);
	    if (!charBbox.isEmpty()) {
		SbVec3f min = charBbox.getMin() + charPosition;
		SbVec3f max = charBbox.getMax() + charPosition;
		screenBbox.extendBy(min);
		screenBbox.extendBy(max);
	    }

	    // And advance...
	    charPosition += myFont->getCharOffset(chars[chr]);
	}
    }
    // Ok, screenBbox now contains the pixel-space extent of the
    // characters.  We'll transform the bounds of that box back into
    // object space and extend the object-space bounding box:
    
    if (!screenBbox.isEmpty()) {
	// Do each of the 4 corners of the screen-space box:
	const SbVec3f &min = screenBbox.getMin();
	const SbVec3f &max = screenBbox.getMax();
	SbVec3f objectPoint, temp;

	temp.setValue(min[0], min[1], screenOrigin[2]);
	objectPoint = toObjectSpace(temp, screenToObj, vpr);
	box.extendBy(objectPoint);

	temp.setValue(max[0], max[1], screenOrigin[2]);
	objectPoint = toObjectSpace(temp, screenToObj, vpr);
	box.extendBy(objectPoint);

	temp.setValue(min[0], max[1], screenOrigin[2]);
	objectPoint = toObjectSpace(temp, screenToObj, vpr);
	box.extendBy(objectPoint);

	temp.setValue(max[0], min[1], screenOrigin[2]);
	objectPoint = toObjectSpace(temp, screenToObj, vpr);
	box.extendBy(objectPoint);

	// Set the center to be the origin, which is the natural "center"
	// of the text, regardless of justification
	center.setValue(0.0, 0.0, 0.0);
    }

    state->pop();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the number of pixels the given line is offset from the
//    text origin.
//
// Use: private

SbVec3f
SoText2::getPixelStringOffset(int line)
//
////////////////////////////////////////////////////////////////////////
{
    SbVec3f result(0,0,0);
    
    if (justification.getValue() == RIGHT) {
	float width = myFont->getWidth(string[line]);
	result[0] = -width;
    }
    if (justification.getValue() == CENTER) {
	float width = myFont->getWidth(string[line]);
	result[0] = -width/2.0;
    }
    result[1] = -line*myFont->getHeight()*spacing.getValue()*2;

    return result;
}

// Static variables for SoBitmapFontCache
SbPList *SoBitmapFontCache::fonts = NULL;
FLcontext SoBitmapFontCache::flContext;

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Get a font cache suitable for using with the given state.
//
// Use: static, internal, public

SoBitmapFontCache *
SoBitmapFontCache::getFont(SoState *state, SbBool forRender)
//
////////////////////////////////////////////////////////////////////////
{
    if (fonts == NULL) {
	// One-time font library initialization
	fonts = new SbPList;
	flContext = flCreateContext(NULL, FL_FONTNAME, NULL,
				  1.0, 1.0);
	if (flContext == NULL) {
#ifdef DEBUG
	    SoDebugError::post("SoText2::getFont",
			       "flCreateContext returned NULL");
#endif
	    return NULL;
	}
	if (flGetCurrentContext() != flContext)
	    flMakeCurrentContext(flContext);
	flSetHint(FL_HINT_MINOUTLINESIZE, 24.0);
    }
    else if (flContext == NULL) return NULL;
    else {
	if (flGetCurrentContext() != flContext)
	    flMakeCurrentContext(flContext);
    }
    
    SoBitmapFontCache *result = NULL;
    for (int i = 0; i < fonts->getLength() && result == NULL; i++) {
	SoBitmapFontCache *fc = (SoBitmapFontCache *)(*fonts)[i];
	if (!fc->fontNumList) continue;
	if (forRender ? fc->isRenderValid(state) : fc->isValid(state)) {
	    result = fc;
	    result->ref();
	    if (flGetCurrentFont() != result->fontId) {
		flMakeCurrentFont(result->fontId);
	    }
	}	    
    }
    if (result == NULL) {
	result = new SoBitmapFontCache(state);

	// If error:
	if (result->fontId == 0) {
	    delete result;
	    return NULL;
	}
    }    
    return result;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sees if this font is valid.  If it is valid, it also makes it
//    current.
//
// Use: public

SbBool
SoBitmapFontCache::isValid(SoState *state) const
//
////////////////////////////////////////////////////////////////////////
{
    SbBool result = SoCache::isValid(state);
    
    if (result) {
	if (flGetCurrentContext() != flContext) {
	    flMakeCurrentContext(flContext);
	    flMakeCurrentFont(fontId);
	}
	else if (flGetCurrentFont() != fontId)
	    flMakeCurrentFont(fontId);
    }
    return result;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Construct a bitmap font cache, given the state and a dummy
//    (empty) list of overridden elements (needed only to pass to the
//    SoCache constructor).
//
// Use: internal, private

SoBitmapFontCache::SoBitmapFontCache(SoState *state) : SoCache(state)
//
////////////////////////////////////////////////////////////////////////
{
    ref();

    list = NULL;

    // Grab all the stuff we'll need to determine our validity from
    // the state.
    SbName fontName = SoFontNameElement::get(state);
    addElement(state->getConstElement(
	SoFontNameElement::getClassStackIndex()));
    if (fontName == SoFontNameElement::getDefault()) {
	fontName = SbName("Utopia-Regular");
    }
    const SbViewportRegion &vpr = SoViewportRegionElement::get(state);
    addElement(state->getConstElement(
	SoViewportRegionElement::getClassStackIndex()));
    float fontSize = SoFontSizeElement::get(state) * vpr.getPixelsPerPoint();
    addElement(state->getConstElement(
	SoFontSizeElement::getClassStackIndex()));

    // Initialize everything
    GLfloat m[2][2];
    m[0][0] = m[1][1] = fontSize;
    m[0][1] = m[1][0] = 0.0;
    fontId = flCreateFont((const GLubyte *)fontName.getString(), m, 0, NULL);

    if (fontId == 0) {
	// Try Utopia-Regular, unless we just did!
	if (fontName != SbName("Utopia-Regular")) {
#ifdef DEBUG
	    SoDebugError::post("SoText2::getFont",
		      "Couldn't find font %s, replacing with Utopia-Regular",
		       fontName.getString());
#endif
	    fontId = flCreateFont((const GLubyte *)"Utopia-Regular",
				  m, 0, NULL);
	}
	if (fontId == 0) {
#ifdef DEBUG
	    SoDebugError::post("SoText3::getFont",
			       "Couldn't find font Utopia-Regular!");
#endif
	    numChars = 0;
	}
    }	
    flMakeCurrentFont(fontId);
    
    numChars = 256;  // ??? JUST DO ASCII FOR NOW!
    listFlags = new SbBool[numChars];
    bitmaps = new FLbitmap*[numChars];
    for (int i = 0; i < numChars; i++) {
	listFlags[i] = FALSE;
	bitmaps[i] = NULL;
    }

    fonts->append(this);
}
    
////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor.  Clean up-- GL stuff, font library stuff, and
//    internally generated stuff.
//
// Use: internal, private

SoBitmapFontCache::~SoBitmapFontCache()
//
////////////////////////////////////////////////////////////////////////
{
    if (fontId) {
	if (flGetCurrentContext() != flContext) {
	    flMakeCurrentContext(flContext);
	    flMakeCurrentFont(fontId);
	}
	else if (flGetCurrentFont() != fontId)
	    flMakeCurrentFont(fontId);

	int i;
	for (i = 0; i < numChars; i++) {
	    if (bitmaps[i] != NULL) flFreeBitmap(bitmaps[i]);
	}

	// Only destroy the font library font if no other font caches
	// are using the same font identifier:
	SbBool otherUsing = FALSE;
	for (i = 0; i < fonts->getLength(); i++) {
	    SoBitmapFontCache *t = (SoBitmapFontCache *)(*fonts)[i];
	    if (t != this && t->fontId == fontId) otherUsing = TRUE;
	}
	if (!otherUsing) {
	    flDestroyFont(fontId);
	}
    
	delete [] listFlags;
	delete [] bitmaps;

	fonts->remove(fonts->find(this));
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destroy this cache.  Called by unref(); frees up OpenGL display
//    lists.
//
// Use: protected, virtual

void
SoBitmapFontCache::destroy(SoState *)
//
////////////////////////////////////////////////////////////////////////
{
    // Pass in NULL to unref because this cache may be destroyed
    // from an action _other_ than GLRender:
    if (list) {
	list->unref(NULL);
	list = NULL;
    }
    SoCache::destroy(NULL);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if this cache is valid
//
// Use: internal, public

SbBool
SoBitmapFontCache::isRenderValid(SoState *state) const
//
////////////////////////////////////////////////////////////////////////
{
    if (!list) return isValid(state);
    else
	return (list->getContext() == SoGLCacheContextElement::get(state)
		 && isValid(state));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets up for GL rendering.
//
// Use: internal

void
SoBitmapFontCache::setupToRender(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
    otherOpen = SoCacheElement::anyOpen(state);

    if (!otherOpen && !list) {
	list = new SoGLDisplayList(state,
				   SoGLDisplayList::DISPLAY_LIST,
				   numChars);
	list->ref();
    }
    if (list) {
	// Set correct list base
	glListBase(list->getFirstIndex());
	list->addDependency(state);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if a display lists exists for given character.
//    Tries to build a display list, if it can.
//
// Use: internal

SbBool
SoBitmapFontCache::hasDisplayList(const char c)
//
////////////////////////////////////////////////////////////////////////
{
    // If we have one, return TRUE
    if (listFlags[c] == TRUE) return TRUE;
    
    // If we don't and we can't build one, return FALSE.
    if (otherOpen) return FALSE;
    
    // Build one:
    glNewList(list->getFirstIndex()+c, GL_COMPILE);
    drawCharacter(c);
    glEndList();
    listFlags[c] = TRUE;

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Assuming that there are display lists built for all the
//    characters in given string, render them using the GL's CallLists
//    routine.
//
// Use: internal

void
SoBitmapFontCache::callLists(const SbString &string)
//
////////////////////////////////////////////////////////////////////////
{
    const char *str = string.getString();

    glCallLists(string.getLength(), GL_UNSIGNED_BYTE, str);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the pixel-space bounding box of given character.
//
// Use: internal, public

void
SoBitmapFontCache::getCharBbox(char c, SbBox3f &box)
//
////////////////////////////////////////////////////////////////////////
{
    box.makeEmpty();

    const FLbitmap *bmap = getBitmap(c);
    if (bmap == NULL) return;
    
    box.extendBy(SbVec3f(-bmap->xorig, -bmap->yorig, 0));
    box.extendBy(SbVec3f(bmap->width - bmap->xorig,
			 bmap->height - bmap->yorig, 0));
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the amount the current raster position will be advanced
//    after drawing the given character.
//
// Use: internal, public

SbVec3f
SoBitmapFontCache::getCharOffset(char c)
//
////////////////////////////////////////////////////////////////////////
{
    const FLbitmap *bmap = getBitmap(c);
    if (bmap != NULL)
	return SbVec3f(bmap->xmove, bmap->ymove, 0);
    else return SbVec3f(0,0,0);
}
    
////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the width of given string.
//
// Use: internal, public

float
SoBitmapFontCache::getWidth(const SbString &str)
//
////////////////////////////////////////////////////////////////////////
{
    float result = 0.0;

    const char *chars = str.getString();
    for (int i = 0; i < str.getLength(); i++) {
	const FLbitmap *bmap = getBitmap(chars[i]);
	if (bmap != NULL)
	    result += bmap->xmove;
    }
    return result;
}
    
////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the height of given string.
//
// Use: internal, public

float
SoBitmapFontCache::getHeight()
//
////////////////////////////////////////////////////////////////////////
{
    const FLbitmap *bmap = getBitmap('M');
    if (bmap != NULL)
	return bmap->height - bmap->yorig;
    else return 0;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Draws a bitmap
//
// Use: internal public

void
SoBitmapFontCache::drawCharacter(char c)
//
////////////////////////////////////////////////////////////////////////
{
    const FLbitmap *bmap = getBitmap(c);
    
    if (bmap != NULL)
	glBitmap(bmap->width, bmap->height, bmap->xorig, bmap->yorig,
	     bmap->xmove, bmap->ymove, bmap->bitmap);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Draws a whole string.  Tries to build display lists, if it can.
//
// Use: internal public

void
SoBitmapFontCache::drawString(const SbString &string)
//
////////////////////////////////////////////////////////////////////////
{
    SbBool useCallLists = TRUE;

    const char *chars = string.getString();

    // If there aren't any other caches open, build display lists for
    // the characters we can:
    for (int i = 0; i < string.getLength(); i++) {
	// See if the font cache already has (or can build) a display
	// list for this character:
	if (!hasDisplayList(chars[i])) {
	    useCallLists = FALSE;
	    break;
	}
    }
	
    // if we have display lists for all of the characters, use
    // glCallLists:
    if (useCallLists) {
	callLists(string);
    } else {
	// if we don't, draw the string character-by-character, using the
	// display lists we do have:
	for (int i = 0; i < string.getLength(); i++) {
	    if (!hasDisplayList(chars[i])) {
		drawCharacter(chars[i]);
	    }
	    else glCallList(list->getFirstIndex()+chars[i]);
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns a bitmap.
//
// Use: private

const FLbitmap *
SoBitmapFontCache::getBitmap(unsigned char c)
//
////////////////////////////////////////////////////////////////////////
{
    if (!fontNumList) return NULL;

    if (bitmaps[c] == NULL) {
	bitmaps[c] = flGetBitmap(fontId, c);
    }
    return bitmaps[c];
}
#endif /*IRIX_6*/
