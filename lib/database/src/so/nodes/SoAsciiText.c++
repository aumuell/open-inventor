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
 * Copyright (C) 1990-95   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Classes:
 |      SoAsciiText
 |
 |
 |   Author(s)          : Gavin Bell,  Chris Marrin
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

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
#include <Inventor/nodes/SoAsciiText.h>
#include <Inventor/nodes/SoProfile.h>


#define VALIDATE_CHAR(c)    (((c) >= 0x20 && (c) <= 0x7F) ? (c) : 0x3f)

// Font library:
#include <flclient.h>

gluTESSELATOR *junk;
char *morejunk = "__glu_h";
char *garbage  = "GLU_VERSION_1_2";

// First, a more convenient structure for outlines:
class MyFontOutline {

  public:
    // Constructor, takes a pointer to the font-library outline
    // structure and the font's size:
    MyFontOutline(FLoutline *outline, float fontSize);
    // Destructor
    ~MyFontOutline();

    // Query routines:
    int		getNumOutlines() { return numOutlines; }
    int		getNumVerts(int i) { return numVerts[i]; }
    SbVec2f	&getVertex(int i, int j) { return verts[i][j]; }
    SbVec2f	getCharAdvance() { return charAdvance; }
    
    static MyFontOutline *getNullOutline();

  private:
    // Internal constructor used by getNullOutline:
    MyFontOutline();
    
    // This basically mimics the FLoutline structure, with the
    // exception that the font size is part of the outline:
    int numOutlines;
    int *numVerts;
    SbVec2f **verts;
    SbVec2f charAdvance;
};

//
// Internal class: MyOutlineFontCache
//

// This is pretty heavyweight-- it is responsible for doing all of the
// grunt work of figuring out the polygons making up the characters in
// the font.
class MyOutlineFontCache : public SoCache
{
  public:
    // Given a state, find an appropriate outline font.
    static MyOutlineFontCache	*getFont(SoState *, SbBool forRender);
    
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

    // Set up for GL rendering:
    void	setupToRenderFront(SoState *state);

    // Returns TRUE if this font cache has a display list for the
    // given character.  It will try to build a display list, if it
    // can.
    SbBool	hasFrontDisplayList(const char c, gluTESSELATOR *tobj);

    // Renders an entire string by using the GL callList() function.
    void	callFrontLists(const SbString &string, float off);

    // Renders a string in cases where display lists can't be buit.
    void	renderFront(const SbString &string, float width, 
			    gluTESSELATOR *tobj);

    // Callback registered with GLU used to detect tesselation errors.
    static void errorCB(GLenum whichErr);

  protected:

    // Free up display lists before being deleted
    virtual void	destroy(SoState *state);

  private:
    // Constructor
    MyOutlineFontCache(SoState *);
    // Destructor
    ~MyOutlineFontCache();

    // Return a convnient little class representing a character's
    // outline.
    MyFontOutline	*getOutline(const char c);
    
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

    // Number of characters in this font. Until we internationalize,
    // this will be 128 or less.
    int		numChars;

    // Display list for fronts of characters
    SoGLDisplayList *frontList;

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

    // List of outlines; these are also cached and created when
    // needed.
    MyFontOutline	**outlines;

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

SbBool MyOutlineFontCache::tesselationError = FALSE;
SbPList *MyOutlineFontCache::fonts = NULL;
FLcontext MyOutlineFontCache::context = NULL;

// Static stuff is used while generating primitives:
SoAsciiText *SoAsciiText::currentGeneratingNode = NULL;
SoPrimitiveVertex *SoAsciiText::genPrimVerts[3];
SbVec3f SoAsciiText::genTranslate;
int SoAsciiText::genWhichVertex = -1;
uint32_t SoAsciiText::genPrimType;
SoAction *SoAsciiText::genAction = NULL;
SbBool SoAsciiText::genBack = FALSE;
SbBool SoAsciiText::genTexCoord = TRUE;
const SoTextureCoordinateElement *SoAsciiText::tce = NULL;

SO_NODE_SOURCE(SoAsciiText);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoAsciiText::SoAsciiText()
//
////////////////////////////////////////////////////////////////////////
{
    SO_NODE_CONSTRUCTOR(SoAsciiText);

    SO_NODE_ADD_FIELD(string,	(""));
    SO_NODE_ADD_FIELD(spacing,	(1.0));
    SO_NODE_ADD_FIELD(justification,	(LEFT));
    SO_NODE_ADD_FIELD(width,	(0));

    // Set up static info for enumerated type field
    SO_NODE_DEFINE_ENUM_VALUE(Justification,	LEFT);
    SO_NODE_DEFINE_ENUM_VALUE(Justification,	RIGHT);
    SO_NODE_DEFINE_ENUM_VALUE(Justification,	CENTER);

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

SoAsciiText::~SoAsciiText()
//
////////////////////////////////////////////////////////////////////////
{
    if (myFont != NULL) myFont->unref();
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Performs GL rendering of a Text3.
//
// Use: extender

void
SoAsciiText::GLRender(SoGLRenderAction *action)
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

    if (tobj == NULL) {
	tobj = (gluTESSELATOR *) gluNewTess();
	gluTessCallback(tobj, GLU_BEGIN, (void (*)())glBegin);
	gluTessCallback(tobj, GLU_END, (void (*)())glEnd);
	gluTessCallback(tobj, GLU_VERTEX, (void (*)())glVertex2fv);
	gluTessCallback(tobj, GLU_ERROR,
			(void (*)())MyOutlineFontCache::errorCB);
    }

    // See if texturing is enabled
    genTexCoord = SoGLTextureEnabledElement::get(action->getState());

    if (materialPerPart) mb.sendFirst();

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
	float w = (line < width.getNum()) ? width[line] : 0;
	SbVec2f p = getStringOffset(line, w);
	if (p[0] != 0.0 || p[1] != 0.0)
	    glTranslatef(p[0], p[1], 0.0);
	renderFront(action, string[line], w, tobj);
	glPopMatrix();
    }
    
    if (genTexCoord) {
	glPopAttrib();
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Implements ray picking
//
// Use: extender

void
SoAsciiText::rayPick(SoRayPickAction *action)
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
SoAsciiText::computeBBox(SoAction *action, SbBox3f &box, SbVec3f &center)
//
////////////////////////////////////////////////////////////////////////
{
    // Set the center to be the origin, which is the natural "center"
    // of the text, regardless of justification
    center.setValue(0.0, 0.0, 0.0);

    SoState *state = action->getState();

    if (!setupFontCache(state))
	return;

    // Get the bounding box of all the characters:
    SbBox2f outlineBox;
    getFrontBBox(outlineBox);

    // If no lines and no characters, return empty bbox:
    if (outlineBox.isEmpty()) return;
    
    const SbVec2f &boxMin = outlineBox.getMin();
    const SbVec2f &boxMax = outlineBox.getMax();
		     
    SbVec3f min(boxMin[0], boxMin[1], 0);
    SbVec3f max(boxMax[0], boxMax[1], 0);
    box.extendBy(min);
    box.extendBy(max);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Generates triangles representing a Text3.
//
// Use: protected

void
SoAsciiText::generatePrimitives(SoAction *action)
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

    if (materialPerPart) {
	v1.setMaterialIndex(0);
	v2.setMaterialIndex(0);
	v3.setMaterialIndex(0);
    }

    v1.setNormal(SbVec3f(0, 0, 1));
    v2.setNormal(SbVec3f(0, 0, 1));
    v3.setNormal(SbVec3f(0, 0, 1));
    
    for (int line = 0; line < string.getNum(); line++) {
	detail.setStringIndex(line);

	float w = (line < width.getNum()) ? width[line] : 0;
	SbVec2f p = getStringOffset(line, w);
	genTranslate.setValue(p[0], p[1], 0);
	generateFront(string[line], w);
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
SoAsciiText::setupFontCache(SoState *state, SbBool forRender)
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
	myFont = MyOutlineFontCache::getFont(state, forRender);
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
SoAsciiText::getFrontBBox(SbBox2f &result)
//
////////////////////////////////////////////////////////////////////////
{
    SbBox2f charBBox;

    int line, character;
    for (line = 0; line < string.getNum(); line++) {
	// Starting position of string, based on justification:
	float w = (line < width.getNum()) ? width[line] : 0;
	SbVec2f charPosition = getStringOffset(line, w);
	SbVec2f curCharPos = charPosition;

	const SbString &str = string[line];
	const char *chars = str.getString();
	
	for (character = 0; character < str.getLength(); character++) {
	    myFont->getCharBBox(chars[character], charBBox);
	    if (!charBBox.isEmpty()) {
		SbVec2f min = charBBox.getMin() + curCharPos;
		SbVec2f max = charBBox.getMax() + curCharPos;
		result.extendBy(min);
		result.extendBy(max);
	    }

	    // And advance...
	    curCharPos += myFont->getCharOffset(chars[character]);
	}
	if (w > 0) { 
	    // force the bbox width
	    SbVec2f min = result.getMin();
	    SbVec2f max = result.getMax();
	    result.setBounds(charPosition[0], min[1], 
			     charPosition[0] + w, max[1]);
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
SoAsciiText::getStringOffset(int line, float width)
//
////////////////////////////////////////////////////////////////////////
{
    SbVec2f result(0,0);
    
    if (justification.getValue() == RIGHT) {
	if (width <= 0)
	    width = myFont->getWidth(string[line]);
	result[0] = -width;
    }
    if (justification.getValue() == CENTER) {
	if (width <= 0)
	    width = myFont->getWidth(string[line]);
	result[0] = -width/2.0;
    }
    result[1] = -line*myFont->getHeight()*spacing.getValue();

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
SoAsciiText::renderFront(SoGLRenderAction *, const SbString &string,
		     float width, gluTESSELATOR *tobj)
//
////////////////////////////////////////////////////////////////////////
{
    const char *chars = string.getString();

    // if we have a fixed width, use it.
    float off = 0;
    if (width > 0) {
	float naturalWidth = myFont->getWidth(string);
	off = (width - naturalWidth) / (string.getLength() - 1);
    }
    
    // First, try to figure out if we can use glCallLists:
    SbBool useCallLists = TRUE;

    for (int i = 0; i < string.getLength(); i++) {
	// See if the font cache already has (or can build) a display
	// list for this character:
	if (!myFont->hasFrontDisplayList(chars[i], tobj)) {
	    useCallLists = FALSE;
	    break;
	}
    }
    
    // if we have display lists for all of the characters, use
    // glCallLists:
    if (useCallLists && off == 0) {
	myFont->callFrontLists(string, off);
    }
    // if we don't, draw the string character-by-character, using the
    // display lists we do have:
    else {
	myFont->renderFront(string, off, tobj);
    }
}    

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Create text detail and fill in extra info.
//
// Use: protected, virtual

SoDetail *
SoAsciiText::createTriangleDetail(SoRayPickAction *,
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
SoAsciiText::generateFront(const SbString &string, float width)
//
////////////////////////////////////////////////////////////////////////
{
    static gluTESSELATOR *tobj = NULL;

    const char *chars = string.getString();

    if (tobj == NULL) {
	tobj = (gluTESSELATOR *) gluNewTess();
	gluTessCallback(tobj, GLU_BEGIN, (void (*)())SoAsciiText::beginCB);
	gluTessCallback(tobj, GLU_END, (void (*)())SoAsciiText::endCB);
	gluTessCallback(tobj, GLU_VERTEX, (void (*)())SoAsciiText::vtxCB);
	gluTessCallback(tobj, GLU_ERROR,
			(void (*)())MyOutlineFontCache::errorCB);
    }

    genWhichVertex = 0;

    SoTextDetail *d = (SoTextDetail *)genPrimVerts[0]->getDetail();
    
    // if we have a fixed width, use it.
    float off = 0;
    if (width > 0) {
	float naturalWidth = myFont->getWidth(string);
	off = (width - naturalWidth) / (string.getLength() - 1);
    }
    
    for (int i = 0; i < string.getLength(); i++) {
	d->setCharacterIndex(i);

	myFont->generateFrontChar(chars[i], tobj);

	SbVec2f p = myFont->getCharOffset(chars[i]);
	genTranslate[0] += p[0] + off;
	genTranslate[1] += p[1];
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
SoAsciiText::beginCB(GLenum primType)
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
SoAsciiText::endCB()
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
SoAsciiText::vtxCB(void *v)
//
////////////////////////////////////////////////////////////////////////
{
    SbVec2f &vv = *((SbVec2f *)v);
    float vertex[3];
    vertex[0] = vv[0] + genTranslate[0];
    vertex[1] = vv[1] + genTranslate[1];
    vertex[2] = genTranslate[2];

    SoAsciiText *t3 = currentGeneratingNode;
    
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

MyOutlineFontCache *
MyOutlineFontCache::getFont(SoState *state, SbBool forRender)
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
	    SoDebugError::post("SoAsciiText::getFont",
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

    MyOutlineFontCache *result = NULL;
    for (int i = 0; i < fonts->getLength() && result == NULL; i++) {
	MyOutlineFontCache *c = (MyOutlineFontCache *) (*fonts)[i];
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
	result = new MyOutlineFontCache(state);

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
MyOutlineFontCache::isValid(SoState *state) const
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
MyOutlineFontCache::isRenderValid(SoState *state) const
//
////////////////////////////////////////////////////////////////////////
{
    if (!isValid(state)) return FALSE;

    if (frontList && 
	frontList->getContext() != SoGLCacheContextElement::get(state))
	return FALSE;

    return TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor.  Called by getFont().
//
// Use: private

MyOutlineFontCache::MyOutlineFontCache(SoState *state) : 
	SoCache(state)
//
////////////////////////////////////////////////////////////////////////
{
    ref();

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
	    SoDebugError::post("SoAsciiText::getFont",
		      "Couldn't find font %s, replacing with Utopia-Regular",
		       font.getString());
#endif
	    fontId = flCreateFont((GLubyte *)"Utopia-Regular", m, 0, NULL);
	}
	if (fontId == 0) {
#ifdef DEBUG
	    SoDebugError::post("SoAsciiText::getFont",
			       "Couldn't find font Utopia-Regular!");
#endif
	    return;
	}
    }

    flMakeCurrentFont(fontId);

    numChars = 256;  // ??? NEED TO REALLY KNOW HOW MANY CHARACTERS IN
		     // FONT!
    frontList = NULL;

    frontFlags = new SbBool[numChars];
    outlines = new MyFontOutline*[numChars];
    int i;
    for (i = 0; i < numChars; i++) {
	frontFlags[i] = FALSE;
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

MyOutlineFontCache::~MyOutlineFontCache()
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
	    MyOutlineFontCache *t = (MyOutlineFontCache *)(*fonts)[i];
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
MyOutlineFontCache::destroy(SoState *)
//
////////////////////////////////////////////////////////////////////////
{
    // Pass in NULL to freeList because this cache may be destroyed
    // from an action _other_ than GLRender:
    if (frontList) {
	frontList->unref();
	frontList = NULL;
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
MyOutlineFontCache::getWidth(const SbString &string)
//
////////////////////////////////////////////////////////////////////////
{
    float total = 0.0;
    const char *chars = string.getString();
    
    for (int i = 0; i < string.getLength(); i++) {
	MyFontOutline *outline = getOutline(chars[i]);
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
MyOutlineFontCache::getCharBBox(const char c, SbBox2f &result)
//
////////////////////////////////////////////////////////////////////////
{
    result.makeEmpty();

    if (!fontId) return;
    
    MyFontOutline *outline = getOutline(c);
    
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

MyFontOutline *
MyOutlineFontCache::getOutline(const char c)
//
////////////////////////////////////////////////////////////////////////
{
    if (!fontId) {
	return MyFontOutline::getNullOutline();
    }
    
    if (outlines[c] == NULL) {
	FLoutline *flo = flGetOutline(fontId, VALIDATE_CHAR(c));
	if (flo == NULL) {
	    outlines[c] = MyFontOutline::getNullOutline();
	} else {
	    outlines[c] = new MyFontOutline(flo, fontSize);
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
MyOutlineFontCache::getCharOffset(const char c)
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
MyOutlineFontCache::generateFrontChar(const char c,
				      gluTESSELATOR *tobj)
//
////////////////////////////////////////////////////////////////////////
{
    if (!fontId) return;

    GLdouble v[3];

    tesselationError = FALSE;
    gluBeginPolygon(tobj);
    
    // Get outline for character
    MyFontOutline *outline = getOutline(c);
    int i;
    for (i = 0; i < outline->getNumOutlines(); i++) {

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
MyOutlineFontCache::setupToRenderFront(SoState *state)
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
//    Returns TRUE if a display lists exists for given character.
//    Tries to build a display list, if it can.
//
// Use: internal

SbBool
MyOutlineFontCache::hasFrontDisplayList(const char c, 
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
//    Assuming that there are display lists built for all the
//    characters in given string, render them using the GL's CallLists
//    routine.
//
// Use: internal

void
MyOutlineFontCache::callFrontLists(const SbString &string, float off)
//
////////////////////////////////////////////////////////////////////////
{
    const char *str = string.getString();

    if (off == 0)
	glCallLists(string.getLength(), GL_UNSIGNED_BYTE, str);
    else {
	for (int i = 0; i < string.getLength(); ++i) {
	    glCallLists(1, GL_UNSIGNED_BYTE, str+i);
	    glTranslatef(off, 0.0, 0.0);
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
MyOutlineFontCache::renderFront(const SbString &string, float off, 
				gluTESSELATOR *tobj)
//
////////////////////////////////////////////////////////////////////////
{
    const char *str = string.getString();

    for (int i = 0; i < string.getLength(); i++) {
	if (frontFlags[str[i]] && off == 0) {
	    glCallList(frontList->getFirstIndex()+str[i]);
	}
	else {
	    generateFrontChar(str[i], tobj);
	    SbVec2f t = getOutline(str[i])->getCharAdvance();
	    glTranslatef(t[0] + off, t[1], 0.0);
	}
    }
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   Copy info from the font library into a more convenient form.
//
// Use: internal

MyFontOutline::MyFontOutline(FLoutline *outline, float fontSize)
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

MyFontOutline::~MyFontOutline()
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

MyFontOutline *
MyFontOutline::getNullOutline()
//
////////////////////////////////////////////////////////////////////////
{
    return new MyFontOutline;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//   Internal constructor used by getNullOutline
//
// Use: internal, static

MyFontOutline::MyFontOutline()
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
//    Given a set of line segments, this figures out the normal at
//    each point in each segment.  It uses the creaseAngle passed in
//    to decide whether or not adjacent segments share normals.  The
//    isClosed flag is used to determine whether or not the first and
//    last points should be considered another segment.
//
//
// Use: private

void
MyOutlineFontCache::figureSegmentNorms(SbVec2f *norms, int nPoints,
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
MyOutlineFontCache::figureSegmentTexCoords(float *texCoords, int nPoints,
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
MyOutlineFontCache::fillBevel(SbVec3f *result, int nPoints,
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
MyOutlineFontCache::fillBevelN(SbVec3f *result, int nNorms,
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
MyOutlineFontCache::errorCB(GLenum whichErr)
{
    SoDebugError::post("SoAsciiText::errorCB", "%s", gluErrorString(whichErr));
    tesselationError = TRUE;
}
#else  /* DEBUG */
void
MyOutlineFontCache::errorCB(GLenum)
{
    tesselationError = TRUE;
}
#endif /* DEBUG */

