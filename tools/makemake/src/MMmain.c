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
 *   File Name	: Inventor/tools/makemake/src/MMmain.c
 *
 *   Description:
 *	This is the main module for the makemake program.
 *
 *   Revision	: $Revision: 1.1.1.1 $
 *   Author	: David Mott
 *
 **************  S I L I C O N   G R A P H I C S   I N C .  ************
 ***********************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "makemake.h"


static char *make_setup = "make.setup";
static char *makefile = "Makefile";
static int  cplusplus_code = 0;

/* public to makemake */
MMenviron *env;
MMsetup	  *setup;

extern void MMwrite_libMakefile(FILE *, char *, char *, char *);
extern void MMwrite_libSrcMakefile(FILE *, char *, char *, char *);
extern void MMwrite_appsMakefile(FILE *, char *, char *, char *);
extern void MMwrite_appsSrcMakefile(FILE *, char *, char *, char *);


/*
**	Get the dir name which is embedded in src somwhere
**	return TRUE if successful.
*/
static int
getDirName(char *dst, char *src, int type)
{
    /* the passed src is toolkits/Inventor/lib/XXX */
    /* or toolkits/Inventor/lib/XXX/src/... */
    /* or apps/Inventor/XXX */
    /* or apps/Inventor/XXX/src/... */
    char *s;
    int  c;
    int  depth;
    int  success = 1;

    if (type == MM_BIN) depth = 2;
    else		depth = 3;

    s = src;
    c = 0;
    while ((c < depth) && (*s != '\0')) {
	if (*s == '/') c++;
	s++;
    }
    
    if (c < depth) return 0;

    c = 0;
    while ((*s != '/') && (*s != '\0')) {
	dst[c] = *s;
	s++;
	c++;
    }
    dst[c] = '\0';

    success = (c != 0);

    return success;
}

/*
 ***********************************************************************
 *
 *   Synopsis:
 *	void	MMget_environment()
 *
 *   Description:
 *	This gets the current environment, including the name of this
 *	program or library, its type (MM_BIN or MM_LIB).
 *
 *	This returns TRUE if this is the toplevel directory.
 *	If it is dir under src, this returns FALSE.
 *
 ***********************************************************************
 */
static int
MMget_environment(char *srcdirs)
{
   int isTopLevel = TRUE;
   char *libDir =   "toolkits/Inventor/lib";
   char *libXtDir = "toolkits/Inventor/libSoXt";
   char *libGLDir = "toolkits/Inventor/libSoGL";
   char *appDir = "apps/Inventor";
   char *srcDir = "src";
   char *includeDir = "include/Inventor";
   char pathname[MM_PATHNAME_LENGTH];
   char *s, *t;


   if (getcwd(pathname, MM_PATHNAME_LENGTH) == NULL) {
      (void) fprintf(stderr, "Error. getcwd returned NULL.\n");
      exit(2);
   }

   env->so_root = getenv("ROOT");
   if (env->so_root == NULL) {
      (void) fprintf(stderr, "Error: setenv ROOT to the src root directory.\n");
      exit(1);
   }

   /* make sure the user is running makemake in the right place */
   s = strstr(pathname, libDir);
    if (s != NULL) {
	env->type = MM_LIB;
	/* see if this is a toplevel Makefile */
	t = (strstr(s, srcDir));
	if (t != NULL) {
	    isTopLevel = FALSE;
	    strcpy(srcdirs, t);
	} else {
	    srcdirs[0]=0;
	}
	t = (strstr(s, includeDir));
	if (t) {
	    isTopLevel = FALSE;
	    strcpy(env->includeDir, t);
	} else {
	    env->includeDir[0]=0;
	}
	if (getDirName(env->name, s, MM_LIB) == 0) {
	    (void) fprintf(stderr, "Error: cannot run makemake here.\n");
	    exit(1);
	}
    }
    else if ( (s = strstr( pathname, libXtDir)) != NULL ) {
	/*
	 *  Xt component directory.
	 */
	env->type = MM_BIN;
	/* see if this is a toplevel Makefile */
	t = (strstr(s, srcDir));
	if (t != NULL) {
	    isTopLevel = FALSE;
	    strcpy(srcdirs, t);
	} 
	if (getDirName(env->name, s, MM_BIN) == 0) {
	    (void) fprintf(stderr, "Error: cannot run makemake here.\n");
	    exit(1);
	}
    }
    else if ( (s = strstr( pathname, appDir)) != NULL ) {
       /*
	* App directory.
	*/
	env->type = MM_BIN;
	/* see if this is a toplevel Makefile */
	t = (strstr(s, srcDir));
	if (t != NULL) {
	    isTopLevel = FALSE;
	    strcpy(srcdirs, t);
	} 
	if (getDirName(env->name, s, MM_BIN) == 0) {
	    (void) fprintf(stderr, "Error: cannot run makemake here.\n");
	    exit(1);
	}
    }
    else {
       (void) fprintf(stderr, "Sorry, cannot run makemake here.\n");
       exit(1);
    }

   return isTopLevel;
}


/*
 ***********************************************************************
 *
 *   Synopsis:
 *	void	MM_get_setup_info( FILE *stream, 
 *				   char *data_out,
 *				   char *line_buf,
 *				   int maxdata )
 *
 *   Description:
 *	Copy data from the stream to data_out until end of line with no 
 *	line continuation character ('\'). line_buf contains the first
 *	line of data, including a keyword followed by ':', which we ignore.
 *
 ***********************************************************************
 */
static
void
MM_get_setup_info(FILE *stream, char *data_out, char *line_buf, int maxdata)
{
#define LINE_CONTINUE(C) ((C) == '\\')
#define END_OF_LINE(C) ((C) == '\n')
#define IS_SPACE(C) (((C) == ' ') || ((C) == '\t'))

   char *p;
   int  done = 0;

   /* skip up to the ':' separation character. */
   for (p = line_buf; *p != ':'; p++)
      ;

   /* and skip that ':' */
   p++;

   /* loop to get the data */
   while (!done) {
      /* find the first non white-space character on this line, if there is one */
      for ( ; IS_SPACE(*p) && !END_OF_LINE(*p); p++)
         ;

      /* copy data lines until we see end of line, or we see the line continuation char. */
      /* also, make sure we don't overflow the data_out buffer. */
      for ( ; (!LINE_CONTINUE(*p) && !END_OF_LINE(*p) && maxdata); p++, data_out++, maxdata--)
         *data_out = *p;
     
      /* if the last char was the line continuation char, then read another line and repeat */
      if (LINE_CONTINUE(*p)) {
         (void) fgets(line_buf, MM_LINE_LENGTH, stream);
         p = line_buf;
         if (!IS_SPACE(*p)) {
            (void) fprintf(stderr, "%s is incorrect - continuation line must begin with white-space.\n",
			   make_setup);
            (void) fprintf(stderr, "culprit line: %s\n", line_buf);
            exit(1);
         }
      }
      else {
         /* no line_continue character */
         done = 1;
      }
   }
   *data_out = '\0'; 

   if (maxdata == 0) {
      (void) fprintf(stderr, "input line too long in make.setup, exiting.\n");
      exit(1);
   }
} /* MM_get_setup_info */

/*
 ***********************************************************************
 *
 *   Synopsis:
 *	void	MMget_makesetup()
 *
 *   Description:
 *	Read the make{prog,lib}.setup file, and init setup with
 *	the information found there.
 *
 ***********************************************************************
 */
static
void
MMget_makesetup()
{
   char line_buf[MM_LINE_LENGTH];
   FILE *setup_file;
   char dummy;
   int  done = 0;

   /* open the make_setup file for this program or library */
   setup_file = fopen(make_setup, "r");
   if (!setup_file) {
      (void) fprintf(stderr, "Cannot open %s, exiting.\n", make_setup);
      exit (1);
   }

   /* init strings to NULL */
   setup->version[0] = '\0';
   setup->link_libs[0] = '\0';
   setup->develop_libs[0] = '\0';
   setup->defines[0] = '\0';
   setup->extra_src[0] = '\0';

   while (fgets(line_buf, MM_LINE_LENGTH, setup_file) && !done) {
      if (line_buf[0] != '#') {
         if (sscanf(line_buf, "_VERSION %c", &dummy))
            MM_get_setup_info(setup_file, setup->version, line_buf, MM_LINE_LENGTH);

         else if (sscanf(line_buf, "_LINK_LIBS %c", &dummy))
            MM_get_setup_info(setup_file, setup->link_libs, line_buf, MM_LINE_LENGTH);

         else if (sscanf(line_buf, "_DEVELOP_LIBS %c", &dummy))
            MM_get_setup_info(setup_file, setup->develop_libs, line_buf, MM_LINE_LENGTH);

         else if (sscanf(line_buf, "_DEFINES %c", &dummy))
            MM_get_setup_info(setup_file, setup->defines, line_buf, MM_LINE_LENGTH);

         else if (sscanf(line_buf, "_EXTRA_SRCDIRS %c", &dummy))
            MM_get_setup_info(setup_file, setup->extra_src, line_buf, MM_LINE_LENGTH);
      }
      else if (!strcmp(line_buf, "# ---- put any custom make rules you want after this line ---- #"))
	 done = 1; /* we'll get these lines later */
   }

   (void) fclose(setup_file);
} /* MMget_makesetup() */

/*
 ***********************************************************************
 *
 *   Synopsis:
 *	void	MMwrite_special_make_rules()
 *
 *   Description:
 *	Read the make.setup file, and grab any special rules at the
 *	bottom of the file and put them in the Makefile.
 *
 ***********************************************************************
 */
static
void
MMwrite_special_make_rules(FILE *make_file)
{
   char line_buf[MM_LINE_LENGTH];
   FILE *setup_file;
   int  found = 0;
   char *delimeter = "# ---- put any custom make rules you want after this line ---- #";
   int  size = strlen(delimeter);

   /* open the make_setup file for this program or library */
   setup_file = fopen(make_setup, "r");
   if (!setup_file) {
      (void) fprintf(stderr, "Cannot open %s, exiting.\n", make_setup);
      exit (1);
   }

   /* look for the delimiter */
   while (fgets(line_buf, MM_LINE_LENGTH, setup_file) && !found) {
      if (strncmp(line_buf, delimeter, size) == 0)
	 found = 1; /* we'll get these lines later */
   }

   /* if found the delimeter, start copying lines from make.setup to Makefile */
   if (found == 1) {
       while (fgets(line_buf, MM_LINE_LENGTH, setup_file)) {
	  fputs(line_buf, make_file);
       }
   }

   (void) fclose(setup_file);
} /* MMget_makesetup() */

/*
 ***********************************************************************
 *
 *   Synopsis:
 *	void	MMgen_link(FILE *f, char *link_list, char *dev_list)
 *
 *   Description:
 *	This generates the link libraries for a program Makefile.
 *   Files in dev_list create links with libraries under development.
 *   All other libraries in link_list are assumed to be in standard
 *   places.
 *
 ***********************************************************************
 */
static
int
MMgen_link(FILE *f, char *link_list, char *dev_list)
{
    int st = 0; /* assume success */
    char linklibs[MM_LIST_LENGTH];
    char devlibs[MM_LIST_LENGTH];

    MM_setup_flags(link_list, "-l", " ", linklibs);
    (void) fprintf(f, "#\n");
    (void) fprintf(f, "# libraries to link with\n");
    (void) fprintf(f, "LLDLIBS   = %s\n", linklibs);
    (void) fprintf(f, "\n");

    MM_setup_flags(dev_list, "-L$(SO_DEVLIB)/", "/obj/$(VERSION) ", devlibs);
    (void) fprintf(f, "#\n");
    (void) fprintf(f, "# directories to search in\n");
    (void) fprintf(f, "LINKDIRS  = %s -L$(SO_LIB)/$(VERSION) -L$(SO_LIB)\n", devlibs);

    return (st);
}

static void
MMgen_toplevel_makefile(char *srcdirs, char *incldirs)
{
    FILE *f;
    short is_lib;
    
    is_lib = (env->type == MM_LIB);

    f = fopen(makefile, "w");
    if (f) {
	if (is_lib)
	    MMwrite_libMakefile(f, env->name, srcdirs, incldirs);
	else {
	    if (cplusplus_code)
	         MMwrite_appsMakefile(f, env->name, srcdirs, "$(C++)");
	    else MMwrite_appsMakefile(f, env->name, srcdirs, "$(CC)");
	}

	/* program Makefiles need a link statement */
	MMgen_link(f, setup->link_libs, setup->develop_libs);

	fclose(f);
        (void) fprintf(stderr, "%s created\n", makefile);
    }
    else (void) fprintf(stderr, "trouble with %s\n", makefile);

}


/*
**	set cplusplus to 1 for LC++INCS
**	0 gives LCINCS.
*/
static
void
MMgen_lcincs(FILE *f, short cplusplus, short is_lib)
{
    char lcincs[MM_LIST_LENGTH];

    if (cplusplus) (void) fprintf(f, "LC++INCS = \\\n");
    else           (void) fprintf(f, "LCINCS = \\\n");
    (void) fprintf(f, "\t-I. \\\n");

    if (is_lib) {
        (void) fprintf(f, "\t-I$(DEPTH)../lib/database/include \\\n");
        (void) fprintf(f, "\t-I$(DEPTH)../lib/interaction/include \\\n");
        (void) fprintf(f, "\t-I$(DEPTH)../lib/nodekits/include \\\n");
        (void) fprintf(f, "\t-I$(DEPTH)../libSoXt/include \\\n");
        (void) fprintf(f, "\t-I$(DEPTH)../libSoGL/include\n");
    }
    (void) fprintf(f, "\n\n");
}

static
void
MMgen_defines(FILE *f, char *define_list)
{
    char defines[MM_LIST_LENGTH];

    MM_setup_flags(define_list, "-D", " ", defines);
    (void) fprintf(f, "# defines are specified in make.setup\n");
    (void) fprintf(f, "DEFINES = %s\n", defines);
    (void) fprintf(f, "\n");
}


static void
get_subdirname(char *dir, char *subdir)
{
    char *p;

    /* simply return the name (without the path) of dir */
    /* e.g., for /usr/src/lib/Inventor/so/src/rw, return rw */
    p = dir + strlen(dir);
    while (*p != '/')
	p--;

    p++; /* back over the last '/' we found */
    strcpy(subdir, p);
}

/*
 ***********************************************************************
 *
 *   Synopsis:
 *	int	MMgen_lex_and_yacc_files(FILE *f, char *srcdir)
 *
 *   Description:
 *	This generates lines for lex and yacc operations on .l and .y
 *   files. This returns 1 if there are lex and yacc files, 0 otherwise.
 *
 ***********************************************************************
 */
static
int
MMgen_lex_and_yacc_files(FILE *f, char *srcdir)
{
    char lfile[64], yfile[64], subdirname[64];
    char yyname[32];
    int  lfile_exists, yfile_exists;

    lfile_exists = MMget_lfile(srcdir, lfile);
    yfile_exists = MMget_yfile(srcdir, yfile);

    if (lfile_exists && yfile_exists) {
	get_subdirname(srcdir, subdirname);
        fprintf(f, "GENC_OBJECTS = $(TARGETDIR)/%s.o\n\n", yfile);

	/* mott begin 10/11/90 added yyname */
	/*
	** instead of producing files lex.yy.c, we want to produce
	** files with a prefix on it. If we are in a subdir, include
	** the subdir name in the prefix. For example: lex.qmso_yy.c,
	** and lex.so__rw_yy_.c
	*/
	if (subdirname[0] != '\0')
	     sprintf(yyname, "%s__%s_yy", env->name, subdirname);
	else sprintf(yyname, "%s__yy", env->name);

	fprintf(f, "# the lex and yacc .c files are in $(TARGETDIR)\n");
	fprintf(f, "$(TARGETDIR)/%s.o: $(TARGETDIR)/lex.%s.c $(TARGETDIR)/%s.c\n",
		yfile, yyname, yfile);
	fprintf(f, "\t$(CC) -c $(CFLAGS) $(TARGETDIR)/%s.c -o $(TARGETDIR)/%s.o\n\n",
		yfile, yfile);

	fprintf(f, "# lex generates lex.yy.c, we call it lex.%s.c\n", yyname);
	fprintf(f, "$(TARGETDIR)/lex.%s.c: %s.l\n", yyname, lfile);
        fprintf(f, "\t@echo Generating $@ due to $?\n");
        fprintf(f, "\tlex $?\n");
        fprintf(f, "\tsed -e \"s?yy?%s?g\" lex.yy.c > $@\n", yyname);
        fprintf(f, "\trm -f lex.yy.c\n\n");

	fprintf(f, "# yacc generates y.tab.c\n");
	fprintf(f, "$(TARGETDIR)/%s.c: %s.y\n", yfile, yfile);
        fprintf(f, "\t@echo Generating $@ due to $?\n");
        fprintf(f, "\tyacc $?\n");
        fprintf(f, "\tsed -e \"s?yy?%s?g\" y.tab.c > $@\n", yyname);
        fprintf(f, "\trm -f y.tab.c\n\n");
	/* mott end */
    }
    else if (lfile_exists)
	fprintf(stderr, "ERROR? %s exists, but there are no .y files!\n", lfile);
    else if (yfile_exists)
	fprintf(stderr, "ERROR? %s exists, but there are no .l files!\n", yfile);
}

/*
 ***********************************************************************
 *
 *   Synopsis:
 *	void	compute_depth(char *srcdir, char *depth)
 *
 *   Description:
 *	This sets depth to "../../"    if srcdir is "src",
 *		        to "../../../" if srcdir is "src/xxx", etc.  
 *
 ***********************************************************************
 */
static
void
compute_depth(char *srcdir, char *depth)
{
    int  i = 0;
    char *c;

    depth[i++] = '.'; depth[i++] = '.'; depth[i++] = '/';
    depth[i++] = '.'; depth[i++] = '.'; depth[i++] = '/';
    for ( c=srcdir; *c; c++ ) {
	if (*c == '/') {
	    depth[i++] = '.'; depth[i++] = '.'; depth[i++] = '/';
	}
    }
    depth[i] = '\0';
}


/* the new version! */
/* srcdir is the directory name relative to lib/xxx */
/* realdir is the directory relative to where we are now */
void
MMgen_src_makefile(char *srcdir, char *realdir, char *src_makefile)
{
    char depth[64];
    int  num_c, num_cpp; /* number of h, c, c++ files found */
    FILE *f;
    short is_lib;
    int  genc_objects;
    
    is_lib = (env->type == MM_LIB);

    /* compute depth based on srcdir */
    compute_depth(srcdir, depth);

    f = fopen(src_makefile, "w");
    if (f != NULL) {
	if (is_lib)
	     MMwrite_libSrcMakefile(f, env->name, srcdir, depth);
	else MMwrite_appsSrcMakefile(f, env->name, srcdir, depth);

	num_c = MMgen_cfiles(f, realdir);
	num_cpp = MMgen_cplusplusfiles(f, realdir);
	genc_objects = MMgen_lex_and_yacc_files(f, realdir);
	if (num_c > 0)   MMgen_lcincs(f, 0, is_lib); /*LCINCS*/
	if (num_cpp > 0) MMgen_lcincs(f, 1, is_lib); /*LC++INCS*/

	if (is_lib) {
	    (void) fprintf(f, "default: $(SOLIB)\n");
	    (void) fprintf(f, "include $(SO_COMMONRULES)\n");
	}
	else {
	    (void) fprintf(f, "do_all: $(OBJECTS) ");
	    if (genc_objects)
		(void) fprintf(f, "$(GENC_OBJECTS)\n");
	    else (void) fprintf(f, "\n");
	}

        (void) fprintf(stderr, "%s created\n", src_makefile);
	fclose(f);
    }
    else {
        (void) fprintf(stderr, "trouble with %s\n", src_makefile);

	/* still need to check whether we are dealing with C or C++ */
	num_cpp = MMcount_cplusplusfiles(srcdir);
    }

    if (num_cpp > 0) cplusplus_code = 1;
}

void
MMgen_include_makefile(char *filename, char *dir)
{
    char depth[64];
    FILE *f;
    
    /* compute depth based on srcdir */
    compute_depth(env->includeDir, depth);

    f = fopen(filename, "w");
    if (f != NULL) {
	MMwrite_includeMakefile(f, env->name, env->includeDir, depth);

	MMgen_hfiles(f, dir);
	MMgen_lcincs(f, 1, 1);

	(void) fprintf(f, "include $(SO_INCLCOMMONRULES)\n");

        (void) fprintf(stderr, "%s created\n", filename);
	fclose(f);
    }
    else {
        (void) fprintf(stderr, "trouble with %s\n", filename);
    }
}

void
MMgen_srclevel_makefiles(char *srcdir)
{
    char src_makefile[MM_PATHNAME_LENGTH];

    (void) sprintf(src_makefile, "%s/%s", srcdir, makefile);
    if (strncmp(srcdir, "src", 3) == 0) {
	MMgen_src_makefile(srcdir, srcdir, src_makefile);
    } else {
	sprintf(env->includeDir, srcdir);
	MMgen_include_makefile(src_makefile, srcdir);
    }
}

/*
 ***********************************************************************
 *
 *   Synopsis:
 *	int	main()
 *
 ***********************************************************************
 */
 

main(int argc, char **argv)
{
    MMenviron _env;
    MMsetup   _setup;
    char      srcdirs[MM_LIST_LENGTH];
    char      incldirs[MM_LIST_LENGTH];
    int       please_makedepend = 1, isTopLevel;

    fprintf(stderr, "\n\nThis is %s for the 3.0 tree!!!\n\n", argv[0]);

    /* check usage: makemake [-d] */
    if (argc > 1) {
	if (strcmp(argv[1], "-d") == 0)
	    please_makedepend = 0;
	else {
	    fprintf(stderr, "usage: %s [-d]\n", argv[0]);
	    fprintf(stderr, "where -d: do not make depend\n");
	    exit (-1);
	}
    }

    env = &_env;
    setup = &_setup;

    isTopLevel = MMget_environment(srcdirs);
    if (isTopLevel) {
	*srcdirs=0;
	*incldirs=0;
        MM_foreach_dir(MMgen_srclevel_makefiles, srcdirs, "src");
	MM_foreach_dir(MMgen_srclevel_makefiles, incldirs, "include/Inventor");
        MMgen_toplevel_makefile(srcdirs, incldirs);
    } else {
	/* only do this directory! */
	if (*srcdirs) {
	    MMgen_src_makefile(srcdirs, "./", makefile);
	} else {
	    MMgen_include_makefile(makefile, "./");
	}
    }

    if (please_makedepend) {
	(void) fprintf(stdout, "\nmake depend\n");
	(void) system("make depend");
    }

    return (0);
} /* main() */

