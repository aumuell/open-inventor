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
 |   $Revision: 1.1 $
 |
 |   Classes:
 |      SoText3
 |
 |
 |   Author(s)          : Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */
//Internationalization changes (i18n) only work with Irix 6
#ifdef IRIX_6
#include <GL/gl.h>
#include <GL/glu.h>
#include <Inventor/SbBox.h>
#include <Inventor/SoPrimitiveVertex.h>
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/bundles/SoMaterialBundle.h>
#include <Inventor/caches/SoCache.h>
#include <Inventor/details/SoTextDetail.h>
#include <Inventor/elements/SoCacheElement.h>
#include <Inventor/elements/SoComplexityElement.h>
#include <Inventor/elements/SoComplexityTypeElement.h>
#include <Inventor/elements/SoCreaseAngleElement.h>
#include <Inventor/elements/SoFontNameElement.h>
#include <Inventor/elements/SoFontSizeElement.h>
#include <Inventor/elements/SoGLCacheContextElement.h>
#include <Inventor/elements/SoGLTextureCoordinateElement.h>
#include <Inventor/elements/SoGLTextureEnabledElement.h>
#include <Inventor/elements/SoMaterialBindingElement.h>
#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/elements/SoProfileElement.h>
#include <Inventor/elements/SoProfileCoordinateElement.h>
#include <Inventor/elements/SoProjectionMatrixElement.h>
#include <Inventor/elements/SoViewportRegionElement.h>
#include <Inventor/elements/SoViewingMatrixElement.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/misc/SoState.h>
#include <Inventor/nodes/SoProfile.h>
#include <Inventor/nodes/SoText3.h>

// Font library:
// Additional for Banyan(6.2):
#include <flclient.h>
#include <iconv.h>

// First, a more convenient structure for outlines:
class SoFontOutline {

  public:
    // Constructor, takes a pointer to the font-library outline
    // structure and the font's size:
    SoFontOutline(FLoutline *outline, float fontSize);
    // Destructor
    ~SoFontOutline();

    // Query routines:
    int		getNumOutlines() { return numOutlines; }
    int		getNumVerts(int i) { return numVerts[i]; }
    SbVec2f	&getVertex(int i, int j) { return verts[i][j]; }
    SbVec2f	getCharAdvance() { return charAdvance; }
    
    static SoFontOutline *getNullOutline();

  private:
    // Internal constructor used by getNullOutline:
    SoFontOutline();
    
    // This basically mimics the FLoutline structure, with the
    // exception that the font size is part of the outline:
    int numOutlines;
    int *numVerts;
    SbVec2f **verts;
    SbVec2f charAdvance;
};

//
// Internal class: SoOutlineFontCache
//

// Callback function for sides of characters-- passed the number of
// points going back, and points and normals on either edge of the
// strip.  tTexCoords[0] and [1] are for the two edges, and the
// sTexCoords are the same for both edges.
typedef void SideCB(int nPoints,
		    const SbVec3f *points1, const SbVec3f *norms1,
		    const SbVec3f *points2, const SbVec3f *norms2,
		    const float *sTexCoords, const float *tTexCoords);

// This is pretty heavyweight-- it is responsible for doing all of the
// grunt work of figuring out the polygons making up the characters in
// the font.
class SoOutlineFontCache : public SoCache
{
  public:
    // Given a state, find an appropriate outline font.
    static SoOutlineFontCache	*getFont(SoState *, SbBool forRender);
    
    // Checks to see if this font is valid
    SbBool	isValid(const SoState *state) const;

    // Figures out if this cache is valid for rendering (the base
    // class isValid can be used for all other actions)
    SbBool	isRenderValid(SoState *state) const;

    // Returns the width of specified line number
    float	getWidth(int line);
    
    // Returns height of font
    float	getHeight() { return fontSize; }

    // Returns the 2D bounding box of a UCS character
    void	getCharBBox(const char* c, SbBox2f &result);
    // ... and the bounding box of the font's bevel
    void	getProfileBBox(SbBox2f &result);
    
    // Return the first/last point in the profile:
    void	getProfileBounds(float &firstZ, float &lastZ);

    // Returns TRUE if there _is_ any profile
    // (if not, act as if SIDES of text are off)
    SbBool	hasProfile() const { return  (nProfileVerts > 1); }

    // Returns how far to advance after drawing given character:
    SbVec2f	getCharOffset(const char* c);
    
    // Uses the given glu tesselator to generate triangles for the
    // given character.  This is used for both rendering and
    // generating primitives, with just different callback routines
    // registered.
    void	generateFrontChar(const char* c, gluTESSELATOR *tobj);
    // Ditto, for sides of characters:
    void	generateSideChar(const char* c, SideCB callbackFunc);

    // Set up for GL rendering:
    void	setupToRenderFront(SoState *state);
    void	setupToRenderSide(SoState *state, SbBool willTexture);

    // Returns TRUE if this font cache has a display list for the
    // given UCS character.  It will try to build a display list, if it
    // can.
    SbBool	hasFrontDisplayList(const char* c, gluTESSELATOR *tobj);
    SbBool	hasSideDisplayList(const char* c, SideCB callbackFunc);

    // Renders an entire line by using the GL callList() function.
    void	callFrontLists(int line);
    void	callSideLists(int line);

    // Renders a  UCS string in cases where display lists can't be buit.
    void	renderFront(int line,   gluTESSELATOR *tobj);
    void	renderSide(int line,  SideCB callbackFunc);

    // Callback registered with GLU used to detect tesselation errors.
    static void errorCB(GLenum whichErr);

    //Convert string to UCS format, keep a copy in this cache.
    //Use nodeid to know when to reconvert.    
    void	convertToUCS(uint32_t nodeid, const SoMFString& string);
    
    //Returns line of UCS-2 text
    char *	getUCSString(int line)
	{ return (char*)UCSStrings[line];}
	
    int		getNumUCSChars(int line)
	{ return (int)(long)UCSNumChars[line];}
	
    GLubyte *SoOutlineFontCache::createUniFontList(const char* fontNameList); 
    
  protected:

    // Free up display lists before being deleted
    virtual void	destroy(SoState *state);

  private:
    // Constructor
    SoOutlineFontCache(SoState *);
    // Destructor
    ~SoOutlineFontCache();

    // Return a convnient little class representing a UCS character's
    // outline.
    SoFontOutline	*getOutline(const char* c);
    
    // Some helper routines for generateSide:
    void figureSegmentNorms(SbVec2f *result, int nPoints,
	 const SbVec2f *points, float cosCreaseAngle, SbBool isClosed);
    void figureSegmentTexCoords(float *texCoords, int nPoints,
	 const SbVec2f *points, SbBool isClosed);
    void fillBevel(SbVec3f *result, int nPoints, 
		   const SbVec2f *points, const SbVec2f &translation,
		   const SbVec2f &n1, const SbVec2f &n2);
    void fillBevelN(SbVec3f *result, int nPoints, 
		    const SbVec2f *norms, const SbVec2f &n);

    // Texture coordinates in side display lists
    int		sidesHaveTexCoords;

    // Number of characters in this font.
 
    int		numChars;

    // Display lists for fronts, sides:
    SoGLDisplayList *frontList;
    SoGLDisplayList *sideList;

    // Profile information:
    float	cosCreaseAngle;
    int32_t	nProfileVerts;	// Number of points in profile
    SbVec2f	*profileVerts;	// Profile vertices
    float	*sTexCoords;	// Texture coordinates along profile
    				// (nProfileVerts of them)
    SbVec2f	*profileNorms;	// Profile normals
				// ((nProfileVerts-1)*2 of them)

    // This flag will be true if there is another cache open (if
    // building GL display lists for render caching, that means we
    // can't also build display lists).
    SbBool	otherOpen;

    // And tables telling us if a display list has been created for
    // each character in the font (we do that lazily since it is
    // expensive):
    SbDict* frontDict;
    SbDict* sideDict;

    // dictionary of outlines; these are also cached and created when
    // needed.
    SbDict* outlineDict;
    
    // Remember nodeId that was used to do UCS translation.
    uint32_t	currentNodeId;
    
    static iconv_t	conversionCode;

    // Font size
    float	fontSize;

    // Flag used to detect tesselation errors:
    static SbBool tesselationError;

    // List of font numbers for this font-list
    GLubyte* fontNumList;
    SbPList* fontNums;
    
    // char* pointers of UCS-2 strings:
    SbPList	UCSStrings;
    // size of these strings, in UCS-2 characters:
    SbPList	UCSNumChars;

    // Font library context for all outline fonts
    static FLcontext	context;

    // Global list of available fonts; a 'font' in this case is a
    // unique set of font name, font size, complexity value/type, and
    // set of profiles-- if any of these changes, the set of polygons
    // representing the font will change, and a different font will be
    // used.
    static SbPList	*fonts;
    // Free up an outline (invoked by SbDict::applyToAll):
    static void freeOutline( unsigned long,  void* value);
};

SbBool SoOutlineFontCache::tesselationError = FALSE;
SbPList *SoOutlineFontCache::fonts = NULL;
FLcontext SoOutlineFontCache::context = NULL;
iconv_t SoOutlineFontCache::conversionCode = NULL;

// Static stuff is used while generating primitives:
SoText3 *SoText3::currentGeneratingNode = NULL;
SoPrimitiveVertex *SoText3::genPrimVerts[3];
SbVec3f SoText3::genTranslate;
int SoText3::genWhichVertex = -1;
uint32_t SoText3::genPrimType;
SoAction *SoText3::genAction = NULL;
SbBool SoText3::genBack = FALSE;
SbBool SoText3::genTexCoord = TRUE;
const SoTextureCoordinateElement *SoText3::tce = NULL;

SO_NODE_SOURCE(SoText3);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoText3::SoText3()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoText3);

    SO_NODE_ADD_FIELD(string,	(""));
    SO_NODE_ADD_FIELD(spacing,	(1.0));
    SO_NODE_ADD_FIELD(justification,	(LEFT));
    SO_NODE_ADD_FIELD(parts,		(FRONT));

    // Set up static info for enumerated type field
    SO_NODE_DEFINE_ENUM_VALUE(Justification,	LEFT);
    SO_NODE_DEFINE_ENUM_VALUE(Justification,	RIGHT);
    SO_NODE_DEFINE_ENUM_VALUE(Justification,	CENTER);

    // Set up static info for enumerated type field
    SO_NODE_DEFINE_ENUM_VALUE(Part, SIDES);
    SO_NODE_DEFINE_ENUM_VALUE(Part, FRONT);
    SO_NODE_DEFINE_ENUM_VALUE(Part, BACK);
    SO_NODE_DEFINE_ENUM_VALUE(Part, ALL);

    // Set up info in enumerated type field
    SO_NODE_SET_SF_ENUM_TYPE(justification, Justification);
    SO_NODE_SET_SF_ENUM_TYPE(parts, Part);

    isBuiltIn = TRUE;
    myFont = NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoText3::~SoText3()
//
////////////////////////////////////////////////////////////////////////
{
    if (myFont != NULL) myFont->unref();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Figure out the bounds of a character.
//
// Use: extender

SbBox3f
SoText3::getCharacterBounds(SoState *state, int stringIndex, int
			    charIndex)
//
////////////////////////////////////////////////////////////////////////
{
    SbBox3f result;

    if (!setupFontCache(state))
	return result;  // Empty bbox
    
#ifdef DEBUG
    if (stringIndex >= string.getNum()) {
	SoDebugError::post("SoText3::getCharacterBounds",
			   "stringIndex (%d) out of range (max %d)",
			   stringIndex, string.getNum());
    }
    if (charIndex >= myFont->getNumUCSChars(stringIndex)) {
	SoDebugError::post("SoText3::getCharacterBounds",
			   "charIndex (%d) out of range (max %d)",
			   charIndex,
			   myFont->getNumUCSChars(stringIndex));
    }
#endif

    float frontZ, backZ;
    myFont->getProfileBounds(frontZ, backZ);
    
    float height = myFont->getHeight();

    const char *chars = myFont->getUCSString(stringIndex);
    float width = (myFont->getCharOffset(chars+2*charIndex))[0];
    
    // Figure out where origin of character is:
    SbVec2f charPosition = getStringOffset(stringIndex);
    for (int i = 0; i < charIndex; i++) {
	charPosition += myFont->getCharOffset(chars+2*charIndex);
    }
    
    // Ok, have width, height, depth and starting position of text,
    // can create the bounds box:
    if (parts.getValue() & (FRONT|SIDES)) {
	result.extendBy(SbVec3f(charPosition[0],
				charPosition[1],
				frontZ));
	result.extendBy(SbVec3f(charPosition[0]+width,
				charPosition[1],
				frontZ));
	result.extendBy(SbVec3f(charPosition[0],
				charPosition[1]+height,
				frontZ));
	result.extendBy(SbVec3f(charPosition[0]+width,
				charPosition[1]+height,
				frontZ));
    }
    if (parts.getValue() & (BACK|SIDES)) {
	result.extendBy(SbVec3f(charPosition[0],
				charPosition[1],
				backZ));
	result.extendBy(SbVec3f(charPosition[0]+width,
				charPosition[1],
				backZ));
	result.extendBy(SbVec3f(charPosition[0],
				charPosition[1]+height,
				backZ));
	result.extendBy(SbVec3f(charPosition[0]+width,
				charPosition[1]+height,
				backZ));
    }
    return result;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Performs GL rendering of a Text3.
//
// Use: extender

void
SoText3::GLRender(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    static gluTESSELATOR *tobj = NULL;

    // First see if the object is visible and should be rendered now
    if (! shouldGLRender(action))
	return;

    SoState *state = action->getState();

    if (!setupFontCache(state, TRUE))
	return;

    SoMaterialBindingElement::Binding mbe =
	SoMaterialBindingElement::get(state);
    SbBool materialPerPart =
	(mbe == SoMaterialBindingElement::PER_PART_INDEXED ||
	 mbe == SoMaterialBindingElement::PER_PART);

    SoMaterialBundle	mb(action);
    if (!materialPerPart) {
	// Make sure the fist current material is sent to GL
	mb.sendFirst();
    }

    float firstZ, lastZ;
    myFont->getProfileBounds(firstZ, lastZ);

    if (tobj == NULL) {
	tobj = gluNewTess();
	gluTessCallback(tobj, GLU_BEGIN, (void (*)())glBegin);
	gluTessCallback(tobj, GLU_END, (void (*)())glEnd);
	gluTessCallback(tobj, GLU_VERTEX, (void (*)())glVertex2fv);
	gluTessCallback(tobj, GLU_ERROR,
			(void (*)())SoOutlineFontCache::errorCB);
    }

    // See if texturing is enabled
    genTexCoord = SoGLTextureEnabledElement::get(action->getState());

    if ((parts.getValue() & SIDES) && (myFont->hasProfile())) {
	if (materialPerPart) mb.send(1, FALSE);

	myFont->setupToRenderSide(state, genTexCoord);
	for (int line = 0; line < string.getNum(); line++) {
	    glPushMatrix();
	    SbVec2f p = getStringOffset(line);
	    if (p[0] != 0.0 || p[1] != 0.0)
		glTranslatef(p[0], p[1], 0.0);
	    renderSide(action, line);
	    glPopMatrix();
	}
    }
    if (parts.getValue() & BACK) {
	if (materialPerPart) mb.send(2, FALSE);

	if (lastZ != 0.0) {
	    glTranslatef(0, 0, lastZ);
	}
	glNormal3f(0, 0, -1);
	glFrontFace(GL_CW);

	myFont->setupToRenderFront(state);
	
	if (genTexCoord) {
	    glPushAttrib(GL_TEXTURE_BIT);
	    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	    GLfloat params[4];
	    params[0] = -1.0/myFont->getHeight();
	    params[1] = params[2] = params[3] = 0.0;
	    glTexGenfv(GL_S, GL_OBJECT_PLANE, params);
	    params[1] = -params[0];
	    params[0] = 0.0;
	    glTexGenfv(GL_T, GL_OBJECT_PLANE, params);
	    
	    glEnable(GL_TEXTURE_GEN_S);
	    glEnable(GL_TEXTURE_GEN_T);
	}
	
	for (int line = 0; line < string.getNum(); line++) {
	    if (string[line].getLength() <= 0) continue;
	    
	    glPushMatrix();
	    SbVec2f p = getStringOffset(line);
	    if (p[0] != 0.0 || p[1] != 0.0)
		glTranslatef(p[0], p[1], 0.0);
	    renderFront(action, line, tobj);
	    glPopMatrix();
	}
	
	if (genTexCoord) {
	    glPopAttrib();
	}

	glFrontFace(GL_CCW);

	if (lastZ != 0)
	    glTranslatef(0, 0, -lastZ);
    }	
    if (parts.getValue() & FRONT) {
	if (materialPerPart) mb.sendFirst();

	if (firstZ != 0.0) {
	    glTranslatef(0, 0, firstZ);
	}

	glNormal3f(0, 0, 1);
	
	myFont->setupToRenderFront(state);

	if (genTexCoord) {
	    glPushAttrib(GL_TEXTURE_BIT);
	    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	    GLfloat params[4];
	    params[0] = 1.0/myFont->getHeight();
	    params[1] = params[2] = params[3] = 0.0;
	    glTexGenfv(GL_S, GL_OBJECT_PLANE, params);
	    params[1] = params[0];
	    params[0] = 0.0;
	    glTexGenfv(GL_T, GL_OBJECT_PLANE, params);
	    
	    glEnable(GL_TEXTURE_GEN_S);
	    glEnable(GL_TEXTURE_GEN_T);
	}
	
	for (int line = 0; line < string.getNum(); line++) {
	    glPushMatrix();
	    SbVec2f p = getStringOffset(line);
	    if (p[0] != 0.0 || p[1] != 0.0)
		glTranslatef(p[0], p[1], 0.0);
	    renderFront(action, line, tobj);
	    glPopMatrix();
	}
	
	if (genTexCoord) {
	    glPopAttrib();
	}

	if (firstZ != 0.0) {
	    glTranslatef(0, 0, -firstZ);
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements ray picking
//
// Use: extender

void
SoText3::rayPick(SoRayPickAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // First see if the object is pickable
    if (! shouldRayPick(action))
	return;

    //
    // NOTE: This could be made more efficient by testing the ray
    // against the bounding box of each character, as follows:
    //
    // 	-- For each string {
    // 	    -- For each character in the string: {
    // 		If the ray intersects the character's bounding box,
    // 		  generate primitives for that character
    // 	    }
    // 	}
    //
    // This is a project for the future... ???

    // For now, just use generatePrimitives() to do the work.
    SoShape::rayPick(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Computes bounding box of text.
//
// Use: protected

void
SoText3::computeBBox(SoAction *action, SbBox3f &box, SbVec3f &center)
//
////////////////////////////////////////////////////////////////////////
{
    // Set the center to be the origin, which is the natural "center"
    // of the text, regardless of justification
    center.setValue(0.0, 0.0, 0.0);

    int prts = parts.getValue();
    if (prts == 0)
	return;

    SoState *state = action->getState();

    if (!setupFontCache(state))
	return;

    // Get the bounding box of all the characters:
    SbBox2f outlineBox;
    getFrontBBox(outlineBox);

    // If no lines and no characters, return empty bbox:
    if (outlineBox.isEmpty()) return;
    
    // .. and extend it based on what parts are turned on:
    float firstZ, lastZ;
    myFont->getProfileBounds(firstZ, lastZ);

    const SbVec2f &boxMin = outlineBox.getMin();
    const SbVec2f &boxMax = outlineBox.getMax();
		     

    // Front and back are straightforward:
    if (prts & FRONT) {
	SbVec3f min(boxMin[0], boxMin[1], firstZ);
	SbVec3f max(boxMax[0], boxMax[1], firstZ);
	box.extendBy(min);
	box.extendBy(max);
    }
    if (prts & BACK) {
	SbVec3f min(boxMin[0], boxMin[1], lastZ);
	SbVec3f max(boxMax[0], boxMax[1], lastZ);
	box.extendBy(min);
	box.extendBy(max);
    }
    //
    // Sides are trickier.  We figure out the maximum offset
    // of the profile we're using.  If the offset is
    // negative, we use its absolute value; normally, negative
    // (indented) characters won't expand the bounding box, but if the
    // offset is negative enough the character will inter-penetrate
    // itself-- think of a Helvetica 'I', with a bevel of a big
    // negative spike.  The bounding box is either the bounding box of
    // the front/back, or, if the spike is big enough, the size of the
    // spike minus the width of the I.  I'm being conservative here
    // and just expanding the front/back bounding boxes by the maximum
    // offset (correct for positive offsets, conservative for negative
    // offsets).
    //
    if ((prts & SIDES) && myFont->hasProfile()) {
	SbBox2f profileBox;
	myFont->getProfileBBox(profileBox);

	const SbVec2f &pBoxMin = profileBox.getMin();
	const SbVec2f &pBoxMax = profileBox.getMax();
	
	// If no profile, return the front/back bbox:
	if (profileBox.isEmpty()) return;

	//
	// Expand the bounding box forward/backward in case the
	// profile extends forwards/backwards:
	//
	SbVec3f min, max;
	min.setValue(boxMin[0], boxMin[1], pBoxMin[0]);
	max.setValue(boxMax[0], boxMax[1], pBoxMax[0]);
	box.extendBy(min);
	box.extendBy(max);

	//
	// And figure out the maximum profile offset, and expand
	// out the outline's bbox:
	//
# define max(a,b)               (a<b ? b : a)
# define abs(x)                 (x>=0 ? x : -(x))
	float maxOffset = max(abs(pBoxMin[1]), abs(pBoxMax[1]));
#undef max
#undef abs
	min.setValue(boxMin[0]-maxOffset, boxMin[1]-maxOffset, firstZ);
	max.setValue(boxMax[0]+maxOffset, boxMax[1]+maxOffset, lastZ);
	box.extendBy(min);
	box.extendBy(max);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Generates triangles representing a Text3.
//
// Use: protected

void
SoText3::generatePrimitives(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoState *state = action->getState();

    if (!setupFontCache(state))
	return;

    currentGeneratingNode = this;

    // Set up default texture coordinate mapping, if necessary:
    SoTextureCoordinateElement::CoordType tcType =
	SoTextureCoordinateElement::getType(state);
    if (tcType == SoTextureCoordinateElement::EXPLICIT) {
	genTexCoord = TRUE;
	tce = NULL;
    } else {
	genTexCoord = FALSE;
	tce = SoTextureCoordinateElement::getInstance(state);
    }

    // Set up 3 vertices we can use
    SoPrimitiveVertex	v1, v2, v3;
    SoTextDetail detail;
    v1.setDetail(&detail);
    v2.setDetail(&detail);
    v3.setDetail(&detail);

    genPrimVerts[0] = &v1;
    genPrimVerts[1] = &v2;
    genPrimVerts[2] = &v3;

    genAction = action;
    genBack = FALSE;

    SoMaterialBindingElement::Binding mbe =
	SoMaterialBindingElement::get(state);
    SbBool materialPerPart =
	(mbe == SoMaterialBindingElement::PER_PART_INDEXED ||
	 mbe == SoMaterialBindingElement::PER_PART);
    if (!materialPerPart) {
	v1.setMaterialIndex(0);
	v2.setMaterialIndex(0);
	v3.setMaterialIndex(0);
    }

    float firstZ, lastZ;
    myFont->getProfileBounds(firstZ, lastZ);

    uint32_t prts = parts.getValue();
    if ((prts & SIDES) && myFont->hasProfile()) {
	if (materialPerPart) {
	    v1.setMaterialIndex(1);
	    v2.setMaterialIndex(1);
	    v3.setMaterialIndex(1);
	}
	detail.setPart(SIDES);

	for (int line = 0; line < string.getNum(); line++) {
	    detail.setStringIndex(line);

	    SbVec2f p = getStringOffset(line);
	    genTranslate.setValue(p[0], p[1], lastZ);
	    generateSide(line);
	}
    }
    if (prts & BACK) {
	genBack = TRUE;
	if (materialPerPart) {
	    v1.setMaterialIndex(2);
	    v2.setMaterialIndex(2);
	    v3.setMaterialIndex(2);
	}
	detail.setPart(BACK);

	v1.setNormal(SbVec3f(0, 0, -1));
	v2.setNormal(SbVec3f(0, 0, -1));
	v3.setNormal(SbVec3f(0, 0, -1));
	
	for (int line = 0; line < string.getNum(); line++) {
	    detail.setStringIndex(line);

	    SbVec2f p = getStringOffset(line);
	    genTranslate.setValue(p[0], p[1], lastZ);
	    generateFront(line);
	}
	genBack = FALSE;
    }	
    if (prts & FRONT) {
	if (materialPerPart) {
	    v1.setMaterialIndex(0);
	    v2.setMaterialIndex(0);
	    v3.setMaterialIndex(0);
	}
	detail.setPart(FRONT);

	v1.setNormal(SbVec3f(0, 0, 1));
	v2.setNormal(SbVec3f(0, 0, 1));
	v3.setNormal(SbVec3f(0, 0, 1));
	
	for (int line = 0; line < string.getNum(); line++) {
	    detail.setStringIndex(line);

	    SbVec2f p = getStringOffset(line);
	    genTranslate.setValue(p[0], p[1], firstZ);
	    generateFront(line);
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Setup internal font cache.  Called by all action methods before
//    doing their thing.  GLRender passes TRUE to do special rendering
//    setup.  Returns FALSE if there are problems and the action
//    should bail.
//
// Use: private

SbBool
SoText3::setupFontCache(SoState *state, SbBool forRender)
//
////////////////////////////////////////////////////////////////////////
{
    // The state must be pushed here because myFont->isRenderValid
    // depends on the state being the correct depth (it must be the
    // same depth as when the font cache was built).
    state->push();

    if (myFont != NULL) {
	SbBool isValid;
	if (forRender)
	    isValid = myFont->isRenderValid(state);
	else
	    isValid = myFont->isValid(state);

	if (!isValid) {
	    myFont->unref(state);
	    myFont = NULL;
	}
    }
    if (myFont == NULL) {
	myFont = SoOutlineFontCache::getFont(state, forRender);
    }
    
    //The current text must be translated to UCS, unless this
    //translation has already been done.
    
    if(myFont) myFont->convertToUCS(getNodeId(), string);
    state->pop();
    return  myFont != NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Gets the 2D bounding box of the outlines of the letters in our
//    string.  How?  Get all the outlines and extend the bbox by every
//    point in every outline... (assuming the lines or characters
//    don't overlap, this could be optimized by just considering the
//    characters on the borders of the string-- although there are
//    some gotchas, like spaces at the beginning of a line...).
//
// Use: private

void
SoText3::getFrontBBox(SbBox2f &result)
//
////////////////////////////////////////////////////////////////////////
{
    SbBox2f charBBox;

    int line, character;
    for (line = 0; line < string.getNum(); line++) {
	// Starting position of string, based on justification:
	SbVec2f charPosition = getStringOffset(line);
       
	const char *chars = myFont->getUCSString(line);
	
	for (character = 0; character < myFont->getNumUCSChars(line); 
		character++) {
	    myFont->getCharBBox(chars+2*character, charBBox);
	    if (!charBBox.isEmpty()) {
		SbVec2f min = charBBox.getMin() + charPosition;
		SbVec2f max = charBBox.getMax() + charPosition;
		result.extendBy(min);
		result.extendBy(max);
	    }

	    // And advance...
	    charPosition += myFont->getCharOffset(chars+2*character);
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Figure out how much to offset the given line of text.  The X
//    offset depends on the justification and the width of the string
//    in the current font.  The Y offset depends on the line spacing
//    and font height.
//
// Use: private

SbVec2f
SoText3::getStringOffset(int line)
//
////////////////////////////////////////////////////////////////////////
{
    SbVec2f result(0,0);
    
    if (justification.getValue() == RIGHT) {
	float width = myFont->getWidth(line);
	result[0] = -width;
    }
    if (justification.getValue() == CENTER) {
	float width = myFont->getWidth(line);
	result[0] = -width/2.0;
    }
    result[1] = -line*myFont->getHeight()*spacing.getValue();

    return result;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Figure out where a given character starts.
//
// Use: private

SbVec2f
SoText3::getCharacterOffset(int line, int whichChar)
//
////////////////////////////////////////////////////////////////////////
{
    SbVec2f result = getStringOffset(line);
    
    const char *chars = myFont->getUCSString(line);

    // Now add on all of the character advances up to char:
    for (int i = 0; i < whichChar; i++) {
	result += myFont->getCharOffset(chars+2*i);
    }
    return result;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Render the fronts of the given string.  The GL transformation
//    matrix is munged by this routine-- surround it by
//    PushMatrix/PopMatrix.
//
// Use: private, internal

void
SoText3::renderFront(SoGLRenderAction *, int line,
		     gluTESSELATOR *tobj)
//
////////////////////////////////////////////////////////////////////////
{
    const char *chars = myFont->getUCSString(line);

    // First, try to figure out if we can use glCallLists:
    SbBool useCallLists = TRUE;

    for (int i = 0; i < myFont->getNumUCSChars(line); i++) {
	// See if the font cache already has (or can build) a display
	// list for this character:
	if (!myFont->hasFrontDisplayList(chars+2*i, tobj)) {
	    useCallLists = FALSE;
	    break;
	}
    }
    // if we have display lists for all of the characters, use
    // glCallLists:
    if (useCallLists) {
	myFont->callFrontLists(line);
    }
    // if we don't, draw the string character-by-character, using the
    // display lists we do have:
    else {
	myFont->renderFront(line, tobj);
    }
}    

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Render the sides of the given string.  The GL transformation
//    matrix is munged by this routine-- surround it by
//    PushMatrix/PopMatrix.
//
// Use: private

void
SoText3::renderSide(SoGLRenderAction *, int line)
//
////////////////////////////////////////////////////////////////////////
{
    const char *chars = myFont->getUCSString(line);

    // First, try to figure out if we can use glCallLists:
    SbBool useCallLists = TRUE;

    for (int i = 0; i < myFont->getNumUCSChars(line); i++) {
	// See if the font cache already has (or can build) a display
	// list for this character:
	if (!myFont->hasSideDisplayList(chars+2*i, renderSideTris)) {
	    useCallLists = FALSE;
	    break;
	}
    }
    // if we have display lists for all of the characters, use
    // glCallLists:
    if (useCallLists) {
	myFont->callSideLists(line);
    }
    // if we don't, draw the string character-by-character, using the
    // display lists we do have:
    else {
	myFont->renderSide(line, renderSideTris);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Create text detail and fill in extra info.
//
// Use: protected, virtual

SoDetail *
SoText3::createTriangleDetail(SoRayPickAction *,
			      const SoPrimitiveVertex *v1,
			      const SoPrimitiveVertex *,
			      const SoPrimitiveVertex *,
			      SoPickedPoint *)
//
////////////////////////////////////////////////////////////////////////
{
    SoTextDetail *result = new SoTextDetail;
    const SoTextDetail *old = (const SoTextDetail *)v1->getDetail();
    
    result->setPart(old->getPart());
    result->setStringIndex(old->getStringIndex());
    result->setCharacterIndex(old->getCharacterIndex());

    return result;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Generates primitives for the fronts of characters.  Assumes that
//    genTranslate has been set to the starting position of the
//    string, and that the genPrimVerts array points to three
//    appropriate vertices.
//
// Use: private internal

void
SoText3::generateFront(int line)
//
////////////////////////////////////////////////////////////////////////
{
    static gluTESSELATOR *tobj = NULL;

    const char *chars = myFont->getUCSString(line);

    if (tobj == NULL) {
	tobj = gluNewTess();
	gluTessCallback(tobj, GLU_BEGIN, (void (*)())SoText3::beginCB);
	gluTessCallback(tobj, GLU_END, (void (*)())SoText3::endCB);
	gluTessCallback(tobj, GLU_VERTEX, (void (*)())SoText3::vtxCB);
	gluTessCallback(tobj, GLU_ERROR,
			(void (*)())SoOutlineFontCache::errorCB);
    }

    genWhichVertex = 0;

    SoTextDetail *d = (SoTextDetail *)genPrimVerts[0]->getDetail();

    for (int i = 0; i < myFont->getNumUCSChars(line); i++) {
	d->setCharacterIndex(i);

	myFont->generateFrontChar(chars+2*i, tobj);

	SbVec2f p = myFont->getCharOffset(chars+2*i);
	genTranslate[0] += p[0];
	genTranslate[1] += p[1];
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Generates sides of the characters.
//
// Use: private internal

void
SoText3::generateSide(int line)
//
////////////////////////////////////////////////////////////////////////
{
    const char *chars = myFont->getUCSString(line);

    SoTextDetail *d = (SoTextDetail *)genPrimVerts[0]->getDetail();

    for (int i = 0; i < myFont->getNumUCSChars(line); i++) {
	d->setCharacterIndex(i);

	myFont->generateSideChar(chars+2*i, generateSideTris);

	SbVec2f p = myFont->getCharOffset(chars+2*i);
	genTranslate[0] += p[0];
	genTranslate[1] += p[1];
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Given two correctly rotated and positioned bevels, this routine
//    fills in the triangles in between them.
//
// Use: private, static

void
SoText3::generateSideTris(int nPoints, const SbVec3f *p1, const SbVec3f *n1,
		const SbVec3f *p2, const SbVec3f *n2,
		const float *sTexCoords, const float *tTexCoords)
//
////////////////////////////////////////////////////////////////////////
{
    float vertex[3];
    SbVec4f texCoord(0,0,0,1);
    
    const SbVec3f *p[2]; p[0] = p1; p[1] = p2;
    const SbVec3f *n[2]; n[0] = n1; n[1] = n2;

// Handy little macro to set a primitiveVertice's point, normal, and
// texture coordinate:
#define SET(pv,i,row,col) \
    {vertex[0] = p[col][i+row][0] + genTranslate[0]; \
     vertex[1] = p[col][i+row][1] + genTranslate[1]; \
     vertex[2] = p[col][i+row][2]; \
     genPrimVerts[pv]->setPoint(vertex);\
     genPrimVerts[pv]->setNormal(n[col][i*2+row]); \
     texCoord[0] = sTexCoords[i+row]; \
     texCoord[1] = tTexCoords[col]; \
     genPrimVerts[pv]->setTextureCoords(texCoord); \
    }

    SoText3 *t3 = currentGeneratingNode;

    for (int i = 0; i < nPoints-1; i++) {
	// First triangle: 
	SET(0, i, 0, 0)
	SET(1, i, 1, 0)
	SET(2, i, 0, 1)
	t3->invokeTriangleCallbacks(genAction, genPrimVerts[0],
				    genPrimVerts[1], genPrimVerts[2]);

	// Second triangle:
	SET(0, i, 1, 1)
	SET(1, i, 0, 1)
	SET(2, i, 1, 0)
	t3->invokeTriangleCallbacks(genAction, genPrimVerts[0],
				    genPrimVerts[1], genPrimVerts[2]);
#undef SET
    }
}
////////////////////////////////////////////////////////////////////////
//
// Description:
//    Given two correctly rotated and positioned bevels, this routine
//    renders triangles in between them.
//
// Use: private, static

void
SoText3::renderSideTris(int nPoints, const SbVec3f *p1, const SbVec3f *n1,
			const SbVec3f *p2, const SbVec3f *n2,
			const float *sTex, const float *tTex)
//
////////////////////////////////////////////////////////////////////////
{
    // Note:  the glBegin(GL_QUADS) is optimized up into the
    // routine that calls generateSideChar, so there is one glBegin
    // per character.
    for (int i = 0; i < nPoints-1; i++) {
	if (genTexCoord) glTexCoord2f(sTex[i+1], tTex[0]);
	glNormal3fv(n1[i*2+1].getValue());
	glVertex3fv(p1[i+1].getValue());

	if (genTexCoord) glTexCoord2f(sTex[i+1], tTex[1]);
	glNormal3fv(n2[i*2+1].getValue());
	glVertex3fv(p2[i+1].getValue());

	if (genTexCoord) glTexCoord2f(sTex[i], tTex[1]);
	glNormal3fv(n2[i*2].getValue());
	glVertex3fv(p2[i].getValue());

	if (genTexCoord) glTexCoord2f(sTex[i], tTex[0]);
	glNormal3fv(n1[i*2].getValue());
	glVertex3fv(p1[i].getValue());
    }
}
	    
////////////////////////////////////////////////////////////////////////
//
// Description:
//    Called by the GLU tesselator when we are beginning a triangle
//    strip, fan, or set of independent triangles.
//
// Use: static, private

void
SoText3::beginCB(GLenum primType)
//
////////////////////////////////////////////////////////////////////////
{
    genPrimType = primType;
    genWhichVertex = 0;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Called by the GLU tesselator when we are done with the
//    strip/fan/etc.
//
// Use: static, private

void
SoText3::endCB()
//
////////////////////////////////////////////////////////////////////////
{
    genWhichVertex = 0;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Called by the GLU tesselator when we are generating primitives.
//
// Use: static, private

void
SoText3::vtxCB(void *v)
//
////////////////////////////////////////////////////////////////////////
{
    SbVec2f &vv = *((SbVec2f *)v);
    float vertex[3];
    vertex[0] = vv[0] + genTranslate[0];
    vertex[1] = vv[1] + genTranslate[1];
    vertex[2] = genTranslate[2];

    SoText3 *t3 = currentGeneratingNode;
    
    // Fill in one of the primitive vertices:
    genPrimVerts[genWhichVertex]->setPoint(vertex);

    SbVec4f texCoord;
    
    // And texture coordinates:
    if (genTexCoord) {
	float textHeight = t3->myFont->getHeight();
	texCoord.setValue(vertex[0]/textHeight, vertex[1]/textHeight,
			  0.0, 1.0);
	// S coordinates go other way on back...
	if (genBack) texCoord[0] = -texCoord[0];
    } else {
	texCoord = tce->get(vertex, genPrimVerts[0]->getNormal());
    }
    genPrimVerts[genWhichVertex]->setTextureCoords(texCoord);
	
    genWhichVertex = (genWhichVertex+1)%3;

    // If we just filled in the third vertex, we can spit out a
    // triangle:
    if (genWhichVertex == 0) {
	// If we are doing the BACK part, reverse the triangle:
	if (genBack) {
	    t3->invokeTriangleCallbacks(genAction,
					genPrimVerts[2],
					genPrimVerts[1],
					genPrimVerts[0]);
	} else {
	    t3->invokeTriangleCallbacks(genAction,
					genPrimVerts[0],
					genPrimVerts[1],
					genPrimVerts[2]);
	}
	// Now, need to set-up for the next vertex.
	// Three cases to deal with-- independent triangles, triangle
	// strips, and triangle fans.
	switch (genPrimType) {
	  case GL_TRIANGLES:
	    // Don't need to do anything-- every three vertices
	    // defines a triangle.
	    break;

// Useful macro:
#define SWAP(a, b) { SoPrimitiveVertex *t = a; a = b; b = t; }

	  case GL_TRIANGLE_FAN:
	    // For triangle fans, vertex zero stays the same, but
	    // vertex 2 becomes vertex 1, and the next vertex to come
	    // in will replace vertex 2 (the old vertex 1).
	    SWAP(genPrimVerts[1], genPrimVerts[2]);
	    genWhichVertex = 2;
	    break;

	  case GL_TRIANGLE_STRIP:
	    // For triangle strips, vertex 1 becomes vertex 0, vertex
	    // 2 becomes vertex 1, and the new triangle will replace
	    // vertex 2 (the old vertex 0).
	    SWAP(genPrimVerts[1], genPrimVerts[0]);
	    SWAP(genPrimVerts[2], genPrimVerts[1]);
	    genWhichVertex = 2;
	    break;
#undef SWAP
	}
    }
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Find an appropriate font, given a state.  A bunch of elements
//    (fontName, fontSize, creaseAngle, complexity and profile) must
//    be enabled in the state...
//
// Use: static, internal

SoOutlineFontCache *
SoOutlineFontCache::getFont(SoState *state, SbBool forRender)
//
////////////////////////////////////////////////////////////////////////
{
    if (fonts == NULL) {
	// One-time font library initialization
	fonts = new SbPList;
	context = flCreateContext(NULL, FL_FONTNAME, NULL,
				  1.0, 1.0);
	if (context == NULL) {
#ifdef DEBUG
	    SoDebugError::post("SoText3::getFont",
			       "flCreateContext returned NULL");
#endif
	    return NULL;
	}
	flMakeCurrentContext(context);
	flSetHint(FL_HINT_FONTTYPE, FL_FONTTYPE_OUTLINE);
    }
    else if (context == NULL) return NULL;
    else {
	if (flGetCurrentContext() != context)
	    flMakeCurrentContext(context);
    }

    SoOutlineFontCache *result = NULL;
    for (int i = 0; i < fonts->getLength() && result == NULL; i++) {
	SoOutlineFontCache *c = (SoOutlineFontCache *) (*fonts)[i];
	if (forRender ? c->isRenderValid(state) : c->isValid(state)) {
	    result = c; // Loop will terminate...
	    result->ref(); // Increment ref count
	}
    }
    // If none match:
    if (result == NULL) {
	result = new SoOutlineFontCache(state);

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
SoOutlineFontCache::createUniFontList(const char* fontNameList) 
//
////////////////////////////////////////////////////////////////////////
{
    char *s, *s1, *ends;
    FLfontNumber fn;
    float mat[2][2];

    mat[0][0] = mat[1][1] = 1.0;
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
       *s1 = NULL;  /* font name is pointed to s */

       if ((fn = flCreateFont((const GLubyte*)s, mat, 0, NULL)) == NULL) {
#ifdef DEBUG
	    SoDebugError::post("SoOutlineFontCache::createUniFontList", 
		"Cannot create font %s", s);         
#endif          
       }
       else fontNums->append((void*)fn);
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
SoOutlineFontCache::isValid(const SoState *state) const
//
////////////////////////////////////////////////////////////////////////
{
    SbBool result = SoCache::isValid(state);
    
    if (result) {
	if (flGetCurrentContext() != context) {
	    flMakeCurrentContext(context);
	}
    }
    return result;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Figures out if this cache is valid for rendering.
//
// Use: internal

SbBool
SoOutlineFontCache::isRenderValid(SoState *state) const
//
////////////////////////////////////////////////////////////////////////
{
    // Special cache case here:  if we generated side display lists
    // without texture coordinates AND we need texture coordinates,
    // we'll have to regenerate and this cache is invalid:
    if (sideList) {
	if (!sidesHaveTexCoords &&
	    SoGLTextureEnabledElement::get(state)) {
	    return FALSE;
	}
    }

    if (!isValid(state)) return FALSE;

    if (frontList && 
	frontList->getContext() != SoGLCacheContextElement::get(state))
	return FALSE;
    if (sideList && 
	sideList->getContext() != SoGLCacheContextElement::get(state))
	return FALSE;

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor.  Called by getFont().
//
// Use: private

SoOutlineFontCache::SoOutlineFontCache(SoState *state) : 
	SoCache(state)
//
////////////////////////////////////////////////////////////////////////
{
    ref();

    frontList = sideList = NULL;

    // Add element dependencies explicitly here; making 'this' the
    // CacheElement doesn't work if we are being constructed in an
    // action that doesn't have caches.
    SbName font = SoFontNameElement::get(state);
    addElement(state->getConstElement(
	SoFontNameElement::getClassStackIndex()));
    if (font == SoFontNameElement::getDefault()) {
	font = SbName("Utopia-Regular");
    }
    
    float uems;
    
    // Remember size
    fontSize = SoFontSizeElement::get(state);
    addElement(state->getConstElement(
	SoFontSizeElement::getClassStackIndex()));
    
    // Figure out complexity...
    float complexity = SoComplexityElement::get(state);
    addElement(state->getConstElement(
	SoComplexityElement::getClassStackIndex()));
    addElement(state->getConstElement(
	SoComplexityTypeElement::getClassStackIndex()));

    switch (SoComplexityTypeElement::get(state)) {
      case SoComplexityTypeElement::OBJECT_SPACE:
	{
	    // Two ramps-- complexity of zero  == 250/1000 of an em
	    //             complexity of .5    == 20/1000 of an em
	    //             complexity of 1     == 1/1000 of an em
	    const float ZERO = 250;
	    const float HALF = 20;
	    const float ONE = 1;
	    if (complexity > 0.5) uems = (2.0-complexity*2.0)*(HALF-ONE)+ONE;
	    else uems = (1.0-complexity*2.0)*(ZERO-HALF)+HALF;
	}
	break;

      case SoComplexityTypeElement::SCREEN_SPACE:
	{
	    SbVec3f p(fontSize, fontSize, fontSize);
	    SbVec2s rectSize;
	    
	    SoShape::getScreenSize(state, SbBox3f(-p, p), rectSize);
	    float maxSize =
		(rectSize[0] > rectSize[1] ? rectSize[0] : rectSize[1]);
	    uems = 250.0 / (1.0 + 0.25 * maxSize * complexity *
			    complexity);
	    
	    // We have to manually add the dependency on the
	    // projection, view and model matrix elements (these are
	    // gotten in the SoShape::getScreenSize routine), and the
	    // ViewportRegionElement:
	    addElement(state->getConstElement(
		SoProjectionMatrixElement::getClassStackIndex()));
	    addElement(state->getConstElement(
		SoViewingMatrixElement::getClassStackIndex()));
	    addElement(state->getConstElement(
		SoModelMatrixElement::getClassStackIndex()));
	    addElement(state->getConstElement(
		SoViewportRegionElement::getClassStackIndex()));
	}
	break;

      case SoComplexityTypeElement::BOUNDING_BOX:
	{
	    uems = 20;
	}
	break;
    }
    flSetHint(FL_HINT_TOLERANCE, uems);

    fontNumList = createUniFontList(font.getString());

    // If error creating font:
    if (fontNumList == NULL) {
	// Try Utopia-Regular, unless we just did!
	if (font != SbName("Utopia-Regular")) {
#ifdef DEBUG
	    SoDebugError::post("SoText3::getFont",
		      "Couldn't find font %s, replacing with Utopia-Regular",
		       font.getString());
#endif
	    fontNumList = createUniFontList("Utopia-Regular");
	}
	if (fontNumList == NULL) {
#ifdef DEBUG
	    SoDebugError::post("SoText3::getFont",
			       "Couldn't find font Utopia-Regular!");
#endif
	    return;
	}
    }

    numChars = 65536;  //Allow for all UCS-2 possibilities.
    sidesHaveTexCoords = FALSE;
    currentNodeId = 0; //guarantee UCS translation occurs first time.

    //sideDict and frontDict indicate if display lists exist for front,sides
    //outlineDict has pointer to outline.
    sideDict = new SbDict;
    frontDict = new SbDict;
    outlineDict = new SbDict;

    // Get profile info:
    const SoNodeList &profiles = SoProfileElement::get(state);
    addElement(state->getConstElement(
	SoProfileElement::getClassStackIndex()));
    addElement(state->getConstElement(
	SoProfileCoordinateElement::getClassStackIndex()));
    nProfileVerts = 0;
    if (profiles.getLength() > 0) {
	SoProfile *profileNode = (SoProfile *)profiles[0];
	profileNode->getVertices(state, nProfileVerts, profileVerts);
    } else {
	nProfileVerts = 2;
	profileVerts = new SbVec2f[2];
	profileVerts[0].setValue(0, 0);
	profileVerts[1].setValue(1, 0);
    }	

    if (nProfileVerts > 1) {
	cosCreaseAngle = cos(SoCreaseAngleElement::get(state));
	addElement(state->getConstElement(
	    SoCreaseAngleElement::getClassStackIndex()));
	int nSegments = (int) nProfileVerts - 1;

	// Figure out normals for profiles; there are twice as many
	// normals as segments.  The two normals for each segment endpoint
	// may be averaged with the normal for the next segment, depending
	// on whether or not the angle between the segments is greater
	// than the creaseAngle.
	profileNorms = new SbVec2f[nSegments*2];
	figureSegmentNorms(profileNorms, (int) nProfileVerts, profileVerts,
			   cosCreaseAngle, FALSE);
	// Need to flip all the normals because of the way the profiles
	// are defined:
	for (int i = 0; i < nSegments*2; i++) {
	    profileNorms[i] *= -1.0;
	}
    
	// Figure out S texture coordinates, which run along the profile:
	sTexCoords = new float[nProfileVerts];
	figureSegmentTexCoords(sTexCoords, (int) nProfileVerts,
			       profileVerts, FALSE);
	// And reverse them, so 0 is at the back of the profile:
	float max = sTexCoords[nProfileVerts-1];
	for (i = 0; i < nProfileVerts; i++) {
	    sTexCoords[i] = max - sTexCoords[i];
	}
    } else {
	profileNorms = NULL;
	sTexCoords = NULL;
    }

    fonts->append(this);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoOutlineFontCache::~SoOutlineFontCache()
//
////////////////////////////////////////////////////////////////////////
{
    if (fontNumList) {
	if (flGetCurrentContext() != context) {
	    flMakeCurrentContext(context);	
	}
	
	// Free up cached outlines, display lists
    
	if (hasProfile()) {
	    delete[] profileVerts;
	    delete[] sTexCoords;
	    delete[] profileNorms;
	}
	//Must free every outline in dictionary:
	
	outlineDict->applyToAll(freeOutline);

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
		SoOutlineFontCache *t = (SoOutlineFontCache *)(*fonts)[i];
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
	delete frontDict;
	delete sideDict;
	delete outlineDict;
	
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
SoOutlineFontCache::destroy(SoState *)
//
////////////////////////////////////////////////////////////////////////
{
    // Pass in NULL to unref because this cache may be destroyed
    // from an action _other_ than GLRender:
    if (frontList) {
	frontList->unref(NULL);
	frontList = NULL;
    }
    if (sideList) {
	sideList->unref(NULL);
	sideList = NULL;
    }
    SoCache::destroy(NULL);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the width of the specified line in the cache.
//
// Use: private

float
SoOutlineFontCache::getWidth(int line)
//
////////////////////////////////////////////////////////////////////////
{
    float total = 0.0;
    const char *chars = getUCSString(line);
    
    for (int i = 0; i < getNumUCSChars(line); i++) {
	SoFontOutline *outline = getOutline(chars+2*i);
	total += outline->getCharAdvance()[0];
    }

    return total;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the 2D bounding box of the given character.
//
// Use: private

void
SoOutlineFontCache::getCharBBox(const char* c, SbBox2f &result)
//
////////////////////////////////////////////////////////////////////////
{
    result.makeEmpty();

    if (!fontNumList) return;

    SoFontOutline *outline = getOutline(c);
    
    for (int i = 0; i < outline->getNumOutlines(); i++) {
	for (int j = 0; j < outline->getNumVerts(i); j++) {
	    result.extendBy(outline->getVertex(i,j));
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Given a UCS character, return an outline for the character.  If, for
//    some reason, we can't get the outline, an 'identity' or 'null'
//    outline is returned.
//
// Use: private

SoFontOutline *
SoOutlineFontCache::getOutline(const char* c)
//
////////////////////////////////////////////////////////////////////////
{
    if (!fontNumList) {
	return SoFontOutline::getNullOutline();
    }
    long key = (c[0]<<8)|c[1];
    void *value;
    if (!outlineDict->find(key, value)){
	
	FLoutline *flo = flUniGetOutline(fontNumList, (GLubyte*)c);
	if (flo == NULL) {
	    value = (void*)SoFontOutline::getNullOutline();
	} else {
	    value = (void*) new SoFontOutline(flo, fontSize);
	    flFreeOutline(flo);
	}
	outlineDict->enter(key, value);
    }
    return (SoFontOutline*)value;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Given a character, return the correct amount to advance after
//    drawing that character.  Note:  if we ever wanted to deal with
//    kerning, we'd have to fix this...
//
// Use: private

SbVec2f
SoOutlineFontCache::getCharOffset(const char* c)
//
////////////////////////////////////////////////////////////////////////
{
    if (!fontNumList) return SbVec2f(0,0);

    return getOutline(c)->getCharAdvance();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Render the fronts of the given string.  The GL transformation
//    matrix is munged by this routine-- surround it by
//    PushMatrix/PopMatrix.
//
// Use: public, internal

void
SoOutlineFontCache::generateFrontChar(const char* c,
				      gluTESSELATOR *tobj)
//
////////////////////////////////////////////////////////////////////////
{
    if (!fontNumList) return;

    GLdouble v[3];

    tesselationError = FALSE;
    gluBeginPolygon(tobj);
    
    // Get outline for character
    SoFontOutline *outline = getOutline(c);
    for (int i = 0; i < outline->getNumOutlines(); i++) {

	// It would be nice if the font manager told us the type of
	// each outline...
	gluNextContour(tobj, GLU_UNKNOWN);

	for (int j = 0; j < outline->getNumVerts(i); j++) {
	    SbVec2f &t = outline->getVertex(i,j);
	    v[0] = t[0];
	    v[1] = t[1];
	    v[2] = 0.0;

	    // Note: The third argument MUST NOT BE a local variable,
	    // since glu just stores the pointer and only calls us
	    // back at the gluEndPolygon call.
	    gluTessVertex(tobj, v, &t);
	}
    }
    gluEndPolygon(tobj);

    // If there was an error tesselating the character, just generate
    // a bounding box for the character:
    if (tesselationError) {
	SbBox2f charBBox;
	getCharBBox(c, charBBox);
	if (!charBBox.isEmpty()) {
	    SbVec2f boxVerts[4];
	    charBBox.getBounds(boxVerts[0], boxVerts[2]);
	    boxVerts[1].setValue(boxVerts[2][0], boxVerts[0][1]);
	    boxVerts[3].setValue(boxVerts[0][0], boxVerts[2][1]);

	    gluBeginPolygon(tobj);
	    for (i = 0; i < 4; i++) {
		v[0] = boxVerts[i][0];
		v[1] = boxVerts[i][1];
		v[2] = 0.0;
		gluTessVertex(tobj, v, &boxVerts[i]);
	    }
	    gluEndPolygon(tobj);
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets up for GL rendering.
//
// Use: internal

void
SoOutlineFontCache::setupToRenderFront(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
    otherOpen = SoCacheElement::anyOpen(state);
    if (!otherOpen && !frontList) {
	frontList = new SoGLDisplayList(state,
					SoGLDisplayList::DISPLAY_LIST,
					numChars);
	frontList->ref();
    }
    if (frontList) {
	// Set correct list base
	glListBase(frontList->getFirstIndex());
	frontList->addDependency(state);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets up for GL rendering.
//
// Use: internal

void
SoOutlineFontCache::setupToRenderSide(SoState *state, SbBool willTexture)
//
////////////////////////////////////////////////////////////////////////
{
    otherOpen = SoCacheElement::anyOpen(state);
    if (!otherOpen && !sideList) {
	sideList = new SoGLDisplayList(state,
					SoGLDisplayList::DISPLAY_LIST,
					numChars);
	sideList->ref();
	sidesHaveTexCoords = willTexture;
    }
    if (sideList) {
	// Set correct list base
	glListBase(sideList->getFirstIndex());
	sideList->addDependency(state);
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
SoOutlineFontCache::hasFrontDisplayList(const char* c,
					gluTESSELATOR *tobj)
//
////////////////////////////////////////////////////////////////////////
{
    // If we have one, return TRUE
    long key = (c[0]<<8) | c[1];
    void *value;
    if (frontDict->find(key, value)) return TRUE;
    
    // If we don't and we can't build one, return FALSE.
    if (otherOpen) return FALSE;
    
    // Build one:
    glNewList(frontList->getFirstIndex()+key, GL_COMPILE);
    generateFrontChar(c, tobj);
    SbVec2f t = getOutline(c)->getCharAdvance();
    glTranslatef(t[0], t[1], 0.0);
    glEndList();
    
    frontDict->enter(key, value);

    return TRUE;
}
////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if a display lists exists for given character.
//    Tries to build a display list, if it can.
//
// Use: internal

SbBool
SoOutlineFontCache::hasSideDisplayList(const char* c,
				       SideCB callbackFunc)
//
////////////////////////////////////////////////////////////////////////
{
    long key = (c[0]<<8) | c[1];
    void *value;
    // If we have one, return TRUE
    if (sideDict->find(key, value)) return TRUE;  
    
    // If we don't and we can't build one, return FALSE.
    if (otherOpen) return FALSE;
    
    // Build one:
    glNewList(sideList->getFirstIndex()+key, GL_COMPILE);

    glBegin(GL_QUADS);    // Render as independent quads:
    generateSideChar(c, callbackFunc);
    glEnd();

    SbVec2f t = getOutline(c)->getCharAdvance();
    glTranslatef(t[0], t[1], 0.0);
    glEndList();
    sideDict->enter(key, value);

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
SoOutlineFontCache::callFrontLists(int line)
//
////////////////////////////////////////////////////////////////////////
{
    const char *str = getUCSString(line);

    glCallLists(getNumUCSChars(line), GL_2_BYTES, str);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Assuming that there are display lists built for all the
//    characters in given line, render them using the GL's CallLists
//    routine.
//
// Use: internal

void
SoOutlineFontCache::callSideLists(int line)
//
////////////////////////////////////////////////////////////////////////
{
    const char *str = getUCSString(line);

    glCallLists(getNumUCSChars(line), GL_2_BYTES, str);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Assuming that there are not display lists built for all the
//    characters in given string, render the string.
//
// Use: internal

void
SoOutlineFontCache::renderFront(int line,
				gluTESSELATOR *tobj)
//
////////////////////////////////////////////////////////////////////////
{
    const char *str = getUCSString(line);

    void *value;
    for (int i = 0; i < getNumUCSChars(line); i++) {
	long key = str[2*i]<<8 | str[2*i+1];	
	if (frontDict->find(key, value)) {
	    glCallList(frontList->getFirstIndex()+key);
	}
	else {
	    generateFrontChar(str+2*i, tobj);
	    SbVec2f t = getOutline(str+2*i)->getCharAdvance();
	    glTranslatef(t[0], t[1], 0.0);
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Assuming that there are not display lists built for all the
//    characters in given line, render the line.
//
// Use: private internal

void
SoOutlineFontCache::renderSide(int line,
			       SideCB callbackFunc)
//
////////////////////////////////////////////////////////////////////////
{
    const char *str = getUCSString(line);
    void* value;
    for (int i = 0; i < getNumUCSChars(line); i++) {
	long key = (str[2*i]<<8)|str[2*i+1];
	if (sideDict->find(key, value)) {
	    glCallList(sideList->getFirstIndex()+key);
	}
	else {
	    glBegin(GL_QUADS);
	    generateSideChar(str+2*i, callbackFunc);
	    glEnd();

	    SbVec2f t = getOutline(str+2*i)->getCharAdvance();
	    glTranslatef(t[0], t[1], 0.0);
	}
    }
}
////////////////////////////////////////////////////////////////////////
//
// Description:
//    Convert MFString to UCS string, if necessary.
//
// Use: internal

void
SoOutlineFontCache::convertToUCS(uint32_t nodeid,
			       const SoMFString& strings)			       
//
////////////////////////////////////////////////////////////////////////
{
    if (nodeid == currentNodeId) return;
    currentNodeId = nodeid;
    
    //delete previously converted UCS string
    for (int i = 0; i< UCSStrings.getLength(); i++){
	delete [] UCSStrings[i];
    }
    UCSStrings.truncate(0);
    UCSNumChars.truncate(0);
 
    //make sure conversion code already set:
    if (conversionCode == NULL){
	conversionCode = iconv_open("UCS-2", "UTF-8");	
    }
    
    if ( conversionCode == (iconv_t)-1 ){
#ifdef DEBUG
	SoDebugError::post("SoOutlineFontCache::convertToUCS", 
		"Invalid UTF-8 to UCS-2 conversion");
#endif /*DEBUG*/
	return;
    }
    
    //for each line of text, allocate a sufficiently large buffer
    //An extra two bytes are allocated.
    for (i = 0; i< strings.getNum(); i++){
	UCSStrings[i] = new char[2*strings[i].getLength()+2];

	char* input = (char *)strings[i].getString();
	size_t inbytes = strings[i].getLength();
	size_t outbytes = 2*inbytes+2;
	char* output = (char*)UCSStrings[i];
    
	if ((iconv(conversionCode, &input, &inbytes, &output, &outbytes) == (size_t)-1)){
#ifdef DEBUG
	    SoDebugError::post("SoOutlineFontCache::convertToUCS", 
		"Error converting text to UCS-2");
#endif /*DEBUG*/ 

	}
    
	if (inbytes){
#ifdef DEBUG
	    SoDebugError::post("SoOutlineFontCache::convertToUCS", 
		"Incomplete conversion to UCS-2");
#endif /*DEBUG*/
	    return;
	}
    
	UCSNumChars[i] = (void*)((2*strings[i].getLength()+2 - outbytes)>>1);
       
    }
 
    return;
    
}
////////////////////////////////////////////////////////////////////////
//
// Description:
//   Copy info from the font library into a more convenient form.
//
// Use: internal

SoFontOutline::SoFontOutline(FLoutline *outline, float fontSize)
//
////////////////////////////////////////////////////////////////////////
{
    charAdvance = SbVec2f(outline->xadvance,
			  outline->yadvance)*fontSize;
    numOutlines = outline->outlinecount;
    if (numOutlines != 0) {
	numVerts = new int[numOutlines];
	verts = new SbVec2f*[numOutlines];
	for (int i = 0; i < numOutlines; i++) {
	    numVerts[i] = outline->vertexcount[i];
	    if (numVerts[i] != 0) {
		verts[i] = new SbVec2f[numVerts[i]];
		for (int j = 0; j < numVerts[i]; j++) {
		    verts[i][j] = SbVec2f(outline->vertex[i][j].x,
					  outline->vertex[i][j].y)*fontSize;
		}
	    } else {
		verts[i] = NULL;
	    }
	}
    } else {
	numVerts = NULL;
	verts = NULL;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   Destructor; free up outline storage
//
// Use: internal

SoFontOutline::~SoFontOutline()
//
////////////////////////////////////////////////////////////////////////
{
    for (int i = 0; i < numOutlines; i++) {
	if (numVerts[i] != 0)
	    delete[] verts[i];
    }
    if (numOutlines != 0) {
	delete[] verts;
	delete[] numVerts;
    }
}
	    
////////////////////////////////////////////////////////////////////////
//
// Description:
//   Get a do-nothing outline:
//
// Use: internal, static

SoFontOutline *
SoFontOutline::getNullOutline()
//
////////////////////////////////////////////////////////////////////////
{
    return new SoFontOutline;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   Internal constructor used by getNullOutline
//
// Use: internal, static

SoFontOutline::SoFontOutline()
//
////////////////////////////////////////////////////////////////////////
{
    charAdvance = SbVec2f(0,0);
    numOutlines = 0;
    numVerts = NULL;
    verts = NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   Find the first and last points in the bevel-- that is where the
//   front and back of the character will be.
// Use:
//   internal

void
SoOutlineFontCache::getProfileBounds(float &firstZ, float &lastZ)
//
////////////////////////////////////////////////////////////////////////
{
    if (hasProfile()) {
	firstZ = -profileVerts[0][0];
	lastZ = -profileVerts[nProfileVerts-1][0];
    } else {
	firstZ = lastZ = 0;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   Get the 2D bounding box of the bevel.
//
//  Use:
//    internal

void
SoOutlineFontCache::getProfileBBox(SbBox2f &profileBox)
//
////////////////////////////////////////////////////////////////////////
{
    for (int i = 0; i < nProfileVerts; i++) {
	profileBox.extendBy(profileVerts[i]);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Called by generateSide.  This generates the bevel triangles for
//    one character.
//
// Use: private

void
SoOutlineFontCache::generateSideChar(const char* c, SideCB callbackFunc)
//
////////////////////////////////////////////////////////////////////////
{
    if (!hasProfile()) return;

    // Get the outline of the character:
    SoFontOutline *outline = getOutline(c);
    
    for (int i = 0; i < outline->getNumOutlines(); i++) {
	// For each outline:

	int nOutline = outline->getNumVerts(i);

	SbVec2f *oVerts = new SbVec2f[nOutline];
	// Copy in verts so figureSegmentNorms can handle them..
	for (int j = 0; j < nOutline; j++) {
	    oVerts[j] = outline->getVertex(i, j);
	}

	// First, figure out a set of normals for the outline:
	SbVec2f *oNorms = new SbVec2f[nOutline*2];
	figureSegmentNorms(oNorms, nOutline, oVerts, cosCreaseAngle, TRUE);

	// And appropriate texture coordinates:
	// Figure out T texture coordinates, which run along the
	// outline:
	float *tTexCoords = new float[nOutline+1];
	figureSegmentTexCoords(tTexCoords, nOutline, oVerts, TRUE);

	// Now, generate a set of triangles for each segment in the
	// outline.  A bevel of profiles is built at each point in the
	// outline; each profile must be flipped perpendicular to the
	// outline (x coordinate becomes -z), rotated to be the
	// average of the normals of the two adjoining segments at
	// that point, and translated to that point.  Triangles are
	// formed between consecutive bevels.
	// Normals are just taken from the 'pNorms' array, after being
	// rotated the appropriate amount.

	SbVec3f *bevel1 = new SbVec3f[nProfileVerts];
	SbVec3f *bevelN1 = new SbVec3f[(nProfileVerts-1)*2];
	SbVec3f *bevel2 = new SbVec3f[nProfileVerts];
	SbVec3f *bevelN2 = new SbVec3f[(nProfileVerts-1)*2];
	    
	// fill out first bevel:
	fillBevel(bevel1, (int) nProfileVerts, profileVerts,
		  outline->getVertex(i,0),
		  oNorms[(nOutline-1)*2+1], oNorms[0*2]);
	
	SbVec3f *s1 = bevel1;
	SbVec3f *s2 = bevel2;
	
	for (j = 0; j < nOutline; j++) {
	    // New normals are calculated for both ends of this
	    // segment, since the normals may or may not be shared
	    // with the previous segment.
	    fillBevelN(bevelN1, (int)(nProfileVerts-1)*2, profileNorms,
		       oNorms[j*2]);

	    int j2 = (j+1)%nOutline;
	    // fill out second bevel:
	    fillBevel(s2, (int) nProfileVerts, profileVerts,
		      outline->getVertex(i,j2),
		      oNorms[j*2+1], oNorms[j2*2]);
	    fillBevelN(bevelN2, (int)(nProfileVerts-1)*2, profileNorms,
		       oNorms[j*2+1]);

	    // And generate triangles between the two bevels:
	    (*callbackFunc)((int) nProfileVerts, s1, bevelN1, s2, bevelN2,
			     sTexCoords, &tTexCoords[j]);

	    // Swap bevel1/2 (avoids some recomputation)
	    SbVec3f *t;
	    t = s1; s1 = s2; s2 = t;
	}
	delete [] bevelN2;
	delete [] bevel2;
	delete [] bevelN1;
	delete [] bevel1;
	delete [] tTexCoords;
	delete [] oNorms;
	delete [] oVerts;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Given a set of line segments, this figures out the normal at
//    each point in each segment.  It uses the creaseAngle passed in
//    to decide whether or not adjacent segments share normals.  The
//    isClosed flag is used to determine whether or not the first and
//    last points should be considered another segment.
//
//
// Use: private

void
SoOutlineFontCache::figureSegmentNorms(SbVec2f *norms, int nPoints,
			    const SbVec2f *points,  float cosCreaseAngle,
			    SbBool isClosed)
//
////////////////////////////////////////////////////////////////////////
{
    int nSegments;
    
    if (isClosed) nSegments = nPoints;
    else nSegments = nPoints-1;
    
    // First, we'll just make all the normals perpendicular to their
    // segments:
    for (int i = 0; i < nSegments; i++) {
	SbVec2f n;
	// This is 2D perpendicular, assuming profile is increasing in
	// X (which becomes 'decreasing in Z' when we actually use
	// it...) (note: if a profile isn't increasing in X, the
	// character will be inside-out, with the front face drawn
	// behind the back face, etc).
	SbVec2f v = points[(i+1)%nPoints] - points[i];
	n[0] = v[1];
	n[1] = -v[0];
	n.normalize();
	
	norms[i*2] = n;
	norms[i*2+1] = n;
    }
    // Now, figure out if the angle between any two segments is small
    // enough to average two of their normals.
    for (i = 0; i < (isClosed ? nSegments : nSegments-1); i++) {
	SbVec2f seg1 = points[(i+1)%nPoints] - points[i];
	seg1.normalize();
	SbVec2f seg2 = points[(i+2)%nPoints] - points[(i+1)%nPoints];
	seg2.normalize();
	
	float dp = seg2.dot(seg1);
	if (dp > cosCreaseAngle) {
	    // Average the second normal for this segment, and the
	    // first normal for the next segment:
	    SbVec2f average = norms[i*2+1] + norms[(i+1)*2];
	    average.normalize();
	    norms[i*2+1] = average;
	    norms[((i+1)%nPoints)*2] = average;
	}
    }
}    

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Given a set of line segments, this figures out the texture
//    coordinates for each vertex.  If the isClosed flag is TRUE,
//    an extra texture coordinate is calculated, AND the points are
//    traversed in reverse order.
//
// Use: private

void
SoOutlineFontCache::figureSegmentTexCoords(float *texCoords, int nPoints,
			    const SbVec2f *points, SbBool isClosed)
//
////////////////////////////////////////////////////////////////////////
{
    float total = 0.0;
    
    int i;

    if (isClosed) {
	for (i = nPoints; i >= 0; i--) {
	    texCoords[i] = total / getHeight();
	    if (i > 0) {
		total += (points[i%nPoints] - points[i-1]).length();
	    }
	}
    } else {
	for (int i = 0; i < nPoints; i++) {
	    texCoords[i] = total / getHeight();
	    if (i+1 < nPoints) {
		total += (points[i+1] - points[i]).length();
	    }
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Given a set of segments that make up a profile or bevel along
//    which we'll extrude the front face of the text, this routine
//    transforms the bevel from its default orientation (in the x-y
//    plane) to its correct position for a particular point on the
//    text outline, rotated and translated into position.  The
//    translation is the point on the outline, and the two normals
//    passed in are the normals for the segments adjoining that point.
//
// Use: private

void
SoOutlineFontCache::fillBevel(SbVec3f *result, int nPoints,
	  const SbVec2f *points,
	  const SbVec2f &translation,
	  const SbVec2f &n1, const SbVec2f &n2)
//
////////////////////////////////////////////////////////////////////////
{
    // First, figure out a rotation for this bevel:
    SbVec2f n = n1+n2;
    n.normalize();
    
    // Now, for each point:
    for (int i = 0; i < nPoints; i++) {
	// This is really the 2D rotation formula,
	// x = x' cos(angle) - y' sin(angle)
	// y = x' sin(angle) + y' cos(angle)
	// Because of the geometry, cos(angle) is n[1] and sin(angle)
	// is -n[0], and x' is zero (the bevel always goes straight
	// back).
	result[i][0] = points[i][1] * n[0] + translation[0];
	result[i][1] = points[i][1] * n[1] + translation[1];
	result[i][2] = -points[i][0];
    }
} 

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Given a set of normals for a profile, this rotates the normals
//    from their default position (int the x-y plane) to the correct
//    orientation for a particular point on the texts outline.  The
//    normal passed in is the normal for one end of one of the
//    outline's segments.

void
SoOutlineFontCache::fillBevelN(SbVec3f *result, int nNorms,
	  const SbVec2f *norms,
	  const SbVec2f &n)
//
////////////////////////////////////////////////////////////////////////
{
    // Now, for each point:
    for (int i = 0; i < nNorms; i++) {
	// This is really the 2D rotation formula,
	// x = x' cos(angle) - y' sin(angle)
	// y = x' sin(angle) + y' cos(angle)
	// Because of the geometry, cos(angle) is n[1] and sin(angle)
	// is -n[0], and x' is zero (the bevel always goes straight
	// back).
	result[i][0] = norms[i][1] * n[0];
	result[i][1] = norms[i][1] * n[1];
	result[i][2] = -norms[i][0];
    }
} 

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Called by the GLU tesselator when there is an error
//
// Use: static, private
//
////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
void
SoOutlineFontCache::errorCB(GLenum whichErr)
{
    SoDebugError::post("SoText3::errorCB", "%s", gluErrorString(whichErr));
    tesselationError = TRUE;
}
#else  /* DEBUG */
void
SoOutlineFontCache::errorCB(GLenum)
{
    tesselationError = TRUE;
}
#endif /* DEBUG */

////////////////////////////////////////////////////////////////////////
//
// Description:
//    function used to free up outline storage
//
// Use: static, private
//
////////////////////////////////////////////////////////////////////////

void SoOutlineFontCache::freeOutline(unsigned long, void* value) 
{
    delete (FLoutline*)value;    
}
#else /*IRIX_6*/
//Following version is for Irix 5:

#include <GL/gl.h>
#include <GL/glu.h>
#include <Inventor/SbBox.h>
#include <Inventor/SoPrimitiveVertex.h>
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/bundles/SoMaterialBundle.h>
#include <Inventor/caches/SoCache.h>
#include <Inventor/details/SoTextDetail.h>
#include <Inventor/elements/SoCacheElement.h>
#include <Inventor/elements/SoComplexityElement.h>
#include <Inventor/elements/SoComplexityTypeElement.h>
#include <Inventor/elements/SoCreaseAngleElement.h>
#include <Inventor/elements/SoFontNameElement.h>
#include <Inventor/elements/SoFontSizeElement.h>
#include <Inventor/elements/SoGLCacheContextElement.h>
#include <Inventor/elements/SoGLTextureCoordinateElement.h>
#include <Inventor/elements/SoGLTextureEnabledElement.h>
#include <Inventor/elements/SoMaterialBindingElement.h>
#include <Inventor/elements/SoModelMatrixElement.h>
#include <Inventor/elements/SoProfileElement.h>
#include <Inventor/elements/SoProfileCoordinateElement.h>
#include <Inventor/elements/SoProjectionMatrixElement.h>
#include <Inventor/elements/SoViewportRegionElement.h>
#include <Inventor/elements/SoViewingMatrixElement.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/misc/SoState.h>
#include <Inventor/nodes/SoProfile.h>
#include <Inventor/nodes/SoText3.h>

// Font library:
#include <flclient.h>

// First, a more convenient structure for outlines:
class SoFontOutline {

  public:
    // Constructor, takes a pointer to the font-library outline
    // structure and the font's size:
    SoFontOutline(FLoutline *outline, float fontSize);
    // Destructor
    ~SoFontOutline();

    // Query routines:
    int		getNumOutlines() { return numOutlines; }
    int		getNumVerts(int i) { return numVerts[i]; }
    SbVec2f	&getVertex(int i, int j) { return verts[i][j]; }
    SbVec2f	getCharAdvance() { return charAdvance; }
    
    static SoFontOutline *getNullOutline();

  private:
    // Internal constructor used by getNullOutline:
    SoFontOutline();
    
    // This basically mimics the FLoutline structure, with the
    // exception that the font size is part of the outline:
    int numOutlines;
    int *numVerts;
    SbVec2f **verts;
    SbVec2f charAdvance;
};

//
// Internal class: SoOutlineFontCache
//

// Callback function for sides of characters-- passed the number of
// points going back, and points and normals on either edge of the
// strip.  tTexCoords[0] and [1] are for the two edges, and the
// sTexCoords are the same for both edges.
typedef void SideCB(int nPoints,
		    const SbVec3f *points1, const SbVec3f *norms1,
		    const SbVec3f *points2, const SbVec3f *norms2,
		    const float *sTexCoords, const float *tTexCoords);

// This is pretty heavyweight-- it is responsible for doing all of the
// grunt work of figuring out the polygons making up the characters in
// the font.
class SoOutlineFontCache : public SoCache
{
  public:
    // Given a state, find an appropriate outline font.
    static SoOutlineFontCache	*getFont(SoState *, SbBool forRender);
    
    // Checks to see if this font is valid
    SbBool	isValid(SoState *state) const;

    // Figures out if this cache is valid for rendering (the base
    // class isValid can be used for all other actions)
    SbBool	isRenderValid(SoState *state) const;

    // Returns the width of given string
    float	getWidth(const SbString &string);
    
    // Returns height of font
    float	getHeight() { return fontSize; }

    // Returns the 2D bounding box of a character
    void	getCharBBox(const char c, SbBox2f &result);
    // ... and the bounding box of the font's bevel
    void	getProfileBBox(SbBox2f &result);
    
    // Return the first/last point in the profile:
    void	getProfileBounds(float &firstZ, float &lastZ);

    // Returns TRUE if there _is_ any profile
    // (if not, act as if SIDES of text are off)
    SbBool	hasProfile() const { return  (nProfileVerts > 1); }

    // Returns how far to advance after drawing given character:
    SbVec2f	getCharOffset(const char c);
    
    // Uses the given glu tesselator to generate triangles for the
    // given character.  This is used for both rendering and
    // generating primitives, with just different callback routines
    // registered.
    void	generateFrontChar(const char c, gluTESSELATOR *tobj);
    // Ditto, for sides of characters:
    void	generateSideChar(const char c, SideCB callbackFunc);

    // Set up for GL rendering:
    void	setupToRenderFront(SoState *state);
    void	setupToRenderSide(SoState *state, SbBool willTexture);

    // Returns TRUE if this font cache has a display list for the
    // given character.  It will try to build a display list, if it
    // can.
    SbBool	hasFrontDisplayList(const char c, gluTESSELATOR *tobj);
    SbBool	hasSideDisplayList(const char c, SideCB callbackFunc);

    // Renders an entire string by using the GL callList() function.
    void	callFrontLists(const SbString &string);
    void	callSideLists(const SbString &string);

    // Renders a string in cases where display lists can't be buit.
    void	renderFront(const SbString &string,
			    gluTESSELATOR *tobj);
    void	renderSide(const SbString &string,
			   SideCB callbackFunc);

    // Callback registered with GLU used to detect tesselation errors.
    static void errorCB(GLenum whichErr);

  protected:

    // Free up display lists before being deleted
    virtual void	destroy(SoState *state);

  private:
    // Constructor
    SoOutlineFontCache(SoState *);
    // Destructor
    ~SoOutlineFontCache();

    // Return a convnient little class representing a character's
    // outline.
    SoFontOutline	*getOutline(const char c);
    
    // Some helper routines for generateSide:
    void figureSegmentNorms(SbVec2f *result, int nPoints,
	 const SbVec2f *points, float cosCreaseAngle, SbBool isClosed);
    void figureSegmentTexCoords(float *texCoords, int nPoints,
	 const SbVec2f *points, SbBool isClosed);
    void fillBevel(SbVec3f *result, int nPoints, 
		   const SbVec2f *points, const SbVec2f &translation,
		   const SbVec2f &n1, const SbVec2f &n2);
    void fillBevelN(SbVec3f *result, int nPoints, 
		    const SbVec2f *norms, const SbVec2f &n);

    // Texture coordinates in side display lists
    int		sidesHaveTexCoords;

    // Number of characters in this font. Until we internationalize,
    // this will be 128 or less.
    int		numChars;

    // Display lists for fronts, sides:
    SoGLDisplayList *frontList;
    SoGLDisplayList *sideList;

    // Profile information:
    float	cosCreaseAngle;
    int32_t	nProfileVerts;	// Number of points in profile
    SbVec2f	*profileVerts;	// Profile vertices
    float	*sTexCoords;	// Texture coordinates along profile
    				// (nProfileVerts of them)
    SbVec2f	*profileNorms;	// Profile normals
				// ((nProfileVerts-1)*2 of them)

    // This flag will be true if there is another cache open (if
    // building GL display lists for render caching, that means we
    // can't also build display lists).
    SbBool	otherOpen;

    // And tables telling us if a display list has been created for
    // each character in the font (we do that lazily since it is
    // expensive):
    SbBool	*frontFlags;
    SbBool	*sideFlags;

    // List of outlines; these are also cached and created when
    // needed.
    SoFontOutline	**outlines;

    // Font size
    float	fontSize;

    // Flag used to detect tesselation errors:
    static SbBool tesselationError;

    // Font library identifier for this font
    FLfontNumber	fontId;

    // Font library context for all outline fonts
    static FLcontext	context;

    // Global list of available fonts; a 'font' in this case is a
    // unique set of font name, font size, complexity value/type, and
    // set of profiles-- if any of these changes, the set of polygons
    // representing the font will change, and a different font will be
    // used.
    static SbPList	*fonts;
};

SbBool SoOutlineFontCache::tesselationError = FALSE;
SbPList *SoOutlineFontCache::fonts = NULL;
FLcontext SoOutlineFontCache::context = NULL;

// Static stuff is used while generating primitives:
SoText3 *SoText3::currentGeneratingNode = NULL;
SoPrimitiveVertex *SoText3::genPrimVerts[3];
SbVec3f SoText3::genTranslate;
int SoText3::genWhichVertex = -1;
uint32_t SoText3::genPrimType;
SoAction *SoText3::genAction = NULL;
SbBool SoText3::genBack = FALSE;
SbBool SoText3::genTexCoord = TRUE;
const SoTextureCoordinateElement *SoText3::tce = NULL;

SO_NODE_SOURCE(SoText3);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoText3::SoText3()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoText3);

    SO_NODE_ADD_FIELD(string,	(""));
    SO_NODE_ADD_FIELD(spacing,	(1.0));
    SO_NODE_ADD_FIELD(justification,	(LEFT));
    SO_NODE_ADD_FIELD(parts,		(FRONT));

    // Set up static info for enumerated type field
    SO_NODE_DEFINE_ENUM_VALUE(Justification,	LEFT);
    SO_NODE_DEFINE_ENUM_VALUE(Justification,	RIGHT);
    SO_NODE_DEFINE_ENUM_VALUE(Justification,	CENTER);

    // Set up static info for enumerated type field
    SO_NODE_DEFINE_ENUM_VALUE(Part, SIDES);
    SO_NODE_DEFINE_ENUM_VALUE(Part, FRONT);
    SO_NODE_DEFINE_ENUM_VALUE(Part, BACK);
    SO_NODE_DEFINE_ENUM_VALUE(Part, ALL);

    // Set up info in enumerated type field
    SO_NODE_SET_SF_ENUM_TYPE(justification, Justification);
    SO_NODE_SET_SF_ENUM_TYPE(parts, Part);

    isBuiltIn = TRUE;
    myFont = NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoText3::~SoText3()
//
////////////////////////////////////////////////////////////////////////
{
    if (myFont != NULL) myFont->unref();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Figure out the bounds of a character.
//
// Use: extender

SbBox3f
SoText3::getCharacterBounds(SoState *state, int stringIndex, int
			    charIndex)
//
////////////////////////////////////////////////////////////////////////
{
    SbBox3f result;

    if (!setupFontCache(state))
	return result;  // Empty bbox
    
#ifdef DEBUG
    if (stringIndex >= string.getNum()) {
	SoDebugError::post("SoText3::getCharacterBounds",
			   "stringIndex (%d) out of range (max %d)",
			   stringIndex, string.getNum());
    }
    if (charIndex >= string[stringIndex].getLength()) {
	SoDebugError::post("SoText3::getCharacterBounds",
			   "charIndex (%d) out of range (max %d)",
			   charIndex,
			   string[stringIndex].getLength());
    }
#endif

    float frontZ, backZ;
    myFont->getProfileBounds(frontZ, backZ);
    
    float height = myFont->getHeight();

    const char *chars = string[stringIndex].getString();
    float width = (myFont->getCharOffset(chars[charIndex]))[0];
    
    // Figure out where origin of character is:
    SbVec2f charPosition = getStringOffset(stringIndex);
    for (int i = 0; i < charIndex; i++) {
	charPosition += myFont->getCharOffset(chars[i]);
    }
    
    // Ok, have width, height, depth and starting position of text,
    // can create the bounds box:
    if (parts.getValue() & (FRONT|SIDES)) {
	result.extendBy(SbVec3f(charPosition[0],
				charPosition[1],
				frontZ));
	result.extendBy(SbVec3f(charPosition[0]+width,
				charPosition[1],
				frontZ));
	result.extendBy(SbVec3f(charPosition[0],
				charPosition[1]+height,
				frontZ));
	result.extendBy(SbVec3f(charPosition[0]+width,
				charPosition[1]+height,
				frontZ));
    }
    if (parts.getValue() & (BACK|SIDES)) {
	result.extendBy(SbVec3f(charPosition[0],
				charPosition[1],
				backZ));
	result.extendBy(SbVec3f(charPosition[0]+width,
				charPosition[1],
				backZ));
	result.extendBy(SbVec3f(charPosition[0],
				charPosition[1]+height,
				backZ));
	result.extendBy(SbVec3f(charPosition[0]+width,
				charPosition[1]+height,
				backZ));
    }
    return result;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Performs GL rendering of a Text3.
//
// Use: extender

void
SoText3::GLRender(SoGLRenderAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    static gluTESSELATOR *tobj = NULL;

    // First see if the object is visible and should be rendered now
    if (! shouldGLRender(action))
	return;

    SoState *state = action->getState();

    if (!setupFontCache(state, TRUE))
	return;

    SoMaterialBindingElement::Binding mbe =
	SoMaterialBindingElement::get(state);
    SbBool materialPerPart =
	(mbe == SoMaterialBindingElement::PER_PART_INDEXED ||
	 mbe == SoMaterialBindingElement::PER_PART);

    SoMaterialBundle	mb(action);
    if (!materialPerPart) {
	// Make sure the fist current material is sent to GL
	mb.sendFirst();
    }

    float firstZ, lastZ;
    myFont->getProfileBounds(firstZ, lastZ);

    if (tobj == NULL) {
	tobj = gluNewTess();
	gluTessCallback(tobj, (GLenum)GLU_BEGIN, (void (*)())glBegin);
	gluTessCallback(tobj, (GLenum)GLU_END, (void (*)())glEnd);
	gluTessCallback(tobj, (GLenum)GLU_VERTEX, (void (*)())glVertex2fv);
	gluTessCallback(tobj, (GLenum)GLU_ERROR,
			(void (*)())SoOutlineFontCache::errorCB);
    }

    // See if texturing is enabled
    genTexCoord = SoGLTextureEnabledElement::get(action->getState());

    if ((parts.getValue() & SIDES) && (myFont->hasProfile())) {
	if (materialPerPart) mb.send(1, FALSE);

	myFont->setupToRenderSide(state, genTexCoord);
	for (int line = 0; line < string.getNum(); line++) {
	    glPushMatrix();
	    SbVec2f p = getStringOffset(line);
	    if (p[0] != 0.0 || p[1] != 0.0)
		glTranslatef(p[0], p[1], 0.0);
	    renderSide(action, line);
	    glPopMatrix();
	}
    }
    if (parts.getValue() & BACK) {
	if (materialPerPart) mb.send(2, FALSE);

	if (lastZ != 0.0) {
	    glTranslatef(0, 0, lastZ);
	}
	glNormal3f(0, 0, -1);
	glFrontFace(GL_CW);

	myFont->setupToRenderFront(state);
	
	if (genTexCoord) {
	    glPushAttrib(GL_TEXTURE_BIT);
	    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	    GLfloat params[4];
	    params[0] = -1.0/myFont->getHeight();
	    params[1] = params[2] = params[3] = 0.0;
	    glTexGenfv(GL_S, GL_OBJECT_PLANE, params);
	    params[1] = -params[0];
	    params[0] = 0.0;
	    glTexGenfv(GL_T, GL_OBJECT_PLANE, params);
	    
	    glEnable(GL_TEXTURE_GEN_S);
	    glEnable(GL_TEXTURE_GEN_T);
	}
	
	for (int line = 0; line < string.getNum(); line++) {
	    if (string[line].getLength() <= 0) continue;
	    
	    glPushMatrix();
	    SbVec2f p = getStringOffset(line);
	    if (p[0] != 0.0 || p[1] != 0.0)
		glTranslatef(p[0], p[1], 0.0);
	    renderFront(action, line, tobj);
	    glPopMatrix();
	}
	
	if (genTexCoord) {
	    glPopAttrib();
	}

	glFrontFace(GL_CCW);

	if (lastZ != 0)
	    glTranslatef(0, 0, -lastZ);
    }	
    if (parts.getValue() & FRONT) {
	if (materialPerPart) mb.sendFirst();

	if (firstZ != 0.0) {
	    glTranslatef(0, 0, firstZ);
	}

	glNormal3f(0, 0, 1);
	
	myFont->setupToRenderFront(state);

	if (genTexCoord) {
	    glPushAttrib(GL_TEXTURE_BIT);
	    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	    GLfloat params[4];
	    params[0] = 1.0/myFont->getHeight();
	    params[1] = params[2] = params[3] = 0.0;
	    glTexGenfv(GL_S, GL_OBJECT_PLANE, params);
	    params[1] = params[0];
	    params[0] = 0.0;
	    glTexGenfv(GL_T, GL_OBJECT_PLANE, params);
	    
	    glEnable(GL_TEXTURE_GEN_S);
	    glEnable(GL_TEXTURE_GEN_T);
	}
	
	for (int line = 0; line < string.getNum(); line++) {
	    glPushMatrix();
	    SbVec2f p = getStringOffset(line);
	    if (p[0] != 0.0 || p[1] != 0.0)
		glTranslatef(p[0], p[1], 0.0);
	    renderFront(action, line, tobj);
	    glPopMatrix();
	}
	
	if (genTexCoord) {
	    glPopAttrib();
	}

	if (firstZ != 0.0) {
	    glTranslatef(0, 0, -firstZ);
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements ray picking
//
// Use: extender

void
SoText3::rayPick(SoRayPickAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    // First see if the object is pickable
    if (! shouldRayPick(action))
	return;

    //
    // NOTE: This could be made more efficient by testing the ray
    // against the bounding box of each character, as follows:
    //
    // 	-- For each string {
    // 	    -- For each character in the string: {
    // 		If the ray intersects the character's bounding box,
    // 		  generate primitives for that character
    // 	    }
    // 	}
    //
    // This is a project for the future... ???

    // For now, just use generatePrimitives() to do the work.
    SoShape::rayPick(action);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Computes bounding box of text.
//
// Use: protected

void
SoText3::computeBBox(SoAction *action, SbBox3f &box, SbVec3f &center)
//
////////////////////////////////////////////////////////////////////////
{
    // Set the center to be the origin, which is the natural "center"
    // of the text, regardless of justification
    center.setValue(0.0, 0.0, 0.0);

    int prts = parts.getValue();
    if (prts == 0)
	return;

    SoState *state = action->getState();

    if (!setupFontCache(state))
	return;

    // Get the bounding box of all the characters:
    SbBox2f outlineBox;
    getFrontBBox(outlineBox);

    // If no lines and no characters, return empty bbox:
    if (outlineBox.isEmpty()) return;
    
    // .. and extend it based on what parts are turned on:
    float firstZ, lastZ;
    myFont->getProfileBounds(firstZ, lastZ);

    const SbVec2f &boxMin = outlineBox.getMin();
    const SbVec2f &boxMax = outlineBox.getMax();
		     

    // Front and back are straightforward:
    if (prts & FRONT) {
	SbVec3f min(boxMin[0], boxMin[1], firstZ);
	SbVec3f max(boxMax[0], boxMax[1], firstZ);
	box.extendBy(min);
	box.extendBy(max);
    }
    if (prts & BACK) {
	SbVec3f min(boxMin[0], boxMin[1], lastZ);
	SbVec3f max(boxMax[0], boxMax[1], lastZ);
	box.extendBy(min);
	box.extendBy(max);
    }
    //
    // Sides are trickier.  We figure out the maximum offset
    // of the profile we're using.  If the offset is
    // negative, we use its absolute value; normally, negative
    // (indented) characters won't expand the bounding box, but if the
    // offset is negative enough the character will inter-penetrate
    // itself-- think of a Helvetica 'I', with a bevel of a big
    // negative spike.  The bounding box is either the bounding box of
    // the front/back, or, if the spike is big enough, the size of the
    // spike minus the width of the I.  I'm being conservative here
    // and just expanding the front/back bounding boxes by the maximum
    // offset (correct for positive offsets, conservative for negative
    // offsets).
    //
    if ((prts & SIDES) && myFont->hasProfile()) {
	SbBox2f profileBox;
	myFont->getProfileBBox(profileBox);

	const SbVec2f &pBoxMin = profileBox.getMin();
	const SbVec2f &pBoxMax = profileBox.getMax();
	
	// If no profile, return the front/back bbox:
	if (profileBox.isEmpty()) return;

	//
	// Expand the bounding box forward/backward in case the
	// profile extends forwards/backwards:
	//
	SbVec3f min, max;
	min.setValue(boxMin[0], boxMin[1], pBoxMin[0]);
	max.setValue(boxMax[0], boxMax[1], pBoxMax[0]);
	box.extendBy(min);
	box.extendBy(max);

	//
	// And figure out the maximum profile offset, and expand
	// out the outline's bbox:
	//
# define max(a,b)               (a<b ? b : a)
# define abs(x)                 (x>=0 ? x : -(x))
	float maxOffset = max(abs(pBoxMin[1]), abs(pBoxMax[1]));
#undef max
#undef abs
	min.setValue(boxMin[0]-maxOffset, boxMin[1]-maxOffset, firstZ);
	max.setValue(boxMax[0]+maxOffset, boxMax[1]+maxOffset, lastZ);
	box.extendBy(min);
	box.extendBy(max);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Generates triangles representing a Text3.
//
// Use: protected

void
SoText3::generatePrimitives(SoAction *action)
//
////////////////////////////////////////////////////////////////////////
{
    SoState *state = action->getState();

    if (!setupFontCache(state))
	return;

    currentGeneratingNode = this;

    // Set up default texture coordinate mapping, if necessary:
    SoTextureCoordinateElement::CoordType tcType =
	SoTextureCoordinateElement::getType(state);
    if (tcType == SoTextureCoordinateElement::EXPLICIT) {
	genTexCoord = TRUE;
	tce = NULL;
    } else {
	genTexCoord = FALSE;
	tce = SoTextureCoordinateElement::getInstance(state);
    }

    // Set up 3 vertices we can use
    SoPrimitiveVertex	v1, v2, v3;
    SoTextDetail detail;
    v1.setDetail(&detail);
    v2.setDetail(&detail);
    v3.setDetail(&detail);

    genPrimVerts[0] = &v1;
    genPrimVerts[1] = &v2;
    genPrimVerts[2] = &v3;

    genAction = action;
    genBack = FALSE;

    SoMaterialBindingElement::Binding mbe =
	SoMaterialBindingElement::get(state);
    SbBool materialPerPart =
	(mbe == SoMaterialBindingElement::PER_PART_INDEXED ||
	 mbe == SoMaterialBindingElement::PER_PART);
    if (!materialPerPart) {
	v1.setMaterialIndex(0);
	v2.setMaterialIndex(0);
	v3.setMaterialIndex(0);
    }

    float firstZ, lastZ;
    myFont->getProfileBounds(firstZ, lastZ);

    uint32_t prts = parts.getValue();
    if ((prts & SIDES) && myFont->hasProfile()) {
	if (materialPerPart) {
	    v1.setMaterialIndex(1);
	    v2.setMaterialIndex(1);
	    v3.setMaterialIndex(1);
	}
	detail.setPart(SIDES);

	for (int line = 0; line < string.getNum(); line++) {
	    detail.setStringIndex(line);

	    SbVec2f p = getStringOffset(line);
	    genTranslate.setValue(p[0], p[1], lastZ);
	    generateSide(line);
	}
    }
    if (prts & BACK) {
	genBack = TRUE;
	if (materialPerPart) {
	    v1.setMaterialIndex(2);
	    v2.setMaterialIndex(2);
	    v3.setMaterialIndex(2);
	}
	detail.setPart(BACK);

	v1.setNormal(SbVec3f(0, 0, -1));
	v2.setNormal(SbVec3f(0, 0, -1));
	v3.setNormal(SbVec3f(0, 0, -1));
	
	for (int line = 0; line < string.getNum(); line++) {
	    detail.setStringIndex(line);

	    SbVec2f p = getStringOffset(line);
	    genTranslate.setValue(p[0], p[1], lastZ);
	    generateFront(line);
	}
	genBack = FALSE;
    }	
    if (prts & FRONT) {
	if (materialPerPart) {
	    v1.setMaterialIndex(0);
	    v2.setMaterialIndex(0);
	    v3.setMaterialIndex(0);
	}
	detail.setPart(FRONT);

	v1.setNormal(SbVec3f(0, 0, 1));
	v2.setNormal(SbVec3f(0, 0, 1));
	v3.setNormal(SbVec3f(0, 0, 1));
	
	for (int line = 0; line < string.getNum(); line++) {
	    detail.setStringIndex(line);

	    SbVec2f p = getStringOffset(line);
	    genTranslate.setValue(p[0], p[1], firstZ);
	    generateFront(line);
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Setup internal font cache.  Called by all action methods before
//    doing their thing.  GLRender passes TRUE to do special rendering
//    setup.  Returns FALSE if there are problems and the action
//    should bail.
//
// Use: private

SbBool
SoText3::setupFontCache(SoState *state, SbBool forRender)
//
////////////////////////////////////////////////////////////////////////
{
    // The state must be pushed here because myFont->isRenderValid
    // depends on the state being the correct depth (it must be the
    // same depth as when the font cache was built).
    state->push();

    if (myFont != NULL) {
	SbBool isValid;
	if (forRender)
	    isValid = myFont->isRenderValid(state);
	else
	    isValid = myFont->isValid(state);

	if (!isValid) {
	    myFont->unref(state);
	    myFont = NULL;
	}
    }
    if (myFont == NULL) {
	myFont = SoOutlineFontCache::getFont(state, forRender);
    }
    state->pop();
    return  myFont != NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Gets the 2D bounding box of the outlines of the letters in our
//    string.  How?  Get all the outlines and extend the bbox by every
//    point in every outline... (assuming the lines or characters
//    don't overlap, this could be optimized by just considering the
//    characters on the borders of the string-- although there are
//    some gotchas, like spaces at the beginning of a line...).
//
// Use: private

void
SoText3::getFrontBBox(SbBox2f &result)
//
////////////////////////////////////////////////////////////////////////
{
    SbBox2f charBBox;

    int line, character;
    for (line = 0; line < string.getNum(); line++) {
	// Starting position of string, based on justification:
	SbVec2f charPosition = getStringOffset(line);

	const SbString &str = string[line];
	const char *chars = str.getString();
	
	for (character = 0; character < str.getLength(); character++) {
	    myFont->getCharBBox(chars[character], charBBox);
	    if (!charBBox.isEmpty()) {
		SbVec2f min = charBBox.getMin() + charPosition;
		SbVec2f max = charBBox.getMax() + charPosition;
		result.extendBy(min);
		result.extendBy(max);
	    }

	    // And advance...
	    charPosition += myFont->getCharOffset(chars[character]);
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Figure out how much to offset the given line of text.  The X
//    offset depends on the justification and the width of the string
//    in the current font.  The Y offset depends on the line spacing
//    and font height.
//
// Use: private

SbVec2f
SoText3::getStringOffset(int line)
//
////////////////////////////////////////////////////////////////////////
{
    SbVec2f result(0,0);
    
    if (justification.getValue() == RIGHT) {
	float width = myFont->getWidth(string[line]);
	result[0] = -width;
    }
    if (justification.getValue() == CENTER) {
	float width = myFont->getWidth(string[line]);
	result[0] = -width/2.0;
    }
    result[1] = -line*myFont->getHeight()*spacing.getValue();

    return result;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Figure out where a given character starts.
//
// Use: private

SbVec2f
SoText3::getCharacterOffset(int line, int whichChar)
//
////////////////////////////////////////////////////////////////////////
{
    SbVec2f result = getStringOffset(line);
    
    const char *chars = string[line].getString();

    // Now add on all of the character advances up to char:
    for (int i = 0; i < whichChar; i++) {
	result += myFont->getCharOffset(chars[i]);
    }
    return result;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Render the fronts of the given string.  The GL transformation
//    matrix is munged by this routine-- surround it by
//    PushMatrix/PopMatrix.
//
// Use: public, internal

void
SoText3::renderFront(SoGLRenderAction *, int line,
		     gluTESSELATOR *tobj)
//
////////////////////////////////////////////////////////////////////////
{
    const char *chars = string[line].getString();

    // First, try to figure out if we can use glCallLists:
    SbBool useCallLists = TRUE;

    for (int i = 0; i < string[line].getLength(); i++) {
	// See if the font cache already has (or can build) a display
	// list for this character:
	if (!myFont->hasFrontDisplayList(chars[i], tobj)) {
	    useCallLists = FALSE;
	    break;
	}
    }
    // if we have display lists for all of the characters, use
    // glCallLists:
    if (useCallLists) {
	myFont->callFrontLists(string[line]);
    }
    // if we don't, draw the string character-by-character, using the
    // display lists we do have:
    else {
	myFont->renderFront(string[line], tobj);
    }
}    

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Render the sides of the given string.  The GL transformation
//    matrix is munged by this routine-- surround it by
//    PushMatrix/PopMatrix.
//
// Use: private

void
SoText3::renderSide(SoGLRenderAction *, int line)
//
////////////////////////////////////////////////////////////////////////
{
    const char *chars = string[line].getString();

    // First, try to figure out if we can use glCallLists:
    SbBool useCallLists = TRUE;

    for (int i = 0; i < string[line].getLength(); i++) {
	// See if the font cache already has (or can build) a display
	// list for this character:
	if (!myFont->hasSideDisplayList(chars[i], renderSideTris)) {
	    useCallLists = FALSE;
	    break;
	}
    }
    // if we have display lists for all of the characters, use
    // glCallLists:
    if (useCallLists) {
	myFont->callSideLists(string[line]);
    }
    // if we don't, draw the string character-by-character, using the
    // display lists we do have:
    else {
	myFont->renderSide(string[line], renderSideTris);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Create text detail and fill in extra info.
//
// Use: protected, virtual

SoDetail *
SoText3::createTriangleDetail(SoRayPickAction *,
			      const SoPrimitiveVertex *v1,
			      const SoPrimitiveVertex *,
			      const SoPrimitiveVertex *,
			      SoPickedPoint *)
//
////////////////////////////////////////////////////////////////////////
{
    SoTextDetail *result = new SoTextDetail;
    const SoTextDetail *old = (const SoTextDetail *)v1->getDetail();
    
    result->setPart(old->getPart());
    result->setStringIndex(old->getStringIndex());
    result->setCharacterIndex(old->getCharacterIndex());

    return result;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Generates primitives for the fronts of characters.  Assumes that
//    genTranslate has been set to the starting position of the
//    string, and that the genPrimVerts array points to three
//    appropriate vertices.
//
// Use: internal

void
SoText3::generateFront(int line)
//
////////////////////////////////////////////////////////////////////////
{
    static gluTESSELATOR *tobj = NULL;

    const char *chars = string[line].getString();

    if (tobj == NULL) {
	tobj = gluNewTess();
	gluTessCallback(tobj, (GLenum)GLU_BEGIN, (void (*)())SoText3::beginCB);
	gluTessCallback(tobj, (GLenum)GLU_END, (void (*)())SoText3::endCB);
	gluTessCallback(tobj, (GLenum)GLU_VERTEX, (void (*)())SoText3::vtxCB);
	gluTessCallback(tobj, (GLenum)GLU_ERROR,
			(void (*)())SoOutlineFontCache::errorCB);
    }

    genWhichVertex = 0;

    SoTextDetail *d = (SoTextDetail *)genPrimVerts[0]->getDetail();

    for (int i = 0; i < string[line].getLength(); i++) {
	d->setCharacterIndex(i);

	myFont->generateFrontChar(chars[i], tobj);

	SbVec2f p = myFont->getCharOffset(chars[i]);
	genTranslate[0] += p[0];
	genTranslate[1] += p[1];
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Generates sides of the characters.
//
// Use: internal

void
SoText3::generateSide(int line)
//
////////////////////////////////////////////////////////////////////////
{
    const char *chars = string[line].getString();

    SoTextDetail *d = (SoTextDetail *)genPrimVerts[0]->getDetail();

    for (int i = 0; i < string[line].getLength(); i++) {
	d->setCharacterIndex(i);

	myFont->generateSideChar(chars[i], generateSideTris);

	SbVec2f p = myFont->getCharOffset(chars[i]);
	genTranslate[0] += p[0];
	genTranslate[1] += p[1];
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Given two correctly rotated and positioned bevels, this routine
//    fills in the triangles in between them.
//
// Use: private, static

void
SoText3::generateSideTris(int nPoints, const SbVec3f *p1, const SbVec3f *n1,
		const SbVec3f *p2, const SbVec3f *n2,
		const float *sTexCoords, const float *tTexCoords)
//
////////////////////////////////////////////////////////////////////////
{
    float vertex[3];
    SbVec4f texCoord(0,0,0,1);
    
    const SbVec3f *p[2]; p[0] = p1; p[1] = p2;
    const SbVec3f *n[2]; n[0] = n1; n[1] = n2;

// Handy little macro to set a primitiveVertice's point, normal, and
// texture coordinate:
#define SET(pv,i,row,col) \
    {vertex[0] = p[col][i+row][0] + genTranslate[0]; \
     vertex[1] = p[col][i+row][1] + genTranslate[1]; \
     vertex[2] = p[col][i+row][2]; \
     genPrimVerts[pv]->setPoint(vertex);\
     genPrimVerts[pv]->setNormal(n[col][i*2+row]); \
     texCoord[0] = sTexCoords[i+row]; \
     texCoord[1] = tTexCoords[col]; \
     genPrimVerts[pv]->setTextureCoords(texCoord); \
    }

    SoText3 *t3 = currentGeneratingNode;

    for (int i = 0; i < nPoints-1; i++) {
	// First triangle: 
	SET(0, i, 0, 0)
	SET(1, i, 1, 0)
	SET(2, i, 0, 1)
	t3->invokeTriangleCallbacks(genAction, genPrimVerts[0],
				    genPrimVerts[1], genPrimVerts[2]);

	// Second triangle:
	SET(0, i, 1, 1)
	SET(1, i, 0, 1)
	SET(2, i, 1, 0)
	t3->invokeTriangleCallbacks(genAction, genPrimVerts[0],
				    genPrimVerts[1], genPrimVerts[2]);
#undef SET
    }
}
////////////////////////////////////////////////////////////////////////
//
// Description:
//    Given two correctly rotated and positioned bevels, this routine
//    renders triangles in between them.
//
// Use: private, static

void
SoText3::renderSideTris(int nPoints, const SbVec3f *p1, const SbVec3f *n1,
			const SbVec3f *p2, const SbVec3f *n2,
			const float *sTex, const float *tTex)
//
////////////////////////////////////////////////////////////////////////
{
    // Note:  the glBegin(GL_QUADS) is optimized up into the
    // routine that calls generateSideChar, so there is one glBegin
    // per character.
    for (int i = 0; i < nPoints-1; i++) {
	if (genTexCoord) glTexCoord2f(sTex[i+1], tTex[0]);
	glNormal3fv(n1[i*2+1].getValue());
	glVertex3fv(p1[i+1].getValue());

	if (genTexCoord) glTexCoord2f(sTex[i+1], tTex[1]);
	glNormal3fv(n2[i*2+1].getValue());
	glVertex3fv(p2[i+1].getValue());

	if (genTexCoord) glTexCoord2f(sTex[i], tTex[1]);
	glNormal3fv(n2[i*2].getValue());
	glVertex3fv(p2[i].getValue());

	if (genTexCoord) glTexCoord2f(sTex[i], tTex[0]);
	glNormal3fv(n1[i*2].getValue());
	glVertex3fv(p1[i].getValue());
    }
}
	    
////////////////////////////////////////////////////////////////////////
//
// Description:
//    Called by the GLU tesselator when we are beginning a triangle
//    strip, fan, or set of independent triangles.
//
// Use: static, private

void
SoText3::beginCB(GLenum primType)
//
////////////////////////////////////////////////////////////////////////
{
    genPrimType = primType;
    genWhichVertex = 0;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Called by the GLU tesselator when we are done with the
//    strip/fan/etc.
//
// Use: static, private

void
SoText3::endCB()
//
////////////////////////////////////////////////////////////////////////
{
    genWhichVertex = 0;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Called by the GLU tesselator when we are generating primitives.
//
// Use: static, private

void
SoText3::vtxCB(void *v)
//
////////////////////////////////////////////////////////////////////////
{
    SbVec2f &vv = *((SbVec2f *)v);
    float vertex[3];
    vertex[0] = vv[0] + genTranslate[0];
    vertex[1] = vv[1] + genTranslate[1];
    vertex[2] = genTranslate[2];

    SoText3 *t3 = currentGeneratingNode;
    
    // Fill in one of the primitive vertices:
    genPrimVerts[genWhichVertex]->setPoint(vertex);

    SbVec4f texCoord;
    
    // And texture coordinates:
    if (genTexCoord) {
	float textHeight = t3->myFont->getHeight();
	texCoord.setValue(vertex[0]/textHeight, vertex[1]/textHeight,
			  0.0, 1.0);
	// S coordinates go other way on back...
	if (genBack) texCoord[0] = -texCoord[0];
    } else {
	texCoord = tce->get(vertex, genPrimVerts[0]->getNormal());
    }
    genPrimVerts[genWhichVertex]->setTextureCoords(texCoord);
	
    genWhichVertex = (genWhichVertex+1)%3;

    // If we just filled in the third vertex, we can spit out a
    // triangle:
    if (genWhichVertex == 0) {
	// If we are doing the BACK part, reverse the triangle:
	if (genBack) {
	    t3->invokeTriangleCallbacks(genAction,
					genPrimVerts[2],
					genPrimVerts[1],
					genPrimVerts[0]);
	} else {
	    t3->invokeTriangleCallbacks(genAction,
					genPrimVerts[0],
					genPrimVerts[1],
					genPrimVerts[2]);
	}
	// Now, need to set-up for the next vertex.
	// Three cases to deal with-- independent triangles, triangle
	// strips, and triangle fans.
	switch (genPrimType) {
	  case GL_TRIANGLES:
	    // Don't need to do anything-- every three vertices
	    // defines a triangle.
	    break;

// Useful macro:
#define SWAP(a, b) { SoPrimitiveVertex *t = a; a = b; b = t; }

	  case GL_TRIANGLE_FAN:
	    // For triangle fans, vertex zero stays the same, but
	    // vertex 2 becomes vertex 1, and the next vertex to come
	    // in will replace vertex 2 (the old vertex 1).
	    SWAP(genPrimVerts[1], genPrimVerts[2]);
	    genWhichVertex = 2;
	    break;

	  case GL_TRIANGLE_STRIP:
	    // For triangle strips, vertex 1 becomes vertex 0, vertex
	    // 2 becomes vertex 1, and the new triangle will replace
	    // vertex 2 (the old vertex 0).
	    SWAP(genPrimVerts[1], genPrimVerts[0]);
	    SWAP(genPrimVerts[2], genPrimVerts[1]);
	    genWhichVertex = 2;
	    break;
#undef SWAP
	}
    }
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Find an appropriate font, given a state.  A bunch of elements
//    (fontName, fontSize, creaseAngle, complexity and profile) must
//    be enabled in the state...
//
// Use: static, internal

SoOutlineFontCache *
SoOutlineFontCache::getFont(SoState *state, SbBool forRender)
//
////////////////////////////////////////////////////////////////////////
{
    if (fonts == NULL) {
	// One-time font library initialization
	fonts = new SbPList;
	context = flCreateContext(NULL, FL_FONTNAME, NULL,
				  1.0, 1.0);
	if (context == NULL) {
#ifdef DEBUG
	    SoDebugError::post("SoText3::getFont",
			       "flCreateContext returned NULL");
#endif
	    return NULL;
	}
	flMakeCurrentContext(context);
	flSetHint(FL_HINT_FONTTYPE, FL_FONTTYPE_OUTLINE);
    }
    else if (context == NULL) return NULL;
    else {
	if (flGetCurrentContext() != context)
	    flMakeCurrentContext(context);
    }

    SoOutlineFontCache *result = NULL;
    for (int i = 0; i < fonts->getLength() && result == NULL; i++) {
	SoOutlineFontCache *c = (SoOutlineFontCache *) (*fonts)[i];
	if (forRender ? c->isRenderValid(state) : c->isValid(state)) {
	    result = c; // Loop will terminate...
	    result->ref(); // Increment ref count
	    if (flGetCurrentFont() != result->fontId) {
		flMakeCurrentFont(result->fontId);
	    }
	}
    }
    // If none match:
    if (result == NULL) {
	result = new SoOutlineFontCache(state);

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
SoOutlineFontCache::isValid(SoState *state) const
//
////////////////////////////////////////////////////////////////////////
{
    SbBool result = SoCache::isValid(state);
    
    if (result) {
	if (flGetCurrentContext() != context) {
	    flMakeCurrentContext(context);
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
//    Figures out if this cache is valid for rendering.
//
// Use: internal

SbBool
SoOutlineFontCache::isRenderValid(SoState *state) const
//
////////////////////////////////////////////////////////////////////////
{
    // Special cache case here:  if we generated side display lists
    // without texture coordinates AND we need texture coordinates,
    // we'll have to regenerate and this cache is invalid:
    if (sideList) {
	if (!sidesHaveTexCoords &&
	    SoGLTextureEnabledElement::get(state)) {
	    return FALSE;
	}
    }

    if (!isValid(state)) return FALSE;

    if (frontList && 
	frontList->getContext() != SoGLCacheContextElement::get(state))
	return FALSE;
    if (sideList && 
	sideList->getContext() != SoGLCacheContextElement::get(state))
	return FALSE;

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor.  Called by getFont().
//
// Use: private

SoOutlineFontCache::SoOutlineFontCache(SoState *state) : 
	SoCache(state)
//
////////////////////////////////////////////////////////////////////////
{
    ref();

    frontList = sideList = NULL;

    // Add element dependencies explicitly here; making 'this' the
    // CacheElement doesn't work if we are being constructed in an
    // action that doesn't have caches.
    SbName font = SoFontNameElement::get(state);
    addElement(state->getConstElement(
	SoFontNameElement::getClassStackIndex()));
    if (font == SoFontNameElement::getDefault()) {
	font = SbName("Utopia-Regular");
    }
    
    float uems;
    
    // Remember size
    fontSize = SoFontSizeElement::get(state);
    addElement(state->getConstElement(
	SoFontSizeElement::getClassStackIndex()));
    
    // Figure out complexity...
    float complexity = SoComplexityElement::get(state);
    addElement(state->getConstElement(
	SoComplexityElement::getClassStackIndex()));
    addElement(state->getConstElement(
	SoComplexityTypeElement::getClassStackIndex()));

    switch (SoComplexityTypeElement::get(state)) {
      case SoComplexityTypeElement::OBJECT_SPACE:
	{
	    // Two ramps-- complexity of zero  == 250/1000 of an em
	    //             complexity of .5    == 20/1000 of an em
	    //             complexity of 1     == 1/1000 of an em
	    const float ZERO = 250;
	    const float HALF = 20;
	    const float ONE = 1;
	    if (complexity > 0.5) uems = (2.0-complexity*2.0)*(HALF-ONE)+ONE;
	    else uems = (1.0-complexity*2.0)*(ZERO-HALF)+HALF;
	}
	break;

      case SoComplexityTypeElement::SCREEN_SPACE:
	{
	    SbVec3f p(fontSize, fontSize, fontSize);
	    SbVec2s rectSize;
	    
	    SoShape::getScreenSize(state, SbBox3f(-p, p), rectSize);
	    float maxSize =
		(rectSize[0] > rectSize[1] ? rectSize[0] : rectSize[1]);
	    uems = 250.0 / (1.0 + 0.25 * maxSize * complexity *
			    complexity);
	    
	    // We have to manually add the dependency on the
	    // projection, view and model matrix elements (these are
	    // gotten in the SoShape::getScreenSize routine), and the
	    // ViewportRegionElement:
	    addElement(state->getConstElement(
		SoProjectionMatrixElement::getClassStackIndex()));
	    addElement(state->getConstElement(
		SoViewingMatrixElement::getClassStackIndex()));
	    addElement(state->getConstElement(
		SoModelMatrixElement::getClassStackIndex()));
	    addElement(state->getConstElement(
		SoViewportRegionElement::getClassStackIndex()));
	}
	break;

      case SoComplexityTypeElement::BOUNDING_BOX:
	{
	    uems = 20;
	}
	break;
    }
    flSetHint(FL_HINT_TOLERANCE, uems);

    static GLfloat m[2][2] = { 1.0, 0.0, 0.0, 1.0 };

    fontId = flCreateFont((const GLubyte *)font.getString(), m, 0, NULL);

    // If error creating font:
    if (fontId == 0) {
	// Try Utopia-Regular, unless we just did!
	if (font != SbName("Utopia-Regular")) {
#ifdef DEBUG
	    SoDebugError::post("SoText3::getFont",
		      "Couldn't find font %s, replacing with Utopia-Regular",
		       font.getString());
#endif
	    fontId = flCreateFont((GLubyte *)"Utopia-Regular", m, 0, NULL);
	}
	if (fontId == 0) {
#ifdef DEBUG
	    SoDebugError::post("SoText3::getFont",
			       "Couldn't find font Utopia-Regular!");
#endif
	    return;
	}
    }

    flMakeCurrentFont(fontId);

    numChars = 256;  // ??? NEED TO REALLY KNOW HOW MANY CHARACTERS IN
		     // FONT!
    sidesHaveTexCoords = FALSE;

    frontFlags = new SbBool[numChars];
    sideFlags = new SbBool[numChars];
    outlines = new SoFontOutline*[numChars];
    int i;
    for (i = 0; i < numChars; i++) {
	frontFlags[i] = sideFlags[i] = FALSE;
	outlines[i] = NULL;
    }

    // Get profile info:
    const SoNodeList &profiles = SoProfileElement::get(state);
    addElement(state->getConstElement(
	SoProfileElement::getClassStackIndex()));
    addElement(state->getConstElement(
	SoProfileCoordinateElement::getClassStackIndex()));
    nProfileVerts = 0;
    if (profiles.getLength() > 0) {
	SoProfile *profileNode = (SoProfile *)profiles[0];
	profileNode->getVertices(state, nProfileVerts, profileVerts);
    } else {
	nProfileVerts = 2;
	profileVerts = new SbVec2f[2];
	profileVerts[0].setValue(0, 0);
	profileVerts[1].setValue(1, 0);
    }	

    if (nProfileVerts > 1) {
	cosCreaseAngle = cos(SoCreaseAngleElement::get(state));
	addElement(state->getConstElement(
	    SoCreaseAngleElement::getClassStackIndex()));
	int nSegments = (int) nProfileVerts - 1;

	// Figure out normals for profiles; there are twice as many
	// normals as segments.  The two normals for each segment endpoint
	// may be averaged with the normal for the next segment, depending
	// on whether or not the angle between the segments is greater
	// than the creaseAngle.
	profileNorms = new SbVec2f[nSegments*2];
	figureSegmentNorms(profileNorms, (int) nProfileVerts, profileVerts,
			   cosCreaseAngle, FALSE);
	// Need to flip all the normals because of the way the profiles
	// are defined:
	for (i = 0; i < nSegments*2; i++) {
	    profileNorms[i] *= -1.0;
	}
    
	// Figure out S texture coordinates, which run along the profile:
	sTexCoords = new float[nProfileVerts];
	figureSegmentTexCoords(sTexCoords, (int) nProfileVerts,
			       profileVerts, FALSE);
	// And reverse them, so 0 is at the back of the profile:
	float max = sTexCoords[nProfileVerts-1];
	for (i = 0; i < nProfileVerts; i++) {
	    sTexCoords[i] = max - sTexCoords[i];
	}
    } else {
	profileNorms = NULL;
	sTexCoords = NULL;
    }

    fonts->append(this);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoOutlineFontCache::~SoOutlineFontCache()
//
////////////////////////////////////////////////////////////////////////
{
    if (fontId) {

	if (flGetCurrentContext() != context) {
	    flMakeCurrentContext(context);
	    flMakeCurrentFont(fontId);
	}
	else if (flGetCurrentFont() != fontId)
	    flMakeCurrentFont(fontId);

	delete[] frontFlags;
	delete[] sideFlags;

	// Free up cached outlines
	int i;
	for (i = 0; i < numChars; i++) {
	    if (outlines[i]) delete outlines[i];
	}
	delete[] outlines;
    
	if (hasProfile()) {
	    delete[] profileVerts;
	    delete[] sTexCoords;
	    delete[] profileNorms;
	}

	// Only destroy the font library font if no other font caches
	// are using the same font identifier:
	SbBool otherUsing = FALSE;
	for (i = 0; i < fonts->getLength(); i++) {
	    SoOutlineFontCache *t = (SoOutlineFontCache *)(*fonts)[i];
	    if (t != this && t->fontId == fontId) otherUsing = TRUE;
	}
	if (!otherUsing) {
	    flDestroyFont(fontId);
	}
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
SoOutlineFontCache::destroy(SoState *)
//
////////////////////////////////////////////////////////////////////////
{
    // Pass in NULL to unref because this cache may be destroyed
    // from an action _other_ than GLRender:
    if (frontList) {
	frontList->unref(NULL);
	frontList = NULL;
    }
    if (sideList) {
	sideList->unref(NULL);
	sideList = NULL;
    }
    SoCache::destroy(NULL);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the width of the given string
//
// Use: private

float
SoOutlineFontCache::getWidth(const SbString &string)
//
////////////////////////////////////////////////////////////////////////
{
    float total = 0.0;
    const char *chars = string.getString();
    
    for (int i = 0; i < string.getLength(); i++) {
	SoFontOutline *outline = getOutline(chars[i]);
	total += outline->getCharAdvance()[0];
    }

    return total;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the 2D bounding box of the given character.
//
// Use: private

void
SoOutlineFontCache::getCharBBox(const char c, SbBox2f &result)
//
////////////////////////////////////////////////////////////////////////
{
    result.makeEmpty();

    if (!fontId) return;

    SoFontOutline *outline = getOutline(c);
    
    for (int i = 0; i < outline->getNumOutlines(); i++) {
	for (int j = 0; j < outline->getNumVerts(i); j++) {
	    result.extendBy(outline->getVertex(i,j));
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Given a character, return an outline for the character.  If, for
//    some reason, we can't get the outline, an 'identity' or 'null'
//    outline is returned.
//
// Use: private

SoFontOutline *
SoOutlineFontCache::getOutline(const char c)
//
////////////////////////////////////////////////////////////////////////
{
    if (!fontId) {
	return SoFontOutline::getNullOutline();
    }
    if (outlines[c] == NULL) {
	FLoutline *flo = flGetOutline(fontId, c);
	if (flo == NULL) {
	    outlines[c] = SoFontOutline::getNullOutline();
	} else {
	    outlines[c] = new SoFontOutline(flo, fontSize);
	    flFreeOutline(flo);
	}
    }
    return outlines[c];
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Given a character, return the correct amount to advance after
//    drawing that character.  Note:  if we ever wanted to deal with
//    kerning, we'd have to fix this...
//
// Use: private

SbVec2f
SoOutlineFontCache::getCharOffset(const char c)
//
////////////////////////////////////////////////////////////////////////
{
    if (!fontId) return SbVec2f(0,0);

    return getOutline(c)->getCharAdvance();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Render the fronts of the given string.  The GL transformation
//    matrix is munged by this routine-- surround it by
//    PushMatrix/PopMatrix.
//
// Use: public, internal

void
SoOutlineFontCache::generateFrontChar(const char c,
				      gluTESSELATOR *tobj)
//
////////////////////////////////////////////////////////////////////////
{
    if (!fontId) return;

    GLdouble v[3];

    tesselationError = FALSE;
    gluBeginPolygon(tobj);
    
    // Get outline for character
    SoFontOutline *outline = getOutline(c);
    int i;
    for (i = 0; i < outline->getNumOutlines(); i++) {

	// It would be nice if the font manager told us the type of
	// each outline...
	gluNextContour(tobj, (GLenum)GLU_UNKNOWN);

	for (int j = 0; j < outline->getNumVerts(i); j++) {
	    SbVec2f &t = outline->getVertex(i,j);
	    v[0] = t[0];
	    v[1] = t[1];
	    v[2] = 0.0;

	    // Note: The third argument MUST NOT BE a local variable,
	    // since glu just stores the pointer and only calls us
	    // back at the gluEndPolygon call.
	    gluTessVertex(tobj, v, &t);
	}
    }
    gluEndPolygon(tobj);

    // If there was an error tesselating the character, just generate
    // a bounding box for the character:
    if (tesselationError) {
	SbBox2f charBBox;
	getCharBBox(c, charBBox);
	if (!charBBox.isEmpty()) {
	    SbVec2f boxVerts[4];
	    charBBox.getBounds(boxVerts[0], boxVerts[2]);
	    boxVerts[1].setValue(boxVerts[2][0], boxVerts[0][1]);
	    boxVerts[3].setValue(boxVerts[0][0], boxVerts[2][1]);

	    gluBeginPolygon(tobj);
	    for (i = 0; i < 4; i++) {
		v[0] = boxVerts[i][0];
		v[1] = boxVerts[i][1];
		v[2] = 0.0;
		gluTessVertex(tobj, v, &boxVerts[i]);
	    }
	    gluEndPolygon(tobj);
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets up for GL rendering.
//
// Use: internal

void
SoOutlineFontCache::setupToRenderFront(SoState *state)
//
////////////////////////////////////////////////////////////////////////
{
    otherOpen = SoCacheElement::anyOpen(state);
    if (!otherOpen && !frontList) {
	frontList = new SoGLDisplayList(state,
					SoGLDisplayList::DISPLAY_LIST,
					numChars);
	frontList->ref();
    }
    if (frontList) {
	// Set correct list base
	glListBase(frontList->getFirstIndex());
	frontList->addDependency(state);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets up for GL rendering.
//
// Use: internal

void
SoOutlineFontCache::setupToRenderSide(SoState *state, SbBool willTexture)
//
////////////////////////////////////////////////////////////////////////
{
    otherOpen = SoCacheElement::anyOpen(state);
    if (!otherOpen && !sideList) {
	sideList = new SoGLDisplayList(state,
					SoGLDisplayList::DISPLAY_LIST,
					numChars);
	sideList->ref();
	sidesHaveTexCoords = willTexture;
    }
    if (sideList) {
	// Set correct list base
	glListBase(sideList->getFirstIndex());
	sideList->addDependency(state);
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
SoOutlineFontCache::hasFrontDisplayList(const char c,
					gluTESSELATOR *tobj)
//
////////////////////////////////////////////////////////////////////////
{
    // If we have one, return TRUE
    if (frontFlags[c] == TRUE) return TRUE;
    
    // If we don't and we can't build one, return FALSE.
    if (otherOpen) return FALSE;
    
    // Build one:
    glNewList(frontList->getFirstIndex()+c, GL_COMPILE);
    generateFrontChar(c, tobj);
    SbVec2f t = getOutline(c)->getCharAdvance();
    glTranslatef(t[0], t[1], 0.0);
    glEndList();
    frontFlags[c] = TRUE;

    return TRUE;
}
////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns TRUE if a display lists exists for given character.
//    Tries to build a display list, if it can.
//
// Use: internal

SbBool
SoOutlineFontCache::hasSideDisplayList(const char c,
				       SideCB callbackFunc)
//
////////////////////////////////////////////////////////////////////////
{
    // If we have one, return TRUE
    if (sideFlags[c] == TRUE) return TRUE;
    
    // If we don't and we can't build one, return FALSE.
    if (otherOpen) return FALSE;
    
    // Build one:
    glNewList(sideList->getFirstIndex()+c, GL_COMPILE);

    glBegin(GL_QUADS);    // Render as independent quads:
    generateSideChar(c, callbackFunc);
    glEnd();

    SbVec2f t = getOutline(c)->getCharAdvance();
    glTranslatef(t[0], t[1], 0.0);
    glEndList();
    sideFlags[c] = TRUE;

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
SoOutlineFontCache::callFrontLists(const SbString &string)
//
////////////////////////////////////////////////////////////////////////
{
    const char *str = string.getString();

    glCallLists(string.getLength(), GL_UNSIGNED_BYTE, str);
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
SoOutlineFontCache::callSideLists(const SbString &string)
//
////////////////////////////////////////////////////////////////////////
{
    const char *str = string.getString();

    glCallLists(string.getLength(), GL_UNSIGNED_BYTE, str);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Assuming that there are not display lists built for all the
//    characters in given string, render the string.
//
// Use: internal

void
SoOutlineFontCache::renderFront(const SbString &string,
				gluTESSELATOR *tobj)
//
////////////////////////////////////////////////////////////////////////
{
    const char *str = string.getString();

    for (int i = 0; i < string.getLength(); i++) {
	if (frontFlags[str[i]]) {
	    glCallList(frontList->getFirstIndex()+str[i]);
	}
	else {
	    generateFrontChar(str[i], tobj);
	    SbVec2f t = getOutline(str[i])->getCharAdvance();
	    glTranslatef(t[0], t[1], 0.0);
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Assuming that there are not display lists built for all the
//    characters in given string, render the string.
//
// Use: internal

void
SoOutlineFontCache::renderSide(const SbString &string,
			       SideCB callbackFunc)
//
////////////////////////////////////////////////////////////////////////
{
    const char *str = string.getString();

    for (int i = 0; i < string.getLength(); i++) {
	if (sideFlags[str[i]]) {
	    glCallList(sideList->getFirstIndex()+str[i]);
	}
	else {
	    glBegin(GL_QUADS);
	    generateSideChar(str[i], callbackFunc);
	    glEnd();

	    SbVec2f t = getOutline(str[i])->getCharAdvance();
	    glTranslatef(t[0], t[1], 0.0);
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   Copy info from the font library into a more convenient form.
//
// Use: internal

SoFontOutline::SoFontOutline(FLoutline *outline, float fontSize)
//
////////////////////////////////////////////////////////////////////////
{
    charAdvance = SbVec2f(outline->xadvance,
			  outline->yadvance)*fontSize;
    numOutlines = outline->outlinecount;
    if (numOutlines != 0) {
	numVerts = new int[numOutlines];
	verts = new SbVec2f*[numOutlines];
	for (int i = 0; i < numOutlines; i++) {
	    numVerts[i] = outline->vertexcount[i];
	    if (numVerts[i] != 0) {
		verts[i] = new SbVec2f[numVerts[i]];
		for (int j = 0; j < numVerts[i]; j++) {
		    verts[i][j] = SbVec2f(outline->vertex[i][j].x,
					  outline->vertex[i][j].y)*fontSize;
		}
	    } else {
		verts[i] = NULL;
	    }
	}
    } else {
	numVerts = NULL;
	verts = NULL;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   Destructor; free up outline storage
//
// Use: internal

SoFontOutline::~SoFontOutline()
//
////////////////////////////////////////////////////////////////////////
{
    for (int i = 0; i < numOutlines; i++) {
	if (numVerts[i] != 0)
	    delete[] verts[i];
    }
    if (numOutlines != 0) {
	delete[] verts;
	delete[] numVerts;
    }
}
	    
////////////////////////////////////////////////////////////////////////
//
// Description:
//   Get a do-nothing outline:
//
// Use: internal, static

SoFontOutline *
SoFontOutline::getNullOutline()
//
////////////////////////////////////////////////////////////////////////
{
    return new SoFontOutline;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   Internal constructor used by getNullOutline
//
// Use: internal, static

SoFontOutline::SoFontOutline()
//
////////////////////////////////////////////////////////////////////////
{
    charAdvance = SbVec2f(0,0);
    numOutlines = 0;
    numVerts = NULL;
    verts = NULL;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   Find the first and last points in the bevel-- that is where the
//   front and back of the character will be.
// Use:
//   internal

void
SoOutlineFontCache::getProfileBounds(float &firstZ, float &lastZ)
//
////////////////////////////////////////////////////////////////////////
{
    if (hasProfile()) {
	firstZ = -profileVerts[0][0];
	lastZ = -profileVerts[nProfileVerts-1][0];
    } else {
	firstZ = lastZ = 0;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   Get the 2D bounding box of the bevel.
//
//  Use:
//    internal

void
SoOutlineFontCache::getProfileBBox(SbBox2f &profileBox)
//
////////////////////////////////////////////////////////////////////////
{
    for (int i = 0; i < nProfileVerts; i++) {
	profileBox.extendBy(profileVerts[i]);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Called by generateSide.  This generates the bevel triangles for
//    one character.
//
// Use: private

void
SoOutlineFontCache::generateSideChar(const char c, SideCB callbackFunc)
//
////////////////////////////////////////////////////////////////////////
{
    if (!hasProfile()) return;

    // Get the outline of the character:
    SoFontOutline *outline = getOutline(c);
    
    for (int i = 0; i < outline->getNumOutlines(); i++) {
	// For each outline:

	int nOutline = outline->getNumVerts(i);

	SbVec2f *oVerts = new SbVec2f[nOutline];
	// Copy in verts so figureSegmentNorms can handle them..
        int j;
	for (j = 0; j < nOutline; j++) {
	    oVerts[j] = outline->getVertex(i, j);
	}

	// First, figure out a set of normals for the outline:
	SbVec2f *oNorms = new SbVec2f[nOutline*2];
	figureSegmentNorms(oNorms, nOutline, oVerts, cosCreaseAngle, TRUE);

	// And appropriate texture coordinates:
	// Figure out T texture coordinates, which run along the
	// outline:
	float *tTexCoords = new float[nOutline+1];
	figureSegmentTexCoords(tTexCoords, nOutline, oVerts, TRUE);

	// Now, generate a set of triangles for each segment in the
	// outline.  A bevel of profiles is built at each point in the
	// outline; each profile must be flipped perpendicular to the
	// outline (x coordinate becomes -z), rotated to be the
	// average of the normals of the two adjoining segments at
	// that point, and translated to that point.  Triangles are
	// formed between consecutive bevels.
	// Normals are just taken from the 'pNorms' array, after being
	// rotated the appropriate amount.

	SbVec3f *bevel1 = new SbVec3f[nProfileVerts];
	SbVec3f *bevelN1 = new SbVec3f[(nProfileVerts-1)*2];
	SbVec3f *bevel2 = new SbVec3f[nProfileVerts];
	SbVec3f *bevelN2 = new SbVec3f[(nProfileVerts-1)*2];
	    
	// fill out first bevel:
	fillBevel(bevel1, (int) nProfileVerts, profileVerts,
		  outline->getVertex(i,0),
		  oNorms[(nOutline-1)*2+1], oNorms[0*2]);
	
	SbVec3f *s1 = bevel1;
	SbVec3f *s2 = bevel2;
	
	for (j = 0; j < nOutline; j++) {
	    // New normals are calculated for both ends of this
	    // segment, since the normals may or may not be shared
	    // with the previous segment.
	    fillBevelN(bevelN1, (int)(nProfileVerts-1)*2, profileNorms,
		       oNorms[j*2]);

	    int j2 = (j+1)%nOutline;
	    // fill out second bevel:
	    fillBevel(s2, (int) nProfileVerts, profileVerts,
		      outline->getVertex(i,j2),
		      oNorms[j*2+1], oNorms[j2*2]);
	    fillBevelN(bevelN2, (int)(nProfileVerts-1)*2, profileNorms,
		       oNorms[j*2+1]);

	    // And generate triangles between the two bevels:
	    (*callbackFunc)((int) nProfileVerts, s1, bevelN1, s2, bevelN2,
			     sTexCoords, &tTexCoords[j]);

	    // Swap bevel1/2 (avoids some recomputation)
	    SbVec3f *t;
	    t = s1; s1 = s2; s2 = t;
	}
	delete [] bevelN2;
	delete [] bevel2;
	delete [] bevelN1;
	delete [] bevel1;
	delete [] tTexCoords;
	delete [] oNorms;
	delete [] oVerts;
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Given a set of line segments, this figures out the normal at
//    each point in each segment.  It uses the creaseAngle passed in
//    to decide whether or not adjacent segments share normals.  The
//    isClosed flag is used to determine whether or not the first and
//    last points should be considered another segment.
//
//
// Use: private

void
SoOutlineFontCache::figureSegmentNorms(SbVec2f *norms, int nPoints,
			    const SbVec2f *points,  float cosCreaseAngle,
			    SbBool isClosed)
//
////////////////////////////////////////////////////////////////////////
{
    int nSegments;
    
    if (isClosed) nSegments = nPoints;
    else nSegments = nPoints-1;
    
    // First, we'll just make all the normals perpendicular to their
    // segments:
    int i;
    for (i = 0; i < nSegments; i++) {
	SbVec2f n;
	// This is 2D perpendicular, assuming profile is increasing in
	// X (which becomes 'decreasing in Z' when we actually use
	// it...) (note: if a profile isn't increasing in X, the
	// character will be inside-out, with the front face drawn
	// behind the back face, etc).
	SbVec2f v = points[(i+1)%nPoints] - points[i];
	n[0] = v[1];
	n[1] = -v[0];
	n.normalize();
	
	norms[i*2] = n;
	norms[i*2+1] = n;
    }
    // Now, figure out if the angle between any two segments is small
    // enough to average two of their normals.
    for (i = 0; i < (isClosed ? nSegments : nSegments-1); i++) {
	SbVec2f seg1 = points[(i+1)%nPoints] - points[i];
	seg1.normalize();
	SbVec2f seg2 = points[(i+2)%nPoints] - points[(i+1)%nPoints];
	seg2.normalize();
	
	float dp = seg2.dot(seg1);
	if (dp > cosCreaseAngle) {
	    // Average the second normal for this segment, and the
	    // first normal for the next segment:
	    SbVec2f average = norms[i*2+1] + norms[(i+1)*2];
	    average.normalize();
	    norms[i*2+1] = average;
	    norms[((i+1)%nPoints)*2] = average;
	}
    }
}    

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Given a set of line segments, this figures out the texture
//    coordinates for each vertex.  If the isClosed flag is TRUE,
//    an extra texture coordinate is calculated, AND the points are
//    traversed in reverse order.
//
// Use: private

void
SoOutlineFontCache::figureSegmentTexCoords(float *texCoords, int nPoints,
			    const SbVec2f *points, SbBool isClosed)
//
////////////////////////////////////////////////////////////////////////
{
    float total = 0.0;
    
    int i;

    if (isClosed) {
	for (i = nPoints; i >= 0; i--) {
	    texCoords[i] = total / getHeight();
	    if (i > 0) {
		total += (points[i%nPoints] - points[i-1]).length();
	    }
	}
    } else {
	for (int i = 0; i < nPoints; i++) {
	    texCoords[i] = total / getHeight();
	    if (i+1 < nPoints) {
		total += (points[i+1] - points[i]).length();
	    }
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Given a set of segments that make up a profile or bevel along
//    which we'll extrude the front face of the text, this routine
//    transforms the bevel from its default orientation (in the x-y
//    plane) to its correct position for a particular point on the
//    text outline, rotated and translated into position.  The
//    translation is the point on the outline, and the two normals
//    passed in are the normals for the segments adjoining that point.
//
// Use: private

void
SoOutlineFontCache::fillBevel(SbVec3f *result, int nPoints,
	  const SbVec2f *points,
	  const SbVec2f &translation,
	  const SbVec2f &n1, const SbVec2f &n2)
//
////////////////////////////////////////////////////////////////////////
{
    // First, figure out a rotation for this bevel:
    SbVec2f n = n1+n2;
    n.normalize();
    
    // Now, for each point:
    for (int i = 0; i < nPoints; i++) {
	// This is really the 2D rotation formula,
	// x = x' cos(angle) - y' sin(angle)
	// y = x' sin(angle) + y' cos(angle)
	// Because of the geometry, cos(angle) is n[1] and sin(angle)
	// is -n[0], and x' is zero (the bevel always goes straight
	// back).
	result[i][0] = points[i][1] * n[0] + translation[0];
	result[i][1] = points[i][1] * n[1] + translation[1];
	result[i][2] = -points[i][0];
    }
} 

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Given a set of normals for a profile, this rotates the normals
//    from their default position (int the x-y plane) to the correct
//    orientation for a particular point on the texts outline.  The
//    normal passed in is the normal for one end of one of the
//    outline's segments.

void
SoOutlineFontCache::fillBevelN(SbVec3f *result, int nNorms,
	  const SbVec2f *norms,
	  const SbVec2f &n)
//
////////////////////////////////////////////////////////////////////////
{
    // Now, for each point:
    for (int i = 0; i < nNorms; i++) {
	// This is really the 2D rotation formula,
	// x = x' cos(angle) - y' sin(angle)
	// y = x' sin(angle) + y' cos(angle)
	// Because of the geometry, cos(angle) is n[1] and sin(angle)
	// is -n[0], and x' is zero (the bevel always goes straight
	// back).
	result[i][0] = norms[i][1] * n[0];
	result[i][1] = norms[i][1] * n[1];
	result[i][2] = -norms[i][0];
    }
} 

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Called by the GLU tesselator when there is an error
//
// Use: static, private
//
////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
void
SoOutlineFontCache::errorCB(GLenum whichErr)
{
    SoDebugError::post("SoText3::errorCB", "%s", gluErrorString(whichErr));
    tesselationError = TRUE;
}
#else  /* DEBUG */
void
SoOutlineFontCache::errorCB(GLenum)
{
    tesselationError = TRUE;
}
#endif /* DEBUG */
#endif /*IRIX_6*/


