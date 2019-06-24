#include  "defns.i"
#include  "extern.i"


	/*  Given tuples T with index TIX, find the tuples that satisfy
	    the column and same-value constraints on case C.  Leave the
	    tuples in Found with their number in NFound  */

	/*  NB: Foil spends a large proportion of its execution time in
	    this single routine.  For that reason, the code has been
	    written for speed (on a DECstation), even though this has
	    reduced its clarity.  If using different hardware, it would
	    probably be worth rewriting this  */

int FoundSize = 0;


Boolean  Join(Tuple *T, Index TIX, Var *A, Tuple C, int N, Boolean YesOrNo)
/*       ----  */
{
    static int	*Pair,			/* Pair[i], Pair[i+1] 
					   are paired variables */
		*Contin,		/* Contin[i] is a continuous variable
					   that must have the given value */
		**Next;			/* Next[i] = next tuple obeying 
					   ith constraint */
    static Boolean *Checked;

    int		*MaxPair,		/* highest pair of same variables */
		*PairPtr,
		*MaxContin,		/* highest continuous variable */
		*ContinPtr,
		MaxCol=0,		/* highest column constraint */
    		MaxNo=0,		/* index numbers in relation */
		**NextPtr,
		**LastNext,
		V, Val, i, j;

    Boolean NoCols;
    Tuple Candidate;

    /*  Allocate arrays first time through  */

    if ( ! FoundSize )
    {
	Pair = Alloc(2*(MAXVARS+1), int);
	Next = Alloc(MAXVARS+1, int *);
	Checked = Alloc(MAXVARS+1, Boolean);

	Contin = Alloc(MAXVARS+1, int);

	FoundSize = 20000;
	Found = Alloc(FoundSize+1, Tuple);
    }

    MaxPair   = Pair;
    MaxContin = Contin;

    NFound = 0;
    OutOfWorld = false;

    /*  Set the column constraints and find pairs of free variables that
	must be the same  */

    memset(Checked+1, 0, N);

    ForEach(i, 1, N)
    {
	/*  If this variable is bound, record a constraint; otherwise see if
	    it is the same as another unbound variable  */

	if ( (V = A[i]) <= Current.MaxVar && (Val = C[V]) != UNBOUND )
	{
	    if ( Variable[V]->TypeRef->Continuous ) *MaxContin++ = i;
	    else
	    if ( ! (Next[MaxCol++] = TIX[i][Val]) ) return false;
	}
	else
	if ( ! Checked[i] )
	{
	    ForEach(j, i+1, N)
	    {
		if ( A[j] == V )
		{
		    *MaxPair++ = i;
		    *MaxPair++ = j;
		    Checked[j] = true;
		}
	    }
	}
    }
    NoCols = MaxCol-- <= 0;
    LastNext = Next + MaxCol;

    while ( true )
    {
	/*  Advance all columns to MaxNo  */

	for ( NextPtr = Next ; NextPtr <= LastNext ; )
	{
	    while ( **NextPtr < MaxNo ) (*NextPtr)++;

	    MaxNo = **NextPtr++;
	}

	if ( MaxNo == FINISH || NoCols && ! T[MaxNo] )
	{
	    Found[NFound] = Nil;
	    return (NFound > 0);
	}
	else
	if ( NoCols || MaxNo == *Next[0] )
	{
	    /*  Found one possibility  -- check same variable constraints  */

	    Candidate = T[MaxNo];

	    for ( PairPtr = Pair ;
		  PairPtr < MaxPair 
                  && Candidate[*PairPtr] == Candidate[*(PairPtr+1)] ;
		  PairPtr += 2 )
		;

	    for ( ContinPtr = Contin ;
		  ContinPtr < MaxContin
		  && Candidate[*ContinPtr] == C[ A[*ContinPtr] ] ;
		  ContinPtr++ )
		;

	    if ( PairPtr >= MaxPair && ContinPtr >= MaxContin )
	    {
		if ( YesOrNo ) return true;

		if ( NFound >= FoundSize )
		{
		    FoundSize += 20000;
		    Realloc(Found, FoundSize+1, Tuple);
		}

		Found[NFound++] = Candidate;

		/*  Check for falling off the end of the (closed) world  */

		for ( i = 1 ; ! OutOfWorld && i <= N ; i++ )
		{
		    if ( Candidate[i] == OUT_OF_RANGE )
		    {
			OutOfWorld = true;
			Found[NFound] = Nil;
			return true;
		    }
		}
	    }

	    MaxNo++;
	}
    }
}
