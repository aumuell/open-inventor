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
 * Copyright (C) 1989,1990,1991   Silicon Graphics, Inc.
 *
 ***********************************************************************
 **************  S I L I C O N   G R A P H I C S   I N C .  ************
 *
 *   Module File Name	:
 *	usr/src/toolkits/Inventor/tools/makemake/src/MMfiles.c
 *
 *   Description:
 *	This module writes out the lines containing C_FILES, H_FILES, 
 *	and O_FILES to the Makefile we are creating.
 *
 *   Functions: 
 *	MMgen_files	: Generate C_FILES, H_FILES, O_FILES vars.
 *
 *   RCS Revision	: $Revision: 1.1.1.1 $
 *
 *   Author(s)		: David Mott
 *
 **************  S I L I C O N   G R A P H I C S   I N C .  ************
 ***********************************************************************
 */

#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include "makemake.h"

static char *prefix;  /* used by gen_h_files */
static char *obj_printout; /* used by gen_o_files */

/*
 ***********************************************************************
 *
 *   Synopsis:
 *	int	MMgen_cfiles(file, srcdir)
 *
 *   Description:
 *	This routine outputs to the Makefile the list of all .c files
 *	in directory dirname. Return number of c files found.
 *
 ***********************************************************************
 */
int
MMgen_cfiles(FILE *f, char *srcdir)
{
    struct dirent *direntry;
    DIR  *dirp;
    char *p;
    int  numfound = 0;

    if ((f == NULL) || (srcdir == NULL)) {
	(void) fprintf(stderr, "Error, NULL pointer in gen_cfiles\n");
	exit (-1);
    }

    if (dirp = opendir(srcdir)) {
        while (direntry = readdir(dirp)) {
            p = direntry->d_name + strlen(direntry->d_name) - 3;
            /* look for .c files (and not ..c files) */
            if ((p[1] == '.') && (p[2] == 'c') && (p[0] != '.')) {
		if (numfound == 0) {
		    (void) fprintf(f, "# c source files in this directory\n");
		    (void) fprintf(f, "CFILES =");
		}
                (void) fprintf(f, " \\\n\t%s", direntry->d_name);
		numfound++;
            }
        }
        (void) closedir(dirp);
   }
   (void) fprintf(f, "\n\n");

   return (numfound);
}

/*
 ***********************************************************************
 *
 *   Synopsis:
 *	int	MMgen_cplusplusfiles(file, srcdir)
 *
 *   Description:
 *	This routine outputs to the Makefile the list of all .c++ and .C
 *	files in directory dirname. Return number of c++ files found.
 *
 ***********************************************************************
 */
int
MMgen_cplusplusfiles(FILE *f, char *srcdir)
{
    struct dirent *direntry;
    DIR  *dirp;
    char *p, *q;
    int  numfound = 0;

    if ((f == NULL) || (srcdir == NULL)) {
	(void) fprintf(stderr, "Error, NULL pointer in gen_cplusplusfiles\n");
	exit (-1);
    }

    if (dirp = opendir(srcdir)) {
        while (direntry = readdir(dirp)) {
            p = direntry->d_name + strlen(direntry->d_name) - 4;
            q = direntry->d_name + strlen(direntry->d_name) - 2;
            /* look for .c++ and .C files */
            if (((p[0] == '.') && (p[1] == 'c') && (p[2] == '+') && (p[3] == '+')) ||
                ((q[0] == '.') && (q[1] == 'C'))) {
		if (numfound == 0) {
		    (void) fprintf(f, "# c++ source files in this directory\n");
		    (void) fprintf(f, "C++FILES =");
		}
                (void) fprintf(f, " \\\n\t%s", direntry->d_name);
		numfound++;
            }
        }
        (void) closedir(dirp);
   }
   (void) fprintf(f, "\n\n");

   return (numfound);
}

/*
 ***********************************************************************
 *
 *   Synopsis:
 *	int	MMcount_cplusplusfiles(srcdir)
 *
 *   Description:
 *	This routine counts the number of c++ files in a directory.
 *
 ***********************************************************************
 */
int
MMcount_cplusplusfiles(char *srcdir)
{
    struct dirent *direntry;
    DIR  *dirp;
    char *p, *q;
    int  numfound = 0;

    if (srcdir == NULL) {
	(void) fprintf(stderr, "Error, srcdir is NULL in MMcount_cplusplusfiles\n");
	exit (-1);
    }

    if (dirp = opendir(srcdir)) {
        while (direntry = readdir(dirp)) {
            p = direntry->d_name + strlen(direntry->d_name) - 4;
            q = direntry->d_name + strlen(direntry->d_name) - 2;
            /* look for .c++ and .C files */
            if (((p[0] == '.') && (p[1] == 'c') && (p[2] == '+') && (p[3] == '+')) ||
                ((q[0] == '.') && (q[1] == 'C')))
		numfound++;
        }
        (void) closedir(dirp);
   }

   return (numfound);
}

/*
 ***********************************************************************
 *
 *   Synopsis:
 *	int	MM_count_hfiles(srcdir)
 *
 *   Description:
 *	This routine counts the number of h files in a directory.
 *
 ***********************************************************************
 */
int
MM_count_hfiles(char *srcdir)
{
    struct dirent *direntry;
    DIR  *dirp;
    char *p, *q;
    int  numfound = 0;

    if (srcdir == NULL) {
	(void) fprintf(stderr, "Error, srcdir is NULL in MM_count_hfiles\n");
	exit (-1);
    }

    if (dirp = opendir(srcdir)) {
        while (direntry = readdir(dirp)) {
            q = direntry->d_name + strlen(direntry->d_name) - 2;
            /* look for .h files */
            if ((q[0] == '.') && (q[1] == 'h'))
		numfound++;
        }
        (void) closedir(dirp);
   }

   return (numfound);
}

/*
 ***********************************************************************
 *
 *	generate a list of .h files in the passed directory.
 *
 ***********************************************************************
*/
int
MM_gen_hfile_list(FILE *f, char *dir, char *dirname)
{
    struct dirent *direntry;
    DIR  *dirp;
    char *p;
    int  numfound = 0;

    if (dirp = opendir(dir)) {
        while (direntry = readdir(dirp)) {
            p = direntry->d_name + strlen(direntry->d_name) - 2;
            /* look for .h files */
            if ((p[0] == '.') && (p[1] == 'h')) {
		if (dirname)
                     (void) fprintf(f, " \\\n\t%s/%s", dirname, direntry->d_name);
                else (void) fprintf(f, " \\\n\t%s", direntry->d_name);
		numfound++;
            }
        }
        (void) closedir(dirp);
   }

   return (numfound);
}

/*
 ***********************************************************************
 *
 *	call func for with each subdir of the passed dir.
 *
 ***********************************************************************
*/
int
now_do_subdirs_of_include(f)
FILE *f;
{
   /* assumption: ls -R prints a ':' after subdirectories */
   char *cmd = "ls -R include";
   char subdir[MM_NAME_LENGTH+1];
   char subdirname[MM_NAME_LENGTH+1];
   FILE *stream;
   int  lastchar_pos;
   int num = 0;
   char *p;

   if ((stream = popen(cmd, "r")) != NULL) {
      while (fgets(subdir, MM_NAME_LENGTH, stream) != NULL) {
         /* see if this is a directory. if it ends in ':', then it is. */
         lastchar_pos = strlen(subdir) - 2;
         if (subdir[lastchar_pos] == ':') {
            /* directory! remove the trailing ':' (and '\n') */
            subdir[lastchar_pos] = '\0';

	    /* subdirname is everything minus the first dirname */
	    p = subdir;
	    while (*p != '/') p++;
	    p++;
	    strcpy(subdirname, p);
            num += MM_gen_hfile_list(f, subdir, subdirname);
         }
      }
      (void) pclose(stream);
   }

    return num;
}

/*
 ***********************************************************************
 *
 *   Synopsis:
 *	int	MMgen_hfiles(file, dirname)
 *
 *   Description:
 *	This routine outputs to the Makefile the list of all .h files
 *	in the specified directory.  Return value is the number found.
 *
 ***********************************************************************
 */
int
MMgen_hfiles(FILE *f, char *dirname)
{
    int numfound = 0;

    if (f == NULL) {
	(void) fprintf(stderr, "Error, NULL pointer in MMgen_hfiles()\n");
	exit (-1);
    }

    if (MM_count_hfiles(dirname) > 0) {
	/* src directory, only list .h files in this directory */
	(void) fprintf(f, "# h files in this directory\n");
	(void) fprintf(f, "HFILES =");
	numfound = MM_gen_hfile_list(f, dirname, NULL);
	(void) fprintf(f, "\n\n");
    }

    return numfound;
}

/*
 ***********************************************************************
 *
 *   Synopsis:
 *	int	MMget_yfile(dirname, filenamebuf)
 *
 *   Description:
 *	This routine gets the name of any .y file in directory dirname
 *	and puts it in filenamebuf. This strips the .y from the name,
 *	simply putting the base file name in filenamebuf.
 *	This returns the number of .y files found.
 *
 *	NOTE: Currently, we can handle at most 1 .y file.
 *
 ***********************************************************************
 */
int
MMget_yfile(char *dirname, char *filenamebuf)
{
    struct dirent *direntry;
    DIR  *dirp;
    char *p;
    int  numfound = 0;

    if ((dirname == NULL) || (filenamebuf == NULL)) {
	(void) fprintf(stderr, "Error, NULL pointer in get_yfile\n");
	exit (-1);
    }

    if (dirp = opendir(dirname)) {
        while ((direntry = readdir(dirp)) && !numfound) {
            p = direntry->d_name + strlen(direntry->d_name) - 2;
            /* look for .y files */
            if ((p[0] == '.') && (p[1] == 'y')) {
		p[0] = '\0';  /* strip the .y from the name! */
		strcpy(filenamebuf, direntry->d_name);
		numfound++;
            }
        }
        (void) closedir(dirp);
   }

   return (numfound);
}

/*
 ***********************************************************************
 *
 *   Synopsis:
 *	int	MMget_lfile(dirname, filenamebuf)
 *
 *   Description:
 *	This routine gets the name of any .l file in directory dirname
 *	and puts it in filenamebuf. This strips the .l from the name,
 *	simply putting the base file name in filenamebuf.
 *	This returns the number of .l files found.
 *
 *	NOTE: Currently, we can handle at most 1 .l file.
 *
 ***********************************************************************
 */
int
MMget_lfile(char *dirname, char *filenamebuf)
{
    struct dirent *direntry;
    DIR  *dirp;
    char *p;
    int  numfound = 0;

    if ((dirname == NULL) || (filenamebuf == NULL)) {
	(void) fprintf(stderr, "Error, NULL pointer in get_yfile\n");
	exit (-1);
    }

    if (dirp = opendir(dirname)) {
        while ((direntry = readdir(dirp)) && !numfound) {
            p = direntry->d_name + strlen(direntry->d_name) - 2;
            /* look for .l files */
            if ((p[0] == '.') && (p[1] == 'l')) {
		p[0] = '\0';  /* strip the .l from the name! */
		strcpy(filenamebuf, direntry->d_name);
		numfound++;
            }
        }
        (void) closedir(dirp);
   }

   return (numfound);
}

/*
 ***********************************************************************
 *
 *   Synopsis:
 *	void	MM_foreach_dir( void (*gen_func) () )
 *
 *   Description:
 *	this calls gen_func for the subdir directory, and any subdirectories
 *	in subdir. 
 *
 ***********************************************************************
 */
void
MM_foreach_dir(gen_func, srcdir_list, subdir)
void (*gen_func)();
char *srcdir_list;
char *subdir;
{
    /* assumption: ls -R prints a ':' after subdirectories */
    char cmd[160];
    char subdir_name[MM_NAME_LENGTH+1];
    FILE *stream;
    int  lastchar_pos;

    sprintf(cmd, "ls -R %s", subdir);
    (*gen_func) (subdir);
    strcat(srcdir_list, subdir);
    strcat(srcdir_list, " ");

    if ((stream = popen(cmd, "r")) != NULL) {
        while (fgets(subdir_name, MM_NAME_LENGTH, stream) != NULL) {
            /* see if this is a directory. if it ends in '/', then it is. */
            lastchar_pos = strlen(subdir_name) - 2;
            if (subdir_name[lastchar_pos] == ':') {
                /* directory! remove the trailing ':' (and '\n') */
                subdir_name[lastchar_pos] = '\0';
                (*gen_func) (subdir_name);

	        /* add the directory to the list of subdirectories */
                strcat(srcdir_list, subdir_name);
                strcat(srcdir_list, " ");
            }
        }
        (void) pclose(stream);
    }
}
