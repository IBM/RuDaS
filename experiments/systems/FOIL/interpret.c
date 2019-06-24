/******************************************************************************/
/*									      */
/*	Routines for evaluating a definition on a case.  Used both during     */
/*	pruning and when testing definitions found			      */
/*									      */
/******************************************************************************/


#include  "defns.i"
#include  "extern.i"

#define  HighestVar	Current.MaxVar	/* must be set externally! */

Boolean	RecordArgOrders = false;	/* flag set by prune */
Const	*Value = Nil;			/* current variable bindings */


Boolean  CheckRHS(Clause C)
/*       --------  */
{
    Relation	R;
    Tuple	Case, *Bindings, *Scan;
    Literal	L;
    int		i, N;
    Var		*A, V, W;
    Const	KVal, *CopyValue;
    float	VVal, WVal;
    Ordering	ThisOrder, PrevOrder;
    Boolean	SomeOrder=false;

    if ( ! (L = C[0]) ) return true;

    R = L->Rel;
    A = L->Args;
    N = R->Arity;

    /*  If literal marked inactive, ignore  */

    if ( A[0] ) return CheckRHS(C+1);

    /*  Record types of unbound variables  */

    ForEach(i, 1, N)
    {
	V = A[i];
	if ( Value[V] == UNBOUND ) Variable[V]->TypeRef = R->TypeRef[i];
    }

    /* Check for missing values */

    if ( MissingValue(R, A, Value) ) return false;

    /*  Adjust ordering information for recursive literals if required  */

    if ( RecordArgOrders && R == Target )
    {
	ForEach(V, 1, N)
	{
	    W = A[V];

	    if ( Value[W] == UNBOUND )
	    {
		ThisOrder = '#';
	    }
	    else
	    {
		if ( Variable[V]->TypeRef->Continuous )
		{
		    VVal = FP(Value[V]);
		    WVal = FP(Value[W]);
		}
		else
		{
		    VVal = Variable[V]->TypeRef->CollSeq[Value[V]];
		    WVal = Variable[V]->TypeRef->CollSeq[Value[W]];
		}

		ThisOrder = ( VVal < WVal ? '<' :
			      VVal > WVal ? '>' : '=' );
	    }

	    PrevOrder = L->ArgOrders[V];
	    if ( PrevOrder != ThisOrder )
	    {
		L->ArgOrders[V] = ( ! PrevOrder ? ThisOrder : '#' );
	    }

	    ThisOrder = L->ArgOrders[V];
	    SomeOrder |= ( ThisOrder == '<' || ThisOrder == '>' );
	}

	if ( ! SomeOrder )
	{
	    return RecordArgOrders = false;
	}
    }

    /*  Various possible cases  */

    if ( Predefined(R) )
    {
	if ( HasConst(R) )
	{
	    GetParam(&A[2], &KVal);
	}
	else
	{
	    KVal = A[2];
	}

	return Satisfies((int)R->Pos, A[1], KVal, Value) == (L->Sign != 0) &&
	       CheckRHS(C+1);
    }

    if ( ! L->Sign )
    {
	return ( ! Join(R->Pos, R->PosIndex, A, Value, N, true) ) &&
	         CheckRHS(C+1);
    }

    if ( ! Join(R->Pos, R->PosIndex, A, Value, N, false) ) return false;

    /*  Each tuple found represents a possible binding for the free variables
	in A.  Copy them (to prevent overwriting on subsequent calls to Join)
	and try them in sequence  */

    Bindings = Alloc(NFound+1, Tuple);
    memcpy(Bindings, Found, (NFound+1)*sizeof(Tuple));

    CopyValue = Alloc(MAXVARS+1, Const);
    memcpy(CopyValue, Value, (HighestVar+1)*sizeof(Const));

    Scan = Bindings;

    /* Check rest of RHS */

    while ( Case = *Scan++ )
    {
	ForEach(i, 1, N)
	{
	    V = L->Args[i];
	    if ( Value[V] == UNBOUND ) Value[V] = Case[i];
	}

	if ( CheckRHS(C+1) )
	{
	    pfree(Bindings);
	    memcpy(Value, CopyValue, (HighestVar+1)*sizeof(Const));
	    pfree(CopyValue);
	    return true;
	}

	memcpy(Value, CopyValue, (HighestVar+1)*sizeof(Const));
    }

    pfree(Bindings);
    pfree(CopyValue);
    return false;
}



Boolean  Interpret(Relation R, Tuple Case)
/*       ---------  */
{
    int i;

    ForEach(i, 1, R->Arity)
    {
	Variable[i]->TypeRef = R->TypeRef[i];
    }
    InitialiseValues(Case, R->Arity);

    for ( i = 0 ; R->Def[i] ; i++ )
    {
	if ( CheckRHS(R->Def[i]) ) return true;
    }

    return false;
}



void  InitialiseValues(Tuple Case, int N)
/*    ----------------  */
{
    int i;

    if ( ! Value )
    {
	Value = Alloc(MAXVARS+1, Const);
    }

    ForEach(i, 1, N)
    {
	Value[i] = Case[i];
    }
    ForEach(i, N+1, MAXVARS)
    {
	Value[i] = UNBOUND;
    }
}
