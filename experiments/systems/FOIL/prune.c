/******************************************************************************/
/*									      */
/*	Stuff for pruning clauses and definitions.  Clauses are first	      */
/*	`quickpruned' to remove determinate literals that introduce unused    */
/*	variables; if this causes the definition to become less accurate,     */
/*	these are restored.  Then literals are removed one at a time to	      */
/*	see whether they contribute anything.  A similar process is	      */
/*	followed when pruning definitions: clauses are removed one at a	      */
/*	time to see whether the remaining clauses suffice		      */
/*									      */
/******************************************************************************/


#include  "defns.i"
#include  "extern.i"

extern Boolean RecordArgOrders;	/* for interpret.c */

#define  MarkLiteral(L,X)	{ (L)->Args[0]=X;\
				  if ((L)->ArgOrders) (L)->ArgOrders[0]=X;}


	/*  See whether literals can be dropped from a clause  */

void  PruneNewClause()
/*    --------------  */
{
    int		Errs, i, j, k;
    Boolean	*Used, Changed=false;
    Literal	L, LL;
    Var		V;

    Errs = Current.NOrigTot - Current.NOrigPos;

    Used = Alloc(MAXVARS+1, Boolean);

    Verbose(2)
    {
	printf("\nInitial clause (%d errs): ", Errs);
	PrintClause(Target, NewClause);
    }

    /*  Save arg orderings for recursive literals and promote any literals
	of form A=B or B=c to immediately after B appears in the clause  */

    ForEach(i, 0, NLit-1)
    {
	L = NewClause[i];

	if ( L->Sign && ( L->Rel == EQVAR || L->Rel == EQCONST ) )
	{
	    V = ( L->Rel == EQVAR ? L->Args[2] : L->Args[1] );

	    if ( V > Target->Arity )
	    {
		ForEach(j, 1, i)
		{
		    LL = NewClause[j-1];
		    if ( LL->Sign && Contains(LL->Args, LL->Rel->Arity, V) )
		    {
			break;
		    }
		}
	    }
	    else
	    {
		j = 0;
	    }

	    for ( k = i ; k > j ; k-- )
	    {
		NewClause[k] = NewClause[k-1];
	    }

	    NewClause[j] = L;
	}
	else
	if ( L->Rel == Target )
	{
	    L->SaveArgOrders = Alloc(Target->Arity+1, Ordering);
	    memcpy(L->SaveArgOrders, L->ArgOrders,
		   (Target->Arity+1)*sizeof(Ordering));
	}
    }

    /*  Look for unexpected relations V=W or V=c  */

    CheckVariables();

    /*  Check for quick pruning of apparently useless literals  */

    if ( QuickPrune(NewClause, Target->Arity, Used) )
    {
	if ( SatisfactoryNewClause(Errs) )
	{
	    Verbose(3) printf("\tAccepting quickpruned clause\n");
	    Changed = true;
	}
	else
	{
	    /*  Mark all literals as active again  */

	    ForEach(i, 0, NLit-1)
	    {
		MarkLiteral(NewClause[i], 0);
	    }
	}
    }

    Changed |= RedundantLiterals(Errs);

    /*  Remove unnecessary literals from NewClause  */

    if ( Changed )
    {
	for ( i = 0 ; i < NLit ; )
	{
	    L = NewClause[i];
	    if ( L->Args[0] )
	    {
		FreeLiteral(L);
		NLit--;
		ForEach(j, i, NLit-1)
		{
		    NewClause[j] = NewClause[j+1];
		}
	    }
	    else
	    {
	       i++;
	    }
	}
	NewClause[NLit] = Nil;

	RenameVariables();

	/*  Need to recompute partial orders and clause coverage  */

	RecoverState(NewClause);
	CheckOriginalCaseCover();
    }
    else
    {
	/*  Restore arg orders  */

	ForEach(i, 0, NLit-1)
	{
	    L = NewClause[i];
	    if ( L->Rel == Target )
	    {
		memcpy(L->ArgOrders,L->SaveArgOrders,
                       (Target->Arity+1)*sizeof(Ordering));
                pfree(L->SaveArgOrders);
	    }
	}
    }

    pfree(Used);
}



	/*  Look for unexpected equivalences of variables or variables
	    with unchanging values.  If found, insert pseudo-literals
	    into the clause so that redundant literals can be pruned  */

void  CheckVariables()
/*    --------------  */
{
    Var A, B;

    ForEach(A, 1, Current.MaxVar)
    {
	if ( TheoryConstant(Current.Tuples[0][A], Variable[A]->TypeRef) &&
	     ConstantVar(A, Current.Tuples[0][A]) )
	{
	    if ( ! Known(EQCONST, A, 0) )
	    {
		Insert(A, EQCONST, A, Current.Tuples[0][A]);
	    }
	}
	else
	{
	    ForEach(B, A+1, Current.MaxVar)
	    {
		if ( Compatible[Variable[A]->Type][Variable[B]->Type] &&
		     IdenticalVars(A,B) && ! Known(EQVAR, A, B) )
		{
		    Insert(B, EQVAR, A, B);
		}
	    }
	}
    }
}



	/*  See whether a constant is a theory constant  */

Boolean  TheoryConstant(Const C, TypeInfo T)
/*       --------------  */
{
    int i;

    if ( T->Continuous ) return false;

    ForEach(i, 0, T->NTheoryConsts-1)
    {
	if ( C == T->TheoryConst[i] ) return true;
    }

    return false;
}



	/*  Check for variable with constant value  */

Boolean  ConstantVar(Var V, Const C)
/*       -----------  */
{
    Tuple	Case, *TSP;

    for ( TSP = Current.Tuples ; Case = *TSP++; )
    {
	if ( Positive(Case) && Case[V] != C ) return false;
    }

    return true;
}



	/*  Check for identical variables  */

Boolean  IdenticalVars(Var V, Var W)
/*       -------------  */
{
    Tuple	Case, *TSP;

    for ( TSP = Current.Tuples ; Case = *TSP++; )
    {
	if ( Positive(Case) &&
	     ( Unknown(V, Case) || Case[V] != Case[W] ) ) return false;
    }

    return true;
}



	/*  Make sure potential literal isn't already in clause  */

Boolean  Known(Relation R, Var V, Var W)
/*       -----  */
{
    int 	i;
    Literal	L;

    ForEach(i, 0, NLit-1)
    {
	L = NewClause[i];
	if ( L->Rel == R && L->Sign &&
	     L->Args[1] == V && ( ! W || L->Args[2] == W ) )
	{
	    return true;
	}
    }

    return false;
}



	/*  Insert new literal after V first bound  */

void  Insert(Var V, Relation R, Var A1, Const A2)
/*    ------  */
{
    int 	i=0, j;
    Literal	L;

    if ( V > Target->Arity )
    {
	ForEach(i, 1, NLit)
	{
	    L = NewClause[i-1];
	    if ( L->Sign && Contains(L->Args, L->Rel->Arity, V) ) break;
	}
    }

    /*  Insert literal before NewClause[i]  */

    if ( ++NLit % 100 == 0 ) Realloc(NewClause, NLit+100, Literal);
    for ( j = NLit ; j > i ; j-- )
    {
	NewClause[j] = NewClause[j-1];
    }

    L = AllocZero(1, struct _lit_rec);

    L->Rel  = R;
    L->Sign = true;
    L->Bits = 0;

    if ( R == EQVAR )
    {
	L->Args = AllocZero(3, Var);
	L->Args[1] = A1;
	L->Args[2] = A2;
    }
    else
    {
	L->Args = AllocZero(2+sizeof(Const), Var);
	L->Args[1] = A1;
	SaveParam(L->Args+2, &A2);
    }

    NewClause[i] = L;
    Verbose(2)
    {
	printf("\tInsert literal ");
	PrintSpecialLiteral(R, true, L->Args);
	printf("\n");
    }
}



	/*  Check for variable in argument list  */

Boolean  Contains(Var *A, int N, Var V)
/*       --------  */
{
    int i;

    ForEach(i, 1, N)
    {
	if ( A[i] == V ) return true;
    }

    return false;
}


		
	/*  Remove determinate literals that introduce useless new variables
	    (that are not used by any subsequent literal)  */

Boolean  QuickPrune(Clause C, Var MaxBound, Boolean *Used)
/*       ----------  */
{
    Var		V, NewMaxBound;
    Literal	L;
    Boolean	SomeUsed=true, Changed=false;

    if ( (L = C[0]) == Nil )
    {
	ForEach(V, 1, MaxBound) Used[V] = false;
	return false;
    }

    L->Args[0] = 0;

    NewMaxBound = MaxBound;
    if ( L->Sign )
    {
	ForEach(V, 1, L->Rel->Arity)
	{
	    if ( L->Args[V] > NewMaxBound ) NewMaxBound = L->Args[V];
	}
    }

    Changed = QuickPrune(C+1, NewMaxBound, Used);

    if ( L->Sign == 2 )
    {
	SomeUsed = false;
	ForEach(V, MaxBound+1, NewMaxBound)
	{
	    SomeUsed = Used[V];
	    if ( SomeUsed ) break;
	}
    }

    if ( ! SomeUsed && C[1] )
    {
	/*  Mark this literal as inactive  */

	MarkLiteral(L, 1);

	Verbose(3)
	{
	    printf("\tQuickPrune literal ");
	    PrintLiteral(L);
	    putchar('\n');
	}

	Changed = true;
    }
    else
    ForEach(V, 1, L->Rel->Arity)
    {
	Used[L->Args[V]] = true;
    }

    return Changed;
}



Boolean  SatisfactoryNewClause(int Errs)
/*       ---------------------  */
{
    Boolean	RecursiveLits=false, ProForma;
    Literal	L;
    int		i, ErrsNow=0;

    /*  Prepare for redetermining argument orders  */

    ForEach(i, 0, NLit-1)
    {
	if ( (L = NewClause[i])->Args[0] ) continue;

	if ( L->Rel == Target )
	{
	    RecursiveLits = true;
	    memset(L->ArgOrders, 0, Target->Arity+1);
	}
    }

    /*  Scan case by case, looking for too many covered neg tuples  */

    RecordArgOrders = RecursiveLits;

    ForEach(i, StartDef.NPos, StartDef.NTot-1)
    {
	InitialiseValues(StartDef.Tuples[i], Target->Arity);

	if ( CheckRHS(NewClause) && ++ErrsNow > Errs ||
	     RecursiveLits && ! RecordArgOrders )
	{
	    RecordArgOrders = false;
	    return false;
	}
    }

    /*  If satisfactory and recursive literals, must also check pos
	tuples covered to complete arg order information  */

    for ( i = 0 ; i < StartDef.NPos && RecordArgOrders ; i++ )
    {
	InitialiseValues(StartDef.Tuples[i], Target->Arity);

	ProForma = CheckRHS(NewClause);
    }

    /*  Now check that recursion still well-behaved  */

    return ( RecursiveLits ? RecordArgOrders && RecursiveCallOK(Nil) : true );
}


    /*  Rename variables in NewClause  */

void  RenameVariables()
/*    ---------------  */
{
    Var		*NewVar, Next, SaveNext, V;
    int		l, i;
    Literal	L;

    NewVar = AllocZero(MAXVARS+1, Var);
    Next = Target->Arity+1;

    ForEach(l, 0, NLit-1)
    {
	if ( (L=NewClause[l])->Args[0] ) continue;

	SaveNext = Next;
	ForEach(i, 1, L->Rel->Arity)
	{
	    V = L->Args[i];

	    if ( V > Target->Arity )
	    {
		if ( ! NewVar[V] ) NewVar[V] = Next++;

		L->Args[i] = NewVar[V];
	    }
	}

	/*  New variables appearing in negated lits are still free  */

	if ( ! L->Sign )
	{
	    Next = SaveNext;
	    ForEach(V, 1, MAXVARS)
	    if ( NewVar[V] >= Next ) NewVar[V] = 0;
	}
    }

    pfree(NewVar);
}



	/*  Omit the first unnecessary literal.
	    This version prunes from the end of the clause; if a literal
	    can't be dropped when it is examined, it will always be
	    needed, since dropping earlier literals can only increase
	    the number of minus tuples getting through to this literal  */


Boolean  RedundantLiterals(int ErrsNow)
/*       -----------------  */
{
    int		i, j;
    Boolean	Changed=false;
    Literal	L;

    /*  Check for the latest literal, omitting which would not increase
	the number of errors.  Note: checking last literal is reinstated
	since clause may contain errors  */

    for ( i = NLit-1 ; i >= 0 ; i-- )
    {
	L = NewClause[i];

        if ( L->Args[0] || EssentialBinding(i) )
	{
	    continue;
	}

	MarkLiteral(L, 1);

	if ( SatisfactoryNewClause(ErrsNow) )
	{
	    Verbose(3)
	    {
		printf("\t\t");
		PrintLiteral(L);
		printf(" removed\n");
	    }
	    Changed = true;
	}
	else
	{
	    Verbose(3)
	    {
		printf("\t\t");
		PrintLiteral(L);
		printf(" essential\n");
	    }
	    MarkLiteral(L, 0);

	    /*  If this literal is V=W, where W is a non-continuous variable
		bound on the RHS of the clause, substitute for W throughout
		and remove the literal  */

	    if ( L->Rel == EQVAR && L->Sign && L->Args[2] > Target->Arity &&
		 ! Variable[L->Args[2]]->TypeRef->Continuous )
	    {
		ForEach(j, i, NLit-1)
		{
		    NewClause[j] = NewClause[j+1];
		}
		NLit--;
		ReplaceVariable(L->Args[2], L->Args[1]);
	    }
	}
    }

    return Changed;
}



	/*  Can't omit a literal that is needed to bind a variable appearing in
	    a later negated literal relation, or whose omission would leave a
	    later literal containing all new variables  */

#define  NONE	-1
#define  MANY	1000000

Boolean  EssentialBinding(int LitNo)
/*       ----------------  */
{
    int		i, j, b, *UniqueBinding;
    Boolean	Needed=false, Other;
    Literal	L;
    Var		V, NArgs;

    /*  UniqueBinding[V] = NONE (if V not yet bound)
			 = i    (if V bound only by ith literal)
			 = MANY (if V bound by more than one literal)  */

    UniqueBinding = Alloc(MAXVARS+1, int);
    ForEach(V, 1, MAXVARS)
    {
	UniqueBinding[V] = ( V <= Target->Arity ? MANY : NONE );
    }

    for ( i = 0 ; i < NLit && ! Needed ; i++ )
    {
	if ( (L = NewClause[i])->Args[0] ) continue;

	NArgs = L->Rel->Arity;

	if ( i > LitNo )
	{
	    if ( Predefined(L->Rel) || ! L->Sign )
	    {
		ForEach(j, 1, NArgs)
		{
		    Needed |= UniqueBinding[L->Args[j]] == LitNo;
		}
	    }
	    else
	    {
		Other = false;
		ForEach(j, 1, NArgs)
		{
		    b = UniqueBinding[L->Args[j]];
		    Other |= b != NONE && b != LitNo;
		}
		Needed = ! Other;
	    }

	    if ( Needed )
	    {
		Verbose(/*3*/2)
		{
		    printf("\t\t");
		    PrintLiteral(NewClause[LitNo]);
		    printf(" needed for binding ");
		    PrintLiteral(NewClause[i]);
		    putchar('\n');
		}
	    }
	}

	if ( L->Sign )
	{
	    /*  Update binding records for new variables  */

	    ForEach(j, 1, NArgs)
	    {
		V = L->Args[j];

		if ( UniqueBinding[V] == NONE )
		{
		    UniqueBinding[V] = i;
		}
		else
		if ( UniqueBinding[V] != i )
		{
		    UniqueBinding[V] = MANY;
		}
	    }
	}
    }

    pfree(UniqueBinding);
    return Needed;
}




	/*  Substitute for variable in clause  */

void  ReplaceVariable(Var Old, Var New)
/*    ---------------  */
{
    int 	i, a;
    Literal	L;
    Boolean	Bound=false;

    ForEach(i, 0, NLit-1)
    {
	L = NewClause[i];

	if ( L->Sign || Bound )
	{
	    ForEach(a, 1, L->Rel->Arity)
	    {
		if ( L->Args[a] == Old )
		{
		    L->Args[a] = New;
		    Bound = true;
		}
	    }
	}
    }
}



	/*  See whether some clauses can be dispensed with  */

void  SiftClauses()
/*    -----------  */
{
    int		i, j, Covers, Last, Retain=0, Remove=0;
    Boolean	*Need, *Delete;

    if ( ! NCl ) return;

    Delete = AllocZero(NCl, Boolean);
    Need   = AllocZero(NCl, Boolean);

    Current.MaxVar = HighestVarInDefinition(Target);

    while ( Retain+Remove < NCl )
    {
	/*  See if a clause uniquely covers a pos tuple  */

	for ( i = 0 ; i < StartDef.NPos && Retain < NCl ; i++ )
	{
	    Covers = 0;
	    for ( j = 0 ; j < NCl && Covers <= 1 && Retain < NCl ; j++ )
	    {
		if ( Delete[j] ) continue;

		InitialiseValues(StartDef.Tuples[i], Target->Arity);

		if ( CheckRHS(Target->Def[j]) )
		{
		    Covers++;
		    Last = j;
		}
	    }

	    if ( Covers == 1 && ! Need[Last] )
	    {
		Verbose(3)
		{
		    printf("\tClause %d needed for ", Last);
		    PrintTuple(StartDef.Tuples[i], Target->Arity,
			       Target->TypeRef, false);
		}

		Need[Last] = true;
		Retain++;
	    }
	}

	if ( Retain == NCl ) break;

	/*  Remove the latest unneeded clause  */

	Last = -1;
	ForEach(i, 0, NCl-1)
	{
	    if ( ! Need[i] && ! Delete[i] ) Last = i;
	}

	if ( Last == -1 ) break;

	Delete[Last] = true;
	Remove++;
    }


    if ( Retain < NCl )
    {
	Verbose(1) printf("\nDelete clause%s\n", Plural(NCl - Retain));

	Retain = 0;

	ForEach(i, 0, NCl-1)
	{
	    if ( Delete[i] )
	    {
		Verbose(1)
		{
		    printf("\t");
		    PrintClause(Target, Target->Def[i]);
		    FreeClause(Target->Def[i]);
		}
	    }
	    else
	    {
		Target->Def[Retain++] = Target->Def[i];
	    }
	}
	Target->Def[Retain] = Nil;
    }

    pfree(Delete);
    pfree(Need);
}



    /*  Find highest variable in any clause  */

Var  HighestVarInDefinition(Relation R)
/*   ----------------------  */
{
    Var		V, HiV;
    Literal	L;
    Clause	C;
    int		i;

    HiV = R->Arity;

    for ( i = 0 ; R->Def[i] ; i++ )
    {
	for ( C = R->Def[i] ; L = *C ; C++ )
	{
	    if ( L->Sign )
	    {
		ForEach(V, 1, L->Rel->Arity)
		{
		    if ( L->Args[V] > HiV ) HiV = L->Args[V];
		}
	    }
	}
    }

    return HiV;
}
