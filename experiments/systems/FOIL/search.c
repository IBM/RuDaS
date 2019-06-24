/******************************************************************************/
/*									      */
/*	Routines to control search.  The search for clauses is basically      */
/*	greedy, but a limited number of alternative possibilities is	      */
/*	kept on hand							      */
/*									      */
/******************************************************************************/


#include "defns.i"
#include "extern.i"

Boolean	FirstSave;	/* flag for printing */


    /*	At any stage, the MAXPOSSLIT best literals to use next are
	kept in the array Possible.  This procedure puts a new literal
	into the list if it qualifies  */


void  ProposeLiteral(Relation R, Boolean TF, Var *A,
		     int Size, float LitBits, int OPos, int OTot,
		     float Gain, Boolean Weak)
/*    --------------  */
{
    PossibleLiteral Entry;
    int		    i, j, ArgSize;

    /*  See where this literal would go.  Other things being equal, prefer
	an unnegated literal - regarding "<=" as unnegated */

    i = NPossible;
    while ( i > 0 &&
	    ( Gain > Possible[i]->Gain + 1E-3 ||
	      Gain == Possible[i]->Gain && TF && ! Possible[i]->Sign ) )
    {
	i--;
    }

    if ( i >= MAXPOSSLIT )  return;

    if ( NPossible < MAXPOSSLIT ) NPossible++;

    Entry = Possible[MAXPOSSLIT];

    for ( j = MAXPOSSLIT ; j > i+1 ; j-- )
    {
	Possible[j] = Possible[j-1];
    }

    Possible[i+1] = Entry;

    ArgSize = (R->Arity+1)*sizeof(Var);
    if ( HasConst(R) ) ArgSize += sizeof(Const);

    Entry->Gain     = Gain;
    Entry->Rel      = R;
    Entry->Sign     = TF;
    memcpy(Entry->Args, A, ArgSize);
    Entry->Bits	    = LitBits;
    Entry->NewSize  = Size;
    Entry->PosCov   = OPos;
    Entry->TotCov   = OTot;
    Entry->WeakLits = ( Weak ? NWeakLits+1 : 0 );
}



    /*  When all possible literals have been explored, the best of them
	(if there are any) is extracted and used.  Others with gain
	close to the best are entered as backups  */

Literal  SelectLiteral()
/*       -------------  */
{
    int i;

    if ( ! NPossible ) return Nil;

    FirstSave = true;
    ForEach(i, 2, NPossible)
    {
	if ( Possible[i]->Gain >= MINALTFRAC * Possible[1]->Gain )
	{
	    Remember(MakeLiteral(i), Possible[i]->PosCov, Possible[i]->TotCov);
	}
    }

    return MakeLiteral(1);
}



Literal  MakeLiteral(int i)
/*       -----------  */
{
    int		ArgSize;
    Literal	L;

    L = AllocZero(1, struct _lit_rec);

    L->Rel  = Possible[i]->Rel;
    L->Sign = Possible[i]->Sign;
    L->Bits = Possible[i]->Bits;

    L->WeakLits = Possible[i]->WeakLits;

    ArgSize = (L->Rel->Arity+1)*sizeof(Var);
    if ( HasConst(L->Rel) ) ArgSize += sizeof(Const);
    L->Args = Alloc(ArgSize, Var);
    memcpy(L->Args, Possible[i]->Args, ArgSize);

    return L;
}



    /*  The array ToBeTried contains all backup points so far.
	This procedure sees whether another proposed backup will fit
	or will displace an existing backup  */

void  Remember(Literal L, int OPos, int OTot)
/*    --------  */
{
    int		i, j;
    Alternative	Entry;
    float	InfoGain;

    InfoGain = OPos *
	       (Info(StartClause.NPos, StartClause.NTot) - Info(OPos, OTot));

    /*  See where this entry would go  */

    for ( i = NToBeTried ; i && ToBeTried[i]->Value < InfoGain ; i-- )
	;

    if ( i >= MAXALTS )
    {
	FreeLiteral(L);
	return;
    }

    if ( NToBeTried < MAXALTS ) NToBeTried++;

    Entry = ToBeTried[MAXALTS];

    for ( j = MAXALTS ; j > i+1 ; j-- )
    {
	ToBeTried[j] = ToBeTried[j-1];
    }

    ToBeTried[i+1] = Entry;

    if ( Entry->UpToHere ) pfree(Entry->UpToHere);

    Entry->UpToHere = Alloc(NLit+2, Literal);
    memcpy(Entry->UpToHere, NewClause, (NLit+1)*sizeof(Literal));
    Entry->UpToHere[NLit]   = L;
    Entry->UpToHere[NLit+1] = Nil;
    Entry->Value	    = InfoGain;

    Verbose(1)
    {
	if ( FirstSave )
	{
	    putchar('\n');
	    FirstSave = false;
	}
	printf("\tSave ");
	PrintLiteral(L);
	printf(" (%d,%d value %.1f)\n", OPos, OTot, InfoGain);
    }
}



Boolean  Recover()
/*       -------  */
{
    int		i;
    Clause	C;
    Alternative	Entry;

    if ( SavedClause || ! NToBeTried || MAXRECOVERS-- < 1 ) return Nil;

    Entry = ToBeTried[1];
    C = ToBeTried[1]->UpToHere;

    Verbose(1)
    {
	printf("\nRecover to ");
	PrintClause(Target, C);
    }

    ForEach(i, 2, NToBeTried)
    {
	ToBeTried[i-1] = ToBeTried[i];
    }
    ToBeTried[NToBeTried] = Entry;
    NToBeTried--;

    RecoverState(C);
    CheckOriginalCaseCover();
    ExamineVariableRelationships();

    return true;
}



void  FreeLiteral(Literal L)
/*    -----------  */
{
    pfree(L->Args);
    if ( L->ArgOrders ) pfree(L->ArgOrders);
    pfree(L);
}



void  FreeClause(Clause C)
/*    ----------  */
{
    Clause CC;

    for ( CC = C ; *CC ; CC++ )
    {
	FreeLiteral(*CC);
    }
    pfree(C);
}
