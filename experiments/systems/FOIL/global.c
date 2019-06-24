#include "defns.i"

/******************************************************************************/
/*									      */
/*	Parameters set by options and variables accessible to many routines   */
/*									      */
/******************************************************************************/


Boolean
	NEGLITERALS   = true,	/* negated literals ok */
	NEGEQUALS     = true,	/* negated equality literals ok */
	UNIFORMCODING = false,	/* uniform coding of literals */

	MissingVals   = false,	/* missing values in input? */
	AnyPartialOrder,	/* quick check to rule out recursive lits */
	*Barred,		/* duplicate variables */
	OutOfWorld;		/* flag ^ constant */


float
	SAMPLE	    = 1.0,	/* fraction of negative tuples to sample */
	MINACCURACY = 0.8,	/* minimum acceptable clause accuracy */
	MINALTFRAC  = 0.8,	/* fraction of best gain required for backup */
	DETERMINATE = 0.8;  	/* use determinate literals unless a literal
				   with this fraction of max possible gain */

int
	MAXVARS	    = 52,	/* max number of variables */
	MAXARGS     = 5,	/* max arity of any relation */
	MAXWEAKLITS = 3,	/* max weak literals in sequence */

	MAXPOSSLIT  = 5,	/* 1 + max backups from single state */
	MAXALTS     = 20,	/* max simultaneous backups */
	MAXRECOVERS,		/* max total backups */
	MAXVARDEPTH = 4,	/* max depth of var in literal */
	MAXTUPLES   = 100000,	/* max number of tuples */
	VERBOSITY   = 1,	/* level of output */

	MaxConst    = 0,	/* no. constants */
	MaxType	    = 0,	/* no. types */
	MaxRel	    = 0,	/* highest relation no */

	AllTuples,		/* effective size of universe */
	NCl,			/* current clause number */
	NLit,			/* current literal number */
	NDetLits,		/* number of determinate lits in clause */
	NWeakLits,		/* current weak lits in sequence */
	SavedClauseCover;	/* coverage of saved clause  */

char
	**ConstName,		/* names of all discrete constants */
	*Flags = Nil;		/* flag bits for original tuples */


Relation
	*Reln,			/* relations */
	*RelnOrder,      	/* order to try relations */
	Target;			/* relation being induced */


State
	StartDef,		/* at start of definition */
	StartClause,		/* at start of clause */
	Current,		/* current state */
	New;			/* possible next state */


float
	*LogFact = Nil,		/* LogFact[i] = log2(i!) */
	MaxPossibleGain,
	ClauseBits,		/* bits used so far in this clause */
	AvailableBits,		/* bits available for this clause */
	AdjMinAccuracy,		/* min accuracy adjusted for sampling */
	SavedClauseAccuracy;	/* accuracy of saved clause */


Clause
	NewClause,		/* clause being constructed */
	SavedClause;		/* best shorter clause discovered
				   while developing current clause */

PossibleLiteral
	AlterSavedClause;	/* last literal of saved clause */


Boolean
	**PartialOrder,		/* partial orders on variables*/
	**Compatible;		/* Compatible[i][j] = true if types i, j have
				   at least one common value  */

Ordering
	**RecursiveLitOrders;	/* pointers to orders in recursive lits  */

int
	NRecLitClause,		/* number of recursive lits in the new clause */
	NRecLitDef;		/* ditto in definition so far */

VarInfo
	*Variable;		/* variables */

Var
	*DefaultVars;		/* default variable list */

TypeInfo
	*Type;			/* types */

Tuple		*Found;		/* join */
int		NFound;		/* number of tuples in join */

Alternative	*ToBeTried;	/* backup points */
int		NToBeTried;

PossibleLiteral	*Possible;	/* possible literals */
int		NPossible,
		NDeterminate;
