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

inline static void
mem_cvt_float(float *t, float *f)
{
    float _tobuf;
    char *_to = (char *)&_tobuf;

    if (*f != 0) {
	_to[0] = ((char *)f)[3];
	_to[1] = ((char *)f)[2];
	_to[2] = ((char *)f)[1];
	_to[3] = ((char *)f)[0];
	*t = _tobuf;
    }
    else
        *t = *f;
}

void
mem_hton_float(float *t, float *f)
{
    mem_cvt_float(t, f);
}

void
mem_ntoh_float(float *t, float *f)
{
    mem_cvt_float(t, f);
}

inline static void
mem_cvt_double(double *t, double *f)
{
    double _tobuf;
    char *_to = (char *)&_tobuf;

    if (*f != 0) {
	_to[0] = ((char *)f)[7];
	_to[1] = ((char *)f)[6];
	_to[2] = ((char *)f)[5];
	_to[3] = ((char *)f)[4];
	_to[4] = ((char *)f)[3];
	_to[5] = ((char *)f)[2];
	_to[6] = ((char *)f)[1];
	_to[7] = ((char *)f)[0];
	*t = _tobuf;
    }
    else
	*t = *f;
}

void
mem_hton_double(double *t, double *f) 
{
    mem_cvt_double(t, f);
}

void
mem_ntoh_double(double *t, double *f)
{
    mem_cvt_double(t, f);
}
