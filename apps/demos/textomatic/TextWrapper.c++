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

//
// Class to do word-wrap and simple storage of strings
//
// Note: I think this still has bugs when a word spans an entire line.
//


#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <iconv.h>

#include "TextWrapper.h"

// This is not a good idea for UCS2, but for quick modification...
#define  OFFSET		sizeof(UCS2)-1	
#define  GETCHAR(x)	(*(((char *)(&((x))))+(OFFSET)))

// UTF-8 stuff. (We'd better avoid globals...) 
extern iconv_t global_iconvCodeL2;
extern iconv_t global_iconvCode28;

//
// Constructor.  This actually does the formatting right away.
//
TextWrapper::TextWrapper(const char *texti, int maxLineLength)
{
    assert(texti);

    // Convert to UCS-2.
    //
    size_t   inbytes  = strlen(texti);
    char    *inbuf    = new char[inbytes+1];
    strncpy( inbuf,     texti,   inbytes ); inbuf[inbytes]='\0';
    size_t   outbytes = inbytes*4;
    char    *outbuf   = new char[outbytes];
    //
    size_t   inbytesleft  = inbytes;
    char    *input        = inbuf;
    size_t   outbytesleft = outbytes;
    char    *output       = outbuf;
    //
    if ( global_iconvCodeL2 == (iconv_t)-1   ||
	 iconv(global_iconvCodeL2,&input,&inbytesleft,&output,&outbytesleft) == (size_t)-1 )
    {
	 fprintf( stderr, "textomatic: iconv error.\n" );
	 (*(UCS2 *)outbuf)=0;
    }
    else {
	 outbuf[outbytes-outbytesleft] = 0;
	 outbuf[outbytes-outbytesleft+OFFSET] = '\0';
    }
    delete [] inbuf;
    UCS2 *text = (UCS2 *)outbuf;

    // Figure out how many paragraphs there are
    n_paragraphs = 0;
    int start = 0; 
    int end = 0;

    while ( GETCHAR(text[end]) != '\0' )
    {
	while ( GETCHAR(text[start]) == '\n' )
	{
	    ++start; // Skip leading newlines
	}
	
	end = start;
	if ( GETCHAR(text[start]) != '\0' )
	{
	    // Search for next newline
	    while ( GETCHAR(text[end]) != '\n' && GETCHAR(text[end]) != '\0' ) ++end;
	    ++n_paragraphs;
	    start = end;
	}
    }
    
    // Now allocate and fill in paragraphs

    paragraphs = new Paragraph[n_paragraphs];
    start = end = 0;
    int pnum = 0;

    while ( GETCHAR(text[end]) != '\0' )
    {
	while ( GETCHAR(text[start]) == '\n' )
	{
	    ++start; // Skip leading newlines
	}
	
	end = start;
	if ( GETCHAR(text[start]) != '\0' )
	{
	    // Search for next newline
	    while ( GETCHAR(text[end]) != '\n' && GETCHAR(text[end]) != '\0') ++end;
	    // And fill in the paragraph
	    WordWrap(paragraphs[pnum], text+start, end-start,
		     maxLineLength);
	    ++pnum;
	    start = end;
	}
    }
}

int
TextWrapper::numParagraphs()
{
    return n_paragraphs;
}

int
TextWrapper::numLines(int whichParagraph)
{
    assert(whichParagraph < n_paragraphs);
    return paragraphs[whichParagraph].n_lines;
}

char *
TextWrapper::getLine(int paragraph, int line)
{
    assert(paragraph < n_paragraphs);
    assert(line < paragraphs[paragraph].n_lines);

    return paragraphs[paragraph].lines[line];
}

//
// Destructor:  frees all storage used
//
TextWrapper::~TextWrapper()
{
    for (int i = 0; i < n_paragraphs; i++)
    {
	for (int j = 0; j < paragraphs[i].n_lines; j++)
	{
	    delete[] paragraphs[i].lines[j];
	}
	delete[] paragraphs[i].lines;
    }
    delete[] paragraphs;
}

void
TextWrapper::WordWrap(Paragraph &p, const UCS2 *text, int nchars, int
		      maxLineLength)
{
    int start = 0; int end = 0;

    p.n_lines = 0;

    // First, figure out how many lines there are
    while (end < nchars)
    {
	if (start + maxLineLength < nchars)
	{
	    end = start + maxLineLength;
	    while ( GETCHAR(text[end]) != ' ' && GETCHAR(text[end]) != '\t' && end >
		   start)
		--end;
	}
	else end = nchars;

	++p.n_lines;

	start = end+1;
    }

    // Allocate and fill in lines
    p.lines = new char *[p.n_lines];
    int linenum = 0; start = end = 0;
    while (end < nchars)
    {
	if (start + maxLineLength < nchars)
	{
	    end = start + maxLineLength;
	    while ( GETCHAR(text[end]) != ' ' && GETCHAR(text[end]) != '\t' && end >
		   start)
		--end;
	}
	else end = nchars;

        // Convert to UTF-8.
        //
	size_t   inbytes  = (end-start)*sizeof(UCS2);
	char    *inbuf    = (char *) new UCS2[end-start];
	memcpy(  inbuf,     text+start, inbytes );
	size_t   outbytes = inbytes*4;
	char    *outbuf   = new char[outbytes];
	//
     	size_t   inbytesleft  = inbytes;
	char    *input= inbuf;
	size_t   outbytesleft = outbytes;
	char    *output       = outbuf;
	//
	if ( global_iconvCode28 == (iconv_t)-1   ||
	     iconv( global_iconvCode28,&input,&inbytesleft,&output,&outbytesleft ) == (size_t)-1 )
	{
	     fprintf( stderr, "textomatic: iconv error.\n" );
	     (*(UCS2 *)outbuf)=0;
        }
        else {
	     outbuf[outbytes-outbytesleft]='\0';
        }
 	delete [] inbuf;
	p.lines[linenum]=outbuf;

	++linenum;
	start = end+1;
    }
}

