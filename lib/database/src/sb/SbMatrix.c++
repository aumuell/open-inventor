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
 |   $Revision: 1.1 $
 |
 |   Classes:
 |	SbMatrix
 |
 |   Author(s)		: Nick Thompson, David Mott, Howard Look
 |
 |   Notes:
 |	Matrices are stored in row-major order.
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <Inventor/SbLinear.h>

// amount squared to figure if two floats are equal
// (used for operator == right now)
#define DELTA 1e-6

//
// Handy absolute value macro:
//

#define ABS(a)		((a) < 0.0 ? -(a) : (a))


//
// Macro for checking is a matrix is idenity.
//

#define IS_IDENTITY(matrix) ( \
    (matrix[0][0] == 1.0) && \
    (matrix[0][1] == 0.0) && \
    (matrix[0][2] == 0.0) && \
    (matrix[0][3] == 0.0) && \
    (matrix[1][0] == 0.0) && \
    (matrix[1][1] == 1.0) && \
    (matrix[1][2] == 0.0) && \
    (matrix[1][3] == 0.0) && \
    (matrix[2][0] == 0.0) && \
    (matrix[2][1] == 0.0) && \
    (matrix[2][2] == 1.0) && \
    (matrix[2][3] == 0.0) && \
    (matrix[3][0] == 0.0) && \
    (matrix[3][1] == 0.0) && \
    (matrix[3][2] == 0.0) && \
    (matrix[3][3] == 1.0))


//
// Constructor given all 16 elements in row-major order
//

SbMatrix::SbMatrix(float a11, float a12, float a13, float a14,
		   float a21, float a22, float a23, float a24, 
		   float a31, float a32, float a33, float a34, 
		   float a41, float a42, float a43, float a44)
{
    matrix[0][0] = a11;
    matrix[0][1] = a12;
    matrix[0][2] = a13;
    matrix[0][3] = a14;

    matrix[1][0] = a21;
    matrix[1][1] = a22;
    matrix[1][2] = a23;
    matrix[1][3] = a24;

    matrix[2][0] = a31;
    matrix[2][1] = a32;
    matrix[2][2] = a33;
    matrix[2][3] = a34;

    matrix[3][0] = a41;
    matrix[3][1] = a42;
    matrix[3][2] = a43;
    matrix[3][3] = a44;
}

//
// Constructor from a 4x4 array of elements
//

SbMatrix::SbMatrix(const SbMat &m)
{
    matrix[0][0] = m[0][0];
    matrix[0][1] = m[0][1];
    matrix[0][2] = m[0][2];
    matrix[0][3] = m[0][3];
    matrix[1][0] = m[1][0];
    matrix[1][1] = m[1][1];
    matrix[1][2] = m[1][2];
    matrix[1][3] = m[1][3];
    matrix[2][0] = m[2][0];
    matrix[2][1] = m[2][1];
    matrix[2][2] = m[2][2];
    matrix[2][3] = m[2][3];
    matrix[3][0] = m[3][0];
    matrix[3][1] = m[3][1];
    matrix[3][2] = m[3][2];
    matrix[3][3] = m[3][3];
}

////////////////////////////////////////////
//
// Setting matrix values
//
////////////////////////////////////////////

//
// Sets value from 4x4 array of elements
//

void
SbMatrix::setValue(const SbMat &m)
{
    matrix[0][0] = m[0][0];
    matrix[0][1] = m[0][1];
    matrix[0][2] = m[0][2];
    matrix[0][3] = m[0][3];
    matrix[1][0] = m[1][0];
    matrix[1][1] = m[1][1];
    matrix[1][2] = m[1][2];
    matrix[1][3] = m[1][3];
    matrix[2][0] = m[2][0];
    matrix[2][1] = m[2][1];
    matrix[2][2] = m[2][2];
    matrix[2][3] = m[2][3];
    matrix[3][0] = m[3][0];
    matrix[3][1] = m[3][1];
    matrix[3][2] = m[3][2];
    matrix[3][3] = m[3][3];
}

//
// Return an identity matrix
//

SbMatrix
SbMatrix::identity()    
{
    return SbMatrix(1.0, 0.0, 0.0, 0.0,
		    0.0, 1.0, 0.0, 0.0,
		    0.0, 0.0, 1.0, 0.0, 
		    0.0, 0.0, 0.0, 1.0);
}

//
// Sets matrix to be identity
//

void
SbMatrix::makeIdentity()
{
    matrix[0][0] = 1.0;
    matrix[0][1] = 0.0;
    matrix[0][2] = 0.0;
    matrix[0][3] = 0.0;
    matrix[1][0] = 0.0;
    matrix[1][1] = 1.0;
    matrix[1][2] = 0.0;
    matrix[1][3] = 0.0;
    matrix[2][0] = 0.0;
    matrix[2][1] = 0.0;
    matrix[2][2] = 1.0;
    matrix[2][3] = 0.0;
    matrix[3][0] = 0.0;
    matrix[3][1] = 0.0;
    matrix[3][2] = 0.0;
    matrix[3][3] = 1.0;
}

//
// Sets matrix to rotate by given rotation
//

void
SbMatrix::setRotate(const SbRotation &rotation)
{
    rotation.getValue(*this);
}

//
// Sets matrix to scale by given uniform factor
//

void
SbMatrix::setScale(float s)
{
    matrix[0][0] = s;
    matrix[0][1] = 0.0;
    matrix[0][2] = 0.0;
    matrix[0][3] = 0.0;
    matrix[1][0] = 0.0;
    matrix[1][1] = s;
    matrix[1][2] = 0.0;
    matrix[1][3] = 0.0;
    matrix[2][0] = 0.0;
    matrix[2][1] = 0.0;
    matrix[2][2] = s;
    matrix[2][3] = 0.0;
    matrix[3][0] = 0.0;
    matrix[3][1] = 0.0;
    matrix[3][2] = 0.0;
    matrix[3][3] = 1.0;
}

//
// Sets matrix to scale by given vector
//

void
SbMatrix::setScale(const SbVec3f &s)
{
    matrix[0][0] = s[0];
    matrix[0][1] = 0.0;
    matrix[0][2] = 0.0;
    matrix[0][3] = 0.0;
    matrix[1][0] = 0.0;
    matrix[1][1] = s[1];
    matrix[1][2] = 0.0;
    matrix[1][3] = 0.0;
    matrix[2][0] = 0.0;
    matrix[2][1] = 0.0;
    matrix[2][2] = s[2];
    matrix[2][3] = 0.0;
    matrix[3][0] = 0.0;
    matrix[3][1] = 0.0;
    matrix[3][2] = 0.0;
    matrix[3][3] = 1.0;
}

//
// Sets matrix to translate by given vector
//

void
SbMatrix::setTranslate(const SbVec3f &t)
{
    matrix[0][0] = 1.0;
    matrix[0][1] = 0.0;
    matrix[0][2] = 0.0;
    matrix[0][3] = 0.0;
    matrix[1][0] = 0.0;
    matrix[1][1] = 1.0;
    matrix[1][2] = 0.0;
    matrix[1][3] = 0.0;
    matrix[2][0] = 0.0;
    matrix[2][1] = 0.0;
    matrix[2][2] = 1.0;
    matrix[2][3] = 0.0;
    matrix[3][0] = t[0];
    matrix[3][1] = t[1];
    matrix[3][2] = t[2];
    matrix[3][3] = 1.0;
}

////////////////////////////////////////////
//
// Returning matrix values and other info
//
////////////////////////////////////////////

//
// Returns 4x4 array of elements
//

void
SbMatrix::getValue(SbMat &m) const
{
    m[0][0] = matrix[0][0];
    m[0][1] = matrix[0][1];
    m[0][2] = matrix[0][2];
    m[0][3] = matrix[0][3];
    m[1][0] = matrix[1][0];
    m[1][1] = matrix[1][1];
    m[1][2] = matrix[1][2];
    m[1][3] = matrix[1][3];
    m[2][0] = matrix[2][0];
    m[2][1] = matrix[2][1];
    m[2][2] = matrix[2][2];
    m[2][3] = matrix[2][3];
    m[3][0] = matrix[3][0];
    m[3][1] = matrix[3][1];
    m[3][2] = matrix[3][2];
    m[3][3] = matrix[3][3];
}

//
// Returns determinant of 3x3 submatrix composed of given row and
// column indices (0-3 for each).
//

float
SbMatrix::det3(int r1, int r2, int r3, int c1, int c2, int c3) const
{
    return (   matrix[r1][c1] * matrix[r2][c2] * matrix[r3][c3]
	    +  matrix[r1][c2] * matrix[r2][c3] * matrix[r3][c1]
	    +  matrix[r1][c3] * matrix[r2][c1] * matrix[r3][c2]
	    -  matrix[r1][c1] * matrix[r2][c3] * matrix[r3][c2]
	    -  matrix[r1][c2] * matrix[r2][c1] * matrix[r3][c3]
	    -  matrix[r1][c3] * matrix[r2][c2] * matrix[r3][c1]);
}

//
// Returns determinant of entire matrix
//

float
SbMatrix::det4() const
{
    return (   matrix[0][3] * det3(1, 2, 3, 0, 1, 2)
	    +  matrix[1][3] * det3(0, 2, 3, 0, 1, 2)
	    +  matrix[2][3] * det3(0, 1, 3, 0, 1, 2)
	    +  matrix[3][3] * det3(0, 1, 2, 0, 1, 2));
}

//
// Factors a matrix m into 5 pieces: m = r s r^ u t, where r^
// means transpose of r, and r and u are rotations, s is a scale,
// and t is a translation. Any projection information is returned
// in proj.
//
// routines for matrix factorization taken from BAGS code, written by
// John Hughes (?).  Original comment follows:
//
/* Copyright 1988, Brown Computer Graphics Group.  All Rights Reserved. */
/* --------------------------------------------------------------------------
 * This file contains routines to do the MAT3factor operation, which
 * factors a matrix m:
 *    m = r s r^ u t, where r^ means transpose of r, and r and u are
 * rotations, s is a scale, and t is a translation.
 *
 * It is based on the Jacobi method for diagonalizing real symmetric
 * matrices, taken from Linear Algebra, Wilkenson and Reinsch, Springer-Verlag
 * math series, Volume II, 1971, page 204.  Call number QA251W623.
 * In ALGOL!
 * -------------------------------------------------------------------------*/
/*
 * Variable declarations from the original source:
 *
 * n	: order of matrix A
 * eivec: true if eigenvectors are desired, false otherwise.
 * a	: Array [1:n, 1:n] of numbers, assumed symmetric!
 *
 * a	: Superdiagonal elements of the original array a are destroyed.
 *	  Diagonal and subdiagonal elements are untouched.
 * d	: Array [1:n] of eigenvalues of a.
 * v	: Array [1:n, 1:n] containing (if eivec = TRUE), the eigenvectors of
 *	  a, with the kth column being the normalized eigenvector with
 *	  eigenvalue d[k].
 * rot	: The number of jacobi rotations required to perform the operation.
 */
	       
SbBool
SbMatrix::factor(SbMatrix &r, SbVec3f &s, SbMatrix &u, SbVec3f &t,
		 SbMatrix &proj) const
{
    double	det;		/* Determinant of matrix A	*/
    double	det_sign;	/* -1 if det < 0, 1 if det > 0	*/
    double	scratch;
    int		i, j;
    int		junk;
    SbMatrix	a, b, si;
    float	evalues[3];
    SbVec3f	evectors[3];
    
    a = *this;
    proj.makeIdentity();
    scratch = 1.0;
    
    for (i = 0; i < 3; i++) {
	for (j = 0; j < 3; j++) {
	    a.matrix[i][j] *= scratch;
	}
	t[i] = matrix[3][i] * scratch;
	a.matrix[3][i] = a.matrix[i][3] = 0.0;
    }
    a.matrix[3][3] = 1.0;
    
    /* (3) Compute det A. If negative, set sign = -1, else sign = 1 */
    det = a.det3();
    det_sign = (det < 0.0 ? -1.0 : 1.0);
    if (det_sign * det < 1e-12)
	return(FALSE);		// singular
    
    /* (4) B = A * A^  (here A^ means A transpose) */
    b = a * a.transpose();
    
    b.jacobi3(evalues, evectors, junk);

    // find min / max eigenvalues and do ratio test to determine singularity
    
    r = SbMatrix(evectors[0][0], evectors[0][1], evectors[0][2], 0.0, 
		 evectors[1][0], evectors[1][1], evectors[1][2], 0.0, 
		 evectors[2][0], evectors[2][1], evectors[2][2], 0.0, 
		 0.0, 0.0, 0.0, 1.0);
    
    /* Compute s = sqrt(evalues), with sign. Set si = s-inverse */
    si.makeIdentity();
    for (i = 0; i < 3; i++) {
	s[i] = det_sign * sqrt(evalues[i]);
	si.matrix[i][i] = 1.0 / s[i];
    }
    
    /* (5) Compute U = R^ S! R A. */
    u = r * si * r.transpose() * a;
    
    return(TRUE);
}

//
// Diagonalizes 3x3 matrix. See comment for factor().
//

#define SB_JACOBI_RANK	3
void
SbMatrix::jacobi3(float evalues[SB_JACOBI_RANK],
		  SbVec3f evectors[SB_JACOBI_RANK], int &rots) const
{
    double	sm;		// smallest entry
    double	theta;		// angle for Jacobi rotation
    double	c, s, t;	// cosine, sine, tangent of theta
    double	tau;		// sine / (1 + cos)
    double	h, g;		// two scrap values
    double	thresh;		// threshold below which no rotation done
    double	b[SB_JACOBI_RANK]; // more scratch
    double	z[SB_JACOBI_RANK]; // more scratch
    int		p, q, i, j;
    double	a[SB_JACOBI_RANK][SB_JACOBI_RANK];
    
    // initializations
    for (i = 0; i < SB_JACOBI_RANK; i++) {
	b[i] = evalues[i] = matrix[i][i];
	z[i] = 0.0;
	for (j = 0; j < SB_JACOBI_RANK; j++) {
	    evectors[i][j] = (i == j) ? 1.0 : 0.0;
	    a[i][j] = matrix[i][j];
	}
    }
    
    rots = 0;

    // Why 50? I don't know--it's the way the folks who wrote the
    // algorithm did it:
    for (i = 0; i < 50; i++) {
	sm = 0.0;
	for (p = 0; p < SB_JACOBI_RANK - 1; p++)
	    for (q = p+1; q < SB_JACOBI_RANK; q++)
		sm += ABS(a[p][q]);
	
	if (sm == 0.0)
	    return;
	
	thresh = (i < 3 ?
		  (.2 * sm / (SB_JACOBI_RANK * SB_JACOBI_RANK)) :
		  0.0);
	
	for (p = 0; p < SB_JACOBI_RANK - 1; p++) {
	    for (q = p+1; q < SB_JACOBI_RANK; q++) {
		
		g = 100.0 * ABS(a[p][q]);
		
		if (i > 3 && (ABS(evalues[p]) + g == ABS(evalues[p])) &&
		    (ABS(evalues[q]) + g == ABS(evalues[q])))
		    a[p][q] = 0.0;
		
		else if (ABS(a[p][q]) > thresh) {
		    h = evalues[q] - evalues[p];
		    
		    if (ABS(h) + g == ABS(h))
			t = a[p][q] / h;
		    else {
			theta = .5 * h / a[p][q];
			t = 1.0 / (ABS(theta) + sqrt(1 + theta * theta));
			if (theta < 0.0)  t = -t;
		    }
		    // End of computing tangent of rotation angle
		    
		    c = 1.0 / sqrt(1.0 + t*t);
		    s = t * c;
		    tau = s / (1.0 + c);
		    h = t * a[p][q];
		    z[p]    -= h;
		    z[q]    += h;
		    evalues[p] -= h;
		    evalues[q] += h;
		    a[p][q] = 0.0;
		    
		    for (j = 0; j < p; j++) {
			g = a[j][p];
			h = a[j][q];
			a[j][p] = g - s * (h + g * tau);
			a[j][q] = h + s * (g - h * tau);
		    }
		    
		    for (j = p+1; j < q; j++) {
			g = a[p][j];
			h = a[j][q];
			a[p][j] = g - s * (h + g * tau);
			a[j][q] = h + s * (g - h * tau);
		    }
		    
		    for (j = q+1; j < SB_JACOBI_RANK; j++) {
			g = a[p][j];
			h = a[q][j];
			a[p][j] = g - s * (h + g * tau);
			a[q][j] = h + s * (g - h * tau);
		    }
		    
		    for (j = 0; j < SB_JACOBI_RANK; j++) {
			g = evectors[j][p];
			h = evectors[j][q];
			evectors[j][p] = g - s * (h + g * tau);
			evectors[j][q] = h + s * (g - h * tau);
		    }
		}
		rots++;
	    }
	}
	for (p = 0; p < SB_JACOBI_RANK; p++) {
	    evalues[p] = b[p] += z[p];
	    z[p] = 0;
	}
    }
}
#undef SB_JACOBI_RANK

//
// Returns inverse of matrix. Results are undefined for
// singular matrices.  Uses LU decomposition, from Numerial Recipies in C,
// pp 43-45.  They say "There is no better way."
//
// Oh, yeah! Well, if the matrix is affine, there IS a better way.
// So we call affine_inverse to see if we can get away with it...
//

SbMatrix
SbMatrix::inverse() const
{
    // Trivial case
    if (IS_IDENTITY(matrix))
	return SbMatrix::identity();

    // Affine case...
    SbMatrix affineAnswer;
#ifndef __linux
    if (  affine_inverse( (const SbMatrix &)matrix, affineAnswer ) )
#else
    if (  affine_inverse( matrix, affineAnswer ) )
#endif
	return affineAnswer;

    int         index[4];
    float       d, invmat[4][4], temp;
    SbMatrix	inverse = *this;
#ifdef DEBUGGING
    int         i, j;
#endif /* DEBUGGING */

    if(inverse.LUDecomposition(index, d)) {

#ifdef DEBUGGING
	for(j = 0; j < 4; j++) {
	    for(i = 0; i < 4; i++)
		invmat[j][i] = 0.0;
	    invmat[j][j] = 1.0;
	    inverse.LUBackSubstitution(index, invmat[j]);
	}
#else
	invmat[0][0] = 1.0;
	invmat[0][1] = 0.0;
	invmat[0][2] = 0.0;
	invmat[0][3] = 0.0;
	inverse.LUBackSubstitution(index, invmat[0]);
	invmat[1][0] = 0.0;
	invmat[1][1] = 1.0;
	invmat[1][2] = 0.0;
	invmat[1][3] = 0.0;
	inverse.LUBackSubstitution(index, invmat[1]);
	invmat[2][0] = 0.0;
	invmat[2][1] = 0.0;
	invmat[2][2] = 1.0;
	invmat[2][3] = 0.0;
	inverse.LUBackSubstitution(index, invmat[2]);
	invmat[3][0] = 0.0;
	invmat[3][1] = 0.0;
	invmat[3][2] = 0.0;
	invmat[3][3] = 1.0;
	inverse.LUBackSubstitution(index, invmat[3]);
#endif /* DEBUGGING */

#ifdef DEBUGGING
	// transpose invmat
	for(j = 0; j < 4; j++) {
	    for(i = 0; i < j; i++) {
		temp = invmat[i][j];
		invmat[i][j] = invmat[j][i];
		invmat[j][i] = temp;
	    }
	}
#else
#define SWAP(i,j) \
	temp = invmat[i][j]; \
	invmat[i][j] = invmat[j][i]; \
	invmat[j][i] = temp;

	SWAP(1,0);
	
	SWAP(2,0);
	SWAP(2,1);
	
	SWAP(3,0);
	SWAP(3,1);
	SWAP(3,2);
#undef SWAP	
#endif /* DEBUGGING */
	inverse.setValue(invmat);
    }

    return inverse;
}

// The following method finds the inverse of an affine matrix.
// The last column MUST be [0 0 0 1] for this to work.
// This is taken from graphics gems 2, page 603
//
// computes the inverse of a 3d affine matrix; i.e. a matrix with a 
// dimensionality of 4 where the right column has the entries (0,0,0,1).
//
// This procedure treats the 4 by 4 matrix as a block matrix and calculates
// the inverse of one submatrix for a significant performance 
// improvement over a general procedure that can invert any nonsingular matrix.
//
//             -1 
//  -1   |    |      |  -1    |
// M   = |A  0|  =   | A     0|
//       |    |      |        |
//       |    |      |   -1   |
//       |C  1|      |-CA    1|
//
// where   M is a 4 by 4 matrix,
//         A is the 3 by 3 upper left submatrix of M,
//         C is the 1 by 3 lower left submatrix of M.
// Input:
//   in - 3D affine matrix
// Output:
//   out - inverse of 3D affine matrix
// Returned Value:
//   TRUE  if input matrix is nonsingular and affine
//   FALSE otherwise

SbBool
SbMatrix::affine_inverse(const SbMatrix &in, SbMatrix &out) const
{
    // Check if matrix is affine...
    if ( in[0][3] != 0.0 || in[1][3] != 0.0 || in[2][3] != 0.0 || 
	 in[3][3] != 1.0 )
	return FALSE;

    // Calculate the determinant of submatrix A and determine if the matrix
    // is singular as limited by the double precision floating 
    // point data representation

    double det_1;
    double pos, neg, temp;

#define ACCUMULATE \
    if (temp >= 0.0) \
	pos += temp; \
    else             \
	neg += temp;

    pos = neg = 0.0;
    temp =  in[0][0] * in[1][1] * in[2][2];
    ACCUMULATE
    temp =  in[0][1] * in[1][2] * in[2][0];
    ACCUMULATE
    temp =  in[0][2] * in[1][0] * in[2][1];
    ACCUMULATE
    temp = -in[0][2] * in[1][1] * in[2][0];
    ACCUMULATE
    temp = -in[0][1] * in[1][0] * in[2][2];
    ACCUMULATE
    temp = -in[0][0] * in[1][2] * in[2][1];
    ACCUMULATE
    det_1 = pos + neg;

#undef ACCUMULATE

#define PRECISION_LIMIT (1.0e-15)

    // Is the submatrix A singular?
    temp = det_1 / (pos - neg);
    if (ABS(temp) < PRECISION_LIMIT)
	return FALSE;

    // Calculate inverse(A) = adj(A) / det(A)
    det_1 = 1.0 / det_1;
    out[0][0] =  (in[1][1] * in[2][2] - in[1][2] * in[2][1]) * det_1;
    out[1][0] = -(in[1][0] * in[2][2] - in[1][2] * in[2][0]) * det_1;
    out[2][0] =  (in[1][0] * in[2][1] - in[1][1] * in[2][0]) * det_1;
    out[0][1] = -(in[0][1] * in[2][2] - in[0][2] * in[2][1]) * det_1;
    out[1][1] =  (in[0][0] * in[2][2] - in[0][2] * in[2][0]) * det_1;
    out[2][1] = -(in[0][0] * in[2][1] - in[0][1] * in[2][0]) * det_1;
    out[0][2] =  (in[0][1] * in[1][2] - in[0][2] * in[1][1]) * det_1;
    out[1][2] = -(in[0][0] * in[1][2] - in[0][2] * in[1][0]) * det_1;
    out[2][2] =  (in[0][0] * in[1][1] - in[0][1] * in[1][0]) * det_1;

    // Calculate -C * inverse(A)
    out[3][0] = -( in[3][0] * out[0][0] + 
		   in[3][1] * out[1][0] + 
		   in[3][2] * out[2][0] );
    out[3][1] = -( in[3][0] * out[0][1] + 
		   in[3][1] * out[1][1] + 
		   in[3][2] * out[2][1] );
    out[3][2] = -( in[3][0] * out[0][2] + 
		   in[3][1] * out[1][2] + 
		   in[3][2] * out[2][2] );

    // Fill in last column
    out[0][3] = out[1][3] = out[2][3] = 0.0;
    out[3][3] = 1.0;

#undef PRECISION_LIMIT

    return TRUE;
}


//
// Perform an LU decomposition of a matrix.  From Numerical Recipes in C, pg 43
// Destroys the original matrix.  The resulting matrix is as follows
//
//      a b c d         1 0 0 0      a b c d
// LU = e f g h     L = e 1 0 0  U = 0 f g h
//      i j k l	        i j 1 0      0 0 k l
//      m n o p	        m n o 1      0 0 0 p
//
// For singular matrices, want to calculate something
// that is close to the inverse, so we can pick
// things scaled to zero in some dimension.
//
SbBool
SbMatrix::LUDecomposition(int index[4], float &d)
{
    int         imax;
    float       big, dum, sum, temp;
    float       vv[4];
#ifdef DEBUGGING
    int         i, j, k;
#endif /* DEBUGGING */

    d = 1.0;
    
#ifdef DEBUGGING
    for(i = 0; i < 4; i++) {
        big = 0.0;
        for(j = 0; j < 4; j++)
            if((temp = ABS(matrix[i][j])) > big) big = temp;
        if(big == 0.0) {
	    matrix[i][i] = 1e-6;
	    big = matrix[i][i];
        }
        vv[i] = 1.0 / big;
    }
#else
#define COMPUTE_VV(i) \
    big = 0.0; \
    if((temp = ABS(matrix[i][0])) > big) big = temp; \
    if((temp = ABS(matrix[i][1])) > big) big = temp; \
    if((temp = ABS(matrix[i][2])) > big) big = temp; \
    if((temp = ABS(matrix[i][3])) > big) big = temp; \
    if(big == 0.0) { \
	matrix[i][i] = 1e-6; \
	big = matrix[i][i]; \
    } \
    vv[i] = 1.0 / big;

    COMPUTE_VV(0);	
    COMPUTE_VV(1);	
    COMPUTE_VV(2);	
    COMPUTE_VV(3);
#undef COMPUTE_VV	
#endif /* DEBUGGING */

#ifdef DEBUGGING
    // This is the code as it originally existed.
    // Below this is the unrolled, really unreadable version.

    for(j = 0; j < 4; j++) {

	// BLOCK 1
        for(i = 0; i < j; i++) {
            sum = matrix[i][j];
            for(k = 0; k < i; k++)
                sum -= matrix[i][k] * matrix[k][j];
            matrix[i][j] = sum;
        }

        big = 0.0;

	// BLOCK 2
        for(i = j; i < 4; i++) {
            sum = matrix[i][j];
            for(k = 0; k < j; k++)
                sum -= matrix[i][k] * matrix[k][j];
            matrix[i][j] = sum;
            if((dum = vv[i] * ABS(sum)) >= big) {
                big = dum;
                imax = i;
            }
        }

	// BLOCK 3
        if(j != imax) {
            for(k = 0; k < 4; k++) {
                dum = matrix[imax][k];
                matrix[imax][k] = matrix[j][k];
                matrix[j][k] = dum;
            }
            d = -d;
            vv[imax] = vv[j];
        }

	// BLOCK 4
        index[j] = imax;
        if(matrix[j][j] == 0.0) matrix[j][j] = 1e-20;
	
	// BLOCK 5
        if(j != 4 - 1) {
            dum = 1.0 / (matrix[j][j]);
            for(i = j + 1; i < 4; i++)
                matrix[i][j] *= dum;
        }
	
    }
    
#else

    // This is the completely unreadable, but much faster
    // version of the above code. First, some macros that
    // never change.
    
// macro for block 3, inner k loop
#define BLOCK3INNER(j,k) \
    dum = matrix[imax][k]; \
    matrix[imax][k] = matrix[j][k]; \
    matrix[j][k] = dum;

// macro for block 4
#define BLOCK4(j) \
    index[j] = imax; \
    if(matrix[j][j] == 0.0) matrix[j][j] = 1e-20;

    // Now the code...

    // *********************************************
    // j = 0
    // *********************************************

    // BLOCK 1, j = 0
    // does nothing when j==0

    big = 0.0;

    // BLOCK 2, j = 0
    
// macro for block 2 when j == 0
// inner k loop does nothing when j == 0
#define BLOCK2J0(i) \
    sum = matrix[i][0]; \
    if((dum = vv[i] * ABS(sum)) >= big) { \
	big = dum; \
	imax = i; \
    }
	
    // for(i = j; i < 4; i++)
    BLOCK2J0(0)
    BLOCK2J0(1)
    BLOCK2J0(2)
    BLOCK2J0(3)
#undef BLOCK2J0
    
    // BLOCK 3, j = 0
    if(0 != imax) {
	BLOCK3INNER(0,0);
	BLOCK3INNER(0,1);
	BLOCK3INNER(0,2);
	BLOCK3INNER(0,3);
	d = -d;
	vv[imax] = vv[0];
    }

    // BLOCK 4, j = 0
    BLOCK4(0);

    // BLOCK 5, j = 0
    dum = 1.0 / (matrix[0][0]);
    // for(i = j + 1; i < 4; i++)
	matrix[1][0] *= dum;
	matrix[2][0] *= dum;
	matrix[3][0] *= dum;

    // *********************************************
    // j = 1
    // *********************************************

    // BLOCK 1, j = 1
    // for(i = 0; i < j; i++) {
    sum = matrix[0][1];
    // for(k = 0; k < i; k++)
	// nothing    

    big = 0.0;

    // BLOCK 2, j = 1
    
// macro for block 2 when j == 1
#define BLOCK2J1(i) \
    sum = matrix[i][1]; \
    sum -= matrix[i][0] * matrix[0][1]; \
    matrix[i][1] = sum; \
    if((dum = vv[i] * ABS(sum)) >= big) { \
	big = dum; \
	imax = i; \
    }
	
    // for(i = j; i < 4; i++)
    BLOCK2J1(1)
    BLOCK2J1(2)
    BLOCK2J1(3)
#undef BLOCK2J1
    
    // BLOCK 3, j = 1
    if(1 != imax) {
	BLOCK3INNER(1,0);
	BLOCK3INNER(1,1);
	BLOCK3INNER(1,2);
	BLOCK3INNER(1,3);
	d = -d;
	vv[imax] = vv[1];
    }

    // BLOCK 4, j = 1
    BLOCK4(1);

    // BLOCK 5, j = 1
    dum = 1.0 / (matrix[1][1]);
    // for(i = j + 1; i < 4; i++)
	matrix[2][1] *= dum;
	matrix[3][1] *= dum;
    
    // *********************************************
    // j = 2
    // *********************************************

    // BLOCK 1, j = 2
    // for(i = 0; i < j; i++) {
	// i = 0
	sum = matrix[0][2];
	// for(k = 0; k < i; k++)
	    // nothing
	// i = 1	    
	sum = matrix[1][2];
	// for(k = 0; k < i; k++)
	    sum -= matrix[1][0] * matrix[0][2];
	matrix[1][2] = sum;
	
    big = 0.0;

    // BLOCK 2, j = 2
    
// macro for block 2 when j == 2
#define BLOCK2J2(i) \
    sum = matrix[i][2]; \
    sum -= matrix[i][0] * matrix[0][2]; \
    sum -= matrix[i][1] * matrix[1][2]; \
    matrix[i][2] = sum; \
    if((dum = vv[i] * ABS(sum)) >= big) { \
	big = dum; \
	imax = i; \
    }
	
    // for(i = j; i < 4; i++)
    BLOCK2J2(2)
    BLOCK2J2(3)
#undef BLOCK2J2
    
    // BLOCK 3, j = 2
    if(2 != imax) {
	BLOCK3INNER(2,0);
	BLOCK3INNER(2,1);
	BLOCK3INNER(2,2);
	BLOCK3INNER(2,3);
	d = -d;
	vv[imax] = vv[2];
    }

    // BLOCK 4, j = 2
    BLOCK4(2);

    // BLOCK 5, j = 2
    dum = 1.0 / (matrix[2][2]);
    // for(i = j + 1; i < 4; i++)
	matrix[3][2] *= dum;

    // *********************************************
    // j = 3
    // *********************************************

    // BLOCK 1, j = 3
    // for(i = 0; i < j; i++) {
	// i = 0
	    sum = matrix[0][3];
	    // for(k = 0; k < i; k++)
		// nothing
	// i = 1	    
	    sum = matrix[1][3];
	    // for(k = 0; k < i; k++)
		sum -= matrix[1][0] * matrix[0][3];
	    matrix[1][3] = sum;
	// i = 2
	    sum = matrix[2][3];
	    // for(k = 0; k < i; k++)
		sum -= matrix[2][0] * matrix[0][3];
		sum -= matrix[2][1] * matrix[1][3];
	    matrix[2][3] = sum;
	
    big = 0.0;

    // BLOCK 2, j = 3
    
// macro for block 2 when j == 3
#define BLOCK2J3(i) \
    sum = matrix[i][3]; \
    sum -= matrix[i][0] * matrix[0][3]; \
    sum -= matrix[i][1] * matrix[1][3]; \
    sum -= matrix[i][2] * matrix[2][3]; \
    matrix[i][3] = sum; \
    if((dum = vv[i] * ABS(sum)) >= big) { \
	big = dum; \
	imax = i; \
    }
	
    // for(i = j; i < 4; i++)
    BLOCK2J3(3)
#undef BLOCK2J3
    
    // BLOCK 3, j = 3
    if(3 != imax) {
	BLOCK3INNER(3,0);
	BLOCK3INNER(3,1);
	BLOCK3INNER(3,2);
	BLOCK3INNER(3,3);
	d = -d;
	vv[imax] = vv[3];
    }

    // BLOCK 4, j = 3
    BLOCK4(3);

    // BLOCK 5, j = 3
    // does not execute when j == 3
    
#endif /* DEBUGGING */

    return TRUE;
}


//
// Perform back-subtitution on input LU matrix, from Numerial Recipies in C,
// pg 44
//
void
SbMatrix::LUBackSubstitution(int index[4], float b[4]) const
{
    int         ii = -1, ip, j;
    float       sum;
#ifdef DEBUGGING
    int         i;
#endif /* DEBUGGING */

#ifdef DEBUGGING
    for(i = 0; i < 4; i++) {
        ip = index[i];
        sum = b[ip];
        b[ip] = b[i];
        if(ii >= 0)
            for(j = ii; j <= i - 1; j++)
                sum -= matrix[i][j] * b[j];
        else if(sum) ii = i;
        b[i] = sum;
    }
#else
#define BACKSUB(i) \
    ip = index[i]; \
    sum = b[ip]; \
    b[ip] = b[i]; \
    if(ii >= 0) \
	for(j = ii; j <= i - 1; j++) \
	    sum -= matrix[i][j] * b[j]; \
    else if(sum) ii = i; \
    b[i] = sum;

    BACKSUB(0);
    BACKSUB(1);
    BACKSUB(2);
    BACKSUB(3);
#undef BACKSUB
#endif /* DEBUGGING */

#ifdef DEBUGGING
    for(i = 4 - 1; i >= 0; i--) {
        sum = b[i];
        for(j = i + 1; j < 4; j++)
            sum -= matrix[i][j]*b[j];
        b[i] = sum / matrix[i][i];
    }
#else
    sum = b[3];
    b[3] = sum / matrix[3][3];

    sum = b[2] - matrix[2][3]*b[3];
    b[2] = sum / matrix[2][2];

    sum = b[1] - matrix[1][2]*b[2] - matrix[1][3]*b[3];
    b[1] = sum / matrix[1][1];
	
    sum = b[0] - matrix[0][1]*b[1] - matrix[0][2]*b[2] - matrix[0][3]*b[3];
    b[0] = sum / matrix[0][0];
#endif /* DEBUGGING */
}

//
// Returns transpose of matrix
//

SbMatrix
SbMatrix::transpose() const
{
    return SbMatrix(matrix[0][0], matrix[1][0], matrix[2][0], matrix[3][0],
		    matrix[0][1], matrix[1][1], matrix[2][1], matrix[3][1],
		    matrix[0][2], matrix[1][2], matrix[2][2], matrix[3][2],
		    matrix[0][3], matrix[1][3], matrix[2][3], matrix[3][3]);
}

//
// Composes the matrix from translation, rotation, scale, etc.
//

void
SbMatrix::setTransform(const SbVec3f &translation,
		 const SbRotation &rotation,
		 const SbVec3f &scaleFactor,
		 const SbRotation &scaleOrientation,
		 const SbVec3f &center)
{
#define TRANSLATE(vec)		m.setTranslate(vec), multLeft(m)
#define ROTATE(rot)		rot.getValue(m), multLeft(m)
    SbMatrix m;

    makeIdentity();
    
    if (translation != SbVec3f(0,0,0))
	TRANSLATE(translation);

    if (center != SbVec3f(0,0,0))
	TRANSLATE(center);

    if (rotation != SbRotation(0,0,0,1))
	ROTATE(rotation);

    if (scaleFactor != SbVec3f(1,1,1)) {
	SbRotation so = scaleOrientation;
	if (so != SbRotation(0,0,0,1))
	    ROTATE(so);
	
	m.setScale(scaleFactor);
	multLeft(m);

	if (so != SbRotation(0,0,0,1)) {
	    so.invert();
	    ROTATE(so);
	}
    }

    if (center != SbVec3f(0,0,0))
	TRANSLATE(-center);

#undef TRANSLATE
#undef ROTATE
}

//
// Decomposes a rotation into translation etc, based on scale
//

void
SbMatrix::getTransform(SbVec3f &translation,
		    SbRotation &rotation,
		    SbVec3f &scaleFactor,
		    SbRotation &scaleOrientation,
		    const SbVec3f &center) const
{
    SbMatrix so, rot, proj;
    if (center != SbVec3f(0,0,0)) {
	// to get fields for a non-0 center, we
	// need to decompose a new matrix "m" such
	// that [-center][m][center] = [this]
	// i.e., [m] = [center][this][-center]
	// (this trick stolen from Showcase code)
	SbMatrix m,c;
	m.setTranslate(-center);
	m.multLeft(*this);
	c.setTranslate(center);
	m.multLeft(c);
	m.factor(so,scaleFactor,rot,translation,proj);
    }
    else
	this->factor(so,scaleFactor,rot,translation,proj);

    scaleOrientation = so.transpose();  // have to transpose because factor 
				        // gives us transpose of correct answer.
    rotation = rot;
}



////////////////////////////////////////////
//
// Matrix/matrix and matrix/vector arithmetic
//
////////////////////////////////////////////

//
// Multiplies matrix by given matrix on right
//

SbMatrix &
SbMatrix::multRight(const SbMatrix &m)
{
    // Trivial cases
    if (IS_IDENTITY(m))
	return *this;
    else if (IS_IDENTITY(matrix))
	return (*this = m);
	
    SbMat	tmp;

#define MULT_RIGHT(i,j) (matrix[i][0]*m.matrix[0][j] + \
			 matrix[i][1]*m.matrix[1][j] + \
			 matrix[i][2]*m.matrix[2][j] + \
			 matrix[i][3]*m.matrix[3][j])

    tmp[0][0] = MULT_RIGHT(0,0);
    tmp[0][1] = MULT_RIGHT(0,1);
    tmp[0][2] = MULT_RIGHT(0,2);
    tmp[0][3] = MULT_RIGHT(0,3);
    tmp[1][0] = MULT_RIGHT(1,0);
    tmp[1][1] = MULT_RIGHT(1,1);
    tmp[1][2] = MULT_RIGHT(1,2);
    tmp[1][3] = MULT_RIGHT(1,3);
    tmp[2][0] = MULT_RIGHT(2,0);
    tmp[2][1] = MULT_RIGHT(2,1);
    tmp[2][2] = MULT_RIGHT(2,2);
    tmp[2][3] = MULT_RIGHT(2,3);
    tmp[3][0] = MULT_RIGHT(3,0);
    tmp[3][1] = MULT_RIGHT(3,1);
    tmp[3][2] = MULT_RIGHT(3,2);
    tmp[3][3] = MULT_RIGHT(3,3);

#undef MULT_RIGHT

    return (*this = tmp);
}

//
// Multiplies matrix by given matrix on left
//

SbMatrix &
SbMatrix::multLeft(const SbMatrix &m)
{
    // Trivial cases
    if (IS_IDENTITY(m))
	return *this;
    else if (IS_IDENTITY(matrix))
	return (*this = m);
	
    SbMat	tmp;

#define MULT_LEFT(i,j) (m.matrix[i][0]*matrix[0][j] + \
			m.matrix[i][1]*matrix[1][j] + \
			m.matrix[i][2]*matrix[2][j] + \
			m.matrix[i][3]*matrix[3][j])

    tmp[0][0] = MULT_LEFT(0,0);
    tmp[0][1] = MULT_LEFT(0,1);
    tmp[0][2] = MULT_LEFT(0,2);
    tmp[0][3] = MULT_LEFT(0,3);
    tmp[1][0] = MULT_LEFT(1,0);
    tmp[1][1] = MULT_LEFT(1,1);
    tmp[1][2] = MULT_LEFT(1,2);
    tmp[1][3] = MULT_LEFT(1,3);
    tmp[2][0] = MULT_LEFT(2,0);
    tmp[2][1] = MULT_LEFT(2,1);
    tmp[2][2] = MULT_LEFT(2,2);
    tmp[2][3] = MULT_LEFT(2,3);
    tmp[3][0] = MULT_LEFT(3,0);
    tmp[3][1] = MULT_LEFT(3,1);
    tmp[3][2] = MULT_LEFT(3,2);
    tmp[3][3] = MULT_LEFT(3,3);

#undef MULT_LEFT

    return (*this = tmp);

}

//
// Multiplies matrix by given column vector, giving vector result
//

void
SbMatrix::multMatrixVec(const SbVec3f &src, SbVec3f &dst) const
{
    float	x,y,z,w;
    
    x = matrix[0][0]*src[0] + matrix[0][1]*src[1] +
	matrix[0][2]*src[2] + matrix[0][3];
    y = matrix[1][0]*src[0] + matrix[1][1]*src[1] +
	matrix[1][2]*src[2] + matrix[1][3];
    z = matrix[2][0]*src[0] + matrix[2][1]*src[1] +
	matrix[2][2]*src[2] + matrix[2][3];
    w = matrix[3][0]*src[0] + matrix[3][1]*src[1] +
	matrix[3][2]*src[2] + matrix[3][3];
    
    dst.setValue(x/w, y/w, z/w);
}

//
// Multiplies given row vector by matrix, giving vector result
//

void
SbMatrix::multVecMatrix(const SbVec3f &src, SbVec3f &dst) const
{
    float	x,y,z,w;
    
    x = src[0]*matrix[0][0] + src[1]*matrix[1][0] +
	src[2]*matrix[2][0] + matrix[3][0];
    y = src[0]*matrix[0][1] + src[1]*matrix[1][1] +
	src[2]*matrix[2][1] + matrix[3][1];
    z = src[0]*matrix[0][2] + src[1]*matrix[1][2] +
	src[2]*matrix[2][2] + matrix[3][2];
    w = src[0]*matrix[0][3] + src[1]*matrix[1][3] +
	src[2]*matrix[2][3] + matrix[3][3];
    
    dst.setValue(x/w, y/w, z/w);
}

//
// Multiplies given row vector by matrix, giving vector result
// src is assumed to be a direction vector, so translation part of
// matrix is ignored.
//

void
SbMatrix::multDirMatrix(const SbVec3f &src, SbVec3f &dst) const
{
    float	x,y,z;
    
    x = src[0]*matrix[0][0] + src[1]*matrix[1][0] + src[2]*matrix[2][0];
    y = src[0]*matrix[0][1] + src[1]*matrix[1][1] + src[2]*matrix[2][1];
    z = src[0]*matrix[0][2] + src[1]*matrix[1][2] + src[2]*matrix[2][2];
    
    dst.setValue(x, y, z);
}

//
// Multiplies the given line by the matrix resulting in a new
// line. The origin of the line is transformed by the whole matrix.
// The translation part of the matrix is ignored when transforming
// the direction of the line.
//

void
SbMatrix::multLineMatrix(const SbLine &src, SbLine &dst) const
{
	SbVec3f pos, dir;
	multVecMatrix(src.getPosition(), pos);
	multDirMatrix(src.getDirection(), dir);
	dst.setValue(pos, pos+dir);
}


////////////////////////////////////////////
//
// Miscellaneous
//
////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Prints a formatted version of the matrix to the given file
//    pointer.
//
// Use: public

void
SbMatrix::print(FILE *fp) const
//
////////////////////////////////////////////////////////////////////////
{
    int	i, j;
    
    for (i = 0; i < 4; i++)
	for (j = 0; j < 4; j++)
	    fprintf(fp, "%10.5g%c", matrix[i][j], j < 3 ? '\t' : '\n');
}

////////////////////////////////////////////
//
// Operators
//
////////////////////////////////////////////

//
// Sets value from 4x4 array of elements
//

SbMatrix &
SbMatrix::operator =(const SbMat &m)
{
    setValue(m);
    return *this;
}

//
// ??? why do we need to write this out like this?
// ??? it doesn't seem to work otherwise, but why not?
//
SbMatrix &
SbMatrix::operator =(const SbMatrix &m)
{
    matrix[0][0] = m.matrix[0][0];
    matrix[0][1] = m.matrix[0][1];
    matrix[0][2] = m.matrix[0][2];
    matrix[0][3] = m.matrix[0][3];
    matrix[1][0] = m.matrix[1][0];
    matrix[1][1] = m.matrix[1][1];
    matrix[1][2] = m.matrix[1][2];
    matrix[1][3] = m.matrix[1][3];
    matrix[2][0] = m.matrix[2][0];
    matrix[2][1] = m.matrix[2][1];
    matrix[2][2] = m.matrix[2][2];
    matrix[2][3] = m.matrix[2][3];
    matrix[3][0] = m.matrix[3][0];
    matrix[3][1] = m.matrix[3][1];
    matrix[3][2] = m.matrix[3][2];
    matrix[3][3] = m.matrix[3][3];

    return *this;
}


//
// Binary multiplication of matrices
//

SbMatrix
operator *(const SbMatrix &l, const SbMatrix &r)
{
    SbMatrix m = l;
    
    m *= r;
    
    return m;
}

//
// Equality comparison operator. All componenents must match exactly.
//

int
operator ==(const SbMatrix &m1, const SbMatrix &m2)
{
    return (
	    m1.matrix[0][0] == m2.matrix[0][0] &&
	    m1.matrix[0][1] == m2.matrix[0][1] &&
	    m1.matrix[0][2] == m2.matrix[0][2] &&
	    m1.matrix[0][3] == m2.matrix[0][3] &&

	    m1.matrix[1][0] == m2.matrix[1][0] &&
	    m1.matrix[1][1] == m2.matrix[1][1] &&
	    m1.matrix[1][2] == m2.matrix[1][2] &&
	    m1.matrix[1][3] == m2.matrix[1][3] &&

	    m1.matrix[2][0] == m2.matrix[2][0] &&
	    m1.matrix[2][1] == m2.matrix[2][1] &&
	    m1.matrix[2][2] == m2.matrix[2][2] &&
	    m1.matrix[2][3] == m2.matrix[2][3] &&

	    m1.matrix[3][0] == m2.matrix[3][0] &&
	    m1.matrix[3][1] == m2.matrix[3][1] &&
	    m1.matrix[3][2] == m2.matrix[3][2] &&
	    m1.matrix[3][3] == m2.matrix[3][3]
	    );
}

//
// Equality comparison operator within given tolerance for each component
//

SbBool
SbMatrix::equals(const SbMatrix &m, float tolerance) const
{
    int		i, j;
    float	d;

    for (i = 0; i < 4; i++)
	for (j = 0; j < 4; j++) {
	    d = matrix[i][j] - m.matrix[i][j];
	    if (ABS(d) > tolerance)
		return FALSE;
	}

    return TRUE;
}
