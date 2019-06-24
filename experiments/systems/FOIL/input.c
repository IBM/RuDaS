/******************************************************************************/
/*									      */
/*	All input routines						      */
/*									      */
/******************************************************************************/


#include  "defns.i"
#include  "extern.i"

#define  Space(s)	(s == ' ' || s == '\t')
#define  SkipComment	while ( ( c = getchar() ) != '\n' )


char    Name[200],
	SectionDelim;
Const   *SortedConst = Nil;
Boolean	ContinuousVars = false;


/******************************************************************************/
/*									      */
/*	Routines for reading types					      */
/*									      */
/******************************************************************************/



Boolean  ReadType()
/*       --------  */
{
    int		i;
    char	Delim;
    TypeInfo	T;
    Boolean	FirstTime=true, RecordTheoryConst;
    Const	C;

    Delim = ReadName(Name);

    if ( ! *Name ) return false;
    else
    if ( Delim != ':' ) Error(1, Name);

    T = AllocZero(1, struct _type_rec);

    T->NValues = T->NTheoryConsts = 0;
    T->Value   = T->TheoryConst   = Nil;

    if ( Name[0] == '*' )	/* order specified by user */
    {
        T->FixedPolarity = true;
	T->Ordered = true;
        T->Name = CopyString(Name+1);
    }
    else
    if ( Name[0] == '#' )	/* specified to be unordered */
    {
        T->FixedPolarity = true;
        T->Ordered = false;
        T->Name = CopyString(Name+1);
    }
    else
    {
        T->FixedPolarity = false;
	T->Name = CopyString(Name);
    }

    /*  Read first name  */

    while ( (Delim = ReadName(Name)) == '\n' )
	;

    if ( ! Name ) Error(1, Name);

    /*  Check for continuous type  */

    if ( ! strcmp(Name, "continuous") )
    {
        T->Continuous = true;
	T->FixedPolarity = true;
	T->Ordered = false; /* Never match on continuous value - hence
			       no point in checking for orders on it */
	ContinuousVars = true;
    }
    else
    {
	/*  Discrete type, read the values  */

        T->Continuous = false;

	do
	{
	    while ( ( FirstTime ? ! (FirstTime = false) :
				    (Delim = ReadName(Name)) ) &&
		    Delim == '\n' )
			;

	    if ( Name[0] == '*' )
	    {
		/*  Theory constant  */

		RecordTheoryConst = true;

		for ( i = 0 ; Name[i++] ; )
		{
		    Name[i-1] = Name[i];
		}
	    }
	    else
	    {
		RecordTheoryConst = false;
	    }
		
	    if ( T->NValues % 100 == 0 )
	    {
		Realloc(T->Value, T->NValues+100, Const);
	    }

	    C = T->Value[ T->NValues++ ] = FindConstant(Name, false);

	    /*  Check for duplicate constants  */

	    ForEach(i, 0, T->NValues-2)
	    {
		if ( T->Value[i] == C ) Error(7, Name, T->Name);
	    }

	    if ( RecordTheoryConst )
	    {
		if ( T->NTheoryConsts % 10 == 0 )
		{
		    Realloc(T->TheoryConst, T->NTheoryConsts+10, Const);
		}

		T->TheoryConst[ T->NTheoryConsts++ ] = C;
	    }
	}
	while ( Delim == ',' );
    }

    if ( Delim != '.' ) Error(2, Name, T->Name);
    ReadToEOLN;

    /* Enter Type */

    MaxType++;

    if ( MaxType % 100 == 1 )
    {
        Realloc(Type, MaxType + 100, TypeInfo);
    }

    Type[MaxType] = T;

    return true;
}



void  ReadTypes()
/*    ---------  */
{
    int		i, j;
    TypeInfo	T;

    /*  Record names for missing value and out-of-range  */

    FindConstant("?", false);
    FindConstant("^", false);

    /*  Read all type definitions  */

    while ( ReadType() )
	;

    /*  Generate collating sequences  */

    ForEach(i, 1, MaxType)
    {
	T = Type[i];

	if ( T->Continuous ) continue; /* Skip continuous type */

	T->CollSeq = Alloc(MaxConst+1, int);

	ForEach(j, 0, MaxConst)
	{
	    T->CollSeq[j] = 0;
	}

	ForEach(j, 0, T->NValues-1)
	{
	    T->CollSeq[T->Value[j]] = j+1;
	}

	T->CollSeq[MISSING_DISC] = T->NValues+1;
    }
}



/******************************************************************************/
/*									      */
/*	Routines for reading tuples and relations			      */
/*									      */
/******************************************************************************/



Tuple  ReadTuple(Relation R)
/*     ---------  */
{
    char	Delim;
    int		N, i;
    Tuple	T;

    N = R->Arity;

    if ( (Delim = ReadName(Name)) == '.' || Delim == ';' )
    {
	return Nil;
    }

    T = Alloc(N+1, Const);

    ForEach(i, 1, N)
    {
	if ( i > 1 )
	{
	    Delim = ReadName(Name);
	}

	if ( R->TypeRef[i]->Continuous )
	{
	    if ( ! strcmp(Name,"?") )
	    {
	        FP(T[i]) = MISSING_FP;
		MissingVals = true;
	    }
	    else
	    {
	        FP(T[i]) = atof(Name);

		if ( FP(T[i]) == MISSING_FP )
		{
		    printf("An input continuous values is equal to the\n");
		    printf("magic number used to designate missing values.\n");
		    printf("Change the definition of MISSING_FP in defns.i\n");
		    printf("and recompile.\n");
		    exit(1);
	        }
	    }
	}
	else
	{
	    if ( ! strcmp(Name,"?") )
	    {
	        T[i] = MISSING_DISC;
		MissingVals = true;
	    }
	    else
	    {
		T[i] = FindConstant(Name, true);
	    }
	}
    }

    if ( Delim != ':' && Delim != '\n' ) ReadToEOLN;

    ForEach(i, 1, N)
    {
	if ( ! R->TypeRef[i]->Continuous && T[i] != OUT_OF_RANGE &&
	     ! R->TypeRef[i]->CollSeq[T[i]] ) 
	{
            Error(4, ConstName[T[i]], Type[R->Type[i]]->Name);
	}
    }

    return T;
}



Tuple  *ReadTuples(Relation R, Boolean Pos)
/*      ----------  */
{
    Tuple	T, *TheTuples=Nil;
    int		ND=0;

    TheTuples = Alloc(101, Tuple);

    while ( T = ReadTuple(R) )
    {
	T[0] = Pos ? PosMark : 0;

        if ( ND && ND % 100 == 0 ) 
	{
	    Realloc(TheTuples, ND+101, Tuple);
	}
	TheTuples[ND++] = T;
    }

    TheTuples[ND] = Nil;

    return TheTuples;
}



Relation  ReadRelation()
/*        ------------  */
{
    Relation	R;
    char	Delim, c;
    int		NArgs=0, Key[100], NKeys=0, i, t;

    if ( ReadName(Name) != '(' ) return Nil;

    printf("\nRelation %s\n", Name);

    /*  Create a new record with all zero counts  */

    R = AllocZero(1, struct _rel_rec);

    if ( Name[0] == '*' )
    {
	/*  Background relation  */

        R->PossibleTarget = false;
        R->Name = CopyString(Name+1);
    }
    else
    {
        R->PossibleTarget = true;
	R->Name = CopyString(Name);
    }

    do
    {
	NArgs++;

        Realloc(R->Type, NArgs+1, int);
        Realloc(R->TypeRef, NArgs+1, TypeInfo); 
	Delim = ReadName(Name);
        t = FindType(Name);
	R->Type[NArgs] = t; 
        R->TypeRef[NArgs] = Type[t]; 
    }
    while ( Delim != ')' );

    R->Arity = NArgs;
    if ( NArgs > MAXARGS ) MAXARGS = NArgs;
    if ( R->PossibleTarget && NArgs > MAXVARS ) MAXVARS = NArgs;

    /*  Read and store access keys  */
    do
    {
	do
	{
	    c = getchar();
	}
	while ( Space(c) ) ;

	if ( c != '\n' )
	{
	    Key[NKeys] = 0;

	    ForEach(i, 1, NArgs)
	    {
		if ( c == '-' ) Key[NKeys] |= (1 << i);
		c = getchar();
	    }
	    NKeys++;
	}
    }
    while ( c == '/');

    R->NKeys = NKeys;
    if ( NKeys )
    {
	R->Key   = Alloc(NKeys, int);
	memcpy(R->Key, Key, NKeys*sizeof(int));
    }

    R->Pos      = ReadTuples(R, true);
    R->PosIndex = MakeIndex(R->Pos, NArgs, R);

    if ( SectionDelim == '.' )
    {
	R->Neg = Nil;
    }
    else
    {
	R->Neg = ReadTuples(R, false);

	/*  The index of negative tuples isn't currently used, but may be
	    useful if you are adapting the system  */

	/*  R->NegIndex = MakeIndex(R->Neg, NArgs, R);  */
    }

    R->BinSym = SymmetryCheck(R);

    DuplicateTuplesCheck(R);

    UnequalArgsCheck(R);
    return R;
}



void  ReadRelations()
/*    -------------   */
{
    int		i, j, Next, Best, PosSize, WorldSize;
    Relation	R;
    Tuple	*T;
    Boolean	*Waiting;
    float	*Imbalance;

    while ( R = ReadRelation() )
    {
	/*  Make sure room for one more  */

        if ( ++MaxRel % 10 == 0 ) 
        {
            Realloc(Reln, MaxRel+10, Relation);
        }

	Reln[MaxRel] = R;

	Verbose(4)
	{
	    if ( Reln[MaxRel]->BinSym )
	    {
		printf("    is binary symmetric\n");
	    }

	    for ( T = Reln[MaxRel]->Pos ; *T ; T++ )
	    {
		PrintTuple(*T, Reln[MaxRel]->Arity,
			   Reln[MaxRel]->TypeRef, Reln[MaxRel]->Neg != Nil);
	    }
		
	    if ( Reln[MaxRel]->Neg )
	    {
		for ( T = Reln[MaxRel]->Neg ; *T ; T++ )
		{
		    PrintTuple(*T, Reln[MaxRel]->Arity,
			       Reln[MaxRel]->TypeRef, true);
		}
	    }
	}
    }

    /*  Now put the relations into the order in which they should be tried.
	The idea is to put lower arity relations earlier to maximise the
	effect of pruning.  Relations of the same arity are resolved by
	preferring relations with higher information  */

    RelnOrder = Alloc(MaxRel+1, Relation);
    Waiting = Alloc(MaxRel+1, Boolean);
    Imbalance = Alloc(MaxRel+1, float);

    memset(Waiting, true, MaxRel+1);
    ForEach(i, 4, MaxRel)
    {
	R = Reln[i];
	PosSize = Number(R->Pos);

	if ( R->Neg )
	{
	    WorldSize = PosSize + Number(R->Neg);
	}
	else
	{
	    WorldSize = 1;
	    ForEach(j, 1, Reln[i]->Arity)
	    {
		if ( ! R->TypeRef[j]->Continuous )
		{
		    WorldSize *= R->TypeRef[j]->NValues;
		}
	    }
	}

	Imbalance[i] = fabs(0.5 - PosSize / (float) WorldSize);
    }

    RelnOrder[0] = Reln[1];
    RelnOrder[1] = Reln[0];
    RelnOrder[2] = Reln[3];
    RelnOrder[3] = Reln[2];
    /*ForEach(i, 0, 3) RelnOrder[i] = Reln[i];*/
    Next = ( ContinuousVars ? 4 : 2 );
    
    while ( true )
    {
	Best = -1;

	ForEach(i, 4, MaxRel)
	{
	    if ( Waiting[i] &&
		 ( Best < 0 ||
		   Reln[i]->Arity < Reln[Best]->Arity ||
		   Reln[i]->Arity == Reln[Best]->Arity
		   && Imbalance[i] < Imbalance[Best] ) )
	    {
		Best = i;
	    }
	}

	if ( Best < 0 ) break;
	RelnOrder[Next++] = Reln[Best];
	Waiting[Best] = false;
    }
    MaxRel = Next-1;

    pfree(Waiting);
    pfree(Imbalance);
}


	/*  Find a type by name  */

int  FindType(char *N)
/*   --------  */
{
    int i;

    ForEach(i, 1, MaxType)
    {
	if ( ! strcmp(N, Type[i]->Name) ) return i;
    }

    Error(5, N);
    return 0; /* keep lint happy */
}



/******************************************************************************/
/*                                                                            */
/*	DuplicateTuplesCheck(R) - check for duplicate tuples in R             */
/*                                                                            */
/******************************************************************************/


void  DuplicateTuplesCheck(Relation R)
/*    --------------------  */
{
    int		i, j, k, N, NPos, NNeg;
    Tuple	*PosCopy, *NegCopy, PosTuple, NegTuple;
    Boolean	MutualDuplicate;

    /* First copy the positive tuples and check number of duplicates */

    NPos = Number(R->Pos);

    PosCopy = Alloc(NPos+1, Tuple);
    ForEach(i, 0, NPos)
    {
	PosCopy[i] = (R->Pos)[i];
    }

    N = R->Arity;

    if ( R->PosDuplicates = CountDuplicates(PosCopy,N,0,NPos-1) )
    {
	printf("    (warning: contains duplicate positive tuples)\n");
    }

    /* If there are neg tuples, check for duplicates and mutual duplicates */

    if ( R->Neg )
    {
        NNeg = Number(R->Neg);
        NegCopy = Alloc(NNeg+1, Tuple);
	ForEach(i, 0, NNeg)
	{
	    NegCopy[i] = (R->Neg)[i];
	}

	if ( CountDuplicates(NegCopy,N,0,NNeg-1) )
        {
	    printf("    (warning: contains duplicate negative tuples)\n");
	}


	/* Existence check for mutual duplicates */

	MutualDuplicate = false;
	i = j = 0;

	while( i < NPos && j < NNeg )
	{
	    PosTuple = PosCopy[i];
	    NegTuple = NegCopy[j];

	    for ( k = 1 ; k <= N && PosTuple[k] == NegTuple[k] ; k++ )
		;

	    if ( k > N ) /* tuples are duplicates */
	    {
	        MutualDuplicate = true;
		break;
	    }
	    else
	    if ( PosTuple[k] < NegTuple[k] )
	    {
		i++;
	    }
	    else
	    {
		j++;
	    }
	}

	if ( MutualDuplicate ) 
	{
	    printf("    (warning: contains tuples that are both ");
	    printf("positive and negative)\n");
	}

	pfree(NegCopy);
    }

    pfree(PosCopy);
}



/******************************************************************************/
/*                                                                            */
/*	CountDuplicates(T,N,left,right) - count the number of duplicate	      */
/*	    tuples in T between left and right.       			      */
/*		Sorts tuples on order given by comparison of Const type.      */
/*		N.B. This comparison is used even for continuous values as    */
/*		only checking for duplicates. 				      */
/*                                                                            */
/******************************************************************************/


int  CountDuplicates(Tuple *T, int N, int Left, int Right)
/*   ---------------  */
{
    register int	i, j, last, first, swap, count=0;
    register Tuple	temp, comp, other;

    if ( Left >= Right ) return 0;

    temp = T[Left];
    T[Left] = T[swap=(Left+Right)/2];
    T[swap] = temp;

    last = Left;

    comp = T[Left];

    for ( i = Left + 1; i <= Right; i++ )
    {
        other = T[i];
	for( j = 1 ; j <= N && other[j] == comp[j] ; j++ )
	    ;

        if ( j > N || other[j] < comp[j] ) /* other <= comp */
	{
	    temp = T[++last];
	    T[last] = T[i];
	    T[i] = temp;
	}
    }

    temp = T[Left];
    T[Left] = T[last];
    T[last] = temp;

    first = last;

    for ( i = last - 1; i >= Left ; i-- )
    {
        other = T[i];
	for ( j = 1 ; j <= N && other[j] == comp[j] ; j++ )
	    ;

	if ( j > N ) /* other == comp */
	{
	    temp = T[--first];
	    T[first] = T[i];
	    T[i] = temp;
	    count++;
	}
    }

    count += CountDuplicates(T,N,Left,first-1);
    count += CountDuplicates(T,N,last+1,Right);

    return count;
}



Boolean  SymmetryCheck(Relation R)
/*       -------------    */
{
    Tuple	*TheTuples;
    Boolean	*SymCheck;
    int		i, j, NPos;
    Const	T1, T2;

    if ( R->Arity != 2 ||
	 R->TypeRef[1]->Continuous || R->TypeRef[2]->Continuous )
    {
        return false;
    }

    TheTuples = R->Pos;
    NPos = Number(TheTuples);

    SymCheck = Alloc(NPos, Boolean);
    memset(SymCheck, false, NPos*sizeof(Boolean));

    ForEach(i, 0, NPos-1)
    {
        if ( SymCheck[i] ) continue; 

        T1 = TheTuples[i][1];
        T2 = TheTuples[i][2];
        for ( j = i ;
	      j < NPos && ( T1 != TheTuples[j][2] || T2 != TheTuples[j][1] ) ;
	      j++ )
	    ;

        if ( j == NPos )
	{
	    pfree(SymCheck);
	    return false;
        }
        SymCheck[j] = true;
    }

    pfree(SymCheck);
    return true;
}


	/*  Construct the index for a set of tuples for relation R  */

Index  MakeIndex(Tuple *T, int N, Relation R)
/*     ---------  */
{
    Index	IX;
    Tuple	Case, *Scan;
    int		**Next, Arg, Val, No = 0;

    /*  Allocate storage  */

    IX = Alloc(N+1, int **);
    Next = Alloc(N+1, int *);

    ForEach(Arg, 1, N)
    {
	IX[Arg] = Alloc(MaxConst+1, int *);
	Next[Arg] = AllocZero(MaxConst+1, int);
    }

    for ( Scan = T ; Case = *Scan++ ; )
    {
	ForEach(Arg, 1, N)
	{
	    if ( ! R->TypeRef[Arg]->Continuous )
	        Next[Arg][Case[Arg]]++;
	}
    }

    ForEach(Arg, 1, N)
    {
	ForEach(Val, 1, MaxConst)
	{
	    IX[Arg][Val] = Next[Arg][Val] ? Alloc(Next[Arg][Val]+1, int) : Nil;
	    Next[Arg][Val] = 0;
	}
    }

    /*  Construct the index  */

    for ( Scan = T ; *Scan ; Scan++ )
    {
	ForEach(Arg, 1, N)
	{
	    if ( ! R->TypeRef[Arg]->Continuous )
	    {
	        Val = (*Scan)[Arg];
		IX[Arg][Val][Next[Arg][Val]++] = No;
	    }
	}

	No++;
    }

    /*  Terminate index and free Next  */

    ForEach(Arg, 1, N)
    {
	ForEach(Val, 1, MaxConst)
	{
	    if ( IX[Arg][Val] )
	    {
		IX[Arg][Val][Next[Arg][Val]] = FINISH;
	    }
	}
	pfree(Next[Arg]);
    }
    pfree(Next);

    return IX;
}



/******************************************************************************/
/*									      */
/*	Basic routine -- read a delimited name into string s		      */
/*									      */
/*	  - Embedded spaces are permitted, but multiple spaces are replaced   */
/*	    by a single space						      */
/*	  - Any character escaped by \ is ok				      */
/*	  - Characters after | are ignored				      */
/*									      */
/******************************************************************************/



char  ReadName(char *s)
/*    ---------  */
{
    register char	*Sp = s;
    register int	c;

    /*  Skip to first non-space character  */

    while ( (c = getchar()) != EOF && ( c == '|' || Space(c) ) )
    {
	if ( c == '|' ) SkipComment;
    }

    /*  Return period if no names to read  */

    if ( c == EOF )
    {
	return (SectionDelim = '.');
    }
    else
    if ( c == ';' || c == '.' )
    {
	ReadToEOLN;
	return (SectionDelim = c);
    }

    /*  Read in characters up to the next delimiter  */

    while ( c != ',' && c != '\n' && c != '|' && c != EOF &&
	    c != '(' && c != ')'  && c != ':' && c != '.' )
    {
	if ( c == '\\' ) c = getchar();

	*Sp++ = c;
	if ( c == ' ' )
	    while ( ( c = getchar() ) == ' ' );
	else
	    c = getchar();

	if ( c == '.' ) /* Check for embedded period in number */
	{
	    c = getchar();
	    if (isdigit(c))
	    {
	        *Sp++ = '.';
	    }
	    else
	    {
	        ungetchar(c);
		c = '.';
	    }
	}
    }

    if ( c == '|' )
    {
	SkipComment;
	c = '\n';
    }

    /*  Strip trailing spaces  */

    while ( Sp > s && Space(*(Sp-1)) ) Sp--;
    *Sp++ = '\0';

    return c;
}



	/*  Find a constant using binary chop search  */


Const  FindConstant(char *N, Boolean MustBeThere)
/*     ------------  */
{
    int	i, Hi=MaxConst+1, Lo=1, Differ=1;

    while ( Lo < Hi-1 )
    {
	Differ = strcmp(N, ConstName[SortedConst[i = (Hi + Lo)/2]]);

	if ( ! Differ )		return SortedConst[i];
	else
	if ( Differ > 0 )	Lo = i;
	else			Hi = i;
    }

    if ( MustBeThere ) Error(3, N);

    /*  This is a new constant -- record it  */

    MaxConst++;
    if ( MaxConst % 1000 == 1 )
    {
	Realloc(ConstName, MaxConst+1000, char *);
	Realloc(SortedConst, MaxConst+1000, int);
    }

    Lo++;
    for ( i = MaxConst ; i > Lo ; i-- )
    {
	SortedConst[i] = SortedConst[i-1];
    }
    SortedConst[Lo] = MaxConst;
    ConstName[MaxConst] = CopyString(N);

    return MaxConst;
}



	/*  Check whether different types are compatible, i.e.
	    share at least one common value  */

void  CheckTypeCompatibility()
/*    ----------------------  */
{
    int T1, T2;

    Compatible = Alloc(MaxType+1, Boolean *);
    ForEach(T1, 1, MaxType)
    {
	Compatible[T1] = Alloc(MaxType+1, Boolean);
    }

    Verbose(2) putchar('\n');

    ForEach(T1, 1, MaxType)
    {
	Compatible[T1][T1] = true;

	ForEach(T2, T1+1, MaxType)
	{
	    Compatible[T1][T2] = Compatible[T2][T1] =
	        ( Type[T1]->Continuous || Type[T2]->Continuous ) ?
		false:
		CommonValue(Type[T1]->NValues, Type[T1]->Value,
		            Type[T2]->NValues, Type[T2]->Value);

	    Verbose(2)
	    {
		printf("Types %s and %s %s compatible\n",
			Type[T1]->Name, Type[T2]->Name,
			Compatible[T1][T2] ? "are" : "are not");
	    }
	}
    }
}


Boolean  CommonValue(int N1, Const *V1, int N2, Const *V2)
/*       -----------  */
{
    int i, j;

    ForEach(i, 0, N1-1)
    {
        ForEach(j, 0, N2-1)
	{
	    if ( V1[i] == V2[j] ) return true;
	}
    }

    return false;
}



int  Number(Tuple *T)
/*   ------  */
{
    int Count=0;

    if ( ! T ) return 0;

    while ( *T++ )
    {
	Count++;
    }

    return Count;
}



char  *CopyString(char *s)
/*     ----------  */
{
    char *new;
    int l;

    l = strlen(s) + 1;
    new = Alloc(l, char);
    memcpy(new, s, l);

    return new;
}



void  Error(int n, char *s1, char *s2)
/*    -----  */
{
    switch ( n )
    {
        case 1:	printf("Illegal delimiter after %s\n", s1);
		exit(1);

	case 2:	printf("Something wrong after %s in type %s\n", s1, s2);
		exit(1);

	case 3: printf("Undeclared constant %s\n", s1);
		exit(1);

	case 4: printf("Constant %s is not of type %s\n", s1, s2);
		exit(1);

	case 5: printf("Undeclared type %s\n", s1);
		exit(1);

	case 6: printf("Cannot use CWA for %s (continuous types)\n", s1);
		exit(1);

	case 7: printf("Type %s contains duplicate constant %s\n", s2, s1);
		exit(1);
    }
}



    /*  Check for arguments that cannot be equal  */

void  UnequalArgsCheck(Relation R)
/*    ----------------  */
{
    Var	S, F;

    R->ArgNotEq = AllocZero(ArgPair(R->Arity,R->Arity), Boolean);
    ForEach(S, 2, R->Arity)
    {
	ForEach(F, 1, S-1)
	{
	    R->ArgNotEq[ ArgPair(S,F) ] = NeverEqual(R->Pos, F, S);
	}
    }
}



Boolean  NeverEqual(Tuple *T, Var F, Var S)
/*       ----------  */
{
    Tuple	Case;

    while ( Case = *T++ )
    {
	if ( Case[F] == Case[S] && Case[F] != OUT_OF_RANGE ) return false;
    }

    return true;
}
