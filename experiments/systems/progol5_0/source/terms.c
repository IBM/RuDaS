/* ####################################################################### */

/*                      PROGOL Terms Routines                               */
/*                      --------------------                               */

/* ####################################################################### */

#include        <stdio.h>
#include        "progol.h"

/*
 * p_vname - creates new variable names from numbers. Conversion is
 *	0,1,..,25,26,27,...   becomes
 *	A,B,.., Z,A0,B0,...
 */

ITEM
p_vname(n)
	LONG n;
	{
	char mess[MAXWORD],c=n + 'A';
	if (0 <= n && n <= 25) sprintf(mess,"%c",c);
	else sprintf(mess,"%c%ld",(n%26)+'A',n/26-1);
	return(i_create('s', (POINTER)strsave(mess)));
}

PREDICATE exp_ap();

/*
 * p_write - writes out term in standard Prolog fashion. Used by i_write.
 *	Variables names in order A,B,..,Z,A0,B0,...
 */

#define	GTP(p1,p2,a)	((*a)=='x'?(p1)>(p2):(p1)>=(p2))
#define	RSMALL		0.01
#define	RLARGE		10000.0

p_write(term,put1ch,vtable,vnum,prec,assoc,subst)
	ITEM term;
	LONG (*put1ch) ();
	ITEM vtable;
	LONG *vnum,prec;
	STRING assoc;
	BIND subst;
	{
	char mess[MAXWORD];
	FUNC f;
	ITEM *fptr,ffsym;
	STRING name,assoc1;
	LONG nargs,pnum,prec1;
	if(subst) SKIPVARS(term,subst);
	switch(term->item_type) {
	  case 'f':
		f=(FUNC)I_GET(term);
		nargs=F_SIZE(term)-1;
		pnum=(LONG)I_GET(F_ELEM(0l,term));
		name=QP_ntos(pnum);
		if(nargs==1) ffsym=i_inc(F_ELEM(0l,term));
		else ffsym=i_create('h',QP_ston(name,0l));
		if(exp_ap(ffsym,&assoc1,&prec1)) { /* Operator */
			if(!GTP(prec,prec1,assoc)) {
			    (*put1ch) ('(');
			    charlast=SEP;
			}
			switch(strlen(assoc1)-1) {	/* Arity */
			  case 1:
				if(*assoc1=='f') { /* Prefix */
				    p_swrite(name,pnum,put1ch);
				    noquotes=FALSE;
				    p_write(F_ELEM(1l,term),put1ch,vtable,vnum,prec1,assoc1+1,subst);
				    noquotes=TRUE;
				}
				else { /* Postfix */
				    noquotes=FALSE;
				    p_write(F_ELEM(1l,term),put1ch,vtable,vnum,prec1,assoc1+1,subst);
				    noquotes=TRUE;
				    p_swrite(name,pnum,put1ch);
				}
				break;
			  case 2:
				noquotes=FALSE;
			        p_write(F_ELEM(1l,term),put1ch,vtable,vnum,prec1,assoc1,subst);
				noquotes=TRUE;
			        p_swrite(name,pnum,put1ch);
				noquotes=FALSE;
			        p_write(F_ELEM(2l,term),put1ch,vtable,vnum,prec1,assoc1+2,subst);
				noquotes=TRUE;
				break;
			  default:
				d_error("p_write - bad associativity");
			}
			if(!GTP(prec,prec1,assoc)) {
			    (*put1ch) (')');
			    charlast=SEP;
			}
		}
		else if (pnum==pdot2) {	/* List */
			(*put1ch) ('['); charlast=SEP;
			p_lwrite(term,put1ch,vtable,vnum,subst);
			(*put1ch) (']'); charlast=SEP;
		}
		else {
			noquotes=FALSE;
			p_swrite(name,pnum,put1ch);
			(*put1ch) ('('); charlast=SEP;
			ARG_LOOP(fptr,f) {
				noquotes=FALSE;
				p_write(*fptr,put1ch,vtable,vnum,250l,"fx",subst);
				noquotes=TRUE;
				if(--nargs) {
				    (*put1ch) (',');
				    charlast=SEP;
				}
			}
			(*put1ch) (')'); charlast=SEP;
		}
		i_delete(ffsym);
		break;
	  case 'h':
		name=QP_ntos(pnum=(LONG)I_GET(term));
		p_swrite(name,pnum,put1ch);
		break;
	  case 'v':
		noquotes=TRUE;
		if (vtable) {
		    LONG vno=(LONG)I_GET(term);
		    ITEM *entry;
		    if(!(*(entry=f_ins(vno,vtable)))) /* Already seen this variable */
		        *entry= p_vname((*vnum)++);
		    p_swrite((STRING)I_GET(*entry),INF,put1ch);
		}
		else if (subst) {
		    sprintf(mess,"_%ld",(LONG)(subst-(BIND)stack)+
			(LONG)I_GET(term));
		    p_swrite(mess,INF,put1ch);
		}
		else {
		    sprintf(mess,"_%ld",(POINTER)I_GET(term));
		    p_swrite(mess,INF,put1ch);
		}
		noquotes=FALSE;
		break;
	  case 'i':
		sprintf(mess,"%ld",(LONG)I_GET(term));
		noquotes=TRUE;
		p_swrite(mess,INF,put1ch);
		noquotes=FALSE;
		break;
	  case 'k':
		sprintf(mess,"$sk%ld",(LONG)I_GET(term));
		p_swrite(mess,INF,put1ch);
		break;
	  case 'r':
		{double v= *((double *)I_GET(term)),v1=fabs(v);
		  if(RSMALL <= v1 && v1 < RLARGE)
		    sprintf(mess,"%.3lf",v);
		  else
		    sprintf(mess,"%.3le",v);
		  noquotes=TRUE;
		  p_swrite(mess,INF,put1ch);
		  noquotes=FALSE;
		}
		break;
	  default:
		d_error("p_write - illegal term type");
	}
}

/* p_swrite - writes out Prolog constant
 */

p_swrite(name,pnum,put1ch)
	STRING name;
	LONG pnum;
	LONG (*put1ch) ();
	{
	PREDICATE unquoted,symbolic=lsymbol(name);
	register char *sp;
	unquoted=(noquotes||(symbolic&& *name!= ',')||lalphanum(name)||
		pnum==pempty);
	if(unquoted && charlast!=SEP && ((symbolic && charlast==SYM) ||
		(!symbolic && charlast==ALPHN))) (*put1ch) (' ');
	if (!unquoted) (*put1ch) ('\'');
	STR_LOOP(sp,name) (*put1ch) (*sp);
	if (!unquoted) {(*put1ch) ('\''); charlast=SEP;}
	else charlast= (symbolic?SYM:ALPHN);
}

/*
 * p_lwrite - write out the internals of a Prolog list (without brackets)
 */

p_lwrite(term,put1ch,vtable,vnum,subst)
	ITEM term;
	LONG (*put1ch) ();
	ITEM vtable;
	LONG *vnum;
	BIND subst;
	{
	for(;;) {	/* Loop through a Prolog list */
		p_write(F_ELEM(1l,term),put1ch,vtable,vnum,250l,"fx",subst);
		term=F_ELEM(2l,term);
		if(subst) SKIPVARS(term,subst);
		if(term->item_type == 'f' && (LONG)I_GET(F_ELEM(0l,term))==pdot2)
			{(*put1ch) (','); charlast=SEP;}
		else break;
	}
	if (!(term->item_type == 'h' && (LONG)I_GET(term)==pempty)) {
		(*put1ch) ('|'); charlast=SEP;
		p_write(term,put1ch,vtable,vnum,250l,"fx",subst);
	}
}

/*
 * p_fwrite - writes term to given file in Prolog format.
 */

int
p_fwrite(out,i)
	FILEREC *out;
	ITEM i;
	{
	ITEM vtable=F_EMPTY;
	LONG vnum=0l;
	line_cnt=0l;
	instring=FALSE;
	glob_file=out;
	charlast=SEP;
	p_write(i,put1tfile,vtable,&vnum,250l,"fx",(BIND)NULL);
	frecflush(glob_file);
	i_delete(vtable);
}

/*
 * p_swrite1 - writes term to given string in Prolog format.
 */

int
p_swrite1(s,i)
	STRING s;
	ITEM i;
	{
	ITEM vtable=F_EMPTY;
	LONG vnum=0l;
	instring=FALSE;
	glob_str=s;
	charlast=SEP;
	p_write(i,put1tstring,vtable,&vnum,250l,"fx",(BIND)NULL);
	put1tstring('\0');
	i_delete(vtable);
}

/* ####################################################################### 
 *
 * p_read/5 - reads any Prolog term
 */


#define		START		0
#define		STR		1
#define		INTGR		2
#define		REALN		3
#define		ARGS_START	4
#define		ARG_SEP		5
#define		ARG		6
#define		VRBL		7
#define		ATM		8
#define		LIST_ELEM	9
#define		LIST_SEP	10
#define		LIST_FIN	11
#define		DCARE		12
#define		CLOSEB		13
#define		EXPNT		14

PREDICATE exp_operator();

ITEM
p_read(get1ch,unget1ch,vtable,toplevel,terminator,expect,varno)
	char (*get1ch) ();
	LONG (*unget1ch) ();
	ITEM vtable;
	PREDICATE toplevel;
	LONG *terminator,expect,*varno;
	{
	register char c;
	int state=START;
	char str[MAXMESS];
	register STRING strp;
	LONG nargs,subtermin;
	PREDICATE symbol_atom=FALSE;
	ITEM subterm,term=(ITEM)NULL,termlist=(ITEM)NULL,
		*fptr,*lelem,*lrest;
	FUNC f;
	LIST *last;
	for(;;) {
		c = (*get1ch) ();
		switch (state) {
		    case START:
			switch (c) {
			    case '\0': /* End of file */
				(*unget1ch) (c);
				return((ITEM)I_TERM);
			    case ' ': case '\t': case '\n': /* White space */
				break;
			    case '%':	/* Comment till end of line */
				while ((c=(*get1ch) ()) != '\n') {
				  if (c == '\0') {
				    (*unget1ch) (c);
				    i_delete(term);
				    return((ITEM)I_TERM);
				  }
				}
				break;
			    case '(':	/* Open subexpression */
				term=exp_read(get1ch,unget1ch,vtable,
					TRUE,terminator,varno);
				if(term==(ITEM)I_ERROR||term==(ITEM)I_TERM) {
			            return(term);
				}
				state=CLOSEB;
				break;
			    case '_':			/* Variable */
				strp=str;
				*strp++ = c;
				state=VRBL;
				break;
			    case '0': case '1': case '2': case '3': case '4':
			    case '5': case '6': case '7': case '8': case '9':
				strp=str;		/* Integer */
				*strp++ = c;
				state=INTGR;
				break;
			    case '\'':	/* Atom or compound */
				strp=str;
				state=STR;
				break;
			    case '[':	/* List */
				if ((c=(*get1ch) ())== ']') {
					term = i_create('h',(POINTER)pempty);
					return(term);
				}
				else (*unget1ch) (c);
				term=i_create('f',(POINTER)(f=f_create(3l)));
				fptr=f->arr;
				*fptr++ = i_create('h',(POINTER)pdot2);
				*(lelem=fptr++) = (ITEM)NULL;
				*(lrest=fptr) = (ITEM)NULL;
				state=LIST_ELEM;
				break;
			    /*
			    case '!':
				return(i_create('h',(POINTER)QP_ston("!",0l)));
			     */
			    case ',':
				if(!toplevel) {*terminator=pcomma; return((ITEM)I_ERROR);}
			    case ';': case '!':
			    case '+': case '-': case '*': case '/': case '\\': case '^':
			    case '<': case '>': case '=': case '`': case '~': case ':':
			    case '.': case '?': case '@': case '#': case '$': case '&': 
				symbol_atom=TRUE;
			    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
			    case 'g': case 'h': case 'i': case 'j': case 'k': case 'l':
			    case 'm': case 'n': case 'o': case 'p': case 'q': case 'r':
			    case 's': case 't': case 'u': case 'v': case 'w': case 'x':
			    case 'y': case 'z':
				strp=str;
				*strp++ = c;
				if ((c== '-' || c== '+') && expect==OPND) {
					c=(*get1ch) ();
					if(DIGIT(c)) state=INTGR;
					else state=ATM;
					(*unget1ch) (c);
				}
				else state=ATM;
				break;
			    default:
				if (UPCASE(c)) {		/* Variable */
					strp=str;
					*strp++ = c;
					state=VRBL;
				}
				else {
					(*unget1ch) (c);
					return((ITEM)I_ERROR);
				}
				break;
			}
			break;
		    case ATM:
			switch(c) {
			    case '+': case '-': case '*': case '/': case '\\': case '^':
			    case '<': case '>': case '=': case '`': case '~': case ':':
			    case '@': case '#': case '$': case '&': case '.':
				if(symbol_atom && !(c=='.' && *(strp-1l)=='!'))
				  *strp++ = c;
				else {
				  (*unget1ch) (c);
				  *strp = '\0';
				  term = i_create('h',(POINTER)QP_ston(str,0l));
				  if(exp_operator(term)) return(term);
				  state= ARGS_START;
				}
				break;
			    default:
				if(ALPHANUM(c) && !symbol_atom) *strp++ = c;
				else {
				  (*unget1ch) (c);
				  *strp = '\0';
				  if(!toplevel && expect==OPTR) {
				    if(STREQ(str,":-"))
					{*terminator=pif; return((ITEM)I_ERROR);}
				    else if(STREQ(str,"."))
					{*terminator=pdot0; return((ITEM)I_ERROR);}
				    else if(STREQ(str,"?"))
					{*terminator=pquest; return((ITEM)I_ERROR);}
				    else if(STREQ(str,"!"))
					{*terminator=pcut; return((ITEM)I_ERROR);}
				    else if(STREQ(str,","))
				    	{*terminator=pcomma; return((ITEM)I_ERROR);}
				  }
				  term = i_create('h',(POINTER)QP_ston(str,0l));
				  if(exp_operator(term)) return(term);
				  state= ARGS_START;
				}
			}
			break;
		    case STR:
			if (c == '\0') {
				  (*unget1ch) (c);
				  return((ITEM)I_ERROR);
			}
			else if (c == '\\') *strp++ = (*get1ch) ();
			else if (c == '\'') {
				*strp = '\0';
				term = i_create('h',(POINTER)QP_ston(str,0l));
				if(exp_operator(term)) return(term);
				state= ARGS_START;
			}
			else	*strp++ = c;
			break;
		    case INTGR:
			switch(c) {
			    case '0': case '1': case '2': case '3': case '4':
			    case '5': case '6': case '7': case '8': case '9':
				*strp++ = c;
				break;
			    case '.':
				c=(*get1ch) ();
				if(!DIGIT(c)) {
				    LONG v;
				    (*unget1ch) (c); (*unget1ch) ('.');
				    *strp = '\0';
				    sscanf(str,"%ld",&v);
				    term = i_create('i',(POINTER)v);
				    return(term);
				}
				(*unget1ch) (c);
				*strp++ = '.';
				state = REALN;
				break;
			    case 'e': case 'E':
				if(c== 'E') c= 'e';
				*strp++ = c;
				state = EXPNT;
				break;
			    default:
				{LONG v;
				  (*unget1ch) (c);
				  *strp = '\0';
				  sscanf(str,"%ld",&v);
				  term = i_create('i',(POINTER)v);
				  return(term);
				}
			}
			break;
		    case REALN:
			switch(c) {
			    case '0': case '1': case '2': case '3': case '4':
			    case '5': case '6': case '7': case '8': case '9':
				*strp++ = c;
				break;
			    case 'e': case 'E':
				*strp++ = c;
				state = EXPNT;
				break;
			    default:
				{double *vp; double v;
				  (*unget1ch) (c);
				  *strp = '\0';
				  sscanf(str,"%lf",&v);
				  vp = r_create(v);
				  term = i_create('r',(POINTER)vp);
				  return(term);
				}
			}
			break;
		    case EXPNT:
			switch(c) {
			    case '0': case '1': case '2': case '3': case '4':
			    case '5': case '6': case '7': case '8': case '9':
			    case '-':
				*strp++ = c;
				break;
			    default:
				{double v, *vp;
				  (*unget1ch) (c);
				  *strp = '\0';
				  sscanf(str,"%le",&v);
				  vp = r_create(v);
				  term = i_create('r',(POINTER)vp);
				  return(term);
				}
			}
			break;
		    case ARGS_START:
			if (c == '(') {
				i_delete(term);
				nargs=0l;
				last = L_LAST(termlist=L_EMPTY);
				state = ARG;
			}
			else {
				(*unget1ch) (c);
				return(term);
			}
			break;
		    case ARG:
			(*unget1ch) (c);
			subtermin=(LONG)I_ERROR;
			subterm=exp_read(get1ch,unget1ch,vtable,
				FALSE,&subtermin,varno);
			if (subterm==(ITEM)I_ERROR||subterm==(ITEM)I_TERM) {
			    i_delete(termlist);
			    return(subterm);
			}
			last=l_end(subterm,last);
			nargs++;
			if(subtermin!=pcomma) state=ARG_SEP;
			break;
		    case ARG_SEP:
			switch(c) {
			    case '\0':
				  (*unget1ch) (c);
				  i_delete(termlist);
				  return((ITEM)I_ERROR);
			    case ' ': case '\t': case '\n': /* White space */
				break;
			    case '%':	/* Comment till end of line */
				while (((*get1ch) ()) != '\n');
				break;
			    case ')':
				l_push(i_dec(i_create('h',(POINTER)QP_ston(str,nargs))),
					termlist);
				term = f_ltof(termlist);
				i_delete(termlist);
				return(term);
				break;
			    default:
				i_delete(termlist);
				return((ITEM)I_ERROR);
			}
			break;
		    case LIST_ELEM:
			(*unget1ch) (c);
			subtermin=(LONG)I_ERROR;
			*lelem=exp_read(get1ch,unget1ch,vtable,
				FALSE,&subtermin,varno);
			if (*lelem==(ITEM)I_ERROR||*lelem==(ITEM)I_TERM) {
			    ITEM result= *lelem;
			    *lelem=(ITEM)NULL;
			    i_delete(term);
			    return(result);
			}
			if(subtermin==pcomma) {
			    *lrest=i_create('f',(POINTER)(f=f_create(3l)));
			    fptr=f->arr;
			    *fptr++ = i_create('h',(POINTER)pdot2);
			    *(lelem=fptr++) = (ITEM)NULL;
			    *(lrest=fptr) = (ITEM)NULL;
			}
			else state=LIST_SEP;
			break;
		    case LIST_SEP:
			switch(c) {
			    case '\0':
				  (*unget1ch) (c);
				  i_delete(term);
				  return((ITEM)I_ERROR);
			    case ' ': case '\t': case '\n': /* White space */
				break;
			    case '%':	/* Comment till end of line */
				while (((*get1ch) ()) != '\n');
				break;
			    case '|':
				*lrest=exp_read(get1ch,unget1ch,vtable,
					TRUE,&subtermin,varno);
				if (*lrest==(ITEM)I_ERROR||*lrest==(ITEM)I_TERM) {
				    i_delete(term);
				    return(*lrest);
				}
				state=LIST_FIN;
				break;
			    case ']':
				*lrest = i_create('h',(POINTER)pempty);
				return(term);
				break;
			    default:
				i_delete(term);
				return((ITEM)I_ERROR);
			}
			break;
		    case LIST_FIN:
			switch(c) {
			    case '\0':
				  (*unget1ch) (c);
				  i_delete(term);
				  return((ITEM)I_ERROR);
			    case ' ': case '\t': case '\n': /* White space */
				break;
			    case '%':	/* Comment till end of line */
				while (((*get1ch) ()) != '\n');
				break;
			    case ']':
				return(term);
				break;
			    default:
				i_delete(term);
				return((ITEM)I_ERROR);
			}
			break;
		    case VRBL:	/* Variable names are mapped to unique no.
					using hash table */
			if (ALPHANUM(c)) *strp++ = c;
			else {
			    ITEM name,*entry;
			    (*unget1ch) (c);
			    *strp = '\0';
			    if(STREQ(str,"_"))
				term=i_create('v',(POINTER)(*varno)++);
			    else {
			      name=i_create('s',(POINTER)strsave(str));
			      if (!(*(entry=h_ins(name,vtable)))) /* New var */
				    *entry= I_INT((*varno)++);
			      term = i_create('v',(POINTER)I_GET(*entry));
			      i_delete(name);
			    }
			    return(term);
			}
			break;
		    case CLOSEB: /* End of bracketted expression */
			if (c == '\0') {
				  (*unget1ch) (c);
				  return((ITEM)I_ERROR);
			}
			else if(c== ')') return(term);
			i_delete(term);
			return((ITEM)I_ERROR);
		    default:
			d_error("p_read - bad state number");
		}
	}
}

ITEM
p_ttyread()
	{
	ITEM result,vtable=F_EMPTY;
	LONG terminator,varno=0l;
	if ((result=p_read(get1ftty,unget1ftty,vtable,TRUE,&terminator,
			OPND,&varno))==(ITEM)I_ERROR)
		while(get1ftty() != '\n');	/* Ignore line */
	i_delete(vtable);
	return(result);
}

/* ####################################################################### 
 *
 * exp_read/2 - reads any Prolog expression and maintains a single term lookahead
 *	by reading terms, and updating the variables tpres and tnext.
 *	This is necessary to deal with multiple operator defs,
 *	eg. for '-' as prefix or infix op.
 */

LONG exp_update();
PREDICATE exp_collapse();

ITEM
exp_read(get1ch,unget1ch,vtable,toplevel,terminator,varno)
	char (*get1ch) ();
	LONG (*unget1ch) ();
	ITEM vtable;
	PREDICATE toplevel;
	LONG *terminator,*varno;
	{
	LONG expect=OPND;
	ITEM result=(ITEM)NULL,optrs=L_EMPTY,opnds=L_EMPTY;
	ITEM tpres,tnext=tpres=(ITEM)NULL;
	*terminator=(LONG)I_ERROR;
	while(!result) {
	  if(!tnext) tpres=p_read(get1ch,unget1ch,vtable,toplevel,
		terminator,expect,varno);
	  else tpres= tnext;
	  if(tpres==(ITEM)I_ERROR||tpres==(ITEM)I_TERM) tnext= tpres;
	  else tnext=p_read(get1ch,unget1ch,vtable,toplevel,terminator,
		OPND+OPTR-expect,varno);
	  if((tpres)==(ITEM)I_ERROR||tpres==(ITEM)I_TERM) {
	      if(exp_collapse(optrs,opnds,INF,"xfx")&&(l_length(opnds)==1l))
		      result=l_pop(opnds);
	      else result=tpres;
	      i_deletes(optrs,opnds,(ITEM)I_TERM);
	  }
	  else if((expect=exp_update(tpres,tnext,expect,optrs,opnds))==
		(LONG)I_ERROR) {
	      if(tnext!=(ITEM)I_ERROR && tnext!=(ITEM)I_TERM) i_delete(tnext);
	      i_deletes(tpres,optrs,opnds,(ITEM)I_TERM);
	      result=(ITEM)I_ERROR;
	  }
	  else i_delete(tpres);
	}
	return(result);
}

extern char get1ffile1();
extern LONG unget1ffile();

/*
 * exp_fread - reads a Prolog expression from the given file.
 */

ITEM
exp_fread(in)
	FILEREC *in;
	{
	LONG terminator,varno=0l;
	register ITEM result,vtable;
	char c;
	char (*get1ch) ();
	LONG (*unget1ch) ();
	glob_file=in;
	if(in->file==stdin) {
	  get1ch=get1ftty; unget1ch=unget1ftty;
	}
	else {
	  get1ch=get1ffile; unget1ch=unget1ffile;
	}
	result=exp_read(get1ch,unget1ch,vtable=F_EMPTY,
		FALSE,&terminator,&varno);
	if (result!=(ITEM)I_TERM &&
			(result==(ITEM)I_ERROR||terminator!=pdot0)) {
		printf("[Syntax error at line %ld in file <%s>]\n",
			glob_file->line_no,glob_file->filename);
		while((c=get1ch()) != '\n' && c!= '\0'); /* Ignore line */
		unget1ch();
		if(result!=(ITEM)I_ERROR) i_delete(result);
		result=(ITEM)I_ERROR;
	}
	i_delete(vtable);
	return(result);
}

/*
 * exp_sread - reads a Prolog expression from the given string.
 */

ITEM
exp_sread(s)
	char *s;
	{
	ITEM result;
	ITEM vtable=F_EMPTY;
	LONG terminator,varno=0l;
	glob_str = s;
	result=exp_read(get1fstring,unget1fstring,vtable,FALSE,
		&terminator,&varno);
	if(result==(ITEM)I_ERROR) result=(ITEM)NULL;
	i_delete(vtable);
	return(result);
}

/* exp_update/5 - updates the operator and operand stacks */

PREDICATE exp_prefix(),exp_postfix();
LONG exp_prec();

LONG
exp_update(term,tnext,expect,optrs,opnds)
	ITEM term,tnext,optrs,opnds;
	LONG expect;
	{
	STRING assoc;
	LONG prec;
	PREDICATE post;
	switch(expect) {
	  case OPND:
	    if(exp_prefix(term,tnext)) l_push(term,optrs);
	    else {
	      l_push(term,opnds);
	      expect=OPTR;
	    }
	    break;
	  case OPTR:
	    post=exp_postfix(term,tnext);
	    if(!exp_ap(term,&assoc,&prec)|| *assoc=='f') return((LONG)I_ERROR);
	    else if(post) {
	      l_push(term,optrs);
	      if(!exp_collapse(optrs,opnds,prec,"yf")) return((LONG)I_ERROR);
	    }
	    else if(!L_EMPTYQ(optrs)&&GTP(prec,exp_prec(HOF((LIST)I_GET(optrs))),assoc)) {
	      if(!exp_collapse(optrs,opnds,prec,assoc)) return((LONG)I_ERROR);
	      l_push(term,optrs);
	    }
	    else l_push(term,optrs);
	    expect=(strlen(assoc)==3?OPND:OPTR);
	    break;
	  default:
	    d_error("exp_update - bad expectation value");
	}
	return(expect);
}

/* exp_operator/1 - decides whether term is an operator */

PREDICATE
exp_operator(term)
	ITEM term;
	{
	if(term->item_type!='h') return(FALSE);
	else return(b_memq((LONG)I_GET(term),op));
}

PREDICATE
exp_ap(term,assoc,prec)
	ITEM term;
	STRING *assoc;
	LONG *prec;
	{
	ITEM *entry;
	if(term->item_type!='h') return(FALSE);
	else if (!(*(entry=f_ins((ITEM)I_GET(term),ops)))) return(FALSE);
	*assoc=(STRING)I_GET(F_ELEM(0l,*entry));
	*prec=(LONG)I_GET(F_ELEM(1l,*entry));
	return(TRUE);
}

/* exp_prefix/1 - decides whether an operator is prefix */

PREDICATE
exp_prefix(term,tnext)
	ITEM term,tnext;
	{
	ITEM *entry;
	LONG psym;
	if(tnext==(ITEM)I_ERROR) return(FALSE);
	if(term->item_type!='h') return(FALSE);
	psym=(LONG)I_GET(term);
	psym=QP_ston(QP_ntos(psym),1l);
	if (!(*(entry=f_ins(psym,ops)))) return(FALSE);
	else if (*((STRING)I_GET(F_ELEM(0l,*entry)))!='f') return(FALSE);
	I_GET(term)=(POINTER)psym;
	return(TRUE);
}

/* exp_postfix/1 - decides whether an operator is postfix.
 *	This is decided on whether there is a next term
 *	and whether the operator is defined as a postfix
 *	and infix operator using the following truthtable.
 *		next pref inf
 *		F    F    -    F
 *		F    T    -    T
 *		T    -    T    F
 *		T    T    F    T
 *		T    F    F    F
 *	This gives preference of infix over postfix.
 */

PREDICATE
exp_postfix(term,tnext)
	ITEM term,tnext;
	{
	ITEM *entry;
	LONG psym1,psym0;
	if(term->item_type!='h') return(FALSE);
	psym0=(LONG)I_GET(term);
	psym1=QP_ston(QP_ntos(psym0),1l);
	if (!(*(entry=f_ins(psym1,ops)))) return(FALSE);
	else if (*((STRING)I_GET(F_ELEM(0l,*entry))+1l)!='f') return(FALSE);
	else if (tnext==(ITEM)I_ERROR|| !(*(entry=f_ins(psym0,ops)))) {
	    I_GET(term)=(POINTER)psym1;
	    return(TRUE);
	}
	return(FALSE);
}

LONG
exp_prec(term)
	ITEM term;
	{
	STRING assoc;
	LONG prec;
	exp_ap(term,&assoc,&prec);
	return(prec);
}

LONG
exp_arity(term)
	ITEM term;
	{
	STRING assoc;
	LONG prec;
	exp_ap(term,&assoc,&prec);
	return(strlen(assoc)-1l);
}

/* exp_collapse/3 - collapses operator and operand stacks until either
 *	one of them is empty or the term's precedence is less than that
 *	at the top of the operator stack.
 */

PREDICATE
exp_collapse(optrs,opnds,prec,assoc)
	ITEM optrs,opnds;
	LONG prec;
	STRING assoc;
	{
	LONG arity,cnt;
	ITEM op,terml,function;
	while(!L_EMPTYQ(optrs)&&!L_EMPTYQ(opnds)&&
		GTP(prec,exp_prec(HOF((LIST)I_GET(optrs))),assoc)) {
	    op=l_pop(optrs); arity=exp_arity(op);
	    terml=L_EMPTY;
	    for(cnt=1l;cnt<=arity;cnt++)
		if(L_EMPTYQ(opnds)) {
		    i_deletes(op,terml,(ITEM)I_TERM);
		    return(FALSE);
		}
		else l_push(i_dec(l_pop(opnds)),terml);
	    function=i_create('h',QP_ston(QP_ntos((LONG)I_GET(op)),arity));
	    l_push(i_dec(f_ltof(l_push(function,terml))),opnds);
	    i_deletes(op,terml,function,(ITEM)I_TERM);
	}
	return(TRUE);
}

/*
 * p_fwritesub - writes term to given file in Prolog format
 *	using the substitution
 */

int
p_fwritesub(out,term,subst)
	FILEREC *out;
	ITEM term;
	BIND subst;
	{
	line_cnt=0l;
	instring=FALSE;
	glob_file=out;
	charlast=SEP;
	p_write(term,put1tfile,(ITEM)NULL,(LONG *)NULL,250l,"fx",subst);
	frecflush(glob_file);
}

/*
 * p_copy(term,subst) - makes a copy of the term with the substitution applied.
 */

ITEM
p_copy1(term,subst,subst0)
	ITEM term;
	BIND subst,subst0;
	{
	FUNC f1,f2;
	ITEM result,*fptr1,*fptr2;
	if(subst) SKIPVARS(term,subst);
	switch(term->item_type) {
	  case 'f':
	    f1=(FUNC)I_GET(term);
	    result=i_create('f',f2=f_create(F_SIZE(term)));
	    FNAME(f2)=i_copy(FNAME(f1));
	    ARG_LOOP2(fptr1,fptr2,f1,f2)
		*fptr2=p_copy1(*fptr1,subst,subst0);
	    break;
	  case 'v':
	    {LONG v=(subst-subst0)+(LONG)I_GET(term);
	    result=i_create('v',(POINTER)v);}
	    break;
	  default:
	    result=i_copy(term);
	    break;
	}
	return(result);
}

ITEM
p_copy(term,subst,absolute)
	ITEM term;
	BIND subst;
	PREDICATE absolute;
	{
	if(absolute) return(p_copy1(term,subst,(BIND)stack));
	else return(p_copy1(term,subst,subst));
}

/*
 * p_swritesub - writes term to given string with substitution.
 */

int
p_swritesub(s,term,subst)
	STRING s;
	ITEM term;
	BIND subst;
	{
	ITEM vtable=F_EMPTY;
	LONG vnum=0l;
	instring=FALSE;
	glob_str=s;
	charlast=SEP;
	p_write(term,put1tstring,vtable,&vnum,250l,"fx",subst);
	put1tstring('\0');
	i_delete(vtable);
}
