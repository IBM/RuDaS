/******************************************************************************/
/*									      */
/*	Routines for controlling recursive definitions.  The basic idea	      */
/*	is to keep track of partial orders between variables (using	      */
/*	either predefined or discovered constant orders) and to ensure	      */
/*	that there is an ordering of all recursive literals that will	      */
/*	guarantee termination.  See Cameron-Jones and Quinlan, IJCAI'93	      */
/*									      */
/******************************************************************************/


#include "defns.i"
#include "extern.i"


    /*  Examine relationships among variables: LHSVar <,=,>,#  anyvar and
	anyvar = anyvar  */

void  ExamineVariableRelationships()
/*    ---------------------------- */
{
    Var		V, W;
    Const	VVal, WVal;
    Ordering	X, ThisX;
    Tuple	*Scan, Case;
    int		*TypeOrder;
    Boolean	FirstTime=true;

    /*  First reset all partial orders  */

    ForEach(V, 1, Target->Arity)
    {
	memset(PartialOrder[V], '#', Current.MaxVar+1);
	PartialOrder[V][V] = '=';
    }

    ForEach(V, 1, Current.MaxVar-1)
    {
	if ( Variable[V]->TypeRef->Continuous ) continue;

	if ( V <= Target->Arity )
	{
	    TypeOrder = Target->TypeRef[V]->CollSeq;
	}

	ForEach(W, V+1, Current.MaxVar)
	{
	    if ( Variable[W]->TypeRef->Continuous ||
		 ! Compatible[Variable[V]->Type][Variable[W]->Type] )  continue;

	    for ( X = 0, Scan = Current.Tuples ; X != '#' && *Scan ; Scan++ )
	    {
		Case = *Scan;

		if ( (VVal = Case[V]) == (WVal = Case[W]) )
		{
		    ThisX = '=';
		}
		else
		if ( V > Target->Arity ||
		     ! Variable[V]->TypeRef->Ordered ||
		     ! Variable[W]->TypeRef->Ordered )
		{
		    ThisX = '#';
		}
		else
		{
		    ThisX = ( TypeOrder[VVal] < TypeOrder[WVal] ? '<' : '>' );
		}

		if ( ! X )
		{
		    X = ThisX;
		}
		else
		if ( X != ThisX )
		{
		    X = '#';
		}
	    }

	    if ( X != '#' )
	    {
		Verbose(2)
		{
		    if ( FirstTime )
		    {
			printf("\t\tNote");
			FirstTime = false;
		    }
		    printf(" %s%c%s", Variable[V]->Name, X, Variable[W]->Name);
		}
	    }

	    if ( X == '=' ) Barred[W] = true;

	    /*  Record partial order for recursive literals.  If polarity
		is fixed, treat > as #  */

	    if ( V <= Target->Arity && X != '#' )
	    {
		ThisX = PartialOrder[V][W] = X;
		AnyPartialOrder |= ThisX == '<' || ThisX == '>';

		if ( W <= Target->Arity )
		{
		    ThisX = PartialOrder[W][V] =
			X == '<' ? '>' : X == '>' ? '<' : '=';
		    AnyPartialOrder |= ThisX == '<' || ThisX == '>';
		}
	    }
	}
    }
    Verbose(2) putchar('\n');
}



	/*  Vet proposed arguments for recursive literal.
	    Uses a mapping from ThisOrder x Cell to ThisOrder  */


Boolean  RecursiveCallOK(Var *A)
/*       ---------------  */
{
    int			i, j, k, N, NRowLeft, Count, NRow, BestCount, BestCol;
    Ordering		*ThisCall, ThisOrder, BestOrder, Cell;
    Boolean		SomeInequality=false, ColOrder[100];

    static Ordering	**Map=Nil;
    static Boolean	*ColLeft, *RowLeft;

    if ( ! AnyPartialOrder ) return false;

    if ( ! Map )
    {
	N = (int) '>';	/* max of '#', '<', '>', '=' */

	Map = Alloc(N+1, Ordering *);
	
	/*  We want the final value for a column to be
		'=' if it contains only '=' entries
		'<' if it contains only '<' and/or '=' entries
		'>' if it contains only '>' and/or '=' entries
	    and '#' otherwise  */

	Map['='] = Alloc(N+1, Ordering);
	Map['<'] = Alloc(N+1, Ordering);
	Map['>'] = Alloc(N+1, Ordering);
	Map['#'] = Alloc(N+1, Ordering);

	Map['=']['='] = '=';
	Map['=']['<'] = '<';
	Map['=']['>'] = '>';
	Map['=']['#'] = '#';

	Map['<']['='] = '<';
	Map['<']['<'] = '<';
	Map['<']['>'] = '#';
	Map['<']['#'] = '#';

	Map['>']['='] = '>';
	Map['>']['<'] = '#';
	Map['>']['>'] = '>';
	Map['>']['#'] = '#';

	Map['#']['='] = '#';
	Map['#']['<'] = '#';
	Map['#']['>'] = '#';
	Map['#']['#'] = '#';

	ColLeft = Alloc(MAXARGS+1, Boolean);
	RowLeft = Alloc(1001, Boolean);	/* assume <1000 recursive lits! */
    }

    N = Target->Arity;

    memset(ColLeft, true, N+1);

    NRow = NRowLeft = NRecLitClause + NRecLitDef;
    memset(RowLeft, true, NRow+1);

    /*  First need to establish ordering constraints for these arguments.
	(Skip this if A is nil, called from pruning routines)  */

    if ( A )
    {
	ThisCall = RecursiveLitOrders[0];
	ForEach(i, 1, N)
	{
	    if ( A[i] > Current.MaxVar )
	    {
		ThisCall[i] = '#';
	    }
	    else
	    {
		ThisCall[i] = PartialOrder[i][A[i]];
		SomeInequality |= ThisCall[i] == '<' || ThisCall[i] == '>';
	    }
	}

	if ( ! SomeInequality ) return false;
    }
    else
    {
	NRowLeft--;
	RowLeft[0] = false;
    }

    /*  Check for a possible ordering by
	* finding a column that has only (< or >) and = orders
	* delete rows containing (< or >)
	* continue until no rows remain  */

    /*  This routine is also invoked during the pruning phase, when some
	literals have been (perhaps temporarily) removed from the most
	recent clause.  Their orderings are marked as inactive; these are
	treated as if already covered  */

    ForEach(j, NRecLitDef+1, NRow)
    {
	if ( RecursiveLitOrders[j][0] )
	{
	    RowLeft[j] = false;
	    NRowLeft--;
	}
    }

    /*  Find the initial column ordering constraints  */

    ForEach(k, 1, N)
    {
	ColOrder[k] = ( Target->TypeRef[k]->FixedPolarity ? '>' : '=' );
    }

    while( NRowLeft >= 0 )
    {
        BestCol = BestCount = 0;

	ForEach(k, 1, N)
	{
	    if ( ! ColLeft[k] ) continue;

	    Count = 0;
	    ThisOrder = ColOrder[k];

	    for ( j = 0 ; ThisOrder != '#' && j <= NRow ; j++ )
	    {
	        if ( ! RowLeft[j] ) continue;

		Cell = RecursiveLitOrders[j][k];
		if ( Cell != '=' ) Count++;

		ThisOrder = Map[ThisOrder][Cell];
	    }

	    if ( ThisOrder != '#' && Count > BestCount )
	    {
	        BestCount = Count;
		BestCol   = k;
		BestOrder = ThisOrder;
	    }
	}

	if ( ! BestCol )
	{
	    /*  Recursive Call Not OK  */

	    return false;
	}

	/*  Process best column  */

	ForEach(j, 0, NRow)
	{
	    if ( RowLeft[j] && RecursiveLitOrders[j][BestCol] != '=' )
	    {
	        RowLeft[j] = false;
		NRowLeft--;
	    }
        }

	ColLeft[BestCol] = false;

	/*  Make sure the same type is treated consistently  */

	ForEach(k, 1, N)
	{
	    if ( k != BestCol &&
		 Target->TypeRef[k] == Target->TypeRef[BestCol] )
	    {
		ColOrder[k] = BestOrder;
	    }
	}
    }

    return  NRowLeft < 0;
}



	/*  Add argument order information for recursive literal.
	    Note: this must be performed before calling NewState so that
	    new variables are correctly given ordering #  */


void  AddOrders(Literal L)
/*    ---------  */
{
    Var		V, W;

    /*  Allocate ordering and mark as active  */

    if ( ! L->ArgOrders )
    {
	L->ArgOrders = Alloc(Target->Arity+1, Ordering);
	L->ArgOrders[0] = 0;
    }

    ForEach(V, 1, Target->Arity)
    {
	W = L->Args[V];
	L->ArgOrders[V] = ( W <= Current.MaxVar ? PartialOrder[V][W] : '#' );
    }
}



	/*  Keep track of argument orders for recursive literals.
	    (The first cell is reserved for testing)  */

void  NoteRecursiveLit(Literal L)
/*    ----------------  */
{
    static int	RecLitSize=0;
    int		i;

    NRecLitClause++;
    i = NRecLitDef + NRecLitClause;

    if ( i >= RecLitSize )
    {
	RecLitSize += 100;
	Realloc(RecursiveLitOrders, RecLitSize, Ordering *);
    }

    RecursiveLitOrders[i] = L->ArgOrders;
}
