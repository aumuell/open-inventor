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
 * Copyright (C) 1990,   Silicon Graphics, Inc.
 *
 ***********************************************************************
 ***************  S I L I C O N   G R A P H I C S   I N C .  ***********
 *
 *   Header File Name	: makemake.h
 *
 *   Description:
 *	This is the include file for the makemake program. Makemake will
 *	automatically generate a Makefile for a program or library in
 *	Inventor.
 * 
 *   Author(s)		: David Mott
 *
 *   RCS Revision	: $Revision: 1.1.1.1 $
 *
 ***************  S I L I C O N   G R A P H I C S   I N C .  ***********
 ***********************************************************************
 */

#ifndef _MAKEMAKE_H_INCLUDE
#define _MAKEMAKE_H_INCLUDE


/* ------------------------------ Constants -------------------------------- */

#define MM_VERSION 1.0

#ifndef NULL
#define NULL	0
#endif
#ifndef FALSE
#define FALSE	0
#define TRUE	!FALSE
#endif

#define MM_PATHNAME_LENGTH	256
#define MM_NAME_LENGTH		32
#define MM_LIST_LENGTH		1024
#define MM_LINE_LENGTH		1024

#define MM_LIB		1
#define MM_BIN		2


/* ---------------------------------- Types -----------------------------------	*/

/*
**	MMenviron contains things concerning the name of this program or lib,
**	and any environment variables which affect the Makefile.
*/
typedef struct MMenviron { 
   char		name[MM_NAME_LENGTH];	/* the name of this program or lib	*/
   char		includeDir[MM_NAME_LENGTH];	/* the name of the include direcory */
   char		*so_root;	 	/* pts to the SO_ROOT env var		*/
   char		*machine; 		/* pts to the MACHINE env var		*/
   int		type;			/* set to MM_LIB or MM_BIN		*/
} MMenviron;

/*
**	MMsetup contains info from the make.setup file for this prog or lib.
*/
typedef struct MMsetup {
   char		version[MM_LIST_LENGTH];	/* debug, optimize, profile	*/
   char		link_libs[MM_LIST_LENGTH];	/* libs to link with		*/
   char		develop_libs[MM_LIST_LENGTH];	/* libs under development	*/
   char		defines[MM_LIST_LENGTH];	/* local defines 		*/
   char		extra_src[MM_LIST_LENGTH];	/* extra src directories	*/
} MMsetup;


/* --------------------------- Function declarations -------------------------- */

extern int	MMgen_cfiles(FILE *, char *);
extern int	MMgen_hfiles(FILE *, char *);
extern int	MMgen_cplusplusfiles(FILE *, char *);
extern int	MMcount_cplusplusfiles(char *);
extern int	MMget_yfile(char *, char *);
extern int	MMget_lfile(char *, char *);
extern void	MM_setup_flags(char *, char *, char *, char *);
extern void	MM_foreach_dir( void (*gen_func) () , char *, char *);
extern void	MMgen_srclevel_makefiles(char *);

extern MMenviron *env;	
extern MMsetup	 *setup;

#endif _MAKEMAKE_H_INCLUDE
