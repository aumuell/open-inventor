/*
 * Copyright (C) 1990,91   Silicon Graphics, Inc.
 *
 _______________________________________________________________________
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 |
 |   $Revision: 1.1.1.1 $
 |
 |   Classes:
 |      ???
 |
 |   Author(s)          : Ronen Barzel
 |
 ______________  S I L I C O N   G R A P H I C S   I N C .  ____________
 _______________________________________________________________________
 */

%{
#include <Inventor/misc/SoBasic.h>
#include <Inventor/errors/SoDebugError.h>
#include "SoCalcExpr.h"
#include <stdio.h>
#include <values.h>
#include <math.h>
#include <ctype.h>
#include <strings.h>
#include <stdlib.h>

void yyerror(const char *);
extern "C" int	yylex();
int yyparse();

static const char *In;
static ExprList	*EList;

static const struct {
    const char *name;
    float	val;
} Constants[] = {
    { "MAXFLOAT",	MAXFLOAT },
    { "MINFLOAT",	MINFLOAT },
    { "M_E",		M_E },
    { "M_LOG2E",	M_LOG2E },
    { "M_LOG10E",	M_LOG10E },
    { "M_LN2",		M_LN2 },
    { "M_LN10",		M_LN10 },
    { "M_PI",		M_PI },
    { "M_SQRT2",	M_SQRT2 },
    { "M_SQRT1_2",	M_SQRT1_2 },
};
#define NCONSTANTS (sizeof(Constants)/sizeof(Constants[0]))

static const struct {
    const char *name;
    Expr::Type	type;
} Inputs[] = {
    { "a", Expr::FLOAT}, { "b", Expr::FLOAT},
    { "c", Expr::FLOAT}, { "d", Expr::FLOAT},
    { "e", Expr::FLOAT}, { "f", Expr::FLOAT},
    { "g", Expr::FLOAT}, { "h", Expr::FLOAT},
    { "A", Expr::VEC3F}, { "B", Expr::VEC3F},
    { "C", Expr::VEC3F}, { "D", Expr::VEC3F},
    { "E", Expr::VEC3F}, { "F", Expr::VEC3F},
    { "G", Expr::VEC3F}, { "H", Expr::VEC3F},
}, Outputs[] = {
    { "oa", Expr::FLOAT}, { "ob", Expr::FLOAT},
    { "oc", Expr::FLOAT}, { "od", Expr::FLOAT},
    { "oA", Expr::VEC3F}, { "oB", Expr::VEC3F},
    { "oC", Expr::VEC3F}, { "oD", Expr::VEC3F},
}, Vars[] = {
    { "ta", Expr::FLOAT}, { "tb", Expr::FLOAT},
    { "tc", Expr::FLOAT}, { "td", Expr::FLOAT},
    { "te", Expr::FLOAT}, { "tf", Expr::FLOAT},
    { "tg", Expr::FLOAT}, { "th", Expr::FLOAT},
    { "tA", Expr::VEC3F}, { "tB", Expr::VEC3F},
    { "tC", Expr::VEC3F}, { "tD", Expr::VEC3F},
    { "tE", Expr::VEC3F}, { "tF", Expr::VEC3F},
    { "tG", Expr::VEC3F}, { "tH", Expr::VEC3F},
};
#define NINPUTS (sizeof(Inputs)/sizeof(Inputs[0]))
#define NOUTPUTS (sizeof(Outputs)/sizeof(Outputs[0]))
#define NVARS (sizeof(Vars)/sizeof(Vars[0]))

static double rand(double max) { return max * drand48(); }
static SbVec3f cross(const SbVec3f &a, const SbVec3f &b) { return a.cross(b); }
static double dot(const SbVec3f &a, const SbVec3f &b) { return a.dot(b); }
static double length(const SbVec3f &a) { return a.length(); }
static SbVec3f normalize(const SbVec3f &v) { SbVec3f t=v; t.normalize(); return t; }
static SbVec3f vec3f(double a, double b, double c) { return SbVec3f(a,b,c); }

// Keep this up to date with the info in initFuncs()
#define NFUNCS 25

static struct {
    const Func *func;
} Funcs[NFUNCS];

%}

%token LEXERR
%token <expr> CONST FUNC INPUT OUTPUT VAR
%token OROR ANDAND GEQ LEQ EQEQ NEQ

%union
{
    Expr	*expr;
    ExprList	*list;
}

%type <expr> asgn primary_expression postfix_expression
%type <expr> unary_expression
%type <expr> multiplicative_expression additive_expression
%type <expr> additive_expression relational_expression
%type <expr> equality_expression logical_AND_expression
%type <expr> logical_OR_expression conditional_expression
%type <list> args

%%

asgnlist:
	asgn { EList->append($1); }
	| asgnlist  ';' asgn { EList->append($3); }
	| asgnlist  ';'
	;

asgn:
	OUTPUT '=' conditional_expression
				{ $$ = new Assign($1, $3); }
	| OUTPUT '[' conditional_expression ']' '=' conditional_expression
				{ $$ = new AssignIndex($1, $3, $6); }
	| VAR	'=' conditional_expression
				{ $$ = new Assign($1, $3); }
	| VAR '[' conditional_expression ']' '=' conditional_expression
				{ $$ = new AssignIndex($1, $3, $6); }
	;

primary_expression:
	CONST	 			{ $$ = $1; }
	| INPUT				{ $$ = $1; }
	| OUTPUT			{ $$ = $1; }
	| VAR				{ $$ = $1; }
        | '(' conditional_expression ')'	{ $$ = $2; }
        ;

postfix_expression:
        primary_expression	{ $$ = $1; }
        | postfix_expression '[' conditional_expression ']'
			 	{ $$ = new Index($1, $3); }
	| FUNC '(' args ')'	{ ((Func *)$1)->setArgs($3); $$ = $1; }
        ;

args	:
	conditional_expression		{ ($$ = new ExprList)->append($1); }
	| args ',' conditional_expression { $1->append($3); $$ = $1; }
	;

unary_expression:
        postfix_expression
        | '-' unary_expression	{ $$ = new Negate($2); }
        | '!' unary_expression	{ $$ = new Not($2); }
        ;

multiplicative_expression:
        unary_expression
        | multiplicative_expression '*' unary_expression
			{ $$ = new Mult($1, $3); }
        | multiplicative_expression '/' unary_expression
			{ $$ = new Divide($1, $3); }
        | multiplicative_expression '%' unary_expression
			{ $$ = new Mod($1, $3); }
        ;

additive_expression:
        multiplicative_expression
        | additive_expression '+' multiplicative_expression
			{ $$ = new Plus($1, $3); }
        | additive_expression '-' multiplicative_expression
			{ $$ = new Minus($1, $3); }
        ;

relational_expression:
        additive_expression
        | relational_expression '<' additive_expression
			{ $$ = new LessThan($1, $3); }
        | relational_expression '>' additive_expression
			{ $$ = new GreaterThan($1, $3); }
        | relational_expression LEQ additive_expression
			{ $$ = new LessEQ($1, $3); }
        | relational_expression GEQ additive_expression
			{ $$ = new GreaterEQ($1, $3); }
        ;

equality_expression:
        relational_expression
        | equality_expression EQEQ relational_expression
			{ $$ = new Equals($1, $3); }
        | equality_expression NEQ relational_expression
			{ $$ = new NotEquals($1, $3); }
        ;

logical_AND_expression:
        equality_expression
        | logical_AND_expression ANDAND equality_expression
			{ $$ = new And($1, $3); }
        ;

logical_OR_expression:
        logical_AND_expression
        | logical_OR_expression OROR logical_AND_expression
			{ $$ = new Or($1, $3); }
        ;

conditional_expression:
        logical_OR_expression
        | logical_OR_expression '?' conditional_expression ':'
                conditional_expression
			{ $$ = new Ternary($1, $3, $5); }
        ;

%%

//
// The static Funcs array contains instances of objects that have
// constructors. Since this wouldn't get set up correctly in a DSO, we
// have to initialize the array at run-time.
//

static void
initFuncs()
{
    int	i = 0;

#define MAKEFUNC(CLASS, NAME)						      \
	Funcs[i++].func = new CLASS(SO__QUOTE(NAME),NAME)

    MAKEFUNC(Func_d, acos);
    MAKEFUNC(Func_d, asin);
    MAKEFUNC(Func_d, atan);
    MAKEFUNC(Func_dd, atan2);
    MAKEFUNC(Func_d, ceil);
    MAKEFUNC(Func_d, cos);
    MAKEFUNC(Func_d, cosh);
    MAKEFUNC(Funcv_vv, cross);
    MAKEFUNC(Func_vv, dot);
    MAKEFUNC(Func_d, exp);
    MAKEFUNC(Func_d, fabs);
    MAKEFUNC(Func_d, floor);
    MAKEFUNC(Func_dd, fmod);
    MAKEFUNC(Func_v, length);
    MAKEFUNC(Func_d, log);
    MAKEFUNC(Func_d, log10);
    MAKEFUNC(Funcv_v, normalize);
    MAKEFUNC(Func_dd, pow);
    MAKEFUNC(Func_d, rand);
    MAKEFUNC(Func_d, sin);
    MAKEFUNC(Func_d, sinh);
    MAKEFUNC(Func_d, sqrt);
    MAKEFUNC(Func_d, tan);
    MAKEFUNC(Func_d, tanh);
    MAKEFUNC(Funcv_ddd, vec3f);

#ifdef DEBUG
    // Sanity check
    if (i != NFUNCS)
	SoDebugError::post("SoCalcParse initFuncs (internal)",
			   "Wrong number of functions declared");
#endif /* DEBUG */
}

static Const *
isConst(const char *nm)
{
    for (int i=0; i<NCONSTANTS; i++)
	if (strcmp(nm, Constants[i].name)==0)
	    return new Const(Constants[i].val);
    return NULL;
}

static Func *
isFunc(const char *nm)
{
    for (int i=0; i<NFUNCS; i++) {
	const Func *f = Funcs[i].func;
	if (strcmp(nm, f->name)==0)
	    return f->dup();
    }
    return NULL;
}

static Var *
isInput(const char *nm)
{
    for (int i=0; i<NINPUTS; i++) if (strcmp(nm, Inputs[i].name)==0) 
	return new Var(nm, Inputs[i].type);
    return NULL;
}

static Var *
isOutput(const char *nm)
{
    for (int i=0; i<NOUTPUTS; i++) if (strcmp(nm, Outputs[i].name)==0) 
	return new Var(nm, Outputs[i].type);
    return NULL;
}

static Var *
isVar(const char *nm)
{
    for (int i=0; i<NVARS; i++) if (strcmp(nm, Vars[i].name)==0) 
	return new Var(nm, Vars[i].type);
    return NULL;
}

int yylex()
{
#define BSZ 100
    char c, buf[BSZ+1], *cp = buf;

    // skip over blanks
    while (isspace(*In))
	In++;

    if (!In[0])
	return EOF;

    if (isdigit(In[0]) || In[0] == '.') {

	// skip past a valid floating-point number
	// (don't need to compute the number, will
	// use atof() to do that).
	SbBool dot = FALSE;
	SbBool exp = FALSE;
	for (;;) {
	    c = *In++;
	    if (cp - buf < BSZ)
		*cp++ = c;
	    *cp = c;
	    if (isdigit(c))
		continue;
	    if (c== '.') {
		if (dot || exp)
		    return LEXERR;
		dot = TRUE;
		continue;
	    }
	    if (c == 'e' || c == 'E') {
		if (exp)
		    return LEXERR;
		exp = TRUE;
		continue;
	    }
	    // Check for negative exponent
	    if (c == '-' && exp && (In[-2] == 'e' || In[-2] == 'E')) {
		continue;
	    }
	    break;	// end of number
	}
	if (In[-1] == 'e')
	    return LEXERR;

	*cp = 0;
	In--;	// push back last character "read"

	yylval.expr = new Const(atof(buf));
	return CONST;
    }

    if (isalpha(In[0])) {
	while (isalnum(In[0]) || In[0] == '_') {
	    if (cp - buf < BSZ)
		*cp++ = *In++;
	}
	*cp = 0;

	if (yylval.expr = isConst(buf))
	    return CONST;

	if (yylval.expr = isFunc(buf))
	    return FUNC;

	if (yylval.expr = isInput(buf))
	    return INPUT;

	if (yylval.expr = isOutput(buf))
	    return OUTPUT;

	if (yylval.expr = isVar(buf))
	    return VAR;

    }

#define EQ2(x,y)	(x[0]==y[0] && x[1]==y[1])
    if (EQ2(In, "==")) { In += 2; return EQEQ; }
    if (EQ2(In, "!=")) { In += 2; return NEQ; }
    if (EQ2(In, ">=")) { In += 2; return GEQ; }
    if (EQ2(In, "<=")) { In += 2; return LEQ; }
    if (EQ2(In, "&&")) { In += 2; return ANDAND; }
    if (EQ2(In, "||")) { In += 2; return OROR; }

    return *In++;
}

void
yyerror(const char *)
{
    // printf("parse error: %s\n", s);
}

SbBool
SoCalcParse(ExprList *elist, const char *buf)
{
    static SbBool initted = FALSE;

    if (! initted) {
	initFuncs();
	initted = TRUE;
    }

    In = buf;
    EList = elist;
    return(yyparse());
}

#ifdef TESTMAIN

void
main()
{
    char buf[1000];
#if YYDEBUG
    extern int yydebug;
    yydebug = 1;
#endif
    EList = new ExprList;

    while (gets(buf)) {
	In = buf;
	EList->truncate(0);
	if (yyparse())
	    printf("Parse error.\n");
	else {
	    EList->eval();
	    for (int i=0; i<EList->length(); i++) {
		(*EList)[i]->pr();
	    }
	}
    }
}

#endif
