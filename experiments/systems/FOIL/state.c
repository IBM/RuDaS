/******************************************************************************/
/*									      */
/*	A state, summarising a point in the search for a clause, consists     */
/*	of a set of tuples and various counts.  The routines here set up      */
/*	an initial state for a relation and produce the new state that	      */
/*	results when a literal is added to the current (partial) clause	      */
/*									      */
/******************************************************************************/


#include  "defns.i"
#include  "extern.i"


	/*  Set up original state for search for definition  */


void  OriginalState(Relation R)
/*    -------------  */
{
    int		i, j, WorldSize=1, TupleArity, NegTuples, Remaining;
    Tuple	Case, *Scan;
    double	drand48();

    if ( ! LogFact )
    {
	LogFact = Alloc(1001, float);

	LogFact[0] = 0.0;
	ForEach(i, 1, 1000)
	{
	    LogFact[i] = LogFact[i-1] + Log2(i);
	}
    }

    /*  Establish number of variables hence size of tuples, and set variable 
        types equal to the type of variable in that position in the relation,
        and variable depths to zero  */

    StartDef.MaxVar = Current.MaxVar = R->Arity;
    TupleArity = R->Arity+1;

    ForEach(i, 1, R->Arity)
    {
	Variable[i]->Type    = R->Type[i];
	Variable[i]->TypeRef = R->TypeRef[i];
	Variable[i]->Depth   = 0;
    }

    StartDef.NPos = Number(R->Pos);
    AdjMinAccuracy = MINACCURACY;

    /* Test whether negative tuples are already defined in the relation */

    if ( R->Neg )
    {
	/*  Simply copy positive and negative tuples  */

	StartDef.NTot = AllTuples = Number(R->Neg) + StartDef.NPos;

	if ( StartDef.NTot > MAXTUPLES )
	{
	    printf("Training Set Size exceeds tuple limit: ");
	    printf("%d > %d\n", StartDef.NTot, MAXTUPLES);
	    printf("Rerun with larger MAXTUPLES to proceed further\n");
	    exit(0);
	}

	StartDef.Tuples = Alloc(StartDef.NTot+1, Tuple);

	i = 0;

	for ( Scan = R->Pos ; *Scan ; Scan++ )
	{
	    AddTuple(i, *Scan, TupleArity, PosMark);
	    i++;
	}

	for ( Scan = R->Neg ; *Scan ; Scan++ )
	{
	    AddTuple(i, *Scan, TupleArity, 0);
	    i++;
	}
    }
    else 
    {
	/* Negative tuples not already defined */

	/* Find maximum training set size */

	ForEach(j, 1, R->Arity)
	{
	    if ( R->TypeRef[j]->Continuous ) Error(6, R->Name, Nil);
	    WorldSize *= R->TypeRef[j]->NValues;
	}

	Remaining = WorldSize - (StartDef.NPos - R->PosDuplicates);

	AllTuples = Remaining + StartDef.NPos;
	NegTuples = SAMPLE * Remaining;
	if ( SAMPLE < 0.99 )
	{
	    AdjMinAccuracy = MINACCURACY /
			     (MINACCURACY + SAMPLE * (1-MINACCURACY));
	    Verbose(1)
	    {
		printf("\t\t(Adjusted minimum clause accuracy %.1f%%)\n",
			100*AdjMinAccuracy);
	    }
	}

	StartDef.NTot = StartDef.NPos + NegTuples;

	if ( StartDef.NTot > MAXTUPLES )
	{
	    printf("Training Set Size will exceed tuple limit: ");
	    printf("%d > %d\n", StartDef.NPos + NegTuples, MAXTUPLES);
	    printf("Rerun with larger MAXTUPLES to proceed further\n");
	    printf("(or use smaller sample of negative tuples).\n");
	    exit(0);
	}

	StartDef.Tuples = Alloc(StartDef.NTot+1, Tuple);

	/*  Copy positive tuples  */

	i = 0;

	for ( Scan = R->Pos ; *Scan ; Scan++ )
	{
	    AddTuple(i, *Scan, TupleArity, PosMark);
	    i++;
	}

	if ( WorldSize <= 10*MAXTUPLES )
	{
	    /* Enumerate all possible tuples and add a sample of the negative
	       tuples to the training set  - note that if SAMPLE is 1, the
	       default, all negative tuples are added to the training set */

	    Case = Nil;

	    while ( NegTuples && (Case = NextConstTuple(R, Case)) )
	    {
	        if ( ! Join(R->Pos, R->PosIndex, DefaultVars, Case, R->Arity, true)
		      && drand48() <= NegTuples / (float) Remaining-- )
	        {
		    AddTuple(i, Case, TupleArity, 0);
		    i++;
		    NegTuples--;
		}
	    }

	    if ( Case ) pfree(Case);
	}
	else
	{
	    /* Might take too long to enumerate all tuples, so generate
	       them randomly - can result in duplicate negative tuples */

	    Case = Alloc(TupleArity, Const);

	    while ( i < StartDef.NTot )
	    {
	        RandomTuple(R, Case);

	        if ( ! Join(R->Pos, R->PosIndex, DefaultVars, Case, R->Arity, 1))
	        {
		    AddTuple(i, Case, TupleArity, 0);
		    i++;
		}
	    }

	    pfree(Case);
	}
    }

    StartDef.Tuples[StartDef.NTot] = Nil;
    StartDef.NOrigPos = StartDef.NPos;
    StartDef.NOrigTot = StartDef.NTot;

    Realloc(Flags, StartDef.NTot+1, char);

    StartDef.BaseInfo = Info(StartDef.NPos, StartDef.NTot);
}



void  AddTuple(int N, Tuple T, int TupleArity, int Mark)
/*    --------  */
{
    StartDef.Tuples[N] = Alloc(TupleArity, Const);
    memcpy(StartDef.Tuples[N], T, TupleArity*sizeof(Const));
    StartDef.Tuples[N][0] = N | Mark;
}



	/*  Generate in Case the next constant tuple taking note of type
	    constraints */

Tuple  NextConstTuple(Relation R, Tuple Case)
/*     --------------  */
{
    int i, v;
    TypeInfo T;

    if ( ! Case )
    {
        Case = Alloc(R->Arity+1, Const);

	ForEach(i, 1, R->Arity)
	{
	    Case[i] = R->TypeRef[i]->Value[0];
	}
    }
    else
    {
        i = R->Arity;
	T = R->TypeRef[i];

	while ( Case[i] == T->Value[T->NValues-1] )
	{
	    if ( i <= 1 )
	    {
	        pfree(Case);
		return Nil;
	    }

	    Case[i] = T->Value[0];

	    T = R->TypeRef[--i];
	}

	for ( v = 1; Case[i] != T->Value[v-1] ; v++ )
	    ;
	Case[i] = T->Value[v];
    }
    return Case;
}



/*****************************************************************************/
/*                                                                           */
/*  RandomTuple - generate a random tuple satisfying type constraints for    */
/*                relation R                                                 */
/*                                                                           */
/*****************************************************************************/


void  RandomTuple(Relation R, Tuple Result)
/*    -----------  */
{
    int i, v;
    TypeInfo T;
    double drand48();

    ForEach(i, 1, R->Arity)
    {
        T = R->TypeRef[i];
	v = (int)(T->NValues * drand48());
	Result[i] = T->Value[v];
    }
}



void  NewState(Literal L, int NewSize)
/*    --------  */
{
    FormNewState(L->Rel, L->Sign, L->Args, NewSize);
    AcceptNewState(L->Rel, L->Args, NewSize);
}



	/*  Construct new state in New  */

void  FormNewState(Relation R, Boolean RSign, Var *A, int NewSize)
/*    ------------  */
{
    Tuple	*TSP, Case, *Bindings, Instance;
    int		i, N, RN;
    Boolean	BuiltIn=false, Match, NotNegated;
    Const	X2;

    if ( Predefined(R) )
    {
	/*  Prepare for calls to Satisfies()  */

	BuiltIn = true;
	RN = (int) R->Pos;
	if ( HasConst(R) )
	{
	    GetParam(&A[2], &X2);
	}
	else
	{
	    X2 = A[2];
	}
    }

    N = R->Arity;

    /*  Find highest variable in new clause  */

    New.MaxVar = Current.MaxVar;
    if ( RSign )
    {
        ForEach(i, 1, N)
	{
	    New.MaxVar = Max(A[i], New.MaxVar);
	}
    }

    New.Tuples = Alloc(NewSize+1, Tuple);

    New.NPos = New.NTot = 0;

    for ( TSP = Current.Tuples ; Case = *TSP++ ; )
    {
        if ( MissingValue(R,A,Case) ) continue;

	Match = ( BuiltIn ? Satisfies(RN, A[1], X2, Case) :
	          Join(R->Pos, R->PosIndex, A, Case, N, ! RSign) );
	NotNegated = RSign != 0;
	if ( Match != NotNegated ) continue;

	if ( ! BuiltIn && RSign )
	{
	    /*  Add tuples from R->Pos  */

	    CheckSize(New.NTot, NFound, &NewSize, &New.Tuples);

	    Bindings = Found;
	    while ( Instance = *Bindings++ )
	    {
		New.Tuples[New.NTot] = Extend(Case, Instance, A, N);
		New.NTot++;
		if ( Positive(Case) ) New.NPos++;
	    }
	}
	else
	{
	    CheckSize(New.NTot, 1, &NewSize, &New.Tuples);

	    New.Tuples[New.NTot] = Alloc(New.MaxVar+1, Const);
	    memcpy(New.Tuples[New.NTot], Case, (New.MaxVar+1)*sizeof(Const));
	    New.NTot++;
	    if ( Positive(Case) ) New.NPos++;
	}
    }
    New.Tuples[New.NTot] = Nil;
}


	/*  Move state in New to Current  */

void  AcceptNewState(Relation R, Var *A, int NewSize)
/*    --------------  */
{
    int		i, N, MaxDepth=0;
    Var		V;

    if ( Current.Tuples != StartClause.Tuples )
    {
	FreeTuples(Current.Tuples, true);
    }

    if ( New.MaxVar > Current.MaxVar )
    {
	/*  New variable(s) - update type and depth info  */

        N = R->Arity;
        ForEach(i, 1, N)
	{
            if ( (V = A[i]) <= Current.MaxVar )
	    {
                MaxDepth = Max(MaxDepth, Variable[V]->Depth);
	    }
	}
        MaxDepth++;

        ForEach(i, 1, N)
	{
            if ( (V = A[i]) > Current.MaxVar )
	    {
                Variable[V]->Type    = R->Type[i];
		Variable[V]->TypeRef = R->TypeRef[i];
                Variable[V]->Depth   = MaxDepth;
	    }
	}
    }

    New.BaseInfo = Info(New.NPos, New.NTot);

    /*  Move all information across and resize tuples if necessary  */

    Current = New;

    if ( New.NTot < NewSize )
    {
	Realloc(Current.Tuples, Current.NTot+1, Tuple);
    }

    Current.BaseInfo = Info(Current.NPos, Current.NTot);
}



    /*  Rebuild a training set by applying the literals in a clause
	to the copy of the training set  */

void  RecoverState(Clause C)
/*    ------------  */
{
    int		i, SaveVerbosity;
    Literal	L;
    float	ExtraBits;

    /*  Turn off messages during recovery  */

    SaveVerbosity = VERBOSITY;
    VERBOSITY = 0;

    if ( Current.Tuples != StartClause.Tuples )
    {
        FreeTuples(Current.Tuples, true);
	Current = StartClause;
    }

    ForEach(i, 1, Target->Arity)
    {
	memset(PartialOrder[i], '#', MAXVARS+1);
    	PartialOrder[i][i] = '=';
    }
    AnyPartialOrder = false;

    memset(Barred, false, MAXVARS+1);

    NRecLitClause = NDetLits = ClauseBits = 0;

    for ( NLit = 0 ; (L = C[NLit]) ; NLit++ )
    {
	NewClause[NLit] = L;

	if ( L->Rel == Target )
	{
	    ExamineVariableRelationships();
	    AddOrders(L);
	}

	NewState(L, Current.NTot);

	if ( L->Sign == 2 )
	{
	    NDetLits++;
	}
	else
	{
	    ExtraBits = L->Bits - Log2(NLit+1.001-NDetLits);
	    ClauseBits += Max(0, ExtraBits);
	}

	if ( L->Rel == Target ) NoteRecursiveLit(L);
	NWeakLits = L->WeakLits;
    }

    NewClause[NLit] = Nil;
    VERBOSITY = SaveVerbosity;
}



void  CheckSize(int SoFar, int Extra, int *NewSize, Tuple **TSAddr)
/*    ---------  */
{
    if ( SoFar+Extra > *NewSize )
    {
	*NewSize += Max(Extra, 1000);
	Realloc(*TSAddr, *NewSize+1, Tuple);
    }
}



    /*  Tack extra variables on a tuple  */

Tuple  Extend(Tuple Case, Tuple Binding, Var *A, int N)
/*     ------  */
{
    Tuple	NewCase;
    int		i;

    NewCase = Alloc(New.MaxVar+1, Const);
    memcpy(NewCase, Case, (Current.MaxVar+1)*sizeof(Const));

    ForEach(i, 1, N)
    {
	NewCase[A[i]] = Binding[i];
    }

    return NewCase;
}



void  CheckOriginalCaseCover()
/*    ----------------------  */
{
    Tuple *TSP, Case;

    ClearFlags;
    Current.NOrigPos = Current.NOrigTot = 0;

    for ( TSP = Current.Tuples ; Case = *TSP++ ; )
    {
        if ( ! TestFlag(Case[0]&Mask, TrueBit) )
	{
            SetFlag(Case[0]&Mask, TrueBit);
	    Current.NOrigTot++;
	    if ( Positive(Case) ) Current.NOrigPos++;
	}
    }
}



    /*  Free up a bunch of tuples  */

void  FreeTuples(Tuple *TT, Boolean TuplesToo)
/*    -------  */
{
    Tuple *P;

    if ( TuplesToo )
    {
	for ( P = TT ; *P ; P++ )
	{
	    pfree(*P);
	}
    }

    pfree(TT);
}



    /*  Find log (base 2) factorials using tabulated values and Stirling's
	approximation (adjusted)  */

double  Log2Fact(int n)
/*      --------  */
{
    return ( n < 1000 ? LogFact[n] :
	     (n+0.5) * Log2(n) - n * Log2e + Log2sqrt2Pi - (n*0.7623)/820000 );
}
