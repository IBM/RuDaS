/* ####################################################################### */

/*                      PROGOL Clause Routines                              */
/*                      ---------------------                              */

/* ####################################################################### */

#include        <stdio.h>
#include        "progol.h"

/*
 * cl_read - reads in a clause from a file. If file ended then
 *	returns item end_of_file.
 */

#define START	0
#define MINUS	1
#define BELEM	2

ITEM
cl_read(get1ch,unget1ch,vtable,type,varno)
	char (*get1ch) ();
	LONG (*unget1ch) ();
	ITEM vtable;
	LONG *type,*varno;
	{
	register char c,state;
	ITEM result=L_EMPTY,term=(ITEM)NULL;
	LONG terminator;
	*varno=0l;
	state=START;
	for(;;) {
		c=(*get1ch) ();
		switch (state) {
		    case START:
			switch (c) {
			    case '\0': /* End of file */
				(*unget1ch) (c);
				i_delete(result);
				return((ITEM)I_TERM);
			    case ' ': case '\t': case '\n': /* White space */
				break;
			    case '%':	/* Comment till end of line */
				while ((c=(*get1ch) ()) != '\n') {
				  if (c == '\0') {
				    (*unget1ch) (c);
				    i_delete(result);
				    return((ITEM)I_TERM);
				  }
				}
				break;
			    case ':':	/* headless body */
				l_suf((ITEM)NULL,result);
				state=MINUS;
				break;
			    default:
				(*unget1ch) (c);
				term=exp_read(get1ch,unget1ch,vtable,FALSE,
					&terminator,varno);
				if (term==(ITEM)I_ERROR||term==(ITEM)I_TERM) {
				    i_delete(result);
				    return(term);
				}
				else l_suf(i_dec(term),result);
				if(terminator==pif) state=BELEM;
				else if(terminator==pdot0||terminator==pcut||
					terminator==pquest) {
				    *type=terminator;
				    return(result);
				}
				else if(terminator==pcomma) {
				    l_push((ITEM)NULL,result);
				    state=BELEM;
				}
				else {
				    i_delete(result);
				    return((ITEM)I_ERROR);
				}
			}
			break;
		    case MINUS:
			if (c == '-') {
			    state=BELEM;
			}
			else {
			    (*unget1ch) (c);
			    i_delete(result);
			    return((ITEM)I_ERROR);
			}
			break;
		    case BELEM:
		        (*unget1ch) (c);
			term=exp_read(get1ch,unget1ch,vtable,FALSE,
				&terminator,varno);
			if (term==(ITEM)I_ERROR||term==(ITEM)I_TERM) {
			    i_delete(result);
			    return(term);
			}
			else l_suf(i_dec(term),result);
			if(terminator==pcomma) state=BELEM;
			else if (terminator==pdot0||terminator==pcut||
				terminator==pquest) {
			    *type=terminator;
			    return(result);
			}
			else {
			    i_delete(result);
			    return((ITEM)I_ERROR);
			}
			break;
		    default:
			d_error("cl_read - bad state number");
		}
	}
}

/*
 * cl_fread - reads a Prolog clause from the given file.
 */

ITEM
cl_fread(in)
	FILEREC *in;
	{
	register ITEM result,vtable;
	char c;
	LONG type,varno;
	glob_file = in;
	result = cl_read(get1ffile,unget1ffile,vtable=F_EMPTY,&type,&varno);
	if (result == (ITEM)I_ERROR) {
		printf("[Syntax error at line %ld in file <%s>]\n",
			glob_file->line_no,glob_file->filename);
		while((c=get1ffile()) != '\n' && c!='\0'); /* Ignore line */
		if(c=='\0') result=(ITEM)I_TERM;
		else unget1ffile(c);
	}
	i_delete(vtable);
	return(result);
}

ITEM
cl_ttyread(type)
	LONG *type;
	{
	ITEM result,vtable;
	LONG varno;
	register char c;
	result = cl_read(get1ftty,unget1ftty,vtable=F_EMPTY,type,&varno);
	if (result == (ITEM)I_ERROR) {
		printf("[Syntax error]\n");
		while((c=get1ftty()) != '\n' && c!='\0');/* Ignore line */
		if(c=='\0') result=(ITEM)I_TERM;
		else unget1ftty(c);
	}
	i_delete(vtable);
	return(result);
}

/* cl_sread(s) read a clause from string s
 */

ITEM
cl_sread(s)
	STRING s;
	{
	register ITEM result;
	ITEM vtable=F_EMPTY;
	LONG type,varno;
	glob_str = s;
	result = cl_read(get1fstring,unget1fstring,vtable,&type,&varno);
	i_delete(vtable);
	if (result == (ITEM)I_ERROR)
		d_error("cl_sread - syntax error");
	return(result);
}

/* cl_vextract(vtable,vno) - constructs a mapping of numbers to variable names
 *	from the hash table of variable names.
 */

ITEM
cl_vextract(vtable,vno)
	ITEM vtable;
	{
	ITEM *rec,elem,*entry,vtable1,vnseen=b_allones(vno),null;
	LONG vno1;
	vtable1=F_EMPTY;
	FUNC_DO(rec,vtable)
	    if(*rec)
	      LIST_DO(elem,*rec)
		b_rem(vno1=(LONG)I_GET(F_ELEM(1l,elem)),vnseen);
		if(*(entry=f_ins(vno1,vtable1)))
		  d_error("cl_vextract - bad variable table");
		else *entry=i_inc(F_ELEM(0l,elem));
	      LIST_END
	FUNC_END
	null=i_create('s',strsave("_"));
	BIT_DO(vno1,vnseen)
	  *f_ins(vno1,vtable1)=i_inc(null);
	BIT_END
	i_deletes(null,vnseen,(ITEM)I_TERM);
	return(vtable1);
}

/*
 * cl_write - write out a clause in standard Prolog form.
 */

int
cl_write(i,put1ch,vtable,subst)
	ITEM i,vtable;
	LONG (*put1ch) ();
	BIND subst;
	{
	LIST lits=(LIST)I_GET(i),body=TOF(lits),elem;
	ITEM head=(ITEM)HOF(lits),lit;
	LONG vnum=0l;
	PREDICATE headq=(head&&(PSYM(head)!=pfalse0));
	if (i->item_type != 'l') d_error("cl_write - not passed a list");
	charlast=SEP;
	if (headq) p_write(head,put1ch,vtable,&vnum,INF,"fx",subst);
	if (headq && body) (*put1ch) (' ');
	if (body) {
		(*put1ch) (':'); (*put1ch) ('-'); (*put1ch) (' ');
		PENL_LOOP(elem,body) {
		    lit= L_GET(elem);
		    charlast=SEP;
		    p_write(lit,put1ch,vtable,&vnum,INF,"fx",subst);
		    (*put1ch) (','); (*put1ch) (' ');
		}
		lit= L_GET(elem);
		charlast=SEP;
		p_write(lit,put1ch,vtable,&vnum,INF,"fx",subst);
	}
}

/*
 * cl_fwrite - writes clause to given file in Prolog format.
 */

int
cl_fwrite(out,i,subst)
	FILEREC *out;
	ITEM i;
	BIND subst;
	{
	ITEM vtable=F_EMPTY;
	line_cnt=0l;
	instring=FALSE;
	glob_file=out;
	charlast=SEP;
	cl_write(i,put1tfile,vtable,subst);
	put1tfile('.');
	frecflush(glob_file);
	i_delete(vtable);
}

cl_print(clause)
	ITEM clause;
	{
	cl_fwrite(ttyout,clause,(BIND)NULL);
	printf("\n");
}

int
cl_swrite(s,i)
	STRING s;
	ITEM i;
	{
	ITEM vtable=F_EMPTY;
	glob_str = s;
	cl_write(i,put1tstring,vtable,(BIND)NULL);
	put1tstring('.');
	put1tstring('\0');
	i_delete(vtable);
}

int
at_fwrite(out,atom)
	FILEREC *out;
	ITEM atom;
	{
	ITEM clause;
	cl_fwrite(out,clause=l_push(atom,L_EMPTY),(BIND)NULL);
	i_delete(clause);
}

int
cl_showas(out,atoms)
	ITEM atoms;
	FILEREC *out;
	{
	register LIST elem;
	LIST_LOOP(elem,(LIST)I_GET(atoms)) {
		at_fwrite(out,L_GET(elem)); i_fnl(out);
	}
}

int
cl_showrls(out,rls)
	ITEM rls;
	FILEREC *out;
	{
	register LIST elem;
	LIST_LOOP(elem,(LIST)I_GET(rls)) {
		cl_fwrite(out,L_GET(elem),(BIND)NULL);
		i_fnl(out);
	}
}

/* cl_vmax(clause) - returns the maximum variable + 1
 */
LONG ct_vmax();

LONG
cl_vmax(clause)
	ITEM clause;
	{
	LONG vmax=-1l,vmax1;
	ITEM atom;
	LIST_DO(atom,clause)
	  if((vmax1=ct_vmax(atom))>vmax) vmax=vmax1;
	LIST_END
	return(vmax+1l);
}

LONG
ct_vmax(term)
	ITEM term;
	{
	LONG vmax=-1l,vmax1;
	ITEM subt;
	TERM_DO(subt,term)
	  if(subt->item_type== 'v'&&(vmax1=(LONG)I_GET(subt))>vmax)
		vmax=vmax1;
	TERM_END
	return(vmax);
}

/* cl_vrenum(clause) - destructively renumbers the variables in
 *	a clause from 0.
 */

ITEM
cl_vrenum(clause,varno)
	ITEM clause;
	LONG *varno;
	{
	ITEM vars=F_EMPTY,atom,term,term1,*entry;
	*varno=0l;
	LIST_DO(atom,clause)
	  TERM_DO(term,atom)
	    if(term->item_type== 'v') {
	      if(!(*(entry=h_ins(term1=i_copy(term),vars))))
		*entry=i_create('v',(POINTER)(*varno)++);
	      I_GET(term)=I_GET(*entry);
	      i_delete(term1);
	    }
	  TERM_END
	LIST_END
	i_delete(vars);
	return(clause);
}

LONG
cl_nvars(clause)
	ITEM clause;
	{
	ITEM atom,term,vars=B_EMPTY;
	LONG result;
	LIST_DO(atom,clause)
	  if(atom)
	    TERM_DO(term,atom)
	      if(term->item_type=='v') b_add((LONG)I_GET(term),vars);
	    TERM_END
	LIST_END
	result=b_size(vars);
	i_delete(vars);
	return(result);
}

ITEM cl_vttyread();

/* ccl_read - reads in a clause and returns a 4-tuple
 *	1) Clause (list of atoms)
 *	2) N, the number of variables in the clause
 *	3) Type, one of `?`, `!`, `.` depending on terminator
 *	4) Vtable, is a table of variable names indexed by variable numbers
 */

ITEM
ccl_read(get1ch,unget1ch)
	char (*get1ch) ();
	LONG (*unget1ch) ();
	{
	LONG type,vno,varno;
	ITEM vtable,vtable1,clause,head,*var;
	clause=cl_read(get1ch,unget1ch,vtable=F_EMPTY,&type,&varno);
	if(clause==(ITEM)I_ERROR||clause==(ITEM)I_TERM) {
	  i_delete(vtable);
	  return(clause);
	}
	else if(type==pquest) {
	  if(l_length(clause)!=1l) {
	    if(HOF((LIST)I_GET(clause))) { /* Clause as question not allowed */
	      i_deletes(vtable,clause,(ITEM)I_TERM);
	      return((ITEM)I_ERROR);
	    }
	    l_pop(clause);
	  }
	  head=i_create('f',f_create(varno+1l));
	  F_ELEM(0l,head)=i_create('h',QP_ston("$ans",0l)); vno=0l;
	  ARG_DO(var,head)
	    *var=i_create('v',vno++);
	  ARG_END
	  l_push(i_dec(head),clause);
	}
	else {
	  if(!HOF((LIST)I_GET(clause)))
	    HOF((LIST)I_GET(clause))=i_inc(fhead);
	}
	vtable1=cl_vextract(vtable,varno); i_delete(vtable);
	return(i_tup4(i_dec(clause),
			   i_dec(i_create('i',varno)),
			   i_dec(i_create('h',type)),
			   i_dec(vtable1)));
}

ITEM
ccl_ttyread()
	{
	ITEM result;
	register char c;
	result = ccl_read(get1ftty,unget1ftty);
	if (result == (ITEM)I_ERROR) {
		printf("[Syntax error]\n");
		while((c=get1ftty()) != '\n' && c!='\0');/* Ignore line */
		if(c=='\0') result=(ITEM)I_TERM;
		else unget1ftty(c);
	}
	return(result);
}

/*
 * ccl_fread - reads a Prolog clause from the given file.
 */

ITEM
ccl_fread(in)
	FILEREC *in;
	{
	register ITEM result;
	char c;
	glob_file = in;
	result = ccl_read(get1ffile,unget1ffile);
	if (result == (ITEM)I_ERROR) {
		printf("[Syntax error at line %ld in file <%s>]\n",
			glob_file->line_no,glob_file->filename);
		while((c=get1ffile()) != '\n' && c!='\0'); /* Ignore line */
		if(c=='\0') result=(ITEM)I_TERM;
		else unget1ffile(c);
	}
	return(result);
}

/* ccl_sread(s) read a clause from string s
 */

ITEM
ccl_sread(s)
	STRING s;
	{
	char mess[MAXMESS];
	register ITEM result;
	glob_str = s;
	result = ccl_read(get1fstring,unget1fstring);
	if (result == (ITEM)I_ERROR || result == (ITEM)I_TERM) {
		sprintf(mess,"ccl_sread - syntax error\n\t%s",s);
		d_error(mess);
	}
	return(result);
}


int
ccl_write(cclause,put1ch,subst)
	ITEM cclause,subst;
	LONG (*put1ch) ();
	{
	LONG type=(LONG)I_GET(F_ELEM(2l,cclause));
	ITEM vtable,clause,head;
	PREDICATE quest;
	clause=F_ELEM(0l,cclause);
	vtable=(subst?(ITEM)NULL:F_ELEM(3l,cclause));
	if(quest=(CTYPE(cclause)==pquest)) {
	  head=l_pop(clause); l_push(NULL,clause);
	}
	cl_write(clause,put1ch,vtable,subst);
	if(quest) {
	  l_pop(clause); l_push(i_dec(head),clause);
	}
	if(type==pdot0) (*put1ch) ('.');
	else if(type==pquest) (*put1ch) ('?');
	else if(type==pcut) (*put1ch) ('!');
}

/*
 * ccl_fwrite - writes clause to given file in Prolog format.
 */

int
ccl_fwrite(out,cclause,subst)
	FILEREC *out;
	ITEM cclause;
	BIND subst;
	{
	line_cnt=0l;
	instring=FALSE;
	glob_file=out;
	charlast=SEP;
	ccl_write(cclause,put1tfile,subst);
	frecflush(glob_file);
}

int
ccl_swrite(s,i)
	STRING s;
	ITEM i;
	{
	glob_str = s;
	ccl_write(i,put1tstring,(BIND)NULL);
	put1tstring('\0');
}

/* ccl_print - print clause to tty followed by newline.
 */

ccl_print(cclause)
	ITEM cclause;
	{
	ccl_fwrite(ttyout,cclause,NULL);
	i_fnl(ttyout);
}
