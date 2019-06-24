/******************************************************************************/
/*									      */
/*	All output routines						      */
/*									      */
/******************************************************************************/


#include  "defns.i"
#include  "extern.i"


void  PrintTuple(Tuple C, int N, TypeInfo *TypeRef, Boolean ShowPosNeg)
/*    ----------  */
{
    int		i;

    printf("\t");

    ForEach(i, 1, N)
    {
        if ( TypeRef ? TypeRef[i]->Continuous :
		       Variable[i]->TypeRef->Continuous )
	{
	    /*  Continuous value  */

	    if ( FP(C[i]) == MISSING_FP )
	    {
		printf("?");
	    }
	    else
	    {
		printf("%g", FP(C[i]));
	    }
	}
	else
	{
	    printf("%s", ConstName[C[i]]);
	}

	if ( i < N ) putchar(',');
    }

    if ( ShowPosNeg )
    {
	printf(": %c", (Positive(C) ? '+' : '-') );
    }

    putchar('\n');
}


    
void  PrintTuples(Tuple *TT, int N)
/*    -----------  */
{
    while ( *TT )
    {
	PrintTuple(*TT, N, Nil, true);
	TT++;
    }
}



void  PrintSpecialLiteral(Relation R, Boolean RSign, Var *A)
/*    -------------------  */
{
    Const	ThConst;
    float	Thresh;

    if ( R == EQVAR )
    {
        printf("%s%s%s", Variable[A[1]]->Name, RSign ? "=":"<>",
                         Variable[A[2]]->Name);
    }
    else
    if ( R == EQCONST )
    {
	GetParam(&A[2], &ThConst);

	printf("%s%s%s", Variable[A[1]]->Name, RSign ? "=" : "<>",
			 ConstName[ThConst]);
    }
    else
    if ( R == GTVAR )
    {
        printf("%s%s%s", Variable[A[1]]->Name, RSign ? ">" : "<=",
                         Variable[A[2]]->Name);
    }
    else
    if ( R == GTCONST )
    {
	GetParam(&A[2], &Thresh);

	if ( Thresh == MISSING_FP )
	{
	    printf("%s%s", Variable[A[1]]->Name, RSign ? ">" : "<=");
	}
	else
	{
	    printf("%s%s%g", Variable[A[1]]->Name, RSign ? ">" : "<=", Thresh);
	}
    }
}



void  PrintComposedLiteral(Relation R, Boolean RSign, Var *A)
/*    --------------------  */
{
    int i, V;

    if ( Predefined(R) )
    {
	PrintSpecialLiteral(R, RSign, A);
    }
    else
    {
	if ( ! RSign )
	{
	    printf("not(");
	}

	printf("%s", R->Name);
	ForEach(i, 1, R->Arity)
	{
	    printf("%c", (i > 1 ? ',' : '('));
	    if ( (V = A[i]) <= MAXVARS )
	    {
		printf("%s", Variable[V]->Name);
	    }
	    else
	    {
		printf("_%d", V - MAXVARS);
	    }
	}
	putchar(')');

	if ( ! RSign )
	{
	    putchar(')');
	}
    }
}



void  PrintLiteral(Literal L)
/*    ------------  */
{
    PrintComposedLiteral(L->Rel, L->Sign, L->Args);
}



void  PrintClause(Relation R, Clause C)
/*    -----------  */
{
    int		Lit;

    PrintComposedLiteral(R, true, DefaultVars);

    for ( Lit = 0 ; C[Lit] ; Lit++ )
    {
	printf("%s ", ( Lit ? "," : " :-" ));

	PrintLiteral(C[Lit]);
    }
    printf(".\n");
}



	/*  Print clause, substituting for variables equivalent to constants  */

void  PrintSimplifiedClause(Relation R, Clause C)
/*    ---------------------  */
{
    int		i, Lit, NextUnbound;
    Literal	L;
    Var		V, Bound, *SaveArgs, *Unbound;
    char	**HoldVarNames;
    Const	TC;
    Boolean	NeedComma=false;

    SaveArgs = Alloc(MAXVARS+1, Var);
    Unbound = Alloc(MAXVARS+1, Var);

    /*  Save copy of variable names  */

    HoldVarNames = Alloc(MAXVARS+1, char *);
    ForEach(V, 1, MAXVARS)
    {
	HoldVarNames[V] = Variable[V]->Name;
    }

    /*  Substitute for equal variables  */

    for ( Lit = 0 ; L = C[Lit] ; Lit++ )
    {
        if ( L->Rel == EQVAR && L->Sign )
	{
	    Substitute(Variable[L->Args[2]]->Name, Variable[L->Args[1]]->Name);
	}
    }

    /*  Set up alternate names for variables equated to theory constants  */

    for ( Lit = 0 ; L = C[Lit] ; Lit++ )
    {
        if ( L->Rel == EQCONST && L->Sign )
	{
	    GetParam(&L->Args[2], &TC);
	    Substitute(Variable[L->Args[1]]->Name, ConstName[TC]);
	}
    }

    PrintComposedLiteral(R, true, DefaultVars);
    Bound = R->Arity;

    for ( Lit = 0 ; L = C[Lit] ; Lit++ )
    {
	/*  Save literal args  */

	memcpy(SaveArgs, L->Args, (L->Rel->Arity+1) * sizeof(Var));

	/*  Update bound vars and change unbound vars in negated literals  */

	NextUnbound = MAXVARS;
	memset(Unbound, 0, MAXVARS+1);

	ForEach(i, 1, L->Rel->Arity )
	{
	    if ( (V = L->Args[i]) > Bound )
	    {
		if ( ! L->Sign )
		{
		    if ( ! Unbound[V] ) Unbound[V] = ++NextUnbound;
		    L->Args[i] = Unbound[V];
		}
		else
		{
		    Bound = V;
		}
	    }
	}
	
	/*  Skip literals that are implicit in changed names  */

        if ( L->Rel != EQCONST && L->Rel != EQVAR || ! L->Sign )
	{
	    printf("%s", ( NeedComma ? ", " : " :- ") );

	    PrintLiteral(L);
	    NeedComma = true;
	}

	/*  Restore args  */

	memcpy(L->Args, SaveArgs, (L->Rel->Arity+1) * sizeof(Var));
    }
    printf(".\n");

    ForEach(V, 1, MAXVARS)
    {
	Variable[V]->Name = HoldVarNames[V];
    }

    pfree(SaveArgs);
    pfree(Unbound);
    pfree(HoldVarNames);
}



void  Substitute(char *Old, char *New)
/*    ----------  */
{
    Var V;

    ForEach(V, 1, MAXVARS)
    {
	if ( Variable[V]->Name == Old ) Variable[V]->Name = New;
    }
}



void  PrintDefinition(Relation R)
/*    ---------------  */
{
    int		Cl, SecondPass=(-1);
    Clause	C;

    putchar('\n');
    for ( Cl = 0 ; C=R->Def[Cl] ; Cl++ )
    {
	if ( Recursive(C) )
	{
	    SecondPass = Cl;
	}
	else
	{
	    PrintSimplifiedClause(R, C);
	}
    }

    ForEach(Cl, 0, SecondPass)
    {
	if ( Recursive(C = R->Def[Cl]) ) PrintSimplifiedClause(R, C);
    }

    printf("\nTime %.1f secs\n", CPUTime());
}



Boolean  Recursive(Clause C)
/*       ---------  */
{
    int Lit;

    for ( Lit = 0 ; C[Lit] ; Lit++ )
    {
	if ( C[Lit]->Rel == Target ) return true;
    }

    return false;
}
