
# This is a shell archive.  Remove anything before this line,
# then unpack it by saving it in a file and typing "sh file".
#
# Wrapped by ml2!quinlan on Mon Jan 22 09:32:46 EST 1996
# Contents:  =README= MANUAL Makefile ackermann.d c4tofoil.c constants.c crx.d
#	crx.data crx.names crx.test defns.i determinate.c evaluatelit.c
#	extern.i finddef.c global.c input.c interpret.c join.c literal.c main.c
#	member.d member.explain ncm.d order.c output.c prune.c qs44.d search.c
#	sort.d state.c utility.c
 
echo x - =README=
sed 's/^@//' > "=README=" <<'@//E*O*F =README=//'
FOIL 6.4
--------

FOIL6 is a fairly comprehensive (and overdue) rewrite of FOIL5.2.
The code is now more compact, better documented, and faster for most
tasks.  The language has changed to ANSI C.

In the process of rewriting, several small changes have been made to
the algorithm.  Some of these correct minor glitches (such as
restoring the number of weak literals after search is restarted).
Others change the behaviour of FOIL: for instance, the checks before
literal evaluation have been strengthened and more pruning heuristics
introduced.  You are therefore advised not to discard FOIL5.2 until
you are satisfied that FOIL6 behaves properly on your tasks.

Here is a quick summary of notable intentional changes.  Of course,
others (aka bugs) might have crept in during the rewrite -- please
report any that you find.

  * Pruning heuristics strengthened.  Even though these are risk-free,
    the behavious of FOIL6 can differ from that of FOIL5.2, which
    sometimes evaluated literals that could not be used in the
    definition.  Generally, FOIL6 prunes more than FOIL5.2 but this
    is not uniformly the case.

  * Encoding cost of clauses changed.  FOIL5.2 did not `charge' for
    determinate literals, but included them in the reordering credit.
    FOIL6 still does not charge for determinate literals, but excludes
    them when calculating reordering credit.

  * Calculation of encoding cost of continuous literals altered.  The
    calculation is approximate, using the number of tuples rather than
    the number of distinct values as in FOIL5.2.

  * Number of variables changeable with -V option and default value
    (52) lower than previous built-in value (104).

  * Allowable number of weak literals now changeable with -w option
    with default value (3) same as previous built-in value.

  * Sampling option renamed -s.

  * Sampling corrected to generate the right number of negative tuples.

  * New option -N to allow A<>B, A<>constant while still excluding
    negated literals on user-defined relations.

  * Continuous types declared as "X: continuous." rather than the
    cryptic "X:@ ."  This change has also been made in c4tofoil.

  * Variables of the same continuous type can be compared with =, <>.

  * Output verbosity levels changed.  The old default level 1 contained
    more information that the current default (also level 1); the new
    level 2 is similar to the old level 1 and so on.  Some verbose
    output has been reformatted.

  * Number of weak literals in succession correctly recovered after
    a backup.

  * Options for uniform coding, booting with determinate literals from
    previous clause, and recursive checking of test cases suspended
    pending some rethinking.  Look for their replacements (and others)
    in later releases (we hope).

  * Theory constants applicable only to their own type, rather than to
    any compatible type.

  * Only highest-gain thresholds of continuous attributes considered for
    saving as best clause encountered during search.  FOIL5.2 monitored
    all literals in this respect, even though only the best threshold
    was considered for saving as a backup.


GENERAL
-------

The file "MANUAL" contains an explanation of the form of the input and
the options.

The executable version of the program, foil6, may be made with the command
"make foilgt" (or "make foil" for the slower version for debugging).

A small example input file, "member.d", is provided to demonstrate the
program, and may be used with the command "foil6 -n -v3 < member.d". The
file member.explain discusses the input and output for this small task.

Several other example files (*.d) are also provided for your use. The
names of these should be sufficiently mnemonic to enable recognition from
the literature.

In addition to the example input files, the program c4tofoil.c for
converting files from the format used by C4.5 to that used by FOIL is also 
included.  An example file (the credit data) has been provided, with a
names file augmented as required to demonstrate the use of this additional
program.  For further details on use of the program, see its header.

FOIL has a mixed authorship.  The original versions were produced by
Ross Quinlan; Mike Cameron-Jones then generated versions 3 to 5.2.
FOIL6 was produced by Ross, incorporating many of the algorithms and
routines developed by Mike (and with Mike identifying numerous glitches
in the recoding).  The utility c4tofoil was produced entirely by Mike.

We would appreciate it if you could mail Mike when you have ftp'ed a
copy of FOIL6 so that we can keep track of its whereabouts.  Comments
and bug reports are most welcome.


		Ross Quinlan (quinlan@cs.su.oz.au)
                Mike Cameron-Jones (mcj@cs.su.oz.au)


Subsidiary notes for release 6.1
--------------------------------

Principal changes for this release are:

  *  A special constant to denote "out of closed world" has been introduced.
     See MANUAL for the explanatory reference.

  *  A time limit (default 100 seconds) has been set for evaluation of
     all possible literals for one relation.  The idea is to truncate some
     very long (and often pointless) searches.

  *  Potential literals are screened to rule out those requiring the
     same value for different arguments when the corresponding relation
     never has equal arguments in those positions.

  *  Relations are ordered differently.  This may cause problems on some
     datasets.

  *  Pruning has been weakened to allow more complete search for saveable
     literals.

  *  The meaning of the maximum alternatives option (-l) has been changed
     by one; -l 4 now means "best plus four alternatives".

  *  When deciding whether to replace a clause with the best saved clause,
     the criterion is clause coding length rather than number of literals.

  *  The problems encountered with some Sun compilers should have been fixed.
     (The difficulty was caused by a statement of the form A[x++] = B; where
     some compilers increment x before evaluating B.)

  *  Time is now measured by getrusage() rather than clock().  This has the
     advantage of eliminating wraparound on long runs, but might require
     the addition of BSD libraries if you do not run BSD Unix.

  *  Various minor errors/omissions have been rectified.


Subsidiary notes for release 6.1a
---------------------------------

Changes for this additional release are:

  *  The program to convert from c4.5 to FOIL format has been corrected
     and modified.

  *  A bug in the printing of simplified clauses has been fixed.

  *  There are now limits on the total number of backups per clause (equal
     to the maximum number of checkpoints, -t option, default 20).

  *  The Quicksort routine has been modified to divide on the middle
     element rather than the last.


Subsidiary notes for release 6.2
--------------------------------

Changes for this release are:

  *  Repetitious literals excluded.  A literal that would repeat a literal
     that already appears in the clause, with perhaps a change of free
     variables, is excluded.

  *  Better clause simplification.  Implicit equalities are Vi=Vj or Vi=c
     established by the relations are now placed into the clause before
     pruning.

  *  The MDL coding was changed to take account of sampling, if used;
     the coding now behaves as if all - tuples (not just a sample) were
     defined.

  *  Several small bugs have been fixed (e.g. don't try to recover if a
     saved clause is available; check for duplicated constants in a type).


Subsidiary notes for release 6.3
--------------------------------

Changes for this release are:

  *  When simplified clauses are printed at the end of a run, unbound
     variables in negated literals are now printed as '_n' where n is
     a small integer.  (This also fixed a bug in the printing, but not
     the interpretation, of simplified clauses.)

  *  A few more small bugs have been identified and fixed.


Subsidiary notes for release 6.4
--------------------------------

Changes for this release are:

  *  Minor efficiency improvements and a couple of bug fixes.  The most
     noticeable bug was that literals with bound continuous variables were
     not being considered.
@//E*O*F =README=//
chmod u=rw,g=,o= =README=
 
echo x - MANUAL
sed 's/^@//' > "MANUAL" <<'@//E*O*F MANUAL//'
NAME
	foil6 - produce Horn clauses from relational data

SYNOPSIS
	foil6 [ -n ] [ -N ] [ -v verb ] [ -V vars ] [ -s frac ] [ -m maxt ]
	      [ -d depth ] [ -w weaklits ] [ -a accur ] [ -l alter ]
	      [ -t chkpt ] [ -f gain ] [ -g max ]

DESCRIPTION
	FOIL is a program that reads extensional specifications of a set of
	relations and produces Horn clause definitions of one or more of them.

INPUT
*****

Input to the program consists of three sections:

        * specification of types
          blank line
        * extensional definitions of relations
          blank line                            |  these are
        * test cases for learned definitions    |  optional

Types
-----

Each discrete type specification consists of the type name followed by a colon,
then a series of constants separated by commas and terminated with a period.
This may occupy several lines and the same constant can appear in many types.

There are three kinds of discrete types:

	* ordered types (type name preceded by '*')
	  The constants have a natural order and appear in this order in
	  the type definition, smallest constant first.
	* unordered types (type name preceded by '#')
	  The constants do not have any natural order.
	* possibly ordered types
	  FOIL will attempt to discover an ordering of the constants that
	  may be useful for recursive definitions.

Each continuous type specification consists of the type name followed by
": continuous." on one line.  The constants corresponding to a continuous type
are the usual integers and real numbers -- any string that can be converted to
a float by C's atof() should work when specifying a value in a tuple.

Constants
---------

A non-numeric constant consist of any string of characters with the exception
that any occurrence of certain delimiter characters (left and right parenthesis,
period, comma, semicolon) must be prefixed by the escape character '\'.
A "theory" constant that can appear in a definition should be preceded by
'*'.  Two one-character constants have a special meaning and should not
be used otherwise:

	* '?' indicates a missing or unknown value
	      (see Cameron-Jones and Quinlan, 1993b)
	* '^' indicates an out-of-closed-world value
	      (see Quinlan and Cameron-Jones, 1994)

Relations
---------

All relations are defined in terms of the set of positive tuples of constants
for which the relation is true, and optionally the set of negative tuples of
constants for which it is false.  If only positive tuples are given, all other
constant tuples of the correct types are considered to be negative.

Each relation is defined by a header and one or two sets of constant tuples.
The header can be specified as follows:

    name(type, type, ... , type) key/key/.../key

The header of all relations other than target relations begins with '*'.
The header consists of relation name, argument types and optional keys.
Keys limit the ways the relation may be used and consist of one character
for each type.  The character '#' indicates that the corresponding argument in
a literal must be bound; the character '-' indicates that the argument can be
bound or unbound.  Each key thus gives a permissible way of accessing the
relation.  If no keys appear, all possible combinations of bound and unbound
arguments are allowed.

Following the header line are a series of lines containing constant tuples:

    positive tuple
    positive tuple
      . . .
    ;			| these
    negative tuple	| are
    negative tuple	| optional
      . . .		|
    .

Each tuple consists of constants separated by commas and must appear on a
single line.  The character ';' separates positive tuples from negative
tuples, which are optional.

Tests
-----

The optional test relations may be given to test the learned Horn clause 
definitions.  The additional input consists of

        a blank line (indicating start of test relation specification)
        relation name
        test tuples
        .
        relation name
        test tuples
        .
          and so on

Each test tuple consists of a constant tuple followed by ": +" if it is 
belongs to the relation and ": -" if it does not.  The definition interpreter is
simple; right-hand sides of the clauses are checked with reference to the
given tuples, not to the definitions of the relations that may have been
learned.

OPTIONS
*******

Options and their meanings are:

        -n      Negative literals are not considered.  This may be useful in
                domains where negated literals wouldn't make sense, or if
                learned definitions must be Horn clauses.

	-N	This is similar, but permits negated equality literals
		A<>B and A<>constant.

        -vverb	Set verbosity level [0, 1, 2, 3, or 4; default 1]
                The program produces rather voluminous trace output controlled
                by this variable.  The default value of 1 gives a fair
                amount of detail; 0 produces very little output; 3 gives
                a blow-by-blow account of what the system is doing;
                4 gives details of tuples in training sets etc.

	-Vvars	Set the maximum number of variables that can be used during
		the search for a definition. [default: 52]

        -sfrac	In some predicates of high arity, the closed world assumption
                will generate very many negative tuples.  This option causes
                only a randomly-selected neg% of negative tuples to be used.
                Note that this option has no effect if negative tuples are
                given explicitly.

	-mmaxt	Set the maximum number of tuples; the default is 100000.
		If the default setting results in warnings that literals are
		being excluded due to the tuple limit, expanding the limit
		may be useful (but time-consuming).

        -ddepth	Set the maximum variable depth [default 4].  This limits the
                possible depth of variables in literals.

	-wwklts Set the maximum number of weak (zero-gain) literals that
		can appear in sequence [default: 4].  A batch of determinate
		literals counts as one literal in this respect.

        -aaccur	Set the minimum accuracy of any clause [default 80%]
                FOIL will not accept any clause with an accuracy lower
                than this.

	-lalter Set the maximum number of alternatives to any literal
		[default 5].  This limits the amount of backup from any 
		one point.

        -tchkpt	Set the maximum number of checkpoints at any one time 
		[default 20].

        -fgain	Any alternative literal must have at least gain%
                of the best literal gain [default 80%].  

        -gmax	Determinate literals are automatically included, unless
                there is a literal which has at least max% of the maximum
                possible gain.  (The maximum possible gain is achieved
                by a literal that is satisfied by all + tuples, but no
                - tuples, in the current training set.)  Obviously, if
                max is zero, no determinate literals are included unless
                there are no other literals.


SEE ALSO

	Quinlan, J.R. (1990), "Learning Logical Definitions from Relations",
	Machine Learning 5, 239-266.

	Quinlan, J.R. (1991), "Determinate Literals in Inductive Logic
	Programming", Proceedings 12th International Joint Conference on
	Artificial Intelligence, 746-750, Morgan Kaufmann.

	Quinlan, J.R. and Cameron-Jones, R.M. (1993), "FOIL: a midterm report",
	3-20, Proceedings European Conference on Machine Learning, Springer
	Verlag.

	Cameron-Jones, R.M. and Quinlan, J.R. (1993a), "Avoiding Pitfalls When
	Learning Recursive Theories", Proceedings IJCAI 93, 1050-1055,
	Morgan Kaufmann.

	Cameron-Jones, R.M. and Quinlan, J.R., (1993b), "First Order Learning,
	Zeroth Order Data", Sixth Australian Joint Conference on Artificial
	Intelligence, World Scientific.

	Quinlan, J.R. and Cameron-Jones, R.M., (1994), "Living in a Closed
	World", draft available by anonymous ftp from ftp.cs.su.oz.au
	(file pub/q+cj.closed.ps).
@//E*O*F MANUAL//
chmod u=rw,g=,o= MANUAL
 
echo x - Makefile
sed 's/^@//' > "Makefile" <<'@//E*O*F Makefile//'
# Vanilla makefile for distribution
# You may need to set local c compiler options

CFLAGS = -g
 
@.SUFFIXES: .o .c .l .ln

@.c.o:	Makefile defns.i extern.i
#	lint -c $<
	cc $(CFLAGS) -c $<

@.c.ln:
	lint -c $<

SRC =	global.c main.c input.c output.c state.c\
	literal.c evaluatelit.c search.c determinate.c order.c\
	join.c utility.c finddef.c interpret.c prune.c constants.c

OBJ =	global.o main.o input.o output.o state.o\
	literal.o evaluatelit.o search.o determinate.o order.o\
	join.o utility.o finddef.o interpret.o prune.o constants.o

LINT =	global.ln main.ln input.ln output.ln state.ln\
	literal.ln evaluatelit.ln search.ln determinate.ln order.ln\
	join.ln utility.ln finddef.ln interpret.ln prune.ln constants.ln


foil:   $(OBJ) Makefile
#	lint -x $(LINT) -lm >,nittygritty
	cc -o foil6 $(OBJ) -lm


foilgt:  $(SRC) defns.i Makefile
	cat defns.i $(SRC) >.temp
	egrep -v '"defns.i"|"extern.i"' .temp >foilgt.c
	cc -O3 -o foil6 foilgt.c -lm
	rm .temp foilgt.c


$(OBJ): defns.i extern.i
@//E*O*F Makefile//
chmod u=rw,g=,o= Makefile
 
echo x - ackermann.d
sed 's/^@//' > "ackermann.d" <<'@//E*O*F ackermann.d//'
*N: *0,*1,2,3,4,5,6,7,8,9,10,
   11,12,13,14,15,16,17,18,19,20.

Ackermann(N,N,N) ##-
0,0,1
0,1,2
0,2,3
0,3,4
0,4,5
0,5,6
0,6,7
0,7,8
0,8,9
0,9,10
0,10,11
0,11,12
0,12,13
0,13,14
0,14,15
0,15,16
0,16,17
0,17,18
0,18,19
0,19,20
1,0,2
1,1,3
1,2,4
1,3,5
1,4,6
1,5,7
1,6,8
1,7,9
1,8,10
1,9,11
1,10,12
1,11,13
1,12,14
1,13,15
1,14,16
1,15,17
1,16,18
1,17,19
1,18,20
2,0,3
2,1,5
2,2,7
2,3,9
2,4,11
2,5,13
2,6,15
2,7,17
2,8,19
3,0,5
3,1,13
4,0,13
@.
*succ(N,N)
0,1
1,2
2,3
3,4
4,5
5,6
6,7
7,8
8,9
9,10
10,11
11,12
12,13
13,14
14,15
15,16
16,17
17,18
18,19
19,20
@.
@//E*O*F ackermann.d//
chmod u=rw,g=,o= ackermann.d
 
echo x - c4tofoil.c
sed 's/^@//' > "c4tofoil.c" <<'@//E*O*F c4tofoil.c//'
/*****************************************************************************/
/*                                                                           */
/* Program to convert files from the standard C4.5 input format to a form    */
/* that can be used by FOIL                                                  */
/*                                                                           */
/* The relation to be found by FOIL will be of the form -                    */
/* is first class named in the .names file                                   */
/*                                                                           */
/* Hence changing the order of the class names will cause FOIL to find other */
/* relations from the same data                                              */
/*                                                                           */
/* Compilation and use:                                                      */
/* cc -o cf c4tofoil.c (produce executable cf)                               */
/* cf -f filestem (take filestem.names and filestem.data (and filestem.test  */
/*                 if present) and produce filestem.d for FOIL)              */
/* option -v produces some extra output on the standard output               */
/*                                                                           */
/* (Any error messages are currently printed on the standard output stream)  */
/*                                                                           */
/* Modification required to filestem.names:                                  */
/* Each line containing attribute information should have information        */
/* specifying the type - this is added as a C4.5 comment thus...             */
/*                                                                           */
/* (attribute info for C4.5) | type: typename                                */
/*                                                                           */
/* where typename is the name of the type of this attribute. Each typename   */
/* shall start with a letter (upper or lower case) and no typename shall be  */
/* the prefix of another typename.                                           */
/* (The latter restriction is required as the output for FOIL distinguishes  */
/* between constants of different types by prefixing them with their         */
/* typename).                                                                */
/*                                                                           */
/* For example:                                                              */
/* aardvarkish: true, false. | type: Boolean                                 */
/*                                                                           */
/* Note that values of discrete attributes which occur in the data file      */
/* become theory constants for FOIL. (However those that occur in the test   */
/* file but not data file, are just constants, not theory constants).        */
/*                                                                           */
/*****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAXNAMELENGTH 10000 /* Vast possible name length */

#define SkipComment     while ( ( (c = getc(fp)) != '\n' ) && (c!=EOF) )

typedef struct _att_info *att_info; /* Attribute information */
struct _att_info
{
    char *name;
    int ignore; /* !0 if to be ignored */
    int discrete; /* !0 if discrete */
    int discrete_n; /* the max number of values if "discrete N", else 0 */
    int n_values; /* values stored for discrete attributes */
    char **values;
    int *value_occurs_tr; /* number of occurrences in training of value */
    char *type_name;
    int type_number; /* number of the corresponding type */
};

typedef struct _type_info *type_info; /* Type information */
struct _type_info
{
    char *name;
    int n_atts; /* number of attributes of this type */
    int *atts; /* attributes of this type */
    int discrete; /* !0 if discrete */
    int n_values; /* values stored for discrete types */
    char **values; 
    int *value_occurs_tr; /* number of occurrences in training of value */
};   

int terminator; /* Character(/EOF) which caused previous scan to end */
char *name; /* Current name being processed */

char *copy_string(char *s);
int  get_next_name(FILE *fp, int skip, int embed);
void get_type_name(att_info attribute, FILE *fp);

/*****************************************************************************/
/* main - monolithic program to read the C4.5-style files and produce the    */
/*        FOIL-style one.                                                    */
/*****************************************************************************/

main(int argc, char *argv[])
{
    extern char *optarg;
    extern int optind;
    int o;

    int verbosity=0;
    char *filestem=NULL, *filename;

    FILE *fp, *tp;

    char **class_names=NULL;
    int n_class_names;

    char ***cases;
    int n_cases;

    char ***testing_cases;
    int n_testing_cases;

    att_info *attributes, attribute;
    int n_attributes;

    type_info *types, type;
    int n_types;

    int i, j, k;
    int c;

    /* Initialise the name char array */
    name = (char *) malloc(MAXNAMELENGTH*sizeof(char));


    /* Option handling */

    while ( (o=getopt(argc,argv,"vf:")) != -1 )
    {
        switch (o)
	{
	  case 'f':   filestem = copy_string(optarg);
                        break;
	  case 'v':   verbosity++;
	                break;
	  case '?':   printf("Option error in c4 to FOIL conversion\n");
                        exit(1);
	}
    }

    if(!filestem)
    {
        printf("Must enter filestem: -f filestem\n");
	exit(1);
    }

    /* Extract the class and attribute/type information from the .names file */

    filename = (char*) malloc((strlen(filestem)+7)*sizeof(char));
    sprintf(filename,"%s.names",filestem);
    fp = fopen(filename,"r");

    if(!fp)
    {
        printf("Can't open %s\n", filename);
	exit(1);
    }

    if(verbosity) printf("Reading from %s\n", filename);
    
    /* Get class names */

    n_class_names = 0;
    class_names = (char**) malloc(11*sizeof(char*));

    do
    {
        if(!get_next_name(fp,1,1))
	{
	    printf("Problem while reading class names\n");
	    exit(1);
	}
        if(n_class_names&&!(n_class_names%10)) 
	    class_names = (char**) realloc((void*)class_names,
					   (n_class_names+11)*sizeof(char*));
	class_names[n_class_names++] = copy_string(name);
    }while(terminator==',');	  


    if(verbosity)
    {
        printf("Have read class names:");
	for(i=0;i<n_class_names;i++)
	{
	    printf(" %s",class_names[i]);
	}
	printf("\n");
    }

    /* Get attribute info */

    n_attributes = 0;
    attributes =(att_info*)malloc(11*sizeof(att_info));

    while(get_next_name(fp,1,1))
    {
	if(terminator!=':')
	{
	    printf("Attribute %s, not followed by :\n", name);
	    exit(1);
	}

	if(n_attributes&&(n_attributes%10==0)) 
	    attributes = (att_info*) realloc((void*)attributes,
				   (n_attributes+11)*sizeof(att_info));

	attributes[n_attributes] = (att_info)malloc(sizeof(struct _att_info));
	attribute = attributes[n_attributes];

	attribute->name = copy_string(name);

	if(!get_next_name(fp,0,1))
	{
	    printf("Information missing for attribute %s\n", name);
	    exit(1);
	}

	if(!strcmp(name,"ignore"))
	{
	    attribute->ignore = 1;
	    attribute->discrete = 0;
	    if(terminator!='\n') SkipComment;
	}
	else if(!strcmp(name,"continuous"))
	{
	    attribute->ignore = 0;
	    attribute->discrete = 0;
	    attribute->discrete_n = 0;
	    attribute->n_values = 0;
	    attribute->values = NULL;
	    get_type_name(attribute,fp);
	}
	else if(!strncmp(name,"discrete",8))
	{
	    attribute->ignore = 0;
	    attribute->discrete = 1;
	    attribute->discrete_n = atoi(name+8);
	    attribute->n_values = 0;
	    attribute->values=(char**)malloc(attribute->discrete_n*
						 sizeof(char*));
	    if(attribute->discrete_n && (attribute->values==NULL))
	    {
	        printf("Problem allocating space for values of %s\n",
		       attribute->name);
		exit(1);
	    }
	    get_type_name(attribute,fp);
	}
	else /* Presume that this is discrete with specified values */
	{
	    attribute->ignore = 0;
	    attribute->discrete = 1;

	    attribute->discrete_n = 0;
	    attribute->n_values = 0;
	    attribute->values = (char**)malloc(11*sizeof(char*));
	    do
	    {
	        if(attribute->n_values&&!(attribute->n_values%10))
		    attribute->values = (char**) realloc(
						     (void*)attribute->values,
				      (attribute->n_values+11)*sizeof(char*));
		attribute->values[attribute->n_values++] = copy_string(name);
		if(terminator!=',') break;
	    }
	    while(get_next_name(fp,0,1));

	    get_type_name(attribute,fp);
	}
	n_attributes++;
    }
    fclose(fp);


    /* Read the .data file, extracting constant occurrence info */

    for(i=0;i<n_attributes;i++)
    {
	attribute = attributes[i];
	if(attribute->discrete)
	    if(attribute->n_values)
	        attribute->value_occurs_tr = 
		  (int*) calloc((size_t)attribute->n_values,sizeof(int));
	    else
	        attribute->value_occurs_tr =
		  (int*) calloc((size_t)attribute->discrete_n,sizeof(int));
    }

    sprintf(filename,"%s.data",filestem);
    fp = fopen(filename,"r");

    if(!fp)
    {
        printf("Can't open %s\n", filename);
	exit(1);
    }

    if(verbosity) printf("Reading from %s\n", filename);

    cases = (char***)malloc(101*sizeof(char**));
    n_cases = 0;

    while(get_next_name(fp,1,attributes[0]->discrete))
    {
        if(n_cases&&!(n_cases%100))
	    cases = (char***) realloc((void*)cases,
				      (n_cases+101)*sizeof(char**));
	cases[n_cases] = (char**)malloc((n_attributes+1)*sizeof(char*));

	i = 0;

	do
	{
	    attribute = attributes[i];
	    if(attribute->ignore)
	    {
	    }
	    else if(!strcmp(name,"?")) 
	    {
	        cases[n_cases][i] = NULL;
	    }
	    else if(attribute->discrete)
	    {
	        for(j=0;j<attribute->n_values;j++)
		{
		    if(!strcmp(name,attribute->values[j])) break;
		}
		if(j==attribute->n_values) /* value not seen before */
		{
		    if(j>=attribute->discrete_n)
		    {
		        printf("%s has extra value %s in data file\n",
			       attribute->name, name);
			exit(1);
		    }
		    else /* add it in */
		    {
		        attribute->values[j] = copy_string(name);
			attribute->n_values++;
		    }
		}
		attribute->value_occurs_tr[j]++;
		cases[n_cases][i] = attribute->values[j];
	    }
	    else /* continuous attribute with value */
	    {
		cases[n_cases][i] = copy_string(name);
	    }
	    i++;
	}
	while(get_next_name(fp,1,(i==n_attributes) ?
			    1 : attributes[i]->discrete)
	      &&(i<n_attributes));
	if(i!=n_attributes)
	{
	    printf("Problem reading line in %s\n", filename);
	    exit(1);
	}
	for(j=0;j<n_class_names;j++)
	{
	    if(!strcmp(name,class_names[j])) break;
	}
	if(j==n_class_names)
	{
	    printf("Undeclared class name %s in data file\n", name);
	    exit(1);
	}
	cases[n_cases][i] = class_names[j];

	n_cases++;
    }
    fclose(fp);

    /* Read the .test file, extracting constant existence info */

    sprintf(filename,"%s.test",filestem);
    fp = fopen(filename,"r");

    if(fp&&verbosity) printf("Reading from %s\n", filename);

    testing_cases = (char***)malloc(101*sizeof(char**));
    n_testing_cases = 0;
    while(fp&&get_next_name(fp,1,attributes[0]->discrete))
    {
        if(n_testing_cases&&!(n_testing_cases%100))
	    testing_cases = (char***) realloc((void*)testing_cases,
                                        (n_testing_cases+101)*sizeof(char**));
	testing_cases[n_testing_cases] = (char**)malloc((n_attributes+1)*
							sizeof(char*));

	i = 0;

	do
	{
	    attribute = attributes[i];
	    if(attribute->ignore)
	    {
	    }
	    else if(!strcmp(name,"?")) 
	    {
	        testing_cases[n_testing_cases][i] = NULL;
	    }
	    else if(attribute->discrete)
	    {
	        for(j=0;j<attribute->n_values;j++)
		{
		    if(!strcmp(name,attribute->values[j])) break;
		}
		if(j==attribute->n_values) /* value not seen before */
		{
		    if(j>=attribute->discrete_n)
		    {
		        printf("%s has extra value %s in data file\n",
			       attribute->name, name);
			exit(1);
		    }
		    else /* add it in */
		    {
		        attribute->values[j] = copy_string(name);
			attribute->n_values++;
		    }
		}
		testing_cases[n_testing_cases][i]=attribute->values[j];
	    }
	    else /* continuous attribute with value */
	    {
		testing_cases[n_testing_cases][i] = copy_string(name);
	    }
	    i++;
	}
	while(get_next_name(fp,1,(i==n_attributes)?
			    1 : attributes[i]->discrete)
	      &&(i<n_attributes));
	if(i!=n_attributes)
	{
	    printf("Problem reading line in %s\n", filename);
	    exit(1);
	}
	for(j=0;j<n_class_names;j++)
	{
	    if(!strcmp(name,class_names[j])) break;
	}
	if(j==n_class_names)
	{
	    printf("Undeclared class name %s in data file\n", name);
	    exit(1);
	}
	testing_cases[n_testing_cases][i] = class_names[j];

	n_testing_cases++;
    }

    /* Now combine the attribute information into type information */

    n_types = 0;
    types = (type_info*) malloc(11*sizeof(type_info));

    for(i=0;i<n_attributes;i++)
    {
        attribute = attributes[i];
	if(attribute->ignore) continue;
	for(j=0;j<n_types;j++)
	{
	    if(!strcmp(types[j]->name,attribute->type_name))
	        break;
	}
	attribute->type_number = j;
	if(j==n_types) /* New type */
	{
            if(n_types&&!(n_types%10))
	        types = (type_info*) realloc((void*)types, 
					     (n_types+11)*sizeof(type_info));
	    types[n_types] = (type_info)malloc(sizeof(struct _type_info));
	    types[n_types]->name = attribute->type_name;
	    types[n_types]->discrete = attribute->discrete;
	    types[n_types]->n_atts = 0;
	    types[n_types]->atts = (int*)malloc(11*sizeof(int));
	    types[n_types]->n_values = 0;
	    if(types[n_types]->discrete)
	    {
	        types[n_types]->values = (char**)malloc(11*sizeof(char*));
		types[n_types]->value_occurs_tr =(int*)malloc(11*sizeof(int*));
	    }
	    else
	    {
	        types[n_types]->values = NULL;
	    }
            n_types++;
	}
	type = types[j];
	if(type->n_atts&&!(type->n_atts%10))
	    type->atts = (int*) realloc((void*)type->atts,
					(type->n_atts+11)*sizeof(int));
	type->atts[type->n_atts++] = i;

	if(type->discrete!=attribute->discrete)
	{
	    printf("Type %s declared to be both discrete and continuous\n",
		   type->name);
	    exit(1);
	}

	if(type->discrete)
	{
	    for(j=0;j<attribute->n_values;j++)
	    {
	        for(k=0;k<type->n_values;k++)
		{
		    if(!strcmp(attribute->values[j],type->values[k]))
		        break;
		}
		if(k==type->n_values) /* New value for this type */
		{
		    if(type->n_values&&!(type->n_values%10))
		    {
		        type->values = (char**) realloc((void*)type->values,
					    (type->n_values+11)*sizeof(char*));
			type->value_occurs_tr =(int*)realloc(
						 (void*)type->value_occurs_tr,
					    (type->n_values+11)*sizeof(int*));
		    }
		    type->values[k] = attribute->values[j];
		    type->value_occurs_tr[k] = 0;
		    type->n_values++;
		}
		type->value_occurs_tr[k] += attribute->value_occurs_tr[j];
	    }
	}
    }

    /* Check the discrete type names to ensure that none is the prefix of
       another */
    for(i=0;i<n_types;i++)
    {
        if(!type->discrete) continue;
	for(j=i+1;j<n_types;j++)
	{
	    if(!type->discrete) continue;
	    if((int)strlen(types[i]->name)>(int)strlen(types[j]->name))
	    {
	        if(!strncmp(types[i]->name,types[j]->name,
			    strlen(types[j]->name)))
		{
		    printf("Type name %s is prefix of type name %s\n",
			   types[j]->name, types[i]->name);
		    exit(1);
		}
	    }
	    else
	    {
	        if(!strncmp(types[j]->name,types[i]->name,
			    strlen(types[i]->name)))
		{
		    printf("Type name %s is prefix of type name %s\n",
			   types[i]->name, types[j]->name);
		    exit(1);
		}
	    }
	}
    }

    /* Output type info on std out */
    if(verbosity)
    {
        printf("Type information from names file:\n\n");
	for(i=0;i<n_types;i++)
	{
	    type = types[i];
	    printf("%s:\n",type->name);
	    if(type->discrete) printf("\tdiscrete\n");
	    else printf("\tcontinuous\n");
	    printf("\tattributes:\n");
	    for(j=0;j<type->n_atts;j++)
	        printf("\t\t%s\n",attributes[type->atts[j]]->name);
	    printf("\tvalues:\n");
	    for(j=0;j<type->n_values;j++)
	        printf("\t\t%s\n",type->values[j]);
	}
    }

    /* Now write out to the .d file for FOIL */

    sprintf(filename,"%s.d",filestem);
    tp = fopen(filename,"w");

    if(verbosity) printf("Writing to %s\n", filename);

    /* First the types complete with constants - note that in the .d file each 
       discrete constant name is augmented by being preceded by its type
       name to prevent FOIL equating two constants of different types */

    for(i=0;i<n_types;i++)
    {
	type = types[i];
	if(type->discrete)
	{
	    fprintf(tp,"#%s: ",type->name);
	    k = 0;
	    for(j=0;j<type->n_values;j++)
	    {
	        if(k) fprintf(tp,", ");
		if(type->value_occurs_tr[j])
		    fprintf(tp,"*"); /* Theory Constant */
		fprintf(tp,"%s%s",type->name, type->values[j]);
		k++;
	    }
	    if(!k) fprintf(tp,"\n"); /* empty type */
	    fprintf(tp,".\n");
	}
	else
	    fprintf(tp,"%s: continuous.\n",type->name);
    }
    fprintf(tp,"\n");

    /* Now the sole relation - is_first_named_class() */

    fprintf(tp,"is_%s(", class_names[0]);

    k = 0;
    for(i=0;i<n_attributes;i++)
    {
        attribute = attributes[i];
        if(attribute->ignore) continue;
        if(k) fprintf(tp,",");
        fprintf(tp,"%s",attribute->type_name);
	k++;
    }
    if(!k)
    {
        printf("All attributes ignored\n");
	exit(1);
    }
    fprintf(tp,")\n");

    /* Now the positive training cases */

    for(i=0;i<n_cases;i++)
    {
        if(cases[i][n_attributes]!=class_names[0]) continue;

	k = 0;
	for(j=0;j<n_attributes;j++)
	{
	    attribute = attributes[j];
	    if(attribute->ignore) continue;

	    if(k) fprintf(tp,",");

	    if(!cases[i][j]) /* Missing Value */
	    {
	        fprintf(tp,"?");
	    }
	    else if(attribute->discrete)
	    {
	        fprintf(tp,"%s%s",attribute->type_name,cases[i][j]);
	    }
	    else /* attribute is continuous */
	    {
	        fprintf(tp,"%s",cases[i][j]);
	    }
	    k++;
	}
	fprintf(tp,"\n");
    }

    /* Now the negative training cases */

    fprintf(tp,";\n");
    for(i=0;i<n_cases;i++)
    {
        if(cases[i][n_attributes]==class_names[0]) continue;

	k = 0;
	for(j=0;j<n_attributes;j++)
	{
	    attribute = attributes[j];
	    if(attribute->ignore) continue;

	    if(k) fprintf(tp,",");
	    if(!cases[i][j]) /* Missing Value */
	    {
	        fprintf(tp,"?");
	    }
	    else if(attribute->discrete)
	    {
	        fprintf(tp,"%s%s",attribute->type_name,cases[i][j]);
	    }
	    else /* attribute is continuous */
	    {
	        fprintf(tp,"%s",cases[i][j]);
	    }
	    k++;
	}
	fprintf(tp,"\n");
    }
    fprintf(tp,".\n");

    /* Now the test cases */

    if(!fp)
    {
	fclose(tp);
	exit(0);
    }

    fprintf(tp,"\nis_%s\n", class_names[0]);

    for(i=0;i<n_testing_cases;i++)
    {
	k = 0;
	for(j=0;j<n_attributes;j++)
	{
	    attribute = attributes[j];
	    if(attribute->ignore) continue;

	    if(k) fprintf(tp,",");
	    if(!testing_cases[i][j]) /* Missing Value */
	    {
	        fprintf(tp,"?");
	    }
	    else if(attribute->discrete)
	    {
	        fprintf(tp,"%s%s",attribute->type_name,testing_cases[i][j]);
	    }
	    else /* attribute is continuous */
	    {
	        fprintf(tp,"%s",testing_cases[i][j]);
	    }
	    k++;
	}
        if(testing_cases[i][n_attributes]==class_names[0])
	    fprintf(tp,":+\n");
	else
	    fprintf(tp,":-\n");
    }
    fprintf(tp,".\n");
    fclose(fp);
    fclose(tp);

    return 0; /* Changed from exit(0) */
}

/*****************************************************************************/
/* copy_string(s)  - return a copy of the string s                           */
/*****************************************************************************/

char *copy_string(char *s)
{
    char *tmp;

    tmp = (char*)malloc((strlen(s)+1)*sizeof(char));
    return strcpy(tmp,s);
}

/*****************************************************************************/
/* get_next_name(fp,skip,embed) - read next name from file *fp into name     */
/*                  Modified version from FOIL's ReadName                    */
/*                  skip is !0 if trailing comment to be skipped             */
/*                  embed is !0 if need to escape an embedded period         */
/*****************************************************************************/

#define Space(c)        (c == ' ' || c == '\t' || c == '\n')

int  get_next_name(FILE *fp, int skip, int embed)
/*   ---------  */
{
    char *Sp = name;
    int c;

    /*  Skip to first non-space character  */

    while ( ( c = getc(fp) ) == '|' || Space(c) )
	if ( c == '|' ) SkipComment;

    /*  Return 0 if no names to read  */

    if ( c == EOF )
    {
	return 0;
    }

    /*  Read in characters up to the next delimiter  */

    while ( c != ',' && c != '\n' && c != '|' && c != EOF && c != ':')
    {
        if ( c == '.' )
	{
	    if(c=getc(fp))
	    {
	        if( Space(c) || c=='|' || c==EOF )
		{
		    c = '.';
		    break;
		}
	    }
	    else
	    {
	        c = '.';
		break;
	    }
	    if(embed) *Sp++ = '\\'; /* Need to escape embedded . */
	    *Sp++ = '.';
	}

	if ( c == '\\' ) 
	{
	    *Sp++ = (char) c; /* Perpetuate embedded characters for FOIL */
	    c = getc(fp);
	}

	*Sp++ = (char) c;
	if ( c == ' ' )
	    while ( ( c = getc(fp) ) == ' ' );
	else
	    c = getc(fp);
    }

    terminator = c;

    /* Skip trailing comment? */

    if(skip && (c == '|'))
    {
        SkipComment;
    }

    /*  Strip trailing spaces  */

    while ( Space(*(Sp-1)) ) Sp--;
    *Sp++ = '\0';

    return 1;
}

/*****************************************************************************/
/* get_type_name(attribute,fp) - get the type name for attribute from fp     */
/*****************************************************************************/

void get_type_name(att_info attribute, FILE *fp)
{
    while(terminator!='|'&&terminator!='\n'&&terminator!=EOF)
    {
        terminator = getc(fp);
    }

    if(terminator=='|'&&get_next_name(fp,1,1)&&!strcmp(name,"type")
       &&get_next_name(fp,1,1))
    {
        attribute->type_name = copy_string(name);
    }
    else
    {
        printf("Attribute %s lacks type info\n", attribute->name);
	exit(1);
    }

    return;
}
@//E*O*F c4tofoil.c//
chmod u=rw,g=,o= c4tofoil.c
 
echo x - constants.c
sed 's/^@//' > "constants.c" <<'@//E*O*F constants.c//'
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
@//E*O*F constants.c//
chmod u=rw,g=,o= constants.c
 
echo x - crx.d
sed 's/^@//' > "crx.d" <<'@//E*O*F crx.d//'
#A: *Ab, *Aa.
B: continuous.
#C: *Cu, *Cy, *Cl, Ct.
#D: *Dg, *Dp, *Dgg.
#E: *Ec, *Ed, *Ecc, *Ei, *Ej, *Ek, *Em, *Er, *Eq, *Ew, *Ex, *Ee, *Eaa, *Eff, *Ev, *Eh, *Ebb, *En, *Ez, *Edd, *Eo.
#F: *Ft, *Ff.
#G: *Gg, *Gp, *Gs.
H: continuous.

is_+(A,B,B,C,D,E,E,B,F,F,B,F,G,H,H)
Ab,30.83,0,Cu,Dg,Ew,Ev,1.25,Ft,Ft,01,Ff,Gg,00202,0
Aa,58.67,4.46,Cu,Dg,Eq,Eh,3.04,Ft,Ft,06,Ff,Gg,00043,560
Aa,24.50,0.5,Cu,Dg,Eq,Eh,1.5,Ft,Ff,0,Ff,Gg,00280,824
Ab,27.83,1.54,Cu,Dg,Ew,Ev,3.75,Ft,Ft,05,Ft,Gg,00100,3
Ab,20.17,5.625,Cu,Dg,Ew,Ev,1.71,Ft,Ff,0,Ff,Gs,00120,0
Ab,32.08,4,Cu,Dg,Em,Ev,2.5,Ft,Ff,0,Ft,Gg,00360,0
Ab,33.17,1.04,Cu,Dg,Er,Eh,6.5,Ft,Ff,0,Ft,Gg,00164,31285
Aa,22.92,11.585,Cu,Dg,Ecc,Ev,0.04,Ft,Ff,0,Ff,Gg,00080,1349
Ab,54.42,0.5,Cy,Dp,Ek,Eh,3.96,Ft,Ff,0,Ff,Gg,00180,314
Ab,42.50,4.915,Cy,Dp,Ew,Ev,3.165,Ft,Ff,0,Ft,Gg,00052,1442
Ab,22.08,0.83,Cu,Dg,Ec,Eh,2.165,Ff,Ff,0,Ft,Gg,00128,0
Ab,29.92,1.835,Cu,Dg,Ec,Eh,4.335,Ft,Ff,0,Ff,Gg,00260,200
Aa,38.25,6,Cu,Dg,Ek,Ev,1,Ft,Ff,0,Ft,Gg,00000,0
Ab,48.08,6.04,Cu,Dg,Ek,Ev,0.04,Ff,Ff,0,Ff,Gg,00000,2690
Aa,45.83,10.5,Cu,Dg,Eq,Ev,5,Ft,Ft,07,Ft,Gg,00000,0
Ab,36.67,4.415,Cy,Dp,Ek,Ev,0.25,Ft,Ft,10,Ft,Gg,00320,0
Ab,28.25,0.875,Cu,Dg,Em,Ev,0.96,Ft,Ft,03,Ft,Gg,00396,0
Aa,23.25,5.875,Cu,Dg,Eq,Ev,3.17,Ft,Ft,10,Ff,Gg,00120,245
Ab,21.83,0.25,Cu,Dg,Ed,Eh,0.665,Ft,Ff,0,Ft,Gg,00000,0
Aa,19.17,8.585,Cu,Dg,Ecc,Eh,0.75,Ft,Ft,07,Ff,Gg,00096,0
Ab,25.00,11.25,Cu,Dg,Ec,Ev,2.5,Ft,Ft,17,Ff,Gg,00200,1208
Ab,23.25,1,Cu,Dg,Ec,Ev,0.835,Ft,Ff,0,Ff,Gs,00300,0
Aa,47.75,8,Cu,Dg,Ec,Ev,7.875,Ft,Ft,06,Ft,Gg,00000,1260
Aa,27.42,14.5,Cu,Dg,Ex,Eh,3.085,Ft,Ft,01,Ff,Gg,00120,11
Aa,41.17,6.5,Cu,Dg,Eq,Ev,0.5,Ft,Ft,03,Ft,Gg,00145,0
Aa,15.83,0.585,Cu,Dg,Ec,Eh,1.5,Ft,Ft,02,Ff,Gg,00100,0
Aa,47.00,13,Cu,Dg,Ei,Ebb,5.165,Ft,Ft,09,Ft,Gg,00000,0
Ab,56.58,18.5,Cu,Dg,Ed,Ebb,15,Ft,Ft,17,Ft,Gg,00000,0
Ab,57.42,8.5,Cu,Dg,Ee,Eh,7,Ft,Ft,03,Ff,Gg,00000,0
Ab,42.08,1.04,Cu,Dg,Ew,Ev,5,Ft,Ft,06,Ft,Gg,00500,10000
Ab,29.25,14.79,Cu,Dg,Eaa,Ev,5.04,Ft,Ft,05,Ft,Gg,00168,0
Ab,42.00,9.79,Cu,Dg,Ex,Eh,7.96,Ft,Ft,08,Ff,Gg,00000,0
Ab,49.50,7.585,Cu,Dg,Ei,Ebb,7.585,Ft,Ft,15,Ft,Gg,00000,5000
Aa,36.75,5.125,Cu,Dg,Ee,Ev,5,Ft,Ff,0,Ft,Gg,00000,4000
Aa,22.58,10.75,Cu,Dg,Eq,Ev,0.415,Ft,Ft,05,Ft,Gg,00000,560
Ab,27.83,1.5,Cu,Dg,Ew,Ev,2,Ft,Ft,11,Ft,Gg,00434,35
Ab,27.25,1.585,Cu,Dg,Ecc,Eh,1.835,Ft,Ft,12,Ft,Gg,00583,713
Aa,23.00,11.75,Cu,Dg,Ex,Eh,0.5,Ft,Ft,02,Ft,Gg,00300,551
Ab,27.75,0.585,Cy,Dp,Ecc,Ev,0.25,Ft,Ft,02,Ff,Gg,00260,500
Ab,54.58,9.415,Cu,Dg,Eff,Eff,14.415,Ft,Ft,11,Ft,Gg,00030,300
Ab,34.17,9.17,Cu,Dg,Ec,Ev,4.5,Ft,Ft,12,Ft,Gg,00000,221
Ab,28.92,15,Cu,Dg,Ec,Eh,5.335,Ft,Ft,11,Ff,Gg,00000,2283
Ab,29.67,1.415,Cu,Dg,Ew,Eh,0.75,Ft,Ft,01,Ff,Gg,00240,100
Ab,39.58,13.915,Cu,Dg,Ew,Ev,8.625,Ft,Ft,06,Ft,Gg,00070,0
Ab,56.42,28,Cy,Dp,Ec,Ev,28.5,Ft,Ft,40,Ff,Gg,00000,15
Ab,54.33,6.75,Cu,Dg,Ec,Eh,2.625,Ft,Ft,11,Ft,Gg,00000,284
Aa,41.00,2.04,Cy,Dp,Eq,Eh,0.125,Ft,Ft,23,Ft,Gg,00455,1236
Ab,31.92,4.46,Cu,Dg,Ecc,Eh,6.04,Ft,Ft,03,Ff,Gg,00311,300
Ab,41.50,1.54,Cu,Dg,Ei,Ebb,3.5,Ff,Ff,0,Ff,Gg,00216,0
Ab,23.92,0.665,Cu,Dg,Ec,Ev,0.165,Ff,Ff,0,Ff,Gg,00100,0
Aa,25.75,0.5,Cu,Dg,Ec,Eh,0.875,Ft,Ff,0,Ft,Gg,00491,0
Ab,26.00,1,Cu,Dg,Eq,Ev,1.75,Ft,Ff,0,Ft,Gg,00280,0
Ab,37.42,2.04,Cu,Dg,Ew,Ev,0.04,Ft,Ff,0,Ft,Gg,00400,5800
Ab,34.92,2.5,Cu,Dg,Ew,Ev,0,Ft,Ff,0,Ft,Gg,00239,200
Ab,34.25,3,Cu,Dg,Ecc,Eh,7.415,Ft,Ff,0,Ft,Gg,00000,0
Ab,23.33,11.625,Cy,Dp,Ew,Ev,0.835,Ft,Ff,0,Ft,Gg,00160,300
Ab,23.17,0,Cu,Dg,Ecc,Ev,0.085,Ft,Ff,0,Ff,Gg,00000,0
Ab,44.33,0.5,Cu,Dg,Ei,Eh,5,Ft,Ff,0,Ft,Gg,00320,0
Ab,35.17,4.5,Cu,Dg,Ex,Eh,5.75,Ff,Ff,0,Ft,Gs,00711,0
Ab,43.25,3,Cu,Dg,Eq,Eh,6,Ft,Ft,11,Ff,Gg,00080,0
Ab,56.75,12.25,Cu,Dg,Em,Ev,1.25,Ft,Ft,04,Ft,Gg,00200,0
Ab,31.67,16.165,Cu,Dg,Ed,Ev,3,Ft,Ft,09,Ff,Gg,00250,730
Aa,23.42,0.79,Cy,Dp,Eq,Ev,1.5,Ft,Ft,02,Ft,Gg,00080,400
Aa,20.42,0.835,Cu,Dg,Eq,Ev,1.585,Ft,Ft,01,Ff,Gg,00000,0
Ab,26.67,4.25,Cu,Dg,Ecc,Ev,4.29,Ft,Ft,01,Ft,Gg,00120,0
Ab,34.17,1.54,Cu,Dg,Ecc,Ev,1.54,Ft,Ft,01,Ft,Gg,00520,50000
Aa,36.00,1,Cu,Dg,Ec,Ev,2,Ft,Ft,11,Ff,Gg,00000,456
Ab,25.50,0.375,Cu,Dg,Em,Ev,0.25,Ft,Ft,03,Ff,Gg,00260,15108
Ab,19.42,6.5,Cu,Dg,Ew,Eh,1.46,Ft,Ft,07,Ff,Gg,00080,2954
Ab,35.17,25.125,Cu,Dg,Ex,Eh,1.625,Ft,Ft,01,Ft,Gg,00515,500
Ab,52.50,6.5,Cu,Dg,Ek,Ev,6.29,Ft,Ft,15,Ff,Gg,00000,11202
Ab,57.83,7.04,Cu,Dg,Em,Ev,14,Ft,Ft,06,Ft,Gg,00360,1332
Aa,20.75,10.335,Cu,Dg,Ecc,Eh,0.335,Ft,Ft,01,Ft,Gg,00080,50
Ab,39.92,6.21,Cu,Dg,Eq,Ev,0.04,Ft,Ft,01,Ff,Gg,00200,300
Ab,25.67,12.5,Cu,Dg,Ecc,Ev,1.21,Ft,Ft,67,Ft,Gg,00140,258
Aa,24.75,12.5,Cu,Dg,Eaa,Ev,1.5,Ft,Ft,12,Ft,Gg,00120,567
Aa,44.17,6.665,Cu,Dg,Eq,Ev,7.375,Ft,Ft,03,Ft,Gg,00000,0
Aa,23.50,9,Cu,Dg,Eq,Ev,8.5,Ft,Ft,05,Ft,Gg,00120,0
Ab,34.92,5,Cu,Dg,Ex,Eh,7.5,Ft,Ft,06,Ft,Gg,00000,1000
Ab,47.67,2.5,Cu,Dg,Em,Ebb,2.5,Ft,Ft,12,Ft,Gg,00410,2510
Ab,22.75,11,Cu,Dg,Eq,Ev,2.5,Ft,Ft,07,Ft,Gg,00100,809
Ab,34.42,4.25,Cu,Dg,Ei,Ebb,3.25,Ft,Ft,02,Ff,Gg,00274,610
Aa,28.42,3.5,Cu,Dg,Ew,Ev,0.835,Ft,Ff,0,Ff,Gs,00280,0
Ab,67.75,5.5,Cu,Dg,Ee,Ez,13,Ft,Ft,01,Ft,Gg,00000,0
Ab,20.42,1.835,Cu,Dg,Ec,Ev,2.25,Ft,Ft,01,Ff,Gg,00100,150
Aa,47.42,8,Cu,Dg,Ee,Ebb,6.5,Ft,Ft,06,Ff,Gg,00375,51100
Ab,36.25,5,Cu,Dg,Ec,Ebb,2.5,Ft,Ft,06,Ff,Gg,00000,367
Ab,32.67,5.5,Cu,Dg,Eq,Eh,5.5,Ft,Ft,12,Ft,Gg,00408,1000
Ab,48.58,6.5,Cu,Dg,Eq,Eh,6,Ft,Ff,0,Ft,Gg,00350,0
Ab,39.92,0.54,Cy,Dp,Eaa,Ev,0.5,Ft,Ft,03,Ff,Gg,00200,1000
Ab,33.58,2.75,Cu,Dg,Em,Ev,4.25,Ft,Ft,06,Ff,Gg,00204,0
Aa,18.83,9.5,Cu,Dg,Ew,Ev,1.625,Ft,Ft,06,Ft,Gg,00040,600
Aa,26.92,13.5,Cu,Dg,Eq,Eh,5,Ft,Ft,02,Ff,Gg,00000,5000
Aa,31.25,3.75,Cu,Dg,Ecc,Eh,0.625,Ft,Ft,09,Ft,Gg,00181,0
Aa,56.50,16,Cu,Dg,Ej,Eff,0,Ft,Ft,15,Ff,Gg,00000,247
Ab,43.00,0.29,Cy,Dp,Ecc,Eh,1.75,Ft,Ft,08,Ff,Gg,00100,375
Ab,22.33,11,Cu,Dg,Ew,Ev,2,Ft,Ft,01,Ff,Gg,00080,278
Ab,27.25,1.665,Cu,Dg,Ecc,Eh,5.085,Ft,Ft,09,Ff,Gg,00399,827
Ab,32.83,2.5,Cu,Dg,Ecc,Eh,2.75,Ft,Ft,06,Ff,Gg,00160,2072
Ab,23.25,1.5,Cu,Dg,Eq,Ev,2.375,Ft,Ft,03,Ft,Gg,00000,582
Aa,40.33,7.54,Cy,Dp,Eq,Eh,8,Ft,Ft,14,Ff,Gg,00000,2300
Aa,30.50,6.5,Cu,Dg,Ec,Ebb,4,Ft,Ft,07,Ft,Gg,00000,3065
Aa,52.83,15,Cu,Dg,Ec,Ev,5.5,Ft,Ft,14,Ff,Gg,00000,2200
Aa,46.67,0.46,Cu,Dg,Ecc,Eh,0.415,Ft,Ft,11,Ft,Gg,00440,6
Aa,58.33,10,Cu,Dg,Eq,Ev,4,Ft,Ft,14,Ff,Gg,00000,1602
Ab,37.33,6.5,Cu,Dg,Em,Eh,4.25,Ft,Ft,12,Ft,Gg,00093,0
Ab,23.08,2.5,Cu,Dg,Ec,Ev,1.085,Ft,Ft,11,Ft,Gg,00060,2184
Ab,32.75,1.5,Cu,Dg,Ecc,Eh,5.5,Ft,Ft,03,Ft,Gg,00000,0
Aa,21.67,11.5,Cy,Dp,Ej,Ej,0,Ft,Ft,11,Ft,Gg,00000,0
Aa,28.50,3.04,Cy,Dp,Ex,Eh,2.54,Ft,Ft,01,Ff,Gg,00070,0
Aa,68.67,15,Cu,Dg,Ee,Ez,0,Ft,Ft,14,Ff,Gg,00000,3376
Ab,28.00,2,Cu,Dg,Ek,Eh,4.165,Ft,Ft,02,Ft,Gg,00181,0
Ab,34.08,0.08,Cy,Dp,Em,Ebb,0.04,Ft,Ft,01,Ft,Gg,00280,2000
Ab,27.67,2,Cu,Dg,Ex,Eh,1,Ft,Ft,04,Ff,Gg,00140,7544
Ab,44.00,2,Cu,Dg,Em,Ev,1.75,Ft,Ft,02,Ft,Gg,00000,15
Ab,25.08,1.71,Cu,Dg,Ex,Ev,1.665,Ft,Ft,01,Ft,Gg,00395,20
Ab,32.00,1.75,Cy,Dp,Ee,Eh,0.04,Ft,Ff,0,Ft,Gg,00393,0
Aa,60.58,16.5,Cu,Dg,Eq,Ev,11,Ft,Ff,0,Ft,Gg,00021,10561
Aa,40.83,10,Cu,Dg,Eq,Eh,1.75,Ft,Ff,0,Ff,Gg,00029,837
Ab,19.33,9.5,Cu,Dg,Eq,Ev,1,Ft,Ff,0,Ft,Gg,00060,400
Aa,32.33,0.54,Cu,Dg,Ecc,Ev,0.04,Ft,Ff,0,Ff,Gg,00440,11177
Ab,36.67,3.25,Cu,Dg,Eq,Eh,9,Ft,Ff,0,Ft,Gg,00102,639
Ab,37.50,1.125,Cy,Dp,Ed,Ev,1.5,Ff,Ff,0,Ft,Gg,00431,0
Aa,25.08,2.54,Cy,Dp,Eaa,Ev,0.25,Ft,Ff,0,Ft,Gg,00370,0
Ab,41.33,0,Cu,Dg,Ec,Ebb,15,Ft,Ff,0,Ff,Gg,00000,0
Ab,56.00,12.5,Cu,Dg,Ek,Eh,8,Ft,Ff,0,Ft,Gg,00024,2028
Aa,49.83,13.585,Cu,Dg,Ek,Eh,8.5,Ft,Ff,0,Ft,Gg,00000,0
Ab,22.67,10.5,Cu,Dg,Eq,Eh,1.335,Ft,Ff,0,Ff,Gg,00100,0
Ab,27.00,1.5,Cy,Dp,Ew,Ev,0.375,Ft,Ff,0,Ft,Gg,00260,1065
Ab,25.00,12.5,Cu,Dg,Eaa,Ev,3,Ft,Ff,0,Ft,Gs,00020,0
Aa,26.08,8.665,Cu,Dg,Eaa,Ev,1.415,Ft,Ff,0,Ff,Gg,00160,150
Aa,18.42,9.25,Cu,Dg,Eq,Ev,1.21,Ft,Ft,04,Ff,Gg,00060,540
Ab,20.17,8.17,Cu,Dg,Eaa,Ev,1.96,Ft,Ft,14,Ff,Gg,00060,158
Ab,47.67,0.29,Cu,Dg,Ec,Ebb,15,Ft,Ft,20,Ff,Gg,00000,15000
Aa,21.25,2.335,Cu,Dg,Ei,Ebb,0.5,Ft,Ft,04,Ff,Gs,00080,0
Aa,20.67,3,Cu,Dg,Eq,Ev,0.165,Ft,Ft,03,Ff,Gg,00100,6
Aa,57.08,19.5,Cu,Dg,Ec,Ev,5.5,Ft,Ft,07,Ff,Gg,00000,3000
Aa,22.42,5.665,Cu,Dg,Eq,Ev,2.585,Ft,Ft,07,Ff,Gg,00129,3257
Ab,48.75,8.5,Cu,Dg,Ec,Eh,12.5,Ft,Ft,09,Ff,Gg,00181,1655
Ab,40.00,6.5,Cu,Dg,Eaa,Ebb,3.5,Ft,Ft,01,Ff,Gg,00000,500
Ab,40.58,5,Cu,Dg,Ec,Ev,5,Ft,Ft,07,Ff,Gg,00000,3065
Aa,28.67,1.04,Cu,Dg,Ec,Ev,2.5,Ft,Ft,05,Ft,Gg,00300,1430
Aa,33.08,4.625,Cu,Dg,Eq,Eh,1.625,Ft,Ft,02,Ff,Gg,00000,0
Ab,21.33,10.5,Cu,Dg,Ec,Ev,3,Ft,Ff,0,Ft,Gg,00000,0
Ab,42.00,0.205,Cu,Dg,Ei,Eh,5.125,Ft,Ff,0,Ff,Gg,00400,0
Ab,41.75,0.96,Cu,Dg,Ex,Ev,2.5,Ft,Ff,0,Ff,Gg,00510,600
Ab,22.67,1.585,Cy,Dp,Ew,Ev,3.085,Ft,Ft,06,Ff,Gg,00080,0
Ab,34.50,4.04,Cy,Dp,Ei,Ebb,8.5,Ft,Ft,07,Ft,Gg,00195,0
Ab,28.25,5.04,Cy,Dp,Ec,Ebb,1.5,Ft,Ft,08,Ft,Gg,00144,7
Ab,33.17,3.165,Cy,Dp,Ex,Ev,3.165,Ft,Ft,03,Ft,Gg,00380,0
Ab,48.17,7.625,Cu,Dg,Ew,Eh,15.5,Ft,Ft,12,Ff,Gg,00000,790
Ab,27.58,2.04,Cy,Dp,Eaa,Ev,2,Ft,Ft,03,Ft,Gg,00370,560
Ab,22.58,10.04,Cu,Dg,Ex,Ev,0.04,Ft,Ft,09,Ff,Gg,00060,396
Aa,24.08,0.5,Cu,Dg,Eq,Eh,1.25,Ft,Ft,01,Ff,Gg,00000,678
Aa,41.33,1,Cu,Dg,Ei,Ebb,2.25,Ft,Ff,0,Ft,Gg,00000,300
Ab,24.83,2.75,Cu,Dg,Ec,Ev,2.25,Ft,Ft,06,Ff,Gg,?,600
Aa,20.75,10.25,Cu,Dg,Eq,Ev,0.71,Ft,Ft,02,Ft,Gg,00049,0
Ab,36.33,2.125,Cy,Dp,Ew,Ev,0.085,Ft,Ft,01,Ff,Gg,00050,1187
Aa,35.42,12,Cu,Dg,Eq,Eh,14,Ft,Ft,08,Ff,Gg,00000,6590
Aa,71.58,0,?,?,?,?,0,Ff,Ff,0,Ff,Gp,?,0
Ab,28.67,9.335,Cu,Dg,Eq,Eh,5.665,Ft,Ft,06,Ff,Gg,00381,168
Ab,35.17,2.5,Cu,Dg,Ek,Ev,4.5,Ft,Ft,07,Ff,Gg,00150,1270
Ab,39.50,4.25,Cu,Dg,Ec,Ebb,6.5,Ft,Ft,16,Ff,Gg,00117,1210
Ab,39.33,5.875,Cu,Dg,Ecc,Eh,10,Ft,Ft,14,Ft,Gg,00399,0
Ab,24.33,6.625,Cy,Dp,Ed,Ev,5.5,Ft,Ff,0,Ft,Gs,00100,0
Ab,60.08,14.5,Cu,Dg,Eff,Eff,18,Ft,Ft,15,Ft,Gg,00000,1000
Ab,23.08,11.5,Cu,Dg,Ei,Ev,3.5,Ft,Ft,09,Ff,Gg,00056,742
Ab,26.67,2.71,Cy,Dp,Ecc,Ev,5.25,Ft,Ft,01,Ff,Gg,00211,0
Ab,48.17,3.5,Cu,Dg,Eaa,Ev,3.5,Ft,Ff,0,Ff,Gs,00230,0
Ab,41.17,4.04,Cu,Dg,Ecc,Eh,7,Ft,Ft,08,Ff,Gg,00320,0
Ab,55.92,11.5,Cu,Dg,Eff,Eff,5,Ft,Ft,05,Ff,Gg,00000,8851
Ab,53.92,9.625,Cu,Dg,Ee,Ev,8.665,Ft,Ft,05,Ff,Gg,00000,0
Aa,18.92,9.25,Cy,Dp,Ec,Ev,1,Ft,Ft,04,Ft,Gg,00080,500
Aa,50.08,12.54,Cu,Dg,Eaa,Ev,2.29,Ft,Ft,03,Ft,Gg,00156,0
Ab,65.42,11,Cu,Dg,Ee,Ez,20,Ft,Ft,07,Ft,Gg,00022,0
Aa,17.58,9,Cu,Dg,Eaa,Ev,1.375,Ft,Ff,0,Ft,Gg,00000,0
Aa,18.83,9.54,Cu,Dg,Eaa,Ev,0.085,Ft,Ff,0,Ff,Gg,00100,0
Aa,37.75,5.5,Cu,Dg,Eq,Ev,0.125,Ft,Ff,0,Ft,Gg,00228,0
Ab,23.25,4,Cu,Dg,Ec,Ebb,0.25,Ft,Ff,0,Ft,Gg,00160,0
Ab,18.08,5.5,Cu,Dg,Ek,Ev,0.5,Ft,Ff,0,Ff,Gg,00080,0
Aa,22.50,8.46,Cy,Dp,Ex,Ev,2.46,Ff,Ff,0,Ff,Gg,00164,0
Ab,19.67,0.375,Cu,Dg,Eq,Ev,2,Ft,Ft,02,Ft,Gg,00080,0
Ab,22.08,11,Cu,Dg,Ecc,Ev,0.665,Ft,Ff,0,Ff,Gg,00100,0
Ab,25.17,3.5,Cu,Dg,Ecc,Ev,0.625,Ft,Ft,07,Ff,Gg,00000,7059
Aa,47.42,3,Cu,Dg,Ex,Ev,13.875,Ft,Ft,02,Ft,Gg,00519,1704
Ab,33.50,1.75,Cu,Dg,Ex,Eh,4.5,Ft,Ft,04,Ft,Gg,00253,857
Ab,27.67,13.75,Cu,Dg,Ew,Ev,5.75,Ft,Ff,0,Ft,Gg,00487,500
Aa,58.42,21,Cu,Dg,Ei,Ebb,10,Ft,Ft,13,Ff,Gg,00000,6700
Aa,20.67,1.835,Cu,Dg,Eq,Ev,2.085,Ft,Ft,05,Ff,Gg,00220,2503
Ab,26.17,0.25,Cu,Dg,Ei,Ebb,0,Ft,Ff,0,Ft,Gg,00000,0
Ab,21.33,7.5,Cu,Dg,Eaa,Ev,1.415,Ft,Ft,01,Ff,Gg,00080,9800
Ab,42.83,4.625,Cu,Dg,Eq,Ev,4.58,Ft,Ff,0,Ff,Gs,00000,0
Ab,38.17,10.125,Cu,Dg,Ex,Ev,2.5,Ft,Ft,06,Ff,Gg,00520,196
Ab,20.50,10,Cy,Dp,Ec,Ev,2.5,Ft,Ff,0,Ff,Gs,00040,0
Ab,48.25,25.085,Cu,Dg,Ew,Ev,1.75,Ft,Ft,03,Ff,Gg,00120,14
Ab,28.33,5,Cu,Dg,Ew,Ev,11,Ft,Ff,0,Ft,Gg,00070,0
Aa,18.75,7.5,Cu,Dg,Eq,Ev,2.71,Ft,Ft,05,Ff,Gg,?,26726
Ab,18.50,2,Cu,Dg,Ei,Ev,1.5,Ft,Ft,02,Ff,Gg,00120,300
Ab,33.17,3.04,Cy,Dp,Ec,Eh,2.04,Ft,Ft,01,Ft,Gg,00180,18027
Ab,45.00,8.5,Cu,Dg,Ecc,Eh,14,Ft,Ft,01,Ft,Gg,00088,2000
Aa,19.67,0.21,Cu,Dg,Eq,Eh,0.29,Ft,Ft,11,Ff,Gg,00080,99
?,24.50,12.75,Cu,Dg,Ec,Ebb,4.75,Ft,Ft,02,Ff,Gg,00073,444
Ab,21.83,11,Cu,Dg,Ex,Ev,0.29,Ft,Ft,06,Ff,Gg,00121,0
Ab,40.25,21.5,Cu,Dg,Ee,Ez,20,Ft,Ft,11,Ff,Gg,00000,1200
Ab,41.42,5,Cu,Dg,Eq,Eh,5,Ft,Ft,06,Ft,Gg,00470,0
Aa,17.83,11,Cu,Dg,Ex,Eh,1,Ft,Ft,11,Ff,Gg,00000,3000
Ab,23.17,11.125,Cu,Dg,Ex,Eh,0.46,Ft,Ft,01,Ff,Gg,00100,0
Ab,59.67,1.54,Cu,Dg,Eq,Ev,0.125,Ft,Ff,0,Ft,Gg,00260,0
Ab,18.00,0.165,Cu,Dg,Eq,En,0.21,Ff,Ff,0,Ff,Gg,00200,40
Ab,37.58,0,?,?,?,?,0,Ff,Ff,0,Ff,Gp,?,0
Ab,17.50,22,Cl,Dgg,Eff,Eo,0,Ff,Ff,0,Ft,Gp,00450,100000
Ab,19.17,0,Cy,Dp,Em,Ebb,0,Ff,Ff,0,Ft,Gs,00500,1
Ab,36.75,0.125,Cy,Dp,Ec,Ev,1.5,Ff,Ff,0,Ft,Gg,00232,113
Ab,21.25,1.5,Cu,Dg,Ew,Ev,1.5,Ff,Ff,0,Ff,Gg,00150,8
Aa,18.08,0.375,Cl,Dgg,Ecc,Eff,10,Ff,Ff,0,Ft,Gs,00300,0
Aa,33.67,0.375,Cu,Dg,Ecc,Ev,0.375,Ff,Ff,0,Ff,Gg,00300,44
Ab,48.58,0.205,Cy,Dp,Ek,Ev,0.25,Ft,Ft,11,Ff,Gg,00380,2732
;
Ab,32.33,7.5,Cu,Dg,Ee,Ebb,1.585,Ft,Ff,0,Ft,Gs,00420,0
Ab,34.83,4,Cu,Dg,Ed,Ebb,12.5,Ft,Ff,0,Ft,Gg,?,0
Aa,38.58,5,Cu,Dg,Ecc,Ev,13.5,Ft,Ff,0,Ft,Gg,00980,0
Ab,44.25,0.5,Cu,Dg,Em,Ev,10.75,Ft,Ff,0,Ff,Gs,00400,0
Ab,44.83,7,Cy,Dp,Ec,Ev,1.625,Ff,Ff,0,Ff,Gg,00160,2
Ab,20.67,5.29,Cu,Dg,Eq,Ev,0.375,Ft,Ft,01,Ff,Gg,00160,0
Ab,34.08,6.5,Cu,Dg,Eaa,Ev,0.125,Ft,Ff,0,Ft,Gg,00443,0
Aa,19.17,0.585,Cy,Dp,Eaa,Ev,0.585,Ft,Ff,0,Ft,Gg,00160,0
Ab,21.67,1.165,Cy,Dp,Ek,Ev,2.5,Ft,Ft,01,Ff,Gg,00180,20
Ab,21.50,9.75,Cu,Dg,Ec,Ev,0.25,Ft,Ff,0,Ff,Gg,00140,0
Ab,49.58,19,Cu,Dg,Eff,Eff,0,Ft,Ft,01,Ff,Gg,00094,0
Aa,27.67,1.5,Cu,Dg,Em,Ev,2,Ft,Ff,0,Ff,Gs,00368,0
Ab,39.83,0.5,Cu,Dg,Em,Ev,0.25,Ft,Ff,0,Ff,Gs,00288,0
Aa,?,3.5,Cu,Dg,Ed,Ev,3,Ft,Ff,0,Ft,Gg,00300,0
Ab,27.25,0.625,Cu,Dg,Eaa,Ev,0.455,Ft,Ff,0,Ft,Gg,00200,0
Ab,37.17,4,Cu,Dg,Ec,Ebb,5,Ft,Ff,0,Ft,Gs,00280,0
Ab,?,0.375,Cu,Dg,Ed,Ev,0.875,Ft,Ff,0,Ft,Gs,00928,0
Ab,25.67,2.21,Cy,Dp,Eaa,Ev,4,Ft,Ff,0,Ff,Gg,00188,0
Ab,34.00,4.5,Cu,Dg,Eaa,Ev,1,Ft,Ff,0,Ft,Gg,00240,0
Aa,49.00,1.5,Cu,Dg,Ej,Ej,0,Ft,Ff,0,Ft,Gg,00100,27
Ab,62.50,12.75,Cy,Dp,Ec,Eh,5,Ft,Ff,0,Ff,Gg,00112,0
Ab,31.42,15.5,Cu,Dg,Ec,Ev,0.5,Ft,Ff,0,Ff,Gg,00120,0
Ab,?,5,Cy,Dp,Eaa,Ev,8.5,Ft,Ff,0,Ff,Gg,00000,0
Ab,52.33,1.375,Cy,Dp,Ec,Eh,9.46,Ft,Ff,0,Ft,Gg,00200,100
Ab,28.75,1.5,Cy,Dp,Ec,Ev,1.5,Ft,Ff,0,Ft,Gg,00000,225
Aa,28.58,3.54,Cu,Dg,Ei,Ebb,0.5,Ft,Ff,0,Ft,Gg,00171,0
Ab,23.00,0.625,Cy,Dp,Eaa,Ev,0.125,Ft,Ff,0,Ff,Gg,00180,1
Ab,?,0.5,Cu,Dg,Ec,Ebb,0.835,Ft,Ff,0,Ft,Gs,00320,0
Aa,22.50,11,Cy,Dp,Eq,Ev,3,Ft,Ff,0,Ft,Gg,00268,0
Aa,28.50,1,Cu,Dg,Eq,Ev,1,Ft,Ft,02,Ft,Gg,00167,500
Ab,37.50,1.75,Cy,Dp,Ec,Ebb,0.25,Ft,Ff,0,Ft,Gg,00164,400
Ab,35.25,16.5,Cy,Dp,Ec,Ev,4,Ft,Ff,0,Ff,Gg,00080,0
Ab,18.67,5,Cu,Dg,Eq,Ev,0.375,Ft,Ft,02,Ff,Gg,00000,38
Ab,25.00,12,Cu,Dg,Ek,Ev,2.25,Ft,Ft,02,Ft,Gg,00120,5
Ab,27.83,4,Cy,Dp,Ei,Eh,5.75,Ft,Ft,02,Ft,Gg,00075,0
Ab,54.83,15.5,Cu,Dg,Ee,Ez,0,Ft,Ft,20,Ff,Gg,00152,130
Ab,28.75,1.165,Cu,Dg,Ek,Ev,0.5,Ft,Ff,0,Ff,Gs,00280,0
Aa,25.00,11,Cy,Dp,Eaa,Ev,4.5,Ft,Ff,0,Ff,Gg,00120,0
Ab,40.92,2.25,Cy,Dp,Ex,Eh,10,Ft,Ff,0,Ft,Gg,00176,0
Aa,19.75,0.75,Cu,Dg,Ec,Ev,0.795,Ft,Ft,05,Ft,Gg,00140,5
Ab,29.17,3.5,Cu,Dg,Ew,Ev,3.5,Ft,Ft,03,Ft,Gg,00329,0
Aa,24.50,1.04,Cy,Dp,Eff,Eff,0.5,Ft,Ft,03,Ff,Gg,00180,147
Ab,24.58,12.5,Cu,Dg,Ew,Ev,0.875,Ft,Ff,0,Ft,Gg,00260,0
Aa,33.75,0.75,Cu,Dg,Ek,Ebb,1,Ft,Ft,03,Ft,Gg,00212,0
Ab,20.67,1.25,Cy,Dp,Ec,Eh,1.375,Ft,Ft,03,Ft,Gg,00140,210
Aa,25.42,1.125,Cu,Dg,Eq,Ev,1.29,Ft,Ft,02,Ff,Gg,00200,0
Ab,37.75,7,Cu,Dg,Eq,Eh,11.5,Ft,Ft,07,Ft,Gg,00300,5
Ab,?,0.625,Cu,Dg,Ek,Ev,0.25,Ff,Ff,0,Ff,Gg,00380,2010
Ab,18.17,10.25,Cu,Dg,Ec,Eh,1.085,Ff,Ff,0,Ff,Gg,00320,13
Ab,20.00,11.045,Cu,Dg,Ec,Ev,2,Ff,Ff,0,Ft,Gg,00136,0
Ab,20.00,0,Cu,Dg,Ed,Ev,0.5,Ff,Ff,0,Ff,Gg,00144,0
Aa,20.75,9.54,Cu,Dg,Ei,Ev,0.04,Ff,Ff,0,Ff,Gg,00200,1000
Aa,24.50,1.75,Cy,Dp,Ec,Ev,0.165,Ff,Ff,0,Ff,Gg,00132,0
Ab,32.75,2.335,Cu,Dg,Ed,Eh,5.75,Ff,Ff,0,Ft,Gg,00292,0
Aa,52.17,0,Cy,Dp,Eff,Eff,0,Ff,Ff,0,Ff,Gg,00000,0
Aa,48.17,1.335,Cu,Dg,Ei,Eo,0.335,Ff,Ff,0,Ff,Gg,00000,120
Aa,20.42,10.5,Cy,Dp,Ex,Eh,0,Ff,Ff,0,Ft,Gg,00154,32
Ab,50.75,0.585,Cu,Dg,Eff,Eff,0,Ff,Ff,0,Ff,Gg,00145,0
Ab,17.08,0.085,Cy,Dp,Ec,Ev,0.04,Ff,Ff,0,Ff,Gg,00140,722
Ab,18.33,1.21,Cy,Dp,Ee,Edd,0,Ff,Ff,0,Ff,Gg,00100,0
Aa,32.00,6,Cu,Dg,Ed,Ev,1.25,Ff,Ff,0,Ff,Gg,00272,0
Ab,32.33,2.5,Cu,Dg,Ec,Ev,1.25,Ff,Ff,0,Ft,Gg,00280,0
Ab,18.08,6.75,Cy,Dp,Em,Ev,0.04,Ff,Ff,0,Ff,Gg,00140,0
Ab,38.25,10.125,Cy,Dp,Ek,Ev,0.125,Ff,Ff,0,Ff,Gg,00160,0
Ab,30.67,2.5,Cu,Dg,Ecc,Eh,2.25,Ff,Ff,0,Ft,Gs,00340,0
Ab,18.58,5.71,Cu,Dg,Ed,Ev,0.54,Ff,Ff,0,Ff,Gg,00120,0
Aa,19.17,5.415,Cu,Dg,Ei,Eh,0.29,Ff,Ff,0,Ff,Gg,00080,484
Aa,18.17,10,Cy,Dp,Eq,Eh,0.165,Ff,Ff,0,Ff,Gg,00340,0
Ab,24.58,13.5,Cy,Dp,Eff,Eff,0,Ff,Ff,0,Ff,Gg,?,0
Ab,16.25,0.835,Cu,Dg,Em,Ev,0.085,Ft,Ff,0,Ff,Gs,00200,0
Ab,21.17,0.875,Cy,Dp,Ec,Eh,0.25,Ff,Ff,0,Ff,Gg,00280,204
Ab,23.92,0.585,Cy,Dp,Ecc,Eh,0.125,Ff,Ff,0,Ff,Gg,00240,1
Ab,17.67,4.46,Cu,Dg,Ec,Ev,0.25,Ff,Ff,0,Ff,Gs,00080,0
Aa,16.50,1.25,Cu,Dg,Eq,Ev,0.25,Ff,Ft,01,Ff,Gg,00108,98
Ab,23.25,12.625,Cu,Dg,Ec,Ev,0.125,Ff,Ft,02,Ff,Gg,00000,5552
Ab,17.58,10,Cu,Dg,Ew,Eh,0.165,Ff,Ft,01,Ff,Gg,00120,1
Aa,?,1.5,Cu,Dg,Eff,Eff,0,Ff,Ft,02,Ft,Gg,00200,105
Ab,29.50,0.58,Cu,Dg,Ew,Ev,0.29,Ff,Ft,01,Ff,Gg,00340,2803
Ab,18.83,0.415,Cy,Dp,Ec,Ev,0.165,Ff,Ft,01,Ff,Gg,00200,1
Aa,21.75,1.75,Cy,Dp,Ej,Ej,0,Ff,Ff,0,Ff,Gg,00160,0
Ab,23.00,0.75,Cu,Dg,Em,Ev,0.5,Ff,Ff,0,Ft,Gs,00320,0
Aa,18.25,10,Cu,Dg,Ew,Ev,1,Ff,Ft,01,Ff,Gg,00120,1
Ab,25.42,0.54,Cu,Dg,Ew,Ev,0.165,Ff,Ft,01,Ff,Gg,00272,444
Ab,35.75,2.415,Cu,Dg,Ew,Ev,0.125,Ff,Ft,02,Ff,Gg,00220,1
Aa,16.08,0.335,Cu,Dg,Eff,Eff,0,Ff,Ft,01,Ff,Gg,00160,126
Aa,31.92,3.125,Cu,Dg,Eff,Eff,3.04,Ff,Ft,02,Ft,Gg,00200,4
Ab,69.17,9,Cu,Dg,Eff,Eff,4,Ff,Ft,01,Ff,Gg,00070,6
Ab,32.92,2.5,Cu,Dg,Eaa,Ev,1.75,Ff,Ft,02,Ft,Gg,00720,0
Ab,16.33,2.75,Cu,Dg,Eaa,Ev,0.665,Ff,Ft,01,Ff,Gg,00080,21
Ab,22.17,12.125,Cu,Dg,Ec,Ev,3.335,Ff,Ft,02,Ft,Gg,00180,173
Aa,57.58,2,Cu,Dg,Eff,Eff,6.5,Ff,Ft,01,Ff,Gg,00000,10
Ab,18.25,0.165,Cu,Dg,Ed,Ev,0.25,Ff,Ff,0,Ft,Gs,00280,0
Ab,23.42,1,Cu,Dg,Ec,Ev,0.5,Ff,Ff,0,Ft,Gs,00280,0
Aa,15.92,2.875,Cu,Dg,Eq,Ev,0.085,Ff,Ff,0,Ff,Gg,00120,0
Aa,24.75,13.665,Cu,Dg,Eq,Eh,1.5,Ff,Ff,0,Ff,Gg,00280,1
Ab,48.75,26.335,Cy,Dp,Eff,Eff,0,Ft,Ff,0,Ft,Gg,00000,0
Ab,23.50,2.75,Cu,Dg,Eff,Eff,4.5,Ff,Ff,0,Ff,Gg,00160,25
Ab,18.58,10.29,Cu,Dg,Eff,Eff,0.415,Ff,Ff,0,Ff,Gg,00080,0
Ab,27.75,1.29,Cu,Dg,Ek,Eh,0.25,Ff,Ff,0,Ft,Gs,00140,0
Aa,31.75,3,Cy,Dp,Ej,Ej,0,Ff,Ff,0,Ff,Gg,00160,20
Aa,24.83,4.5,Cu,Dg,Ew,Ev,1,Ff,Ff,0,Ft,Gg,00360,6
Ab,19.00,1.75,Cy,Dp,Ec,Ev,2.335,Ff,Ff,0,Ft,Gg,00112,6
Aa,16.33,0.21,Cu,Dg,Eaa,Ev,0.125,Ff,Ff,0,Ff,Gg,00200,1
Aa,18.58,10,Cu,Dg,Ed,Ev,0.415,Ff,Ff,0,Ff,Gg,00080,42
Ab,16.25,0,Cy,Dp,Eaa,Ev,0.25,Ff,Ff,0,Ff,Gg,00060,0
Ab,23.00,0.75,Cu,Dg,Em,Ev,0.5,Ft,Ff,0,Ft,Gs,00320,0
Ab,21.17,0.25,Cy,Dp,Ec,Eh,0.25,Ff,Ff,0,Ff,Gg,00280,204
Ab,33.67,1.25,Cu,Dg,Ew,Ev,1.165,Ff,Ff,0,Ff,Gg,00120,0
Aa,29.50,1.085,Cy,Dp,Ex,Ev,1,Ff,Ff,0,Ff,Gg,00280,13
Ab,30.17,1.085,Cy,Dp,Ec,Ev,0.04,Ff,Ff,0,Ff,Gg,00170,179
?,40.83,3.5,Cu,Dg,Ei,Ebb,0.5,Ff,Ff,0,Ff,Gs,01160,0
Ab,34.83,2.5,Cy,Dp,Ew,Ev,3,Ff,Ff,0,Ff,Gs,00200,0
Ab,?,4,Cy,Dp,Ei,Ev,0.085,Ff,Ff,0,Ft,Gg,00411,0
Ab,20.42,0,?,?,?,?,0,Ff,Ff,0,Ff,Gp,?,0
Aa,33.25,2.5,Cy,Dp,Ec,Ev,2.5,Ff,Ff,0,Ft,Gg,00000,2
Ab,34.08,2.5,Cu,Dg,Ec,Ev,1,Ff,Ff,0,Ff,Gg,00460,16
Aa,25.25,12.5,Cu,Dg,Ed,Ev,1,Ff,Ff,0,Ft,Gg,00180,1062
Ab,34.75,2.5,Cu,Dg,Ecc,Ebb,0.5,Ff,Ff,0,Ff,Gg,00348,0
Ab,27.67,0.75,Cu,Dg,Eq,Eh,0.165,Ff,Ff,0,Ft,Gg,00220,251
Ab,47.33,6.5,Cu,Dg,Ec,Ev,1,Ff,Ff,0,Ft,Gg,00000,228
Aa,34.83,1.25,Cy,Dp,Ei,Eh,0.5,Ff,Ff,0,Ft,Gg,00160,0
Aa,33.25,3,Cy,Dp,Eaa,Ev,2,Ff,Ff,0,Ff,Gg,00180,0
Ab,28.00,3,Cu,Dg,Ew,Ev,0.75,Ff,Ff,0,Ft,Gg,00300,67
Aa,39.08,4,Cu,Dg,Ec,Ev,3,Ff,Ff,0,Ff,Gg,00480,0
Ab,42.75,4.085,Cu,Dg,Eaa,Ev,0.04,Ff,Ff,0,Ff,Gg,00108,100
Ab,26.92,2.25,Cu,Dg,Ei,Ebb,0.5,Ff,Ff,0,Ft,Gg,00640,4000
Ab,33.75,2.75,Cu,Dg,Ei,Ebb,0,Ff,Ff,0,Ff,Gg,00180,0
Ab,38.92,1.75,Cu,Dg,Ek,Ev,0.5,Ff,Ff,0,Ft,Gg,00300,2
Ab,62.75,7,Cu,Dg,Ee,Ez,0,Ff,Ff,0,Ff,Gg,00000,12
?,32.25,1.5,Cu,Dg,Ec,Ev,0.25,Ff,Ff,0,Ft,Gg,00372,122
Ab,26.75,4.5,Cy,Dp,Ec,Ebb,2.5,Ff,Ff,0,Ff,Gg,00200,1210
Ab,63.33,0.54,Cu,Dg,Ec,Ev,0.585,Ft,Ft,03,Ft,Gg,00180,0
Ab,27.83,1.5,Cu,Dg,Ew,Ev,2.25,Ff,Ft,01,Ft,Gg,00100,3
Aa,26.17,2,Cu,Dg,Ej,Ej,0,Ff,Ff,0,Ft,Gg,00276,1
Ab,22.17,0.585,Cy,Dp,Eff,Eff,0,Ff,Ff,0,Ff,Gg,00100,0
Ab,22.50,11.5,Cy,Dp,Em,Ev,1.5,Ff,Ff,0,Ft,Gg,00000,4000
Ab,30.75,1.585,Cu,Dg,Ed,Ev,0.585,Ff,Ff,0,Ft,Gs,00000,0
Ab,36.67,2,Cu,Dg,Ei,Ev,0.25,Ff,Ff,0,Ft,Gg,00221,0
Aa,16.00,0.165,Cu,Dg,Eaa,Ev,1,Ff,Ft,02,Ft,Gg,00320,1
Ab,41.17,1.335,Cu,Dg,Ed,Ev,0.165,Ff,Ff,0,Ff,Gg,00168,0
Aa,19.50,0.165,Cu,Dg,Eq,Ev,0.04,Ff,Ff,0,Ft,Gg,00380,0
Ab,32.42,3,Cu,Dg,Ed,Ev,0.165,Ff,Ff,0,Ft,Gg,00120,0
Aa,36.75,4.71,Cu,Dg,Eff,Eff,0,Ff,Ff,0,Ff,Gg,00160,0
Aa,30.25,5.5,Cu,Dg,Ek,Ev,5.5,Ff,Ff,0,Ft,Gs,00100,0
Ab,23.08,2.5,Cu,Dg,Eff,Eff,0.085,Ff,Ff,0,Ft,Gg,00100,4208
Ab,26.83,0.54,Cu,Dg,Ek,Eff,0,Ff,Ff,0,Ff,Gg,00100,0
Ab,16.92,0.335,Cy,Dp,Ek,Ev,0.29,Ff,Ff,0,Ff,Gs,00200,0
Ab,24.42,2,Cu,Dg,Ee,Edd,0.165,Ff,Ft,02,Ff,Gg,00320,1300
Ab,42.83,1.25,Cu,Dg,Em,Ev,13.875,Ff,Ft,01,Ft,Gg,00352,112
Aa,22.75,6.165,Cu,Dg,Eaa,Ev,0.165,Ff,Ff,0,Ff,Gg,00220,1000
Ab,39.42,1.71,Cy,Dp,Em,Ev,0.165,Ff,Ff,0,Ff,Gs,00400,0
Aa,23.58,11.5,Cy,Dp,Ek,Eh,3,Ff,Ff,0,Ft,Gg,00020,16
Ab,21.42,0.75,Cy,Dp,Er,En,0.75,Ff,Ff,0,Ft,Gg,00132,2
Ab,33.00,2.5,Cy,Dp,Ew,Ev,7,Ff,Ff,0,Ft,Gg,00280,0
Ab,26.33,13,Cu,Dg,Ee,Edd,0,Ff,Ff,0,Ft,Gg,00140,1110
Aa,45.00,4.585,Cu,Dg,Ek,Eh,1,Ff,Ff,0,Ft,Gs,00240,0
Ab,26.25,1.54,Cu,Dg,Ew,Ev,0.125,Ff,Ff,0,Ff,Gg,00100,0
?,28.17,0.585,Cu,Dg,Eaa,Ev,0.04,Ff,Ff,0,Ff,Gg,00260,1004
Aa,20.83,0.5,Cy,Dp,Ee,Edd,1,Ff,Ff,0,Ff,Gg,00260,0
Ab,28.67,14.5,Cu,Dg,Ed,Ev,0.125,Ff,Ff,0,Ff,Gg,00000,286
Ab,20.67,0.835,Cy,Dp,Ec,Ev,2,Ff,Ff,0,Ft,Gs,00240,0
Ab,34.42,1.335,Cu,Dg,Ei,Ebb,0.125,Ff,Ff,0,Ft,Gg,00440,4500
Ab,33.58,0.25,Cu,Dg,Ei,Ebb,4,Ff,Ff,0,Ft,Gs,00420,0
Ab,43.17,5,Cu,Dg,Ei,Ebb,2.25,Ff,Ff,0,Ft,Gg,00141,0
Aa,22.67,7,Cu,Dg,Ec,Ev,0.165,Ff,Ff,0,Ff,Gg,00160,0
Aa,24.33,2.5,Cy,Dp,Ei,Ebb,4.5,Ff,Ff,0,Ff,Gg,00200,456
Aa,56.83,4.25,Cy,Dp,Eff,Eff,5,Ff,Ff,0,Ft,Gg,00000,4
Ab,22.08,11.46,Cu,Dg,Ek,Ev,1.585,Ff,Ff,0,Ft,Gg,00100,1212
Ab,34.00,5.5,Cy,Dp,Ec,Ev,1.5,Ff,Ff,0,Ft,Gg,00060,0
Ab,22.58,1.5,Cy,Dp,Eaa,Ev,0.54,Ff,Ff,0,Ft,Gg,00120,67
Ab,21.17,0,Cu,Dg,Ec,Ev,0.5,Ff,Ff,0,Ft,Gs,00000,0
Ab,26.67,14.585,Cu,Dg,Ei,Ebb,0,Ff,Ff,0,Ft,Gg,00178,0
Ab,22.92,0.17,Cu,Dg,Em,Ev,0.085,Ff,Ff,0,Ff,Gs,00000,0
Ab,15.17,7,Cu,Dg,Ee,Ev,1,Ff,Ff,0,Ff,Gg,00600,0
Ab,39.92,5,Cu,Dg,Ei,Ebb,0.21,Ff,Ff,0,Ff,Gg,00550,0
Ab,27.42,12.5,Cu,Dg,Eaa,Ebb,0.25,Ff,Ff,0,Ft,Gg,00720,0
Ab,24.75,0.54,Cu,Dg,Em,Ev,1,Ff,Ff,0,Ft,Gg,00120,1
Ab,41.17,1.25,Cy,Dp,Ew,Ev,0.25,Ff,Ff,0,Ff,Gg,00000,195
Aa,33.08,1.625,Cu,Dg,Ed,Ev,0.54,Ff,Ff,0,Ft,Gg,00000,0
Ab,29.83,2.04,Cy,Dp,Ex,Eh,0.04,Ff,Ff,0,Ff,Gg,00128,1
Aa,23.58,0.585,Cy,Dp,Eff,Eff,0.125,Ff,Ff,0,Ff,Gg,00120,87
Ab,26.17,12.5,Cy,Dp,Ek,Eh,1.25,Ff,Ff,0,Ft,Gg,00000,17
Ab,31.00,2.085,Cu,Dg,Ec,Ev,0.085,Ff,Ff,0,Ff,Gg,00300,0
Ab,20.75,5.085,Cy,Dp,Ej,Ev,0.29,Ff,Ff,0,Ff,Gg,00140,184
Ab,28.92,0.375,Cu,Dg,Ec,Ev,0.29,Ff,Ff,0,Ff,Gg,00220,140
Aa,51.92,6.5,Cu,Dg,Ei,Ebb,3.085,Ff,Ff,0,Ft,Gg,00073,0
Aa,22.67,0.335,Cu,Dg,Eq,Ev,0.75,Ff,Ff,0,Ff,Gs,00160,0
Ab,34.00,5.085,Cy,Dp,Ei,Ebb,1.085,Ff,Ff,0,Ft,Gg,00480,0
Aa,69.50,6,Cu,Dg,Eff,Eff,0,Ff,Ff,0,Ff,Gs,00000,0
Aa,40.33,8.125,Cy,Dp,Ek,Ev,0.165,Ff,Ft,02,Ff,Gg,?,18
Aa,19.58,0.665,Cy,Dp,Ec,Ev,1,Ff,Ft,01,Ff,Gg,02000,2
Ab,16.00,3.125,Cu,Dg,Ew,Ev,0.085,Ff,Ft,01,Ff,Gg,00000,6
Ab,17.08,0.25,Cu,Dg,Eq,Ev,0.335,Ff,Ft,04,Ff,Gg,00160,8
Ab,31.25,2.835,Cu,Dg,Eff,Eff,0,Ff,Ft,05,Ff,Gg,00176,146
Ab,25.17,3,Cu,Dg,Ec,Ev,1.25,Ff,Ft,01,Ff,Gg,00000,22
Aa,22.67,0.79,Cu,Dg,Ei,Ev,0.085,Ff,Ff,0,Ff,Gg,00144,0
Ab,40.58,1.5,Cu,Dg,Ei,Ebb,0,Ff,Ff,0,Ff,Gs,00300,0
Ab,22.25,0.46,Cu,Dg,Ek,Ev,0.125,Ff,Ff,0,Ft,Gg,00280,55
Aa,22.25,1.25,Cy,Dp,Eff,Eff,3.25,Ff,Ff,0,Ff,Gg,00280,0
Ab,22.50,0.125,Cy,Dp,Ek,Ev,0.125,Ff,Ff,0,Ff,Gg,00200,70
Ab,23.58,1.79,Cu,Dg,Ec,Ev,0.54,Ff,Ff,0,Ft,Gg,00136,1
Ab,38.42,0.705,Cu,Dg,Ec,Ev,0.375,Ff,Ft,02,Ff,Gg,00225,500
Aa,26.58,2.54,Cy,Dp,Eff,Eff,0,Ff,Ff,0,Ft,Gg,00180,60
Ab,35.00,2.5,Cu,Dg,Ei,Ev,1,Ff,Ff,0,Ft,Gg,00210,0
Ab,20.42,1.085,Cu,Dg,Eq,Ev,1.5,Ff,Ff,0,Ff,Gg,00108,7
Ab,29.42,1.25,Cu,Dg,Ew,Ev,1.75,Ff,Ff,0,Ff,Gg,00200,0
Ab,26.17,0.835,Cu,Dg,Ecc,Ev,1.165,Ff,Ff,0,Ff,Gg,00100,0
Ab,33.67,2.165,Cu,Dg,Ec,Ev,1.5,Ff,Ff,0,Ff,Gp,00120,0
Ab,24.58,1.25,Cu,Dg,Ec,Ev,0.25,Ff,Ff,0,Ff,Gg,00110,0
Aa,27.67,2.04,Cu,Dg,Ew,Ev,0.25,Ff,Ff,0,Ft,Gg,00180,50
Ab,37.50,0.835,Cu,Dg,Ee,Ev,0.04,Ff,Ff,0,Ff,Gg,00120,5
Ab,49.17,2.29,Cu,Dg,Eff,Eff,0.29,Ff,Ff,0,Ff,Gg,00200,3
Ab,33.58,0.335,Cy,Dp,Ecc,Ev,0.085,Ff,Ff,0,Ff,Gg,00180,0
Ab,51.83,3,Cy,Dp,Eff,Eff,1.5,Ff,Ff,0,Ff,Gg,00180,4
Ab,22.92,3.165,Cy,Dp,Ec,Ev,0.165,Ff,Ff,0,Ff,Gg,00160,1058
Ab,21.83,1.54,Cu,Dg,Ek,Ev,0.085,Ff,Ff,0,Ft,Gg,00356,0
Ab,25.25,1,Cu,Dg,Eaa,Ev,0.5,Ff,Ff,0,Ff,Gg,00200,0
Ab,58.58,2.71,Cu,Dg,Ec,Ev,2.415,Ff,Ff,0,Ft,Gg,00320,0
Ab,19.00,0,Cy,Dp,Eff,Eff,0,Ff,Ft,04,Ff,Gg,00045,1
Ab,19.58,0.585,Cu,Dg,Eff,Eff,0,Ff,Ft,03,Ff,Gg,00350,769
Aa,53.33,0.165,Cu,Dg,Eff,Eff,0,Ff,Ff,0,Ft,Gs,00062,27
Aa,27.17,1.25,Cu,Dg,Eff,Eff,0,Ff,Ft,01,Ff,Gg,00092,300
Ab,25.92,0.875,Cu,Dg,Ek,Ev,0.375,Ff,Ft,02,Ft,Gg,00174,3
Ab,23.08,0,Cu,Dg,Ek,Ev,1,Ff,Ft,11,Ff,Gs,00000,0
Ab,39.58,5,Cu,Dg,Eff,Eff,0,Ff,Ft,02,Ff,Gg,00017,1
Ab,30.58,2.71,Cy,Dp,Em,Ev,0.125,Ff,Ff,0,Ft,Gs,00080,0
Ab,17.25,3,Cu,Dg,Ek,Ev,0.04,Ff,Ff,0,Ft,Gg,00160,40
Aa,17.67,0,Cy,Dp,Ej,Eff,0,Ff,Ff,0,Ff,Gg,00086,0
Aa,?,11.25,Cu,Dg,Eff,Eff,0,Ff,Ff,0,Ff,Gg,?,5200
Ab,16.50,0.125,Cu,Dg,Ec,Ev,0.165,Ff,Ff,0,Ff,Gg,00132,0
Aa,27.33,1.665,Cu,Dg,Eff,Eff,0,Ff,Ff,0,Ff,Gg,00340,1
Ab,31.25,1.125,Cu,Dg,Eff,Eff,0,Ff,Ft,01,Ff,Gg,00096,19
Ab,20.00,7,Cu,Dg,Ec,Ev,0.5,Ff,Ff,0,Ff,Gg,00000,0
Ab,?,3,Cy,Dp,Ei,Ebb,7,Ff,Ff,0,Ff,Gg,00000,1
Ab,39.50,1.625,Cu,Dg,Ec,Ev,1.5,Ff,Ff,0,Ff,Gg,00000,316
Ab,36.50,4.25,Cu,Dg,Eq,Ev,3.5,Ff,Ff,0,Ff,Gg,00454,50
?,29.75,0.665,Cu,Dg,Ew,Ev,0.25,Ff,Ff,0,Ft,Gg,00300,0
Ab,52.42,1.5,Cu,Dg,Ed,Ev,3.75,Ff,Ff,0,Ft,Gg,00000,350
Ab,36.17,18.125,Cu,Dg,Ew,Ev,0.085,Ff,Ff,0,Ff,Gg,00320,3552
Ab,34.58,0,?,?,?,?,0,Ff,Ff,0,Ff,Gp,?,0
Ab,29.67,0.75,Cy,Dp,Ec,Ev,0.04,Ff,Ff,0,Ff,Gg,00240,0
Ab,36.17,5.5,Cu,Dg,Ei,Ebb,5,Ff,Ff,0,Ff,Gg,00210,687
Ab,25.67,0.29,Cy,Dp,Ec,Ev,1.5,Ff,Ff,0,Ft,Gg,00160,0
Aa,24.50,2.415,Cy,Dp,Ec,Ev,0,Ff,Ff,0,Ff,Gg,00120,0
Ab,24.08,0.875,Cu,Dg,Em,Ev,0.085,Ff,Ft,04,Ff,Gg,00254,1950
Ab,21.92,0.5,Cu,Dg,Ec,Ev,0.125,Ff,Ff,0,Ff,Gg,00360,0
Aa,36.58,0.29,Cu,Dg,Eff,Eff,0,Ff,Ft,10,Ff,Gg,00200,18
Aa,23.00,1.835,Cu,Dg,Ej,Ej,0,Ff,Ft,01,Ff,Gg,00200,53
Aa,27.58,3,Cu,Dg,Em,Ev,2.79,Ff,Ft,01,Ft,Gg,00280,10
Ab,31.08,3.085,Cu,Dg,Ec,Ev,2.5,Ff,Ft,02,Ft,Gg,00160,41
Aa,30.42,1.375,Cu,Dg,Ew,Eh,0.04,Ff,Ft,03,Ff,Gg,00000,33
Ab,22.08,2.335,Cu,Dg,Ek,Ev,0.75,Ff,Ff,0,Ff,Gg,00180,0
Ab,16.33,4.085,Cu,Dg,Ei,Eh,0.415,Ff,Ff,0,Ft,Gg,00120,0
Aa,21.92,11.665,Cu,Dg,Ek,Eh,0.085,Ff,Ff,0,Ff,Gg,00320,5
Ab,21.08,4.125,Cy,Dp,Ei,Eh,0.04,Ff,Ff,0,Ff,Gg,00140,100
Ab,17.42,6.5,Cu,Dg,Ei,Ev,0.125,Ff,Ff,0,Ff,Gg,00060,100
Ab,19.17,4,Cy,Dp,Ei,Ev,1,Ff,Ff,0,Ft,Gg,00360,1000
Ab,20.67,0.415,Cu,Dg,Ec,Ev,0.125,Ff,Ff,0,Ff,Gg,00000,44
Ab,26.75,2,Cu,Dg,Ed,Ev,0.75,Ff,Ff,0,Ft,Gg,00080,0
Ab,23.58,0.835,Cu,Dg,Ei,Eh,0.085,Ff,Ff,0,Ft,Gg,00220,5
Ab,39.17,2.5,Cy,Dp,Ei,Eh,10,Ff,Ff,0,Ft,Gs,00200,0
Ab,22.75,11.5,Cu,Dg,Ei,Ev,0.415,Ff,Ff,0,Ff,Gg,00000,0
?,26.50,2.71,Cy,Dp,?,?,0.085,Ff,Ff,0,Ff,Gs,00080,0
Aa,16.92,0.5,Cu,Dg,Ei,Ev,0.165,Ff,Ft,06,Ft,Gg,00240,35
Ab,23.50,3.165,Cy,Dp,Ek,Ev,0.415,Ff,Ft,01,Ft,Gg,00280,80
Aa,17.33,9.5,Cu,Dg,Eaa,Ev,1.75,Ff,Ft,10,Ft,Gg,00000,10
Ab,23.75,0.415,Cy,Dp,Ec,Ev,0.04,Ff,Ft,02,Ff,Gg,00128,6
Ab,34.67,1.08,Cu,Dg,Em,Ev,1.165,Ff,Ff,0,Ff,Gs,00028,0
Ab,74.83,19,Cy,Dp,Eff,Eff,0.04,Ff,Ft,02,Ff,Gg,00000,351
Ab,28.17,0.125,Cy,Dp,Ek,Ev,0.085,Ff,Ff,0,Ff,Gg,00216,2100
Ab,24.50,13.335,Cy,Dp,Eaa,Ev,0.04,Ff,Ff,0,Ft,Gg,00120,475
Ab,18.83,3.54,Cy,Dp,Eff,Eff,0,Ff,Ff,0,Ft,Gg,00180,1
?,45.33,1,Cu,Dg,Eq,Ev,0.125,Ff,Ff,0,Ft,Gg,00263,0
@.

is_+
Aa,47.25,0.75,Cu,Dg,Eq,Eh,2.75,Ft,Ft,01,Ff,Gg,00333,892:+
Ab,24.17,0.875,Cu,Dg,Eq,Ev,4.625,Ft,Ft,02,Ft,Gg,00520,2000:+
Ab,39.25,9.5,Cu,Dg,Em,Ev,6.5,Ft,Ft,14,Ff,Gg,00240,4607:+
Aa,20.50,11.835,Cu,Dg,Ec,Eh,6,Ft,Ff,0,Ff,Gg,00340,0:+
Aa,18.83,4.415,Cy,Dp,Ec,Eh,3,Ft,Ff,0,Ff,Gg,00240,0:+
Ab,19.17,9.5,Cu,Dg,Ew,Ev,1.5,Ft,Ff,0,Ff,Gg,00120,2206:+
Aa,25.00,0.875,Cu,Dg,Ex,Eh,1.04,Ft,Ff,0,Ft,Gg,00160,5860:+
Ab,20.17,9.25,Cu,Dg,Ec,Ev,1.665,Ft,Ft,03,Ft,Gg,00040,28:+
Ab,25.75,0.5,Cu,Dg,Ec,Ev,1.46,Ft,Ft,05,Ft,Gg,00312,0:+
Ab,20.42,7,Cu,Dg,Ec,Ev,1.625,Ft,Ft,03,Ff,Gg,00200,1391:+
Ab,?,4,Cu,Dg,Ex,Ev,5,Ft,Ft,03,Ft,Gg,00290,2279:+
Ab,39.00,5,Cu,Dg,Ecc,Ev,3.5,Ft,Ft,10,Ft,Gg,00000,0:+
Aa,64.08,0.165,Cu,Dg,Eff,Eff,0,Ft,Ft,01,Ff,Gg,00232,100:+
Ab,28.25,5.125,Cu,Dg,Ex,Ev,4.75,Ft,Ft,02,Ff,Gg,00420,7:+
Aa,28.75,3.75,Cu,Dg,Ec,Ev,1.085,Ft,Ft,01,Ft,Gg,00371,0:+
Ab,31.33,19.5,Cu,Dg,Ec,Ev,7,Ft,Ft,16,Ff,Gg,00000,5000:+
Aa,18.92,9,Cu,Dg,Eaa,Ev,0.75,Ft,Ft,02,Ff,Gg,00088,591:+
Aa,24.75,3,Cu,Dg,Eq,Eh,1.835,Ft,Ft,19,Ff,Gg,00000,500:+
Aa,30.67,12,Cu,Dg,Ec,Ev,2,Ft,Ft,01,Ff,Gg,00220,19:+
Ab,21.00,4.79,Cy,Dp,Ew,Ev,2.25,Ft,Ft,01,Ft,Gg,00080,300:+
Ab,13.75,4,Cy,Dp,Ew,Ev,1.75,Ft,Ft,02,Ft,Gg,00120,1000:+
Aa,46.00,4,Cu,Dg,Ej,Ej,0,Ft,Ff,0,Ff,Gg,00100,960:+
Aa,44.33,0,Cu,Dg,Ec,Ev,2.5,Ft,Ff,0,Ff,Gg,00000,0:+
Ab,20.25,9.96,Cu,Dg,Ee,Edd,0,Ft,Ff,0,Ff,Gg,00000,0:+
Ab,22.67,2.54,Cy,Dp,Ec,Eh,2.585,Ft,Ff,0,Ff,Gg,00000,0:+
Ab,?,10.5,Cu,Dg,Ex,Ev,6.5,Ft,Ff,0,Ff,Gg,00000,0:+
Aa,60.92,5,Cu,Dg,Eaa,Ev,4,Ft,Ft,04,Ff,Gg,00000,99:+
Ab,16.08,0.75,Cu,Dg,Ec,Ev,1.75,Ft,Ft,05,Ft,Gg,00352,690:+
Aa,28.17,0.375,Cu,Dg,Eq,Ev,0.585,Ft,Ft,04,Ff,Gg,00080,0:+
Ab,39.17,1.71,Cu,Dg,Ex,Ev,0.125,Ft,Ft,05,Ft,Gg,00480,0:+
?,20.42,7.5,Cu,Dg,Ek,Ev,1.5,Ft,Ft,01,Ff,Gg,00160,234:+
Aa,30.00,5.29,Cu,Dg,Ee,Edd,2.25,Ft,Ft,05,Ft,Gg,00099,500:+
Ab,22.83,3,Cu,Dg,Em,Ev,1.29,Ft,Ft,01,Ff,Gg,00260,800:+
Aa,22.50,8.5,Cu,Dg,Eq,Ev,1.75,Ft,Ft,10,Ff,Gg,00080,990:-
Aa,28.58,1.665,Cu,Dg,Eq,Ev,2.415,Ft,Ff,0,Ft,Gg,00440,0:-
Ab,45.17,1.5,Cu,Dg,Ec,Ev,2.5,Ft,Ff,0,Ft,Gg,00140,0:-
Ab,41.58,1.75,Cu,Dg,Ek,Ev,0.21,Ft,Ff,0,Ff,Gg,00160,0:-
Aa,57.08,0.335,Cu,Dg,Ei,Ebb,1,Ft,Ff,0,Ft,Gg,00252,2197:-
Aa,55.75,7.08,Cu,Dg,Ek,Eh,6.75,Ft,Ft,03,Ft,Gg,00100,50:-
Ab,43.25,25.21,Cu,Dg,Eq,Eh,0.21,Ft,Ft,01,Ff,Gg,00760,90:-
Aa,25.33,2.085,Cu,Dg,Ec,Eh,2.75,Ft,Ff,0,Ft,Gg,00360,1:-
Aa,24.58,0.67,Cu,Dg,Eaa,Eh,1.75,Ft,Ff,0,Ff,Gg,00400,0:-
Ab,43.17,2.25,Cu,Dg,Ei,Ebb,0.75,Ft,Ff,0,Ff,Gg,00560,0:-
Ab,40.92,0.835,Cu,Dg,Eff,Eff,0,Ft,Ff,0,Ff,Gg,00130,1:-
Ab,31.83,2.5,Cu,Dg,Eaa,Ev,7.5,Ft,Ff,0,Ft,Gg,00523,0:-
Aa,33.92,1.585,Cy,Dp,Eff,Eff,0,Ft,Ff,0,Ff,Gg,00320,0:-
Aa,24.92,1.25,Cu,Dg,Eff,Eff,0,Ft,Ff,0,Ff,Gg,00080,0:-
Ab,35.25,3.165,Cu,Dg,Ex,Eh,3.75,Ft,Ff,0,Ft,Gg,00680,0:-
Ab,34.25,1.75,Cu,Dg,Ew,Ebb,0.25,Ft,Ff,0,Ft,Gg,00163,0:-
Ab,80.25,5.5,Cu,Dg,?,?,0.54,Ft,Ff,0,Ff,Gg,00000,340:-
Ab,19.42,1.5,Cy,Dp,Ecc,Ev,2,Ft,Ff,0,Ft,Gg,00100,20:-
Ab,42.75,3,Cu,Dg,Ei,Ebb,1,Ft,Ff,0,Ff,Gg,00000,200:-
Ab,19.67,10,Cy,Dp,Ek,Eh,0.835,Ft,Ff,0,Ft,Gg,00140,0:-
Ab,36.33,3.79,Cu,Dg,Ew,Ev,1.165,Ft,Ff,0,Ft,Gg,00200,0:-
Ab,30.08,1.04,Cy,Dp,Ei,Ebb,0.5,Ft,Ft,10,Ft,Gg,00132,28:-
Ab,44.25,11,Cy,Dp,Ed,Ev,1.5,Ft,Ff,0,Ff,Gs,00000,0:-
Ab,23.58,0.46,Cy,Dp,Ew,Ev,2.625,Ft,Ft,06,Ft,Gg,00208,347:-
Ab,23.92,1.5,Cu,Dg,Ed,Eh,1.875,Ft,Ft,06,Ff,Gg,00200,327:+
Ab,33.17,1,Cu,Dg,Ex,Ev,0.75,Ft,Ft,07,Ft,Gg,00340,4071:+
Ab,48.33,12,Cu,Dg,Em,Ev,16,Ft,Ff,0,Ff,Gs,00110,0:+
Ab,76.75,22.29,Cu,Dg,Ee,Ez,12.75,Ft,Ft,01,Ft,Gg,00000,109:+
Ab,51.33,10,Cu,Dg,Ei,Ebb,0,Ft,Ft,11,Ff,Gg,00000,1249:+
Ab,34.75,15,Cu,Dg,Er,En,5.375,Ft,Ft,09,Ft,Gg,00000,134:+
Ab,38.58,3.335,Cu,Dg,Ew,Ev,4,Ft,Ft,14,Ff,Gg,00383,1344:+
Aa,22.42,11.25,Cy,Dp,Ex,Eh,0.75,Ft,Ft,04,Ff,Gg,00000,321:+
Ab,41.92,0.42,Cu,Dg,Ec,Eh,0.21,Ft,Ft,06,Ff,Gg,00220,948:+
Ab,29.58,4.5,Cu,Dg,Ew,Ev,7.5,Ft,Ft,02,Ft,Gg,00330,0:+
Aa,32.17,1.46,Cu,Dg,Ew,Ev,1.085,Ft,Ft,16,Ff,Gg,00120,2079:+
Ab,51.42,0.04,Cu,Dg,Ex,Eh,0.04,Ft,Ff,0,Ff,Gg,00000,3000:+
Aa,22.83,2.29,Cu,Dg,Eq,Eh,2.29,Ft,Ft,07,Ft,Gg,00140,2384:+
Aa,25.00,12.33,Cu,Dg,Ecc,Eh,3.5,Ft,Ft,06,Ff,Gg,00400,458:+
Ab,26.75,1.125,Cu,Dg,Ex,Eh,1.25,Ft,Ff,0,Ff,Gg,00000,5298:+
Ab,23.33,1.5,Cu,Dg,Ec,Eh,1.415,Ft,Ff,0,Ff,Gg,00422,200:+
Ab,24.42,12.335,Cu,Dg,Eq,Eh,1.585,Ft,Ff,0,Ft,Gg,00120,0:+
Ab,42.17,5.04,Cu,Dg,Eq,Eh,12.75,Ft,Ff,0,Ft,Gg,00092,0:+
Aa,20.83,3,Cu,Dg,Eaa,Ev,0.04,Ft,Ff,0,Ff,Gg,00100,0:+
Ab,23.08,11.5,Cu,Dg,Ew,Eh,2.125,Ft,Ft,11,Ft,Gg,00290,284:+
Aa,25.17,2.875,Cu,Dg,Ex,Eh,0.875,Ft,Ff,0,Ff,Gg,00360,0:+
Ab,43.08,0.375,Cy,Dp,Ec,Ev,0.375,Ft,Ft,08,Ft,Gg,00300,162:+
Aa,35.75,0.915,Cu,Dg,Eaa,Ev,0.75,Ft,Ft,04,Ff,Gg,00000,1583:+
Ab,59.50,2.75,Cu,Dg,Ew,Ev,1.75,Ft,Ft,05,Ft,Gg,00060,58:+
Ab,21.00,3,Cy,Dp,Ed,Ev,1.085,Ft,Ft,08,Ft,Gg,00160,1:+
Ab,21.92,0.54,Cy,Dp,Ex,Ev,0.04,Ft,Ft,01,Ft,Gg,00840,59:+
Aa,65.17,14,Cu,Dg,Eff,Eff,0,Ft,Ft,11,Ft,Gg,00000,1400:+
Aa,20.33,10,Cu,Dg,Ec,Eh,1,Ft,Ft,04,Ff,Gg,00050,1465:+
Ab,32.25,0.165,Cy,Dp,Ec,Eh,3.25,Ft,Ft,01,Ft,Gg,00432,8000:+
Ab,30.17,0.5,Cu,Dg,Ec,Ev,1.75,Ft,Ft,11,Ff,Gg,00032,540:+
Ab,25.17,6,Cu,Dg,Ec,Ev,1,Ft,Ft,03,Ff,Gg,00000,0:+
Ab,39.17,1.625,Cu,Dg,Ec,Ev,1.5,Ft,Ft,10,Ff,Gg,00186,4700:+
Ab,39.08,6,Cu,Dg,Em,Ev,1.29,Ft,Ft,05,Ft,Gg,00108,1097:+
Ab,31.67,0.83,Cu,Dg,Ex,Ev,1.335,Ft,Ft,08,Ft,Gg,00303,3290:+
Ab,41.00,0.04,Cu,Dg,Ee,Ev,0.04,Ff,Ft,01,Ff,Gs,00560,0:+
Ab,48.50,4.25,Cu,Dg,Em,Ev,0.125,Ft,Ff,0,Ft,Gg,00225,0:+
Ab,32.67,9,Cy,Dp,Ew,Eh,5.25,Ft,Ff,0,Ft,Gg,00154,0:+
Aa,28.08,15,Cy,Dp,Ee,Ez,0,Ft,Ff,0,Ff,Gg,00000,13212:+
Ab,73.42,17.75,Cu,Dg,Eff,Eff,0,Ft,Ff,0,Ft,Gg,00000,0:+
Ab,64.08,20,Cu,Dg,Ex,Eh,17.5,Ft,Ft,09,Ft,Gg,00000,1000:+
Ab,51.58,15,Cu,Dg,Ec,Ev,8.5,Ft,Ft,09,Ff,Gg,00000,0:+
Ab,26.67,1.75,Cy,Dp,Ec,Ev,1,Ft,Ft,05,Ft,Gg,00160,5777:+
Ab,25.33,0.58,Cu,Dg,Ec,Ev,0.29,Ft,Ft,07,Ft,Gg,00096,5124:+
Ab,30.17,6.5,Cu,Dg,Ecc,Ev,3.125,Ft,Ft,08,Ff,Gg,00330,1200:+
Ab,27.00,0.75,Cu,Dg,Ec,Eh,4.25,Ft,Ft,03,Ft,Gg,00312,150:+
Ab,23.17,0,?,?,?,?,0,Ff,Ff,0,Ff,Gp,?,0:+
Ab,34.17,5.25,Cu,Dg,Ew,Ev,0.085,Ff,Ff,0,Ft,Gg,00290,6:+
Ab,38.67,0.21,Cu,Dg,Ek,Ev,0.085,Ft,Ff,0,Ft,Gg,00280,0:+
Ab,25.75,0.75,Cu,Dg,Ec,Ebb,0.25,Ft,Ff,0,Ff,Gg,00349,23:+
Aa,46.08,3,Cu,Dg,Ec,Ev,2.375,Ft,Ft,08,Ft,Gg,00396,4159:+
Aa,21.50,6,Cu,Dg,Eaa,Ev,2.5,Ft,Ft,03,Ff,Gg,00080,918:+
?,20.08,0.125,Cu,Dg,Eq,Ev,1,Ff,Ft,01,Ff,Gg,00240,768:+
Ab,20.50,2.415,Cu,Dg,Ec,Ev,2,Ft,Ft,11,Ft,Gg,00200,3000:+
Aa,29.50,0.46,Cu,Dg,Ek,Ev,0.54,Ft,Ft,04,Ff,Gg,00380,500:+
?,42.25,1.75,Cy,Dp,?,?,0,Ff,Ff,0,Ft,Gg,00150,1:-
Ab,29.83,1.25,Cy,Dp,Ek,Ev,0.25,Ff,Ff,0,Ff,Gg,00224,0:-
Ab,20.08,0.25,Cu,Dg,Eq,Ev,0.125,Ff,Ff,0,Ff,Gg,00200,0:-
Ab,23.42,0.585,Cu,Dg,Ec,Eh,0.085,Ft,Ff,0,Ff,Gg,00180,0:-
Aa,29.58,1.75,Cy,Dp,Ek,Ev,1.25,Ff,Ff,0,Ft,Gg,00280,0:-
Ab,16.17,0.04,Cu,Dg,Ec,Ev,0.04,Ff,Ff,0,Ff,Gg,00000,0:+
Ab,32.33,3.5,Cu,Dg,Ek,Ev,0.5,Ff,Ff,0,Ft,Gg,00232,0:-
Ab,?,0.04,Cy,Dp,Ed,Ev,4.25,Ff,Ff,0,Ft,Gg,00460,0:-
Ab,47.83,4.165,Cu,Dg,Ex,Ebb,0.085,Ff,Ff,0,Ft,Gg,00520,0:-
Ab,20.00,1.25,Cy,Dp,Ek,Ev,0.125,Ff,Ff,0,Ff,Gg,00140,4:-
Ab,27.58,3.25,Cy,Dp,Eq,Eh,5.085,Ff,Ft,02,Ft,Gg,00369,1:-
Ab,22.00,0.79,Cu,Dg,Ew,Ev,0.29,Ff,Ft,01,Ff,Gg,00420,283:-
Ab,19.33,10.915,Cu,Dg,Ec,Ebb,0.585,Ff,Ft,02,Ft,Gg,00200,7:-
Aa,38.33,4.415,Cu,Dg,Ec,Ev,0.125,Ff,Ff,0,Ff,Gg,00160,0:-
Ab,29.42,1.25,Cu,Dg,Ec,Eh,0.25,Ff,Ft,02,Ft,Gg,00400,108:-
Ab,22.67,0.75,Cu,Dg,Ei,Ev,1.585,Ff,Ft,01,Ft,Gg,00400,9:-
Ab,32.25,14,Cy,Dp,Eff,Eff,0,Ff,Ft,02,Ff,Gg,00160,1:-
Ab,29.58,4.75,Cu,Dg,Em,Ev,2,Ff,Ft,01,Ft,Gg,00460,68:-
Ab,18.42,10.415,Cy,Dp,Eaa,Ev,0.125,Ft,Ff,0,Ff,Gg,00120,375:-
Ab,22.17,2.25,Cu,Dg,Ei,Ev,0.125,Ff,Ff,0,Ff,Gg,00160,10:-
Ab,22.67,0.165,Cu,Dg,Ec,Ej,2.25,Ff,Ff,0,Ft,Gs,00000,0:+
Aa,25.58,0,?,?,?,?,0,Ff,Ff,0,Ff,Gp,?,0:+
Ab,18.83,0,Cu,Dg,Eq,Ev,0.665,Ff,Ff,0,Ff,Gg,00160,1:-
Ab,21.58,0.79,Cy,Dp,Ecc,Ev,0.665,Ff,Ff,0,Ff,Gg,00160,0:-
Ab,23.75,12,Cu,Dg,Ec,Ev,2.085,Ff,Ff,0,Ff,Gs,00080,0:-
Ab,22.00,7.835,Cy,Dp,Ei,Ebb,0.165,Ff,Ff,0,Ft,Gg,?,0:-
Ab,36.08,2.54,Cu,Dg,Eff,Eff,0,Ff,Ff,0,Ff,Gg,00000,1000:-
Ab,29.25,13,Cu,Dg,Ed,Eh,0.5,Ff,Ff,0,Ff,Gg,00228,0:-
Aa,19.58,0.665,Cu,Dg,Ew,Ev,1.665,Ff,Ff,0,Ff,Gg,00220,5:-
Aa,22.92,1.25,Cu,Dg,Eq,Ev,0.25,Ff,Ff,0,Ft,Gg,00120,809:-
Aa,27.25,0.29,Cu,Dg,Em,Eh,0.125,Ff,Ft,01,Ft,Gg,00272,108:-
Aa,38.75,1.5,Cu,Dg,Eff,Eff,0,Ff,Ff,0,Ff,Gg,00076,0:-
Ab,32.42,2.165,Cy,Dp,Ek,Eff,0,Ff,Ff,0,Ff,Gg,00120,0:-
Aa,23.75,0.71,Cu,Dg,Ew,Ev,0.25,Ff,Ft,01,Ft,Gg,00240,4:-
Ab,18.17,2.46,Cu,Dg,Ec,En,0.96,Ff,Ft,02,Ft,Gg,00160,587:-
Ab,40.92,0.5,Cy,Dp,Em,Ev,0.5,Ff,Ff,0,Ft,Gg,00130,0:-
Ab,19.50,9.585,Cu,Dg,Eaa,Ev,0.79,Ff,Ff,0,Ff,Gg,00080,350:-
Ab,28.58,3.625,Cu,Dg,Eaa,Ev,0.25,Ff,Ff,0,Ft,Gg,00100,0:-
Ab,35.58,0.75,Cu,Dg,Ek,Ev,1.5,Ff,Ff,0,Ft,Gg,00231,0:-
Ab,34.17,2.75,Cu,Dg,Ei,Ebb,2.5,Ff,Ff,0,Ft,Gg,00232,200:-
?,33.17,2.25,Cy,Dp,Ecc,Ev,3.5,Ff,Ff,0,Ft,Gg,00200,141:-
Ab,31.58,0.75,Cy,Dp,Eaa,Ev,3.5,Ff,Ff,0,Ft,Gg,00320,0:-
Aa,52.50,7,Cu,Dg,Eaa,Eh,3,Ff,Ff,0,Ff,Gg,00000,0:-
Ab,36.17,0.42,Cy,Dp,Ew,Ev,0.29,Ff,Ff,0,Ft,Gg,00309,2:-
Ab,37.33,2.665,Cu,Dg,Ecc,Ev,0.165,Ff,Ff,0,Ft,Gg,00000,501:-
Aa,20.83,8.5,Cu,Dg,Ec,Ev,0.165,Ff,Ff,0,Ff,Gg,00000,351:-
Ab,24.08,9,Cu,Dg,Eaa,Ev,0.25,Ff,Ff,0,Ft,Gg,00000,0:-
Ab,25.58,0.335,Cu,Dg,Ek,Eh,3.5,Ff,Ff,0,Ft,Gg,00340,0:-
Aa,35.17,3.75,Cu,Dg,Eff,Eff,0,Ff,Ft,06,Ff,Gg,00000,200:-
Ab,48.08,3.75,Cu,Dg,Ei,Ebb,1,Ff,Ff,0,Ff,Gg,00100,2:-
Aa,15.83,7.625,Cu,Dg,Eq,Ev,0.125,Ff,Ft,01,Ft,Gg,00000,160:-
Aa,22.50,0.415,Cu,Dg,Ei,Ev,0.335,Ff,Ff,0,Ft,Gs,00144,0:-
Ab,21.50,11.5,Cu,Dg,Ei,Ev,0.5,Ft,Ff,0,Ft,Gg,00100,68:-
Aa,23.58,0.83,Cu,Dg,Eq,Ev,0.415,Ff,Ft,01,Ft,Gg,00200,11:-
Aa,21.08,5,Cy,Dp,Eff,Eff,0,Ff,Ff,0,Ff,Gg,00000,0:-
Ab,25.67,3.25,Cu,Dg,Ec,Eh,2.29,Ff,Ft,01,Ft,Gg,00416,21:-
Aa,38.92,1.665,Cu,Dg,Eaa,Ev,0.25,Ff,Ff,0,Ff,Gg,00000,390:-
Aa,15.75,0.375,Cu,Dg,Ec,Ev,1,Ff,Ff,0,Ff,Gg,00120,18:-
Aa,28.58,3.75,Cu,Dg,Ec,Ev,0.25,Ff,Ft,01,Ft,Gg,00040,154:-
Ab,22.25,9,Cu,Dg,Eaa,Ev,0.085,Ff,Ff,0,Ff,Gg,00000,0:-
Ab,29.83,3.5,Cu,Dg,Ec,Ev,0.165,Ff,Ff,0,Ff,Gg,00216,0:-
Aa,23.50,1.5,Cu,Dg,Ew,Ev,0.875,Ff,Ff,0,Ft,Gg,00160,0:-
Ab,32.08,4,Cy,Dp,Ecc,Ev,1.5,Ff,Ff,0,Ft,Gg,00120,0:-
Ab,31.08,1.5,Cy,Dp,Ew,Ev,0.04,Ff,Ff,0,Ff,Gs,00160,0:-
Ab,31.83,0.04,Cy,Dp,Em,Ev,0.04,Ff,Ff,0,Ff,Gg,00000,0:-
Aa,21.75,11.75,Cu,Dg,Ec,Ev,0.25,Ff,Ff,0,Ft,Gg,00180,0:-
Aa,17.92,0.54,Cu,Dg,Ec,Ev,1.75,Ff,Ft,01,Ft,Gg,00080,5:-
Ab,30.33,0.5,Cu,Dg,Ed,Eh,0.085,Ff,Ff,0,Ft,Gs,00252,0:-
Ab,51.83,2.04,Cy,Dp,Eff,Eff,1.5,Ff,Ff,0,Ff,Gg,00120,1:-
Ab,47.17,5.835,Cu,Dg,Ew,Ev,5.5,Ff,Ff,0,Ff,Gg,00465,150:-
Ab,25.83,12.835,Cu,Dg,Ecc,Ev,0.5,Ff,Ff,0,Ff,Gg,00000,2:-
Aa,50.25,0.835,Cu,Dg,Eaa,Ev,0.5,Ff,Ff,0,Ft,Gg,00240,117:-
?,29.50,2,Cy,Dp,Ee,Eh,2,Ff,Ff,0,Ff,Gg,00256,17:-
Aa,37.33,2.5,Cu,Dg,Ei,Eh,0.21,Ff,Ff,0,Ff,Gg,00260,246:-
Aa,41.58,1.04,Cu,Dg,Eaa,Ev,0.665,Ff,Ff,0,Ff,Gg,00240,237:-
Aa,30.58,10.665,Cu,Dg,Eq,Eh,0.085,Ff,Ft,12,Ft,Gg,00129,3:-
Ab,19.42,7.25,Cu,Dg,Em,Ev,0.04,Ff,Ft,01,Ff,Gg,00100,1:-
Aa,17.92,10.21,Cu,Dg,Eff,Eff,0,Ff,Ff,0,Ff,Gg,00000,50:-
Aa,20.08,1.25,Cu,Dg,Ec,Ev,0,Ff,Ff,0,Ff,Gg,00000,0:-
Ab,19.50,0.29,Cu,Dg,Ek,Ev,0.29,Ff,Ff,0,Ff,Gg,00280,364:-
Ab,27.83,1,Cy,Dp,Ed,Eh,3,Ff,Ff,0,Ff,Gg,00176,537:-
Ab,17.08,3.29,Cu,Dg,Ei,Ev,0.335,Ff,Ff,0,Ft,Gg,00140,2:-
Ab,36.42,0.75,Cy,Dp,Ed,Ev,0.585,Ff,Ff,0,Ff,Gg,00240,3:-
Ab,40.58,3.29,Cu,Dg,Em,Ev,3.5,Ff,Ff,0,Ft,Gs,00400,0:-
Ab,21.08,10.085,Cy,Dp,Ee,Eh,1.25,Ff,Ff,0,Ff,Gg,00260,0:-
Aa,22.67,0.75,Cu,Dg,Ec,Ev,2,Ff,Ft,02,Ft,Gg,00200,394:-
Aa,25.25,13.5,Cy,Dp,Eff,Eff,2,Ff,Ft,01,Ft,Gg,00200,1:-
Ab,17.92,0.205,Cu,Dg,Eaa,Ev,0.04,Ff,Ff,0,Ff,Gg,00280,750:-
Ab,35.00,3.375,Cu,Dg,Ec,Eh,8.29,Ff,Ff,0,Ft,Gg,00000,0:-
@.
@//E*O*F crx.d//
chmod u=rw,g=,o= crx.d
 
echo x - crx.data
sed 's/^@//' > "crx.data" <<'@//E*O*F crx.data//'
b,30.83,0,u,g,w,v,1.25,t,t,01,f,g,00202,0,+
a,58.67,4.46,u,g,q,h,3.04,t,t,06,f,g,00043,560,+
a,24.50,0.5,u,g,q,h,1.5,t,f,0,f,g,00280,824,+
b,27.83,1.54,u,g,w,v,3.75,t,t,05,t,g,00100,3,+
b,20.17,5.625,u,g,w,v,1.71,t,f,0,f,s,00120,0,+
b,32.08,4,u,g,m,v,2.5,t,f,0,t,g,00360,0,+
b,33.17,1.04,u,g,r,h,6.5,t,f,0,t,g,00164,31285,+
a,22.92,11.585,u,g,cc,v,0.04,t,f,0,f,g,00080,1349,+
b,54.42,0.5,y,p,k,h,3.96,t,f,0,f,g,00180,314,+
b,42.50,4.915,y,p,w,v,3.165,t,f,0,t,g,00052,1442,+
b,22.08,0.83,u,g,c,h,2.165,f,f,0,t,g,00128,0,+
b,29.92,1.835,u,g,c,h,4.335,t,f,0,f,g,00260,200,+
a,38.25,6,u,g,k,v,1,t,f,0,t,g,00000,0,+
b,48.08,6.04,u,g,k,v,0.04,f,f,0,f,g,00000,2690,+
a,45.83,10.5,u,g,q,v,5,t,t,07,t,g,00000,0,+
b,36.67,4.415,y,p,k,v,0.25,t,t,10,t,g,00320,0,+
b,28.25,0.875,u,g,m,v,0.96,t,t,03,t,g,00396,0,+
a,23.25,5.875,u,g,q,v,3.17,t,t,10,f,g,00120,245,+
b,21.83,0.25,u,g,d,h,0.665,t,f,0,t,g,00000,0,+
a,19.17,8.585,u,g,cc,h,0.75,t,t,07,f,g,00096,0,+
b,25.00,11.25,u,g,c,v,2.5,t,t,17,f,g,00200,1208,+
b,23.25,1,u,g,c,v,0.835,t,f,0,f,s,00300,0,+
a,47.75,8,u,g,c,v,7.875,t,t,06,t,g,00000,1260,+
a,27.42,14.5,u,g,x,h,3.085,t,t,01,f,g,00120,11,+
a,41.17,6.5,u,g,q,v,0.5,t,t,03,t,g,00145,0,+
a,15.83,0.585,u,g,c,h,1.5,t,t,02,f,g,00100,0,+
a,47.00,13,u,g,i,bb,5.165,t,t,09,t,g,00000,0,+
b,56.58,18.5,u,g,d,bb,15,t,t,17,t,g,00000,0,+
b,57.42,8.5,u,g,e,h,7,t,t,03,f,g,00000,0,+
b,42.08,1.04,u,g,w,v,5,t,t,06,t,g,00500,10000,+
b,29.25,14.79,u,g,aa,v,5.04,t,t,05,t,g,00168,0,+
b,42.00,9.79,u,g,x,h,7.96,t,t,08,f,g,00000,0,+
b,49.50,7.585,u,g,i,bb,7.585,t,t,15,t,g,00000,5000,+
a,36.75,5.125,u,g,e,v,5,t,f,0,t,g,00000,4000,+
a,22.58,10.75,u,g,q,v,0.415,t,t,05,t,g,00000,560,+
b,27.83,1.5,u,g,w,v,2,t,t,11,t,g,00434,35,+
b,27.25,1.585,u,g,cc,h,1.835,t,t,12,t,g,00583,713,+
a,23.00,11.75,u,g,x,h,0.5,t,t,02,t,g,00300,551,+
b,27.75,0.585,y,p,cc,v,0.25,t,t,02,f,g,00260,500,+
b,54.58,9.415,u,g,ff,ff,14.415,t,t,11,t,g,00030,300,+
b,34.17,9.17,u,g,c,v,4.5,t,t,12,t,g,00000,221,+
b,28.92,15,u,g,c,h,5.335,t,t,11,f,g,00000,2283,+
b,29.67,1.415,u,g,w,h,0.75,t,t,01,f,g,00240,100,+
b,39.58,13.915,u,g,w,v,8.625,t,t,06,t,g,00070,0,+
b,56.42,28,y,p,c,v,28.5,t,t,40,f,g,00000,15,+
b,54.33,6.75,u,g,c,h,2.625,t,t,11,t,g,00000,284,+
a,41.00,2.04,y,p,q,h,0.125,t,t,23,t,g,00455,1236,+
b,31.92,4.46,u,g,cc,h,6.04,t,t,03,f,g,00311,300,+
b,41.50,1.54,u,g,i,bb,3.5,f,f,0,f,g,00216,0,+
b,23.92,0.665,u,g,c,v,0.165,f,f,0,f,g,00100,0,+
a,25.75,0.5,u,g,c,h,0.875,t,f,0,t,g,00491,0,+
b,26.00,1,u,g,q,v,1.75,t,f,0,t,g,00280,0,+
b,37.42,2.04,u,g,w,v,0.04,t,f,0,t,g,00400,5800,+
b,34.92,2.5,u,g,w,v,0,t,f,0,t,g,00239,200,+
b,34.25,3,u,g,cc,h,7.415,t,f,0,t,g,00000,0,+
b,23.33,11.625,y,p,w,v,0.835,t,f,0,t,g,00160,300,+
b,23.17,0,u,g,cc,v,0.085,t,f,0,f,g,00000,0,+
b,44.33,0.5,u,g,i,h,5,t,f,0,t,g,00320,0,+
b,35.17,4.5,u,g,x,h,5.75,f,f,0,t,s,00711,0,+
b,43.25,3,u,g,q,h,6,t,t,11,f,g,00080,0,+
b,56.75,12.25,u,g,m,v,1.25,t,t,04,t,g,00200,0,+
b,31.67,16.165,u,g,d,v,3,t,t,09,f,g,00250,730,+
a,23.42,0.79,y,p,q,v,1.5,t,t,02,t,g,00080,400,+
a,20.42,0.835,u,g,q,v,1.585,t,t,01,f,g,00000,0,+
b,26.67,4.25,u,g,cc,v,4.29,t,t,01,t,g,00120,0,+
b,34.17,1.54,u,g,cc,v,1.54,t,t,01,t,g,00520,50000,+
a,36.00,1,u,g,c,v,2,t,t,11,f,g,00000,456,+
b,25.50,0.375,u,g,m,v,0.25,t,t,03,f,g,00260,15108,+
b,19.42,6.5,u,g,w,h,1.46,t,t,07,f,g,00080,2954,+
b,35.17,25.125,u,g,x,h,1.625,t,t,01,t,g,00515,500,+
b,32.33,7.5,u,g,e,bb,1.585,t,f,0,t,s,00420,0,-
b,34.83,4,u,g,d,bb,12.5,t,f,0,t,g,?,0,-
a,38.58,5,u,g,cc,v,13.5,t,f,0,t,g,00980,0,-
b,44.25,0.5,u,g,m,v,10.75,t,f,0,f,s,00400,0,-
b,44.83,7,y,p,c,v,1.625,f,f,0,f,g,00160,2,-
b,20.67,5.29,u,g,q,v,0.375,t,t,01,f,g,00160,0,-
b,34.08,6.5,u,g,aa,v,0.125,t,f,0,t,g,00443,0,-
a,19.17,0.585,y,p,aa,v,0.585,t,f,0,t,g,00160,0,-
b,21.67,1.165,y,p,k,v,2.5,t,t,01,f,g,00180,20,-
b,21.50,9.75,u,g,c,v,0.25,t,f,0,f,g,00140,0,-
b,49.58,19,u,g,ff,ff,0,t,t,01,f,g,00094,0,-
a,27.67,1.5,u,g,m,v,2,t,f,0,f,s,00368,0,-
b,39.83,0.5,u,g,m,v,0.25,t,f,0,f,s,00288,0,-
a,?,3.5,u,g,d,v,3,t,f,0,t,g,00300,0,-
b,27.25,0.625,u,g,aa,v,0.455,t,f,0,t,g,00200,0,-
b,37.17,4,u,g,c,bb,5,t,f,0,t,s,00280,0,-
b,?,0.375,u,g,d,v,0.875,t,f,0,t,s,00928,0,-
b,25.67,2.21,y,p,aa,v,4,t,f,0,f,g,00188,0,-
b,34.00,4.5,u,g,aa,v,1,t,f,0,t,g,00240,0,-
a,49.00,1.5,u,g,j,j,0,t,f,0,t,g,00100,27,-
b,62.50,12.75,y,p,c,h,5,t,f,0,f,g,00112,0,-
b,31.42,15.5,u,g,c,v,0.5,t,f,0,f,g,00120,0,-
b,?,5,y,p,aa,v,8.5,t,f,0,f,g,00000,0,-
b,52.33,1.375,y,p,c,h,9.46,t,f,0,t,g,00200,100,-
b,28.75,1.5,y,p,c,v,1.5,t,f,0,t,g,00000,225,-
a,28.58,3.54,u,g,i,bb,0.5,t,f,0,t,g,00171,0,-
b,23.00,0.625,y,p,aa,v,0.125,t,f,0,f,g,00180,1,-
b,?,0.5,u,g,c,bb,0.835,t,f,0,t,s,00320,0,-
a,22.50,11,y,p,q,v,3,t,f,0,t,g,00268,0,-
a,28.50,1,u,g,q,v,1,t,t,02,t,g,00167,500,-
b,37.50,1.75,y,p,c,bb,0.25,t,f,0,t,g,00164,400,-
b,35.25,16.5,y,p,c,v,4,t,f,0,f,g,00080,0,-
b,18.67,5,u,g,q,v,0.375,t,t,02,f,g,00000,38,-
b,25.00,12,u,g,k,v,2.25,t,t,02,t,g,00120,5,-
b,27.83,4,y,p,i,h,5.75,t,t,02,t,g,00075,0,-
b,54.83,15.5,u,g,e,z,0,t,t,20,f,g,00152,130,-
b,28.75,1.165,u,g,k,v,0.5,t,f,0,f,s,00280,0,-
a,25.00,11,y,p,aa,v,4.5,t,f,0,f,g,00120,0,-
b,40.92,2.25,y,p,x,h,10,t,f,0,t,g,00176,0,-
a,19.75,0.75,u,g,c,v,0.795,t,t,05,t,g,00140,5,-
b,29.17,3.5,u,g,w,v,3.5,t,t,03,t,g,00329,0,-
a,24.50,1.04,y,p,ff,ff,0.5,t,t,03,f,g,00180,147,-
b,24.58,12.5,u,g,w,v,0.875,t,f,0,t,g,00260,0,-
a,33.75,0.75,u,g,k,bb,1,t,t,03,t,g,00212,0,-
b,20.67,1.25,y,p,c,h,1.375,t,t,03,t,g,00140,210,-
a,25.42,1.125,u,g,q,v,1.29,t,t,02,f,g,00200,0,-
b,37.75,7,u,g,q,h,11.5,t,t,07,t,g,00300,5,-
b,52.50,6.5,u,g,k,v,6.29,t,t,15,f,g,00000,11202,+
b,57.83,7.04,u,g,m,v,14,t,t,06,t,g,00360,1332,+
a,20.75,10.335,u,g,cc,h,0.335,t,t,01,t,g,00080,50,+
b,39.92,6.21,u,g,q,v,0.04,t,t,01,f,g,00200,300,+
b,25.67,12.5,u,g,cc,v,1.21,t,t,67,t,g,00140,258,+
a,24.75,12.5,u,g,aa,v,1.5,t,t,12,t,g,00120,567,+
a,44.17,6.665,u,g,q,v,7.375,t,t,03,t,g,00000,0,+
a,23.50,9,u,g,q,v,8.5,t,t,05,t,g,00120,0,+
b,34.92,5,u,g,x,h,7.5,t,t,06,t,g,00000,1000,+
b,47.67,2.5,u,g,m,bb,2.5,t,t,12,t,g,00410,2510,+
b,22.75,11,u,g,q,v,2.5,t,t,07,t,g,00100,809,+
b,34.42,4.25,u,g,i,bb,3.25,t,t,02,f,g,00274,610,+
a,28.42,3.5,u,g,w,v,0.835,t,f,0,f,s,00280,0,+
b,67.75,5.5,u,g,e,z,13,t,t,01,t,g,00000,0,+
b,20.42,1.835,u,g,c,v,2.25,t,t,01,f,g,00100,150,+
a,47.42,8,u,g,e,bb,6.5,t,t,06,f,g,00375,51100,+
b,36.25,5,u,g,c,bb,2.5,t,t,06,f,g,00000,367,+
b,32.67,5.5,u,g,q,h,5.5,t,t,12,t,g,00408,1000,+
b,48.58,6.5,u,g,q,h,6,t,f,0,t,g,00350,0,+
b,39.92,0.54,y,p,aa,v,0.5,t,t,03,f,g,00200,1000,+
b,33.58,2.75,u,g,m,v,4.25,t,t,06,f,g,00204,0,+
a,18.83,9.5,u,g,w,v,1.625,t,t,06,t,g,00040,600,+
a,26.92,13.5,u,g,q,h,5,t,t,02,f,g,00000,5000,+
a,31.25,3.75,u,g,cc,h,0.625,t,t,09,t,g,00181,0,+
a,56.50,16,u,g,j,ff,0,t,t,15,f,g,00000,247,+
b,43.00,0.29,y,p,cc,h,1.75,t,t,08,f,g,00100,375,+
b,22.33,11,u,g,w,v,2,t,t,01,f,g,00080,278,+
b,27.25,1.665,u,g,cc,h,5.085,t,t,09,f,g,00399,827,+
b,32.83,2.5,u,g,cc,h,2.75,t,t,06,f,g,00160,2072,+
b,23.25,1.5,u,g,q,v,2.375,t,t,03,t,g,00000,582,+
a,40.33,7.54,y,p,q,h,8,t,t,14,f,g,00000,2300,+
a,30.50,6.5,u,g,c,bb,4,t,t,07,t,g,00000,3065,+
a,52.83,15,u,g,c,v,5.5,t,t,14,f,g,00000,2200,+
a,46.67,0.46,u,g,cc,h,0.415,t,t,11,t,g,00440,6,+
a,58.33,10,u,g,q,v,4,t,t,14,f,g,00000,1602,+
b,37.33,6.5,u,g,m,h,4.25,t,t,12,t,g,00093,0,+
b,23.08,2.5,u,g,c,v,1.085,t,t,11,t,g,00060,2184,+
b,32.75,1.5,u,g,cc,h,5.5,t,t,03,t,g,00000,0,+
a,21.67,11.5,y,p,j,j,0,t,t,11,t,g,00000,0,+
a,28.50,3.04,y,p,x,h,2.54,t,t,01,f,g,00070,0,+
a,68.67,15,u,g,e,z,0,t,t,14,f,g,00000,3376,+
b,28.00,2,u,g,k,h,4.165,t,t,02,t,g,00181,0,+
b,34.08,0.08,y,p,m,bb,0.04,t,t,01,t,g,00280,2000,+
b,27.67,2,u,g,x,h,1,t,t,04,f,g,00140,7544,+
b,44.00,2,u,g,m,v,1.75,t,t,02,t,g,00000,15,+
b,25.08,1.71,u,g,x,v,1.665,t,t,01,t,g,00395,20,+
b,32.00,1.75,y,p,e,h,0.04,t,f,0,t,g,00393,0,+
a,60.58,16.5,u,g,q,v,11,t,f,0,t,g,00021,10561,+
a,40.83,10,u,g,q,h,1.75,t,f,0,f,g,00029,837,+
b,19.33,9.5,u,g,q,v,1,t,f,0,t,g,00060,400,+
a,32.33,0.54,u,g,cc,v,0.04,t,f,0,f,g,00440,11177,+
b,36.67,3.25,u,g,q,h,9,t,f,0,t,g,00102,639,+
b,37.50,1.125,y,p,d,v,1.5,f,f,0,t,g,00431,0,+
a,25.08,2.54,y,p,aa,v,0.25,t,f,0,t,g,00370,0,+
b,41.33,0,u,g,c,bb,15,t,f,0,f,g,00000,0,+
b,56.00,12.5,u,g,k,h,8,t,f,0,t,g,00024,2028,+
a,49.83,13.585,u,g,k,h,8.5,t,f,0,t,g,00000,0,+
b,22.67,10.5,u,g,q,h,1.335,t,f,0,f,g,00100,0,+
b,27.00,1.5,y,p,w,v,0.375,t,f,0,t,g,00260,1065,+
b,25.00,12.5,u,g,aa,v,3,t,f,0,t,s,00020,0,+
a,26.08,8.665,u,g,aa,v,1.415,t,f,0,f,g,00160,150,+
a,18.42,9.25,u,g,q,v,1.21,t,t,04,f,g,00060,540,+
b,20.17,8.17,u,g,aa,v,1.96,t,t,14,f,g,00060,158,+
b,47.67,0.29,u,g,c,bb,15,t,t,20,f,g,00000,15000,+
a,21.25,2.335,u,g,i,bb,0.5,t,t,04,f,s,00080,0,+
a,20.67,3,u,g,q,v,0.165,t,t,03,f,g,00100,6,+
a,57.08,19.5,u,g,c,v,5.5,t,t,07,f,g,00000,3000,+
a,22.42,5.665,u,g,q,v,2.585,t,t,07,f,g,00129,3257,+
b,48.75,8.5,u,g,c,h,12.5,t,t,09,f,g,00181,1655,+
b,40.00,6.5,u,g,aa,bb,3.5,t,t,01,f,g,00000,500,+
b,40.58,5,u,g,c,v,5,t,t,07,f,g,00000,3065,+
a,28.67,1.04,u,g,c,v,2.5,t,t,05,t,g,00300,1430,+
a,33.08,4.625,u,g,q,h,1.625,t,t,02,f,g,00000,0,+
b,21.33,10.5,u,g,c,v,3,t,f,0,t,g,00000,0,+
b,42.00,0.205,u,g,i,h,5.125,t,f,0,f,g,00400,0,+
b,41.75,0.96,u,g,x,v,2.5,t,f,0,f,g,00510,600,+
b,22.67,1.585,y,p,w,v,3.085,t,t,06,f,g,00080,0,+
b,34.50,4.04,y,p,i,bb,8.5,t,t,07,t,g,00195,0,+
b,28.25,5.04,y,p,c,bb,1.5,t,t,08,t,g,00144,7,+
b,33.17,3.165,y,p,x,v,3.165,t,t,03,t,g,00380,0,+
b,48.17,7.625,u,g,w,h,15.5,t,t,12,f,g,00000,790,+
b,27.58,2.04,y,p,aa,v,2,t,t,03,t,g,00370,560,+
b,22.58,10.04,u,g,x,v,0.04,t,t,09,f,g,00060,396,+
a,24.08,0.5,u,g,q,h,1.25,t,t,01,f,g,00000,678,+
a,41.33,1,u,g,i,bb,2.25,t,f,0,t,g,00000,300,+
b,24.83,2.75,u,g,c,v,2.25,t,t,06,f,g,?,600,+
a,20.75,10.25,u,g,q,v,0.71,t,t,02,t,g,00049,0,+
b,36.33,2.125,y,p,w,v,0.085,t,t,01,f,g,00050,1187,+
a,35.42,12,u,g,q,h,14,t,t,08,f,g,00000,6590,+
a,71.58,0,?,?,?,?,0,f,f,0,f,p,?,0,+
b,28.67,9.335,u,g,q,h,5.665,t,t,06,f,g,00381,168,+
b,35.17,2.5,u,g,k,v,4.5,t,t,07,f,g,00150,1270,+
b,39.50,4.25,u,g,c,bb,6.5,t,t,16,f,g,00117,1210,+
b,39.33,5.875,u,g,cc,h,10,t,t,14,t,g,00399,0,+
b,24.33,6.625,y,p,d,v,5.5,t,f,0,t,s,00100,0,+
b,60.08,14.5,u,g,ff,ff,18,t,t,15,t,g,00000,1000,+
b,23.08,11.5,u,g,i,v,3.5,t,t,09,f,g,00056,742,+
b,26.67,2.71,y,p,cc,v,5.25,t,t,01,f,g,00211,0,+
b,48.17,3.5,u,g,aa,v,3.5,t,f,0,f,s,00230,0,+
b,41.17,4.04,u,g,cc,h,7,t,t,08,f,g,00320,0,+
b,55.92,11.5,u,g,ff,ff,5,t,t,05,f,g,00000,8851,+
b,53.92,9.625,u,g,e,v,8.665,t,t,05,f,g,00000,0,+
a,18.92,9.25,y,p,c,v,1,t,t,04,t,g,00080,500,+
a,50.08,12.54,u,g,aa,v,2.29,t,t,03,t,g,00156,0,+
b,65.42,11,u,g,e,z,20,t,t,07,t,g,00022,0,+
a,17.58,9,u,g,aa,v,1.375,t,f,0,t,g,00000,0,+
a,18.83,9.54,u,g,aa,v,0.085,t,f,0,f,g,00100,0,+
a,37.75,5.5,u,g,q,v,0.125,t,f,0,t,g,00228,0,+
b,23.25,4,u,g,c,bb,0.25,t,f,0,t,g,00160,0,+
b,18.08,5.5,u,g,k,v,0.5,t,f,0,f,g,00080,0,+
a,22.50,8.46,y,p,x,v,2.46,f,f,0,f,g,00164,0,+
b,19.67,0.375,u,g,q,v,2,t,t,02,t,g,00080,0,+
b,22.08,11,u,g,cc,v,0.665,t,f,0,f,g,00100,0,+
b,25.17,3.5,u,g,cc,v,0.625,t,t,07,f,g,00000,7059,+
a,47.42,3,u,g,x,v,13.875,t,t,02,t,g,00519,1704,+
b,33.50,1.75,u,g,x,h,4.5,t,t,04,t,g,00253,857,+
b,27.67,13.75,u,g,w,v,5.75,t,f,0,t,g,00487,500,+
a,58.42,21,u,g,i,bb,10,t,t,13,f,g,00000,6700,+
a,20.67,1.835,u,g,q,v,2.085,t,t,05,f,g,00220,2503,+
b,26.17,0.25,u,g,i,bb,0,t,f,0,t,g,00000,0,+
b,21.33,7.5,u,g,aa,v,1.415,t,t,01,f,g,00080,9800,+
b,42.83,4.625,u,g,q,v,4.58,t,f,0,f,s,00000,0,+
b,38.17,10.125,u,g,x,v,2.5,t,t,06,f,g,00520,196,+
b,20.50,10,y,p,c,v,2.5,t,f,0,f,s,00040,0,+
b,48.25,25.085,u,g,w,v,1.75,t,t,03,f,g,00120,14,+
b,28.33,5,u,g,w,v,11,t,f,0,t,g,00070,0,+
a,18.75,7.5,u,g,q,v,2.71,t,t,05,f,g,?,26726,+
b,18.50,2,u,g,i,v,1.5,t,t,02,f,g,00120,300,+
b,33.17,3.04,y,p,c,h,2.04,t,t,01,t,g,00180,18027,+
b,45.00,8.5,u,g,cc,h,14,t,t,01,t,g,00088,2000,+
a,19.67,0.21,u,g,q,h,0.29,t,t,11,f,g,00080,99,+
?,24.50,12.75,u,g,c,bb,4.75,t,t,02,f,g,00073,444,+
b,21.83,11,u,g,x,v,0.29,t,t,06,f,g,00121,0,+
b,40.25,21.5,u,g,e,z,20,t,t,11,f,g,00000,1200,+
b,41.42,5,u,g,q,h,5,t,t,06,t,g,00470,0,+
a,17.83,11,u,g,x,h,1,t,t,11,f,g,00000,3000,+
b,23.17,11.125,u,g,x,h,0.46,t,t,01,f,g,00100,0,+
b,?,0.625,u,g,k,v,0.25,f,f,0,f,g,00380,2010,-
b,18.17,10.25,u,g,c,h,1.085,f,f,0,f,g,00320,13,-
b,20.00,11.045,u,g,c,v,2,f,f,0,t,g,00136,0,-
b,20.00,0,u,g,d,v,0.5,f,f,0,f,g,00144,0,-
a,20.75,9.54,u,g,i,v,0.04,f,f,0,f,g,00200,1000,-
a,24.50,1.75,y,p,c,v,0.165,f,f,0,f,g,00132,0,-
b,32.75,2.335,u,g,d,h,5.75,f,f,0,t,g,00292,0,-
a,52.17,0,y,p,ff,ff,0,f,f,0,f,g,00000,0,-
a,48.17,1.335,u,g,i,o,0.335,f,f,0,f,g,00000,120,-
a,20.42,10.5,y,p,x,h,0,f,f,0,t,g,00154,32,-
b,50.75,0.585,u,g,ff,ff,0,f,f,0,f,g,00145,0,-
b,17.08,0.085,y,p,c,v,0.04,f,f,0,f,g,00140,722,-
b,18.33,1.21,y,p,e,dd,0,f,f,0,f,g,00100,0,-
a,32.00,6,u,g,d,v,1.25,f,f,0,f,g,00272,0,-
b,59.67,1.54,u,g,q,v,0.125,t,f,0,t,g,00260,0,+
b,18.00,0.165,u,g,q,n,0.21,f,f,0,f,g,00200,40,+
b,37.58,0,?,?,?,?,0,f,f,0,f,p,?,0,+
b,32.33,2.5,u,g,c,v,1.25,f,f,0,t,g,00280,0,-
b,18.08,6.75,y,p,m,v,0.04,f,f,0,f,g,00140,0,-
b,38.25,10.125,y,p,k,v,0.125,f,f,0,f,g,00160,0,-
b,30.67,2.5,u,g,cc,h,2.25,f,f,0,t,s,00340,0,-
b,18.58,5.71,u,g,d,v,0.54,f,f,0,f,g,00120,0,-
a,19.17,5.415,u,g,i,h,0.29,f,f,0,f,g,00080,484,-
a,18.17,10,y,p,q,h,0.165,f,f,0,f,g,00340,0,-
b,24.58,13.5,y,p,ff,ff,0,f,f,0,f,g,?,0,-
b,16.25,0.835,u,g,m,v,0.085,t,f,0,f,s,00200,0,-
b,21.17,0.875,y,p,c,h,0.25,f,f,0,f,g,00280,204,-
b,23.92,0.585,y,p,cc,h,0.125,f,f,0,f,g,00240,1,-
b,17.67,4.46,u,g,c,v,0.25,f,f,0,f,s,00080,0,-
a,16.50,1.25,u,g,q,v,0.25,f,t,01,f,g,00108,98,-
b,23.25,12.625,u,g,c,v,0.125,f,t,02,f,g,00000,5552,-
b,17.58,10,u,g,w,h,0.165,f,t,01,f,g,00120,1,-
a,?,1.5,u,g,ff,ff,0,f,t,02,t,g,00200,105,-
b,29.50,0.58,u,g,w,v,0.29,f,t,01,f,g,00340,2803,-
b,18.83,0.415,y,p,c,v,0.165,f,t,01,f,g,00200,1,-
a,21.75,1.75,y,p,j,j,0,f,f,0,f,g,00160,0,-
b,23.00,0.75,u,g,m,v,0.5,f,f,0,t,s,00320,0,-
a,18.25,10,u,g,w,v,1,f,t,01,f,g,00120,1,-
b,25.42,0.54,u,g,w,v,0.165,f,t,01,f,g,00272,444,-
b,35.75,2.415,u,g,w,v,0.125,f,t,02,f,g,00220,1,-
a,16.08,0.335,u,g,ff,ff,0,f,t,01,f,g,00160,126,-
a,31.92,3.125,u,g,ff,ff,3.04,f,t,02,t,g,00200,4,-
b,69.17,9,u,g,ff,ff,4,f,t,01,f,g,00070,6,-
b,32.92,2.5,u,g,aa,v,1.75,f,t,02,t,g,00720,0,-
b,16.33,2.75,u,g,aa,v,0.665,f,t,01,f,g,00080,21,-
b,22.17,12.125,u,g,c,v,3.335,f,t,02,t,g,00180,173,-
a,57.58,2,u,g,ff,ff,6.5,f,t,01,f,g,00000,10,-
b,18.25,0.165,u,g,d,v,0.25,f,f,0,t,s,00280,0,-
b,23.42,1,u,g,c,v,0.5,f,f,0,t,s,00280,0,-
a,15.92,2.875,u,g,q,v,0.085,f,f,0,f,g,00120,0,-
a,24.75,13.665,u,g,q,h,1.5,f,f,0,f,g,00280,1,-
b,48.75,26.335,y,p,ff,ff,0,t,f,0,t,g,00000,0,-
b,23.50,2.75,u,g,ff,ff,4.5,f,f,0,f,g,00160,25,-
b,18.58,10.29,u,g,ff,ff,0.415,f,f,0,f,g,00080,0,-
b,27.75,1.29,u,g,k,h,0.25,f,f,0,t,s,00140,0,-
a,31.75,3,y,p,j,j,0,f,f,0,f,g,00160,20,-
a,24.83,4.5,u,g,w,v,1,f,f,0,t,g,00360,6,-
b,19.00,1.75,y,p,c,v,2.335,f,f,0,t,g,00112,6,-
a,16.33,0.21,u,g,aa,v,0.125,f,f,0,f,g,00200,1,-
a,18.58,10,u,g,d,v,0.415,f,f,0,f,g,00080,42,-
b,16.25,0,y,p,aa,v,0.25,f,f,0,f,g,00060,0,-
b,23.00,0.75,u,g,m,v,0.5,t,f,0,t,s,00320,0,-
b,21.17,0.25,y,p,c,h,0.25,f,f,0,f,g,00280,204,-
b,17.50,22,l,gg,ff,o,0,f,f,0,t,p,00450,100000,+
b,19.17,0,y,p,m,bb,0,f,f,0,t,s,00500,1,+
b,36.75,0.125,y,p,c,v,1.5,f,f,0,t,g,00232,113,+
b,21.25,1.5,u,g,w,v,1.5,f,f,0,f,g,00150,8,+
a,18.08,0.375,l,gg,cc,ff,10,f,f,0,t,s,00300,0,+
a,33.67,0.375,u,g,cc,v,0.375,f,f,0,f,g,00300,44,+
b,48.58,0.205,y,p,k,v,0.25,t,t,11,f,g,00380,2732,+
b,33.67,1.25,u,g,w,v,1.165,f,f,0,f,g,00120,0,-
a,29.50,1.085,y,p,x,v,1,f,f,0,f,g,00280,13,-
b,30.17,1.085,y,p,c,v,0.04,f,f,0,f,g,00170,179,-
?,40.83,3.5,u,g,i,bb,0.5,f,f,0,f,s,01160,0,-
b,34.83,2.5,y,p,w,v,3,f,f,0,f,s,00200,0,-
b,?,4,y,p,i,v,0.085,f,f,0,t,g,00411,0,-
b,20.42,0,?,?,?,?,0,f,f,0,f,p,?,0,-
a,33.25,2.5,y,p,c,v,2.5,f,f,0,t,g,00000,2,-
b,34.08,2.5,u,g,c,v,1,f,f,0,f,g,00460,16,-
a,25.25,12.5,u,g,d,v,1,f,f,0,t,g,00180,1062,-
b,34.75,2.5,u,g,cc,bb,0.5,f,f,0,f,g,00348,0,-
b,27.67,0.75,u,g,q,h,0.165,f,f,0,t,g,00220,251,-
b,47.33,6.5,u,g,c,v,1,f,f,0,t,g,00000,228,-
a,34.83,1.25,y,p,i,h,0.5,f,f,0,t,g,00160,0,-
a,33.25,3,y,p,aa,v,2,f,f,0,f,g,00180,0,-
b,28.00,3,u,g,w,v,0.75,f,f,0,t,g,00300,67,-
a,39.08,4,u,g,c,v,3,f,f,0,f,g,00480,0,-
b,42.75,4.085,u,g,aa,v,0.04,f,f,0,f,g,00108,100,-
b,26.92,2.25,u,g,i,bb,0.5,f,f,0,t,g,00640,4000,-
b,33.75,2.75,u,g,i,bb,0,f,f,0,f,g,00180,0,-
b,38.92,1.75,u,g,k,v,0.5,f,f,0,t,g,00300,2,-
b,62.75,7,u,g,e,z,0,f,f,0,f,g,00000,12,-
?,32.25,1.5,u,g,c,v,0.25,f,f,0,t,g,00372,122,-
b,26.75,4.5,y,p,c,bb,2.5,f,f,0,f,g,00200,1210,-
b,63.33,0.54,u,g,c,v,0.585,t,t,03,t,g,00180,0,-
b,27.83,1.5,u,g,w,v,2.25,f,t,01,t,g,00100,3,-
a,26.17,2,u,g,j,j,0,f,f,0,t,g,00276,1,-
b,22.17,0.585,y,p,ff,ff,0,f,f,0,f,g,00100,0,-
b,22.50,11.5,y,p,m,v,1.5,f,f,0,t,g,00000,4000,-
b,30.75,1.585,u,g,d,v,0.585,f,f,0,t,s,00000,0,-
b,36.67,2,u,g,i,v,0.25,f,f,0,t,g,00221,0,-
a,16.00,0.165,u,g,aa,v,1,f,t,02,t,g,00320,1,-
b,41.17,1.335,u,g,d,v,0.165,f,f,0,f,g,00168,0,-
a,19.50,0.165,u,g,q,v,0.04,f,f,0,t,g,00380,0,-
b,32.42,3,u,g,d,v,0.165,f,f,0,t,g,00120,0,-
a,36.75,4.71,u,g,ff,ff,0,f,f,0,f,g,00160,0,-
a,30.25,5.5,u,g,k,v,5.5,f,f,0,t,s,00100,0,-
b,23.08,2.5,u,g,ff,ff,0.085,f,f,0,t,g,00100,4208,-
b,26.83,0.54,u,g,k,ff,0,f,f,0,f,g,00100,0,-
b,16.92,0.335,y,p,k,v,0.29,f,f,0,f,s,00200,0,-
b,24.42,2,u,g,e,dd,0.165,f,t,02,f,g,00320,1300,-
b,42.83,1.25,u,g,m,v,13.875,f,t,01,t,g,00352,112,-
a,22.75,6.165,u,g,aa,v,0.165,f,f,0,f,g,00220,1000,-
b,39.42,1.71,y,p,m,v,0.165,f,f,0,f,s,00400,0,-
a,23.58,11.5,y,p,k,h,3,f,f,0,t,g,00020,16,-
b,21.42,0.75,y,p,r,n,0.75,f,f,0,t,g,00132,2,-
b,33.00,2.5,y,p,w,v,7,f,f,0,t,g,00280,0,-
b,26.33,13,u,g,e,dd,0,f,f,0,t,g,00140,1110,-
a,45.00,4.585,u,g,k,h,1,f,f,0,t,s,00240,0,-
b,26.25,1.54,u,g,w,v,0.125,f,f,0,f,g,00100,0,-
?,28.17,0.585,u,g,aa,v,0.04,f,f,0,f,g,00260,1004,-
a,20.83,0.5,y,p,e,dd,1,f,f,0,f,g,00260,0,-
b,28.67,14.5,u,g,d,v,0.125,f,f,0,f,g,00000,286,-
b,20.67,0.835,y,p,c,v,2,f,f,0,t,s,00240,0,-
b,34.42,1.335,u,g,i,bb,0.125,f,f,0,t,g,00440,4500,-
b,33.58,0.25,u,g,i,bb,4,f,f,0,t,s,00420,0,-
b,43.17,5,u,g,i,bb,2.25,f,f,0,t,g,00141,0,-
a,22.67,7,u,g,c,v,0.165,f,f,0,f,g,00160,0,-
a,24.33,2.5,y,p,i,bb,4.5,f,f,0,f,g,00200,456,-
a,56.83,4.25,y,p,ff,ff,5,f,f,0,t,g,00000,4,-
b,22.08,11.46,u,g,k,v,1.585,f,f,0,t,g,00100,1212,-
b,34.00,5.5,y,p,c,v,1.5,f,f,0,t,g,00060,0,-
b,22.58,1.5,y,p,aa,v,0.54,f,f,0,t,g,00120,67,-
b,21.17,0,u,g,c,v,0.5,f,f,0,t,s,00000,0,-
b,26.67,14.585,u,g,i,bb,0,f,f,0,t,g,00178,0,-
b,22.92,0.17,u,g,m,v,0.085,f,f,0,f,s,00000,0,-
b,15.17,7,u,g,e,v,1,f,f,0,f,g,00600,0,-
b,39.92,5,u,g,i,bb,0.21,f,f,0,f,g,00550,0,-
b,27.42,12.5,u,g,aa,bb,0.25,f,f,0,t,g,00720,0,-
b,24.75,0.54,u,g,m,v,1,f,f,0,t,g,00120,1,-
b,41.17,1.25,y,p,w,v,0.25,f,f,0,f,g,00000,195,-
a,33.08,1.625,u,g,d,v,0.54,f,f,0,t,g,00000,0,-
b,29.83,2.04,y,p,x,h,0.04,f,f,0,f,g,00128,1,-
a,23.58,0.585,y,p,ff,ff,0.125,f,f,0,f,g,00120,87,-
b,26.17,12.5,y,p,k,h,1.25,f,f,0,t,g,00000,17,-
b,31.00,2.085,u,g,c,v,0.085,f,f,0,f,g,00300,0,-
b,20.75,5.085,y,p,j,v,0.29,f,f,0,f,g,00140,184,-
b,28.92,0.375,u,g,c,v,0.29,f,f,0,f,g,00220,140,-
a,51.92,6.5,u,g,i,bb,3.085,f,f,0,t,g,00073,0,-
a,22.67,0.335,u,g,q,v,0.75,f,f,0,f,s,00160,0,-
b,34.00,5.085,y,p,i,bb,1.085,f,f,0,t,g,00480,0,-
a,69.50,6,u,g,ff,ff,0,f,f,0,f,s,00000,0,-
a,40.33,8.125,y,p,k,v,0.165,f,t,02,f,g,?,18,-
a,19.58,0.665,y,p,c,v,1,f,t,01,f,g,02000,2,-
b,16.00,3.125,u,g,w,v,0.085,f,t,01,f,g,00000,6,-
b,17.08,0.25,u,g,q,v,0.335,f,t,04,f,g,00160,8,-
b,31.25,2.835,u,g,ff,ff,0,f,t,05,f,g,00176,146,-
b,25.17,3,u,g,c,v,1.25,f,t,01,f,g,00000,22,-
a,22.67,0.79,u,g,i,v,0.085,f,f,0,f,g,00144,0,-
b,40.58,1.5,u,g,i,bb,0,f,f,0,f,s,00300,0,-
b,22.25,0.46,u,g,k,v,0.125,f,f,0,t,g,00280,55,-
a,22.25,1.25,y,p,ff,ff,3.25,f,f,0,f,g,00280,0,-
b,22.50,0.125,y,p,k,v,0.125,f,f,0,f,g,00200,70,-
b,23.58,1.79,u,g,c,v,0.54,f,f,0,t,g,00136,1,-
b,38.42,0.705,u,g,c,v,0.375,f,t,02,f,g,00225,500,-
a,26.58,2.54,y,p,ff,ff,0,f,f,0,t,g,00180,60,-
b,35.00,2.5,u,g,i,v,1,f,f,0,t,g,00210,0,-
b,20.42,1.085,u,g,q,v,1.5,f,f,0,f,g,00108,7,-
b,29.42,1.25,u,g,w,v,1.75,f,f,0,f,g,00200,0,-
b,26.17,0.835,u,g,cc,v,1.165,f,f,0,f,g,00100,0,-
b,33.67,2.165,u,g,c,v,1.5,f,f,0,f,p,00120,0,-
b,24.58,1.25,u,g,c,v,0.25,f,f,0,f,g,00110,0,-
a,27.67,2.04,u,g,w,v,0.25,f,f,0,t,g,00180,50,-
b,37.50,0.835,u,g,e,v,0.04,f,f,0,f,g,00120,5,-
b,49.17,2.29,u,g,ff,ff,0.29,f,f,0,f,g,00200,3,-
b,33.58,0.335,y,p,cc,v,0.085,f,f,0,f,g,00180,0,-
b,51.83,3,y,p,ff,ff,1.5,f,f,0,f,g,00180,4,-
b,22.92,3.165,y,p,c,v,0.165,f,f,0,f,g,00160,1058,-
b,21.83,1.54,u,g,k,v,0.085,f,f,0,t,g,00356,0,-
b,25.25,1,u,g,aa,v,0.5,f,f,0,f,g,00200,0,-
b,58.58,2.71,u,g,c,v,2.415,f,f,0,t,g,00320,0,-
b,19.00,0,y,p,ff,ff,0,f,t,04,f,g,00045,1,-
b,19.58,0.585,u,g,ff,ff,0,f,t,03,f,g,00350,769,-
a,53.33,0.165,u,g,ff,ff,0,f,f,0,t,s,00062,27,-
a,27.17,1.25,u,g,ff,ff,0,f,t,01,f,g,00092,300,-
b,25.92,0.875,u,g,k,v,0.375,f,t,02,t,g,00174,3,-
b,23.08,0,u,g,k,v,1,f,t,11,f,s,00000,0,-
b,39.58,5,u,g,ff,ff,0,f,t,02,f,g,00017,1,-
b,30.58,2.71,y,p,m,v,0.125,f,f,0,t,s,00080,0,-
b,17.25,3,u,g,k,v,0.04,f,f,0,t,g,00160,40,-
a,17.67,0,y,p,j,ff,0,f,f,0,f,g,00086,0,-
a,?,11.25,u,g,ff,ff,0,f,f,0,f,g,?,5200,-
b,16.50,0.125,u,g,c,v,0.165,f,f,0,f,g,00132,0,-
a,27.33,1.665,u,g,ff,ff,0,f,f,0,f,g,00340,1,-
b,31.25,1.125,u,g,ff,ff,0,f,t,01,f,g,00096,19,-
b,20.00,7,u,g,c,v,0.5,f,f,0,f,g,00000,0,-
b,?,3,y,p,i,bb,7,f,f,0,f,g,00000,1,-
b,39.50,1.625,u,g,c,v,1.5,f,f,0,f,g,00000,316,-
b,36.50,4.25,u,g,q,v,3.5,f,f,0,f,g,00454,50,-
?,29.75,0.665,u,g,w,v,0.25,f,f,0,t,g,00300,0,-
b,52.42,1.5,u,g,d,v,3.75,f,f,0,t,g,00000,350,-
b,36.17,18.125,u,g,w,v,0.085,f,f,0,f,g,00320,3552,-
b,34.58,0,?,?,?,?,0,f,f,0,f,p,?,0,-
b,29.67,0.75,y,p,c,v,0.04,f,f,0,f,g,00240,0,-
b,36.17,5.5,u,g,i,bb,5,f,f,0,f,g,00210,687,-
b,25.67,0.29,y,p,c,v,1.5,f,f,0,t,g,00160,0,-
a,24.50,2.415,y,p,c,v,0,f,f,0,f,g,00120,0,-
b,24.08,0.875,u,g,m,v,0.085,f,t,04,f,g,00254,1950,-
b,21.92,0.5,u,g,c,v,0.125,f,f,0,f,g,00360,0,-
a,36.58,0.29,u,g,ff,ff,0,f,t,10,f,g,00200,18,-
a,23.00,1.835,u,g,j,j,0,f,t,01,f,g,00200,53,-
a,27.58,3,u,g,m,v,2.79,f,t,01,t,g,00280,10,-
b,31.08,3.085,u,g,c,v,2.5,f,t,02,t,g,00160,41,-
a,30.42,1.375,u,g,w,h,0.04,f,t,03,f,g,00000,33,-
b,22.08,2.335,u,g,k,v,0.75,f,f,0,f,g,00180,0,-
b,16.33,4.085,u,g,i,h,0.415,f,f,0,t,g,00120,0,-
a,21.92,11.665,u,g,k,h,0.085,f,f,0,f,g,00320,5,-
b,21.08,4.125,y,p,i,h,0.04,f,f,0,f,g,00140,100,-
b,17.42,6.5,u,g,i,v,0.125,f,f,0,f,g,00060,100,-
b,19.17,4,y,p,i,v,1,f,f,0,t,g,00360,1000,-
b,20.67,0.415,u,g,c,v,0.125,f,f,0,f,g,00000,44,-
b,26.75,2,u,g,d,v,0.75,f,f,0,t,g,00080,0,-
b,23.58,0.835,u,g,i,h,0.085,f,f,0,t,g,00220,5,-
b,39.17,2.5,y,p,i,h,10,f,f,0,t,s,00200,0,-
b,22.75,11.5,u,g,i,v,0.415,f,f,0,f,g,00000,0,-
?,26.50,2.71,y,p,?,?,0.085,f,f,0,f,s,00080,0,-
a,16.92,0.5,u,g,i,v,0.165,f,t,06,t,g,00240,35,-
b,23.50,3.165,y,p,k,v,0.415,f,t,01,t,g,00280,80,-
a,17.33,9.5,u,g,aa,v,1.75,f,t,10,t,g,00000,10,-
b,23.75,0.415,y,p,c,v,0.04,f,t,02,f,g,00128,6,-
b,34.67,1.08,u,g,m,v,1.165,f,f,0,f,s,00028,0,-
b,74.83,19,y,p,ff,ff,0.04,f,t,02,f,g,00000,351,-
b,28.17,0.125,y,p,k,v,0.085,f,f,0,f,g,00216,2100,-
b,24.50,13.335,y,p,aa,v,0.04,f,f,0,t,g,00120,475,-
b,18.83,3.54,y,p,ff,ff,0,f,f,0,t,g,00180,1,-
?,45.33,1,u,g,q,v,0.125,f,f,0,t,g,00263,0,-
@//E*O*F crx.data//
chmod u=rw,g=,o= crx.data
 
echo x - crx.names
sed 's/^@//' > "crx.names" <<'@//E*O*F crx.names//'
|  This file concerns credit card applications.  All attribute names
|  and values have been changed to meaningless symbols to protect
|  confidentiality of the data.
|  
|  Title:
|  	Credit Approval
|  
|  Sources:
|  	(confidential)
|  	Submitted by quinlan@cs.su.oz.au
|  
|  Past Usage:
|  	See Quinlan,
|	* "Simplifying decision trees", Int J Man-Machine Studies 27,
|	   Dec 1987, pp. 221-234.
|       * "C4.5: Programs for Machine Learning", Morgan Kaufmann, Oct 1992
|  
|  Relevant Information:
|  	This dataset is interesting because there is a good mix of
|  	attributes -- continuous, discrete with small numbers of
|  	values, and discrete with larger numbers of values.  There
|  	are also a few missing values.
|  
|  Instances:
|  	690
|  
|  Attributes:
|  	15
|  
|  Missing Attribute Values:
|  	37 cases (5%) have one or more missing values.  The missing
|  	values from particular attributes are:
|  		A1:  12
|  		A2:  12
|  		A4:   6
|  		A5:   6
|  		A6:   9
|  		A7:   9
|  		A14: 13
|  
|  Class Distribution:
|  	+: 307 (44.5%)
|  	-: 383 (55.5%)


+, -.	| classes

A1:	b, a. |type: A
A2:	continuous. |type: B
A3:	continuous. |type: B
A4:	u, y, l, t. |type: C
A5:	g, p, gg. |type: D
A6:	c, d, cc, i, j, k, m, r, q, w, x, e, aa, ff. |type: E
A7:	v, h, bb, j, n, z, dd, ff, o. |type: E
A8:	continuous. |type: B
A9:	t, f. |type: F
A10:	t, f. |type: F
A11:	continuous. |type: B
A12:	t, f. |type: F
A13:	g, p, s. |type: G
A14:	continuous. |type: H
A15:	continuous. |type: H
@//E*O*F crx.names//
chmod u=rw,g=w,o=w crx.names
 
echo x - crx.test
sed 's/^@//' > "crx.test" <<'@//E*O*F crx.test//'
a,47.25,0.75,u,g,q,h,2.75,t,t,01,f,g,00333,892,+
b,24.17,0.875,u,g,q,v,4.625,t,t,02,t,g,00520,2000,+
b,39.25,9.5,u,g,m,v,6.5,t,t,14,f,g,00240,4607,+
a,20.50,11.835,u,g,c,h,6,t,f,0,f,g,00340,0,+
a,18.83,4.415,y,p,c,h,3,t,f,0,f,g,00240,0,+
b,19.17,9.5,u,g,w,v,1.5,t,f,0,f,g,00120,2206,+
a,25.00,0.875,u,g,x,h,1.04,t,f,0,t,g,00160,5860,+
b,20.17,9.25,u,g,c,v,1.665,t,t,03,t,g,00040,28,+
b,25.75,0.5,u,g,c,v,1.46,t,t,05,t,g,00312,0,+
b,20.42,7,u,g,c,v,1.625,t,t,03,f,g,00200,1391,+
b,?,4,u,g,x,v,5,t,t,03,t,g,00290,2279,+
b,39.00,5,u,g,cc,v,3.5,t,t,10,t,g,00000,0,+
a,64.08,0.165,u,g,ff,ff,0,t,t,01,f,g,00232,100,+
b,28.25,5.125,u,g,x,v,4.75,t,t,02,f,g,00420,7,+
a,28.75,3.75,u,g,c,v,1.085,t,t,01,t,g,00371,0,+
b,31.33,19.5,u,g,c,v,7,t,t,16,f,g,00000,5000,+
a,18.92,9,u,g,aa,v,0.75,t,t,02,f,g,00088,591,+
a,24.75,3,u,g,q,h,1.835,t,t,19,f,g,00000,500,+
a,30.67,12,u,g,c,v,2,t,t,01,f,g,00220,19,+
b,21.00,4.79,y,p,w,v,2.25,t,t,01,t,g,00080,300,+
b,13.75,4,y,p,w,v,1.75,t,t,02,t,g,00120,1000,+
a,46.00,4,u,g,j,j,0,t,f,0,f,g,00100,960,+
a,44.33,0,u,g,c,v,2.5,t,f,0,f,g,00000,0,+
b,20.25,9.96,u,g,e,dd,0,t,f,0,f,g,00000,0,+
b,22.67,2.54,y,p,c,h,2.585,t,f,0,f,g,00000,0,+
b,?,10.5,u,g,x,v,6.5,t,f,0,f,g,00000,0,+
a,60.92,5,u,g,aa,v,4,t,t,04,f,g,00000,99,+
b,16.08,0.75,u,g,c,v,1.75,t,t,05,t,g,00352,690,+
a,28.17,0.375,u,g,q,v,0.585,t,t,04,f,g,00080,0,+
b,39.17,1.71,u,g,x,v,0.125,t,t,05,t,g,00480,0,+
?,20.42,7.5,u,g,k,v,1.5,t,t,01,f,g,00160,234,+
a,30.00,5.29,u,g,e,dd,2.25,t,t,05,t,g,00099,500,+
b,22.83,3,u,g,m,v,1.29,t,t,01,f,g,00260,800,+
a,22.50,8.5,u,g,q,v,1.75,t,t,10,f,g,00080,990,-
a,28.58,1.665,u,g,q,v,2.415,t,f,0,t,g,00440,0,-
b,45.17,1.5,u,g,c,v,2.5,t,f,0,t,g,00140,0,-
b,41.58,1.75,u,g,k,v,0.21,t,f,0,f,g,00160,0,-
a,57.08,0.335,u,g,i,bb,1,t,f,0,t,g,00252,2197,-
a,55.75,7.08,u,g,k,h,6.75,t,t,03,t,g,00100,50,-
b,43.25,25.21,u,g,q,h,0.21,t,t,01,f,g,00760,90,-
a,25.33,2.085,u,g,c,h,2.75,t,f,0,t,g,00360,1,-
a,24.58,0.67,u,g,aa,h,1.75,t,f,0,f,g,00400,0,-
b,43.17,2.25,u,g,i,bb,0.75,t,f,0,f,g,00560,0,-
b,40.92,0.835,u,g,ff,ff,0,t,f,0,f,g,00130,1,-
b,31.83,2.5,u,g,aa,v,7.5,t,f,0,t,g,00523,0,-
a,33.92,1.585,y,p,ff,ff,0,t,f,0,f,g,00320,0,-
a,24.92,1.25,u,g,ff,ff,0,t,f,0,f,g,00080,0,-
b,35.25,3.165,u,g,x,h,3.75,t,f,0,t,g,00680,0,-
b,34.25,1.75,u,g,w,bb,0.25,t,f,0,t,g,00163,0,-
b,80.25,5.5,u,g,?,?,0.54,t,f,0,f,g,00000,340,-
b,19.42,1.5,y,p,cc,v,2,t,f,0,t,g,00100,20,-
b,42.75,3,u,g,i,bb,1,t,f,0,f,g,00000,200,-
b,19.67,10,y,p,k,h,0.835,t,f,0,t,g,00140,0,-
b,36.33,3.79,u,g,w,v,1.165,t,f,0,t,g,00200,0,-
b,30.08,1.04,y,p,i,bb,0.5,t,t,10,t,g,00132,28,-
b,44.25,11,y,p,d,v,1.5,t,f,0,f,s,00000,0,-
b,23.58,0.46,y,p,w,v,2.625,t,t,06,t,g,00208,347,-
b,23.92,1.5,u,g,d,h,1.875,t,t,06,f,g,00200,327,+
b,33.17,1,u,g,x,v,0.75,t,t,07,t,g,00340,4071,+
b,48.33,12,u,g,m,v,16,t,f,0,f,s,00110,0,+
b,76.75,22.29,u,g,e,z,12.75,t,t,01,t,g,00000,109,+
b,51.33,10,u,g,i,bb,0,t,t,11,f,g,00000,1249,+
b,34.75,15,u,g,r,n,5.375,t,t,09,t,g,00000,134,+
b,38.58,3.335,u,g,w,v,4,t,t,14,f,g,00383,1344,+
a,22.42,11.25,y,p,x,h,0.75,t,t,04,f,g,00000,321,+
b,41.92,0.42,u,g,c,h,0.21,t,t,06,f,g,00220,948,+
b,29.58,4.5,u,g,w,v,7.5,t,t,02,t,g,00330,0,+
a,32.17,1.46,u,g,w,v,1.085,t,t,16,f,g,00120,2079,+
b,51.42,0.04,u,g,x,h,0.04,t,f,0,f,g,00000,3000,+
a,22.83,2.29,u,g,q,h,2.29,t,t,07,t,g,00140,2384,+
a,25.00,12.33,u,g,cc,h,3.5,t,t,06,f,g,00400,458,+
b,26.75,1.125,u,g,x,h,1.25,t,f,0,f,g,00000,5298,+
b,23.33,1.5,u,g,c,h,1.415,t,f,0,f,g,00422,200,+
b,24.42,12.335,u,g,q,h,1.585,t,f,0,t,g,00120,0,+
b,42.17,5.04,u,g,q,h,12.75,t,f,0,t,g,00092,0,+
a,20.83,3,u,g,aa,v,0.04,t,f,0,f,g,00100,0,+
b,23.08,11.5,u,g,w,h,2.125,t,t,11,t,g,00290,284,+
a,25.17,2.875,u,g,x,h,0.875,t,f,0,f,g,00360,0,+
b,43.08,0.375,y,p,c,v,0.375,t,t,08,t,g,00300,162,+
a,35.75,0.915,u,g,aa,v,0.75,t,t,04,f,g,00000,1583,+
b,59.50,2.75,u,g,w,v,1.75,t,t,05,t,g,00060,58,+
b,21.00,3,y,p,d,v,1.085,t,t,08,t,g,00160,1,+
b,21.92,0.54,y,p,x,v,0.04,t,t,01,t,g,00840,59,+
a,65.17,14,u,g,ff,ff,0,t,t,11,t,g,00000,1400,+
a,20.33,10,u,g,c,h,1,t,t,04,f,g,00050,1465,+
b,32.25,0.165,y,p,c,h,3.25,t,t,01,t,g,00432,8000,+
b,30.17,0.5,u,g,c,v,1.75,t,t,11,f,g,00032,540,+
b,25.17,6,u,g,c,v,1,t,t,03,f,g,00000,0,+
b,39.17,1.625,u,g,c,v,1.5,t,t,10,f,g,00186,4700,+
b,39.08,6,u,g,m,v,1.29,t,t,05,t,g,00108,1097,+
b,31.67,0.83,u,g,x,v,1.335,t,t,08,t,g,00303,3290,+
b,41.00,0.04,u,g,e,v,0.04,f,t,01,f,s,00560,0,+
b,48.50,4.25,u,g,m,v,0.125,t,f,0,t,g,00225,0,+
b,32.67,9,y,p,w,h,5.25,t,f,0,t,g,00154,0,+
a,28.08,15,y,p,e,z,0,t,f,0,f,g,00000,13212,+
b,73.42,17.75,u,g,ff,ff,0,t,f,0,t,g,00000,0,+
b,64.08,20,u,g,x,h,17.5,t,t,09,t,g,00000,1000,+
b,51.58,15,u,g,c,v,8.5,t,t,09,f,g,00000,0,+
b,26.67,1.75,y,p,c,v,1,t,t,05,t,g,00160,5777,+
b,25.33,0.58,u,g,c,v,0.29,t,t,07,t,g,00096,5124,+
b,30.17,6.5,u,g,cc,v,3.125,t,t,08,f,g,00330,1200,+
b,27.00,0.75,u,g,c,h,4.25,t,t,03,t,g,00312,150,+
b,23.17,0,?,?,?,?,0,f,f,0,f,p,?,0,+
b,34.17,5.25,u,g,w,v,0.085,f,f,0,t,g,00290,6,+
b,38.67,0.21,u,g,k,v,0.085,t,f,0,t,g,00280,0,+
b,25.75,0.75,u,g,c,bb,0.25,t,f,0,f,g,00349,23,+
a,46.08,3,u,g,c,v,2.375,t,t,08,t,g,00396,4159,+
a,21.50,6,u,g,aa,v,2.5,t,t,03,f,g,00080,918,+
?,20.08,0.125,u,g,q,v,1,f,t,01,f,g,00240,768,+
b,20.50,2.415,u,g,c,v,2,t,t,11,t,g,00200,3000,+
a,29.50,0.46,u,g,k,v,0.54,t,t,04,f,g,00380,500,+
?,42.25,1.75,y,p,?,?,0,f,f,0,t,g,00150,1,-
b,29.83,1.25,y,p,k,v,0.25,f,f,0,f,g,00224,0,-
b,20.08,0.25,u,g,q,v,0.125,f,f,0,f,g,00200,0,-
b,23.42,0.585,u,g,c,h,0.085,t,f,0,f,g,00180,0,-
a,29.58,1.75,y,p,k,v,1.25,f,f,0,t,g,00280,0,-
b,16.17,0.04,u,g,c,v,0.04,f,f,0,f,g,00000,0,+
b,32.33,3.5,u,g,k,v,0.5,f,f,0,t,g,00232,0,-
b,?,0.04,y,p,d,v,4.25,f,f,0,t,g,00460,0,-
b,47.83,4.165,u,g,x,bb,0.085,f,f,0,t,g,00520,0,-
b,20.00,1.25,y,p,k,v,0.125,f,f,0,f,g,00140,4,-
b,27.58,3.25,y,p,q,h,5.085,f,t,02,t,g,00369,1,-
b,22.00,0.79,u,g,w,v,0.29,f,t,01,f,g,00420,283,-
b,19.33,10.915,u,g,c,bb,0.585,f,t,02,t,g,00200,7,-
a,38.33,4.415,u,g,c,v,0.125,f,f,0,f,g,00160,0,-
b,29.42,1.25,u,g,c,h,0.25,f,t,02,t,g,00400,108,-
b,22.67,0.75,u,g,i,v,1.585,f,t,01,t,g,00400,9,-
b,32.25,14,y,p,ff,ff,0,f,t,02,f,g,00160,1,-
b,29.58,4.75,u,g,m,v,2,f,t,01,t,g,00460,68,-
b,18.42,10.415,y,p,aa,v,0.125,t,f,0,f,g,00120,375,-
b,22.17,2.25,u,g,i,v,0.125,f,f,0,f,g,00160,10,-
b,22.67,0.165,u,g,c,j,2.25,f,f,0,t,s,00000,0,+
a,25.58,0,?,?,?,?,0,f,f,0,f,p,?,0,+
b,18.83,0,u,g,q,v,0.665,f,f,0,f,g,00160,1,-
b,21.58,0.79,y,p,cc,v,0.665,f,f,0,f,g,00160,0,-
b,23.75,12,u,g,c,v,2.085,f,f,0,f,s,00080,0,-
b,22.00,7.835,y,p,i,bb,0.165,f,f,0,t,g,?,0,-
b,36.08,2.54,u,g,ff,ff,0,f,f,0,f,g,00000,1000,-
b,29.25,13,u,g,d,h,0.5,f,f,0,f,g,00228,0,-
a,19.58,0.665,u,g,w,v,1.665,f,f,0,f,g,00220,5,-
a,22.92,1.25,u,g,q,v,0.25,f,f,0,t,g,00120,809,-
a,27.25,0.29,u,g,m,h,0.125,f,t,01,t,g,00272,108,-
a,38.75,1.5,u,g,ff,ff,0,f,f,0,f,g,00076,0,-
b,32.42,2.165,y,p,k,ff,0,f,f,0,f,g,00120,0,-
a,23.75,0.71,u,g,w,v,0.25,f,t,01,t,g,00240,4,-
b,18.17,2.46,u,g,c,n,0.96,f,t,02,t,g,00160,587,-
b,40.92,0.5,y,p,m,v,0.5,f,f,0,t,g,00130,0,-
b,19.50,9.585,u,g,aa,v,0.79,f,f,0,f,g,00080,350,-
b,28.58,3.625,u,g,aa,v,0.25,f,f,0,t,g,00100,0,-
b,35.58,0.75,u,g,k,v,1.5,f,f,0,t,g,00231,0,-
b,34.17,2.75,u,g,i,bb,2.5,f,f,0,t,g,00232,200,-
?,33.17,2.25,y,p,cc,v,3.5,f,f,0,t,g,00200,141,-
b,31.58,0.75,y,p,aa,v,3.5,f,f,0,t,g,00320,0,-
a,52.50,7,u,g,aa,h,3,f,f,0,f,g,00000,0,-
b,36.17,0.42,y,p,w,v,0.29,f,f,0,t,g,00309,2,-
b,37.33,2.665,u,g,cc,v,0.165,f,f,0,t,g,00000,501,-
a,20.83,8.5,u,g,c,v,0.165,f,f,0,f,g,00000,351,-
b,24.08,9,u,g,aa,v,0.25,f,f,0,t,g,00000,0,-
b,25.58,0.335,u,g,k,h,3.5,f,f,0,t,g,00340,0,-
a,35.17,3.75,u,g,ff,ff,0,f,t,06,f,g,00000,200,-
b,48.08,3.75,u,g,i,bb,1,f,f,0,f,g,00100,2,-
a,15.83,7.625,u,g,q,v,0.125,f,t,01,t,g,00000,160,-
a,22.50,0.415,u,g,i,v,0.335,f,f,0,t,s,00144,0,-
b,21.50,11.5,u,g,i,v,0.5,t,f,0,t,g,00100,68,-
a,23.58,0.83,u,g,q,v,0.415,f,t,01,t,g,00200,11,-
a,21.08,5,y,p,ff,ff,0,f,f,0,f,g,00000,0,-
b,25.67,3.25,u,g,c,h,2.29,f,t,01,t,g,00416,21,-
a,38.92,1.665,u,g,aa,v,0.25,f,f,0,f,g,00000,390,-
a,15.75,0.375,u,g,c,v,1,f,f,0,f,g,00120,18,-
a,28.58,3.75,u,g,c,v,0.25,f,t,01,t,g,00040,154,-
b,22.25,9,u,g,aa,v,0.085,f,f,0,f,g,00000,0,-
b,29.83,3.5,u,g,c,v,0.165,f,f,0,f,g,00216,0,-
a,23.50,1.5,u,g,w,v,0.875,f,f,0,t,g,00160,0,-
b,32.08,4,y,p,cc,v,1.5,f,f,0,t,g,00120,0,-
b,31.08,1.5,y,p,w,v,0.04,f,f,0,f,s,00160,0,-
b,31.83,0.04,y,p,m,v,0.04,f,f,0,f,g,00000,0,-
a,21.75,11.75,u,g,c,v,0.25,f,f,0,t,g,00180,0,-
a,17.92,0.54,u,g,c,v,1.75,f,t,01,t,g,00080,5,-
b,30.33,0.5,u,g,d,h,0.085,f,f,0,t,s,00252,0,-
b,51.83,2.04,y,p,ff,ff,1.5,f,f,0,f,g,00120,1,-
b,47.17,5.835,u,g,w,v,5.5,f,f,0,f,g,00465,150,-
b,25.83,12.835,u,g,cc,v,0.5,f,f,0,f,g,00000,2,-
a,50.25,0.835,u,g,aa,v,0.5,f,f,0,t,g,00240,117,-
?,29.50,2,y,p,e,h,2,f,f,0,f,g,00256,17,-
a,37.33,2.5,u,g,i,h,0.21,f,f,0,f,g,00260,246,-
a,41.58,1.04,u,g,aa,v,0.665,f,f,0,f,g,00240,237,-
a,30.58,10.665,u,g,q,h,0.085,f,t,12,t,g,00129,3,-
b,19.42,7.25,u,g,m,v,0.04,f,t,01,f,g,00100,1,-
a,17.92,10.21,u,g,ff,ff,0,f,f,0,f,g,00000,50,-
a,20.08,1.25,u,g,c,v,0,f,f,0,f,g,00000,0,-
b,19.50,0.29,u,g,k,v,0.29,f,f,0,f,g,00280,364,-
b,27.83,1,y,p,d,h,3,f,f,0,f,g,00176,537,-
b,17.08,3.29,u,g,i,v,0.335,f,f,0,t,g,00140,2,-
b,36.42,0.75,y,p,d,v,0.585,f,f,0,f,g,00240,3,-
b,40.58,3.29,u,g,m,v,3.5,f,f,0,t,s,00400,0,-
b,21.08,10.085,y,p,e,h,1.25,f,f,0,f,g,00260,0,-
a,22.67,0.75,u,g,c,v,2,f,t,02,t,g,00200,394,-
a,25.25,13.5,y,p,ff,ff,2,f,t,01,t,g,00200,1,-
b,17.92,0.205,u,g,aa,v,0.04,f,f,0,f,g,00280,750,-
b,35.00,3.375,u,g,c,h,8.29,f,f,0,t,g,00000,0,-
@//E*O*F crx.test//
chmod u=rw,g=,o= crx.test
 
echo x - defns.i
sed 's/^@//' > "defns.i" <<'@//E*O*F defns.i//'
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <sys/types.h>


/******************************************************************************/
/*									      */
/*	Type definitions, including synonyms for structure pointers	      */
/*									      */
/******************************************************************************/


typedef unsigned char Var;

typedef char	Boolean,
		Ordering;	/* valid instances are <,=,>,# */


				/* A tuple is stored as an array of int / fp.

				   * T[0] = tuple number
						+ POSMARK (pos tuples)
				   * T[i] = constant number, or fp value

				   NB: If continuous (fp) values are used,
				       type Const must be the same size as
				       float --if this is not the case, change
				       Const to long.

				   Relation tuple sets are indexed by 3D array.

				   * I[i][j][k] = no. of kth tuple with T[j] = i

				   The last entry is followed by FINISH  */


typedef int			Const,
				*Tuple;

typedef int			***Index;

typedef struct _rel_rec		*Relation;

typedef struct _type_rec	*TypeInfo;

typedef struct _state_rec	State;

typedef struct _lit_rec		*Literal,
				**Clause;

typedef struct _arg_ord_rec	*ArgOrder;

typedef struct _poss_lit_rec	*PossibleLiteral;

typedef struct _backup_rec	*Alternative;

typedef struct _var_rec		*VarInfo;


/******************************************************************************/
/*									      */
/*	Structure definitions						      */
/*									      */
/******************************************************************************/

	/* State */

	/*  A state represents information about the search for a new
	    clause, including the tuples that satisfy the clause and
	    various counts for the clause and the tuples  */

struct _state_rec
{
	int		MaxVar,		/* highest variable */
			NPos,		/* number of pos tuples */
			NTot,		/* number of all tuples */
			NOrigPos,	/* original pos tuples covered */
			NOrigTot;	/* original tuples covered */
	Tuple		*Tuples;	/* training set */
	float		BaseInfo;	/* information per pos tuple */
};

	/* Literal */

struct _lit_rec
{
	char		Sign;		/* 0=negated, 1=pos, 2=determinate */
	Relation	Rel;
	Var		*Args;
	int		WeakLits;	/* value up to this literal */
	Ordering	*ArgOrders,	/* recursive lits: =, <, >, # */
			*SaveArgOrders;	/* copy during pruning */
	float		Bits;		/* encoding length */
};

	/* Relation */

	/* Note:

	   Relations are represented by the (positive) tuples they contain;
	   if the closed world assumption is not in force, negative tuples
	   known not to be in the relation can be given explicitly.

	   A key specifies a sensible way that a relation can be accessed
	   by noting which arguments must have bound values.  There can
	   be any number of keys; if there are none, all possible ways
	   of accessing the relation are ok. */
	   
struct _rel_rec
{
	char		*Name;
	int		Arity,		/* number of arguments */
			NKeys,		/* number of keys (0=all ok) */
			*Key,		/* keys, each packed in an int */
			*Type;		/* types of arguments */
	TypeInfo	*TypeRef;	/* redundant pointers to types */
	Tuple		*Pos,		/* positive tuples */
			*Neg;		/* negative tuples or Nil (CWA) */
	Index		PosIndex,	/* index for positive tuples */
			NegIndex;	/* ditto for explicit negative tuples */
	int		PosDuplicates,	/* number of duplicate pos tuples*/
			NTrialArgs,	/* number of args to try (gain) */
			NTried;		/* number of them evaluated */
	Clause		*Def;		/* definition is array of clauses */
	Boolean		BinSym,		/* true for binary symmetric relns */
			PossibleTarget,
			**PosOrder,	/* argument order info for R() */
			**NegOrder,	/*    "       "    "    " ~R() */
			*ArgNotEq;	/* args that cannot be the same var
					   (indexed by ArgPair) */
	float		Bits;		/* current encoding cost */
};

	/* Type */

struct _type_rec
{	char		*Name;		/* type name */
	Boolean		Continuous,	/* continuous (non-discrete) */
			Ordered,	/* ordered discrete type */
			FixedPolarity;
	int		NValues,	/* number of discrete constants */
			NTheoryConsts;  /* number of theory constants */
	Const		*Value,		/* constants */
			*TheoryConst;	/* theory constants */
	int		*CollSeq;	/* CollSeq[k] = x if (global) constant
					   k is x'th const of this type */
};


	/* Possible argument order -- used in discovering constant orders */

struct _arg_ord_rec
{
	Relation	Rel;		/* relation */
	Boolean		Sign;		/* sign */
	int		A1, A2,		/* A1 < A2 or A1 > A2 */
			In;		/* 0, -1 (reverse) or +1 */
};


	/* Structures used in backing up search */

struct _poss_lit_rec
{
	Relation	Rel;
	Boolean		Sign;
	Var		*Args;
	float		Gain,
			Bits;
	int		WeakLits,
			NewSize,
			TotCov,
			PosCov;
};

struct _backup_rec
{
	float		Value;
	Clause		UpToHere;
};


	/* Variables */

struct _var_rec
{
	char		*Name;
	int		Type,
			Depth;
	TypeInfo	TypeRef;
}; 


#define  FP(X)			(*((float *)(&X)))

	/*  The first four relations are predefined comparisons with
	    aliased names defined here  */

#define  EQVAR			Reln[0]		/* var = var */
#define  EQCONST		Reln[1]		/* var = theory const */
#define  GTVAR			Reln[2]		/* var > var */
#define  GTCONST		Reln[3]		/* var > threshold */

#define  Predefined(R)		(R==EQVAR||R==EQCONST||R==GTVAR||R==GTCONST)
#define  HasConst(R)		(R==EQCONST||R==GTCONST)

#define  Verbose(x)		if (VERBOSITY >= x)

#define  LN2	 		0.693147
#define  Log2(x)		(log((float) x)/LN2)
#define  Log2e			1.44269
#define  Log2sqrt2Pi		1.32575
#define  LogComb(n,r)		(Log2Fact(n) - Log2Fact(r) - Log2Fact((n)-(r)))
#define  Except(n,e)		((n) ? (1.1*(Log2(n) + LogComb(n, e))) : 0.0)
#define  Encode(n)		Except(AllTuples, n)

#define  Nil			0
#define  false			0 
#define  true			1 
#define  Max(a,b)		( (a)>(b) ? a : b ) 
#define  Min(a,b)		( (a)<(b) ? a : b ) 

#define  ForEach(V,First,Last)	for(V=First;V<=Last;V++) 

#define  Mask			 077777777
#define  PosMark		0100000000	/* Max tuples = 16.7M */
#define  Positive(T)		((T)[0]&PosMark)

#define  TrueBit		02
#define  FalseBit		01
#define  SetFlag(A,B)		Flags[A] |= B
#define  TestFlag(A,B)		(Flags[A] & B)
#define  ClearFlags		memset(Flags,0,StartDef.NTot)

#define  BestLitGain		(NPossible ? Possible[1]->Gain : 0.0)

#define  MonitorWeakLits(W)	if (W) NWeakLits++; else NWeakLits=0

#define  Plural(n)		((n) != 1 ? "s" : "")
#define  ReadToEOLN		while ( getchar() != '\n' )

#define  Alloc(N,T)		(T *) pmalloc((N)*sizeof(T))
#define  AllocZero(N,T)		(T *) pcalloc(N, sizeof(T))
#define  Realloc(V,N,T)		V = (T *) prealloc(V, (N)*sizeof(T))

#define  MissingValue(R,A,X)	(MissingVals && MissingVal(R,A,X))
#define  ungetchar(A)		ungetc(A, stdin)

#define	 ArgPair(A2,A1)		(((A2-1)*(A2-2))/2 + A1-1)

	/*  The following are used to pack and unpack parameters into
	    argument lists.  AV must be the address of an int or float  */

#define  SaveParam(A,AV)	memcpy(A,AV,sizeof(Const))
#define  GetParam(A,AV)		memcpy(AV,A,sizeof(Const))



/******************************************************************************/
/*									      */
/*	Various constants						      */
/*									      */
/******************************************************************************/


#define  FINISH	   10000000	/* large constant used as a terminator */

#define	 UNBOUND    0357357	/* odd marker used in interpret.c, join.c */

#define  MISSING_DISC     1	/* missing value "?" is the first constant */

#define  MISSING_FP 0.03125	/* arbitrary number used as the floating
				   point equivalent of MISSING_DISC - if
				   it clashes with a genuine data value, just
				   change this */

#define	 OUT_OF_RANGE	  2	/* denotes constant outside closed world */



/******************************************************************************/
/*									      */
/*	Synopsis of functions						      */
/*									      */
/******************************************************************************/


	/* main.c */

void	  main(int Argc, char *Argv[]);

	/* utility.c */

void	  *pmalloc(unsigned arg);
void	  *prealloc(void * arg1, unsigned arg2);
void	  *pcalloc(unsigned arg1, unsigned arg2);
void	  pfree(void *arg);
float	  CPUTime();

	  /* input.c */

Boolean	  ReadType();
void	  ReadTypes();
Tuple	  ReadTuple(Relation R);
Tuple	  *ReadTuples(Relation R, Boolean Pos);
Relation  ReadRelation();
void	  ReadRelations();
int	  FindType(char *N);
char	  *CopyString(char *s);
void	  Error();
void	  DuplicateTuplesCheck(Relation R);
int	  CountDuplicates(Tuple *T, int N, int Left, int Right);
Boolean	  SymmetryCheck(Relation R);
char	  ReadName(char *s);
Const	  FindConstant(char *N, Boolean MustBeThere);
int	  Number(Tuple *T);
void	  CheckTypeCompatibility();
Boolean	  CommonValue(int N1, Const *V1, int N2, Const *V2);
Index	  MakeIndex(Tuple *T, int N, Relation R);
void	  UnequalArgsCheck(Relation R);
Boolean	  NeverEqual(Tuple *T, Var F, Var S);

	  /* output.c */

void	  PrintTuple(Tuple C, int N, TypeInfo *TypeRef, Boolean ShowPosNeg);
void	  PrintTuples(Tuple *TT, int N);
void	  PrintSpecialLiteral(Relation R, Boolean RSign, Var *A);
void	  PrintComposedLiteral(Relation R, Boolean RSign, Var *A);
void	  PrintLiteral(Literal L);
void	  PrintClause(Relation R, Clause C);
void	  PrintSimplifiedClause(Relation R, Clause C);
void	  Substitute(char *Old, char *New);
void	  PrintDefinition(Relation R);

	  /* literal.c */

void	  ExploreArgs(Relation R, Boolean CountFlag);
Boolean	  AcceptableKey(Relation R, int Key);
Boolean   Repetitious(Relation R, Var *A);
Boolean   SameArgs(int N, Var *A1, int MV1, Var *A2, int MV2, int LitN);
void	  ExploreEQVAR();
void	  ExploreEQCONST();
void	  ExploreGTVAR();
void	  ExploreGTCONST();
Boolean	  TryArgs(Relation R, int This, int HiVar, int FreeVars, int MaxDepth,
	  	    int Key, Boolean TryMostGeneral, Boolean RecOK);
int	  EstimatePossibleArgs(int TNo);

	  /* evaluatelit.c */

void	  EvaluateLiteral(Relation R, Var *A, float LitBits, Boolean *Prune);
void	  PrepareForScan(Relation R, Var *A);
float	  NegThresh(int P, int P1);
Boolean	  TerminateScan(Relation R, Var *A);
Boolean	  Satisfies(int RN, Const V, Const W, Tuple Case);
void	  CheckForPrune(Relation R, Var *A);
void	  CheckNewVars(Tuple Case);
float	  Worth(int N, int P, int T, int UV);
float	  Info(int P, int T);
Boolean	  MissingVal(Relation R, Var *A, Tuple T);
Boolean	  Unknown(Var V, Tuple T);
void	  FindThreshold(Var *A);
void	  PossibleCut(float C);
int	  MissingAndSort(Var V, int Fp, int Lp);
void	  Quicksort(Tuple *Vec, int Fp, int Lp, Var V);

	  /* join.c */

Boolean	  Join(Tuple *T, Index TIX, Var *A, Tuple C, int N, Boolean YesOrNo);

	  /* state.c */

void	  OriginalState(Relation R);
void	  AddTuple(int N, Tuple T, int ByteSize, int Mark);
Tuple	  NextConstTuple(Relation R, Tuple Case);
void	  RandomTuple(Relation R, Tuple Result);
void	  NewState(Literal L, int NewSize);
void	  FormNewState(Relation R, Boolean RSign, Var *A, int NewSize);
void	  AcceptNewState(Relation R, Var *A, int NewSize);
void	  RecoverState(Clause C);
void	  CheckSize(int SoFar, int Extra, int *NewSize, Tuple **TSAddr);
Tuple	  Extend(Tuple Case, Tuple Binding, Var *A, int N);
void	  CheckOriginalCaseCover();
void	  FreeTuples(Tuple *TT, Boolean TuplesToo);
double	  Log2Fact(int n);

	  /* determinate.c */

Boolean	  GoodDeterminateLiteral(Relation R, Var *A, float LitBits);
void	  ProcessDeterminateLiterals(Boolean AllWeak);
Boolean	  SameVar(Var A, Var B);
void	  ShiftVarsDown(int s);

	  /* search.c */

void	  ProposeLiteral(Relation R, Boolean TF, Var *A,
	  		   int Size, float LitBits, int OPos, int OTot,
	  		   float Gain, Boolean Weak);
Boolean	  Recover();
void	  Remember(Literal L, int OPos, int OTot);
Literal	  MakeLiteral(int i);
Literal	  SelectLiteral();
void	  FreeLiteral(Literal L);
void	  FreeClause(Clause C);

	  /* order.c */

void	  ExamineVariableRelationships();
Boolean	  RecursiveCallOK(Var *A);
void	  AddOrders(Literal L);
void	  NoteRecursiveLit(Literal L);

	  /* finddef.c */

void	  FindDefinition(Relation R);
Clause	  FindClause();
void	  ExamineLiterals();
void	  GrowNewClause();
Boolean	  SameLiteral(Relation R, Boolean Sign, Var *A);
Boolean	  AllLHSVars(Literal L);
Boolean	  AllDeterminate();
float	  CodingCost(Clause C);

	  /* prune.c */

void	  PruneNewClause();
void	  CheckVariables();
Boolean	  TheoryConstant(Const C, TypeInfo T);
Boolean	  ConstantVar(Var V, Const C);
Boolean	  IdenticalVars(Var V, Var W);
Boolean	  Known(Relation R, Var V, Var W);
void	  Insert(Var V, Relation R, Var A1, Const A2);
Boolean	  Contains(Var *A, int N, Var V);
Boolean	  QuickPrune(Clause C, Var MaxBound, Boolean *Used);
Boolean	  SatisfactoryNewClause(int Errs);
void	  RenameVariables();
Boolean	  RedundantLiterals(int ErrsNow);
Boolean	  EssentialBinding(int LitNo);
void	  ReplaceVariable(Var Old, Var New);
void	  SiftClauses();
Var	  HighestVarInDefinition(Relation R);
Boolean	  Recursive(Clause C);

	  /* interpret.c */

Boolean	  CheckRHS(Clause C);
Boolean	  Interpret(Relation R, Tuple Case);
void	  InitialiseValues(Tuple Case, int N);

	  /* constants.c */

void	  OrderConstants();
void	  FindArgumentOrders();
void	  ExamineArgumentPairs(Relation R, Boolean Sign, Tuple *TP);
Boolean	  ConsistentClosure(Boolean **Table, Tuple *TP, Var A, Var B);
Boolean	  AddPair(Boolean **Table, int A, int B);
void	  AddArgOrder(Relation R, Boolean Sig, int A1, int A2);
Boolean	  **AllocatePartOrd(int Size);
void	  FreePartOrd(Boolean **PO, int Size);
void	  ClearPartOrd(Boolean **PO);
void	  FindConsistentSubset(int Included, int TryNext, Boolean **PO);
int	  CountEntries(int K);
@//E*O*F defns.i//
chmod u=rw,g=,o= defns.i
 
echo x - determinate.c
sed 's/^@//' > "determinate.c" <<'@//E*O*F determinate.c//'
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
@//E*O*F determinate.c//
chmod u=rw,g=,o= determinate.c
 
echo x - evaluatelit.c
sed 's/^@//' > "evaluatelit.c" <<'@//E*O*F evaluatelit.c//'
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
@//E*O*F evaluatelit.c//
chmod u=rw,g=,o= evaluatelit.c
 
echo x - extern.i
sed 's/^@//' > "extern.i" <<'@//E*O*F extern.i//'
/******************************************************************************/
/*									      */
/*	Variables defined in global.c					      */
/*									      */
/******************************************************************************/


extern Boolean
	NEGLITERALS,
	NEGEQUALS,
	UNIFORMCODING,

	MissingVals,
	AnyPartialOrder,
	*Barred,
	OutOfWorld;


extern float
	SAMPLE,
	MINACCURACY,
	MINALTFRAC,
	DETERMINATE;

extern int
	MAXVARS,
	MAXARGS,
	MAXWEAKLITS,

	MAXPOSSLIT,
	MAXALTS,
	MAXVARDEPTH,
	MAXRECOVERS,
	MAXTUPLES,
	VERBOSITY,

	MaxConst,
	MaxType,
	MaxRel,

	AllTuples,
	NCl,
	NLit,
	NDetLits,
	NWeakLits,
	SavedClauseCover;


extern PossibleLiteral
	AlterSavedClause;


extern char
	**ConstName,
	*Flags;


extern Relation
	*Reln,
	*RelnOrder,
	Target;


extern State
	StartDef,
	StartClause,
	Current,
	New;


extern float
	*LogFact,
	MaxPossibleGain,
	ClauseBits,
	AvailableBits,
	AdjMinAccuracy,
	SavedClauseAccuracy;


extern Clause
	NewClause,
	SavedClause;

extern Boolean
	**PartialOrder,
	**Compatible;

extern Ordering
	**RecursiveLitOrders;

extern int
	NRecLitClause,
	NRecLitDef;

extern VarInfo
	*Variable;

extern Var
	*DefaultVars;

extern TypeInfo
	*Type;

extern Tuple		*Found;
extern int		NFound;

extern Alternative	*ToBeTried;
extern int		NToBeTried;

extern PossibleLiteral	*Possible;
extern int		NPossible,
			NDeterminate;
@//E*O*F extern.i//
chmod u=rw,g=,o= extern.i
 
echo x - finddef.c
sed 's/^@//' > "finddef.c" <<'@//E*O*F finddef.c//'
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
@//E*O*F finddef.c//
chmod u=rw,g=,o= finddef.c
 
echo x - global.c
sed 's/^@//' > "global.c" <<'@//E*O*F global.c//'
#include "defns.i"

/******************************************************************************/
/*									      */
/*	Parameters set by options and variables accessible to many routines   */
/*									      */
/******************************************************************************/


Boolean
	NEGLITERALS   = true,	/* negated literals ok */
	NEGEQUALS     = true,	/* negated equality literals ok */
	UNIFORMCODING = false,	/* uniform coding of literals */

	MissingVals   = false,	/* missing values in input? */
	AnyPartialOrder,	/* quick check to rule out recursive lits */
	*Barred,		/* duplicate variables */
	OutOfWorld;		/* flag ^ constant */


float
	SAMPLE	    = 1.0,	/* fraction of negative tuples to sample */
	MINACCURACY = 0.8,	/* minimum acceptable clause accuracy */
	MINALTFRAC  = 0.8,	/* fraction of best gain required for backup */
	DETERMINATE = 0.8;  	/* use determinate literals unless a literal
				   with this fraction of max possible gain */

int
	MAXVARS	    = 52,	/* max number of variables */
	MAXARGS     = 5,	/* max arity of any relation */
	MAXWEAKLITS = 3,	/* max weak literals in sequence */

	MAXPOSSLIT  = 5,	/* 1 + max backups from single state */
	MAXALTS     = 20,	/* max simultaneous backups */
	MAXRECOVERS,		/* max total backups */
	MAXVARDEPTH = 4,	/* max depth of var in literal */
	MAXTUPLES   = 100000,	/* max number of tuples */
	VERBOSITY   = 1,	/* level of output */

	MaxConst    = 0,	/* no. constants */
	MaxType	    = 0,	/* no. types */
	MaxRel	    = 0,	/* highest relation no */

	AllTuples,		/* effective size of universe */
	NCl,			/* current clause number */
	NLit,			/* current literal number */
	NDetLits,		/* number of determinate lits in clause */
	NWeakLits,		/* current weak lits in sequence */
	SavedClauseCover;	/* coverage of saved clause  */

char
	**ConstName,		/* names of all discrete constants */
	*Flags = Nil;		/* flag bits for original tuples */


Relation
	*Reln,			/* relations */
	*RelnOrder,      	/* order to try relations */
	Target;			/* relation being induced */


State
	StartDef,		/* at start of definition */
	StartClause,		/* at start of clause */
	Current,		/* current state */
	New;			/* possible next state */


float
	*LogFact = Nil,		/* LogFact[i] = log2(i!) */
	MaxPossibleGain,
	ClauseBits,		/* bits used so far in this clause */
	AvailableBits,		/* bits available for this clause */
	AdjMinAccuracy,		/* min accuracy adjusted for sampling */
	SavedClauseAccuracy;	/* accuracy of saved clause */


Clause
	NewClause,		/* clause being constructed */
	SavedClause;		/* best shorter clause discovered
				   while developing current clause */

PossibleLiteral
	AlterSavedClause;	/* last literal of saved clause */


Boolean
	**PartialOrder,		/* partial orders on variables*/
	**Compatible;		/* Compatible[i][j] = true if types i, j have
				   at least one common value  */

Ordering
	**RecursiveLitOrders;	/* pointers to orders in recursive lits  */

int
	NRecLitClause,		/* number of recursive lits in the new clause */
	NRecLitDef;		/* ditto in definition so far */

VarInfo
	*Variable;		/* variables */

Var
	*DefaultVars;		/* default variable list */

TypeInfo
	*Type;			/* types */

Tuple		*Found;		/* join */
int		NFound;		/* number of tuples in join */

Alternative	*ToBeTried;	/* backup points */
int		NToBeTried;

PossibleLiteral	*Possible;	/* possible literals */
int		NPossible,
		NDeterminate;
@//E*O*F global.c//
chmod u=rw,g=,o= global.c
 
echo x - input.c
sed 's/^@//' > "input.c" <<'@//E*O*F input.c//'
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
@//E*O*F input.c//
chmod u=rw,g=,o= input.c
 
echo x - interpret.c
sed 's/^@//' > "interpret.c" <<'@//E*O*F interpret.c//'
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
@//E*O*F interpret.c//
chmod u=rw,g=,o= interpret.c
 
echo x - join.c
sed 's/^@//' > "join.c" <<'@//E*O*F join.c//'
#include  "defns.i"
#include  "extern.i"


	/*  Given tuples T with index TIX, find the tuples that satisfy
	    the column and same-value constraints on case C.  Leave the
	    tuples in Found with their number in NFound  */

	/*  NB: Foil spends a large proportion of its execution time in
	    this single routine.  For that reason, the code has been
	    written for speed (on a DECstation), even though this has
	    reduced its clarity.  If using different hardware, it would
	    probably be worth rewriting this  */

int FoundSize = 0;


Boolean  Join(Tuple *T, Index TIX, Var *A, Tuple C, int N, Boolean YesOrNo)
/*       ----  */
{
    static int	*Pair,			/* Pair[i], Pair[i+1] 
					   are paired variables */
		*Contin,		/* Contin[i] is a continuous variable
					   that must have the given value */
		**Next;			/* Next[i] = next tuple obeying 
					   ith constraint */
    static Boolean *Checked;

    int		*MaxPair,		/* highest pair of same variables */
		*PairPtr,
		*MaxContin,		/* highest continuous variable */
		*ContinPtr,
		MaxCol=0,		/* highest column constraint */
    		MaxNo=0,		/* index numbers in relation */
		**NextPtr,
		**LastNext,
		V, Val, i, j;

    Boolean NoCols;
    Tuple Candidate;

    /*  Allocate arrays first time through  */

    if ( ! FoundSize )
    {
	Pair = Alloc(2*(MAXVARS+1), int);
	Next = Alloc(MAXVARS+1, int *);
	Checked = Alloc(MAXVARS+1, Boolean);

	Contin = Alloc(MAXVARS+1, int);

	FoundSize = 20000;
	Found = Alloc(FoundSize+1, Tuple);
    }

    MaxPair   = Pair;
    MaxContin = Contin;

    NFound = 0;
    OutOfWorld = false;

    /*  Set the column constraints and find pairs of free variables that
	must be the same  */

    memset(Checked+1, 0, N);

    ForEach(i, 1, N)
    {
	/*  If this variable is bound, record a constraint; otherwise see if
	    it is the same as another unbound variable  */

	if ( (V = A[i]) <= Current.MaxVar && (Val = C[V]) != UNBOUND )
	{
	    if ( Variable[V]->TypeRef->Continuous ) *MaxContin++ = i;
	    else
	    if ( ! (Next[MaxCol++] = TIX[i][Val]) ) return false;
	}
	else
	if ( ! Checked[i] )
	{
	    ForEach(j, i+1, N)
	    {
		if ( A[j] == V )
		{
		    *MaxPair++ = i;
		    *MaxPair++ = j;
		    Checked[j] = true;
		}
	    }
	}
    }
    NoCols = MaxCol-- <= 0;
    LastNext = Next + MaxCol;

    while ( true )
    {
	/*  Advance all columns to MaxNo  */

	for ( NextPtr = Next ; NextPtr <= LastNext ; )
	{
	    while ( **NextPtr < MaxNo ) (*NextPtr)++;

	    MaxNo = **NextPtr++;
	}

	if ( MaxNo == FINISH || NoCols && ! T[MaxNo] )
	{
	    Found[NFound] = Nil;
	    return (NFound > 0);
	}
	else
	if ( NoCols || MaxNo == *Next[0] )
	{
	    /*  Found one possibility  -- check same variable constraints  */

	    Candidate = T[MaxNo];

	    for ( PairPtr = Pair ;
		  PairPtr < MaxPair 
                  && Candidate[*PairPtr] == Candidate[*(PairPtr+1)] ;
		  PairPtr += 2 )
		;

	    for ( ContinPtr = Contin ;
		  ContinPtr < MaxContin
		  && Candidate[*ContinPtr] == C[ A[*ContinPtr] ] ;
		  ContinPtr++ )
		;

	    if ( PairPtr >= MaxPair && ContinPtr >= MaxContin )
	    {
		if ( YesOrNo ) return true;

		if ( NFound >= FoundSize )
		{
		    FoundSize += 20000;
		    Realloc(Found, FoundSize+1, Tuple);
		}

		Found[NFound++] = Candidate;

		/*  Check for falling off the end of the (closed) world  */

		for ( i = 1 ; ! OutOfWorld && i <= N ; i++ )
		{
		    if ( Candidate[i] == OUT_OF_RANGE )
		    {
			OutOfWorld = true;
			Found[NFound] = Nil;
			return true;
		    }
		}
	    }

	    MaxNo++;
	}
    }
}
@//E*O*F join.c//
chmod u=rw,g=,o= join.c
 
echo x - literal.c
sed 's/^@//' > "literal.c" <<'@//E*O*F literal.c//'
/******************************************************************************/
/*									      */
/*	Examine the space of possible literals on a relation		      */
/*									      */
/******************************************************************************/


#include "defns.i"
#include "extern.i"

#define		MAXTIME		100

Var		*Arg = Nil;	/* potential arguments */
Boolean		CountOnly;
float		StartTime;	/* CPU time at start of literal */
int		Ticks,		/* calls of TryArgs() */
                TicksCheck;     /* point to check time again */


	/*  Examine possible variable assignments for next literal using
	    relation R.  If CountOnly specified, this will only set the
	    number of them in R->NTrialArgs; otherwise, it will evaluate
	    the possible arguments  */

void  ExploreArgs(Relation R, Boolean CountFlag)
/*    -----------  */
{
    int	MaxArgs=1;
    Var	V;

    CountOnly = CountFlag;
    if ( CountOnly )
    {
	R->NTrialArgs = 0;
    }
    else
    {
	R->NTried = 0;
    }

    if ( R == Target && ! AnyPartialOrder ) return;

    if ( Predefined(R) )
    {
	if ( R == EQVAR )
	{
	    ExploreEQVAR();
	}
	else
	if ( R == EQCONST )
	{
	    ExploreEQCONST();
	}
	else
	if ( R == GTVAR )
	{
	    ExploreGTVAR();
	}
	else
	if ( R == GTCONST )
	{
	    ExploreGTCONST();
	}

	return;
    }

    if ( CountOnly )
    {
	/*  Carry out a preliminary feasibility check  */

	for ( V = 1 ; MaxArgs <= 1E7 && V <= R->Arity ; V++ )
	{
	    MaxArgs *= EstimatePossibleArgs(R->Type[V]);
	}

	if ( MaxArgs > 1E7 )
	{
	    Verbose(2)
	    {
		printf("\t\t\t\t[%s: too many possibilities]\n", R->Name);
	    }
	    R->NTrialArgs = 0;
	    return;
	}
    }
    else
    {
	Ticks      = 0;
	TicksCheck = 10;
	StartTime  = CPUTime();
    }

    TryArgs(R, 1, Current.MaxVar, 0, 0, 0, true, false);
}



int  EstimatePossibleArgs(int TNo)
/*   --------------------  */
{
    int Sum=1, V;

    ForEach(V, 1, Current.MaxVar)
    {
	if ( Compatible[Variable[V]->Type][TNo] ) Sum++;
    }

    return Sum;
}



	/*  Determine whether a key is acceptable for the relation being
	    explored.  Note that keys are packed bit strings with a 1
	    wherever there is an unbound variable  */


Boolean  AcceptableKey(Relation R, int Key)
/*	 -------------  */
{
    int i;

    if ( ! R->NKeys ) return true;

    ForEach(i, 0, R->NKeys-1)
    {
	if ( (R->Key[i] | Key) == R->Key[i] ) return true;
    }

    return false;
}



	/*  See whether a potential literal is actually a repeat of a literal
	    already in the clause (with perhaps different free variables)  */


Boolean  Repetitious(Relation R, Var *A)
/*       -----------  */
{
    Literal	L;
    Var		V, MaxV;
    int		i, a, N;

    MaxV = Target->Arity;

    ForEach(i, 0, NLit-1)
    {
	L = NewClause[i];
	if ( L->Rel == R &&
	     SameArgs(R->Arity, A, Current.MaxVar, L->Args, MaxV, i+1) )
	{
	    return true;
	}

	if ( L->Sign )
	{
	    N = L->Rel->Arity;
	    ForEach(a, 1, N)
	    {
		if ( (V = L->Args[a]) > MaxV ) MaxV = V;
	    }
	}
    }

    return false;
}



Boolean  Mentioned(Var V, int First)
/*       ---------  */
{
    int		i, a, N;
    Literal	L;

    ForEach(i, First, NLit-1)
    {
	L = NewClause[i];
	N = L->Rel->Arity;

	ForEach(a, 1, N)
	{
	    if ( L->Args[a] == V ) return true;
	}
    }

    return false;
}



	/*  Check whether two aruments are identical up to substitution
	    of free variables  */


Boolean  SameArgs(int N, Var *A1, int MV1, Var *A2, int MV2, int LN)
/*       --------  */
{
    int a;

    ForEach(a, 1, N)
    {
	if ( ( A1[a] <= MV1 ? A2[a] != A1[a] : 
			  ( A2[a]-MV2 != A1[a]-MV1 || Mentioned(A2[a], LN) ) ) )
	{
	    return false;
	}
    }

    return true;
}



	/*  Find arguments for predefined relations  */


void  ExploreEQVAR()
/*    ------------  */
{
    Var		V, W;

    ForEach(V, 1, Current.MaxVar-1)
    {
	if ( Barred[V] ) continue;

	Arg[1] = V;

	ForEach(W, V+1, Current.MaxVar)
	{
	    if ( Barred[W] ) continue;

	    if ( Compatible[Variable[V]->Type][Variable[W]->Type] )
	    {
		if ( CountOnly )
		{
		    EQVAR->NTrialArgs++;
		}
		else
		{
		    Arg[2] = W;
		    EvaluateLiteral(EQVAR, Arg, EQVAR->Bits, Nil);
		    EQVAR->NTried++;
		}
	    }
	}
    }
}

		

void  ExploreEQCONST()
/*    --------------  */
{
    Var		V;
    int		T, i, n;
    Const	C;
    Boolean	*VarBound;
    Literal	L;

    /*  Find variables that have bound values  */

    VarBound = AllocZero(Current.MaxVar+1, Boolean);
    ForEach(i, 0, NLit-1)
    {
	L = NewClause[i];
	if ( L->Rel == EQCONST && L->Sign )
	{
	    VarBound[L->Args[1]] = true;
	}
    }

    ForEach(V, 1, Current.MaxVar)
    {
	if ( Barred[V] || VarBound[V] ) continue;

	T = Variable[V]->Type;

	if ( n = Type[T]->NTheoryConsts )
	{
	    Arg[1] = V;

	    ForEach(i, 0, n-1)
	    {
		if ( CountOnly )
		{
		    EQCONST->NTrialArgs++;
		}
		else
		{
		    C = Type[T]->TheoryConst[i];
		    SaveParam(&Arg[2], &C);

		    EvaluateLiteral(EQCONST, Arg, EQCONST->Bits, Nil);
		    EQCONST->NTried++;
		}
	    }
	}
    }

    free(VarBound);
}



void  ExploreGTVAR()
/*    ------------  */
{
    Var		V, W;

    ForEach(V, 1, Current.MaxVar-1)
    {
	if ( Barred[V] || ! Variable[V]->TypeRef->Continuous ) continue;

	Arg[1] = V;

	ForEach(W, V+1, Current.MaxVar)
	{
	    if ( Barred[W] || Variable[W]->Type != Variable[V]->Type ) continue;

	    if ( CountOnly )
	    {
		GTVAR->NTrialArgs++;
	    }
	    else
	    {
		Arg[2] = W;
		EvaluateLiteral(GTVAR, Arg, GTVAR->Bits, Nil);
		GTVAR->NTried++;
	    }
	}
    }
}

		

void  ExploreGTCONST()
/*    --------------  */
{
    Var		V;
    float	Z=MISSING_FP;

    ForEach(V, 1, Current.MaxVar)
    {
	if ( Barred[V] || ! Variable[V]->TypeRef->Continuous ) continue;

	if ( CountOnly )
	{
	    GTCONST->NTrialArgs++;
	}
	else
	{
	    Arg[1] = V;
	    SaveParam(&Arg[2], &Z);

	    EvaluateLiteral(GTCONST, Arg, GTCONST->Bits, Nil);
	    GTCONST->NTried++;
	}
    }
}



    /*  Generate argument lists starting from position This.
	In the partial argument list Arg[1]..Arg[This-1],
	    HiVar is the highest variable (min value MaxVar)
	    FreeVars is the number of free variable occurrences
	    MaxDepth is the highest depth of a bound variable
	    Key gives the key so far
	TryMostGeneral is true when we need to fill all remaining argument
	positions with new free variables.
	RecOK is true when a more general argument list has been found to
	satisfy RecursiveCallOK(), so this must also.

	Return false if hit time limit.  */
	
	
Boolean  TryArgs(Relation R, int This, int HiVar, int FreeVars, int MaxDepth,
	      int Key, Boolean TryMostGeneral, Boolean RecOK)
/*       -------  */
{
    Var		V, W, MaxV;
    Boolean	Prune, UselessSameVar;
    int		NewFreeVars, NewMaxDepth, CopyKey;
    float       TimeSpent;


    /*  This version contains a time cutout to prevent too much effort
	begin invested in a single literal.  Unfortunately, direct
	monitoring of time is too expensive (in system time and overhead)
	so a more circuitous method that calls CPUTime() rarely is used  */

    if ( ! CountOnly )
    {
        if ( Ticks > TicksCheck )
	{
	    if ( (TimeSpent = CPUTime() - StartTime) > MAXTIME )
	    {
		return false;
	    }
	    else
	    if ( TimeSpent > 0.001 * MAXTIME )
	    {
		TicksCheck += 0.01 * Ticks * MAXTIME / TimeSpent;
	    }
	    else
	    {
		TicksCheck *= 10;
	    }
	}

	Ticks++;
    }

    /*  Try with all remaining positions (if any) as new free variables  */

    NewFreeVars = R->Arity - This + 1;

    if ( TryMostGeneral &&
	 HiVar + NewFreeVars <= MAXVARS	/* enough variables */ &&
	 FreeVars < This-1		/* at least one bound */ &&
	 ( ! NewFreeVars || MaxDepth < MAXVARDEPTH ) )
    {
	CopyKey = Key;
	ForEach(V, This, R->Arity)
	{
	    Arg[V] = W = HiVar + (V - This + 1);
	    CopyKey |= 1<<V;

	    Variable[W]->Type    = R->Type[V];
	    Variable[W]->TypeRef = R->TypeRef[V];
	}

	if ( AcceptableKey(R, CopyKey) &&
	     ( R != Target ||
	       RecOK ||
	       (RecOK = RecursiveCallOK(Arg)) ) )
	{
	    if ( CountOnly )
	    {
		R->NTrialArgs++;
	    }
	    else
	    if ( ! Repetitious(R, Arg) )
	    {
		EvaluateLiteral(R, Arg, R->Bits, &Prune);
		R->NTried++;

		if ( Prune && NewFreeVars )
		{
		    Verbose(3) printf("\t\t\t\t(pruning subsumed arguments)\n");
		    return true;
		}
	    }
	}
    }
		
    if ( This > R->Arity ) return true;

    /*  Now try substitutions recursively  */

    MaxV = ( Predefined(R) ? HiVar :
	     This < R->Arity && HiVar < MAXVARS ? HiVar+1 : HiVar );

    for ( V = 1 ; V <= MaxV && BestLitGain < MaxPossibleGain ; V++ )
    {
	if ( V <= Current.MaxVar )
	{
	    if ( Barred[V] ) continue;
	    NewMaxDepth = Max(MaxDepth, Variable[V]->Depth);
	    NewFreeVars = FreeVars;
	}
	else
	{
	    NewMaxDepth = MaxDepth;
	    NewFreeVars = FreeVars+1;
	}

	if ( V > HiVar )
	{
	    Variable[V]->Type    = R->Type[This];
	    Variable[V]->TypeRef = R->TypeRef[This];
	    Key |= 1<<This;
	    if ( ! AcceptableKey(R, Key) ) break;
	}

	/*  Check same variables where impossible  */

	UselessSameVar = false;
	ForEach(W, 1, This-1)
	{
	    UselessSameVar |= Arg[W] == V && R->ArgNotEq[ ArgPair(This,W) ];
	}

	if ( UselessSameVar ||
	     V <= HiVar && ! Compatible[Variable[V]->Type][R->Type[This]] ||
	     NewMaxDepth + (NewFreeVars > 0) > MAXVARDEPTH ||
	     R->BinSym && This == 2 && V < Arg[1] )
	{
	    continue;
	}

	Arg[This] = V;

	if ( ! TryArgs(R, This+1, Max(HiVar, V), NewFreeVars, NewMaxDepth,
		       Key, V <= HiVar, RecOK) ) return false;
    }

    return true;
}
@//E*O*F literal.c//
chmod u=rw,g=,o= literal.c
 
echo x - main.c
sed 's/^@//' > "main.c" <<'@//E*O*F main.c//'
#include  "defns.i"
#include  "extern.i"


	/*  Read parameters and initialise variables  */


void  main(int Argc, char *Argv[])
/*    ----  */
{
    int		o, i, Cases, Errors;
    extern char	*optarg;
    extern int	optind;
    Boolean	FirstTime=true;
    Var		V;
    extern Var	*Arg;	/* in literal.c */
    Relation	R;
    Tuple	Case;
    char	Line[200], PlusOrMinus;

    /* Check overlaying of Const and float */

    if ( sizeof(Const) != sizeof(float) )
    {
	printf("Integers and floating point numbers are different sizes\n");
	printf("Alter declaration of type Const (defns.i) and recompile\n");
	exit(1);
    }

    printf("FOIL 6.4   [January 1996]\n--------\n");

    /*  Process options  */

    while ( (o = getopt(Argc, Argv, "nNus:a:f:g:V:d:A:w:l:t:m:v:")) != EOF )
    {
	if ( FirstTime )
	{
	    printf("\n    Options:\n");
	    FirstTime = false;
	}

	switch (o)
	{
	    case 'n':	NEGLITERALS = NEGEQUALS = false;
			printf("\tno negated literals\n");
			break;

	    case 'N':	NEGLITERALS = false;
			printf("\tnegated literals only for =, > relations\n");
			break;

	    case 'u':   /*UNIFORMCODING = true;*/
	                printf("\tuniform coding of literals not available\n");
	                break;

	    case 's':	SAMPLE = atof(optarg);
			printf("\tsample %g%% of negative tuples\n", SAMPLE);
			SAMPLE /= 100;
			break;

	    case 'a':	MINACCURACY = atof(optarg);
			printf("\tminimum clause accuracy %g%%\n",MINACCURACY);
			MINACCURACY /= 100;
			break;

	    case 'f':	MINALTFRAC = atof(optarg);
			printf("\tbacked-up literals have %g%% of best gain\n",
				MINALTFRAC);
			MINALTFRAC /= 100;
			break;

	    case 'g':	i = atoi(optarg);
			printf("\tuse determinate literals when gain <= ");
	                printf("%d%% possible\n", i);
			DETERMINATE = i / 100.0;
			break;

	    case 'V':   MAXVARS = atoi(optarg);
			if ( MAXVARS > pow(2.0, 8*sizeof(Var)-1.0) )
			{
			    MAXVARS = pow(2.0, 8*sizeof(Var)-1.0) -0.9;
			}
                        printf("\tallow up to %d variables\n", MAXVARS);
	                break;

	    case 'd':	MAXVARDEPTH = atoi(optarg);
			printf("\tmaximum variable depth %d\n", MAXVARDEPTH);
			break;

	    case 'w':   MAXWEAKLITS = atoi(optarg);
                        printf("\tallow up to %d consecutive weak literals\n",
				MAXWEAKLITS);
	                break;

	    case 'l':	MAXPOSSLIT = atoi(optarg)+1;
			printf("\tmaximum %d backups from one literal\n", 
				MAXPOSSLIT-1);
			break;

	    case 't':	MAXALTS = atoi(optarg);
			printf("\tmaximum %d total backups\n", MAXALTS);
			break;

	    case 'm':   MAXTUPLES = atoi(optarg);
                        printf("\tmaximum %d tuples \n", MAXTUPLES);
	                break;

	    case 'v':	VERBOSITY = atoi(optarg);
			printf("\tverbosity level %d\n", VERBOSITY);
			break;

	    case '?':	printf("unrecognised option\n");
			exit(1);
	}
    }

    /*  Set up predefined relations.

	These are treated specially in Join().  Rather than giving explicit
	pos tuples, the Pos field identifies the relation  */

    /*  Note: EQCONST and GTCONST take one argument and one parameter
	(a theory constant or floating-point threshold).  To simplify the
	code for all other relations, this parameter is packed into a
	"standard" arglist; the number of variable positions that it
	occupies will depend on the implementation.  */

    Reln = Alloc(10, Relation);

    EQVAR = AllocZero(1, struct _rel_rec);

    EQVAR->Name = "=";
    EQVAR->Arity = 2;

    EQVAR->Type = AllocZero(3, int);
    EQVAR->TypeRef = AllocZero(3, TypeInfo);

    EQVAR->Pos = (Tuple *) 0;
    EQVAR->BinSym = true;

    EQCONST = AllocZero(1, struct _rel_rec);

    EQCONST->Name = "==";
    EQCONST->Arity = 1;

    EQCONST->Type = AllocZero(2, int);
    EQCONST->TypeRef = AllocZero(2, TypeInfo);

    EQCONST->Pos = (Tuple *) 1;
    EQCONST->BinSym = false;

    GTVAR = AllocZero(1, struct _rel_rec);

    GTVAR->Name = ">";
    GTVAR->Arity = 2;

    GTVAR->Type = AllocZero(3, int);
    GTVAR->TypeRef = AllocZero(3, TypeInfo);

    GTVAR->Pos = (Tuple *) 2;
    GTVAR->BinSym = true;

    GTCONST = AllocZero(1, struct _rel_rec);

    GTCONST->Name = ">";
    GTCONST->Arity = 1;

    GTCONST->Type = AllocZero(2, int);
    GTCONST->TypeRef = AllocZero(2, TypeInfo);

    GTCONST->Pos = (Tuple *) 3;
    GTCONST->BinSym = false;

    MaxRel = 3;

    ForEach(i, 0, MaxRel)
    {
	Reln[i]->PossibleTarget = false;
    }

    /*  Read input  */

    ReadTypes();

    CheckTypeCompatibility();

    ReadRelations();

    /*  Initialise all global variables that depend on parameters  */

    Variable    = Alloc(MAXVARS+1, VarInfo);
    DefaultVars = Alloc(MAXVARS+1, Var);

    ForEach(V, 0, MAXVARS)
    {
	Variable[V] = Alloc(1, struct _var_rec);

	if ( V == 0 )
	{
	    Variable[0]->Name = "_";
	}
	else
	if ( V <= 26 )
	{
	    Variable[V]->Name = Alloc(2, char);
	    Variable[V]->Name[0] = 'A' + V - 1;
	    Variable[V]->Name[1] = '\0';
	}
	else
	{
	    Variable[V]->Name = Alloc(3, char);
	    Variable[V]->Name[0] = 'A' + ((V-27) / 26);
	    Variable[V]->Name[1] = 'A' + ((V-27) % 26);
	    Variable[V]->Name[2] = '\0';
	}

	DefaultVars[V] = V;
    }

    Barred = AllocZero(MAXVARS+1, Boolean);

    ToBeTried = Alloc(MAXALTS+1, Alternative);
    ForEach(i, 0, MAXALTS)
    {
	ToBeTried[i] = Alloc(1, struct _backup_rec);
	ToBeTried[i]->UpToHere = Nil;
    }

    PartialOrder = Alloc(MAXARGS+1, Boolean *);
    ForEach(V, 1, MAXARGS)
    {
	PartialOrder[V] = Alloc(MAXVARS+1, Boolean);
    }

    Possible = Alloc(MAXPOSSLIT+1, PossibleLiteral);
    ForEach(i, 1, MAXPOSSLIT)
    {
	Possible[i] = Alloc(1, struct _poss_lit_rec);
	Possible[i]->Args = Alloc(MAXARGS+1, Var);
    }

    Arg = Alloc(MAXARGS+1, Var);
    Arg[0] = 0;	/* active */

    /*  Allocate space for trial recursive call */

    RecursiveLitOrders = Alloc(1, Ordering *);
    RecursiveLitOrders[0] = Alloc(MAXARGS+1, Ordering);

    /*  Find plausible orderings  for constants of each type  */

    OrderConstants();

    /* Find Definitions */

    ForEach(i, 0, MaxRel)
    {
	R = RelnOrder[i];

	if ( R->PossibleTarget )
	{
	    FindDefinition(R);
	}
    }

    /*  Test definitions  */

    while ( gets(Line) )
    {
	R = Nil;
	for ( i = 0 ; i <= MaxRel && ! R ; i++ )
	{
	    if ( ! strcmp(RelnOrder[i]->Name, Line) ) R = RelnOrder[i];
	}

	if ( ! R )
	{
	    printf("\nUnknown relation %s\n", Line);
	    exit(1);
	}
	else
	{
	    printf("\nTest relation %s\n", Line);
	}

	Cases = Errors = 0;
	Current.MaxVar = HighestVarInDefinition(R);

	while ( Case = ReadTuple(R) )
	{
	    while ( (PlusOrMinus = getchar()) == ' ' || PlusOrMinus == '\t' )
		;
	    ReadToEOLN;

	    Cases++;

	    if ( Interpret(R, Case) != ( PlusOrMinus == '+' ) )
	    {
		Verbose(1)
		{
		    printf("    (%c)", PlusOrMinus);
		    PrintTuple(Case, R->Arity, R->TypeRef, false);
		}
		Errors++;
	    }
	}

	printf("Summary: %d error%s in %d trial%s\n",
		Errors, Plural(Errors), Cases, Plural(Cases));
    }

    exit(0);
}
@//E*O*F main.c//
chmod u=rw,g=,o= main.c
 
echo x - member.d
sed 's/^@//' > "member.d" <<'@//E*O*F member.d//'
X: 1, 2, 3.
L: [111], [112], [113], [11], [121], [122], [123], [12], [131], [132],
   [133], [13], [1], [211], [212], [213], [21], [221], [222], [223],
   [22], [231], [232], [233], [23], [2], [311], [312], [313], [31],
   [321], [322], [323], [32], [331], [332], [333], [33], [3], *[].

member(X,L)
1,[1]
3,[3]
1,[11]
1,[13]
3,[13]
1,[31]
3,[31]
3,[33]
1,[111]
1,[113]
3,[113]
1,[131]
3,[131]
1,[133]
3,[133]
1,[311]
3,[311]
1,[313]
3,[313]
1,[331]
3,[331]
3,[333]
;
1,[]
1,[3]
1,[33]
1,[333]
3,[]
3,[1]
3,[11]
3,[111]
1,[2]
1,[22]
1,[23]
1,[32]
1,[222]
1,[223]
1,[232]
1,[233]
1,[322]
1,[323]
1,[332]
@.
*components(L,X,L) #--/-##
[1],1,[]
[2],2,[]
[3],3,[]
[11],1,[1]
[12],1,[2]
[13],1,[3]
[21],2,[1]
[22],2,[2]
[23],2,[3]
[31],3,[1]
[32],3,[2]
[33],3,[3]
[111],1,[11]
[112],1,[12]
[113],1,[13]
[121],1,[21]
[122],1,[22]
[123],1,[23]
[131],1,[31]
[132],1,[32]
[133],1,[33]
[211],2,[11]
[212],2,[12]
[213],2,[13]
[221],2,[21]
[222],2,[22]
[223],2,[23]
[231],2,[31]
[232],2,[32]
[233],2,[33]
[311],3,[11]
[312],3,[12]
[313],3,[13]
[321],3,[21]
[322],3,[22]
[323],3,[23]
[331],3,[31]
[332],3,[32]
[333],3,[33]
@.

member
2,[]: -
3,[121]: -
3,[23]: +
3,[232]: +
@.
@//E*O*F member.d//
chmod u=rw,g=,o= member.d
 
echo x - member.explain
sed 's/^@//' > "member.explain" <<'@//E*O*F member.explain//'
>>>>>>  This file contains comments interspersed with FOIL6.4 input and
>>>>>>  output.  Comments (even blank lines) begin with >>>>>>.
>>>>>>  
>>>>>>  The first section of input identifies the types and the constants
>>>>>>  associated with each type.  Here there are two types, X and L
>>>>>>  (corresponding to element and list).  Individual constant names
>>>>>>  can be complex; remember, though, that any structure implied by
>>>>>>  the name is invisible to FOIL.  The * in front of [] indicates
>>>>>>  that this is a 'theory constant' that can appear in the learned
>>>>>>  definition.  The blank line following the last type delineates
>>>>>>  this section from the next.
>>>>>>  
X: 1, 2, 3.
L: [111], [112], [113], [11], [121], [122], [123], [12], [131], [132],
   [133], [13], [1], [211], [212], [213], [21], [221], [222], [223],
   [22], [231], [232], [233], [23], [2], [311], [312], [313], [31],
   [321], [322], [323], [32], [331], [332], [333], [33], [3], *[].

>>>>>>  
>>>>>>  The second section defines relations by specifying the tuples of
>>>>>>  constants that they contain ('positive' tuples); tuples that do
>>>>>>  not belong in the relation can be listed explicitly or FOIL can
>>>>>>  determine them using the closed world assumption.  Here member
>>>>>>  is a relation with two arguments, an element and a list.  The
>>>>>>  pairs 1,[1] through 3,[333] are given as belonging to member
>>>>>>  while 1,[] through 1,[332] are noted as not belonging to member
>>>>>>  ('negative' tuples).
>>>>>>  
member(X,L)
1,[1]
3,[3]
1,[11]
1,[13]
3,[13]
1,[31]
3,[31]
3,[33]
1,[111]
1,[113]
3,[113]
1,[131]
3,[131]
1,[133]
3,[133]
1,[311]
3,[311]
1,[313]
3,[313]
1,[331]
3,[331]
3,[333]
;
1,[]
1,[3]
1,[33]
1,[333]
3,[]
3,[1]
3,[11]
3,[111]
1,[2]
1,[22]
1,[23]
1,[32]
1,[222]
1,[223]
1,[232]
1,[233]
1,[322]
1,[323]
1,[332]
@.
>>>>>>  
>>>>>>  The input then defines another relation components with three
>>>>>>  arguments: a list, an element (the head of the list) and another
>>>>>>  list (the tail of the list).  The asterisk in front of the
>>>>>>  relation name tells FOIL not to construct a definition for it.
>>>>>>  After the closing parenthesis there is a blank and the characters
>>>>>>  '#--/-##'.  This is a 'key' (similar to that used in databases);
>>>>>>  to use this relation, either the value of the first argument or
>>>>>>  the values of both the second and third argument must be known,
>>>>>>  meaning that the corresponding variables in the definition must be
>>>>>>  bound.
>>>>>>  
*components(L,X,L) #--/-##
[1],1,[]
[2],2,[]
[3],3,[]
[11],1,[1]
[12],1,[2]
[13],1,[3]
[21],2,[1]
[22],2,[2]
[23],2,[3]
[31],3,[1]
[32],3,[2]
[33],3,[3]
[111],1,[11]
[112],1,[12]
[113],1,[13]
[121],1,[21]
[122],1,[22]
[123],1,[23]
[131],1,[31]
[132],1,[32]
[133],1,[33]
[211],2,[11]
[212],2,[12]
[213],2,[13]
[221],2,[21]
[222],2,[22]
[223],2,[23]
[231],2,[31]
[232],2,[32]
[233],2,[33]
[311],3,[11]
[312],3,[12]
[313],3,[13]
[321],3,[21]
[322],3,[22]
[323],3,[23]
[331],3,[31]
[332],3,[32]
[333],3,[33]
@.

>>>>>>  
>>>>>>  Any number of relations may be defined.  The order does not matter,
>>>>>>  as FOIL will reorder them anyway.  This section also ends with a
>>>>>>  blank line.
>>>>>>  
>>>>>>  The final section contains cases to test the definitions found.
>>>>>>  The first line names the relation to be tested, followed by test
>>>>>>  tuples.  Each tuple ends with a colon, space, and a sign that
>>>>>>  indicates whether or not the tuple is supposed to satisfy the
>>>>>>  definition.  The test parcel ends with a line containing a period;
>>>>>>  there can be any number of such parcels.
>>>>>>  
member
2,[]: -
3,[121]: -
3,[23]: +
3,[232]: +
@.
>>>>>>  
>>>>>>  Let us now examine the output produced by FOIL for this example.
>>>>>>  The command used to run FOIL was 'foil6 -n -v3 <member.d', where
>>>>>>  -n excludes negated literals and -v3 selects fairly verbose output.
>>>>>>  
FOIL 6.4   [January 1996]
--------

    Options:
	no negated literals
	verbosity level 3

Types X and L are not compatible

Relation member

Relation *components

>>>>>>  
>>>>>>  After reading the relations, FOIL tries to find ordering for
>>>>>>  the constants of types X and L.  It fails to find an ordering
>>>>>>  for type X, but discovers one for type L -- the original
>>>>>>  unhelpful lexicographic order (produced by 'sort') has been
>>>>>>  replaced by a length ordering.
>>>>>>  
Ordering constants of type X
	Checking arguments of member
	Checking arguments of ~member
	Checking arguments of components
		unordered

Ordering constants of type L
	Checking arguments of member
	Checking arguments of ~member
	Checking arguments of components
		arguments 1,3 are consistent
	Finding maximal consistent set
		best so far components:1>3
	Final order:
		[] [1] [2] [3] [11] [12] [13] [21] [22] [23] [31] [32] [33] [111] [112] [113] [121] [122] [123] [131] [132] [133] [211] [212] [213] [221] [222] [223] [231] [232] [233] [311] [312] [313] [321] [322] [323] [331] [332] [333] 

----------
member:
>>>>>>  
>>>>>>  FOIL then commences the search for the first clause of the
>>>>>>  definition of member.  There are initially 22 positive tuples,
>>>>>>  41 tuples in all.  Literals are evaluated to see the result
>>>>>>  of adding them to the clause.  If the literal "B=[]" were
>>>>>>  added, the clause would be satisfied by only two of the 41
>>>>>>  tuples, 0 of them being positive.  The negated form of this
>>>>>>  literal would be satisfied by 39 of the 41 tuples, of which
>>>>>>  22 would be positive.  The gains for the unnegated and negated
>>>>>>  forms of this literal would then be 0.0 bits and 1.5 bits
>>>>>>  respectively.
>>>>>>  

State (22/41, 47.5 bits available)

>>>>>>  
>>>>>>  Evaluation of the literal B=[] is abandoned when it becomes clear
>>>>>>  that no plus tuples are covered by it.
>>>>>>  
	B=[]  0[0/1] [22/22]   abandoned(56%)
				[== tried 1/1] 0.0 secs
>>>>>>  
>>>>>>  The following literal is noted as determinate ('[Det]').
>>>>>>  
	components(B,C,D)  22[22/39]   [Det] gain 1.5
	components(B,A,C)  14[14/14]   gain 12.2
>>>>>>
>>>>>>  Evaluation of the following literal is also pruned since it cannot
>>>>>>  achieve a gain as high as the previous one.
>>>>>>
	components(C,A,B)  8[8/8] [9/9]   abandoned(41%)
				[components tried 3/3] 0.0 secs

>>>>>>
>>>>>>  Even though the following literal is not chosen, it is remembered
>>>>>>  since it would produce a complete clause.
>>>>>>
	Save clause ending with components(B,A,C) (cover 14, accuracy 100%)
>>>>>>  
>>>>>>  The literal added is the determinate literal components(B,C,D);
>>>>>>  under the ordering discovered for type L, variable B is now
>>>>>>  greater than variable D.
>>>>>>  

Determinate literals
	components(B,C,D)
		Note B>D

State (22/39, 47.5 bits available, 1 weak literal)

	B=[]  0[0/0]   #
	D=[]  2[2/5] [20/24]   abandoned(74%)
				[== tried 2/2] 0.0 secs
	A=C  14[14/14]   gain 11.2
	B=D  0[0/0] [11/11]   abandoned(28%)
				[= tried 2/2] 0.0 secs
	member(A,D)  16[16/16]   gain 12.8
	member(C,D)  7[7/7] [10/10]   abandoned(43%)
	member(E,D)  20[28/34]   gain 10.5
				[member tried 3/3] 0.0 secs
	components(B,A,E)  14[14/14]   D=E #
	components(B,A,D)  14[14/14]   gain 11.2
	components(B,C,E)  22[22/39]   D=E [XDet] #
	components(D,E,F)  20[20/25] [2/5]   abandoned(76%)
	components(D,A,E)  10[10/10] [10/10]   abandoned(51%)
				(pruning subsumed arguments)
	components(D,C,E)  5[5/5] [10/10]   abandoned(38%)
				(pruning subsumed arguments)
	components(D,E,B)  0[0/0] [10/10]   abandoned(25%)
	components(E,A,B)  8[8/8] [10/10]   abandoned(46%)
	components(E,A,D)  22[22/39]   [Det] gain 0.0
	components(E,C,B)  8[8/8] [7/7]   abandoned(38%)
	components(E,C,D)  22[22/39]   B=E [XDet] #
				[components tried 11/16] 0.0 secs

>>>>>>
>>>>>>  We now have a better saveable clause.
>>>>>>
	Save clause ending with member(A,D) (cover 16, accuracy 100%)

Determinate literals
	components(E,A,D)
		Note B>D

State (22/39, 47.5 bits available, 2 weak literals)

	B=[]  0[0/0]   #
	D=[]  2[2/5] [20/24]   abandoned(74%)
	E=[]  0[0/0]   #
				[== tried 3/3] 0.0 secs
	A=C  14[14/14]   gain 11.2
	B=D  0[0/0] [11/11]   abandoned(28%)
	B=E  14[14/14]   gain 11.2
	D=E  0[0/0] [11/11]   abandoned(28%)
				[= tried 4/4] 0.0 secs
	member(A,D)  16[16/16]   gain 12.8
	member(C,D)  7[7/7] [10/10]   abandoned(43%)
	member(F,D)  20[28/34]   gain 10.5
				[member tried 3/3] 0.0 secs
	components(B,F,G)  22[22/39]   C=F D=G [XDet] #
	components(B,A,F)  14[14/14]   D=F #
	components(B,A,D)  14[14/14]   gain 11.2
	components(B,A,E)  0[0/0] [9/9]   abandoned(23%)
	components(B,C,F)  22[22/39]   D=F [XDet] #
	components(B,C,E)  0[0/0] [9/9]   abandoned(23%)
	components(B,F,E)  0[0/0] [9/9]   abandoned(23%)
	components(D,F,G)  20[20/25] [2/5]   abandoned(76%)
	components(D,A,F)  10[10/10] [9/9]   abandoned(48%)
				(pruning subsumed arguments)
	components(D,C,F)  5[5/5] [9/9]   abandoned(35%)
				(pruning subsumed arguments)
	components(D,F,B)  0[0/0] [9/9]   abandoned(23%)
	components(D,F,E)  0[0/0] [9/9]   abandoned(23%)
	components(E,F,G)  22[22/39]   A=F D=G [XDet] #
	components(E,A,F)  22[22/39]   D=F [XDet] #
	components(E,A,B)  0[0/0] [9/9]   abandoned(23%)
	components(E,C,F)  14[14/14]   D=F #
	components(E,C,B)  0[0/0] [9/9]   abandoned(23%)
	components(E,C,D)  14[14/14]   gain 11.2
	components(E,F,B)  0[0/0] [9/9]   abandoned(23%)
	components(E,F,D)  22[22/39]   A=F [XDet] #
	components(F,A,B)  8[8/8] [9/9]   abandoned(43%)
	components(F,A,E)  8[8/8] [9/9]   abandoned(43%)
	components(F,C,B)  8[8/8] [9/9]   abandoned(43%)
	components(F,C,D)  22[22/39]   B=F [XDet] #
	components(F,C,E)  8[8/8] [9/9]   abandoned(43%)
				[components tried 25/33] 0.0 secs

	Save A=C (14,14 value 12.2)
	Save B=E (14,14 value 12.2)
	Save components(B,A,D) (14,14 value 12.2)
	Save components(E,C,D) (14,14 value 12.2)

Best literal member(A,D) (3.6 bits)

>>>>>>  
>>>>>>  After adding literals components(E,A,D) and member(A,D), the
>>>>>>  clause is found to be complete.  However, it is more complex
>>>>>>  than the best saved clause which has the same coverage, so
>>>>>>  the saved clause is preferred.
>>>>>>  
[Replace by saved clause]

Initial clause (0 errs): member(A,B) :- components(B,C,D), member(A,D).
		member(A,D) essential
		components(B,C,D) essential

Clause 0: member(A,B) :- components(B,C,D), member(A,D).

State (6/25, 30.2 bits available)

	B=[]  0[0/1] [6/6]   abandoned(28%)
				[== tried 1/1] 0.0 secs
>>>>>>
>>>>>>  The apparent new variable C introduced by the next literal is
>>>>>>  identical to existing variable A on all positive tuples.
>>>>>>  It also gets the axe.
>>>>>>
	components(B,C,D)  6[6/23]   A=+C [XDet] #
	components(B,A,C)  6[6/6]   [Det] gain 11.4
				[components tried 2/3] 0.0 secs

	Save clause ending with components(B,A,C) (cover 6, accuracy 100%)

Best literal components(B,A,C) (2.6 bits)

Initial clause (0 errs): member(A,B) :- components(B,A,C).
		components(B,A,C) essential

Clause 1: member(A,B) :- components(B,A,C).
>>>>>>  
>>>>>>  The final definition is screened and the clauses reordered
>>>>>>  to place the non-recursive base case ahead of the general
>>>>>>  recursive clause.
>>>>>>  
	Clause 1 needed for 	1,[1]
	Clause 0 needed for 	3,[13]

member(A,B) :- components(B,A,C).
member(A,B) :- components(B,C,D), member(A,D).

Time 0.0 secs

>>>>>>  
>>>>>>  The definition is then tested on the four test cases.  Horror:
>>>>>>  even though the definition seems fine, it gives an error on
>>>>>>  case 3,[232]!  In order to determine by the recursive clause
>>>>>>  that 3 is a member of [232], it is necessary to be able to
>>>>>>  establish that 3 is a member of [32].  However, the tuple
>>>>>>  3,[23] does not appear among the tuples known to be in the
>>>>>>  relation member, so 3,[232] is judged (incorrectly) to be
>>>>>>  false.  A future release will allow the definition to be
>>>>>>  tested in Prolog interpretive mode, which would have produced
>>>>>>  the correct result for this case.
>>>>>>  
Test relation member
    (+)	3,[232]
Summary: 1 error in 4 trials
@//E*O*F member.explain//
chmod u=rw,g=,o= member.explain
 
echo x - ncm.d
sed 's/^@//' > "ncm.d" <<'@//E*O*F ncm.d//'
N: *0, *1, 2, 3, 4, 5, 6, 7, 8, 9,
10, 11, 12, 14, 15, 16, 18, 20, 21, 24,
25, 27, 28, 30, 32, 35, 36, 40, 42, 45,
48, 49, 50, 54, 56, 60, 63, 64, 70, 72,
80, 81, 90, 100.

*dec(N,N) #-
1,0
2,1
3,2
4,3
5,4
6,5
7,6
8,7
9,8
10,9
11,10
@.
*mult(N,N,N) ##-
0,0,0
0,1,0
0,2,0
0,3,0
0,4,0
0,5,0
0,6,0
0,7,0
0,8,0
0,9,0
0,10,0
1,0,0
1,1,1
1,2,2
1,3,3
1,4,4
1,5,5
1,6,6
1,7,7
1,8,8
1,9,9
1,10,10
2,0,0
2,1,2
2,2,4
2,3,6
2,4,8
2,5,10
2,6,12
2,7,14
2,8,16
2,9,18
2,10,20
3,0,0
3,1,3
3,2,6
3,3,9
3,4,12
3,5,15
3,6,18
3,7,21
3,8,24
3,9,27
3,10,30
4,0,0
4,1,4
4,2,8
4,3,12
4,4,16
4,5,20
4,6,24
4,7,28
4,8,32
4,9,36
4,10,40
5,0,0
5,1,5
5,2,10
5,3,15
5,4,20
5,5,25
5,6,30
5,7,35
5,8,40
5,9,45
5,10,50
6,0,0
6,1,6
6,2,12
6,3,18
6,4,24
6,5,30
6,6,36
6,7,42
6,8,48
6,9,54
6,10,60
7,0,0
7,1,7
7,2,14
7,3,21
7,4,28
7,5,35
7,6,42
7,7,49
7,8,56
7,9,63
7,10,70
8,0,0
8,1,8
8,2,16
8,3,24
8,4,32
8,5,40
8,6,48
8,7,56
8,8,64
8,9,72
8,10,80
9,0,0
9,1,9
9,2,18
9,3,27
9,4,36
9,5,45
9,6,54
9,7,63
9,8,72
9,9,81
9,10,90
10,0,0
10,1,10
10,2,20
10,3,30
10,4,40
10,5,50
10,6,60
10,7,70
10,8,80
10,9,90
10,10,100
@.
choose(N,N,N) ##-
0,0,1
1,0,1
1,1,1
2,0,1
2,1,2
2,2,1
3,0,1
3,1,3
3,2,3
3,3,1
4,0,1
4,1,4
4,2,6
4,3,4
4,4,1
5,0,1
5,1,5
5,2,10
5,3,10
5,4,5
5,5,1
;
0,0,2
0,0,3
0,0,4
0,0,5
0,0,6
0,0,7
0,0,8
0,0,9
0,0,10
1,0,2
1,0,3
1,0,4
1,0,5
1,0,6
1,0,7
1,0,8
1,0,9
1,0,10
1,1,2
1,1,3
1,1,4
1,1,5
1,1,6
1,1,7
1,1,8
1,1,9
1,1,10
2,0,2
2,0,3
2,0,4
2,0,5
2,0,6
2,0,7
2,0,8
2,0,9
2,0,10
2,1,1
2,1,3
2,1,4
2,1,5
2,1,6
2,1,7
2,1,8
2,1,9
2,1,10
2,2,2
2,2,3
2,2,4
2,2,5
2,2,6
2,2,7
2,2,8
2,2,9
2,2,10
3,0,2
3,0,3
3,0,4
3,0,5
3,0,6
3,0,7
3,0,8
3,0,9
3,0,10
3,1,1
3,1,2
3,1,4
3,1,5
3,1,6
3,1,7
3,1,8
3,1,9
3,1,10
3,2,1
3,2,2
3,2,4
3,2,5
3,2,6
3,2,7
3,2,8
3,2,9
3,2,10
3,3,2
3,3,3
3,3,4
3,3,5
3,3,6
3,3,7
3,3,8
3,3,9
3,3,10
4,0,2
4,0,3
4,0,4
4,0,5
4,0,6
4,0,7
4,0,8
4,0,9
4,0,10
4,1,1
4,1,2
4,1,3
4,1,5
4,1,6
4,1,7
4,1,8
4,1,9
4,1,10
4,2,1
4,2,2
4,2,3
4,2,4
4,2,5
4,2,7
4,2,8
4,2,9
4,2,10
4,3,1
4,3,2
4,3,3
4,3,5
4,3,6
4,3,7
4,3,8
4,3,9
4,3,10
4,4,2
4,4,3
4,4,4
4,4,5
4,4,6
4,4,7
4,4,8
4,4,9
4,4,10
5,0,2
5,0,3
5,0,4
5,0,5
5,0,6
5,0,7
5,0,8
5,0,9
5,0,10
5,1,1
5,1,2
5,1,3
5,1,4
5,1,6
5,1,7
5,1,8
5,1,9
5,1,10
5,2,1
5,2,2
5,2,3
5,2,4
5,2,5
5,2,6
5,2,7
5,2,8
5,2,9
5,3,1
5,3,2
5,3,3
5,3,4
5,3,5
5,3,6
5,3,7
5,3,8
5,3,9
5,4,1
5,4,2
5,4,3
5,4,4
5,4,6
5,4,7
5,4,8
5,4,9
5,4,10
5,5,2
5,5,3
5,5,4
5,5,5
5,5,6
5,5,7
5,5,8
5,5,9
5,5,10
@.
@//E*O*F ncm.d//
chmod u=rw,g=,o= ncm.d
 
echo x - order.c
sed 's/^@//' > "order.c" <<'@//E*O*F order.c//'
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
@//E*O*F order.c//
chmod u=rw,g=,o= order.c
 
echo x - output.c
sed 's/^@//' > "output.c" <<'@//E*O*F output.c//'
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
@//E*O*F output.c//
chmod u=rw,g=,o= output.c
 
echo x - prune.c
sed 's/^@//' > "prune.c" <<'@//E*O*F prune.c//'
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
@//E*O*F prune.c//
chmod u=rw,g=,o= prune.c
 
echo x - qs44.d
sed 's/^@//' > "qs44.d" <<'@//E*O*F qs44.d//'
*E: 0,1,2,3.
*L: *[],[0],[1],[2],[3],[01],[02],[03],[10],[12],[13],[20],[21],[23],[30],[31],[32],[012],[013],[021],[023],[031],[032],[102],[103],[120],[123],[130],[132],[201],[203],[210],[213],[230],[231],[301],[302],[310],[312],[320],[321],[0123],[0132],[0213],[0231],[0312],[0321],[1023],[1032],[1203],[1230],[1302],[1320],[2013],[2031],[2103],[2130],[2301],[2310],[3012],[3021],[3102],[3120],[3201],[3210].

*elt(E)
0
1
2
3
@.
*append(L,L,L) ##-
[], [], []
[], [0], [0]
[0], [], [0]
[], [1], [1]
[1], [], [1]
[], [2], [2]
[2], [], [2]
[], [3], [3]
[3], [], [3]
[], [01], [01]
[0], [1], [01]
[01], [], [01]
[], [02], [02]
[0], [2], [02]
[02], [], [02]
[], [03], [03]
[0], [3], [03]
[03], [], [03]
[], [10], [10]
[1], [0], [10]
[10], [], [10]
[], [12], [12]
[1], [2], [12]
[12], [], [12]
[], [13], [13]
[1], [3], [13]
[13], [], [13]
[], [20], [20]
[2], [0], [20]
[20], [], [20]
[], [21], [21]
[2], [1], [21]
[21], [], [21]
[], [23], [23]
[2], [3], [23]
[23], [], [23]
[], [30], [30]
[3], [0], [30]
[30], [], [30]
[], [31], [31]
[3], [1], [31]
[31], [], [31]
[], [32], [32]
[3], [2], [32]
[32], [], [32]
[], [012], [012]
[0], [12], [012]
[01], [2], [012]
[012], [], [012]
[], [013], [013]
[0], [13], [013]
[01], [3], [013]
[013], [], [013]
[], [021], [021]
[0], [21], [021]
[02], [1], [021]
[021], [], [021]
[], [023], [023]
[0], [23], [023]
[02], [3], [023]
[023], [], [023]
[], [031], [031]
[0], [31], [031]
[03], [1], [031]
[031], [], [031]
[], [032], [032]
[0], [32], [032]
[03], [2], [032]
[032], [], [032]
[], [102], [102]
[1], [02], [102]
[10], [2], [102]
[102], [], [102]
[], [103], [103]
[1], [03], [103]
[10], [3], [103]
[103], [], [103]
[], [120], [120]
[1], [20], [120]
[12], [0], [120]
[120], [], [120]
[], [123], [123]
[1], [23], [123]
[12], [3], [123]
[123], [], [123]
[], [130], [130]
[1], [30], [130]
[13], [0], [130]
[130], [], [130]
[], [132], [132]
[1], [32], [132]
[13], [2], [132]
[132], [], [132]
[], [201], [201]
[2], [01], [201]
[20], [1], [201]
[201], [], [201]
[], [203], [203]
[2], [03], [203]
[20], [3], [203]
[203], [], [203]
[], [210], [210]
[2], [10], [210]
[21], [0], [210]
[210], [], [210]
[], [213], [213]
[2], [13], [213]
[21], [3], [213]
[213], [], [213]
[], [230], [230]
[2], [30], [230]
[23], [0], [230]
[230], [], [230]
[], [231], [231]
[2], [31], [231]
[23], [1], [231]
[231], [], [231]
[], [301], [301]
[3], [01], [301]
[30], [1], [301]
[301], [], [301]
[], [302], [302]
[3], [02], [302]
[30], [2], [302]
[302], [], [302]
[], [310], [310]
[3], [10], [310]
[31], [0], [310]
[310], [], [310]
[], [312], [312]
[3], [12], [312]
[31], [2], [312]
[312], [], [312]
[], [320], [320]
[3], [20], [320]
[32], [0], [320]
[320], [], [320]
[], [321], [321]
[3], [21], [321]
[32], [1], [321]
[321], [], [321]
[], [0123], [0123]
[0], [123], [0123]
[01], [23], [0123]
[012], [3], [0123]
[0123], [], [0123]
[], [0132], [0132]
[0], [132], [0132]
[01], [32], [0132]
[013], [2], [0132]
[0132], [], [0132]
[], [0213], [0213]
[0], [213], [0213]
[02], [13], [0213]
[021], [3], [0213]
[0213], [], [0213]
[], [0231], [0231]
[0], [231], [0231]
[02], [31], [0231]
[023], [1], [0231]
[0231], [], [0231]
[], [0312], [0312]
[0], [312], [0312]
[03], [12], [0312]
[031], [2], [0312]
[0312], [], [0312]
[], [0321], [0321]
[0], [321], [0321]
[03], [21], [0321]
[032], [1], [0321]
[0321], [], [0321]
[], [1023], [1023]
[1], [023], [1023]
[10], [23], [1023]
[102], [3], [1023]
[1023], [], [1023]
[], [1032], [1032]
[1], [032], [1032]
[10], [32], [1032]
[103], [2], [1032]
[1032], [], [1032]
[], [1203], [1203]
[1], [203], [1203]
[12], [03], [1203]
[120], [3], [1203]
[1203], [], [1203]
[], [1230], [1230]
[1], [230], [1230]
[12], [30], [1230]
[123], [0], [1230]
[1230], [], [1230]
[], [1302], [1302]
[1], [302], [1302]
[13], [02], [1302]
[130], [2], [1302]
[1302], [], [1302]
[], [1320], [1320]
[1], [320], [1320]
[13], [20], [1320]
[132], [0], [1320]
[1320], [], [1320]
[], [2013], [2013]
[2], [013], [2013]
[20], [13], [2013]
[201], [3], [2013]
[2013], [], [2013]
[], [2031], [2031]
[2], [031], [2031]
[20], [31], [2031]
[203], [1], [2031]
[2031], [], [2031]
[], [2103], [2103]
[2], [103], [2103]
[21], [03], [2103]
[210], [3], [2103]
[2103], [], [2103]
[], [2130], [2130]
[2], [130], [2130]
[21], [30], [2130]
[213], [0], [2130]
[2130], [], [2130]
[], [2301], [2301]
[2], [301], [2301]
[23], [01], [2301]
[230], [1], [2301]
[2301], [], [2301]
[], [2310], [2310]
[2], [310], [2310]
[23], [10], [2310]
[231], [0], [2310]
[2310], [], [2310]
[], [3012], [3012]
[3], [012], [3012]
[30], [12], [3012]
[301], [2], [3012]
[3012], [], [3012]
[], [3021], [3021]
[3], [021], [3021]
[30], [21], [3021]
[302], [1], [3021]
[3021], [], [3021]
[], [3102], [3102]
[3], [102], [3102]
[31], [02], [3102]
[310], [2], [3102]
[3102], [], [3102]
[], [3120], [3120]
[3], [120], [3120]
[31], [20], [3120]
[312], [0], [3120]
[3120], [], [3120]
[], [3201], [3201]
[3], [201], [3201]
[32], [01], [3201]
[320], [1], [3201]
[3201], [], [3201]
[], [3210], [3210]
[3], [210], [3210]
[32], [10], [3210]
[321], [0], [3210]
[3210], [], [3210]
@.
*components(L,E,L) #--/-##
[0], 0, []
[1], 1, []
[2], 2, []
[3], 3, []
[01], 0, [1]
[02], 0, [2]
[03], 0, [3]
[10], 1, [0]
[12], 1, [2]
[13], 1, [3]
[20], 2, [0]
[21], 2, [1]
[23], 2, [3]
[30], 3, [0]
[31], 3, [1]
[32], 3, [2]
[012], 0, [12]
[013], 0, [13]
[021], 0, [21]
[023], 0, [23]
[031], 0, [31]
[032], 0, [32]
[102], 1, [02]
[103], 1, [03]
[120], 1, [20]
[123], 1, [23]
[130], 1, [30]
[132], 1, [32]
[201], 2, [01]
[203], 2, [03]
[210], 2, [10]
[213], 2, [13]
[230], 2, [30]
[231], 2, [31]
[301], 3, [01]
[302], 3, [02]
[310], 3, [10]
[312], 3, [12]
[320], 3, [20]
[321], 3, [21]
[0123], 0, [123]
[0132], 0, [132]
[0213], 0, [213]
[0231], 0, [231]
[0312], 0, [312]
[0321], 0, [321]
[1023], 1, [023]
[1032], 1, [032]
[1203], 1, [203]
[1230], 1, [230]
[1302], 1, [302]
[1320], 1, [320]
[2013], 2, [013]
[2031], 2, [031]
[2103], 2, [103]
[2130], 2, [130]
[2301], 2, [301]
[2310], 2, [310]
[3012], 3, [012]
[3021], 3, [021]
[3102], 3, [102]
[3120], 3, [120]
[3201], 3, [201]
[3210], 3, [210]
@.
sort(L, L) #-
[], []
[0], [0]
[1], [1]
[2], [2]
[3], [3]
[01], [01]
[02], [02]
[03], [03]
[10], [01]
[12], [12]
[13], [13]
[20], [02]
[21], [12]
[23], [23]
[30], [03]
[31], [13]
[32], [23]
[012], [012]
[013], [013]
[021], [012]
[023], [023]
[031], [013]
[032], [023]
[102], [012]
[103], [013]
[120], [012]
[123], [123]
[130], [013]
[132], [123]
[201], [012]
[203], [023]
[210], [012]
[213], [123]
[230], [023]
[231], [123]
[301], [013]
[302], [023]
[310], [013]
[312], [123]
[320], [023]
[321], [123]
[0123], [0123]
[0132], [0123]
[0213], [0123]
[0231], [0123]
[0312], [0123]
[0321], [0123]
[1023], [0123]
[1032], [0123]
[1203], [0123]
[1230], [0123]
[1302], [0123]
[1320], [0123]
[2013], [0123]
[2031], [0123]
[2103], [0123]
[2130], [0123]
[2301], [0123]
[2310], [0123]
[3012], [0123]
[3021], [0123]
[3102], [0123]
[3120], [0123]
[3201], [0123]
[3210], [0123]
@.
*partition(E,L,L,L) ##--
0, [], [], []
1, [], [], []
2, [], [], []
3, [], [], []
1, [0], [0], []
2, [0], [0], []
3, [0], [0], []
0, [1], [], [1]
2, [1], [1], []
3, [1], [1], []
0, [2], [], [2]
1, [2], [], [2]
3, [2], [2], []
0, [3], [], [3]
1, [3], [], [3]
2, [3], [], [3]
2, [01], [01], []
3, [01], [01], []
1, [02], [0], [2]
3, [02], [02], []
1, [03], [0], [3]
2, [03], [0], [3]
2, [10], [10], []
3, [10], [10], []
0, [12], [], [12]
3, [12], [12], []
0, [13], [], [13]
2, [13], [1], [3]
1, [20], [0], [2]
3, [20], [20], []
0, [21], [], [21]
3, [21], [21], []
0, [23], [], [23]
1, [23], [], [23]
1, [30], [0], [3]
2, [30], [0], [3]
0, [31], [], [31]
2, [31], [1], [3]
0, [32], [], [32]
1, [32], [], [32]
3, [012], [012], []
2, [013], [01], [3]
3, [021], [021], []
1, [023], [0], [23]
2, [031], [01], [3]
1, [032], [0], [32]
3, [102], [102], []
2, [103], [10], [3]
3, [120], [120], []
0, [123], [], [123]
2, [130], [10], [3]
0, [132], [], [132]
3, [201], [201], []
1, [203], [0], [23]
3, [210], [210], []
0, [213], [], [213]
1, [230], [0], [23]
0, [231], [], [231]
2, [301], [01], [3]
1, [302], [0], [32]
2, [310], [10], [3]
0, [312], [], [312]
1, [320], [0], [32]
0, [321], [], [321]
@.
@//E*O*F qs44.d//
chmod u=rw,g=,o= qs44.d
 
echo x - search.c
sed 's/^@//' > "search.c" <<'@//E*O*F search.c//'
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
@//E*O*F search.c//
chmod u=rw,g=,o= search.c
 
echo x - sort.d
sed 's/^@//' > "sort.d" <<'@//E*O*F sort.d//'
E: 0,1,2,3.
L: *[],[0],[1],[2],[3],[01],[02],[03],[10],[12],[13],[20],[21],[23],[30],[31],[32],[012],[013],[021],[023],[031],[032],[102],[103],[120],[123],[130],[132],[201],[203],[210],[213],[230],[231],[301],[302],[310],[312],[320],[321],[0123],[0132],[0213],[0231],[0312],[0321],[1023],[1032],[1203],[1230],[1302],[1320],[2013],[2031],[2103],[2130],[2301],[2310],[3012],[3021],[3102],[3120],[3201],[3210].

*elt(E)
0
1
2
3
@.
*components(L,E,L) #--/-##
[0], 0, []
[1], 1, []
[2], 2, []
[3], 3, []
[01], 0, [1]
[02], 0, [2]
[03], 0, [3]
[10], 1, [0]
[12], 1, [2]
[13], 1, [3]
[20], 2, [0]
[21], 2, [1]
[23], 2, [3]
[30], 3, [0]
[31], 3, [1]
[32], 3, [2]
[012], 0, [12]
[013], 0, [13]
[021], 0, [21]
[023], 0, [23]
[031], 0, [31]
[032], 0, [32]
[102], 1, [02]
[103], 1, [03]
[120], 1, [20]
[123], 1, [23]
[130], 1, [30]
[132], 1, [32]
[201], 2, [01]
[203], 2, [03]
[210], 2, [10]
[213], 2, [13]
[230], 2, [30]
[231], 2, [31]
[301], 3, [01]
[302], 3, [02]
[310], 3, [10]
[312], 3, [12]
[320], 3, [20]
[321], 3, [21]
[0123], 0, [123]
[0132], 0, [132]
[0213], 0, [213]
[0231], 0, [231]
[0312], 0, [312]
[0321], 0, [321]
[1023], 1, [023]
[1032], 1, [032]
[1203], 1, [203]
[1230], 1, [230]
[1302], 1, [302]
[1320], 1, [320]
[2013], 2, [013]
[2031], 2, [031]
[2103], 2, [103]
[2130], 2, [130]
[2301], 2, [301]
[2310], 2, [310]
[3012], 3, [012]
[3021], 3, [021]
[3102], 3, [102]
[3120], 3, [120]
[3201], 3, [201]
[3210], 3, [210]
@.
sort(L, L) #-
[], []
[0], [0]
[1], [1]
[2], [2]
[3], [3]
[01], [01]
[02], [02]
[03], [03]
[10], [01]
[12], [12]
[13], [13]
[20], [02]
[21], [12]
[23], [23]
[30], [03]
[31], [13]
[32], [23]
[012], [012]
[013], [013]
[021], [012]
[023], [023]
[031], [013]
[032], [023]
[102], [012]
[103], [013]
[120], [012]
[123], [123]
[130], [013]
[132], [123]
[201], [012]
[203], [023]
[210], [012]
[213], [123]
[230], [023]
[231], [123]
[301], [013]
[302], [023]
[310], [013]
[312], [123]
[320], [023]
[321], [123]
[0123], [0123]
[0132], [0123]
[0213], [0123]
[0231], [0123]
[0312], [0123]
[0321], [0123]
[1023], [0123]
[1032], [0123]
[1203], [0123]
[1230], [0123]
[1302], [0123]
[1320], [0123]
[2013], [0123]
[2031], [0123]
[2103], [0123]
[2130], [0123]
[2301], [0123]
[2310], [0123]
[3012], [0123]
[3021], [0123]
[3102], [0123]
[3120], [0123]
[3201], [0123]
[3210], [0123]
@.
*lt(E,E) ##
0, 1
0, 2
0, 3
1, 2
1, 3
2, 3
@.
@//E*O*F sort.d//
chmod u=rw,g=,o= sort.d
 
echo x - state.c
sed 's/^@//' > "state.c" <<'@//E*O*F state.c//'
/******************************************************************************/
/*									      */
/*	A state, summarising a point in the search for a clause, consists     */
/*	of a set of tuples and various counts.  The routines here set up      */
/*	an initial state for a relation and produce the new state that	      */
/*	results when a literal is added to the current (partial) clause	      */
/*									      */
/******************************************************************************/


#include  "defns.i"
#include  "extern.i"


	/*  Set up original state for search for definition  */


void  OriginalState(Relation R)
/*    -------------  */
{
    int		i, j, WorldSize=1, TupleArity, NegTuples, Remaining;
    Tuple	Case, *Scan;
    double	drand48();

    if ( ! LogFact )
    {
	LogFact = Alloc(1001, float);

	LogFact[0] = 0.0;
	ForEach(i, 1, 1000)
	{
	    LogFact[i] = LogFact[i-1] + Log2(i);
	}
    }

    /*  Establish number of variables hence size of tuples, and set variable 
        types equal to the type of variable in that position in the relation,
        and variable depths to zero  */

    StartDef.MaxVar = Current.MaxVar = R->Arity;
    TupleArity = R->Arity+1;

    ForEach(i, 1, R->Arity)
    {
	Variable[i]->Type    = R->Type[i];
	Variable[i]->TypeRef = R->TypeRef[i];
	Variable[i]->Depth   = 0;
    }

    StartDef.NPos = Number(R->Pos);
    AdjMinAccuracy = MINACCURACY;

    /* Test whether negative tuples are already defined in the relation */

    if ( R->Neg )
    {
	/*  Simply copy positive and negative tuples  */

	StartDef.NTot = AllTuples = Number(R->Neg) + StartDef.NPos;

	if ( StartDef.NTot > MAXTUPLES )
	{
	    printf("Training Set Size exceeds tuple limit: ");
	    printf("%d > %d\n", StartDef.NTot, MAXTUPLES);
	    printf("Rerun with larger MAXTUPLES to proceed further\n");
	    exit(0);
	}

	StartDef.Tuples = Alloc(StartDef.NTot+1, Tuple);

	i = 0;

	for ( Scan = R->Pos ; *Scan ; Scan++ )
	{
	    AddTuple(i, *Scan, TupleArity, PosMark);
	    i++;
	}

	for ( Scan = R->Neg ; *Scan ; Scan++ )
	{
	    AddTuple(i, *Scan, TupleArity, 0);
	    i++;
	}
    }
    else 
    {
	/* Negative tuples not already defined */

	/* Find maximum training set size */

	ForEach(j, 1, R->Arity)
	{
	    if ( R->TypeRef[j]->Continuous ) Error(6, R->Name, Nil);
	    WorldSize *= R->TypeRef[j]->NValues;
	}

	Remaining = WorldSize - (StartDef.NPos - R->PosDuplicates);

	AllTuples = Remaining + StartDef.NPos;
	NegTuples = SAMPLE * Remaining;
	if ( SAMPLE < 0.99 )
	{
	    AdjMinAccuracy = MINACCURACY /
			     (MINACCURACY + SAMPLE * (1-MINACCURACY));
	    Verbose(1)
	    {
		printf("\t\t(Adjusted minimum clause accuracy %.1f%%)\n",
			100*AdjMinAccuracy);
	    }
	}

	StartDef.NTot = StartDef.NPos + NegTuples;

	if ( StartDef.NTot > MAXTUPLES )
	{
	    printf("Training Set Size will exceed tuple limit: ");
	    printf("%d > %d\n", StartDef.NPos + NegTuples, MAXTUPLES);
	    printf("Rerun with larger MAXTUPLES to proceed further\n");
	    printf("(or use smaller sample of negative tuples).\n");
	    exit(0);
	}

	StartDef.Tuples = Alloc(StartDef.NTot+1, Tuple);

	/*  Copy positive tuples  */

	i = 0;

	for ( Scan = R->Pos ; *Scan ; Scan++ )
	{
	    AddTuple(i, *Scan, TupleArity, PosMark);
	    i++;
	}

	if ( WorldSize <= 10*MAXTUPLES )
	{
	    /* Enumerate all possible tuples and add a sample of the negative
	       tuples to the training set  - note that if SAMPLE is 1, the
	       default, all negative tuples are added to the training set */

	    Case = Nil;

	    while ( NegTuples && (Case = NextConstTuple(R, Case)) )
	    {
	        if ( ! Join(R->Pos, R->PosIndex, DefaultVars, Case, R->Arity, true)
		      && drand48() <= NegTuples / (float) Remaining-- )
	        {
		    AddTuple(i, Case, TupleArity, 0);
		    i++;
		    NegTuples--;
		}
	    }

	    if ( Case ) pfree(Case);
	}
	else
	{
	    /* Might take too long to enumerate all tuples, so generate
	       them randomly - can result in duplicate negative tuples */

	    Case = Alloc(TupleArity, Const);

	    while ( i < StartDef.NTot )
	    {
	        RandomTuple(R, Case);

	        if ( ! Join(R->Pos, R->PosIndex, DefaultVars, Case, R->Arity, 1))
	        {
		    AddTuple(i, Case, TupleArity, 0);
		    i++;
		}
	    }

	    pfree(Case);
	}
    }

    StartDef.Tuples[StartDef.NTot] = Nil;
    StartDef.NOrigPos = StartDef.NPos;
    StartDef.NOrigTot = StartDef.NTot;

    Realloc(Flags, StartDef.NTot+1, char);

    StartDef.BaseInfo = Info(StartDef.NPos, StartDef.NTot);
}



void  AddTuple(int N, Tuple T, int TupleArity, int Mark)
/*    --------  */
{
    StartDef.Tuples[N] = Alloc(TupleArity, Const);
    memcpy(StartDef.Tuples[N], T, TupleArity*sizeof(Const));
    StartDef.Tuples[N][0] = N | Mark;
}



	/*  Generate in Case the next constant tuple taking note of type
	    constraints */

Tuple  NextConstTuple(Relation R, Tuple Case)
/*     --------------  */
{
    int i, v;
    TypeInfo T;

    if ( ! Case )
    {
        Case = Alloc(R->Arity+1, Const);

	ForEach(i, 1, R->Arity)
	{
	    Case[i] = R->TypeRef[i]->Value[0];
	}
    }
    else
    {
        i = R->Arity;
	T = R->TypeRef[i];

	while ( Case[i] == T->Value[T->NValues-1] )
	{
	    if ( i <= 1 )
	    {
	        pfree(Case);
		return Nil;
	    }

	    Case[i] = T->Value[0];

	    T = R->TypeRef[--i];
	}

	for ( v = 1; Case[i] != T->Value[v-1] ; v++ )
	    ;
	Case[i] = T->Value[v];
    }
    return Case;
}



/*****************************************************************************/
/*                                                                           */
/*  RandomTuple - generate a random tuple satisfying type constraints for    */
/*                relation R                                                 */
/*                                                                           */
/*****************************************************************************/


void  RandomTuple(Relation R, Tuple Result)
/*    -----------  */
{
    int i, v;
    TypeInfo T;
    double drand48();

    ForEach(i, 1, R->Arity)
    {
        T = R->TypeRef[i];
	v = (int)(T->NValues * drand48());
	Result[i] = T->Value[v];
    }
}



void  NewState(Literal L, int NewSize)
/*    --------  */
{
    FormNewState(L->Rel, L->Sign, L->Args, NewSize);
    AcceptNewState(L->Rel, L->Args, NewSize);
}



	/*  Construct new state in New  */

void  FormNewState(Relation R, Boolean RSign, Var *A, int NewSize)
/*    ------------  */
{
    Tuple	*TSP, Case, *Bindings, Instance;
    int		i, N, RN;
    Boolean	BuiltIn=false, Match, NotNegated;
    Const	X2;

    if ( Predefined(R) )
    {
	/*  Prepare for calls to Satisfies()  */

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

    N = R->Arity;

    /*  Find highest variable in new clause  */

    New.MaxVar = Current.MaxVar;
    if ( RSign )
    {
        ForEach(i, 1, N)
	{
	    New.MaxVar = Max(A[i], New.MaxVar);
	}
    }

    New.Tuples = Alloc(NewSize+1, Tuple);

    New.NPos = New.NTot = 0;

    for ( TSP = Current.Tuples ; Case = *TSP++ ; )
    {
        if ( MissingValue(R,A,Case) ) continue;

	Match = ( BuiltIn ? Satisfies(RN, A[1], X2, Case) :
	          Join(R->Pos, R->PosIndex, A, Case, N, ! RSign) );
	NotNegated = RSign != 0;
	if ( Match != NotNegated ) continue;

	if ( ! BuiltIn && RSign )
	{
	    /*  Add tuples from R->Pos  */

	    CheckSize(New.NTot, NFound, &NewSize, &New.Tuples);

	    Bindings = Found;
	    while ( Instance = *Bindings++ )
	    {
		New.Tuples[New.NTot] = Extend(Case, Instance, A, N);
		New.NTot++;
		if ( Positive(Case) ) New.NPos++;
	    }
	}
	else
	{
	    CheckSize(New.NTot, 1, &NewSize, &New.Tuples);

	    New.Tuples[New.NTot] = Alloc(New.MaxVar+1, Const);
	    memcpy(New.Tuples[New.NTot], Case, (New.MaxVar+1)*sizeof(Const));
	    New.NTot++;
	    if ( Positive(Case) ) New.NPos++;
	}
    }
    New.Tuples[New.NTot] = Nil;
}


	/*  Move state in New to Current  */

void  AcceptNewState(Relation R, Var *A, int NewSize)
/*    --------------  */
{
    int		i, N, MaxDepth=0;
    Var		V;

    if ( Current.Tuples != StartClause.Tuples )
    {
	FreeTuples(Current.Tuples, true);
    }

    if ( New.MaxVar > Current.MaxVar )
    {
	/*  New variable(s) - update type and depth info  */

        N = R->Arity;
        ForEach(i, 1, N)
	{
            if ( (V = A[i]) <= Current.MaxVar )
	    {
                MaxDepth = Max(MaxDepth, Variable[V]->Depth);
	    }
	}
        MaxDepth++;

        ForEach(i, 1, N)
	{
            if ( (V = A[i]) > Current.MaxVar )
	    {
                Variable[V]->Type    = R->Type[i];
		Variable[V]->TypeRef = R->TypeRef[i];
                Variable[V]->Depth   = MaxDepth;
	    }
	}
    }

    New.BaseInfo = Info(New.NPos, New.NTot);

    /*  Move all information across and resize tuples if necessary  */

    Current = New;

    if ( New.NTot < NewSize )
    {
	Realloc(Current.Tuples, Current.NTot+1, Tuple);
    }

    Current.BaseInfo = Info(Current.NPos, Current.NTot);
}



    /*  Rebuild a training set by applying the literals in a clause
	to the copy of the training set  */

void  RecoverState(Clause C)
/*    ------------  */
{
    int		i, SaveVerbosity;
    Literal	L;
    float	ExtraBits;

    /*  Turn off messages during recovery  */

    SaveVerbosity = VERBOSITY;
    VERBOSITY = 0;

    if ( Current.Tuples != StartClause.Tuples )
    {
        FreeTuples(Current.Tuples, true);
	Current = StartClause;
    }

    ForEach(i, 1, Target->Arity)
    {
	memset(PartialOrder[i], '#', MAXVARS+1);
    	PartialOrder[i][i] = '=';
    }
    AnyPartialOrder = false;

    memset(Barred, false, MAXVARS+1);

    NRecLitClause = NDetLits = ClauseBits = 0;

    for ( NLit = 0 ; (L = C[NLit]) ; NLit++ )
    {
	NewClause[NLit] = L;

	if ( L->Rel == Target )
	{
	    ExamineVariableRelationships();
	    AddOrders(L);
	}

	NewState(L, Current.NTot);

	if ( L->Sign == 2 )
	{
	    NDetLits++;
	}
	else
	{
	    ExtraBits = L->Bits - Log2(NLit+1.001-NDetLits);
	    ClauseBits += Max(0, ExtraBits);
	}

	if ( L->Rel == Target ) NoteRecursiveLit(L);
	NWeakLits = L->WeakLits;
    }

    NewClause[NLit] = Nil;
    VERBOSITY = SaveVerbosity;
}



void  CheckSize(int SoFar, int Extra, int *NewSize, Tuple **TSAddr)
/*    ---------  */
{
    if ( SoFar+Extra > *NewSize )
    {
	*NewSize += Max(Extra, 1000);
	Realloc(*TSAddr, *NewSize+1, Tuple);
    }
}



    /*  Tack extra variables on a tuple  */

Tuple  Extend(Tuple Case, Tuple Binding, Var *A, int N)
/*     ------  */
{
    Tuple	NewCase;
    int		i;

    NewCase = Alloc(New.MaxVar+1, Const);
    memcpy(NewCase, Case, (Current.MaxVar+1)*sizeof(Const));

    ForEach(i, 1, N)
    {
	NewCase[A[i]] = Binding[i];
    }

    return NewCase;
}



void  CheckOriginalCaseCover()
/*    ----------------------  */
{
    Tuple *TSP, Case;

    ClearFlags;
    Current.NOrigPos = Current.NOrigTot = 0;

    for ( TSP = Current.Tuples ; Case = *TSP++ ; )
    {
        if ( ! TestFlag(Case[0]&Mask, TrueBit) )
	{
            SetFlag(Case[0]&Mask, TrueBit);
	    Current.NOrigTot++;
	    if ( Positive(Case) ) Current.NOrigPos++;
	}
    }
}



    /*  Free up a bunch of tuples  */

void  FreeTuples(Tuple *TT, Boolean TuplesToo)
/*    -------  */
{
    Tuple *P;

    if ( TuplesToo )
    {
	for ( P = TT ; *P ; P++ )
	{
	    pfree(*P);
	}
    }

    pfree(TT);
}



    /*  Find log (base 2) factorials using tabulated values and Stirling's
	approximation (adjusted)  */

double  Log2Fact(int n)
/*      --------  */
{
    return ( n < 1000 ? LogFact[n] :
	     (n+0.5) * Log2(n) - n * Log2e + Log2sqrt2Pi - (n*0.7623)/820000 );
}
@//E*O*F state.c//
chmod u=rw,g=,o= state.c
 
echo x - utility.c
sed 's/^@//' > "utility.c" <<'@//E*O*F utility.c//'
#include "defns.i"
#include <sys/time.h>
#include <sys/resource.h>

extern int	VERBOSITY;


	/*  Protected memory allocation routines on call for memory which
	    is not allocated rather than let program continue erroneously */

void  *pmalloc(unsigned arg)
/*     -------  */
{
    void *p;

    p = (void *) malloc(arg);
    if( p || !arg ) return p;
    printf("\n*** malloc erroneously returns NULL\n");
    exit(1);
}



void  *prealloc(void * arg1, unsigned arg2)
/*     --------  */
{
    void *p;

    if ( arg1 == Nil ) return pmalloc(arg2);

    p = (void *)realloc(arg1,arg2); 
    if( p || !arg2 ) return p;
    printf("\n*** realloc erroneously returns NULL\n");
    exit(1);
}



void  *pcalloc(unsigned arg1, unsigned arg2)
/*     -------  */
{
    void *p;

    p = (void *)calloc(arg1,arg2);
    if( p || !arg1 || !arg2 ) return p;
    printf("\n*** calloc erroneously returns NULL\n");
    exit(1);
}



void  pfree(void *arg)
{
    if ( arg ) free(arg);
}



float  CPUTime()
{
    struct rusage usage;

    getrusage(RUSAGE_SELF, &usage);

    return (usage.ru_utime.tv_sec + usage.ru_stime.tv_sec) +
    	   (usage.ru_utime.tv_usec + usage.ru_stime.tv_usec) / 1E6;
}
@//E*O*F utility.c//
chmod u=rw,g=,o= utility.c
 
exit 0
