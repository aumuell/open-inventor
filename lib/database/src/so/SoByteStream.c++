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
 |   $Revision: 1.2 $
 |
 |   Classes:
 |	SoByteStream
 |
 |   Authors: David Mott
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


#include <stdlib.h>
#include <Inventor/misc/SoByteStream.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>
#include <Inventor/SoLists.h>
#include <Inventor/SoOutput.h>
#include <Inventor/SoPath.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/nodes/SoSeparator.h>

//////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
//  Use: public
//
SoByteStream::SoByteStream()
//
//////////////////////////////////////////////////////////////////////////////
{
    data = NULL;
    numBytes = 0;
    isRaw = FALSE;
}

//////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
//  Use: public
//
SoByteStream::~SoByteStream()
//
//////////////////////////////////////////////////////////////////////////////
{
    if (data != NULL)
	free(data);
}

//////////////////////////////////////////////////////////////////////////////
//
//  Description:
//    Convert the passed path list to a byte stream.
//    binaryFormat determines whether to copy the data in binary (TRUE) or
//    ascii (FALSE) format.
//
//  Use: public
//
void
SoByteStream::convert(SoPathList *pathList, SbBool binaryFormat)
//
//////////////////////////////////////////////////////////////////////////////
{
    // clear out any old data
    if (data != NULL) {
	free(data);
	data = NULL;
	numBytes = 0;
    }
	
    if ((pathList == NULL) || (pathList->getLength() == 0))
	return;

    // Write all the paths in the path list into an in-memory buffer
    SoWriteAction wa;
    SoOutput *out = wa.getOutput();
    out->setBinary(binaryFormat);
    out->setBuffer(malloc(128), 128, realloc);

    for (int i = 0; i < pathList->getLength(); i++) {
    	wa.apply((*pathList)[i]);
    }

    // Point to the byte stream data
    void *buf;
    size_t size;

    out->getBuffer(buf, size);
    data = buf;
    numBytes = (uint32_t) size;
    isRaw = FALSE;
}

//////////////////////////////////////////////////////////////////////////////
//
//  Description:
//    Convert the passed path to a byte stream.
//    binaryFormat determines whether to copy the data in binary (TRUE) or
//    ascii (FALSE) format.
//
//  Use: public
//
void
SoByteStream::convert(SoPath *path, SbBool binaryFormat)
//
//////////////////////////////////////////////////////////////////////////////
{
    SoPathList pathList;

    pathList.append(path);
    convert(&pathList, binaryFormat);
}

//////////////////////////////////////////////////////////////////////////////
//
//  Description:
//    Convert the passed node to a byte stream.
//    binaryFormat determines whether to copy the data in binary (TRUE) or
//    ascii (FALSE) format.
//
//  Use: public
//
void
SoByteStream::convert(SoNode *node, SbBool binaryFormat)
//
//////////////////////////////////////////////////////////////////////////////
{
    SoPath *path = new SoPath(node);
    convert(path, binaryFormat);
}

//////////////////////////////////////////////////////////////////////////////
//
//  Description:
//    Convert the passed byte stream to a path list.
//    The caller should delete the returned path list when done with it.
//
//  Use: static, public
//
SoPathList *
SoByteStream::unconvert(SoByteStream *bs)
//
//////////////////////////////////////////////////////////////////////////////
{
    if (bs->isRawData())
	 return NULL;
    else return SoByteStream::unconvert(bs->getData(), bs->getNumBytes());
}

//////////////////////////////////////////////////////////////////////////////
//
//  Description:
//    Convert the passed byte stream to a path list.
//    The caller should delete the returned path list when done with it.
//
//  Use: static, public
//
SoPathList *
SoByteStream::unconvert(void *data, uint32_t numBytes)
//
//////////////////////////////////////////////////////////////////////////////
{
    if (data == NULL) {
	SoDebugError::post("SoByteStream::unconvert", "data is NULL");
	return NULL;
    }
    if (numBytes == 0) {
	SoDebugError::post("SoByteStream::unconvert", "numBytes is 0");
	return NULL;
    }

    SoInput in;
    SoPathList *pathList = new SoPathList;
    SoPath  *path = NULL;

    in.setBuffer((void *) data, (size_t) numBytes);
    
    // Try to read paths
    while ((SoDB::read(&in, path) != FALSE) && (path != NULL))
    	pathList->append(path);

    // If that failed, try reading the scene as a node
    if (pathList->getLength() == 0) {
	in.setBuffer((void *) data, (size_t) numBytes); // reset
	SoSeparator *sep = SoDB::readAll(&in);		// read again
	if (sep != NULL) {
	    path = new SoPath(sep);
	    pathList->append(path);
	}
    }

    return pathList;
}

//////////////////////////////////////////////////////////////////////////////
//
//  Description:
//   This allows apps to store raw data here without converting 
//   an Inventor node, path, or path list. This sets isRaw to TRUE,
//   and that data cannot be unconverted.
void		
SoByteStream::copy(void *d, size_t len)
//
//////////////////////////////////////////////////////////////////////////////
{
    data = malloc(len);
    if (data != NULL) {
	memcpy(data, d, (int)len);
	numBytes = len;
	isRaw = TRUE;
    }
}
