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
 |   $Revision: 1.1.1.1 $
 |
 |   This is the engine mechanism for the SoCalculator class.  The
 |   expression parsing and evaluating code are located in separate
 |   files.
 |
 |   Classes:
 |      SoCalculator
 |
 |   Author(s)          : Ronen Barzel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <assert.h>
#include <Inventor/engines/SoCalculator.h>
#include <Inventor/errors/SoDebugError.h>
#include "SoCalcExpr.h"
#include "SoEngineUtil.h"

SO_ENGINE_SOURCE(SoCalculator);

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Constructor
//
// Use: public

SoCalculator::SoCalculator()
//
////////////////////////////////////////////////////////////////////////
{
    SO_ENGINE_CONSTRUCTOR(SoCalculator);
    SO_ENGINE_ADD_INPUT(a,	  (0));
    SO_ENGINE_ADD_INPUT(b,	  (0));
    SO_ENGINE_ADD_INPUT(c,	  (0));
    SO_ENGINE_ADD_INPUT(d,	  (0));
    SO_ENGINE_ADD_INPUT(e,	  (0));
    SO_ENGINE_ADD_INPUT(f,	  (0));
    SO_ENGINE_ADD_INPUT(g,	  (0));
    SO_ENGINE_ADD_INPUT(h,	  (0));
    SO_ENGINE_ADD_INPUT(A,	  (SbVec3f(0,0,0)));
    SO_ENGINE_ADD_INPUT(B,	  (SbVec3f(0,0,0)));
    SO_ENGINE_ADD_INPUT(C,	  (SbVec3f(0,0,0)));
    SO_ENGINE_ADD_INPUT(D,	  (SbVec3f(0,0,0)));
    SO_ENGINE_ADD_INPUT(E,	  (SbVec3f(0,0,0)));
    SO_ENGINE_ADD_INPUT(F,	  (SbVec3f(0,0,0)));
    SO_ENGINE_ADD_INPUT(G,	  (SbVec3f(0,0,0)));
    SO_ENGINE_ADD_INPUT(H,	  (SbVec3f(0,0,0)));
    SO_ENGINE_ADD_INPUT(expression,	  (""));
    SO_ENGINE_ADD_OUTPUT(oa, SoMFFloat);
    SO_ENGINE_ADD_OUTPUT(ob, SoMFFloat);
    SO_ENGINE_ADD_OUTPUT(oc, SoMFFloat);
    SO_ENGINE_ADD_OUTPUT(od, SoMFFloat);
    SO_ENGINE_ADD_OUTPUT(oA, SoMFVec3f);
    SO_ENGINE_ADD_OUTPUT(oB, SoMFVec3f);
    SO_ENGINE_ADD_OUTPUT(oC, SoMFVec3f);
    SO_ENGINE_ADD_OUTPUT(oD, SoMFVec3f);

    parser = new SoCalcParser(lookupFloat, lookupVec3f, this);
    reparse = FALSE;
    isBuiltIn = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Destructor
//
// Use: private

SoCalculator::~SoCalculator()
//
////////////////////////////////////////////////////////////////////////
{
    delete parser;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    called when new inputs are presented
//
// Use: private

void
SoCalculator::inputChanged(SoField *which)
{
    if (which == &expression)
	reparse = TRUE;
}

////////////////////////////////////////////////////////////////////////
//
// Description:
//    Evaluation routine
//
// Use: private

void
SoCalculator::evaluate()
//
////////////////////////////////////////////////////////////////////////
{
    if (reparse) {
	parser->clear();
	SbBool OK = TRUE;
        int i;
	for (i=0; i<expression.getNum(); i++) {
	    if (!parser->parse(expression[i].getString())) {
		OK = FALSE;
		break;
	    }
	}
	if (! OK) {
#ifdef DEBUG
	    SoDebugError::post("SoCalculator::evaluate",
			       "Invalid expression '%s'",
			       expression[i].getString());
#endif /* DEBUG */

	    expression.setValue("");
	    parser->clear();
	}
	reparse = FALSE;
    }

    int max = 0;
    int	na = a.getNum(); if (na > max) max = na;
    int nb = b.getNum(); if (nb > max) max = nb;
    int nc = c.getNum(); if (nc > max) max = nc;
    int nd = d.getNum(); if (nd > max) max = nd;
    int ne = e.getNum(); if (ne > max) max = ne;
    int nf = f.getNum(); if (nf > max) max = nf;
    int ng = g.getNum(); if (ng > max) max = ng;
    int nh = h.getNum(); if (nh > max) max = nh;

    int	nA = A.getNum(); if (nA > max) max = nA;
    int nB = B.getNum(); if (nB > max) max = nB;
    int nC = C.getNum(); if (nC > max) max = nC;
    int nD = D.getNum(); if (nD > max) max = nD;
    int nE = E.getNum(); if (nE > max) max = nE;
    int nF = F.getNum(); if (nF > max) max = nF;
    int nG = G.getNum(); if (nG > max) max = nG;
    int nH = H.getNum(); if (nG > max) max = nH;

    int nout = max;
    SO_ENGINE_OUTPUT(oa, SoMFFloat, setNum(nout));
    SO_ENGINE_OUTPUT(ob, SoMFFloat, setNum(nout));
    SO_ENGINE_OUTPUT(oc, SoMFFloat, setNum(nout));
    SO_ENGINE_OUTPUT(od, SoMFFloat, setNum(nout));
    SO_ENGINE_OUTPUT(oA, SoMFFloat, setNum(nout));
    SO_ENGINE_OUTPUT(oB, SoMFFloat, setNum(nout));
    SO_ENGINE_OUTPUT(oC, SoMFFloat, setNum(nout));
    SO_ENGINE_OUTPUT(oD, SoMFFloat, setNum(nout));

    for (int i=0; i<nout; i++) {
	va = a[clamp(i,na)];
	vb = b[clamp(i,nb)];
	vc = c[clamp(i,nc)];
	vd = d[clamp(i,nd)];
	ve = e[clamp(i,ne)];
	vf = f[clamp(i,nf)];
	vg = g[clamp(i,ng)];
	vh = h[clamp(i,nh)];
	vA = A[clamp(i,nA)];
	vB = B[clamp(i,nB)];
	vC = C[clamp(i,nC)];
	vD = D[clamp(i,nD)];
	vE = E[clamp(i,nE)];
	vF = F[clamp(i,nF)];
	vG = G[clamp(i,nG)];
	vH = H[clamp(i,nH)];
	ova = 0;
	ovb = 0;
	ovc = 0;
	ovd = 0;
	ovA = SbVec3f(0,0,0);
	ovB = SbVec3f(0,0,0);
	ovC = SbVec3f(0,0,0);
	ovD = SbVec3f(0,0,0);

	parser->eval();

	SO_ENGINE_OUTPUT(oa, SoMFFloat, set1Value(i, ova));
	SO_ENGINE_OUTPUT(ob, SoMFFloat, set1Value(i, ovb));
	SO_ENGINE_OUTPUT(oc, SoMFFloat, set1Value(i, ovc));
	SO_ENGINE_OUTPUT(od, SoMFFloat, set1Value(i, ovd));
	SO_ENGINE_OUTPUT(oA, SoMFVec3f, set1Value(i, ovA));
	SO_ENGINE_OUTPUT(oB, SoMFVec3f, set1Value(i, ovB));
	SO_ENGINE_OUTPUT(oC, SoMFVec3f, set1Value(i, ovC));
	SO_ENGINE_OUTPUT(oD, SoMFVec3f, set1Value(i, ovD));
    }
}


////////////////////////////////////////////////////////////////////////
//
// Description:
//    maps field names to storage locations
//
// Use: private, given as callback to the parser

float *SoCalculator::lookupFloat(void *data, const char *nm)
{
    SoCalculator *e = (SoCalculator *) data;
    assert(e->isOfType(SoCalculator::getClassTypeId()));
    if (strcmp(nm, "a") ==0) return &e->va;
    if (strcmp(nm, "b") ==0) return &e->vb;
    if (strcmp(nm, "c") ==0) return &e->vc;
    if (strcmp(nm, "d") ==0) return &e->vd;
    if (strcmp(nm, "e") ==0) return &e->ve;
    if (strcmp(nm, "f") ==0) return &e->vf;
    if (strcmp(nm, "g") ==0) return &e->vg;
    if (strcmp(nm, "h") ==0) return &e->vh;
    if (strcmp(nm, "oa") ==0) return &e->ova;
    if (strcmp(nm, "ob") ==0) return &e->ovb;
    if (strcmp(nm, "oc") ==0) return &e->ovc;
    if (strcmp(nm, "od") ==0) return &e->ovd;
    if (strcmp(nm, "ta") ==0) return &e->ta;
    if (strcmp(nm, "tb") ==0) return &e->tb;
    if (strcmp(nm, "tc") ==0) return &e->tc;
    if (strcmp(nm, "td") ==0) return &e->td;
    if (strcmp(nm, "te") ==0) return &e->te;
    if (strcmp(nm, "tf") ==0) return &e->tf;
    if (strcmp(nm, "tg") ==0) return &e->tg;
    if (strcmp(nm, "th") ==0) return &e->th;
    return NULL;
}

SbVec3f *SoCalculator::lookupVec3f(void *data, const char *nm)
{
    SoCalculator *e = (SoCalculator *) data;
    assert(e->isOfType(SoCalculator::getClassTypeId()));
    if (strcmp(nm, "A") ==0) return &e->vA;
    if (strcmp(nm, "B") ==0) return &e->vB;
    if (strcmp(nm, "C") ==0) return &e->vC;
    if (strcmp(nm, "D") ==0) return &e->vD;
    if (strcmp(nm, "E") ==0) return &e->vE;
    if (strcmp(nm, "F") ==0) return &e->vF;
    if (strcmp(nm, "G") ==0) return &e->vG;
    if (strcmp(nm, "H") ==0) return &e->vH;
    if (strcmp(nm, "oA") ==0) return &e->ovA;
    if (strcmp(nm, "oB") ==0) return &e->ovB;
    if (strcmp(nm, "oC") ==0) return &e->ovC;
    if (strcmp(nm, "oD") ==0) return &e->ovD;
    if (strcmp(nm, "tA") ==0) return &e->tA;
    if (strcmp(nm, "tB") ==0) return &e->tB;
    if (strcmp(nm, "tC") ==0) return &e->tC;
    if (strcmp(nm, "tD") ==0) return &e->tD;
    if (strcmp(nm, "tE") ==0) return &e->tE;
    if (strcmp(nm, "tF") ==0) return &e->tF;
    if (strcmp(nm, "tG") ==0) return &e->tG;
    if (strcmp(nm, "tH") ==0) return &e->tH;
    return NULL;
}
