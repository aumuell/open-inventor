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

#ifndef _SO_CALC_EXPR_
#define _SO_CALC_EXPR_

#include <Inventor/SbLinear.h>
#include <Inventor/SbPList.h>

#define Expr SoCalcExpr 
#define ExprList SoCalcExprList 
#define Const SoCalcConst 
#define Var SoCalcVar 
#define Func SoCalcFunc 
#define Func_d SoCalcFunc_d 
#define Func_dd SoCalcFunc_dd 
#define Func_id SoCalcFunc_id 
#define Func_v SoCalcFunc_v 
#define Funcv_v SoCalcFuncv_v 
#define Func_vv SoCalcFunc_vv 
#define Funcv_vv SoCalcFuncv_vv 
#define Funcv_ddd SoCalcFuncv_ddd 
#define BinaryOp SoCalcBinaryOp 
#define Assign SoCalcAssign 
#define AssignIndex SoCalcAssignIndex
#define Plus SoCalcPlus 
#define Minus SoCalcMinus 
#define Mult SoCalcMult 
#define Divide SoCalcDivide 
#define Mod SoCalcMod 
#define Index SoCalcIndex 
#define LessThan SoCalcLessThan 
#define GreaterThan SoCalcGreaterThan 
#define LessEQ SoCalcLessEQ 
#define GreaterEQ SoCalcGreaterEQ 
#define Equals SoCalcEquals 
#define NotEquals SoCalcNotEquals 
#define And SoCalcAnd 
#define Or SoCalcOr 
#define Not SoCalcNot 
#define Negate SoCalcNegate 
#define Ternary SoCalcTernary 
#define Parser SoCalcParser 

//
// base expression class
//
class Expr {
  public:
    enum Type {
	FLOAT,
	VEC3F
    };
    Type	type;

    Expr() {}
    Expr(Type t) : type(t) {}
    virtual ~Expr();

    virtual void	print(int level);
    virtual float	getFloat();
    virtual SbVec3f	getVec3f();
    virtual float	setFloat(float v);
    virtual SbVec3f	setVec3f(const SbVec3f &);
    virtual void	eval();

    const char *typeName()
    {
	return type==FLOAT ? "float" : "Vec3f";
    }

  protected:
    friend class Parser;
    // global flag gets set true if there was an error
    static SbBool error;

    // Functions to look up data storage locations by name.
    // These should be set by the client before creating a parse tree.
    static float *(*lookupFloatField)(void *data, const char *name);
    static SbVec3f *(*lookupVec3fField)(void *data, const char *name);
    static void *data;

    void printSpace(int level);

  protected:
    // Use this like printf...
    void err(const char *formatString ...);
};

//
// List of expressions
//
class ExprList : public SbPList {
  public:
    ExprList() {}
    ExprList(const ExprList &l) { copy(l); }
    ~ExprList() { truncate(0); }

    void truncate(int n);
    void append(Expr *e) { SbPList::append((void*) e); }
    Expr *operator[](int i) const
	{ return (Expr*) (*(const SbPList*)this)[i]; }

    // evaluate each expression in the list.
    void eval();

    void	print();
};


//
// Terminal expressions
//
class Const : public Expr {
  public:
    const float	val;
    Const(float v) : Expr(FLOAT), val(v) {}
    ~Const();
    virtual void print(int level);
    virtual float getFloat();
};

class Var : public Expr {
  public:
    const char		*const name;
    Var(const char *nm, Type type);
    ~Var();
    virtual void	print(int level);
    virtual float	getFloat();
    virtual SbVec3f	getVec3f();
    virtual float	setFloat(float v);
    virtual SbVec3f	setVec3f(const SbVec3f &v);
  private:
    union {
	float	*pfloat;
	SbVec3f	*pvec3f;
    };
};

//
// Functions
//
class Func : public Expr
{
  public:
    const char *const name;
    virtual Func *dup() const = 0;
    ~Func();
    virtual void setArgs(ExprList *args) = 0;
    virtual void print(int level);
  protected:
    Func(const char *nm, Type type);
    ExprList	*args;
};

class Func_d : public Func
{
  public:
    Func_d(const char *name, double (*f)(double));
    ~Func_d();
    virtual Func *dup() const;
    virtual float getFloat();
    virtual void setArgs(ExprList *args);
  private:
    double	(*const func)(double);
};

class Func_dd : public Func
{
  public:
    Func_dd(const char *name, double (*f)(double, double)) : Func(name, FLOAT), func(f) {}
    ~Func_dd();
    virtual Func *dup() const;
    virtual float getFloat();
    virtual void setArgs(ExprList *args);
  private:
    double	(*const func)(double, double);
};

class Func_id : public Func
{
  public:
    Func_id(const char *name, double (*f)(int, double)) : Func(name, FLOAT), func(f) {}
    ~Func_id();
    virtual Func *dup() const;
    virtual float getFloat();
    virtual void setArgs(ExprList *args);
  private:
    double	(*const func)(int, double);
};

class Func_v : public Func
{
  public:
    Func_v(const char *name, double (*f)(const SbVec3f &)) : Func(name, FLOAT), func(f) {}
    ~Func_v();
    virtual Func *dup() const;
    virtual float getFloat();
    virtual void setArgs(ExprList *args);
  private:
    double	(*const func)(const SbVec3f &);
};

class Funcv_v : public Func
{
  public:
    Funcv_v(const char *name, SbVec3f (*f)(const SbVec3f &)) : Func(name, VEC3F), func(f) {}
    ~Funcv_v();
    virtual Func *dup() const;
    virtual SbVec3f getVec3f();
    virtual void setArgs(ExprList *args);
  private:
    SbVec3f	(*const func)(const SbVec3f &);
};

class Func_vv : public Func
{
  public:
    Func_vv(const char *name, double (*f)(const SbVec3f &, const SbVec3f &)) : Func(name, FLOAT), func(f) {}
    ~Func_vv();
    virtual Func *dup() const;
    virtual float getFloat();
    virtual void setArgs(ExprList *args);
  private:
    double	(*const func)(const SbVec3f &, const SbVec3f &);
};

class Funcv_vv : public Func
{
  public:
    Funcv_vv(const char *name, SbVec3f (*f)(const SbVec3f &, const SbVec3f &)) : Func(name, VEC3F), func(f) {}
    ~Funcv_vv();
    virtual Func *dup() const;
    virtual SbVec3f getVec3f();
    virtual void setArgs(ExprList *args);
  private:
    SbVec3f	(*const func)(const SbVec3f &, const SbVec3f &);
};

class Funcv_ddd : public Func
{
  public:
    Funcv_ddd(const char *name, SbVec3f (*f)(double, double, double)) : Func(name, VEC3F), func(f) {}
    ~Funcv_ddd();
    virtual Func *dup() const;
    virtual SbVec3f getVec3f();
    virtual void setArgs(ExprList *args);
  private:
    SbVec3f	(*const func)(double, double, double);
};

//
// Operations
//
class BinaryOp : public Expr
{
  public:
    virtual void print(int level);
  protected:
    // bit mask to select allowed operation types
    enum Types { FF=(1<<0), FV=(1<<1), VF=(1<<2), VV=(1<<3) };
    BinaryOp(Expr *ea, Expr *eb, int TypeBits);
    ~BinaryOp();
    Expr	*a, *b;
};

class Assign : public BinaryOp
{
  public:
    Assign(Expr *a, Expr *b) : BinaryOp(a,b,FF|VV) { type=a->type; }
    ~Assign();
    virtual float getFloat();
    virtual SbVec3f getVec3f();
};

class AssignIndex : public Expr
{
  public:
    // Expression is "a[b] = c"
    AssignIndex(Expr *a, Expr *b, Expr *c);
    ~AssignIndex();
    virtual float getFloat();
  private:
    Expr *a, *b, *c;
};

class Plus : public BinaryOp
{
  public:
    Plus(Expr *a, Expr *b) : BinaryOp(a,b,FF|VV) { type=a->type; }
    ~Plus();
    virtual float getFloat();
    virtual SbVec3f getVec3f();
};

class Minus : public BinaryOp
{
  public:
    Minus(Expr *a, Expr *b) : BinaryOp(a,b,FF|VV) { type=a->type; }
    ~Minus();
    virtual float getFloat();
    virtual SbVec3f getVec3f();
};

class Mult : public BinaryOp
{
  public:
    Mult(Expr *a, Expr *b) : BinaryOp(a,b,FF|FV|VF) { type=a->type==b->type?a->type:VEC3F; }
    ~Mult();
    virtual float getFloat();
    virtual SbVec3f getVec3f();
};

class Divide : public BinaryOp
{
  public:
    Divide(Expr *a, Expr *b) : BinaryOp(a,b,FF|VF) { type=a->type; }
    ~Divide();
    virtual float getFloat();
    virtual SbVec3f getVec3f();
};

class Mod : public BinaryOp
{
  public:
    Mod(Expr *a, Expr *b) : BinaryOp(a,b,FF) { type=FLOAT; }
    ~Mod();
    virtual float getFloat();
};

class Index : public BinaryOp
{
  public:
    Index(Expr *a, Expr *b) : BinaryOp(a,b,VF) { type=FLOAT; }
    ~Index();
    virtual float getFloat();
};

class LessThan : public BinaryOp
{
  public:
    LessThan(Expr *a, Expr *b) : BinaryOp(a,b,FF) { type=FLOAT; }
    ~LessThan();
    virtual float getFloat();
};

class GreaterThan : public BinaryOp
{
  public:
    GreaterThan(Expr *a, Expr *b) : BinaryOp(a,b,FF) { type=FLOAT; }
    ~GreaterThan();
    virtual float getFloat();
};

class LessEQ : public BinaryOp
{
  public:
    LessEQ(Expr *a, Expr *b) : BinaryOp(a,b,FF) { type=FLOAT; }
    ~LessEQ();
    virtual float getFloat();
};

class GreaterEQ : public BinaryOp
{
  public:
    GreaterEQ(Expr *a, Expr *b) : BinaryOp(a,b,FF) { type=FLOAT; }
    ~GreaterEQ();
    virtual float getFloat();
};

class Equals : public BinaryOp
{
  public:
    Equals(Expr *a, Expr *b) : BinaryOp(a,b,FF) { type=FLOAT; }
    ~Equals();
    virtual float getFloat();
};

class NotEquals : public BinaryOp
{
  public:
    NotEquals(Expr *a, Expr *b) : BinaryOp(a,b,FF) { type=FLOAT; }
    ~NotEquals();
    virtual float getFloat();
};

class And : public BinaryOp
{
  public:
    And(Expr *a, Expr *b) : BinaryOp(a,b,FF) { type=FLOAT; }
    ~And();
    virtual float getFloat();
};

class Or : public BinaryOp
{
  public:
    Or(Expr *a, Expr *b) : BinaryOp(a,b,FF) { type=FLOAT; }
    ~Or();
    virtual float getFloat();
};

class Not : public Expr
{
  public:
    Not(Expr *exp) : e(exp) { type=FLOAT;}
    ~Not();
    virtual float getFloat();
    virtual void print(int level);
  private:
    Expr *e;
};

class Negate : public Expr
{
  public:
    Negate(Expr *exp) : e(exp) { type=e->type; }
    ~Negate();
    virtual float getFloat();
    virtual SbVec3f getVec3f();
    virtual void print(int level);
  private:
    Expr *e;
};

class Ternary : public Expr
{
  public:
    Ternary(Expr *a, Expr *b, Expr *c);
    ~Ternary();
    virtual float getFloat();
    virtual SbVec3f getVec3f();
    virtual void print(int level);
  private:
    Expr *etest;
    Expr *etrue;
    Expr *efalse;
};

//
// Top-level parser class
//
class Parser {
  public:
    Parser(float *(*lookupFloatField)(void *data, const char *name),
	 SbVec3f *(*lookupVec3fField)(void *data, const char *name),
	void *data);
    ~Parser();

    // parse some more expressions, add to the collection
    SbBool	parse(const char *buf);

    // evaluate the collection of expressions
    void	eval() { elist->eval(); }

    // empty the collection of parsed expressions
    void	clear() { elist->truncate(0); }

  private:
    float *(*lookupFloatField)(void *data, const char *name);
    SbVec3f *(*lookupVec3fField)(void *data, const char *name);
    void *data;

    ExprList	*elist;
};

#endif
