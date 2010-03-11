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
 * Copyright (C) 1990,91,92   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Description:
 |	Internal debugging helper routines.  This entire class is #ifdef
 |	DEBUG, it is not part of the optimized library.
 |
 |   Classes:
 |	SoDebug
 |
 |   Author(s)		: Gavin Bell
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#ifndef _SO_DEBUG_
#define _SO_DEBUG_

class SoBase;
class SoNode;
class SoField;

//
// List of environment variables for tracing:
//
//   IV_DEBUG_BUFLEN   : Number of lines RTPrintf saves up before
//	dumping out.  Defaults to 100, set to 1 to get it to dump
//	after every line.
//   IV_DEBUG_SENSORS  : will print out info as sensors are
//	triggered/etc
//   IV_DEBUG_CACHES   : print out info on cache validity, etc.
//   IV_DEBUG_CACHELIST : print out info on Separator render caches.
//

class SoDebug {
  public:
    // Returns value of environment variable; faster than getenv()
    // because the environment variables are stored in a dictionary.
    static const char *	GetEnv(const char *envVar);

    // Prints into an internal buffer that is emptied every once in a
    // while.  Useful when doing the print right away screws up the
    // timing of the thing you're trying to debug.
    static void		RTPrintf(const char *formatString ...);

    // Assign the given pointer a name.  The passed name string isn't
    // copied; if it changes, the name of the pointer changes.
    static void		NamePtr(const char *name, void *ptr);

    // Return the name of the given pointer (returns "<noName>" if not
    // named previously with NamePtr).
    static const char *	PtrName(void *ptr);

    // Applies an SoWriteAction to the graph rooted by the given node.
    // The results go to stdout.  Very useful when called from within
    // a debugger!
    static void		write(SoNode *node);

    // Applies an SoWriteAction to the graph rooted by the given node,
    // writing to given file, or /tmp/debug.iv if filename is NULL.
    static void		writeFile(SoNode *node, const char *filename);

    // Applies an SoWriteAction to the container of the given field.
    // The results go to stdout.  Very useful when called from within
    // a debugger!
    static void		writeField(SoField *node);

    // Writes the name of the given object to stdout.  Very useful
    // when called from within a debugger!
    static void		printName(SoBase *base);
};

#endif /* _SO_DEBUG_ */
