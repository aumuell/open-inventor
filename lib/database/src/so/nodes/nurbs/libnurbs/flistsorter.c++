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
 * flistsorter.c++ - $Revision: 1.1.1.1 $
 * 	Derrick Burns - 1991
 */

#include "glimports.h"
#include "flistsorter.h"

FlistSorter::FlistSorter( void ) : Sorter( sizeof( REAL ) )
{
}

void
FlistSorter::qsort( REAL *p, int n )
{
    Sorter::qsort( (char *)p, n );
}

int
FlistSorter::qscmp( char *i, char *j )
{
    REAL f0 = *(REAL *)i;
    REAL f1 = *(REAL *)j;
    return (f0 < f1) ? -1 : 1;
}

void
FlistSorter::qsexc( char *i, char *j )
{
    REAL *f0 = (REAL *)i;
    REAL *f1 = (REAL *)j;
    REAL tmp = *f0;
    *f0 = *f1;
    *f1 = tmp;
}

void
FlistSorter::qstexc( char *i, char *j, char *k )
{
    REAL *f0 = (REAL *)i;
    REAL *f1 = (REAL *)j;
    REAL *f2 = (REAL *)k;
    REAL tmp = *f0;
    *f0 = *f2;
    *f2 = *f1;
    *f1 = tmp;
}
