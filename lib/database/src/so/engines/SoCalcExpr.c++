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
 |   $Revision: 1.2 $
 |
 |   Classes:
 |      ???
 |
 |   Author(s)          : Ronen Barzel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <Inventor/errors/SoDebugError.h>
#include "SoCalcExpr.h"

// for development, these provide a builtin
// data storage location (don't need to
// connect the parser to an engine).
static float *lookupfloat(void *, const char *nm)
{
    static float store[20];
    if (strcmp(nm, "a") ==0) return &store[0];
    if (strcmp(nm, "b") ==0) return &store[1];
    if (strcmp(nm, "c") ==0) return &store[2];
    if (strcmp(nm, "d") ==0) return &store[3];
    if (strcmp(nm, "e") ==0) return &store[4];
    if (strcmp(nm, "f") ==0) return &store[5];
    if (strcmp(nm, "g") ==0) return &store[6];
    if (strcmp(nm, "h") ==0) return &store[7];
    if (strcmp(nm, "ta") ==0) return &store[8];
    if (strcmp(nm, "tb") ==0) return &store[9];
    if (strcmp(nm, "tc") ==0) return &store[10];
    if (strcmp(nm, "td") ==0) return &store[11];
    if (strcmp(nm, "te") ==0) return &store[12];
    if (strcmp(nm, "tf") ==0) return &store[13];
    if (strcmp(nm, "tg") ==0) return &store[14];
    if (strcmp(nm, "th") ==0) return &store[15];
    if (strcmp(nm, "oa") ==0) return &store[16];
    if (strcmp(nm, "ob") ==0) return &store[17];
    if (strcmp(nm, "oc") ==0) return &store[18];
    if (strcmp(nm, "od") ==0) return &store[19];
    return NULL;
}
static SbVec3f *lookupvec3f(void *, const char *nm)
{
    static SbVec3f store[20];
    if (strcmp(nm, "A") ==0) return &store[0];
    if (strcmp(nm, "B") ==0) return &store[1];
    if (strcmp(nm, "C") ==0) return &store[2];
    if (strcmp(nm, "D") ==0) return &store[3];
    if (strcmp(nm, "E") ==0) return &store[4];
    if (strcmp(nm, "F") ==0) return &store[5];
    if (strcmp(nm, "G") ==0) return &store[6];
    if (strcmp(nm, "H") ==0) return &store[7];
    if (strcmp(nm, "tA") ==0) return &store[8];
    if (strcmp(nm, "tB") ==0) return &store[9];
    if (strcmp(nm, "tC") ==0) return &store[10];
    if (strcmp(nm, "tD") ==0) return &store[11];
    if (strcmp(nm, "tE") ==0) return &store[12];
    if (strcmp(nm, "tF") ==0) return &store[13];
    if (strcmp(nm, "tG") ==0) return &store[14];
    if (strcmp(nm, "tH") ==0) return &store[15];
    if (strcmp(nm, "oA") ==0) return &store[16];
    if (strcmp(nm, "oB") ==0) return &store[17];
    if (strcmp(nm, "oC") ==0) return &store[18];
    if (strcmp(nm, "oD") ==0) return &store[19];
    return NULL;
}

//
// base expression class
//

SbBool Expr::error;
float *(*Expr::lookupFloatField)(void *, const char *) = lookupfloat;
SbVec3f *(*Expr::lookupVec3fField)(void *, const char *) = lookupvec3f;
void *Expr::data;



Expr::~Expr() {}

void
Expr::print(int level)
{
#ifdef DEBUG
    printSpace(level);
    printf("Expr: type %s\n", type == FLOAT ? "FLOAT" : "VEC3F");
#endif /* DEBUG */
}

void
Expr::printSpace(int level)
{
#ifdef DEBUG
    for (int i = level / 2; i > 0; --i)
	putchar('\t');

    if (level & 1) {
	putchar(' ');
	putchar(' ');
	putchar(' ');
	putchar(' ');
    }
#endif /* DEBUG */
}

float
Expr::getFloat() {
    err("Expr: internal error: can't get float val");
    return 0;
}

SbVec3f
Expr::getVec3f() {
    err("Expr: internal error: can't get vector val");
    return SbVec3f(0,0,0);
}


float
Expr::setFloat(float) {
    err("Expr: internal error: can't set float val");
    return 0;
}

SbVec3f
Expr::setVec3f(const SbVec3f &) {
    err("Expr: internal error: can't set vector val");
    return SbVec3f(0,0,0);
}

void
Expr::eval() {
    if (type == FLOAT)
	getFloat();
    else
	getVec3f();
}

void
Expr::err(const char *formatString ...)
{
    char        buf[1000];
    va_list     ap;

    va_start(ap, formatString);
    vsprintf(buf, formatString, ap);
    va_end(ap);

#ifdef DEBUG
    SoDebugError::post("SoCalculator::evaluate", buf);
#endif
    error = TRUE;
}


//
// List of expressions
//
void
ExprList::truncate(int n)
{
    for (int i=n; i<getLength(); i++)
    {
	delete (*this)[i];
    }

    SbPList::truncate(0);
}

void
ExprList::eval()
{
    for (int i=0; i<getLength(); i++)
	(*this)[i]->eval();
}

void
ExprList::print()
{
#ifdef DEBUG
    printf("List: %d expressions\n", getLength());
    for (int i = 0; i < getLength(); i++) {
	printf("\nExpression %d:\n\n", i);
	(*this)[i]->print(0);
    }
#endif /* DEBUG */
}

//
// Terminal expressions
//
Const::~Const() {}

void
Const::print(int level)
{
#ifdef DEBUG
    printSpace(level);
    printf("Const: val = %g\n", val);
#endif /* DEBUG */
}

float
Const::getFloat()
{
    return val;
}


Var::Var(const char *nm, Type type)
    : Expr(type), name(strdup(nm))
{
    if (type == FLOAT)
	pfloat = lookupFloatField(data, name);
    else
	pvec3f = lookupVec3fField(data, name);
}

Var::~Var() { free((void *) name); }

void Var::print(int level)
{
#ifdef DEBUG
    printSpace(level);
    printf("Var: name = %s, value = ", name);
    if (type == FLOAT)
	printf("%g", *pfloat);
    else
	printf("(%g %g %g)", (*pvec3f)[0], (*pvec3f)[1], (*pvec3f)[2]);
    printf("\n");
#endif /* DEBUG */
}

float Var::getFloat()
{
    return *pfloat;
}

SbVec3f Var::getVec3f()
{
    return *pvec3f;
}

float Var::setFloat(float v)
{
    return *pfloat = v;
}

SbVec3f Var::setVec3f(const SbVec3f &v)
{
    return *pvec3f = v;
}

//
// Functions
//
Func::Func(const char *nm, Type type) : Expr(type), name(nm) {}
Func::~Func() { delete args; }

void
Func::print(int level)
{
#ifdef DEBUG
    printSpace(level);
    printf("Func: name = %s, %d args:\n", name, args->getLength());
    for (int i = 0; i < args->getLength(); i++)
	(*args)[i]->print(level + 1);
#endif /* DEBUG */
}

Func_d::Func_d(const char *name, double (*f)(double)) :
		Func(name, FLOAT), func(f) {}
Func_d::~Func_d() {}
Func *Func_d::dup() const { return new Func_d(name, func); }
float Func_d::getFloat() { return (*func)((*args)[0]->getFloat()); }
void Func_d::setArgs(ExprList *a)
{
    args = a;
    if (args->getLength() != 1)
	err("Function %s: wrong number of arguments (%d should be 1)", name, args->getLength());
    if ((*args)[0]->type != FLOAT)
	err("Function %s: argument must be scalar", name);
};

Func_dd::~Func_dd() {}
Func *Func_dd::dup() const { return new Func_dd(name, func); }
float Func_dd::getFloat() {
    Expr *e0 = (*args)[0], *e1 = (*args)[1];
    return (*func)(e0->getFloat(), e1->getFloat());
}
void Func_dd::setArgs(ExprList *a)
{
    args = a;
    if (args->getLength() != 2)
	err("Function %s: wrong number of arguments (%d should be 2)", name, args->getLength());
    if ((*args)[0]->type != FLOAT)
	err("Function %s: first argument must be scalar", name);
    if ((*args)[1]->type != FLOAT)
	err("Function %s: second argument must be scalar", name);
};

Func_id::~Func_id() {}
Func *Func_id::dup() const { return new Func_id(name, func); }
float Func_id::getFloat() {
    Expr *e0 = (*args)[0], *e1 = (*args)[1];
    return (*func)((int) e0->getFloat(), e1->getFloat());
}
void Func_id::setArgs(ExprList *a)
{
    args = a;
    if (args->getLength() != 2)
	err("Function %s: wrong number of arguments (%d should be 2)", name, args->getLength());
    if ((*args)[0]->type != FLOAT)
	err("Function %s: first argument must be scalar", name);
    if ((*args)[1]->type != FLOAT)
	err("Function %s: second argument must be scalar", name);
};

Func_v::~Func_v() {}
Func *Func_v::dup() const { return new Func_v(name, func); }
float Func_v::getFloat() { return (*func)((*args)[0]->getVec3f()); }
void Func_v::setArgs(ExprList *a)
{
    args = a;
    if (args->getLength() != 1)
	err("Function %s: wrong number of arguments (%d should be 1)", name, args->getLength());
    if ((*args)[0]->type != VEC3F)
	err("Function %s: first argument must be vector", name);
};

Funcv_v::~Funcv_v() {}
Func *Funcv_v::dup() const { return new Funcv_v(name, func); }
SbVec3f Funcv_v::getVec3f() { return (*func)((*args)[0]->getVec3f()); }
void Funcv_v::setArgs(ExprList *a)
{
    args = a;
    if (args->getLength() != 1)
	err("Function %s: wrong number of arguments (%d should be 1)", name, args->getLength());
    if ((*args)[0]->type != VEC3F)
	err("Function %s: first argument must be vector", name);
};

Func_vv::~Func_vv() {}
Func *Func_vv::dup() const { return new Func_vv(name, func); }
float Func_vv::getFloat() {
    Expr *e0 = (*args)[0], *e1 = (*args)[1];
    return (*func)(e0->getVec3f(), e1->getVec3f());
}
void Func_vv::setArgs(ExprList *a)
{
    args = a;
    if (args->getLength() != 2)
	err("Function %s: wrong number of arguments (%d should be 2)", name, args->getLength());
    if ((*args)[0]->type != VEC3F)
	err("Function %s: first argument must be vector", name);
    if ((*args)[1]->type != VEC3F)
	err("Function %s: second argument must be vector", name);
};

Funcv_vv::~Funcv_vv() {}
Func *Funcv_vv::dup() const { return new Funcv_vv(name, func); }
SbVec3f Funcv_vv::getVec3f() {
    Expr *e0 = (*args)[0], *e1 = (*args)[1];
    return (*func)(e0->getVec3f(), e1->getVec3f());
}
void Funcv_vv::setArgs(ExprList *a)
{
    args = a;
    if (args->getLength() != 2)
	err("Function %s: wrong number of arguments (%d should be 2)", name, args->getLength());
    if ((*args)[0]->type != VEC3F)
	err("Function %s: first argument must be vector", name);
    if ((*args)[1]->type != VEC3F)
	err("Function %s: second argument must be vector", name);
};

Funcv_ddd::~Funcv_ddd() {}
Func *Funcv_ddd::dup() const { return new Funcv_ddd(name, func); }
SbVec3f Funcv_ddd::getVec3f() {
    Expr *e0 = (*args)[0], *e1 = (*args)[1], *e2 = (*args)[2];
    return (*func)(e0->getFloat(), e1->getFloat(), e2->getFloat());
}
void Funcv_ddd::setArgs(ExprList *a)
{
    args = a;
    if (args->getLength() != 3)
	err("Function %s: wrong number of arguments (%d should be 3)", name, args->getLength());
    if ((*args)[0]->type != FLOAT)
	err("Function %s: first argument must be scalar", name);
    if ((*args)[1]->type != FLOAT)
	err("Function %s: second argument must be scalar", name);
    if ((*args)[2]->type != FLOAT)
	err("Function %s: third argument must be scalar", name);
};

//
// Operations
//
BinaryOp::~BinaryOp() { delete a; delete b; }
BinaryOp::BinaryOp(Expr *ea, Expr *eb, int TypeBits) : a(ea), b(eb)
{
    SbBool OK = FALSE;

    if (a->type == FLOAT && b->type == FLOAT)
	OK = TypeBits & FF;

    else if (a->type == FLOAT && b->type == VEC3F)
	OK = TypeBits & FV;

    else if (a->type == VEC3F && b->type == FLOAT)
	OK = TypeBits & VF;

    else if (a->type == VEC3F && b->type == VEC3F)
	OK = TypeBits & VV;
    
    if (!OK)
	err("BinaryOp:  incompatible types (%s and %s)",
	    a->typeName(), b->typeName());
}

AssignIndex::AssignIndex(Expr *_a, Expr *_b, Expr *_c) 
	: Expr(Expr::FLOAT)
{
    a = _a;
    b = _b;
    c = _c;

    if (a->type != Expr::VEC3F)
	err("In expressions of the form 'A[b] = c', A must be a vector");
    if (b->type != Expr::FLOAT)
	err("In expressions of the form 'A[b] = c', b must be a float");
    if (c->type != Expr::FLOAT)
	err("In expressions of the form 'A[b] = c', c must be a float");
}
    
AssignIndex::~AssignIndex() {}

float
AssignIndex::getFloat() 
{
    float result = c->getFloat();
    SbVec3f val = a->getVec3f();
    int index = int(b->getFloat());
    if (index < 0 || index > 2)
	err("Vector index out of range (must be 0-2, is %d)", index);
    else {
	val[index] = result;
	a->setVec3f(val);
    }
    return result;
}

void BinaryOp::print(int level)
{
#ifdef DEBUG
    printSpace(level);
    printf("BinaryOp: args:\n");
    a->print(level + 1);
    b->print(level + 1);
#endif /* DEBUG */
}

Assign::~Assign() {}
Plus::~Plus() {}
Minus::~Minus() {}
Mult::~Mult() {}
Divide::~Divide() {}
Mod::~Mod() {}
Index::~Index() {}

float Assign::getFloat() { return a->setFloat(b->getFloat()); }
SbVec3f Assign::getVec3f() { return a->setVec3f(b->getVec3f()); }

float Plus::getFloat() { return a->getFloat() + b->getFloat(); }
SbVec3f Plus::getVec3f() { return a->getVec3f() + b->getVec3f(); }

float Minus::getFloat(){ return a->getFloat() - b->getFloat(); }
SbVec3f Minus::getVec3f() { return a->getVec3f() - b->getVec3f(); }

float Mult::getFloat() { return a->getFloat() * b->getFloat(); }
SbVec3f Mult::getVec3f()
{
    return (a->type == FLOAT) ?
	a->getFloat() * b->getVec3f() :
	a->getVec3f() * b->getFloat();
}

float Divide::getFloat() { return a->getFloat() / b->getFloat(); }
SbVec3f Divide::getVec3f() { return a->getVec3f() / b->getFloat(); }

float Mod::getFloat() { return (int) a->getFloat() % (int) b->getFloat(); }

float Index::getFloat() { return a->getVec3f()[(int) b->getFloat()]; }

LessThan::~LessThan() {}
GreaterThan::~GreaterThan() {}
LessEQ::~LessEQ() {}
GreaterEQ::~GreaterEQ() {}
Equals::~Equals() {}
NotEquals::~NotEquals() {}
And::~And() {}
Or::~Or() {}

float LessThan::getFloat() { return a->getFloat() < b->getFloat(); }
float GreaterThan::getFloat() { return a->getFloat() > b->getFloat(); }
float LessEQ::getFloat() { return a->getFloat() <= b->getFloat(); }
float GreaterEQ::getFloat() { return a->getFloat() >= b->getFloat(); }
float Equals::getFloat() { return a->getFloat() == b->getFloat(); }
float NotEquals::getFloat() { return a->getFloat() != b->getFloat(); }
float And::getFloat() { return a->getFloat() && b->getFloat(); }
float Or::getFloat() { return a->getFloat() || b->getFloat(); }

Not::~Not() { delete e; }
float Not::getFloat() { return ! e->getFloat(); }

void Not::print(int level)
{
#ifdef DEBUG
    printSpace(level);
    printf("Not: arg:\n");
    e->print(level + 1);
#endif /* DEBUG */
}

Negate::~Negate() { delete e; }
float Negate::getFloat() { return - e->getFloat(); }
SbVec3f Negate::getVec3f() { return - e->getVec3f(); }

void Negate::print(int level)
{
#ifdef DEBUG
    printSpace(level);
    printf("Negate: arg:\n");
    e->print(level + 1);
#endif /* DEBUG */
}

Ternary::~Ternary() { delete etest; delete etrue; delete efalse; }
Ternary::Ternary(Expr *a, Expr *b, Expr *c)
    : etest(a), etrue(b), efalse(c)
{
    if (etest->type != FLOAT)
	err("Ternary:: conditional expression in ?: operator must be scalar");

    if (etrue->type != efalse->type)
	err("Ternary:: different types across ?: operator (%s and %s)",
		etrue->typeName(), efalse->typeName());
    
    type = etrue->type;
};

float Ternary::getFloat() { return etest->getFloat() ? etrue->getFloat() : efalse->getFloat(); }
SbVec3f Ternary::getVec3f() { return etest->getFloat() ? etrue->getVec3f() : efalse->getVec3f(); }

void Ternary::print(int level)
{
#ifdef DEBUG
    printSpace(level);
    printf("Ternary: conditional + 2 args:\n");
    etest->print(level + 1);
    etrue->print(level + 1);
    efalse->print(level + 1);
#endif /* DEBUG */
}

/******************************************
**
** This is the top-level interface to the parser
**
******************************************/

Parser::Parser(float *(*lf)(void *data, const char *name),
	       SbVec3f *(*lv)(void *data, const char *name),
	       void *d)
{
    lookupFloatField = lf;
    lookupVec3fField = lv;
    data = d;

    elist = new ExprList;
}

Parser::~Parser()
{
    delete elist;
}

SbBool
Parser::parse(const char *buf)
{
    Expr::lookupFloatField = lookupFloatField;
    Expr::lookupVec3fField = lookupVec3fField;
    Expr::data = data;

    Expr::error = FALSE;

    extern SbBool SoCalcParse(ExprList *, const char *);

    if (SoCalcParse(elist, buf))
	return FALSE;

// Enable this to print all parsed expressions
#if DEBUG_PARSED_RESULT
    elist->print();
#endif

    return !Expr::error;
}
