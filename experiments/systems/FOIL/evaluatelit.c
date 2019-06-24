/******************************************************************************/
/*									      */
/*	This group of routines has responsibility for evaluating a proposed   */
/*	literal.  There are many aspects that are checked simultaneously:     */
/*									      */
/*	  *  whether the literal is determinate				      */
/*	  *  whether this literal would result in a completed clause	      */
/*	  *  whether this literal would produce too many tuples		      */
/*	  *  pruning, both of this literal and of any specialisations of it   */
/*	  *  new variables that duplicate existing variables		      */
/*		- for all tuples					      */
/*		- for pos tuples (determinate literals)			      */
/*	  *  all new variables being bound to constants on pos tuples	      */
/*	  *  gain computation (including weak literal sequence check)	      */
/*	  *  adjusting records of best literals so far			      */
/*									      */
/*	The principles underlying this routine are:			      */
/*									      */
/*	  *  No literal may introduce a variable that duplicates an existing  */
/*	     variable (since the same effect could be obtained with a more    */
/*	     specific literal).						      */
/*	  *  No determinate literal may introduce a new variable that is      */
/*	     the same as an existing variable on pos tuples (since the more   */
/*	     specific literal would also be determinate).		      */
/*	  *  Exploration of a literal can cease as soon as it is clear that   */
/*	     the literal will not be used.  In some cases, it is also possible*/
/*	     to exclude other literals subsumed by this.  Such pruning is     */
/*	     tied to the calculation of gain and determinacy, and would need  */
/*	     to be altered if these are changed.			      */
/*									      */
/*	The various counts have components with the following meanings:	      */
/*									      */
/*		Pos	pos tuples					      */
/*		Neg	neg tuples					      */
/*		Tot	all tuples					      */
/*									      */
/*		T	pertaining to the unnegated literal R(A)	      */
/*		F	pertaining to ~R(A)				      */
/*		M	missing value (so neither)			      */
/*									      */
/*		Now	number of tuples in current state		      */
/*		Orig	number of tuples in original state		      */
/*		New	referring to state if use R(A) or ~R(A)		      */
/*									      */
/******************************************************************************/


#include  "defns.i"
#include  "extern.i"


int
	OrigTPos,		/* orig pos tuples with extensions, R(A) */
	OrigTTot,
	OrigFPos,		/* ditto, ~R(A) */
	OrigFTot,

	NowTPos,		/* current pos tuples with extensions, R(A) */
	NowTTot,
	NowFPos,		/* ditto, ~R(A); NewFPos is identical */
	NowFNeg,
	NowFTot,		/* calculated for brevity at end */
	NowMPos,		/* pos tuples with missing values */
	NowMTot,		/* pos tuples with missing values */

	NewTPos,		/* pos tuples in new state, R(A) */
	NewTTot,

	NNewVars,		/* number of unbound vars in R(A) */
	NConstVars,		/* number of constant pseudo-vars  */
	NDuplicateVars,		/* number of duplications so far */
	NArgs,			/* number of relation arguments */
	BestCover,		/* coverage of best saveable clause */
	MinSaveableCover,	/* min coverage if clause saveable */
	MaxFPos,
	PossibleCuts;

Var
	*OldVar,		/* old var possibly equal to a new var */
	*NewVarPosn,		/* argument number of new var */
	*ConstVarPosn;		/* new vars with unchanging values */

Tuple
	FirstBinding;		/* check for consts masquerading as new vars */

Boolean
	Allocate = true,
	NegatedLiteralOK,	/* ~R(A) permissible  */
	*DifferOnNegs,		/* corresponding pair not identical */
	*FirstOccurrence,	/* first time variable appears in args */
	PossibleT,		/* R(A) is a candidate */
	PossibleF,		/* ~R(A) is a candidate */
	Determinate;		/* R(A) is determinate */

float
	MinUsefulGain,		/* min gain to affect outcome */
	MinPos,			/* min pos tuples to achieve MinUsefulGain */
	BestAccuracy,		/* accuracy of best saveable clause */
	MaxFNeg,		/* max FNeg tuples to avoid pruning ~R(A) */
	NewClauseBits;		/* if add literal to clause  */



	/*  Examine literals R(A) and ~R(A) (if appropriate)  */

void  EvaluateLiteral(Relation R, Var *A, float LitBits, Boolean *Prune)
/*    ---------------  */
{
    Boolean	Abandoned, RealDuplicateVars=false, WeakT, WeakF, SavedSign;
    Var		V, W;
    int		i, j, Coverage, Size;
    float	PosGain, NegGain, Gain, CurrentRatio, Accuracy, Extra;

    Verbose(3)
    {
	putchar('\t');
	PrintComposedLiteral(R, true, A);
    }

    if ( Prune ) *Prune = false;


    Extra = LitBits - Log2(NLit+1.001-NDetLits);
    NewClauseBits = ClauseBits + Max(0, Extra);

    PrepareForScan(R, A);

    if ( R == GTCONST )
    {
	/*  Find best threshold and counts in one go  */

	FindThreshold(A);
	if ( PossibleCuts > 0 ) LitBits += Log2(PossibleCuts);
	Determinate = Abandoned = false;
	NDuplicateVars = NConstVars = 0;
    }
    else
    if ( (Abandoned = TerminateScan(R, A)) &&
	 NewTTot <= MAXTUPLES	/* not because out of room for tuples */ &&
	 NNewVars		/* new vars, so potential for pruning */ &&
	 Current.NPos - (NowFPos+NowMPos) < MinPos  /* R(A) has insuff gain */ )
    {
	/*  Check for possible pruning of more specific literals  */

	if ( NegatedLiteralOK )
	{
	    /*  A more specific ~R(A) will match more tuples.  In order to
		prune, we must be sure that the current ~R(A) matches
		enough neg tuples to ensure that the gain is not interesting.
		Assume all pos tuples other than those known to have missing
		values would be matched by a more specific ~R(A)  */

	    MaxFPos = Current.NPos - NowMPos;
	    MaxFNeg = NegThresh(MaxFPos, MaxFPos);

	    CheckForPrune(R, A);

	    *Prune = NowFNeg > MaxFNeg;
	}
	else
	{
	    /*  Can prune only when a more specific literal than R(A)
		could not be saveable  */

	    *Prune = OrigTPos < MinSaveableCover;
	}
    }

    NowFTot = NowFPos + NowFNeg;
    PossibleT &= NowTPos > 0;
    PossibleF &= NowFPos > 0;

    Verbose(3)
    {
	printf("  %d[%d/%d]", NowTPos, NewTPos, NewTTot);

	if ( NegatedLiteralOK || Abandoned )
	{
	    printf(" [%d/%d]", NowFPos, NowFTot);
	}
	printf("  ");
    }

    if ( Abandoned )
    {
	Verbose(3)
	{
	    printf(" abandoned");
	    if ( OutOfWorld ) printf(" ^");
	    printf("(%d%%)\n",
		   (100 * (NowTTot+NowFTot+NowMTot)) / Current.NTot);
	}

	return;
    }

    ForEach(i, 0, NDuplicateVars-1)
    {
	V = A[ NewVarPosn[i] ];
	W = OldVar[i];
	if ( W > Current.MaxVar ) W = A[ W-Current.MaxVar ];  /* special */
	if ( NowTPos || ! DifferOnNegs[i] )
	{
	    Verbose(3)
		printf(" %s%s%s",
		       Variable[W]->Name,
		       ( DifferOnNegs[i] ? "=+" : "=" ),
		       Variable[V]->Name);
	}

	RealDuplicateVars |= ! DifferOnNegs[i];
    }

    Verbose(3)
    {
	if ( NewTPos > 1 )
	{
	    ForEach(i, 0, NConstVars-1)
	    {
		V = A[ j=ConstVarPosn[i] ];

		printf(" %s=", Variable[V]->Name);

		if ( Variable[V]->TypeRef->Continuous )
		{
		    printf("%g", FP(FirstBinding[j]));
		}
		else
		{
		    printf("%s", ConstName[FirstBinding[j]]);
		}
	    }
	}
    }

    if ( Determinate )
    {
	/*  Check whether determinacy has been violated  */

	Determinate = ! NDuplicateVars && NConstVars < NNewVars;

	Verbose(3) printf(" [%s]", Determinate ? "Det" : "XDet");
    }

    /*  Now assess gain if applicable.  Any literal that introduces a
	duplicate variable is unacceptable (the more specific literal
	would give the same result.  A literal R(A) that introduces a
	new variable that replicates an existing variable on pos tuples
	is also excluded -- the more specific literal would match the
	same number of pos tuples and perhaps fewer neg tuples  */

    PossibleT &= ! NDuplicateVars;

    if ( RealDuplicateVars || ( ! PossibleT && ! PossibleF ) )
    {
	Verbose(3) printf(" #\n");
	return;
    }

    /*  Due to arithmetic roundoff, ratios rather than gain are used to
	detect weak literals  */

    CurrentRatio = Current.NPos/(float) Current.NTot;

    if ( PossibleT )
    {
	if ( OrigTPos == OrigTTot && OrigTPos == StartClause.NPos )
	{
	    PosGain = MaxPossibleGain;
	}
	else
	{
	    PosGain = Worth(NowTPos, NewTPos, NewTTot, NNewVars-NConstVars);
	}
	WeakT = NowFTot <= 0 ||
		NewTPos / (NewTTot+1E-3) <= 0.9999 * CurrentRatio;
    }
    else
    {
	PosGain = 0.0;
    }

    if ( PossibleF )
    {
	if ( OrigFPos == OrigFTot && OrigFPos == StartClause.NPos )
	{
	    NegGain = MaxPossibleGain;
	}
	else
	{
	    NegGain = Worth(NowFPos, NowFPos, NowFTot, 0);
	}
	WeakF = NowTTot <= 0 ||
		NowFPos / (NowFTot+1E-3) <= 0.9999 * CurrentRatio;
    }
    else
    {
	NegGain = 0.0;
    }

    Verbose(3)
    {
	printf(" gain %.1f", PosGain);
	if ( NegatedLiteralOK ) printf(",%.1f", NegGain);
    }

    /*  Weak literal sequence check  */

    if ( NWeakLits >= MAXWEAKLITS && ! Determinate &&
	 ( ! PossibleT || WeakT ) && ( ! PossibleF || WeakF ) )
    {
	Verbose(3) printf(" (weak)\n");
	return;
    }

    Verbose(3) putchar('\n');

    /*  Would the addition of this literal to the clause create the best
	saved clause so far? */

    if ( PossibleT &&
	 ( ! PossibleF ||
	   OrigTPos / (float) OrigTTot > OrigFPos / (float) OrigFTot ) )
    {
	SavedSign = 1;
	Accuracy = OrigTPos / (float) OrigTTot;
	Coverage = OrigTPos;
    }
    else
    if ( PossibleF )
    {
	SavedSign = 0;
	Accuracy = OrigFPos / (float) OrigFTot;
	Coverage = OrigFPos;
    }
    else
    {
	Accuracy = 0;
    }

    if ( Accuracy >= AdjMinAccuracy-1E-3 &&
	 ( Accuracy > BestAccuracy ||
	   Accuracy == BestAccuracy && Coverage > BestCover ) )
    {
	if ( ! AlterSavedClause )
	{
	    AlterSavedClause = AllocZero(1, struct _poss_lit_rec);
	}

	AlterSavedClause->Rel      = R;
	AlterSavedClause->Sign     = SavedSign;
	AlterSavedClause->Bits     = LitBits;
	AlterSavedClause->WeakLits = 0;
	AlterSavedClause->TotCov   = ( SavedSign ? OrigTTot : OrigFTot );
	AlterSavedClause->PosCov   = ( SavedSign ? OrigTPos : OrigFPos );

	Size = R->Arity+1;
	if ( HasConst(R) ) Size += sizeof(Const) / sizeof(Var);

	AlterSavedClause->Args = Alloc(Size, Var);
	memcpy(AlterSavedClause->Args, A, Size*sizeof(Var));
    }

    /*  Compute gains and save if appropriate  */

    Gain = Max(PosGain, NegGain);

    if ( Determinate &&
	 Gain < DETERMINATE * MaxPossibleGain &&
	 GoodDeterminateLiteral(R, A, LitBits) )
    {
	return;
    }

    if ( PosGain > 1E-3 &&
	 ( ! SavedClause || SavedClauseAccuracy < .999 || ! WeakT ) )
    {
	ProposeLiteral(R, true, A,
		       NowTTot, LitBits, OrigTPos, OrigTTot, 
		       PosGain, WeakT);
    }

    if ( NegGain > 1E-3 &&
	 ( ! SavedClause || SavedClauseAccuracy < .999 || ! WeakF ) )
    {
	ProposeLiteral(R, false, A,
		       NowFTot, LitBits, OrigFPos, OrigFTot, 
		       NegGain, WeakF);
    }
}



	/*  Initialise variables for scan.  NOTE: this assumes that
	    Current.BaseInfo and MaxPossibleGain have been set externally.  */


void  PrepareForScan(Relation R, Var *A)
/*    --------------  */
{
    int i, j, PossibleVarComps;
    Var V, W, VP;

    /*  First time through, allocate arrays  */

    if ( Allocate )
    {
	Allocate = false;

	PossibleVarComps = MAXARGS * MAXVARS + (MAXARGS * (MAXARGS-1) / 2);
	OldVar       = Alloc(PossibleVarComps, Var);
	NewVarPosn   = Alloc(PossibleVarComps, Var);

	ConstVarPosn    = Alloc(MAXARGS+1, Var);
	DifferOnNegs    = Alloc((MAXARGS+1)*(MAXVARS+1), Boolean);
	FirstOccurrence = Alloc(MAXVARS+MAXARGS, Boolean);
	FirstBinding    = Alloc(MAXARGS+1, Const);
    }

    OrigTPos = 0;
    OrigTTot = 0;
    OrigFPos = 0;
    OrigFTot = 0;

    NowTPos = 0;
    NowTTot = 0;
    NowFPos = 0;
    NowFNeg = 0;
    NowMPos = 0;
    NowMTot = 0;

    NewTPos = 0;
    NewTTot = 0;

    NDuplicateVars = NNewVars = NConstVars = 0;

    NArgs = R->Arity;
    memset(FirstOccurrence, true, Current.MaxVar+NArgs);

    OutOfWorld = false;

    ForEach(i, 1, NArgs)
    {
	if ( (V = A[i]) > Current.MaxVar && FirstOccurrence[V] )
	{
	    NNewVars++;
	    FirstOccurrence[V] = false;

	    ConstVarPosn[NConstVars++] = i;

	    ForEach(W, 1, Current.MaxVar)
	    {
		if ( ! Compatible[Variable[W]->Type][R->Type[i]] ) continue;

		NewVarPosn[NDuplicateVars]   = i;
		OldVar[NDuplicateVars]       = W;
		DifferOnNegs[NDuplicateVars] = false;
		NDuplicateVars++;
	    }
	}
    }

    /*  New variables shouldn't replicate each other, either  */

    ForEach(i, 0, NConstVars-2)
    {
	VP = ConstVarPosn[i];
	V  = A[VP];

	ForEach(j, i+1, NConstVars-1)
	{
	    W = ConstVarPosn[j];

	    if ( ! Compatible[Variable[V]->Type][R->Type[W]] ) continue;

	    NewVarPosn[NDuplicateVars]   = W;
	    OldVar[NDuplicateVars]       = Current.MaxVar+VP;	/* special */
	    DifferOnNegs[NDuplicateVars] = false;
	    NDuplicateVars++;
	}
    }

    ClearFlags;

    PossibleT = true;
    PossibleF = NegatedLiteralOK =
		  ( NEGLITERALS ||
		    R == GTVAR || R == GTCONST ||
		    ( NEGEQUALS && ( R == EQVAR || R == EQCONST ) ) );

    Determinate = NNewVars > 0;

    /*  The minimum gain that would be of interest is just enough to give
	a literal a chance to be saved by the backup procedure or, if
	there are determinate literals, to reach the required fraction
	of the maximum possible gain  */

    MinUsefulGain = NPossible < MAXPOSSLIT ? MINALTFRAC * BestLitGain :
		    Max(Possible[MAXPOSSLIT]->Gain, MINALTFRAC * BestLitGain);

    if ( NDeterminate && MinUsefulGain < DETERMINATE * MaxPossibleGain )
    {
	MinUsefulGain = DETERMINATE * MaxPossibleGain;
    }

    /*  Set thresholds for pos tuples  */

    MinPos = MinUsefulGain / Current.BaseInfo - 0.001;

    /*  Now check coverage required for a saveable clause that would pass
	the MDL criterion.  Don't worry about long saveable clauses.  */

    if ( AlterSavedClause )
    {
	BestCover    = AlterSavedClause->PosCov;
	BestAccuracy = BestCover / (float) AlterSavedClause->TotCov;
    }
    else
    {
	BestCover    = SavedClauseCover;
	BestAccuracy = SavedClauseAccuracy;
    }

    if ( NLit < 5 )
    {
	MinSaveableCover = BestCover+1;
	while ( Encode(MinSaveableCover) <= NewClauseBits ) MinSaveableCover++;
    }
    else
    {
	MinSaveableCover = StartDef.NPos;
    }
}



	/*  Make a pass through the tuples, terminating if it becomes clear
	    that neither R(A) or ~R(A) can achieve the minimum useful gain.
	    Since all pos tuples appear first in the tuple sets, thresholds
	    for NewTNeg and NowFNeg can be set when the first neg tuple
	    is encountered.  */


#define  TermTest(Cond, Test)\
	    if ( Cond && Test && ! Determinate ) {\
		 Cond = false; if ( ! PossibleT && ! PossibleF ) return true; }
		

Boolean  TerminateScan(Relation R, Var *A)
/*       -------------  */
{
    Tuple	*TSP, Case;
    Boolean	BuiltIn=false, FirstNegTuple=true;
    int		RN, MaxCover, OrigPos=0;
    Const	X2;
    float	NewTNegThresh, NowFNegThresh;

    if ( Predefined(R) )
    {
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

    for ( TSP = Current.Tuples ; Case = *TSP++ ; )
    {
	if ( FirstNegTuple && ! Positive(Case) )
	{
	    /*  Encoding length checks  */

	    PossibleT &= Encode(OrigTPos) > NewClauseBits;
	    PossibleF &= Encode(OrigFPos) > NewClauseBits;

	    if ( ! PossibleT && ! PossibleF )
	    {
		Verbose(3)
		{
		    printf("  MDL prune %d,%d", OrigTPos, OrigFPos);
		}
		return true;
	    }

	    /*  Set thresholds now that NowTPos and NowFPos are known  */

	    NewTNegThresh = ( NNewVars && BestLitGain < 1E-2 ? MAXTUPLES :
			      NegThresh(NowTPos, NewTPos) );
	    NowFNegThresh = NegThresh(NowFPos, NowFPos);
	    FirstNegTuple = false;
	}
		
	if ( MissingValue(R, A, Case) )
	{
	    NowMTot++;
	    if ( Positive(Case) ) NowMPos++;
	    NFound = 0;
	}
	else
	if ( BuiltIn ? Satisfies(RN, A[1], X2, Case) :
	     Join(R->Pos, R->PosIndex, A, Case, NArgs, false) )
	{
	    /*  R(A) is barred if it would introduce an out-of-world constant.
		Note: can't use TermTest() since check for Determinate does
		not matter  */

	    if ( OutOfWorld )
	    {
		PossibleT = false;
		if ( ! PossibleF ) return true;
	    }
		
	    /*  Extensions of this tuple from R(A)  */

	    CheckNewVars(Case);

	    NowTTot++;
	    NewTTot += NFound;

	    TermTest(PossibleT,
		     NewTTot > MAXTUPLES);

	    if ( Positive(Case) )
	    {
		NowTPos++;
		NewTPos += NFound;

		/*  If all remaining pos tuples go to NowFPos, are there
		    sufficient to make ~R(A) viable?
		    Note: do not abandon ~R(A) if it could lead to a
		    saveable clause (assuming all remaining original
		    pos tuples are covered by ~R(A))  */

		MaxCover = OrigFPos + (Current.NOrigPos - OrigPos);
		if ( MaxCover < MinSaveableCover )
		{
		    TermTest(PossibleF,
			     Current.NPos - (NowTPos + NowMPos) < MinPos-1E-3);
		}
	    }
	    else
	    {
		/*  We already know the final number of NewTPos tuples.
		    Are there now enough NewTNeg tuples to make the gain of
		    R(A) insufficient?  (Note: since R(A) matches a neg tuple,
		    don't have to worry about saveable clause.)  */

		TermTest(PossibleT,
			 (NewTTot - NewTPos) > NewTNegThresh+1E-3);
	    }

	    if ( ! TestFlag(Case[0]&Mask, TrueBit) )
	    {
		SetFlag(Case[0]&Mask, TrueBit);
		OrigTTot++;
		if ( Positive(Case) )
		{
		    OrigTPos++;
		    if ( ! TestFlag(Case[0]&Mask, FalseBit) ) OrigPos++;
		}
	    }
	}
	else
	{
	    if ( Positive(Case) )
	    {
		NowFPos++;

		/*  If all remaining pos tuples go to NowTPos, are there
		    sufficient to make R(A) viable?
		    Note: don't kill R(A) if it could lead to a saveable
		    clause when all remaining original pos tuples covered
		    by R(A)  */

		MaxCover = OrigTPos + (Current.NOrigPos - OrigPos);
		if ( MaxCover < MinSaveableCover )
		{
		    TermTest(PossibleT,
			     Current.NPos - (NowFPos + NowMPos) < MinPos-1E-3);
		}
	    }
	    else
	    {
		NowFNeg++;

		/*  We already know the final number of NowFPos tuples.
		    Are there already enough NowFNeg tuples to make the gain of
		    ~R(A) insufficient? (As above saveability not relevant.)  */

		TermTest(PossibleF,
			 NowFNeg > NowFNegThresh+1E-3);
	    }

            if ( ! TestFlag(Case[0]&Mask, FalseBit) )
	    {
                SetFlag(Case[0]&Mask, FalseBit);
                OrigFTot++;
                if ( Positive(Case) )
		{
		    OrigFPos++;
		    if ( ! TestFlag(Case[0]&Mask, TrueBit) ) OrigPos++;
		}
	    }
	}

	Determinate &= ( Positive(Case) ? NFound == 1 : NFound <= 1 );
    }

    return false;
}



	/*  If there are unbound variables, try to satisfy the
	    pruning criterion for more specific literals.

	    A more specific negated literal will cover more tuples;
	    NowFNeg must be great enough so that, if all positive tuples
	    were covered by ~R(A), the gain would still be too low.  */


void  CheckForPrune(Relation R, Var *A)
/*    -------------  */
{
    Tuple	*TSP, Case;
    int		RemainingNeg;

    RemainingNeg = (Current.NTot - Current.NPos)
		   - (NowMTot - NowMPos) - (NowTTot - NowTPos) - NowFNeg;

    for ( TSP = Current.Tuples + (NowMTot+NowTTot+NowFPos+NowFNeg) ;
	  Case = *TSP++ ; )
    {
	if ( Positive(Case) || MissingValue(R, A, Case) )
	{	
	    continue;
	}

	if ( ! Join(R->Pos, R->PosIndex, A, Case, NArgs, true) )
	{
	    NowFNeg++;

	    /*  See if have found enough  */

	    if ( NowFNeg > MaxFNeg ) break;
	}

	RemainingNeg--;

	/*  See whether not enough left  */

	if ( NowFNeg + RemainingNeg <= MaxFNeg ) break;
    }
}



	/*  Check new variables for non-utility, specifically
	    *  replicating existing variables on all or pos tuples
	    *  all being bound to constants on pos tuples  */


void  CheckNewVars(Tuple Case)
/*    ------------  */
{
    Var		P;
    Const	OldVarVal;
    int		i, j, Col;

    for ( i = 0 ; i < NDuplicateVars ; i++ )
    {
	if ( ! Positive(Case) && DifferOnNegs[i] ) continue;

	P = OldVar[i];
	if ( P <= Current.MaxVar ) OldVarVal = Case[P];

	Col = NewVarPosn[i];

	for ( j = 0 ; j < NFound ; j++ )
	{
	    if ( Found[j][Col] == ( P <= Current.MaxVar ? OldVarVal :
				                  Found[j][P-Current.MaxVar] ) )
	{
	    continue;
	}

	    if ( Positive(Case) )
	    {
		NDuplicateVars--;

		for ( j = i ; j < NDuplicateVars ; j++ )
		{
		    NewVarPosn[j]   = NewVarPosn[j+1];
		    OldVar[j]       = OldVar[j+1];
		    DifferOnNegs[j] = DifferOnNegs[j+1];
		}
		i--;
	    }
	    else
	    {
		DifferOnNegs[i] = true;
	    }

	    break;
	}
    }

    /*  Check for new vars bound to constants  */

    if ( NConstVars )
    {
	if ( ! NowTTot )
	{
	    memcpy(FirstBinding, Found[0], (NArgs+1)*sizeof(Const));
	}

	ForEach(i, 0, NConstVars-1)
	{
	    Col = ConstVarPosn[i];

	    for ( j = 0 ; j < NFound ; j++ )
	    {
		if ( FirstBinding[Col] == Found[j][Col] ) continue;

		NConstVars--;

		for ( j = i ; j < NConstVars ; j++ )
		{
		    ConstVarPosn[j] = ConstVarPosn[j+1];
		}

		i--;
		break;
	    }
	}
    }
}



	/*  Compute the maximum number N1 of neg tuples that would allow
	    P1 pos tuples (P orig pos tuples) to give a gain >= threshold.
	    The underlying relation is
		P * (Current.BaseInfo + log(P1/(P1+N1)) >= MinUsefulGain
	    where N1 is adjusted by the sampling factor.

	    NOTE: This is the inverse of the gain calculation in Worth.
	    If one is changed, the other must be modified accordingly  */


float  NegThresh(int P, int P1)
/*     ---------  */
{
    return P <= 0 ? 0.0 :
	   SAMPLE *
		(P1+1) * (exp(LN2 * (Current.BaseInfo - MinUsefulGain/P)) - 1);
}



	/*  Compute aggregate gain from a test on relation R, tuple T.
	    The Basic gain is the number of positive tuples * information
	    gained regarding each; but there is a minor adjustment:
	      - a literal that has some positive tuples and no gain but
		introduces one or more new variables, is given a slight gain  */


float  Worth(int N, int P, int T, int UV)
/*     -----  */
{
    float G, TG;

    TG = N * (G = Current.BaseInfo - Info(P, T));

    if ( G < 1E-3 && N && UV )
    {
	return 0.0009 + UV * 0.0001;  /* very small notional gain */
    }
    else
    {
	return TG;
    }
}



	/*  The ratio P/T is tweaked slightly to (P+1)/(T+1) so that, if
	    two sets of tuples have the same proportion of pos tuples,
	    the smaller is preferred.  The reasoning is that it is easier
	    to filter out all neg tuples from a smaller set.  If you don't
	    like this idea and change it back to P/T, NegThresh must be
	    changed also  */


float  Info(int P, int T)
/*     ----  */
{
    /*  Adjust total T to take account of sampling  */

    T = (int)((float)(T-P) / SAMPLE) + P;

    return Log2(T+1) - Log2(P+1);
}



Boolean  MissingVal(Relation R, Var *A, Tuple T)
/*       ----------       */
{
    register Var i, V;

    ForEach(i, 1, R->Arity)
    {
	V = A[i];

        if ( V <= Current.MaxVar && Unknown(V, T) ) return true;
    }

    return false;
}



Boolean  Unknown(Var V, Tuple T)
/*       -------  */
{
    return ( Variable[V]->TypeRef->Continuous ?
	     FP(T[V]) == MISSING_FP : T[V] == MISSING_DISC );
}



	/*  See whether a case satisfies built-in relation RN  */

Boolean  Satisfies(int RN, Const V, Const W, Tuple Case)
/*       ---------  */
{
    switch ( RN )
    {
	case 0:	/* EQVAR */
		NFound = ( Case[V] == Case[W] );
		break;

	case 1:	/* EQCONST */
		NFound = ( Case[V] == W );
		break;

	case 2:	/* GTVAR */
		NFound = ( FP(Case[V]) > FP(Case[W]) );
		break;

	case 3:	/* GTCONST */
		NFound = ( FP(Case[V]) > FP(W) );
		break;

	default:	exit(0);
    }

    return NFound;
}



	/*  The following stuff calculates thresholds for GTCONST relations.

	    The pos and neg tuples are sorted separately, then merged.
	    The current value is acceptable as a threshold unless it is
	    in the middle of a run of tuples of the same sign or a run
	    of the same value  */


float	BestGain, BestThresh;
int	BestTPos, BestTTot;


void  FindThreshold(Var *A)
/*    -------------  */
{
    Tuple	*ScanPos, *ScanNeg, Case;
    float	PosVal, NegVal, ThisVal, PrevVal=(-1E30);
    Var		V;
    int		ThisSign, NextSign, Signs=0, i;

    V = A[1];

    BestGain = -1E10;
    PossibleCuts = 0;

    NowMPos = MissingAndSort(V, 0, Current.NPos-1);
    NowMTot = NowMPos + MissingAndSort(V, Current.NPos, Current.NTot-1);

    NowTPos = Current.NPos - NowMPos;
    NowTTot = Current.NTot - NowMTot;

    ScanPos = &Current.Tuples[NowMPos];
    ScanNeg = &Current.Tuples[Current.NPos + (NowMTot - NowMPos)];
    PosVal = ( NowTPos ? FP(((*ScanPos)[V])) : 1E30 );
    NegVal = ( NowTTot > NowTPos ? FP(((*ScanNeg)[V])) : 1E30 );

    while ( NowTTot >= 1 )
    {
	if ( PosVal <= NegVal )
	{
	    NowTPos--;
	    NowTTot--;
	    ScanPos++;
	    ThisVal = PosVal;
	    PosVal = ( NowTPos ? FP(((*ScanPos)[V])) : 1E30 );
	    ThisSign = 1;
	}
	else
	{
	    NowTTot--;
	    ScanNeg++;
	    ThisVal = NegVal;
	    NegVal = ( NowTTot > NowTPos ? FP(((*ScanNeg)[V])) : 1E30 );
	    ThisSign = 2;
	}

	if ( ThisVal == PrevVal )
	{
	    Signs |= ThisSign;
	}
	else
	{
	    Signs = ThisSign;
	    PrevVal = ThisVal;
	    PossibleCuts++;
	}

	NextSign = ( PosVal == NegVal ? 3 : PosVal < NegVal ? 1 : 2 );

	if ( NowTTot && ThisVal != PosVal && ThisVal != NegVal &&
	     (Signs | NextSign) == 3 )
	{
	    PossibleCut(ThisVal);
	}
    }
    PossibleCuts--;

    /*  Fix up all required counts  */

    if ( BestGain >= 0 )
    {
	NewTPos = NowTPos = BestTPos;
	NewTTot = NowTTot = BestTTot;

	NowFPos = Current.NPos - NowTPos - NowMPos;
	NowFNeg = (Current.NTot - Current.NPos)
		    - (NowTTot - NowTPos) - (NowMTot - NowMPos);

	SaveParam(&A[2], &BestThresh);
	Verbose(3) printf("%g", BestThresh);

	/* Now determine coverage of original tuples  */

	ClearFlags;
	OrigTPos = OrigTTot = OrigFPos = OrigFTot = 0;

	ForEach(i, 0, Current.NTot-1)
	{
	    Case = Current.Tuples[i];
	    ThisVal = FP(Case[V]);

	    if ( ThisVal == MISSING_FP ) continue;

	    if ( ThisVal > BestThresh )
	    {
		if ( ! TestFlag(Case[0]&Mask, TrueBit) )
		{
		    SetFlag(Case[0]&Mask, TrueBit);
		    OrigTTot++;
		    if ( Positive(Case) ) OrigTPos++;
		}
	    }
	    else
	    {
		if ( ! TestFlag(Case[0]&Mask, FalseBit) )
		{
		    SetFlag(Case[0]&Mask, FalseBit);
		    OrigFTot++;
		    if ( Positive(Case) ) OrigFPos++;
		}
	    }
	}

	/*  Encoding length checks  */

	PossibleT &= Encode(OrigTPos) > NewClauseBits;
	PossibleF &= Encode(OrigFPos) > NewClauseBits;
    }
    else
    {
	NewTPos = NowTPos = Current.NPos;
	NewTTot = NowTTot = Current.NTot;
	NowFPos = NowFNeg = 0;
	PossibleT = PossibleF = false;
    }
}



void  PossibleCut(float C)
/*    -----------  */
{
    float	TGain, FGain, Better;

    TGain = Worth(NowTPos, NowTPos, NowTTot, 0);
    FGain = Worth(Current.NPos-NowTPos-NowMPos,
		  Current.NPos-NowTPos-NowMPos,
		  Current.NTot-NowTTot-NowMTot, 0);
    Better = Max(TGain, FGain);

    if ( Better > BestGain )
    {
	BestGain   = Better;
	BestThresh = C;
	BestTPos   = NowTPos;
	BestTTot   = NowTTot;
    }
}



	/*  Count missing values and sort the remainder  */

Tuple			Hold;
#define  Swap(V,A,B)	{ Hold = V[A]; V[A] = V[B]; V[B] = Hold; }


int  MissingAndSort(Var V, int Fp, int Lp)
/*   --------------  */
{
    int	i, Xp;

    /*  Omit and count unknown values */

    Xp = Fp;
    ForEach(i, Fp, Lp)
    {
	if ( FP(Current.Tuples[i][V]) == MISSING_FP )
	{
	    Swap(Current.Tuples, Xp, i);
	    Xp++;
	}
    }

    Quicksort(Current.Tuples, Xp, Lp, V);

    return Xp - Fp;
}



void  Quicksort(Tuple *Vec, int Fp, int Lp, Var V)
/*    ---------  */
{
    register int Middle, i;
    register float Thresh;

    if ( Fp < Lp )
    {
	Thresh = FP(Vec[ (Fp+Lp)/2 ][V]);

	/*  Isolate all items with values < threshold  */

	Middle = Fp;

	for ( i = Fp ; i <= Lp ; i++ )
	{ 
	    if ( FP(Vec[i][V]) < Thresh )
	    { 
		if ( i != Middle ) Swap(Vec, Middle, i);
		Middle++; 
	    } 
	} 

	/*  Sort the lower values  */

	Quicksort(Vec, Fp, Middle-1, V);

	/*  Extract all values equal to the threshold  */

	for ( i = Middle ; i <= Lp ; i++ )
	{
	    if ( FP(Vec[i][V]) == Thresh )
	    { 
		if ( i != Middle ) Swap(Vec, Middle, i);
		Middle++;
	    } 
	} 

	/*  Sort the higher values  */

	Quicksort(Vec, Middle, Lp, V);
    }
}
