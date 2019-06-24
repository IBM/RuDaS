/******************************************************************************/
/*									      */
/*	Routines concerned with discovering a plausible ordering of the	      */
/*	constants of each type.  There are three phases:		      */
/*	  *  finding possible orderings on pairs of relation arguments	      */
/*	  *  finding a partial ordering that satisfies as many of these	      */
/*	     as possible						      */
/*	  *  selecting a constant ordering consistent with the partial	      */
/*	     ordering							      */
/*									      */
/******************************************************************************/


#include  "defns.i"
#include  "extern.i"

Boolean		**Table=Nil;		/* partial order for target type */

int		TTN,			/* target type number */
		NC,			/* size of target type */
		*TTCollSeq,		/* collation sequence */
		NArgOrders = 0,		/* number of possible arg orders */
		MaxConsistent;		/* max consistent arg orders */

ArgOrder	*ArgOrderList = Nil;


	/*  Find ordering for constants of each type  */

void  OrderConstants()
/*    --------------  */
{
    int		i, j;
    Boolean	**PO;
    Tuple	*NLT;

    ForEach(TTN, 1, MaxType)
    {
	if ( Type[TTN]->FixedPolarity ) continue;

	NC = Type[TTN]->NValues;
	TTCollSeq = Type[TTN]->CollSeq;
	Verbose(3) printf("\nOrdering constants of type %s\n",Type[TTN]->Name);

	if ( ! Table ) Table = AllocatePartOrd(MaxConst);

	FindArgumentOrders();

	if ( NArgOrders == 0 )
	{
	    Type[TTN]->Ordered = false;
	    Verbose(3) printf("\t\tunordered\n");
	    continue;
	}
	else
	{
	    Type[TTN]->Ordered = true;
	}

	/*  Assemble in Table the partial order consistent with the maximum
	    number of arg orders  */

	MaxConsistent = 0;
	PO = AllocatePartOrd(NC);
	ClearPartOrd(PO);
	Verbose(3) printf("\tFinding maximal consistent set\n");

	FindConsistentSubset(0, 0, PO);
	FreePartOrd(PO, NC);

	/*  Sort constants on number of entries in partial order; resolve
	    ties in favour of the initial constant order  */

	NLT = Alloc(NC, Tuple);
	ForEach(i, 0, NC-1)
	{
	    NLT[i] = Alloc(2, Const);
	    NLT[i][0] = Type[TTN]->Value[i];
	    FP(NLT[i][1]) = CountEntries(i+1) + i / (float) NC;
	}
	Quicksort(NLT, 0, NC-1, 1);

	/*  Change collation sequence and print message  */

	Verbose(3) printf("\tFinal order:\n\t\t");
	ForEach(i, 0, NC-1)
	{
	    j = NLT[i][0];
	    Type[TTN]->CollSeq[j] = i+1;
	    Verbose(3) printf("%s ", ConstName[j]);
	    pfree(NLT[i]);
	}
	Verbose(3) putchar('\n');

	pfree(NLT);
    }

    if ( Table ) FreePartOrd(Table, MaxConst);

    ForEach(i, 0, NArgOrders-1)
    {
	pfree(ArgOrderList[i]);
    }
    pfree(ArgOrderList);
}


	/*  Find potential orderings between pairs of arguments of each
	    relation for type TTN  */


void  FindArgumentOrders()
/*    ------------------  */
{
    int		i;
    Relation	R;

    ForEach(i, 0, MaxRel)
    {
	R = RelnOrder[i];
	if ( Predefined(R) || R->Arity < 2 ) continue;

	ExamineArgumentPairs(R, true, R->Pos);

	if ( R->Neg )
	{
	    ExamineArgumentPairs(R, false, R->Neg);
	}
    }
}



	/*  Find potential orderings between pairs of arguments of R where
	    relevant to the type under investigation  */


void  ExamineArgumentPairs(Relation R, Boolean Sign, Tuple *TP)
/*    --------------------    */
{
    int	FirstArg, SecondArg;

    Verbose(3)
	printf("\tChecking arguments of %s%s\n", Sign ? "" : "~", R->Name);

    ForEach(FirstArg, 1, R->Arity-1)
    {
	if ( ! Compatible[R->Type[FirstArg]][TTN] ) continue;

	ForEach(SecondArg, FirstArg+1, R->Arity)
	{
	    if ( ! Compatible[R->Type[SecondArg]][TTN] ) continue;

	    Verbose(3) 
		printf("\t\targuments %d,%d ", FirstArg, SecondArg);

	    ClearPartOrd(Table);
	    if ( ConsistentClosure(Table, TP, FirstArg, SecondArg) )
	    {
		Verbose(3) printf("are consistent\n");
		AddArgOrder(R, Sign, FirstArg, SecondArg);
	    }
	    else
	    {
		Verbose(3) printf("are not consistent\n");
	    }
	}
    }
}



	/*  Investigate args A and B of a set of tuples TP.  See whether each
	    pair of constants is consistent with TP; if so, add and form
	    closure  */

Boolean  ConsistentClosure(Boolean **PO, Tuple *TP, Var A, Var B)
/*       -----------------  */
{
    Const	K, L;
    int		i, j;

    while( *TP )
    {
	K = (*TP)[A];
	L = (*TP)[B];
	TP++;

	if ( K == MISSING_DISC || K == OUT_OF_RANGE ||
	     L == MISSING_DISC || L == OUT_OF_RANGE ) continue;

	/*  Not consistent if either constant missing from type or if
	    current pair in conflict with existing table  */

	if ( (i = TTCollSeq[K]) == 0 || (j = TTCollSeq[L]) == 0  ||
	     ! AddPair(PO, i, j) )
	{
	    return false;
	}
    }

    return true;
}
	


	/*  Note partial order between A and B; add to table if not already
	    there and generate closure.  Return false if the table is no
	    longer consistent  */

Boolean  AddPair(Boolean **PO, int A, int B)
/*       -------  */
{
    int	i, j;

    if ( PO[A][B] ) return true;		/* already there */
    else
    if ( A == B || PO[B][A] ) return false;	/* not consistent */

    ForEach(i, 1, NC)
    {
	if ( i == A || PO[i][A] )
	{
	    ForEach(j, 1, NC)
	    {
		if ( j == B || PO[B][j] )
		{
		    if ( PO[j][i] ) return false;

		    PO[i][j] = true;
		}
	    }
	}
    }

    return true;
}



void  AddArgOrder(Relation R, Boolean Sign, int A1, int A2)
/*    -----------  */
{
    ArgOrder	AO;

    if ( NArgOrders % 100 == 0 )
    {
	ArgOrderList = Realloc(ArgOrderList, NArgOrders+100, ArgOrder);
    }

    ArgOrderList[NArgOrders++] = AO = Alloc(1, struct _arg_ord_rec);

    AO->Rel  = R;
    AO->Sign = Sign;
    AO->A1   = A1;
    AO->A2   = A2;
    AO->In   = 0;
}


	/*  Routines for constant partial order tables  */

Boolean  **AllocatePartOrd(int Size)
/*         ---------------   */
{
    Boolean	**PO;
    int		i;

    PO = Alloc(Size+1, Boolean *);

    ForEach(i, 1, Size)
    {
	PO[i] = Alloc(Size+1, Boolean);
    }

    return PO;
}



void  FreePartOrd(Boolean **PO, int Size)
/*    -----------   */
{
    int i;

    ForEach(i, 1, Size)
    {
	pfree(PO[i]);
    }

    pfree(PO);
}


void  ClearPartOrd(Boolean **PO)
/*    ------------  */
{
    int i;

    ForEach(i, 1, NC)
    {
	memset(PO[i], false, NC+1);
    }
}


void  CopyPartOrd(Boolean **To, Boolean **From)
/*    -----------  */
{
    int i;

    ForEach(i, 1, NC)
    {
	memcpy(To[i], From[i], (NC+1)*sizeof(Boolean));
    }
}



void  FindConsistentSubset(int Included, int TryNext, Boolean **PO)
/*    --------------------  */
{
    Boolean	**CopyPO;
    ArgOrder	AO;
    Tuple	*Entries;
    int		i;

    if ( Included > MaxConsistent )
    {
	/*  Note best consistent partial order so far  */

	CopyPartOrd(Table, PO);
	MaxConsistent = Included;

	Verbose(3)
	{
	    printf("\t\tbest so far");
	    ForEach(i, 0, NArgOrders-1)
	    {
		AO = ArgOrderList[i];
		if ( AO->In )
		{
		    printf(" %s%s:", AO->Sign ? "" : "~", AO->Rel->Name);
		    if ( AO->In == 1 )
		    {
			printf("%d>%d", AO->A1, AO->A2);
		    }
		    else
		    {
			printf("%d>%d", AO->A2, AO->A1);
		    }
		}
	    }
	    putchar('\n');
	}
    }

    if ( TryNext >= NArgOrders ||
	 Included + (NArgOrders - TryNext) <= MaxConsistent )
    {
	return;
    }

    AO = ArgOrderList[TryNext];
    Entries = AO->Sign ? AO->Rel->Pos : AO->Rel->Neg;
    CopyPO = AllocatePartOrd(NC);
    CopyPartOrd(CopyPO, PO);

    if ( ConsistentClosure(PO, Entries, AO->A1, AO->A2) )
    {
	AO->In = 1;
	FindConsistentSubset(Included+1, TryNext+1, PO);
    }

    /*  Do not have to try both polarities of first argument ordering  */

    if ( Included > 0 )
    {
	CopyPartOrd(PO, CopyPO);
	if ( ConsistentClosure(PO, Entries, AO->A2, AO->A1) )
	{
	    AO->In = -1;
	    FindConsistentSubset(Included+1, TryNext+1, PO);
	}
    }

    CopyPartOrd(PO, CopyPO);
    AO->In = 0;
    FindConsistentSubset(Included, TryNext+1, PO);

    FreePartOrd(CopyPO, NC);
}



int  CountEntries(int K)
/*   ------------  */
{
    int i, Sum=0;

    ForEach(i, 1, NC)
    {
	if ( Table[K][i] ) Sum++;
    }

    return Sum;
}
