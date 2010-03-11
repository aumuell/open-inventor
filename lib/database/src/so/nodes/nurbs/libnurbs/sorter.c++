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
 * sorter.c++ - $Revision: 1.1.1.1 $
 * 	Derrick Burns - 1991
 */

#include "glimports.h"
#include "sorter.h"
#include "mystdio.h"

Sorter::Sorter( int _es )
{
    es = _es;
}

void
Sorter::qsort( void *a, int n )
{
    qs1( (char *)a, ((char *)a)+n*es);
}

int
Sorter::qscmp( char *, char * )
{
    dprintf( "Sorter::qscmp: pure virtual called\n" );
    return 0;
}


void
Sorter::qsexc( char *, char * )
{
    dprintf( "Sorter::qsexc: pure virtual called\n" );
}


void
Sorter::qstexc( char *, char *, char * )
{
    dprintf( "Sorter::qstexc: pure virtual called\n" );
}

void
Sorter::qs1( char *a,  char *l )
{
    char *i, *j;
    char	*lp, *hp;
    int	c;
    unsigned int n;

start:
    if((n=l-a) <= es)
	    return;
    n = es * (n / (2*es));
    hp = lp = a+n;
    i = a;
    j = l-es;
    while(1) {
	if(i < lp) {
	    if((c = qscmp(i, lp)) == 0) {
		qsexc(i, lp -= es);
		continue;
	    }
	    if(c < 0) {
		i += es;
		continue;
	    }
	}

loop:
	if(j > hp) {
	    if((c = qscmp(hp, j)) == 0) {
		qsexc(hp += es, j);
		goto loop;
	    }
	    if(c > 0) {
		if(i == lp) {
		    qstexc(i, hp += es, j);
		    i = lp += es;
		    goto loop;
		}
		qsexc(i, j);
		j -= es;
		i += es;
		continue;
	    }
	    j -= es;
	    goto loop;
	}

	if(i == lp) {
	    if(lp-a >= l-hp) {
		qs1(hp+es, l);
		l = lp;
	    } else {
		qs1(a, lp);
		a = hp+es;
	    }
	    goto start;
	}

	qstexc(j, lp -= es, i);
	j = hp -= es;
    }
}

