/******************************************************************************/
/*									      */
/*	Examine the space of possible literals on a relation		      */
/*									      */
/******************************************************************************/


#include "defns.i"
#include "extern.i"

#define		MAXTIME		100

Var		*Arg = Nil;	/* potential arguments */
Boolean		CountOnly;
float		StartTime;	/* CPU time at start of literal */
int		Ticks,		/* calls of TryArgs() */
                TicksCheck;     /* point to check time again */


	/*  Examine possible variable assignments for next literal using
	    relation R.  If CountOnly specified, this will only set the
	    number of them in R->NTrialArgs; otherwise, it will evaluate
	    the possible arguments  */

void  ExploreArgs(Relation R, Boolean CountFlag)
/*    -----------  */
{
    int	MaxArgs=1;
    Var	V;

    CountOnly = CountFlag;
    if ( CountOnly )
    {
	R->NTrialArgs = 0;
    }
    else
    {
	R->NTried = 0;
    }

    if ( R == Target && ! AnyPartialOrder ) return;

    if ( Predefined(R) )
    {
	if ( R == EQVAR )
	{
	    ExploreEQVAR();
	}
	else
	if ( R == EQCONST )
	{
	    ExploreEQCONST();
	}
	else
	if ( R == GTVAR )
	{
	    ExploreGTVAR();
	}
	else
	if ( R == GTCONST )
	{
	    ExploreGTCONST();
	}

	return;
    }

    if ( CountOnly )
    {
	/*  Carry out a preliminary feasibility check  */

	for ( V = 1 ; MaxArgs <= 1E7 && V <= R->Arity ; V++ )
	{
	    MaxArgs *= EstimatePossibleArgs(R->Type[V]);
	}

	if ( MaxArgs > 1E7 )
	{
	    Verbose(2)
	    {
		printf("\t\t\t\t[%s: too many possibilities]\n", R->Name);
	    }
	    R->NTrialArgs = 0;
	    return;
	}
    }
    else
    {
	Ticks      = 0;
	TicksCheck = 10;
	StartTime  = CPUTime();
    }

    TryArgs(R, 1, Current.MaxVar, 0, 0, 0, true, false);
}



int  EstimatePossibleArgs(int TNo)
/*   --------------------  */
{
    int Sum=1, V;

    ForEach(V, 1, Current.MaxVar)
    {
	if ( Compatible[Variable[V]->Type][TNo] ) Sum++;
    }

    return Sum;
}



	/*  Determine whether a key is acceptable for the relation being
	    explored.  Note that keys are packed bit strings with a 1
	    wherever there is an unbound variable  */


Boolean  AcceptableKey(Relation R, int Key)
/*	 -------------  */
{
    int i;

    if ( ! R->NKeys ) return true;

    ForEach(i, 0, R->NKeys-1)
    {
	if ( (R->Key[i] | Key) == R->Key[i] ) return true;
    }

    return false;
}



	/*  See whether a potential literal is actually a repeat of a literal
	    already in the clause (with perhaps different free variables)  */


Boolean  Repetitious(Relation R, Var *A)
/*       -----------  */
{
    Literal	L;
    Var		V, MaxV;
    int		i, a, N;

    MaxV = Target->Arity;

    ForEach(i, 0, NLit-1)
    {
	L = NewClause[i];
	if ( L->Rel == R &&
	     SameArgs(R->Arity, A, Current.MaxVar, L->Args, MaxV, i+1) )
	{
	    return true;
	}

	if ( L->Sign )
	{
	    N = L->Rel->Arity;
	    ForEach(a, 1, N)
	    {
		if ( (V = L->Args[a]) > MaxV ) MaxV = V;
	    }
	}
    }

    return false;
}



Boolean  Mentioned(Var V, int First)
/*       ---------  */
{
    int		i, a, N;
    Literal	L;

    ForEach(i, First, NLit-1)
    {
	L = NewClause[i];
	N = L->Rel->Arity;

	ForEach(a, 1, N)
	{
	    if ( L->Args[a] == V ) return true;
	}
    }

    return false;
}



	/*  Check whether two aruments are identical up to substitution
	    of free variables  */


Boolean  SameArgs(int N, Var *A1, int MV1, Var *A2, int MV2, int LN)
/*       --------  */
{
    int a;

    ForEach(a, 1, N)
    {
	if ( ( A1[a] <= MV1 ? A2[a] != A1[a] : 
			  ( A2[a]-MV2 != A1[a]-MV1 || Mentioned(A2[a], LN) ) ) )
	{
	    return false;
	}
    }

    return true;
}



	/*  Find arguments for predefined relations  */


void  ExploreEQVAR()
/*    ------------  */
{
    Var		V, W;

    ForEach(V, 1, Current.MaxVar-1)
    {
	if ( Barred[V] ) continue;

	Arg[1] = V;

	ForEach(W, V+1, Current.MaxVar)
	{
	    if ( Barred[W] ) continue;

	    if ( Compatible[Variable[V]->Type][Variable[W]->Type] )
	    {
		if ( CountOnly )
		{
		    EQVAR->NTrialArgs++;
		}
		else
		{
		    Arg[2] = W;
		    EvaluateLiteral(EQVAR, Arg, EQVAR->Bits, Nil);
		    EQVAR->NTried++;
		}
	    }
	}
    }
}

		

void  ExploreEQCONST()
/*    --------------  */
{
    Var		V;
    int		T, i, n;
    Const	C;
    Boolean	*VarBound;
    Literal	L;

    /*  Find variables that have bound values  */

    VarBound = AllocZero(Current.MaxVar+1, Boolean);
    ForEach(i, 0, NLit-1)
    {
	L = NewClause[i];
	if ( L->Rel == EQCONST && L->Sign )
	{
	    VarBound[L->Args[1]] = true;
	}
    }

    ForEach(V, 1, Current.MaxVar)
    {
	if ( Barred[V] || VarBound[V] ) continue;

	T = Variable[V]->Type;

	if ( n = Type[T]->NTheoryConsts )
	{
	    Arg[1] = V;

	    ForEach(i, 0, n-1)
	    {
		if ( CountOnly )
		{
		    EQCONST->NTrialArgs++;
		}
		else
		{
		    C = Type[T]->TheoryConst[i];
		    SaveParam(&Arg[2], &C);

		    EvaluateLiteral(EQCONST, Arg, EQCONST->Bits, Nil);
		    EQCONST->NTried++;
		}
	    }
	}
    }

    free(VarBound);
}



void  ExploreGTVAR()
/*    ------------  */
{
    Var		V, W;

    ForEach(V, 1, Current.MaxVar-1)
    {
	if ( Barred[V] || ! Variable[V]->TypeRef->Continuous ) continue;

	Arg[1] = V;

	ForEach(W, V+1, Current.MaxVar)
	{
	    if ( Barred[W] || Variable[W]->Type != Variable[V]->Type ) continue;

	    if ( CountOnly )
	    {
		GTVAR->NTrialArgs++;
	    }
	    else
	    {
		Arg[2] = W;
		EvaluateLiteral(GTVAR, Arg, GTVAR->Bits, Nil);
		GTVAR->NTried++;
	    }
	}
    }
}

		

void  ExploreGTCONST()
/*    --------------  */
{
    Var		V;
    float	Z=MISSING_FP;

    ForEach(V, 1, Current.MaxVar)
    {
	if ( Barred[V] || ! Variable[V]->TypeRef->Continuous ) continue;

	if ( CountOnly )
	{
	    GTCONST->NTrialArgs++;
	}
	else
	{
	    Arg[1] = V;
	    SaveParam(&Arg[2], &Z);

	    EvaluateLiteral(GTCONST, Arg, GTCONST->Bits, Nil);
	    GTCONST->NTried++;
	}
    }
}



    /*  Generate argument lists starting from position This.
	In the partial argument list Arg[1]..Arg[This-1],
	    HiVar is the highest variable (min value MaxVar)
	    FreeVars is the number of free variable occurrences
	    MaxDepth is the highest depth of a bound variable
	    Key gives the key so far
	TryMostGeneral is true when we need to fill all remaining argument
	positions with new free variables.
	RecOK is true when a more general argument list has been found to
	satisfy RecursiveCallOK(), so this must also.

	Return false if hit time limit.  */
	
	
Boolean  TryArgs(Relation R, int This, int HiVar, int FreeVars, int MaxDepth,
	      int Key, Boolean TryMostGeneral, Boolean RecOK)
/*       -------  */
{
    Var		V, W, MaxV;
    Boolean	Prune, UselessSameVar;
    int		NewFreeVars, NewMaxDepth, CopyKey;
    float       TimeSpent;


    /*  This version contains a time cutout to prevent too much effort
	begin invested in a single literal.  Unfortunately, direct
	monitoring of time is too expensive (in system time and overhead)
	so a more circuitous method that calls CPUTime() rarely is used  */

    if ( ! CountOnly )
    {
        if ( Ticks > TicksCheck )
	{
	    if ( (TimeSpent = CPUTime() - StartTime) > MAXTIME )
	    {
		return false;
	    }
	    else
	    if ( TimeSpent > 0.001 * MAXTIME )
	    {
		TicksCheck += 0.01 * Ticks * MAXTIME / TimeSpent;
	    }
	    else
	    {
		TicksCheck *= 10;
	    }
	}

	Ticks++;
    }

    /*  Try with all remaining positions (if any) as new free variables  */

    NewFreeVars = R->Arity - This + 1;

    if ( TryMostGeneral &&
	 HiVar + NewFreeVars <= MAXVARS	/* enough variables */ &&
	 FreeVars < This-1		/* at least one bound */ &&
	 ( ! NewFreeVars || MaxDepth < MAXVARDEPTH ) )
    {
	CopyKey = Key;
	ForEach(V, This, R->Arity)
	{
	    Arg[V] = W = HiVar + (V - This + 1);
	    CopyKey |= 1<<V;

	    Variable[W]->Type    = R->Type[V];
	    Variable[W]->TypeRef = R->TypeRef[V];
	}

	if ( AcceptableKey(R, CopyKey) &&
	     ( R != Target ||
	       RecOK ||
	       (RecOK = RecursiveCallOK(Arg)) ) )
	{
	    if ( CountOnly )
	    {
		R->NTrialArgs++;
	    }
	    else
	    if ( ! Repetitious(R, Arg) )
	    {
		EvaluateLiteral(R, Arg, R->Bits, &Prune);
		R->NTried++;

		if ( Prune && NewFreeVars )
		{
		    Verbose(3) printf("\t\t\t\t(pruning subsumed arguments)\n");
		    return true;
		}
	    }
	}
    }
		
    if ( This > R->Arity ) return true;

    /*  Now try substitutions recursively  */

    MaxV = ( Predefined(R) ? HiVar :
	     This < R->Arity && HiVar < MAXVARS ? HiVar+1 : HiVar );

    for ( V = 1 ; V <= MaxV && BestLitGain < MaxPossibleGain ; V++ )
    {
	if ( V <= Current.MaxVar )
	{
	    if ( Barred[V] ) continue;
	    NewMaxDepth = Max(MaxDepth, Variable[V]->Depth);
	    NewFreeVars = FreeVars;
	}
	else
	{
	    NewMaxDepth = MaxDepth;
	    NewFreeVars = FreeVars+1;
	}

	if ( V > HiVar )
	{
	    Variable[V]->Type    = R->Type[This];
	    Variable[V]->TypeRef = R->TypeRef[This];
	    Key |= 1<<This;
	    if ( ! AcceptableKey(R, Key) ) break;
	}

	/*  Check same variables where impossible  */

	UselessSameVar = false;
	ForEach(W, 1, This-1)
	{
	    UselessSameVar |= Arg[W] == V && R->ArgNotEq[ ArgPair(This,W) ];
	}

	if ( UselessSameVar ||
	     V <= HiVar && ! Compatible[Variable[V]->Type][R->Type[This]] ||
	     NewMaxDepth + (NewFreeVars > 0) > MAXVARDEPTH ||
	     R->BinSym && This == 2 && V < Arg[1] )
	{
	    continue;
	}

	Arg[This] = V;

	if ( ! TryArgs(R, This+1, Max(HiVar, V), NewFreeVars, NewMaxDepth,
		       Key, V <= HiVar, RecOK) ) return false;
    }

    return true;
}
