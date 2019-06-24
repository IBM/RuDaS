#include <stdio.h>
#include "progol.h"

/*
 * #######################################################################
 *
 *			Clause deduction Routines
 *                      -------------------------
 *
 * #######################################################################
 */

/* d_pushfores - copy and skolemise all clauses of given predicate
 *	symbol. Body atoms asserted, head atoms placed in fores array.
 *	Each head atom numbered with clause number. This allows
 *	virtual clause retraction before calling. Array of numbers
 *	returned gives numbers of skolemised unit clauses to be
 *	retracted from each predicate definition by d_popfores.
 */

d_pushfores(psym,retract)
	LONG psym;
	ITEM retract;
	{
	ITEM bcl,cclause,clause,head,pseen=B_EMPTY,
		nums=L_EMPTY,bcls;
	LONG cno,ano;
	if(!fores) {
	  fores=F_EMPTY; bfores=B_EMPTY; catoms=F_EMPTY;
	}
	ano=b_size(bfores);
	if(bcl= *f_ins(psym,bptab)) {
	  BIT_DO(cno,bcl)
	    cclause=i_dec(b_elem(cno,spcls));
	    clause=i_copy(F_ELEM(0l,cclause));
	    head=l_pop(cl_skolem(clause));
	    cl_assatoms(clause,retract,pseen,cno,nums);
		/* Assert Skolemised body */
	    head->extra=cno;
	    b_add(ano,bfores);
	    *f_ins(ano++,fores)=head;
	    i_delete(clause);
	  BIT_END
	  BIT_DO(cno,bcl)
	    cclause=i_dec(b_elem(cno,spcls));
	    cl_leprop(cclause->extra,i_sort(nums));
	  BIT_END
	  cl_psfirstarg(pseen);
	}
	i_deletes(pseen,nums,(ITEM)I_TERM);
}

d_popfores(retract)
	ITEM retract;
	{
	cl_retatoms(retract);
	i_deletes(fores,bfores,(ITEM)I_TERM);
	bfores=fores=(ITEM)NULL;
}


/* d_tredundant - returns bitset of logically redundant clauses
 *	having psym as head predicate symbol
 */

ITEM
d_tredundant(psym)
	LONG psym;
	{
	ITEM retract=Y_EMPTY,brem;
	d_pushfores(psym,retract);
	brem=d_tredundant1();
	d_popfores(retract);
	i_delete(retract);
	return(brem);
}

ITEM
d_tredundant1()
	{
	register ITEM brem=B_EMPTY,atom,*entry;
	register LONG cno,ano;
	BIT_DO(ano,bfores)
	  b_rem(cno=(atom=F_ELEM(ano,fores))->extra,bclauses);
	  catoms->extra=cno;
	  if(*(entry=f_ins(cno,catoms))) b_uni(bclauses,*entry);
	  if(d_groundcall(atom)) b_add(cno,brem);
	  else b_add(cno,bclauses);
	  if(*entry) b_sub(bclauses,*entry);
	BIT_END
	b_uni(bclauses,brem);
	return(brem);
}

/* d_treduce - find redundant clauses in defn of psym and retract them.
 */

d_treduce(psym)
	LONG psym;
	{
	ITEM brem,defs;
	reducing=TRUE;
	brem=d_tredundant(psym);
	reducing=FALSE;
	defs=F_ELEM(psym,ptab);
	if(!b_emptyq(brem)) {
	  g_message(1l,"%d redundant clauses retracted from %s/%d",
		b_size(brem),QP_ntos(psym),QP_ntoa(psym));
	  b_lsub(defs,brem);
	  b_sub(bclauses,brem);
	  b_sub(F_ELEM(psym,bptab),brem);
	}
	cl_pfirstarg(psym);
	i_delete(brem);
}

PREDICATE
d_groundcall(atom)
	ITEM atom;
	{
	ITEM *a;
	PREDICATE result;
	*(a=&(HOF(TOF((LIST)I_GET(F_ELEM(0l,gcall))))))=atom;
	result=interp_quest(gcall);
	*a=(ITEM)NULL;
	return(result);
}

/* d_gcpush - prepares call for cover testing.
 */

ITEM
d_gcpush(clause)
	ITEM clause;
	{
	ITEM result,head,eq;
	result=i_tup4(clause,i_dec(I_INT(cl_vmax(clause))),
		(ITEM)NULL,(ITEM)NULL);
	head=l_pop(clause);
	eq=i_tup3(i_dec(i_create('h',peq2)),NULL,i_dec(head));
	l_push(NULL,l_push(i_dec(eq),clause));
	return(result);
}

/* d_gcpop - undoes preparation of clause for cover testing.
 */

d_gcpop(clause)
	ITEM clause;
	{
	ITEM eq;
	l_pop(clause);
	eq=l_pop(clause);
	l_push(F_ELEM(2l,eq),clause);
	i_delete(eq);
}

/* cl_push - adds clause to end of definition table.
 */

LIST *
cl_push(clause)
	ITEM clause;
	{
	ITEM *entry,cclause;
	LONG psym=PSYM(HOF((LIST)I_GET(clause))),
		*cno= (LONG *)&(I_GET(F_ELEM(2l,spcls)));
	LIST *end;
	cclause=i_tup4(clause,i_dec(I_INT(cl_vmax(clause))),
		i_dec(i_create('h',(POINTER)pdot0)),NULL);
	if(!(*(entry=f_ins(psym,ptab)))) *entry=L_EMPTY;
	end=(LIST *)(*entry)->extra;
	l_suf(i_dec(cclause),*entry);
	cclause->extra=(*cno)++;
	cl_pfirstarg(psym);
	return(end);
}

/* cl_pop - remove last clause from definition table.
 */

int
cl_pop(end)
	LIST *end;
	{
	ITEM cclause=(ITEM)L_GET(*end);
	LONG psym=PSYM(HOF((LIST)I_GET(F_ELEM(0l,cclause)))),
		*cno= (LONG *)&(I_GET(F_ELEM(2l,spcls))),cno1=cclause->extra;
	ITEM *entry;
	if(!(*(entry=f_ins(psym,ptab))))
	  d_error("cl_pop - bad definition");
	i_delete(cclause);
	L_DEL(*end);
	*end=(LIST)NULL;
	(*entry)->extra=(ULONG)end;
	if(cno1==*cno+1l) --(*cno);
	cl_pfirstarg(psym);
}

/* cl_pcoverage - counts the cost of clauses made redundant by the
 *	assertion of the new clause.
 */


double
cl_pcoverage(call,ccl)
	ITEM call,ccl;
	{
	LONG cno,cno1,ano,result=0l,psym=PSYM(HOF(CLAUSE(ccl)));
	ITEM *atom=ATMP(call),*entry;
	double dresult;
	if(!cover) {
	  hyp1=ccl;			/* hypothesis being tested */
	  b_add(cno1=ccl->extra,bclauses);
	}
	BIT_DO(ano,bfores)
	  cno=(*atom=F_ELEM(ano,fores))->extra;
	  catoms->extra=cno;
	  if(*(entry=f_ins(cno,catoms))) b_uni(bclauses,*entry);
	  b_rem(cno,bclauses);
	  if(cover && psym==PSYM(*atom)) { /* Test foreground clause */
	    if(interp_quest(call)) result+= *y_ins(cno,costs);
	  }		/* Otherwise test background clause */
	  else if(d_groundcall(*atom)) result+= *y_ins(cno,costs);
	  b_add(cno,bclauses);
	  if(*entry) b_sub(bclauses,*entry);
	BIT_END
	*atom=(ITEM)NULL;
	if(!cover) {
	  hyp1=(ITEM)NULL;		/* hypothesis being tested */
	  b_rem(cno1,bclauses);
	}
	dresult=result;
	return(dresult*inflate/100.0);
}

/* cl_ncoverage - returns the number of headless clauses which succeed.
 */

#define NINTEGRITY(e,c)	(GRNDQ(e) && l_length(F_ELEM(0l,e))==2 && \
			  (PSYM(HOF(TOF((LIST)I_GET(F_ELEM(0l,(e))))))== \
			   PSYM(HOF((LIST)I_GET(F_ELEM(0l,(c)))))))

PREDICATE r_posonly();
double d_generality();

double
cl_ncoverage(negq,call,ccl)
	PREDICATE negq;
	ITEM call,ccl;
	{
	double result=0.0;
	LONG cno,cno1;
	ITEM *entry,ccl1,cl,*atom=ATMP(call);
	if(negq) {
	  if(interp_quest(ccl)) result+= l_length(F_ELEM(0l,ccl))-1l;
	}
	else if(*(entry=f_ins(pfalse0,ptab))) {
	  LIST_DO(ccl1,*entry)
	    if(b_memq(cno1=ccl1->extra,bclauses)) {
	      if(NINTEGRITY(ccl1,ccl)) {
			/* Cover test ground atoms */
	        *atom=HOF(TOF((LIST)I_GET(F_ELEM(0l,ccl1))));
	        if(interp_quest(call)) result+= *y_ins(cno1,costs);
	      }
	    }
	  LIST_END
	}
	*atom=(ITEM)NULL;
	return(result);
}

/* cl_cq - test integrity constraints. Return false if any fail.
 */

PREDICATE
cl_cq(negq,ccl)
	PREDICATE negq;
	ITEM ccl;
	{
	PREDICATE result;
	LONG cno,cno1;
	ITEM *entry,ccl1;
	if(negq) {
	  if(interp_quest(ccl)) result+= l_length(F_ELEM(0l,ccl))-1l;
	}
	else if(*(entry=f_ins(pfalse0,ptab))) {
	  LIST_DO(ccl1,*entry)
	    if(b_memq(cno1=ccl1->extra,bclauses)) {
	      if(NINTEGRITY(ccl1,ccl))
		continue;
	      else {				/* Assert clause and test */
		hyp1=ccl;			/* hypothesis being tested */
	        b_add(cno=ccl->extra,bclauses);
	        result=(!interp_quest(ccl1));
		hyp1=(ITEM)NULL;		/* hypothesis being tested */
	        b_rem(cno,bclauses);
		if(!result) return(FALSE);
	      }
	    }
	  LIST_END
	}
	return(TRUE);
}

/* cl_dcoverage - calculates the number of detractors as N=gm
 * This follows from quasi-Bayes I(H|E) = I(H)-m(1-g(H))

	I(H|E)-I(E|E) = (I(H)-m(1-g(H)))-(I(E)-m(1-g(E)))
	I(E|E)-I(H|E) = (I(E)+m ln(g(E)))-(I(H)+m ln(g(H)))
		      = (I(E)-I(H))-m(ln(1/g(E))-ln(1/g(H)))
 */

double
cl_dcoverage(call,m)
	ITEM call;
	double m;
	{
	double result,gen,pre;
	ITEM *atom=ATMP(call);
	result=m*d_generality(call,atom);
	/* gen=d_generality(call,atom,&pre);
	result=m*(gen-pre); */
	*atom=(ITEM)NULL;
	return(result);
}

/*
 * d_generality -the generality of a clause is defined as the sum
 *	of the probability of instances entailed by the clause.
 *	This is estimated on a randomly drawn sample.
 */

double
d_generality(call,atom)
	ITEM call,*atom;
	{
	register LONG cnt,total=cnt=0l,
		psym=PSYM(F_ELEM(2l,HOF(TOF((LIST)I_GET(F_ELEM(0l,call))))));
	register ITEM sample,atom1,bproved;
	double rhat,result,fcnt,ftotal;
	if(!(sample= *(f_ins(psym,psamps))))
	  d_error("d_generality - no sample of given predicate");
	if(!(bproved= *(f_ins(psym,bpsamps))))
	  d_error("d_generality - no proved sample of given predicate");
	LIST_DO(atom1,sample)
	  *atom=atom1;
	  if(!b_memq(atom1->extra,bproved)&&interp_quest(call)) cnt++;
	  total++;
	LIST_END
	fcnt=cnt; ftotal=total; rhat=(fcnt+1.0)/(ftotal+2.0);
				/* Laplace correction */
	result=rhat; /* +sqrt(rhat*(1-rhat)/ftotal); */
				/* Correction by expected error */
	return(result);
}

/* d_creduce - destructively reduces clause.
 */

ITEM
d_creduce(cclause,relative)
	ITEM cclause;
	PREDICATE relative;
	{
	ITEM clause0=F_ELEM(0l,cclause),clause,head0,retract=Y_EMPTY,atom,
		pseen=B_EMPTY,entry=(ITEM)NULL,term,bvars;
	register LIST *elemp;
	LONG cno=(LONG)I_GET(F_ELEM(2l,spcls));
	ct_bvars(head0=l_pop(clause0),bvars=B_EMPTY);
	cl_skolem(clause=i_copy(clause0),TRUE);
	LIST_DO(atom,clause0) /* Skolemise variables from head in body */
	  TERM_DO(term,atom)
	    if(term->item_type== 'v'&&b_memq((LONG)I_GET(term),bvars))
	      term->item_type= 'k';
	  TERM_END
	LIST_END
	cl_assatoms(clause,retract,pseen,cno,NULL); /* Assert Skolemised body */
	cl_psfirstarg(pseen);
	if(*f_ins(cno,catoms))  {
	  entry=b_copy(*f_ins(cno,catoms));
	  if(relative) b_uni(entry,bclauses);
	  i_swap(bclauses,entry);
	}
	l_push((ITEM)NULL,clause0);
	elemp= &(TOF((LIST)I_GET(clause0)));
	while(*elemp) {
	  atom=L_GET(*elemp);
	  b_rem(cno,bclauses);
	  if(interp_quest(cclause)) L_REM(elemp)
	  else {
	    b_add(cno,bclauses);
	    elemp = &((*elemp)->next);
	  }
	  cno++;
	}
	if(entry) i_swap(bclauses,entry);
	l_pop(clause0);
	LIST_DO(atom,clause0) /* Deskolemise variables from head in body */
	  TERM_DO(term,atom)
	    if(term->item_type== 'k'&&b_memq((LONG)I_GET(term),bvars))
	      term->item_type= 'v';
	  TERM_END
	LIST_END
	l_push(head0,clause0);
	cl_retatoms(retract);
	i_deletes(entry,clause,retract,head0,pseen,bvars,(ITEM)I_TERM);
	return(cclause);
}

/* d_sample(p,n) - generates sample of n atoms from stochastic interpretation
 *	of predicate p.
 */

ITEM
d_sample(psym,n)
	LONG psym,n;
	{
	ITEM *var,atom,atom1,result=L_EMPTY,call;
	LONG vno=0l,arity;
	PREDICATE warned=FALSE;
	if(!(*f_ins(psym,ptab))) {
	  g_message(1l,"Generator predicate %s/%d has no definition",
		QP_ntos(psym),QP_ntoa(psym));
	  return(result);
	}
	if(arity=QP_ntoa(psym)) {
	  atom=i_create('f',f_create(arity+1l));
	  F_ELEM(0l,atom)=i_create('h',psym);
	  ARG_DO(var,atom)
	    *var=i_create('v',vno++);
	  ARG_END
	}
	else atom=i_create('h',psym);
	call=i_tup4(l_push(NULL,l_push(i_dec(atom), i_dec(L_EMPTY))),
		i_dec(I_INT(vno)),NULL,NULL);
	SET(sampling);
	while(n-- >0l) {
	  if(interp(call,TRUE,TRUE)) {
	    atom1=ct_skolem(p_copy(atom,(BIND)term_stack0,FALSE));
	    l_suf(i_dec(atom1),result);
	    interp((ITEM)NULL,TRUE,TRUE);
	  }
	  else if(!warned) {
	    g_message(1l,"WARNING: predicate %s/%d failed when sampling",
		QP_ntos(psym), QP_ntoa(psym));
	    warned=TRUE;
	  }
	}
	UNSET(sampling); i_delete(call);
	return(result);
}

/* d_cimplied(ccl) - check whether given clause is implied by
 *	existing background knowledge. Clause is temporarily
 *	added to spcls and checked for redundancy using d_tredundant.
 */

PREDICATE
d_cimplied(ccl)
	ITEM ccl;
	{
	LONG cno,psym;
	ITEM bpcls,bred;
	PREDICATE result;
	cno=(LONG)I_GET(F_ELEM(2l,spcls)); /* Add new clause to spcls */
	I_GET(F_ELEM(2l,spcls))=(POINTER)(cno+1l);
	ccl->extra=cno;
	*f_ins(cno,F_ELEM(0l,spcls))=i_inc(ccl);
	psym=PSYM(HOF((LIST)I_GET(F_ELEM(0l,ccl))));
	bpcls=F_ELEM(psym,bptab);
	F_ELEM(psym,bptab)=b_add(cno,B_EMPTY); /* Test clause implied */
	result= !b_emptyq(bred=d_tredundant(psym));
	i_deletes(bred,F_ELEM(psym,bptab),(ITEM)I_TERM);
	F_ELEM(psym,bptab)=bpcls;
	i_delete(F_ELEM(cno,F_ELEM(0l,spcls)));
	F_ELEM(cno,F_ELEM(0l,spcls))=(ITEM)NULL;
	I_GET(F_ELEM(2l,spcls))=(POINTER)cno;
	return(result);
}

