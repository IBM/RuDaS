/* ####################################################################### */

/*			PROGOL initialise globals			   */
/*			-------------------------			   */

/* ####################################################################### */

#include <stdio.h>
#include "progol.h"

LONG memout;
FILEREC *ttyin;
FILEREC *ttyout;
FILEREC *plgin;
FILEREC *plgout;
ITEM spatoms,spsyms,spcls,ops,ptab,bptab,pftab,pvtab,ptopf,comms,cclauses;
ITEM bfores,bclauses,lib,lib1,psymtlib,spies,commutes,repeats,labels;
ITEM hmodes,bmodes,determs,hypothesis,hyp1,idot0,store,indexed,osplit;
ITEM types,op,allpsyms,nums,fores,catoms,clt,cle,sple,fhead,gcall,eof,ptog;
ITEM psamps,bpsamps,costs,prune,bppar,bpchd,bpntab,bpstk,catoms1,clt1,cle1;
ITEM aux,observables;
double noiselim,clim;
LONG pprune0,inflate,stage;
LONG vlim,pnlim,hlim,rlim,pdot2,pempty,pminus,pplus,phash,pdiv2,errorchk;
LONG pif,pcomma,pcomma2,pdot0,pcut,pquest,pplus2,pminus2,ptimes2,pplus1;
LONG pminus1,phash1,pceil1,pfloor1,prand0,pcputime0,pfalse0,pimplies2;
LONG phat2,pmod2,peq2,ptimes0,plog1,pexp1,por2,pand2,psin1,pcos1,ptan1,ptrue;
LONG pasin1,pacos1,patan1;
LONG charlast,verbose,*hxy[HASH10+1l],hxyn,ple2,plt2,pge2,pgt2;
LONG pnolearn0,psaturate0,prefine0;
PREDICATE cover,condition,conditioning,memoing,posonly,reductive,sampling,
	searching,splitting,tracing,full_pruning,reducing;
PREDICATE pdebug,namecmp,noquotes,reconsult,mseen;
STRING editor;

ITEM tag=(ITEM)0x401bb458;

/*****************************************************************
 *  c_open/0 - creates and initialises global data structures.
 */

c_open()
	{
	LONG **iptr;
	LONG cnt;
	memout = 0;
	if (memout) printf("Mem out = %ld\n",memout);
	a_zero_table();
#ifdef MEMCHECK
	{
	FUNC f;
	ITEM *fptr;
        all_items = (ITEM)PROGOL_CALLOC(1l, sizeof(struct item));
	all_items->item_type='f';
	all_items->usage=1;
	if (!(f = (FUNC)PROGOL_CALLOC(1l, sizeof(struct functor))))
		d_error("c_open - calloc failure 1");
	else if (!(f->arr = (ITEM *)PROGOL_CALLOC(HASH10+1l,sizeof(ITEM))))
		d_error("c_open - calloc failure 2");
	else
		f->arr_size= HASH10+1l;
	FUNC_LOOP(fptr,f) *fptr=(ITEM)NULL; 
	all_items->obj=(POINTER)f;
	}
#endif
	/* SRAND(1l); */
	ttyin=plgin=frecreate("stdin"); ttyin->file=stdin;
	ttyout=plgout=frecreate("stdout"); ttyout->file=stdout;
	spsyms=b_spcreate(10l);		/* Superset of predicate and function symbols */
	nums=i_create('f',(POINTER)f_create(2*MAXN)); /* num ITEMS */
	for(cnt= -MAXN;cnt<MAXN;cnt++) F_ELEM(cnt+MAXN,nums)=I_INT(cnt);
	ops=i_create('f',(POINTER)f_zero(f_create(64l))); /* Operators */
	stage=NOLEARN;			/* Learning stage (user testable) */
	ptab=F_EMPTY;			/* Indexed predicate definitions */
	bptab=F_EMPTY;			/* Indexed bitset definitions */
	pftab=F_EMPTY;			/* Pred/func sym indexed clauses */
	pvtab=F_EMPTY;			/* Pred/var indexed clauses */
	ptopf=F_EMPTY;			/* Pred->pred/func mapping */
	bppar=F_EMPTY;			/* Pred->parents bitset */
	bpchd=F_EMPTY;			/* Pred->children bitset */
	bpntab=F_EMPTY;			/* Pred->bitset ndefs (Pred in body) */
	bpstk=F_EMPTY;			/* Pred->bitset Stickelled clauses */
	psamps=F_EMPTY;			/* Pred-> atom samples */
	bpsamps=F_EMPTY;		/* Pred-> unproved subsample */
	bmodes=F_EMPTY;			/* Table of body IO modes */
	hmodes=F_EMPTY;			/* Table of head IO modes */
	osplit=F_EMPTY;			/* Tabulated variable partitions */
	mseen=FALSE;			/* Modes seen */
	determs=F_EMPTY;		/* Table of determinations */
	spatoms=b_spcreate(10l);	/* Ground atom bit superset */
	spcls=b_spcreate(10l);		/* Clause superset */
	cclauses=F_ELEM(0l,spcls);	/* Table of cclauses */
	fores=(ITEM)NULL;		/* List of foreground atoms */
	bfores=(ITEM)NULL;		/* Bitset of foreground atoms */
	catoms=(ITEM)NULL;		/* Asserted sk atoms (testing) */
	cle=(ITEM)NULL;			/* LE rel for a's in catoms clause */
	clt=(ITEM)NULL;			/* LT rel for a's in catoms clause */
	sple=b_spcreate(6l);		/* LE object superset */
	catoms1=(ITEM)NULL;		/* Asserted sk atoms (saturating) */
	cle1=(ITEM)NULL;		/* LE rel for a's in catoms1 clause */
	clt1=(ITEM)NULL;		/* LT rel for a's in catoms1 clause */
	bclauses=B_EMPTY;		/* Bitset of clauses */
	lib=B_EMPTY;			/* Built-in C predicates */
	lib1=B_EMPTY;			/* Built-in C+Prolog predicates */
	aux=B_EMPTY;			/* Auxilliary C predicates */
	allpsyms=B_EMPTY;		/* All background predicate symbols */
	op=B_EMPTY;			/* Operators */
	types=B_EMPTY;			/* Types used in mode declarations */
	psymtlib=Y_EMPTY;		/* Mapping psym -> lib function no */
	labels=Y_EMPTY;			/* Labels for stocastic LPs */
	costs=Y_EMPTY;			/* Costs of clauses */
	ptog=Y_EMPTY;			/* psym -> generator-psym */
	hypothesis=(ITEM)NULL;
	hyp1=(ITEM)NULL;
	c_reset();
	c_initops();			/* Initialise operator precedences */
	pdot2=QP_ston(".",2l);		/* ./2 */
	pempty=QP_ston("[]",0l);	/* []/0 */
	plog1=QP_ston("log",1l);	/* log/1 */
	pexp1=QP_ston("exp",1l);	/* exp/1 */
	psin1=QP_ston("sin",1l);	/* sin/1 */
	pcos1=QP_ston("cos",1l);	/* cos/1 */
	ptan1=QP_ston("tan",1l);	/* tan/1 */
	pasin1=QP_ston("asin",1l);	/* asin/1 */
	pacos1=QP_ston("acos",1l);	/* acos/1 */
	patan1=QP_ston("atan",1l);	/* atan/1 */
	pceil1=QP_ston("ceil",1l);	/* ceil/1 */
	pfloor1=QP_ston("floor",1l);	/* floor/1 */
	prand0=QP_ston("random",0l);	/* random/0 */
	pcputime0=QP_ston("cputime",0l);/* cputime/0 */
	pfalse0=QP_ston("false",0l);	/* false/0 */
	pnolearn0=QP_ston("nolearn",0l);/* nolearn/0 */
	psaturate0=QP_ston("saturate",0l);/* saturate/0 */
	prefine0=QP_ston("refine",0l);/* refine/0 */
	pprune0=QP_ston("prune",0l); /* prune/0 */
	pminus=QP_ston("-",0l);		/* -/0 */
	pminus1=QP_ston("-",1l);	/* -/1 */
	pminus2=QP_ston("-",2l);	/* -/2 */
	pplus=QP_ston("+",0l);		/* +/0 */
	pplus1=QP_ston("+",1l);		/* +/1 */
	pplus2=QP_ston("+",2l);		/* +/2 */
	por2=QP_ston("\\/",2l);		/* \/ /2 */
	pand2=QP_ston("/\\",2l);	/* /\ /2 */
	phash=QP_ston("#",0l);		/* #/0 */
	phash1=QP_ston("#",1l);		/* #/1 */
	pif=QP_ston(":-",0l);		/* ':-'/0 */
	pcomma=QP_ston(",",0l);		/* ','/0 */
	pimplies2=QP_ston(":-",2l);	/* ':-'/2 */
	ple2=QP_ston("=<",2l);		/* '=<'/2 */
	plt2=QP_ston("<",2l);		/* '<'/2 */
	pge2=QP_ston(">=",2l);		/* '>='/2 */
	pgt2=QP_ston(">",2l);		/* '>'/2 */
	pcomma2=QP_ston(",",2l);	/* ','/2 */
	pdot0=QP_ston(".",0l);		/* '.'/0 */
	pdiv2=QP_ston("/",2l);		/* '/'/2 */
	ptimes2=QP_ston("*",2l);	/* '*'/2 */
	ptimes0=QP_ston("*",0l);	/* '*'/0 */
	phat2=QP_ston("^",2l);		/* '^'/2 */
	pmod2=QP_ston("mod",2l);	/* 'mod'/2 */
	peq2=QP_ston("=",2l);		/* '='/2 */
	pcut=QP_ston("!",0l);		/* ! */
	pquest=QP_ston("?",0l);		/* ? */
	ptrue=QP_ston("true",0l);	/* true */
	idot0=i_create('h',(POINTER)pdot0); /* . */
	prune=i_create('h',(POINTER)pprune0); /* Abandon atom */
	reconsult=FALSE;		/* Reconsulting a file? */
	pdebug=FALSE;			/* Trace debugger flag */
	namecmp=FALSE;			/* Name comparison */
	noquotes=FALSE;			/* No quotes on Prolog constants */
	spies=B_EMPTY;			/* Spy points */
	observables=B_EMPTY;		/* Obervable predicates */
	commutes=B_EMPTY;		/* Symmetric predicates */
	repeats=B_EMPTY;		/* Repeat library predicates */
	indexed=B_EMPTY;		/* Predicates indexed on first arg */
	charlast=SEP;			/* Used for printing Prolog expressions */
	store=(ITEM)NULL;		/* Used by l_record */
	comms=L_EMPTY;			/* Commands read from files */
	editor="";			/* Default editor */
	fhead=i_create('h',(POINTER)pfalse0); /* False atom for heads of
					   non-definite Horn clauses */
	eof=i_create('h',(POINTER)QP_ston("end_of_file",0l));/* end_of_file/0 */
	gcall=i_tup4(l_push((ITEM)NULL,l_push((ITEM)NULL, i_dec(L_EMPTY))),
		i_dec(I_INT(0l)),(ITEM)NULL,(ITEM)NULL);
					/* Calling clause for ground goal */
	for(iptr=hxy+HASH10+1l;--iptr>=hxy;*iptr=(LONG *)NULL);
	hxyn=0l;			/* Zero numbering scheme for hxy */
}

/*****************************************************************
 *  c_close/0 - deletes global data structures.
 */

c_close()
	{
	LONG **iptr;
	frecdelete(ttyin); frecdelete(ttyout);
	if(plgin!=ttyin) frecdelete(plgin);
	if(plgout!=ttyout) frecdelete(plgout);
	i_deletes(fileroot,spsyms,store,pftab,pvtab,ptopf,(ITEM)I_TERM);
	i_deletes(spatoms,spcls,sple,lib,lib1,osplit,eof,psamps,(ITEM)I_TERM);
	i_deletes(bfores,hmodes,bmodes,determs,hypothesis,(ITEM)I_TERM);
	i_deletes(ops,psymtlib,ptab,bptab,fhead,gcall,ptog,(ITEM)I_TERM);
	i_deletes(comms,op,types,bclauses,idot0,indexed,costs,(ITEM)I_TERM);
	i_deletes(allpsyms,nums,spies,bpsamps,bppar,bpchd,bpntab,(ITEM)I_TERM);
	i_deletes(commutes,repeats,labels,prune,bpstk,aux,observables,(ITEM)I_TERM);
	for(iptr=hxy+HASH10+1l;--iptr>=hxy;) if(*iptr) xy_adelete(*iptr);
	spsyms=(ITEM)NULL;
#ifdef MEMCHECK
	{
	FUNC f;
	ITEM *felem;
	LIST elem;

	i_print_all();
	f= (FUNC)I_GET(all_items);
	FUNC_LOOP(felem,f) {
	    if (*felem) {
		while(elem=(LIST)I_GET(*felem)) {
		    (LIST)I_GET(*felem)=elem->next;
		    L_DEL(elem);
		}
	        PROGOL_CFREE(*felem,sizeof(struct item));
	    }
	}
	PROGOL_CFREE(f->arr,(HASH10+1l)*sizeof(ITEM));
	PROGOL_CFREE(f,sizeof(struct functor));
	PROGOL_CFREE(all_items,sizeof(struct item));
	}
#endif
	if (memout) printf("\nMem out = %ld\n",memout);
	else if (interactive) printf("\n");
}

/* c_initops/0 - initialise operator precedences and associativities
 *	This is initialised to all operator settings in Clocksin/Mellish
 *	minus	'.'/2, spy/1 and nospy/1
 *	plus	^/2
 */

struct op_prec {
	STRING operator;
	LONG precedence;
	STRING assoc;
};

struct op_prec prolops[] = {
	":-", 255, "xfx",
	"?-", 255, "fx",
	";", 254, "xfy",
	"->", 253, "xfy",
	",", 252, "xfy",
	/* "not", 60, "fx", */
	"\\+", 60, "fx",
	"is", 40, "xfx",
	"=..", 40, "xfx",
	"=", 40, "xfx",
	"\\=", 40, "xfx",
	"@<", 40, "xfx",
	"@=<", 40, "xfx",
	"@>=", 40, "xfx",
	"@>", 40, "xfx",
	"<", 40, "xfx",
	"=<", 40, "xfx",
	">=", 40, "xfx",
	">", 40, "xfx",
	"==", 40, "xfx",
	"\\==", 40, "xfx",
	"+/-", 35, "xfx",
	"-", 31, "yfx",
	"+", 31, "yfx",
	"/\\", 25, "yfx",
	"\\/", 25, "yfx",
	"/", 21, "yfx",
	"*", 21, "yfx",
	"#", 15, "fx",
	"+", 15, "fx",
	"-", 15, "fx",
	"mod", 11, "xfx",
	"^", 10, "xfy",
	"log", 10, "fx",

        0, 0, 0
};

c_initops()
	{
	struct op_prec *optr;
	ITEM *entry,astring;
	LONG psym;
	for (optr=prolops;optr->operator;optr++) {
	  b_add(psym=QP_ston(optr->operator,0l),op);
	  if(strlen(optr->assoc)==2l) psym=QP_ston(optr->operator,1l);
	  if(*(entry=f_ins(psym,ops))) d_error("c_initops - redeclaration of operator");
	  astring=i_create('s',strsave(optr->assoc));
	  *entry=i_tup2(i_dec(astring),i_dec(I_INT(optr->precedence)));
	}
}

/* Progol execution settings
 */

struct setrec settings[] = {
	"c", REAL, (POINTER)&clim,
	"condition", BOOLEAN, (POINTER)&condition,
	"cover", BOOLEAN, (POINTER)&cover,
	/* "errorcheck", NATURAL, (POINTER)&errorchk, */
	"h", NATURAL, (POINTER)&hlim,
	"r", NATURAL, (POINTER)&rlim,
	"i", NATURAL, (POINTER)&vlim,
	"inflate", NATURAL, (POINTER)&inflate,
	"memoing", BOOLEAN, (POINTER)&memoing,
	"nodes", NATURAL, (POINTER)&pnlim,
	"noise", REAL, (POINTER)&noiselim,
	"posonly", BOOLEAN, (POINTER)&posonly,
	"reductive", BOOLEAN, (POINTER)&reductive,
	"sampling", BOOLEAN, (POINTER)&sampling,
	"searching", BOOLEAN, (POINTER)&searching,
	"splitting", BOOLEAN, (POINTER)&splitting,
	"tracing", BOOLEAN, (POINTER)&tracing,
	"full_pruning", BOOLEAN, (POINTER)&full_pruning,
	"verbose", NATURAL, (POINTER)&verbose,

        0, 0, 0
};

/* c_reset - sets Progol parameters to default settings.
 */

c_reset()
	{
	hlim=30l;	/* Interpreter depth bound */
	rlim=400l;	/* Resolution limit */
	vlim=3l;	/* Depth of variable chains in bottom clause */
	clim=4.0;	/* Hypothesised clause body bound */
	pnlim=200l;	/* Search node limit */
	noiselim=0.0;	/* Amount of noise allowed */
	inflate=105l;	/* Example inflation */
	verbose=2l; 	/* Verbosity - (0 = no messages),(1 = command timings),
				(2 = trace search) */
	conditioning=FALSE;	/* Conditioning on? */
	reducing=FALSE;		/* Reducing theory? */
	SET(condition);		/* Condition stochastic l.p. using data? */
	SET(cover);		/* Cover-testing? */
	SET(memoing);		/* Memoing for search? */
	UNSET(posonly);		/* Learn from pos only? */
	UNSET(reductive);	/* Reductive language constraint? */
	UNSET(sampling);	/* Stochastic theorem proving? */
	UNSET(searching);	/* Hypothesis search? */
	SET(splitting);		/* Variable splitting? */
	UNSET(tracing);		/* Prolog interpreter trace? */
	SET(full_pruning);	/* Full pruning of the search? */
}
