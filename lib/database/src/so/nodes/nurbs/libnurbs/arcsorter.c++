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

#ifndef __gluarcsorter_c_
#define __gluarcsorter_c_
/*
 * arcsorter.c++ - $Revision: 1.1.1.1 $
 * 	Derrick Burns - 1991
 */

#include "glimports.h"
#include "arc.h"
#include "arcsorter.h"
#include "subdivider.h"

ArcSorter::ArcSorter(Subdivider &s) : Sorter( sizeof( Arc ** ) ), subdivider(s)
{
}

int
ArcSorter::qscmp( char *, char * )
{
    dprintf( "ArcSorter::qscmp: pure virtual called\n" );
    return 0;
}

void
ArcSorter::qsort( Arc **a, int n )
{
    Sorter::qsort( (void *) a, n );
}

void		
ArcSorter::qsexc( char *i, char *j )// i<-j, j<-i 
{
    Arc **jarc1 = (Arc **) i;
    Arc **jarc2 = (Arc **) j;
    Arc *tmp = *jarc1;
    *jarc1 = *jarc2;
    *jarc2 = tmp;
}	

void		
ArcSorter::qstexc( char *i, char *j, char *k )// i<-k, k<-j, j<-i
{
    Arc **jarc1 = (Arc **) i;
    Arc **jarc2 = (Arc **) j;
    Arc **jarc3 = (Arc **) k;
    Arc *tmp = *jarc1;
    *jarc1 = *jarc3;
    *jarc3 = *jarc2;
    *jarc2 = tmp;
}
  

ArcSdirSorter::ArcSdirSorter( Subdivider &s ) : ArcSorter(s)
{
}

int
ArcSdirSorter::qscmp( char *i, char *j )
{
    Arc *jarc1 = *(Arc **) i;
    Arc *jarc2 = *(Arc **) j;

    int v1 = (jarc1->getitail() ? 0 : (jarc1->pwlArc->npts - 1));
    int	v2 = (jarc2->getitail() ? 0 : (jarc2->pwlArc->npts - 1));

    REAL diff =  jarc1->pwlArc->pts[v1].param[1] -
	    	 jarc2->pwlArc->pts[v2].param[1];

    if( diff < 0.0)
	return -1;
    else if( diff > 0.0)
	return 1;
    else {
	if( v1 == 0 ) {
	    if( jarc2->tail()[0] < jarc1->tail()[0] ) {
	        return subdivider.ccwTurn_sl( jarc2, jarc1 ) ? 1 : -1;
	    } else {
	        return subdivider.ccwTurn_sr( jarc2, jarc1 ) ? -1 : 1;
	    }
	} else {
	    if( jarc2->head()[0] < jarc1->head()[0] ) {
	        return subdivider.ccwTurn_sl( jarc1, jarc2 ) ? -1 : 1;
	    } else {
	        return subdivider.ccwTurn_sr( jarc1, jarc2 ) ? 1 : -1;
	    }
	}
    }    
}

ArcTdirSorter::ArcTdirSorter( Subdivider &s ) : ArcSorter(s)
{
}

/*----------------------------------------------------------------------------
 * ArcTdirSorter::qscmp - 
  *		   compare two axis monotone arcs that are incident 
 *		   to the line T == compare_value. Determine which of the
 *		   two intersects that line with a LESSER S value.  If
 *		   jarc1 does, return 1.  If jarc2 does, return -1. 
 *----------------------------------------------------------------------------
 */
int
ArcTdirSorter::qscmp( char *i, char *j )
{
    Arc *jarc1 = *(Arc **) i;
    Arc *jarc2 = *(Arc **) j;

    int v1 = (jarc1->getitail() ? 0 : (jarc1->pwlArc->npts - 1));
    int	v2 = (jarc2->getitail() ? 0 : (jarc2->pwlArc->npts - 1));

    REAL diff =  jarc1->pwlArc->pts[v1].param[0] -
	         jarc2->pwlArc->pts[v2].param[0];
 
    if( diff < 0.0)
	return 1;
    else if( diff > 0.0)
	return -1;
    else {
	if( v1 == 0 ) {
	    if (jarc2->tail()[1] < jarc1->tail()[1]) {
	        return subdivider.ccwTurn_tl( jarc2, jarc1 ) ? 1 : -1;
	    } else {
	        return subdivider.ccwTurn_tr( jarc2, jarc1 ) ? -1 : 1;
	    }
	} else {
	    if( jarc2->head()[1] < jarc1->head()[1] )  {
	        return subdivider.ccwTurn_tl( jarc1, jarc2 ) ? -1 : 1;
	    } else {
	        return subdivider.ccwTurn_tr( jarc1, jarc2 ) ? 1 : -1;
	    }
	}
    }
}



#endif /* __gluarcsorter_c_ */
