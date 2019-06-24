/******************************************************************************/
/*									      */
/*	Routines for processing determinate literals			      */
/*									      */
/******************************************************************************/


#include  "defns.i"
#include  "extern.i"


Boolean  GoodDeterminateLiteral(Relation R, Var *A, float LitBits)
/*       ----------------------  */
{
    int		MaxSoFar, PreviousMax, This, i, j;
    Var		V;
    Literal	L;
    Boolean	SensibleBinding=false;

    /*  See whether this determinate literal's bound variables were all bound
	by determinate literals on the same relation  */

    ForEach(j, 1, R->Arity)
    {
	SensibleBinding |= ( A[j] <= Target->Arity );
    }

    MaxSoFar = Target->Arity;

    for ( i = 0 ; ! SensibleBinding && i < NLit ; i++ )
    {
	if ( ! NewClause[i]->Sign ) continue;

	PreviousMax = MaxSoFar;

	ForEach(j, 1, NewClause[i]->Rel->Arity)
	{
	    if ( (V = NewClause[i]->Args[j]) > MaxSoFar ) MaxSoFar = V;
	}

	if ( NewClause[i]->Rel != R || NewClause[i]->Sign != 2 )
	{
	    ForEach(j, 1, R->Arity)
	    {
		SensibleBinding |= ( A[j] <= MaxSoFar && A[j] > PreviousMax );
	    }
	}
    }

    if ( ! SensibleBinding )
    {
	Verbose(3)
	{
	    printf("\tall vars bound by determinate lits on same relation\n");
	}
	return false;
    }

    /*  Record this determinate literal  */

    This = NLit + NDeterminate;
    if ( This && This%100 == 0 )
    {
        Realloc(NewClause, This+100, Literal);
    }

    L = NewClause[This] = AllocZero(1, struct _lit_rec);

    L->Rel  = R;
    L->Sign = 2;
    L->Bits = LitBits;
    L->Args = Alloc(R->Arity+1, Var);
    memcpy(L->Args, A, (R->Arity+1)*sizeof(Var));

    NDeterminate++;

    return true;
}



void  ProcessDeterminateLiterals(Boolean AllWeak)
/*    --------------------------  */
{
    int		PrevMaxVar, i, j, l, m;
    Literal	L;
    Var		V;
    Boolean	Unique, Changed;

    PrevMaxVar = Current.MaxVar;

    Verbose(1) printf("\nDeterminate literals\n");

    ForEach(l, 1, NDeterminate)
    {
	L = NewClause[NLit++];

	Verbose(1)
	{
	    putchar('\t');
	    PrintLiteral(L);
	}

	Changed = PrevMaxVar != Current.MaxVar;

	/*  Rename free variables  */

	ForEach(i, 1, L->Rel->Arity)
	{
	    if ( L->Args[i] > PrevMaxVar )
	    {
		L->Args[i] += Current.MaxVar - PrevMaxVar;

		if ( L->Args[i] > MAXVARS )
		{
		    Verbose(1) printf("\t\tno more variables\n");
		    NLit--;
		    MonitorWeakLits(AllWeak);
		    return;
		}
	    }
	}

	if ( Changed )
	{
	    Verbose(1)
	    {
		printf(" ->");
		PrintLiteral(L);
	    }
	    Changed = false;
	}

	if ( L->Rel == Target ) AddOrders(L);

        FormNewState(L->Rel, true, L->Args, Current.NTot);

	/*  Verify that new variables introduced by this determinate literal
	    don't replicate new variables introduced by previous determinate
	    literals.  [Note: new variables checked against old variables
	    in EvaluateLiteral() ]  */

	for ( i = Current.MaxVar+1 ; i <= New.MaxVar ; )
	{
	    Unique = true;

	    for ( j = PrevMaxVar+1 ; Unique && j <= Current.MaxVar ; j++ )
	    {
		Unique = ! SameVar(i, j);
	    }

	    if ( Unique )
	    {
		i++;
	    }
	    else
	    {
		j--;

		Verbose(1)
		{
		    printf(" %s=%s", Variable[i]->Name, Variable[j]->Name);
		}

		ShiftVarsDown(i);

		ForEach(V, 1, L->Rel->Arity)
		{
		    if ( L->Args[V] == i ) L->Args[V] = j;
		    else
		    if ( L->Args[V] >  i ) L->Args[V]--;
		}

		Changed = true;
	    }
	}

	/*  If no variables remain, delete this literal  */

	if ( Current.MaxVar == New.MaxVar )
	{
	    Verbose(1) printf(" (no new vars)");
	
	    NLit--;
	    ForEach(m, 1, NDeterminate-l)
	    {
		NewClause[NLit+m-1] = NewClause[NLit+m];
	    }

	    FreeTuples(New.Tuples, true);
	}
	else
	{
	    /* This determinate Literal is being kept in the clause */

	    if ( Changed )
	    {
		Verbose(1)
		{
		    printf(" ->");
		    PrintLiteral(L);
		}
	    }

	    AcceptNewState(L->Rel, L->Args, Current.NTot);
	    NDetLits++;

	    if ( L->Rel == Target ) NoteRecursiveLit(L);
	}

	Verbose(1) putchar('\n');
    }

    MonitorWeakLits(AllWeak);
}



	/*  See whether variable a is always the same as variable b in
	    all positive tuples of the new state  */

Boolean  SameVar(Var A, Var B)
/*       -------  */
{
    Tuple	*TSP, Case;

    for ( TSP = New.Tuples ; Case = *TSP++; )
    {
	if ( Positive(Case) && Case[A] != Case[B] ) return false;
    }

    /*  If same, delete any negative tuples where different  */

    for ( TSP = New.Tuples ; Case = *TSP; )
    {
	if ( ! Positive(Case) && Case[A] != Case[B] )
	{
	    *TSP = New.Tuples[New.NTot-1];
	    New.NTot--;
	    New.Tuples[New.NTot] = Nil;
	}
	else
	{
	    TSP++;
	}
    }

    return true;
}



void  ShiftVarsDown(int s)
/*    -------------  */
{
    Tuple	*TSP, Case;
    Var		V;

    New.MaxVar--;

    for ( TSP = New.Tuples ; Case = *TSP++ ; )
    {
	ForEach(V, s, New.MaxVar)
	{
	    Case[V] = Case[V+1];
	}
    }

    ForEach(V, s, New.MaxVar)
    {
	Variable[V]->Type = Variable[V+1]->Type;
        Variable[V]->TypeRef = Variable[V+1]->TypeRef;
    }
}
