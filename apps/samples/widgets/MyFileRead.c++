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
 * Copyright (C) 1994-95   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.2 $
 |
 |   Functions:
 |	MyFileRead()
 |
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <unistd.h>
#include <stdlib.h>
#include <libgen.h>
#include <sys/param.h>
#include <Inventor/SoDB.h>
#include <Inventor/SoInput.h>

////////////////////////////////////////////////////////////////////////
//
// Convert filename to Inventor..
// Returns a pipe to read data from if conversion was successful.
// That pipe is NULL if the conversion failed.
// This uses routeprint to perform the conversion via FTR rules.
// It will fail gracefully if routeprint and/or the FTR database don't exist.
//
static FILE *
convertToInventor(const char *filename)
//
////////////////////////////////////////////////////////////////////////
{
#define BUFSIZE 512
#define destinationFileType "Inventor2.1File"

    char routeprintCmd[BUFSIZE];
    char conversionCmd[BUFSIZE];
    FILE *pipeFile;
    FILE *ivDataPipe = NULL;
    
    // Use routeprint to figure out how to convert to Inventor format
    sprintf(routeprintCmd, "/usr/sbin/routeprint -d %s %s 2> /dev/null",
	    destinationFileType, filename);

    // routeprint will return a conversion command which we can
    // then execute to convert the file.
    if (NULL != (pipeFile = popen(routeprintCmd, "r"))) {
	// read the command, which may be garbage: check return code
	(void) fgets(conversionCmd, BUFSIZE, pipeFile);
	// use pclose to check the return code of routeprint:
	// you have to check for non-zero return before executing conversionCmd
	if (0 != pclose(pipeFile)) {
	    // Failed!
	    return ivDataPipe;
	}
	// Now try to execute the actual conversion.
	// If conversion is successful, we can read from the data pipe.
	// If not, the data pipe will be NULL: which is our return code.
	ivDataPipe = popen(conversionCmd, "r");
    } 
    
    return ivDataPipe;
}

/////////////////////////////////////////////////////////////////////////////
//
// Read all objects from the given file and return under one separator.
//
SoSeparator *
MyFileRead(const char *filename, SbString &errorMessage)
//
/////////////////////////////////////////////////////////////////////////////
{
    SoInput in;
    SbBool needToClosePipe = FALSE;
    FILE *ivDataPipe;

    // First check to make sure the file exists and is readable
    if (0 != access(filename, R_OK)) {
	errorMessage = "Error opening the file\n\"";
	errorMessage += filename;
	errorMessage += "\".";
	errorMessage += "\n\nThe file is unreadable or does not exist.";
	errorMessage += "\nYou may not have read permission on this file.";
	return NULL;
    }

    // Now open the file in Inventor so we can see if it's valid 
    if (! in.openFile(filename, TRUE)) {
	errorMessage = "Error opening the file\n\"";
	errorMessage += filename;
	errorMessage += "\".";
	errorMessage += "\n\nInventor was unable to read that file.";
	errorMessage += "\nYou may not have permission to read this file.";
	return NULL;
    }

    // Is the file a valid Inventor file?  Use isValidFile() since the
    // list of file headers Inventor accepts may grow from release to release.
    
    // We check to see if it is NOT a valid Inventor file first.
    if (! in.isValidFile() ) {
	// Not recognized: try to convert to a valid Inventor filetype
	
	if ((ivDataPipe = convertToInventor(filename)) != NULL) {
	    // Set up SoInput for this pipe
	    in.setFilePointer(ivDataPipe);
	    needToClosePipe = TRUE;
	} else {
	    // File could not be CONVERTed to readable format.
	    errorMessage = "Unable to read the file\n\"";
	    errorMessage += filename;
	    errorMessage += "\".";
	    errorMessage += "\n\nInventor was unable to read that file, and";
	    errorMessage += "\nthat file could not be translated to Inventor format.\n\n";
	    errorMessage += "Are you sure that this is a supported filetype?";
	    errorMessage += "\nYou may not have the proper translators installed,";
	    errorMessage += "\nor the desktop filetype database may not be built.";
            return NULL; 
	}
    } 

    // If we get here, we have a valid SoInput and a valid Inventor file.
    
    // Read that file
    SoSeparator *sep = SoDB::readAll(&in);

    // If a bad read occurred, or no node was read, error
    if (sep == NULL) {
	errorMessage = "Inventor encountered an error while reading\n\"";
	errorMessage += filename;
	errorMessage += "\".";
	errorMessage += "\n\nThis may not be a valid Inventor file.";
	return NULL;
    }

    // Close all opened files (and the pipe if necessary)
    in.closeFile();
    if (TRUE == needToClosePipe) {
	fclose(ivDataPipe);
    }
    return sep;
}

