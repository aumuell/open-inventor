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
 * isocurveevaluator.c++ - isocurve evaluator
 *
 * $Revision: 1.1.1.1 $
 */

#include "myassert.h"
#include "defines.h"
#include "isocurveeval.h"


const
int _SoNurbsIsocurveEvaluator::Comb[157] =
{
    1,
    1,
    1,2,
    1,3,
    1,4,6,
    1,5,10,
    1,6,15,20,
    1,7,21,35,
    1,8,28,56,70,
    1,9,36,84,126,
    1,10,45,120,210,252,
    1,11,55,165,330,462,
    1,12,66,220,495,792,924,
    1,13,78,286,715,1287,1716,
    1,14,91,364,1001,2002,3003,3432,
    1,15,105,455,1365,3003,5005,6435,
    1,16,120,560,1820,4368,8008,11440,12870,
    1,17,136,680,2380,6188,12376,19448,24310,
    1,18,153,816,3060,8568,18564,31824,43758,48620,
    1,19,171,969,3876,11628,27132,50388,75582,92378,
    1,20,190,1140,4845,15504,38760,77520,125970,167960,184756,
    1,21,210,1330,5985,20349,54264,116280,203490,293930,352716,
    1,22,231,1540,7315,26334,74613,170544,319770,497420,646646,705432,
    1,23,253,1771,8855,33649,100947,245157,490314,817190,1144066,1352078
};


/*
 * Ch(n,m)
 * retrieves the binomial coefficient (n)  from the above array.
 *                                    (m) 
 */
	
unsigned int
_SoNurbsIsocurveEvaluator::Ch( unsigned int n, unsigned int m )
{
    /*
     * given the n and m find the index into the Comb array
     */
    unsigned int index;
    if( n < 24 )  {
	if(m>(n>>1)) /*store 1/2 array since (n chs m)=(n chs n-m)*/  
		m = n - m;
	index = (n>>1) * ((n>>1) + 1) + m + (n&1) * ((n+1)>>1);
	return Comb[index];
    }  else  {  /*  n > table entries, use Pascal's triangle  */
	if( m==0 || m==n )  {
		return 1;
	}  else  {
		return (Ch(n-1,m-1) + Ch(n-1,m));
	}
    }
}

/*---------------------------------------------------------------------------
 * eval_init - initialize evaluation parameters
 *---------------------------------------------------------------------------
 */

void
_SoNurbsIsocurveEvaluator::init( int order, int stride, REAL param, REAL lo, REAL hi )
{
#ifdef DEBUG
    assert( order < MAXORDER );
#endif
    Evaluation_order  = (int) order;
    Evaluation_stride = stride;
    REAL range = 1.0 / ( hi - lo);
    Evaluation_param  = (param - lo) * range;
    Evaluation_nparam = (hi - param) * range;
    Evaluation_order1 = (double) (order-1) * range;
    Evaluation_order2 = (double) ((order-1) * (order-2) * range * range); 
}

/*---------------------------------------------------------------------------
 * eval_row - evaluate a row of control points
 *---------------------------------------------------------------------------
 */

#ifndef HORNER
void
_SoNurbsIsocurveEvaluator::evaluate( REAL *ptr, REAL *p, REAL *dp, REAL *ddp )
{
    double		tmp[MAXORDER];
    int 		order = Evaluation_order;
    double 		order1 = Evaluation_order1;
    double 		order2 = Evaluation_order2;
    register int	stride = Evaluation_stride;
    register double	param = Evaluation_param;
    register double	nparam = Evaluation_nparam;

    if( p == 0 && dp == 0 && ddp == 0 ) return;

    if( p ) *p = 0.0;
    if( dp ) *dp = 0.0;
    if( ddp ) *ddp = 0.0;

    { 

	register int 	offset = (order-1) * stride;
	switch( order ) {
	    case 8: tmp[7] = ptr[offset]; offset -= stride;
	    case 7: tmp[6] = ptr[offset]; offset -= stride;
	    case 6: tmp[5] = ptr[offset]; offset -= stride;
	    case 5: tmp[4] = ptr[offset]; offset -= stride;
	    case 4: tmp[3] = ptr[offset]; offset -= stride;
	    case 3: tmp[2] = ptr[offset];
	    case 2: tmp[1] = ptr[stride];
	    case 1: tmp[0] = ptr[0]; 
		    break;
	    default: {
		for( register int j=0; j<order; j++ )
		    tmp[j] = ptr[stride * j];
	    }
	}
    }

    if( order > 8 ) {
	for( register int i=order-1; i>=8; i-- )
	    for( register int j=0; j<i; j++) 
        	tmp[j] = tmp[j]   * nparam + tmp[j+1] * param;
    }	

    register double a, b, c, d, e, f, g, h;

    switch( order ) {
        case 8: h = tmp[7];
        case 7: g = tmp[6];
        case 6: f = tmp[5];
        case 5: e = tmp[4];
        case 4: d = tmp[3];
        case 3: c = tmp[2];
        case 2: b = tmp[1];
        case 1: a = tmp[0];
    }

    switch( order ) {
    case 8:
        a = a * nparam + b * param;
        b = b * nparam + c * param;
        c = c * nparam + d * param;
        d = d * nparam + e * param;
        e = e * nparam + f * param;
        f = f * nparam + g * param;
        g = g * nparam + h * param;
    case 7:
        a = a * nparam + b * param;
        b = b * nparam + c * param;
        c = c * nparam + d * param;
        d = d * nparam + e * param;
        e = e * nparam + f * param;
        f = f * nparam + g * param;
    case 6:
        a = a * nparam + b * param;
        b = b * nparam + c * param;
        c = c * nparam + d * param;
        d = d * nparam + e * param;
        e = e * nparam + f * param;
    case 5:
        a = a * nparam + b * param;
        b = b * nparam + c * param;
        c = c * nparam + d * param;
        d = d * nparam + e * param;
    case 4:
        a = a * nparam + b * param;
        b = b * nparam + c * param;
        c = c * nparam + d * param;
    case 3:
	if( ddp ) *ddp = ((c-b)-(b-a)) * order2;
        a = a * nparam + b * param;
        b = b * nparam + c * param;
    case 2:
	if( dp ) *dp = (b-a) * order1;
	a = a * nparam + b * param;
    case 1:
	if( p ) *p = a;
        break;
    }
}

#else

void
_SoNurbsIsocurveEvaluator::evaluate( REAL *ptr, REAL *p, REAL *dp, REAL *ddp )
{
    int i,j;
    int str = Evaluation_stride;
    int ord = Evaluation_order;
    int deg = ord - 1;
    REAL t = Evaluation_param;
    REAL tc = Evaluation_nparam;
    REAL t_pow = 1.;
    REAL res;

    if( p == 0 && dp == 0 && ddp == 0 ) return;
    if( p ) {
	unsigned int l;
	res = ptr[0] * tc;
	for(i = 1, l = str; i < deg; i++, l+= str) {
	    t_pow *= t;
	    res = (res + t_pow * ptr[l]) * tc;
	}
	res += t * t_pow * ptr[l];
	*p = res;
    }

    if( dp ) {
	unsigned int l;
	REAL fdeg = deg;
	REAL fip; /* i + 1 */
	REAL fi; 
	t_pow = 1.;
	res = (ptr[str] - fdeg*ptr[0]) * tc;
	for(i = 1,l=str,fi=1.,fip=2.; i < deg-1; i++,l+=str,fi+=1,fip+=1.) {
	    t_pow *= t;
	    res = (res + t_pow*(fip*ptr[l+str]-(fdeg-fi)*ptr[l]))*tc;
	}
	res += t * t_pow * (fdeg *ptr[l+str]-ptr[l]);
	*dp = res;
    }
}

#endif /* HORNER */
