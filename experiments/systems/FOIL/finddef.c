/******************************************************************************/
/*									      */
/*	All the stuff for trying possible next literals, growing clauses      */
/*	and assembling definitions					      */
/*									      */
/******************************************************************************/


#include  "defns.i"
#include  "extern.i"


int	FalsePositive,
	FalseNegative;


void  FindDefinition(Relation R)
/*    --------------  */
{
    int Size, i, TargetPos, FirstDefR;

    Target = R;
    NCl = 0;

    printf("\n----------\n%s:\n", R->Name);

    /*  Reorder the relations so that the target relation comes first  */

    FirstDefR = ( RelnOrder[2] == GTVAR ? 4 : 2 );

    for ( TargetPos = FirstDefR ; RelnOrder[TargetPos] != Target ; TargetPos++ )
	;

    for ( i = TargetPos ; i > FirstDefR ; i-- )
    {
	RelnOrder[i] = RelnOrder[i-1];
    }
    RelnOrder[FirstDefR] = Target;

    /*  Generate initial tuples and make a copy  */

    OriginalState(R);

    StartClause = StartDef;

    Size = StartDef.NTot+1;
    StartClause.Tuples = Alloc(Size, Tuple);
    memcpy(StartClause.Tuples, StartDef.Tuples, Size*sizeof(Tuple));

    NRecLitDef = 0;

    FalsePositive = 0;
    FalseNegative = StartDef.NPos;

    R->Def = Alloc(100, Clause);

    while ( StartClause.NPos )
    { 
	if ( ! (R->Def[NCl] = FindClause()) ) break;

	R->Def[NCl++][NLit] = Nil;

	if ( NCl % 100 == 0 )
	{
	    Realloc(R->Def, NCl+100, Clause);
	}

	NRecLitDef += NRecLitClause;
    }
    R->Def[NCl] = Nil;

    SiftClauses();

    if ( FalsePositive || FalseNegative )
    {
	printf("\n***  Warning: the following definition\n");

	if ( FalsePositive )
	{
	    printf("***  matches %d tuple%s not in the relation\n",
		FalsePositive, Plural(FalsePositive));
	}

	if ( FalseNegative )
	{
	    printf("***  does not cover %d tuple%s in the relation\n",
		FalseNegative, Plural(FalseNegative));
	}
    }

    PrintDefinition(R);

    pfree(StartClause.Tuples);
    pfree(StartDef.Tuples);

    /*  Restore original relation order  */

    for ( i = FirstDefR ; i < TargetPos ; i++ )
    {
	RelnOrder[i] = RelnOrder[i+1];
    }
    RelnOrder[TargetPos] = Target;
}



Clause  FindClause()
/*      ----------  */
{
    Tuple Case, *TSP;

    InitialiseClauseInfo();

    GrowNewClause();

    /*  Now that pruning includes addition of implicit equalities,
	should try even when there is a single RHS literal  */

    PruneNewClause();

    /*  Make sure accuracy criterion is satisfied  */

    if ( ! NLit || Current.NOrigPos+1E-3 < AdjMinAccuracy * Current.NOrigTot )
    {
	if ( NLit )
	{
	    Verbose(1)
		printf("\nClause too inaccurate (%d/%d)\n",
		       Current.NOrigPos, Current.NOrigTot);
	}

	pfree(NewClause);
	return Nil;
    }

    FalsePositive += Current.NOrigTot - Current.NOrigPos;
    FalseNegative -= Current.NOrigPos;

    /*  Set flags for positive covered tuples  */

    ClearFlags;

    for ( TSP = Current.Tuples ; Case = *TSP ; TSP++ )
    {
	if ( Positive(Case) )
	{
	    SetFlag(Case[0]&Mask, TrueBit);
	}
    }

    if ( Current.Tuples != StartClause.Tuples )
    {
	FreeTuples(Current.Tuples, true);
    }

    /*  Copy all negative tuples and uncovered positive tuples  */

    StartClause.NTot = StartClause.NPos = 0;

    for ( TSP = StartClause.Tuples ; Case = *TSP ; TSP++ )
    {
	if ( ! Positive(Case) || ! TestFlag(Case[0]&Mask, TrueBit) )
	{
	    StartClause.Tuples[StartClause.NTot++] = Case;
	    if ( Positive(Case) ) StartClause.NPos++;
	}
    }
    StartClause.Tuples[StartClause.NTot] = Nil;

    StartClause.NOrigPos = StartClause.NPos;
    StartClause.NOrigTot = StartClause.NTot;

    StartClause.BaseInfo = Info(StartClause.NPos, StartClause.NTot);
    Current = StartClause;

    Verbose(1)
    {
	printf("\nClause %d: ", NCl);
	PrintClause(Target, NewClause);
    }

    return NewClause;
}



void  ExamineLiterals()
/*    ---------------  */
{
    Relation	R;
    int		i, Relns=0;

    NPossible = NDeterminate = 0;

    MaxPossibleGain = Current.NPos * Current.BaseInfo;

    /*  If this is not the first literal, review coverage and check
	variable orderings, identical variables etc.  */

    if ( NLit != 0 )
    {
	CheckOriginalCaseCover();
	ExamineVariableRelationships();
    }

    AvailableBits = Encode(Current.NOrigPos) - ClauseBits;

    Verbose(1)
    {
	printf("\nState (%d/%d", Current.NPos, Current.NTot);
	if ( Current.NTot != Current.NOrigTot )
	{
	    printf(" [%d/%d]", Current.NOrigPos, Current.NOrigTot);
	}
	printf(", %.1f bits available", AvailableBits);

	if ( NWeakLits )
	{
	    Verbose(2)
		printf(", %d weak literal%s", NWeakLits, Plural(NWeakLits));
	}
	printf(")\n");

	Verbose(4)
	    PrintTuples(Current.Tuples, Current.MaxVar);
    }

    /*  Find possible literals for each relation  */

    ForEach(i, 0, MaxRel)
    {
	R = RelnOrder[i];

	ExploreArgs(R, true);

	if ( R->NTrialArgs ) Relns++;
    }

    /*  Evaluate them  */

    AlterSavedClause = Nil;
    Verbose(2) putchar('\n');

    for ( i = 0 ; i <= MaxRel && BestLitGain < MaxPossibleGain ; i++ )
    {
	R = RelnOrder[i];
	if ( ! R->NTrialArgs ) continue;

	R->Bits = Log2(Relns) + Log2(R->NTrialArgs+1E-3);
	if ( NEGLITERALS || Predefined(R) ) R->Bits += 1.0;

	if ( R->Bits - Log2(NLit+1.001-NDetLits) > AvailableBits )
	{
	    Verbose(2)
	    {
		printf("\t\t\t\t[%s requires %.1f bits]\n", R->Name, R->Bits);
	    }
	}
	else
	{
	    ExploreArgs(R, false);

	    Verbose(2)
		printf("\t\t\t\t[%s tried %d/%d] %.1f secs\n",
		       R->Name, R->NTried, R->NTrialArgs, CPUTime());
	}
    }
}



void  GrowNewClause()
/*    -------------                */
{
    Literal	L;
    int		i, OldNLit;
    Boolean	Progress=true;
    float	Accuracy, ExtraBits;

    while ( Progress && Current.NPos < Current.NTot )
    {
	ExamineLiterals();

	/*  If have noted better saveable clause, record it  */

	if ( AlterSavedClause )
	{
	    Realloc(SavedClause, NLit+2, Literal);
	    ForEach(i, 0, NLit-1)
	    {
		SavedClause[i] = NewClause[i];
	    }
	    SavedClause[NLit]   = AllocZero(1, struct _lit_rec);
	    SavedClause[NLit+1] = Nil;

	    SavedClause[NLit]->Rel      = AlterSavedClause->Rel;
	    SavedClause[NLit]->Sign     = AlterSavedClause->Sign;
	    SavedClause[NLit]->Args     = AlterSavedClause->Args;
	    SavedClause[NLit]->Bits     = AlterSavedClause->Bits;
	    SavedClause[NLit]->WeakLits = 0;

	    SavedClauseCover    = AlterSavedClause->PosCov;
	    SavedClauseAccuracy = AlterSavedClause->PosCov /
					  (float) AlterSavedClause->TotCov;

	    Verbose(1)
	    {
		printf("\n\tSave clause ending with ");
		PrintLiteral(SavedClause[NLit]);
		printf(" (cover %d, accuracy %d%%)\n",
		       SavedClauseCover, (int) (100*SavedClauseAccuracy));
	    }

	    pfree(AlterSavedClause);
	}

	if ( NDeterminate && BestLitGain < DETERMINATE * MaxPossibleGain )
	{
	    ProcessDeterminateLiterals(true);
	}
	else
	if ( NPossible )
	{
	    /*  At least one gainful literal  */

	    NewClause[NLit] = L = SelectLiteral();
	    if ( ++NLit % 100 == 0 ) Realloc(NewClause, NLit+100, Literal);

	    ExtraBits = L->Bits - Log2(NLit-NDetLits+1E-3);
	    ClauseBits += Max(ExtraBits, 0);

	    Verbose(1)
	    {
		printf("\nBest literal ");
		PrintLiteral(L);
		printf(" (%.1f bits)\n", L->Bits);
	    }

	    /*  Check whether should regrow clause  */

	    if ( L->Rel != Target && AllLHSVars(L) &&
		 Current.MaxVar > Target->Arity && ! AllDeterminate() )
	    {
		OldNLit = NLit;
		NLit = 0;
		ForEach(i, 0, OldNLit-1)
		{
		    if ( AllLHSVars(NewClause[i]) )
		    {
			NewClause[NLit++] = NewClause[i];
		    }
		}
		NewClause[NLit] = Nil;

		RecoverState(NewClause);

		Verbose(1)
		{
		    printf("\n[Regrow clause] ");
		    PrintClause(Target,NewClause);
		}
		GrowNewClause();
		return;
	    }

	    NWeakLits = L->WeakLits;

	    if ( L->Rel == Target ) AddOrders(L);

	    NewState(L, Current.NTot);

	    if ( L->Rel == Target ) NoteRecursiveLit(L);
	}
	else
	{
	    Verbose(1) printf("\nNo literals\n");

	    Progress = Recover();
	}
    }
    NewClause[NLit] = Nil;

    /*  Finally, see whether saved clause is better  */

    CheckOriginalCaseCover();
    Accuracy = Current.NOrigPos / (float) Current.NOrigTot;
    if ( SavedClause &&
	 ( SavedClauseAccuracy > Accuracy ||
	   SavedClauseAccuracy == Accuracy &&
	   SavedClauseCover > Current.NOrigPos ||
	   SavedClauseAccuracy == Accuracy &&
	   SavedClauseCover == Current.NOrigPos &&
	   CodingCost(SavedClause) < CodingCost(NewClause) ) )
    {
	Verbose(1) printf("\n[Replace by saved clause]\n");
	RecoverState(SavedClause);
	CheckOriginalCaseCover();
    }
}


    InitialiseClauseInfo()
/*  --------------------  */
{
    Var V;

    /*  Initialise everything for start of new clause  */

    NewClause = Alloc(100, Literal);

    Current = StartClause;

    NLit = NDetLits = NWeakLits = NRecLitClause = 0;

    NToBeTried = 0;
    AnyPartialOrder = false;

    ClauseBits = 0;

    ForEach(V, 1, Target->Arity)
    {
	Variable[V]->Depth   = 0;
	Variable[V]->Type    = Target->Type[V];
	Variable[V]->TypeRef = Target->TypeRef[V];
    }

    memset(Barred, false, MAXVARS+1);

    SavedClause = Nil;
    SavedClauseAccuracy = SavedClauseCover = 0;

    MAXRECOVERS = MAXALTS;
}



Boolean  SameLiteral(Relation R, Boolean Sign, Var *A)
/*       -----------  */
{
    Var V, NArgs;

    if ( R != AlterSavedClause->Rel || Sign != AlterSavedClause->Sign )
    {
	return false;
    }

    NArgs = ( HasConst(R) ? 1 + sizeof(Const) : R->Arity);
    ForEach(V, 1, NArgs)
    {
	if ( A[V] != AlterSavedClause->Args[V] ) return false;
    }

    return true;
}



Boolean  AllLHSVars(Literal L)
/*       ----------  */
{
    Var V;

    ForEach(V, 1, L->Rel->Arity)
    {
	if ( L->Args[V] > Target->Arity ) return false;
    }

    return true;
}



	/*  See whether all literals in clause are determinate  */

Boolean  AllDeterminate()
/*       --------------  */
{
    int i;

    ForEach(i, 0, NLit-2)
    {
	if ( NewClause[i]->Sign != 2 ) return false;
    }

    return true;
}


	/*  Find the coding cost for a clause  */

float	CodingCost(Clause C)
/*      ----------  */
{
    float	SumBits=0, Contrib;
    int		Lits=0;

    while ( *C )
    {
	Lits++;
	if ( (Contrib = (*C)->Bits - Log2(Lits)) > 0 ) SumBits += Contrib;
	C++;
    }

    return SumBits;
}
