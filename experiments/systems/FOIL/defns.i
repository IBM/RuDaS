#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <sys/types.h>


/******************************************************************************/
/*									      */
/*	Type definitions, including synonyms for structure pointers	      */
/*									      */
/******************************************************************************/


typedef unsigned char Var;

typedef char	Boolean,
		Ordering;	/* valid instances are <,=,>,# */


				/* A tuple is stored as an array of int / fp.

				   * T[0] = tuple number
						+ POSMARK (pos tuples)
				   * T[i] = constant number, or fp value

				   NB: If continuous (fp) values are used,
				       type Const must be the same size as
				       float --if this is not the case, change
				       Const to long.

				   Relation tuple sets are indexed by 3D array.

				   * I[i][j][k] = no. of kth tuple with T[j] = i

				   The last entry is followed by FINISH  */


typedef int			Const,
				*Tuple;

typedef int			***Index;

typedef struct _rel_rec		*Relation;

typedef struct _type_rec	*TypeInfo;

typedef struct _state_rec	State;

typedef struct _lit_rec		*Literal,
				**Clause;

typedef struct _arg_ord_rec	*ArgOrder;

typedef struct _poss_lit_rec	*PossibleLiteral;

typedef struct _backup_rec	*Alternative;

typedef struct _var_rec		*VarInfo;


/******************************************************************************/
/*									      */
/*	Structure definitions						      */
/*									      */
/******************************************************************************/

	/* State */

	/*  A state represents information about the search for a new
	    clause, including the tuples that satisfy the clause and
	    various counts for the clause and the tuples  */

struct _state_rec
{
	int		MaxVar,		/* highest variable */
			NPos,		/* number of pos tuples */
			NTot,		/* number of all tuples */
			NOrigPos,	/* original pos tuples covered */
			NOrigTot;	/* original tuples covered */
	Tuple		*Tuples;	/* training set */
	float		BaseInfo;	/* information per pos tuple */
};

	/* Literal */

struct _lit_rec
{
	char		Sign;		/* 0=negated, 1=pos, 2=determinate */
	Relation	Rel;
	Var		*Args;
	int		WeakLits;	/* value up to this literal */
	Ordering	*ArgOrders,	/* recursive lits: =, <, >, # */
			*SaveArgOrders;	/* copy during pruning */
	float		Bits;		/* encoding length */
};

	/* Relation */

	/* Note:

	   Relations are represented by the (positive) tuples they contain;
	   if the closed world assumption is not in force, negative tuples
	   known not to be in the relation can be given explicitly.

	   A key specifies a sensible way that a relation can be accessed
	   by noting which arguments must have bound values.  There can
	   be any number of keys; if there are none, all possible ways
	   of accessing the relation are ok. */
	   
struct _rel_rec
{
	char		*Name;
	int		Arity,		/* number of arguments */
			NKeys,		/* number of keys (0=all ok) */
			*Key,		/* keys, each packed in an int */
			*Type;		/* types of arguments */
	TypeInfo	*TypeRef;	/* redundant pointers to types */
	Tuple		*Pos,		/* positive tuples */
			*Neg;		/* negative tuples or Nil (CWA) */
	Index		PosIndex,	/* index for positive tuples */
			NegIndex;	/* ditto for explicit negative tuples */
	int		PosDuplicates,	/* number of duplicate pos tuples*/
			NTrialArgs,	/* number of args to try (gain) */
			NTried;		/* number of them evaluated */
	Clause		*Def;		/* definition is array of clauses */
	Boolean		BinSym,		/* true for binary symmetric relns */
			PossibleTarget,
			**PosOrder,	/* argument order info for R() */
			**NegOrder,	/*    "       "    "    " ~R() */
			*ArgNotEq;	/* args that cannot be the same var
					   (indexed by ArgPair) */
	float		Bits;		/* current encoding cost */
};

	/* Type */

struct _type_rec
{	char		*Name;		/* type name */
	Boolean		Continuous,	/* continuous (non-discrete) */
			Ordered,	/* ordered discrete type */
			FixedPolarity;
	int		NValues,	/* number of discrete constants */
			NTheoryConsts;  /* number of theory constants */
	Const		*Value,		/* constants */
			*TheoryConst;	/* theory constants */
	int		*CollSeq;	/* CollSeq[k] = x if (global) constant
					   k is x'th const of this type */
};


	/* Possible argument order -- used in discovering constant orders */

struct _arg_ord_rec
{
	Relation	Rel;		/* relation */
	Boolean		Sign;		/* sign */
	int		A1, A2,		/* A1 < A2 or A1 > A2 */
			In;		/* 0, -1 (reverse) or +1 */
};


	/* Structures used in backing up search */

struct _poss_lit_rec
{
	Relation	Rel;
	Boolean		Sign;
	Var		*Args;
	float		Gain,
			Bits;
	int		WeakLits,
			NewSize,
			TotCov,
			PosCov;
};

struct _backup_rec
{
	float		Value;
	Clause		UpToHere;
};


	/* Variables */

struct _var_rec
{
	char		*Name;
	int		Type,
			Depth;
	TypeInfo	TypeRef;
}; 


#define  FP(X)			(*((float *)(&X)))

	/*  The first four relations are predefined comparisons with
	    aliased names defined here  */

#define  EQVAR			Reln[0]		/* var = var */
#define  EQCONST		Reln[1]		/* var = theory const */
#define  GTVAR			Reln[2]		/* var > var */
#define  GTCONST		Reln[3]		/* var > threshold */

#define  Predefined(R)		(R==EQVAR||R==EQCONST||R==GTVAR||R==GTCONST)
#define  HasConst(R)		(R==EQCONST||R==GTCONST)

#define  Verbose(x)		if (VERBOSITY >= x)

#define  LN2	 		0.693147
#define  Log2(x)		(log((float) x)/LN2)
#define  Log2e			1.44269
#define  Log2sqrt2Pi		1.32575
#define  LogComb(n,r)		(Log2Fact(n) - Log2Fact(r) - Log2Fact((n)-(r)))
#define  Except(n,e)		((n) ? (1.1*(Log2(n) + LogComb(n, e))) : 0.0)
#define  Encode(n)		Except(AllTuples, n)

#define  Nil			0
#define  false			0 
#define  true			1 
#define  Max(a,b)		( (a)>(b) ? a : b ) 
#define  Min(a,b)		( (a)<(b) ? a : b ) 

#define  ForEach(V,First,Last)	for(V=First;V<=Last;V++) 

#define  Mask			 077777777
#define  PosMark		0100000000	/* Max tuples = 16.7M */
#define  Positive(T)		((T)[0]&PosMark)

#define  TrueBit		02
#define  FalseBit		01
#define  SetFlag(A,B)		Flags[A] |= B
#define  TestFlag(A,B)		(Flags[A] & B)
#define  ClearFlags		memset(Flags,0,StartDef.NTot)

#define  BestLitGain		(NPossible ? Possible[1]->Gain : 0.0)

#define  MonitorWeakLits(W)	if (W) NWeakLits++; else NWeakLits=0

#define  Plural(n)		((n) != 1 ? "s" : "")
#define  ReadToEOLN		while ( getchar() != '\n' )

#define  Alloc(N,T)		(T *) pmalloc((N)*sizeof(T))
#define  AllocZero(N,T)		(T *) pcalloc(N, sizeof(T))
#define  Realloc(V,N,T)		V = (T *) prealloc(V, (N)*sizeof(T))

#define  MissingValue(R,A,X)	(MissingVals && MissingVal(R,A,X))
#define  ungetchar(A)		ungetc(A, stdin)

#define	 ArgPair(A2,A1)		(((A2-1)*(A2-2))/2 + A1-1)

	/*  The following are used to pack and unpack parameters into
	    argument lists.  AV must be the address of an int or float  */

#define  SaveParam(A,AV)	memcpy(A,AV,sizeof(Const))
#define  GetParam(A,AV)		memcpy(AV,A,sizeof(Const))



/******************************************************************************/
/*									      */
/*	Various constants						      */
/*									      */
/******************************************************************************/


#define  FINISH	   10000000	/* large constant used as a terminator */

#define	 UNBOUND    0357357	/* odd marker used in interpret.c, join.c */

#define  MISSING_DISC     1	/* missing value "?" is the first constant */

#define  MISSING_FP 0.03125	/* arbitrary number used as the floating
				   point equivalent of MISSING_DISC - if
				   it clashes with a genuine data value, just
				   change this */

#define	 OUT_OF_RANGE	  2	/* denotes constant outside closed world */



/******************************************************************************/
/*									      */
/*	Synopsis of functions						      */
/*									      */
/******************************************************************************/


	/* main.c */

void	  main(int Argc, char *Argv[]);

	/* utility.c */

void	  *pmalloc(unsigned arg);
void	  *prealloc(void * arg1, unsigned arg2);
void	  *pcalloc(unsigned arg1, unsigned arg2);
void	  pfree(void *arg);
float	  CPUTime();

	  /* input.c */

Boolean	  ReadType();
void	  ReadTypes();
Tuple	  ReadTuple(Relation R);
Tuple	  *ReadTuples(Relation R, Boolean Pos);
Relation  ReadRelation();
void	  ReadRelations();
int	  FindType(char *N);
char	  *CopyString(char *s);
void	  Error();
void	  DuplicateTuplesCheck(Relation R);
int	  CountDuplicates(Tuple *T, int N, int Left, int Right);
Boolean	  SymmetryCheck(Relation R);
char	  ReadName(char *s);
Const	  FindConstant(char *N, Boolean MustBeThere);
int	  Number(Tuple *T);
void	  CheckTypeCompatibility();
Boolean	  CommonValue(int N1, Const *V1, int N2, Const *V2);
Index	  MakeIndex(Tuple *T, int N, Relation R);
void	  UnequalArgsCheck(Relation R);
Boolean	  NeverEqual(Tuple *T, Var F, Var S);

	  /* output.c */

void	  PrintTuple(Tuple C, int N, TypeInfo *TypeRef, Boolean ShowPosNeg);
void	  PrintTuples(Tuple *TT, int N);
void	  PrintSpecialLiteral(Relation R, Boolean RSign, Var *A);
void	  PrintComposedLiteral(Relation R, Boolean RSign, Var *A);
void	  PrintLiteral(Literal L);
void	  PrintClause(Relation R, Clause C);
void	  PrintSimplifiedClause(Relation R, Clause C);
void	  Substitute(char *Old, char *New);
void	  PrintDefinition(Relation R);

	  /* literal.c */

void	  ExploreArgs(Relation R, Boolean CountFlag);
Boolean	  AcceptableKey(Relation R, int Key);
Boolean   Repetitious(Relation R, Var *A);
Boolean   SameArgs(int N, Var *A1, int MV1, Var *A2, int MV2, int LitN);
void	  ExploreEQVAR();
void	  ExploreEQCONST();
void	  ExploreGTVAR();
void	  ExploreGTCONST();
Boolean	  TryArgs(Relation R, int This, int HiVar, int FreeVars, int MaxDepth,
	  	    int Key, Boolean TryMostGeneral, Boolean RecOK);
int	  EstimatePossibleArgs(int TNo);

	  /* evaluatelit.c */

void	  EvaluateLiteral(Relation R, Var *A, float LitBits, Boolean *Prune);
void	  PrepareForScan(Relation R, Var *A);
float	  NegThresh(int P, int P1);
Boolean	  TerminateScan(Relation R, Var *A);
Boolean	  Satisfies(int RN, Const V, Const W, Tuple Case);
void	  CheckForPrune(Relation R, Var *A);
void	  CheckNewVars(Tuple Case);
float	  Worth(int N, int P, int T, int UV);
float	  Info(int P, int T);
Boolean	  MissingVal(Relation R, Var *A, Tuple T);
Boolean	  Unknown(Var V, Tuple T);
void	  FindThreshold(Var *A);
void	  PossibleCut(float C);
int	  MissingAndSort(Var V, int Fp, int Lp);
void	  Quicksort(Tuple *Vec, int Fp, int Lp, Var V);

	  /* join.c */

Boolean	  Join(Tuple *T, Index TIX, Var *A, Tuple C, int N, Boolean YesOrNo);

	  /* state.c */

void	  OriginalState(Relation R);
void	  AddTuple(int N, Tuple T, int ByteSize, int Mark);
Tuple	  NextConstTuple(Relation R, Tuple Case);
void	  RandomTuple(Relation R, Tuple Result);
void	  NewState(Literal L, int NewSize);
void	  FormNewState(Relation R, Boolean RSign, Var *A, int NewSize);
void	  AcceptNewState(Relation R, Var *A, int NewSize);
void	  RecoverState(Clause C);
void	  CheckSize(int SoFar, int Extra, int *NewSize, Tuple **TSAddr);
Tuple	  Extend(Tuple Case, Tuple Binding, Var *A, int N);
void	  CheckOriginalCaseCover();
void	  FreeTuples(Tuple *TT, Boolean TuplesToo);
double	  Log2Fact(int n);

	  /* determinate.c */

Boolean	  GoodDeterminateLiteral(Relation R, Var *A, float LitBits);
void	  ProcessDeterminateLiterals(Boolean AllWeak);
Boolean	  SameVar(Var A, Var B);
void	  ShiftVarsDown(int s);

	  /* search.c */

void	  ProposeLiteral(Relation R, Boolean TF, Var *A,
	  		   int Size, float LitBits, int OPos, int OTot,
	  		   float Gain, Boolean Weak);
Boolean	  Recover();
void	  Remember(Literal L, int OPos, int OTot);
Literal	  MakeLiteral(int i);
Literal	  SelectLiteral();
void	  FreeLiteral(Literal L);
void	  FreeClause(Clause C);

	  /* order.c */

void	  ExamineVariableRelationships();
Boolean	  RecursiveCallOK(Var *A);
void	  AddOrders(Literal L);
void	  NoteRecursiveLit(Literal L);

	  /* finddef.c */

void	  FindDefinition(Relation R);
Clause	  FindClause();
void	  ExamineLiterals();
void	  GrowNewClause();
Boolean	  SameLiteral(Relation R, Boolean Sign, Var *A);
Boolean	  AllLHSVars(Literal L);
Boolean	  AllDeterminate();
float	  CodingCost(Clause C);

	  /* prune.c */

void	  PruneNewClause();
void	  CheckVariables();
Boolean	  TheoryConstant(Const C, TypeInfo T);
Boolean	  ConstantVar(Var V, Const C);
Boolean	  IdenticalVars(Var V, Var W);
Boolean	  Known(Relation R, Var V, Var W);
void	  Insert(Var V, Relation R, Var A1, Const A2);
Boolean	  Contains(Var *A, int N, Var V);
Boolean	  QuickPrune(Clause C, Var MaxBound, Boolean *Used);
Boolean	  SatisfactoryNewClause(int Errs);
void	  RenameVariables();
Boolean	  RedundantLiterals(int ErrsNow);
Boolean	  EssentialBinding(int LitNo);
void	  ReplaceVariable(Var Old, Var New);
void	  SiftClauses();
Var	  HighestVarInDefinition(Relation R);
Boolean	  Recursive(Clause C);

	  /* interpret.c */

Boolean	  CheckRHS(Clause C);
Boolean	  Interpret(Relation R, Tuple Case);
void	  InitialiseValues(Tuple Case, int N);

	  /* constants.c */

void	  OrderConstants();
void	  FindArgumentOrders();
void	  ExamineArgumentPairs(Relation R, Boolean Sign, Tuple *TP);
Boolean	  ConsistentClosure(Boolean **Table, Tuple *TP, Var A, Var B);
Boolean	  AddPair(Boolean **Table, int A, int B);
void	  AddArgOrder(Relation R, Boolean Sig, int A1, int A2);
Boolean	  **AllocatePartOrd(int Size);
void	  FreePartOrd(Boolean **PO, int Size);
void	  ClearPartOrd(Boolean **PO);
void	  FindConsistentSubset(int Included, int TryNext, Boolean **PO);
int	  CountEntries(int K);
