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

#ifndef __MACHINE_H__
#define __MACHINE_H__

/*
 * Copyright (C) 1990,91,92,93   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.5 $
 |
 |   Description:
 |	This include file contains the machine-dependent macros
 |   and defines for converting basic datatypes across machines.
 |   This is only used by the SoInput and SoOutput classes defined
 |   in the Inventor lib/database source.
 |       TO ADD A NEW MACHINE:
 |
 |		1. Add a machine independent setup section
 |		   (define MACHINE_WORD_FORMAT and MACHINE_FLOAT_FORMAT)
 |
 |		2. Define DGL_HTON_*, DGL_NTOH_*, and SHORT/INT32/FLOAT/DOUBLE
 |
 |       IMPORTANT NOTE: The only complete examples found in this
 |   file are the SGI and Cray implementations.  Note that the other
 |   implementations are incomplete!
 |
 |   Author(s)		: Dave Immel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */


/*
 * SGI machine dependent setup
 */

#if sgi

#define MACHINE_WORD_FORMAT	DGL_BIG_ENDIAN
#define MACHINE_FLOAT_FORMAT	DGL_BIG_IEEE
#if m68000			/* on 3K, times() returns 60'ths regardless */
#undef HZ
#define HZ 60
#define double long float    /* System long, 
				make m68020 behave like everyone */
#endif /* m68000 */

#endif /* sgi */


/*
 * CRAY machine dependent setup
 */

#ifdef _CRAY

#define MACHINE_WORD_FORMAT	DGL_BIG_ENDIAN
#define MACHINE_FLOAT_FORMAT	DGL_BIG_IEEE
#define signed			/* CRAY default is signed chars	*/
#ifndef __STDC__
#include <use_stdc.h>		/* Make sure we use the ANSI C compiler. */
#endif

#endif /* _CRAY */


/*
 * SUN/BSD machine dependent setup
 */

#if sun

#define MACHINE_WORD_FORMAT	DGL_BIG_ENDIAN
#define MACHINE_FLOAT_FORMAT	DGL_BIG_IEEE
#define signed			/* SUN default is signed chars		*/
#define h_errno errno		/* BSD has these merged???		*/
typedef unsigned long ulong;	/* System long missing from types.h	*/

#endif /* sun */


/*
 * VAX VMS/BSD machine dependent setup
 */

#if vax

#define MACHINE_WORD_FORMAT	DGL_LITTLE_ENDIAN
#define MACHINE_FLOAT_FORMAT	DGL_NON_IEEE
#define signed			/* VAX default is signed chars		*/
#if vms				/* VMS only statements			*/
#define	HZ	100		/* missing from param.h			*/
#define dn_perror dgl_perror	/* use normal error handler		*/
#define fork vfork		/* just for now - doesn't really work	*/
#else				/* VAX/BSD only statements		*/
#define HZ 60			/* missing from param.h			*/
typedef unsigned long ulong;	/* System long missing from types.h	*/
extern int errno;		/* missing from errno.h			*/
#endif /* vms */

#endif /* vax */


/*
 * Linux i386/ia64 machine dependent setup
 */

#if __i386__ || __ia64__

#define MACHINE_WORD_FORMAT	DGL_LITTLE_ENDIAN
#define MACHINE_FLOAT_FORMAT	DGL_NON_IEEE

#endif /* __i386__ || __ia64__ */


/*
 * Apple Darwin (Mac OS X) machine dependent setup
 */

#ifdef __APPLE__

#define MACHINE_WORD_FORMAT	DGL_BIG_ENDIAN
#define MACHINE_FLOAT_FORMAT	DGL_BIG_IEEE

#endif /* __APPLE__ */


/*
 * IBM RS/6000 series machine dependent setup
 */

#ifdef _IBMR2

#define MACHINE_WORD_FORMAT	DGL_BIG_ENDIAN
#define MACHINE_FLOAT_FORMAT	DGL_BIG_IEEE
#undef DOUBLE	/* bogusly defined in <gai/g3dm2types.h> */

#endif



/*
 * 32/64-bit architecture dependent statements
 */

#ifdef _CRAY

#define size_char 1
#define size_short 2
#define size_int 4
#define size_long 4		/* System long */
#define size_float 4
#define size_double 8
#define size_Fontchar 8
#ifdef __STDC__
#define M_SIZEOF(x) size_##x
#else
#define M_SIZEOF(x) size_/**/x
#endif /*__STDC__*/

#else /* SGI and not CRAY */

#define M_SIZEOF(x) sizeof(x)

#endif /* ifdef _CRAY */



/*
 * Defines for the various data formats
 */

#define DGL_LITTLE_ENDIAN 1		/* integer formats		*/
#define DGL_BIG_ENDIAN 2

#define DGL_BIG_IEEE 1			/* floating point formats	*/
#define DGL_NON_IEEE 3


/*
 * Data conversion (byte swapping) algorithms:
 *	HTON - client host to network (server)
 *	NTOH - network (server) to client host
 */


/*
 * DGL_BIG_ENDIAN: no conversion necessary (INTEGER)
 */

#ifdef	_CRAY

#ifdef __cplusplus
extern "C" {
	void DGL_HTON_SHORT(char *, short);
	void DGL_HTON_LONG(char *, long);	/* System long */
	long dgl_ntoh_long(char *);		/* System long */
	short dgl_ntoh_short(char *);
};
#else
extern long dgl_ntoh_long();			/* System long */
extern short dgl_ntoh_short();
#endif /* __cplusplus_ */
#define DGL_NTOH_LONG(t,f)  t = dgl_ntoh_long(f)	/* System long */
#define DGL_NTOH_SHORT(t,f)  t = dgl_ntoh_short(f)

#else /* SGI or !_CRAY */

#if MACHINE_WORD_FORMAT == DGL_BIG_ENDIAN
#define DGL_HTON_SHORT(t,f) t = f
#define DGL_NTOH_SHORT DGL_HTON_SHORT
#define DGL_HTON_INT32(t,f) t = f
#define DGL_NTOH_INT32 DGL_HTON_INT32
#endif /* MACHINE_WORD_FORMAT */

#endif /* _CRAY */


/*
 * DGL_BIG_IEEE: no conversion necessary (FLOAT)
 */

#ifdef _CRAY

#ifdef __cplusplus
extern "C" {
    float dgl_ntoh_float(char *);
    float dgl_ntoh_double(char *);
};
#else
extern float dgl_ntoh_float();
extern float dgl_ntoh_double();
#endif /* __cplusplus */
#define DGL_NTOH_FLOAT(t,f)  t = dgl_ntoh_float(f)
#define DGL_NTOH_DOUBLE(t,f)  t = dgl_ntoh_double(f)

#else 	/* SGI or !_CRAY */

#if MACHINE_FLOAT_FORMAT == DGL_BIG_IEEE
#define DGL_HTON_FLOAT(t,f) t = f
#define DGL_NTOH_FLOAT DGL_HTON_FLOAT
#define DGL_HTON_DOUBLE(t,f) t = f
#define DGL_NTOH_DOUBLE DGL_HTON_DOUBLE
#endif

#endif /* _CRAY */


/*
 * DGL_LITTLE_ENDIAN: conversion necessary (INTEGER)
 *	NOTE: non-floating point conversions are the same for both
 *		directions and thus one macro suffices
 */

#if MACHINE_WORD_FORMAT == DGL_LITTLE_ENDIAN

/* like DGL_HTON_INT32, but more efficient if f is a constant */
#define DGL_HTON_SHORT(t,f) 	\
	{			\
		short _from = f,_to;	\
		((char *)&_to)[0] = ((char *)&_from)[1];	\
		((char *)&_to)[1] = ((char *)&_from)[0];	\
		t = _to;	\
	}
#define DGL_NTOH_SHORT DGL_HTON_SHORT
#define DGL_HTON_INT32(t,f)	\
	{			\
		int32_t _from = f,_to;	\
		((char *)&_to)[0] = ((char *)&_from)[3];	\
		((char *)&_to)[1] = ((char *)&_from)[2];	\
		((char *)&_to)[2] = ((char *)&_from)[1];	\
		((char *)&_to)[3] = ((char *)&_from)[0];	\
		t = _to;	\
	}
#define DGL_NTOH_INT32 DGL_HTON_INT32

#endif /* LITTLE_ENDIAN */


/*
 * DGL_NON_IEEE: conversion necessary (FLOAT)
 *	conversion is done within procedure calls for simplicity
 */

#if MACHINE_FLOAT_FORMAT == DGL_NON_IEEE
#if __i386__ || __ia64__
void mem_hton_float(float *t, float *f);
void mem_ntoh_float(float *t, float *f);
void mem_hton_double(double *t, double *f);
void mem_ntoh_double(double *t, double *f);
#endif /* __i386__ || __ia64__ */
#define DGL_HTON_FLOAT(t,f) mem_hton_float(&t,&f)
#define DGL_NTOH_FLOAT(t,f) mem_ntoh_float(&t,&f)
#define DGL_HTON_DOUBLE(t,f) mem_hton_double(&t,&f)
#define DGL_NTOH_DOUBLE(t,f) mem_ntoh_double(&t,&f)
#endif


/*
 * get/set a data item located at address p regardless what it really is
 */

#ifdef _CRAY
#define LONG(p) (p)		/* System long */
#define FLOAT(p) (p)
#define DOUBLE(p) (p)
#define SHORT(p) (p)
#else /* !_CRAY */
#define INT32(p) (*(int32_t *)(p))
#define FLOAT(p) (*(float *)(p))
#define DOUBLE(p) (*(double *)(p))
#define SHORT(p) (*(short *)(p))
#endif

#endif /* __MACHINE_H__ */
