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
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.3 $
 |
 |   Classes:
 |	SbTime
 |
 |   Author(s)		: Nick Thompson
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SbTime.h>
#include <time.h>

#ifdef _CRAY
extern "C" {
    int gettimeofday(struct timeval *tp,struct timezone *tzp);
}
#endif /* _CRAY */

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor that takes the number of seconds as a double
//
// Use: public

#ifndef __sgi
inline static int trunc(double x) { return int(x); }
#endif // !__sgi

SbTime::SbTime(double sec)
//
////////////////////////////////////////////////////////////////////////
{
    if (sec >= 0) {
	t.tv_sec = trunc(sec);
	t.tv_usec = (time_t) (0.5 + (sec - t.tv_sec) * 1000000.0);
    }
    else
	*this = -SbTime(-sec);
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    Returns the current time in an SbTime.
//
// Use: public

SbTime
SbTime::getTimeOfDay()
//
////////////////////////////////////////////////////////////////////////
{
    SbTime	tm;

    if (-1 == gettimeofday(&tm.t, NULL))
	perror("gettimeofday");

    return tm;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Sets to the current time.
//
// Use: public

void
SbTime::setToTimeOfDay()
//
////////////////////////////////////////////////////////////////////////
{
    if (-1 == gettimeofday(&t, NULL))
	perror("gettimeofday");
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Converts to a formatted string.  The format string supports the following:
//
//	%%			the '%' character
//	%D	 		total number of days
//    	%H	 		total number of hours 
//	%M			total number of minutes
//    	%S			total number of seconds
//	%I			total number of milliseconds
//	%U			total number of microseconds
//	%h	00-23		hours remaining after the days
//	%m	00-59		minutes remaining after the hours
//	%s	00-59		seconds remaining after the minutes
//	%i	000-999		milliseconds remaining after the seconds
//	%u	000000-999999	microseconds remaining after the seconds
//
// uppercase descriptors are formatted with a leading '-' for negative times
// lowercase descriptors are formatted fixed width with leading zeros
//
// Use: public

SbString
SbTime::format(const char *fmt) const
//
////////////////////////////////////////////////////////////////////////
{
    SbBool		negative;
    struct timeval	tv;

    // turn into sign-magnitude form
    if (t.tv_sec >= 0) {
	negative = 0;
	tv = t;
    }
    else {
	negative = 1;
	tv = (-(*this)).t;
    }

    // first calculate total durations
    const int32_t tday = tv.tv_sec / (60*60*24);
    const int32_t thour = tv.tv_sec / (60*60);
    const int32_t tmin = tv.tv_sec / 60;
    const int32_t tsec = tv.tv_sec;
    const int32_t tmilli = 1000*tv.tv_sec + tv.tv_usec / 1000;
    const int32_t tmicro = 1000000*tv.tv_sec + tv.tv_usec;

    // then calculate remaining durations
    const int32_t rhour = thour - 24*tday;
    const int32_t rmin = tmin - 60*thour;
    const int32_t rsec = tsec - 60*tmin;
    const int32_t rmilli = tmilli - 1000*tsec;
    const int32_t rmicro = tmicro - 1000000*tsec;
    
    char buf[200];
    char *s = buf;

    for (; *fmt; fmt++) {
	if (*fmt != '%')
	    *s++ = *fmt;
	else
	    switch(*++fmt) {
	      case 0:
		fmt--;	// trailing '%' in format string
		break;

	      case '%':
		*s++ = '%';	// "%%" in format string
		break;

	      case 'D':
		if (negative) *s++ = '-';
		s += sprintf(s, "%ld", tday);
		break;

	      case 'H':
		if (negative) *s++ = '-';
		s += sprintf(s, "%ld", thour);
		break;

	      case 'M':
		if (negative) *s++ = '-';
		s += sprintf(s, "%ld", tmin);
		break;

	      case 'S':
		if (negative) *s++ = '-';
		s += sprintf(s, "%ld", tsec);
		break;

	      case 'I':
		if (negative) *s++ = '-';
		s += sprintf(s, "%ld", tmilli);
		break;

	      case 'U':
		if (negative) *s++ = '-';
		s += sprintf(s, "%ld", tmicro);
		break;

	      case 'h':
		s += sprintf(s, "%.2ld", rhour);
		break;

	      case 'm':
		s += sprintf(s, "%.2ld", rmin);
		break;

	      case 's':
		s += sprintf(s, "%.2ld", rsec);
		break;

	      case 'i':
		s += sprintf(s, "%.3ld", rmilli);
		break;

	      case 'u':
		s += sprintf(s, "%.6ld", rmicro);
		break;

	      default:
		*s++ = '%';	// echo any bad '%?'
		*s++ = *fmt;	// specifier
	    }
	if (s-buf >= sizeof(buf)-7) // don't overshoot the buffer
	    break;
    }
    *s = 0;

    return buf;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Formats as an absolute date/time, using unix "strftime" mechanism.
//
// Use: public

SbString
SbTime::formatDate(const char *fmt) const
//
////////////////////////////////////////////////////////////////////////
{
    char buf[200];

#if (_MIPS_SZLONG == 64)
    int  seconds;
    seconds = (int) t.tv_sec;
    strftime(buf, sizeof(buf), fmt, localtime(&seconds));
#else
    strftime(buf, sizeof(buf), fmt, localtime(&t.tv_sec));
#endif

    return buf;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//
// Use: public

SbTime
operator +(const SbTime &t0, const SbTime &t1)
//
////////////////////////////////////////////////////////////////////////
{
    SbTime tm(t0.t.tv_sec + t1.t.tv_sec,
	      t0.t.tv_usec + t1.t.tv_usec);
    if (tm.t.tv_usec >= 1000000) {
	tm.t.tv_sec  += 1;
	tm.t.tv_usec -= 1000000;
    }
    return tm;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//
// Use: public

SbTime
operator -(const SbTime &t0, const SbTime &t1)
//
////////////////////////////////////////////////////////////////////////
{
    int32_t sec; 
    long    usec;					// System long

    sec =  t0.t.tv_sec - t1.t.tv_sec;
    usec = t0.t.tv_usec - t1.t.tv_usec;

    while (usec < 0 && sec > 0) {
	usec += 1000000;
	sec -= 1;
    }

    return SbTime(sec, usec);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//
// Use: public

SbTime
operator *(const SbTime &tm, double s)
//
////////////////////////////////////////////////////////////////////////
{
    return SbTime(tm.getValue() * s);
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//
// Use: public

SbTime
operator /(const SbTime &tm, double s)
//
////////////////////////////////////////////////////////////////////////
{
    return tm * (1.0 / s);
}

