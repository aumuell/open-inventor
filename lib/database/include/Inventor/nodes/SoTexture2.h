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
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1 $
 |
 |   Description:
 |	This file defines the SoTexture node class.
 |
 |   Author(s)		: John Rohlf, Thad Beier, Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_TEXTURE_2_
#define  _SO_TEXTURE_2_

#include <Inventor/fields/SoSFColor.h>
#include <Inventor/fields/SoSFEnum.h>
#include <Inventor/fields/SoSFImage.h>
#include <Inventor/fields/SoSFString.h>
#include <Inventor/nodes/SoSubNode.h>

#include <GL/gl.h>

class SoSensor;
class SoFieldSensor;
class SoGLDisplayList;

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoTexture
//
//  Texture node.
//
//////////////////////////////////////////////////////////////////////////////

// C-api: prefix=SoTex2
// C-api: public=filename, image, wrapS, wrapT, model, blendColor
class SoTexture2 : public SoNode {

    SO_NODE_HEADER(SoTexture2);

  public:
    enum Model {			// Texture model
	MODULATE		= GL_MODULATE,
	DECAL			= GL_DECAL,
	BLEND			= GL_BLEND
    };

    enum Wrap {				// Texture wrap type
	REPEAT			= GL_REPEAT,
	CLAMP			= GL_CLAMP
    };

    // Fields.
    SoSFString		filename;	// file to read texture from
    SoSFImage		image;		// The texture
    SoSFEnum		wrapS;
    SoSFEnum		wrapT;
    SoSFEnum		model;
    SoSFColor		blendColor;

    // Constructor
    SoTexture2();
    
  SoEXTENDER public:
    virtual void	doAction(SoAction *action);
    virtual void	GLRender(SoGLRenderAction *action);
    virtual void	callback(SoCallbackAction *action);

  SoINTERNAL public:
    static void		initClass();

    // Read libimage-format file
    static SbBool   readImage(const SbString& fname, int &w, int &h, int &nc,
				unsigned char *&bytes);

  protected:
    // Reads stuff into instance. Returns FALSE on error.
    virtual SbBool	readInstance(SoInput *in, unsigned short flags);

    virtual ~SoTexture2();

    int		    getReadStatus()		{ return readStatus; }
    void	    setReadStatus(int s)	{ readStatus = s; }

  private:
    // These keep the image and filename fields in sync.
    SoFieldSensor *	imageSensor;
    static void		imageChangedCB(void *, SoSensor *);
    SoFieldSensor *	filenameSensor;
    static void		filenameChangedCB(void *, SoSensor *);
    
    int			readStatus;
    

    // Display list info for this texture:
    SoGLDisplayList *renderList;
    float	renderListQuality;
};

#endif /* _SO_TEXTURE_2_ */
