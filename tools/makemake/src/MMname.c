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
 * Copyright (C) 1989,   Silicon Graphics, Inc.
 *
 ***********************************************************************
 **************  S I L I C O N   G R A P H I C S   I N C .  ************
 *
 *   Module File Name	:
 *	usr/src/toolkits/Inventor/tools/makemake/src/MMname.c
 *
 *   Description:
 *	This module deals with names in strings.
 *
 *   Functions: 
 *	MM_next_name	: Finds the next name in a list.
 *	MM_setup_flags	: Puts a prefix,name,postfix together for a list
 *			  of names.
 *
 *   RCS Revision	: $Revision: 1.1.1.1 $
 *
 *   Author(s)		: David Mott
 *
 **************  S I L I C O N   G R A P H I C S   I N C .  ************
 ***********************************************************************
 */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include "makemake.h"


/*
 ***********************************************************************
 *
 *   Synopsis:
 *	void	MM_next_name( char *list, char *name )
 *
 *   Description:
 *	This gets the next name from list and copies it into name, then
 *	returns the length of name. A return value of 0 means that
 *	name is the empty string.
 *
 ***********************************************************************
 */
int
MM_next_name(char *list, char *name)
{
   char *listptr = list;
   char *nameptr = name;

   /* find start of the next name */
   while ((*listptr == ' ') && (*listptr != '\0'))
      listptr++;
   
   /* copy characters from buf to name until end of name */
   while ((*listptr != ' ') && (*listptr != '\0'))
      *nameptr++ = *listptr++;
   
   /* make name a string */
   *nameptr = '\0';

   return (strlen(name));
}

/*
 ***********************************************************************
 *
 *   Synopsis:
 *	void	MM_name_in_list( char *name, char *list )
 *
 *   Description:
 *	Returns TRUE if name is in list, else returns FALSE.
 *
 ***********************************************************************
 */
int
MM_name_in_list(char *name, char *list)
{
   int success = 0;
   int namelen;
   char member[MM_NAME_LENGTH];

   while (!success && ((namelen = MM_next_name(list, member)) != 0)) {
      success = (strcmp(name, member) == 0);
      list += (namelen + 1);
   }

   return (success);
}

/*
 ****************************************************************************
 *
 *   Synopsis:
 *	void	MM_setup_flags( MMsetup *setup, char *buf )
 *
 *   Description:
 *	This sets up a list of flags or names, based on the namelist
 *	passed. buf will consist of each name surrounded by the prefix
 *	and the postfix.
 *
 *	NOTE: if the name starts with '$' or '/', it is assumed to be
 *	a hard path name, and it is NOT surrounded by the prefix and postfix.
 *
 ****************************************************************************
 */
void
MM_setup_flags(char *namelist, char *prefix, char *suffix, char *buf)
{
   char *b = buf;
   char name[MM_NAME_LENGTH];
   int  namelen, pre_and_post_len; /* lengths of name, prefix, suffix strings */

   buf[0] = '\0';
   pre_and_post_len = strlen(prefix) + strlen(suffix);

   if (namelist) {
      while ((namelen = MM_next_name(namelist, name)) != 0) {
	 if ((name[0] == '$') || (name[0] == '/')) {
	     /* this is a hard path, do not surround the name */
	     (void) sprintf(b, "%s ", name);
	     namelist += (namelen + 1);
	     b += (namelen + 1); /* + 1 for the blank space */
	 }
	 else {
	     (void) sprintf(b, "%s%s%s", prefix, name, suffix);
	     namelist += (namelen + 1);
	     b += (namelen + pre_and_post_len);
	 }
      }
   }
   *b = '\0';
} /* MM_setup_flags() */



