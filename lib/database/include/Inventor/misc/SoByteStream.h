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
 |	This file defines the SoByteStream class.
 |
 |   Author(s): David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef  _SO_BYTE_STREAM_
#define  _SO_BYTE_STREAM_

#include <Inventor/SbBasic.h>

class SoNode;
class SoPath;
class SoPathList;

//////////////////////////////////////////////////////////////////////////////
//
//  Class: SoByteStream
//
//  This class creates a byte stream representation of a scene graph,
//  using the SoWriteAction to write to an in memory buffer. Byte streams
//  are commonly used to transfer data, for instance during copy and paste.
//
//////////////////////////////////////////////////////////////////////////////

class SoByteStream {
  public:
    SoByteStream();
   ~SoByteStream();

    // Convert the passed node, path, or path list into a byte stream.
    // Caller may specify whether the byte stream is written in binary
    // (TRUE) or ascii (FALSE) format. The converted data can be accessed
    // through getData() and getNumBytes().
    
    // C-api: name=convertNode
    void	        convert(SoNode *node, SbBool binaryFormat = TRUE);
    
    // C-api: name=convertPath
    void	        convert(SoPath *path, SbBool binaryFormat = TRUE);
    
    // C-api: name=convertPathList
    void	        convert(SoPathList *pathList, SbBool binaryFormat = TRUE);

    // Access the byte stream data
    void *    	    	getData()   	{ return data; }
    uint32_t   		getNumBytes()	{ return numBytes; }
    
    // Unconvert a byte stream back to a path list.
    // This static routine performs an SoDB::read on the data,
    // and returns a path list of the paths read in.
    
    // C-api: name=unconvertStream
    static SoPathList *	unconvert(SoByteStream *byteStream);
    
    // C-api: name=unconvertData
    static SoPathList *	unconvert(void *data, uint32_t numBytes);
  
  SoEXTENDER public:
    // This allows apps to store raw data here without converting 
    // an Inventor node, path, or path list. This sets isRaw to TRUE,
    // and that data cannot be unconverted.
    void		copy(void *d, size_t len);
    SbBool		isRawData() const { return isRaw; }
     
  private:
    void		*data;
    uint32_t		numBytes;
    SbBool		isRaw;
};

#endif // _SO_BYTE_STREAM_
