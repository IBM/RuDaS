#include <stdio.h>
#include "progol.h"

/*
 * #######################################################################
 *
 *			Clause reduction Routines
 *                      -------------------------
 *
 * #######################################################################
 */

/* cl_amap - produces a mapping from body-atom sequence number to
 *	atom number. Returns an integer array structure.
 */
#define SKIPA 	-RINF

#define DOBIND(h) {ITEM nin,nout; r_precombo(atom,vmap,&nin,&nout,h); \
		    r_bind(acode,atom,vmap,vmax,nin,nout,h,NULL,bdvout); \
		    l_suf(i_dec(atom),clause); \
		    i_deletes(nin,nout,(ITEM)I_TERM);}

ITEM
cl_amap(clause)
	ITEM clause;
	{
	ITEM result=Y_EMPTY,atom;
	LIST_DO(atom,clause)
	  Y_PUSH(atom->extra,result);
	LIST_END
	return(result);
}


void r_showchoice(s)
	ITEM s;
	{
	ITEM p=(ITEM)R_IPARENT(s);
	if(!p) return;
	r_showchoice(p);
	if(A_CODE(s)==SKIPA) printf("0");
	else printf("1");
}


/* cl_unflatten - unflattens the clause by taking all equalities of the
 *	form X=f(Y,Z,..) and forming a substitution using the interpreter
 *	stack. The unflattened clause is then read off using p_copy.
 */

cl_unflatten(clause)
	ITEM *clause;
	{
	BIND sub,sub1;
	LONG cnt;
	ITEM atom,clause1,clause2,var;
	for(cnt=cl_vmax(*clause),sub=(BIND)term_stack0;cnt>0l;cnt--,sub++) {
	  sub->subst=(BIND)NULL;	/* Clear term stack */
	  sub->term=(ITEM)NULL;
	}
	clause1=L_EMPTY; sub=(BIND)term_stack0;
	LIST_DO(atom,*clause)
	  if(atom->item_type=='f' &&
		(LONG)I_GET(F_ELEM(0l,atom))==peq2 &&
		(var=F_ELEM(1l,atom))->item_type== 'v') {
	    sub1=sub+(LONG)I_GET(var); /* Place substitution */
	    if(sub1!=sub) sub1->subst=sub;
	    sub1->term=F_ELEM(2l,atom);
	  }
	  else l_suf(atom,clause1);
	LIST_END
	clause2=L_EMPTY;
	LIST_DO(atom,clause1)
	  l_suf(i_dec(p_copy(atom,sub,FALSE)),clause2); /* Apply sub */
	LIST_END
	i_deletes(*clause,clause1,(ITEM)I_TERM); 
	*clause=clause2;
}

/* cl_symreduce - removes all commutative atoms from the body of the clause
 *	which are equivalent up to input variable reordering.
 */

cl_symreduce(clause,atoio,head1)
	ITEM *clause,atoio,head1;
	{
	ITEM htab=h_create(6l),clause1=L_EMPTY,rec,rec1,ivars,ovars,term,head,
		atom,*entry;
	LONG psym;
	head=l_pop(*clause);
	if(b_memq(psym=PSYM(head1),commutes)) {
	  rec1=i_tup3(i_dec(i_create('h',psym)),ivars=i_dec(B_EMPTY),
		ovars=i_dec(B_EMPTY));
	  TERM_DO(term,head1)
	    if(term->item_type=='v') {
	      if(IS(OUT,term->extra)) b_add((LONG)I_GET(term),ovars);
	      else b_add((LONG)I_GET(term),ivars);
	    }
	  TERM_END
	  *h_ins(rec1,htab)=i_inc(head);
	  i_delete(rec1);
	}
	LIST_DO(atom,*clause)
	  if(b_memq(psym=PSYM(atom),commutes)) {
	    rec=F_ELEM(atom->extra,atoio);
	    rec1=i_tup3(i_dec(i_create('h',psym)),F_ELEM(1l,rec),
		F_ELEM(2l,rec));
	    if(!(*(entry=h_ins(rec1,htab)))) {
	      *entry=i_inc(atom);
	      l_suf(atom,clause1);
	    }
	    i_delete(rec1);
	  }
	  else l_suf(atom,clause1);
	LIST_END
	l_push(i_dec(head),clause1);
	i_deletes(*clause,htab,(ITEM)I_TERM);
	*clause=clause1;
}

/* r_search 
  A*-like algorithm based on pseudo-compression
  T is background knowledge.
  C = h<-B is most specific i-bounded clause in mode language s.t. T/\C |= e.
  Search space is C' = h<-B' where B' subset of B. Open list is sorted
  in descending order on heuristic function
  
  	g = (c+n)-p		[neg. pseudo-compression]
  	  = (c+negcover(C'))-poscover(C')	[(c+n)-p]
  
  	where cardinality c = |B'|
  	
  Let bestclosed = <g0,p0,c0>
  Let Open = <<g1,p1,c1>,..<gn,pn,cn>>. Search terminates when
  n0=0 and p0>pi for 2<=i<=p. Proof that search returns consistent clause
  with maximum positive coverage is immediate from the fact that
  p1>pi and p decreases monotonically along any path in the search.
  When there is more than one clause with this coverage, the one with
  maximum compression will be returned.
  
  Search pruned for
  	a) Children of node i with ni=0
  	b) Children of node with pi<ci  [there can be no eventual compression]
  	c) Any node i with gi<g0 when n0=0
	d) Any node with ci>=Clause-length limit

  Lower-bound distance to goal h can be introduced by tabulating
  	the "distance", minimal number of atoms from
  	each atom to an atom which computes the output variables
  	in the bottom clause. This number is minimised over a given clause
  	to give h, the number of required atoms to compute output variables.
  	When no outputs in the head then distance is zero for
  	all atoms in bottom. Atoms not on path to output
  	variables are given a large non-negative distance value.
  
  Actual state record used is the following integer array
  
	f - g+h = ((c+n)-p)+h
  	p - positive coverage (n calcuated as f+p-(c+h))
  	c - the clause length
	h - min. no atoms to output
  	a/u - encoded binding of chosen atom
  	par - parent state
  
 */

ITEM r_mkstate(),r_expand(),r_clauseof(),r_minxerr();
PREDICATE r_atgoal(),r_better(),r_posonly();
LONG r_maxminh();


r_search(clause,atoio,otoa,outlook,vdomains,fnex,best)
	ITEM *clause,atoio,otoa,outlook,vdomains,*best;
	double fnex;
	{
	ITEM head,open,bestopen,children,start,*entry,oldbest= *best,
		amap,atom,term,closed,equiv=h_create(10l),*elem,
		vouts=B_EMPTY,vins=B_EMPTY,batoms,vvis,vknown,mins;
	LONG v,vmax=0l,aindex=0l,explored=0l;
	double m=0.0,negs=0.0,maxh=0.0,bestpass=0.0;
	PREDICATE bestchanged,negq;
	stage=REFINE;
	if(negq=(PSYM(HOF((LIST)I_GET(*clause)))==pfalse0))
	  i_delete(l_pop(*clause));
	r_mark(*clause);	/* Mark the variables in extra fields */
	head=l_pop(*clause);
	batoms=B_EMPTY;
	LIST_DO(atom,*clause)
	  b_add(atom->extra,batoms);
	LIST_END
	FUNC_DO(elem,otoa)
	  if(*elem) b_int(*elem,batoms);
	FUNC_END
	TERM_DO(term,head)
	  if(term->item_type=='v') {
	    if((v=(LONG)I_GET(term))>vmax) vmax=v;
	    b_add((LONG)I_GET(term),(IS(OUT,term->extra)?vouts:vins));
	  }
	TERM_END
	vmax++;
	if(!b_emptyq(vouts)) {
	  maxh=r_maxminh(vouts,vins,atoio,otoa,vdomains,mins=Y_EMPTY,
		vvis=B_EMPTY,vknown=B_EMPTY);
	  i_deletes(vvis,vknown,mins,(ITEM)I_TERM);
	}
	amap=cl_amap(*clause);
	/* Initialise open list and best */
	m=fnex*100.0/inflate;
	negs=fnex;
	start=r_mkstate(m,negs,0.0,maxh,0l,(ITEM)NULL,fnex);
	open=l_push(i_dec(start),L_EMPTY); closed=L_EMPTY;
	if(!(*best)) *best=i_inc(start);
	do {	/* Search loop */
	  bestopen=l_pop(open);
	  bestchanged=FALSE;
	  if(!R_PRUNE(bestopen)) {
	    children=r_expand(bestopen,head,atoio,otoa,amap,vdomains,
		vins,vouts,vmax,&explored,best,&bestchanged,&bestpass,
		equiv,negq,fnex);
	    set_uni(open,children); i_delete(children);
	  }
	  l_push(i_dec(bestopen),closed);
	  if(explored>=pnlim) {
	    printf("Resource limit exceeded\n");
	    break;
	  }
	} while(!L_EMPTYQ(open)&& !r_atgoal(*best,open,bestchanged));
	i_delete(*clause); *clause=(ITEM)NULL;
	g_message(2l,"%d explored search nodes",explored);
	if(verbose>=2l) printf("f=%.0f,p=%.0f,n=%.0f,h=%.0f\n",
			F_VAL(*best),P_VAL(*best),N_VAL(*best),
			H_VAL(*best));
	if(R_PASS(*best) && *best!=oldbest)
		*clause=r_clauseof(*best,head,atoio,amap,&aindex,(ITEM)NULL,
		&vmax,(ITEM)NULL);
	if(negq&& *clause) l_push(fhead,*clause);
	i_deletes(head,amap,open,closed,equiv,(ITEM)I_TERM);
	i_deletes(batoms,vouts,vins,(ITEM)I_TERM);
	stage=NOLEARN;
}

/*
  Actual state record used is the following integer array
  
	f - g+h = ((c+n)-p)+h
  	p - positive coverage (n calcuated as f+p-(c+h))
  	a - attractors (used for learning from positives only)
  	c - the clause length
	h - min. no atoms to output
  	a/u - encoded binding of chosen atom
  	par - parent state
 */

ITEM
r_mkstate(p,n,c,h,choice,parent,fnex)
	ITEM parent;
	double p,n,c,h,fnex;
	LONG choice;
	{
	double f,fch=choice,fpar=(LONG)parent;
	ITEM state=R_EMPTY;
	f=(fnex/p)*((c+n+h)-p);
	/* f=(fnex/p)*(c+h)+fnex*log(n/p)/log(2.0); */
	R_PUSH(h,state);	/* No. of extra atoms to complete I/O */
	R_PUSH(f,state);	/* Evaluation function */
	R_PUSH(-p,state);	/* Positive examples covered */
	R_PUSH(n,state);	/* Negative examples covered */
	R_PUSH(c,state);	/* Length of clause body */
	R_PUSH(fch,state);	/* Binding choice for last literal */
	R_PUSH(fpar,state);	/* Pointer to parent search state */
	state->extra = (1<<6);	/* Used to avoid i_cmp comparison of
					pointers */
	return(state);
}

/* r_expand - constructs children of given state s. This is done
 *	by constructing the clause c associated with s. Then
 *	add next atom a onto c. Then find permutations of
 *	valid bindings for a. Permutations found by calculating
 *	number n of permutations given variable mapping vmap from
 *	bottom variables to vars in c. Then count from 0 to n
 *	and decode count to a particular binding. Each clause
 *	c,l is tested on pos and neg examples and resulting
 *	state records returned.
 */

LONG r_precombo(),r_minh();
LIST *cl_push();
extern double cl_dcoverage();
#define	HV	((h>0.0||(b_subseteq(vouts,b_uni(bdvout1,bdvout))))?h:1.0)

ITEM
r_expand(s,head,atoio,otoa,amap,vdomains,vins,vouts,vmax,explored,
		best,bestchanged,bestpass,equiv,negq,fnex)
	ITEM s,head,atoio,otoa,amap,vdomains,vins,vouts,equiv,*best;
	LONG vmax,*explored;
	double fnex;
	PREDICATE *bestchanged;
	double *bestpass;
	PREDICATE negq;
	{
	ITEM c,vmap,vmap1,children=L_EMPTY,atom,s1,nin,nout,nin1,nout1,
		*entry,sname=(ITEM)NULL,bdvin=(ITEM)NULL,bdvout=B_EMPTY,
		bdvout1=(ITEM)NULL;
	LONG max,m,aindex=0l,vmax1;
	double p,n,clen,h,h1,r;
	char mess[MAXMESS];
	PREDICATE headq= !R_IPARENT(s),cq;
	c=r_clauseof(s,head,atoio,amap,&aindex,&vmap,&vmax,bdvout);
	if(negq) l_push(fhead,c);
	if(!headq && aindex>=Y_SIZE(amap)) { /* No more extension */
	  i_deletes(c,vmap,bdvout,(ITEM)I_TERM);
	  return(children);
	}
	atom=i_copy(headq?head:F_ELEM(0l,F_ELEM(Y_ELEM(aindex,amap),atoio)));
	h=H_VAL(s);
	l_suf(i_dec(atom),c);
	clen=l_length(c);
	if((h1=r_minh(vins,vouts,c,atoio,otoa,vdomains))<h) h=h1;
	max=r_precombo(atom,vmap,&nin,&nout,headq);
	for(m=0l;m<max;m++) { /* For each binding permutation */
	  vmax1=vmax; vmap1=i_copy(vmap); i_delete(bdvin); i_delete(bdvout1);
	  r_bind(m,atom,vmap1,&vmax1,nin1=i_copy(nin),nout1=i_copy(nout),headq,
		bdvin=B_EMPTY,bdvout1=B_EMPTY);
	  i_deletes(sname,vmap1,nin1,nout1,(ITEM)I_TERM);
	  sname=(ITEM)NULL;
	  if(!headq) /* Check I/0 connectness */
	      if(!b_subseteq(bdvin,bdvout)) continue;
          cl_swrite(mess,c);
	  if(!SETQ(memoing)|| !(*(entry=
		h_ins(sname=i_create('s',strsave(mess)),equiv)))) {
	    ITEM c1,call=d_gcpush(c1=i_copy(c));
	    LIST *end=cl_push(c);
	    hyp1=L_GET(*end);
	    if(d_groundcall(prune)) {
		hyp1=(ITEM)NULL;
		cl_pop(end);
		i_deletes(c1,call,(ITEM)I_TERM);
		continue;
	    }
	    cq=cl_cq(negq,hyp1);     /* Integrity constraints */
	    hyp1=(ITEM)NULL;
	    if(cq) {
	      if(((p=cl_pcoverage(call,L_GET(*end)))-(clen+h))> *bestpass) {
	        if(r_posonly()) {
		  if(cl_ncoverage(negq,call,L_GET(*end)))
		    n=RINF;
		  else n=cl_dcoverage(call,fnex);
		  /* NOTE: Needs to be changed to include abductive learning */
	        }
	        else n=cl_ncoverage(negq,call,L_GET(*end));
	      }
	      else {
	        if(SETQ(memoing)) *entry=r_mkstate(p,RINF,clen,HV,m,s,fnex);
	        cl_pop(end); i_deletes(c1,call,(ITEM)I_TERM);
	        continue;
	      }
	    }
	    else {p=P_VAL(s); n=RINF;}
	    cl_pop(end); i_deletes(c1,call,(ITEM)I_TERM);
	    s1=r_mkstate(p,n,clen,HV,m,s,fnex);
	    if(SETQ(memoing)) *entry=i_inc(s1);
	  }
	  else {
	    if(((p=P_VAL(*entry))-(clen+h))<= *bestpass) continue;
	    s1=r_mkstate(p,N_VAL(*entry),clen,HV,m,s,fnex);
	  }
	  l_suf(i_dec(s1),children);
	  (*explored)++;
	  /* if(R_PASS(s1)&&(F_VAL(s1)>*bestpass)) *bestpass=F_VAL0(s1); */
	  if(verbose>=2l) {
	    printf("[C:%.0f,%.0f,%.0f,%.0f %s]\n",
		F_VAL(s1),P_VAL(s1),N_VAL(s1),H_VAL(s1),mess);
	  }
	  if(!(*best)||r_better(s1,*best)) {
		i_delete(*best);
		*best=i_inc(s1);
		*bestchanged=TRUE;
	  }
	}
	if(!headq) {
	  l_suf(i_dec(s1=i_copy(s)),children);
	  A_CODE(s1)=SKIPA; R_PARENT(s1)=(LONG)s;
	}
	i_deletes(sname,c,vmap,nin,nout,bdvin,bdvout,bdvout1,(ITEM)I_TERM);
	return(i_sort(children));
}


#define L24	(0xffffff)
#define VNO(t)	(t->extra&L24)

/* r_precombo - calculate number of combinations of a particular literal.
 */

#define	OUT1(x)	((headq&& !IS(OUT,x))||(!headq&&IS(OUT,x)))

LONG r_osplit();

LONG
r_precombo(a,vmap,nin,nout,headq)
	ITEM a,vmap,*nin,*nout;
	PREDICATE headq;
	{
	ITEM term,*vset,vseen;
	LONG result,n,s,vno;
	*nin=Y_EMPTY; *nout=Y_EMPTY; vseen=B_EMPTY;
	TERM_DO(term,a) /* Initialise numbers of I/O var occurences */
	  if(term->item_type=='v') {
	    (*y_ins(vno=VNO(term),OUT1(term->extra)?*nout:*nin))++;
	    b_add(vno,vseen);
	  }
	TERM_END
	result=1l;
	BIT_DO(vno,vseen)	/* Calculate combinations */
	  s=(*(vset=f_ins(vno,vmap))?b_size(*vset):0l);
	  if(n= *y_ins(vno,*nout)) result*=r_osplit(s,n);
	  if(n= *y_ins(vno,*nin)) for(;n--;) result*=s;
	BIT_END
	i_delete(vseen);
	return((result&& !SETQ(splitting))?1:result);
}

/* r_minxerr - minimises the expected error of hypotheses
 *	by comparing against each other. Cost of hypothesis h_l
 *	evaluated as follows.
 *		cost(h_l) = Sum_{h in H} p(h|D)cost(h_l,h)
 *		cost(h_l,h) = Sum_{x in X} p(x)cost(h_l,h,x)
 *			{ c_o if x not in h_l and x in h
 *	cost(h_,h,x) =	{ c_c if x in h_l and x not in h
 *			{ 0 otherwise
 */

/*

ITEM
r_minxerr(closed,head,atoio,amap,aindex,vmax,psym)
	ITEM closed,head,atoio,amap;
	LONG aindex,vmax,psym;
	{
	ITEM comp=L_EMPTY,s,bestclosed=(ITEM)NULL,c,call,fcall,r1,r2,
		sample,*atom1,*atom2,call2,atom,bdvars;
	LONG cnte=errorchk,aindex1,vmax1,acode;
	double min,omin,max,omax,f,*f1,*f2,*e1,*e2,cnt,tot,sump,p1;
	double df;
	omin=min=RINF;
	omax=max=0.0;
	LIST_DO(s,closed)
	  acode=A_CODE(s);
	  if(R_PASS(s)&&acode!=r_intof(SKIPA)) {
	    f=F_VAL(s); MIN(omin,f); MAX(omax,f);
	    if(omax-omin>10.0) break;
	    min=omin; max=omax; aindex1=aindex; vmax1=vmax;
	    c=r_clauseof(s,head,atoio,amap,&aindex1,(ITEM)NULL,&vmax1,
		(ITEM)NULL);
	    call=d_gcpush(i_dec(c));
	    fcall=i_tup4(i_dec(i_create('r',(POINTER)r_create(0.0))),
		i_dec(i_create('r',(POINTER)r_create(f))),
		i_dec(call),s);
	    l_suf(i_dec(fcall),comp);
	    if(cnte-- <=0l) break;
	  }
	LIST_END
	LIST_DO(r1,comp)
	  f1=I_FP(F_ELEM(1l,r1));
	  sump+=(p1=pow(2.0,df= *f1-min));
	  *f1=p1;
	LIST_END
	if(!(sample= *(f_ins(psym,psamps))))
	  d_error("r_minxerr - no sample of given predicate");
	tot=l_length(sample);
	LIST_DO(r1,comp)
	  e1=I_FP(F_ELEM(0l,r1)); call=F_ELEM(2l,r1); atom1=ATMP(call);
	  LIST_DO(r2,comp)
	    if(r1==r2) continue;
	    f2=I_FP(F_ELEM(1l,r2)); call2=F_ELEM(2l,r2); atom2=ATMP(call2);
	    cnt=0.0;
	    LIST_DO(atom,sample)
	      *atom1= *atom2=atom;
	      if(interp_quest(call)!=interp_quest(call2)) cnt+=1.0;
	    LIST_END
	    *atom1= *atom2=(ITEM)NULL;
	    *e1+=(*f2/sump)*(cnt/tot);
	  LIST_END
	LIST_END
	if(l_length(comp)) {
	  i_sort(comp);
	  bestclosed=i_inc(F_ELEM(3l,HOF((LIST)I_GET(comp))));
	}
	i_delete(comp);
	return(bestclosed);
}
*/

/* r_mark - mark the variable number of each variable in lower 24 bits
 *	of extra field.
 */

r_mark(clause)
	ITEM clause;
	{
	ITEM atom,term;
	LIST_DO(atom,clause)
	  TERM_DO(term,atom)
	    if(term->item_type=='v')
	      term->extra=((term->extra& ~L24)|((LONG)I_GET(term)&L24));
	  TERM_END
	LIST_END
}

/* r_bind - decode binding and apply to given atom.
 */

r_bind(p,atom,vmap,vmax,nin,nout,headq,bdvin,bdvout)
	LONG p,*vmax;
	ITEM atom,vmap,nin,nout,bdvin,bdvout;
	PREDICATE headq;
	{
	register LONG i,nv,vno,s,n,t,u;
	ULONG *p1;
	ITEM term,*vset,vset1,vsofar,pin,pout;
	vsofar=b_allones(*vmax); pin=Y_EMPTY; pout=Y_EMPTY;
	for(i= *vmax;i--;) { /* Decompose codes for individual variables */
	  s=(*(vset=f_ins(i,vmap))?b_size(*vset):0l);
	  if(n= *y_ins(i,nout)) {*y_ins(i,pout)=p%(t=r_osplit(s,n)); p/=t;}
	  if(n= *y_ins(i,nin)) {for(t=1l;n--;) t*=s; *y_ins(i,pin)=p%t; p/=t;}
	}
	TERM_DO(term,atom)	/* Decode binding */
	  if(term->item_type=='v') {
	    if(!(*(vset=f_ins(vno=VNO(term),vmap)))) *vset=B_EMPTY;
	    if(OUT1(term->extra)) {
	      p1=y_ins(vno,pout); s=b_size(*vset);
	      n= --(*y_ins(vno,nout));
	      if(*p1<(u=(s*(t=r_osplit(s,n)))))
		{nv= *p1/t; *p1%=t;}
	      else {b_add((*vmax)++,*vset); nv=s; *p1-=u;}
		/* Split variable */
	    }
	    else {
	      p1=y_ins(vno,pin);
	      s=b_size(b_int(vset1=i_copy(*vset),vsofar)); i_delete(vset1);
	      nv= *p1%s; *p1/=s;
	    }
	    I_GET(term)=(POINTER)(vno=b_ith(nv+1l,*vset));
	    if(bdvin&&(!OUT1(term->extra))) b_add(vno,bdvin);
	    else if(bdvout&&OUT1(term->extra)) b_add(vno,bdvout);
	  }
	TERM_END
	i_deletes(vsofar,pin,pout,(ITEM)I_TERM);
}
void r_cl1(s,head,clause,atoio,amap,aindex,vmap,vmax,bdvout)
	ITEM s,head,clause,atoio,amap,vmap,bdvout;
	LONG *vmax,*aindex;
	{
	ITEM atom;
	LONG ano,rp,acode;
	if(!(rp=R_IPARENT(s))) return;
	r_cl1((ITEM)rp,head,clause,atoio,amap,aindex,vmap,vmax,bdvout);
	acode=A_CODE(s);
	if(R_PARENT((ITEM)rp)==0.0) {	/* Clause head */
	  atom=i_copy(head);
	  DOBIND(TRUE);
	}
	else {				/* Clause body */
	  if(*aindex>=Y_SIZE(amap)) d_error("r_cl1 - bad atom set");
	  else ano=Y_ELEM(*aindex,amap);
	  if(acode==r_intof(SKIPA)) (*aindex)++;
	  else {
	    atom=i_copy(F_ELEM(0l,F_ELEM(ano,atoio)));
	    DOBIND(FALSE);
	    aindex++;	/* This is incomplete */
	  }
	}
}
/* r_clauseof - constructs clause related to a given search state
 *	record by backward chaining through the state's parent.
 */

ITEM
r_clauseof(s,head,atoio,amap,aindex,vmap,vmax,bdvout)
	ITEM s,head,atoio,amap,*vmap,bdvout;
	LONG *vmax,*aindex;
	{
	ITEM clause=L_EMPTY,term,*entry,vmap1;
	LONG vno;
	vmap1=F_EMPTY;
	TERM_DO(term,head)
	  if(term->item_type=='v' && IS(OUT,term->extra)) {
	    if(!(*(entry=f_ins(vno=(LONG)I_GET(term),vmap1))))
	      *entry=B_EMPTY;
	    b_add(vno,*entry);
	  }
	TERM_END
	r_cl1(s,head,clause,atoio,amap,aindex,vmap1,vmax,bdvout);
	if(vmap) *vmap=vmap1;
	else i_delete(vmap1);
	return(clause);
}




PREDICATE
r_better(s1,s2)
	ITEM s1,s2;
	{
	if(R_PASS(s1)) {
	  if(R_PASS(s2))
	    return(F_VAL(s1)>F_VAL(s2));
	  else return(TRUE);
	}
	else {
	  if(R_PASS(s2))
	    return(FALSE);
	  else return(F_VAL(s1)>F_VAL(s2));
	}
}

/* r_atgoal - decides whether to terminate search. Also prunes
 *	the open list of states with a g value less than or equal
 *	to the best state when the best state has h=0.
 */

PREDICATE
r_atgoal(s,open,bestchanged)
	ITEM s,open;
	PREDICATE bestchanged;
	{
	/* double left; */
	PREDICATE best=TRUE;
	ITEM s1;
	LIST *elemp1;
	if(!R_PASS(s)) return(FALSE);
	/* if(P_VAL(s)==(left=b_size(bfores))) return(TRUE); */
	if(!bestchanged) return(FALSE);
	elemp1=L_LAST(open);
	while (*elemp1) {
	  s1=L_GET(*elemp1);
	  if(full_pruning) { /* full_pruning -
				prune any trees which
				could only lead to clauses with compression
				 AS GOOD AS the best so far */
	    if(F_VAL(s)>=R_CMP(s1)) L_REM(elemp1)
	    else {
	      best=FALSE;
	      elemp1 = &((*elemp1)->next);
	    }
	  }
	  else {	/* NOT full_pruning - prune any trees which
				could only lead to clauses with compression
				WORSE THAN the best so far */
	    if(F_VAL(s)>=R_CMP(s1)) L_REM(elemp1)
	    else {
	      best=FALSE;
	      elemp1 = &((*elemp1)->next);
	    }
	  }
	}
	return(best);
}

/* r_outlook - constructs table giving minimum length of chain of
 *	atoms to construct head output variables from any atom in the
 *	body. When there are no head output variables outlook is
 *	zero for all atoms. When there are output variables the
 *	outlook of any atom which does not chain to a head output
 *	is made 1 greater than the maximum outlook of any atom that does
 *	chain.
 */

ITEM
r_outlook(clause,head1,otoa,atoio)
	ITEM clause,head1,otoa,atoio;
	{
	ITEM outlook=Y_EMPTY,vout=B_EMPTY,vseen=B_EMPTY,vnew,
		atoms=B_EMPTY,aseen=B_EMPTY,anew,arec,atom,term,head,set;
	LONG ano,vno,depth=0l;
	head=l_pop(clause);
	TERM_DO(term,head1)	/* Collect head variables */
	  if(term->item_type=='v' && IS(OUT,term->extra))
	    b_add((LONG)I_GET(term),vout);
	TERM_END
	LIST_DO(atom,clause)	/* Initialise atoms and outlook */
	  b_add(ano=atom->extra,atoms);
	  *y_ins(ano,outlook)=0;
	LIST_END
	if(!b_emptyq(vout)) {
	  while(!b_emptyq(vout)) { /* Backpropogate depths in outlook */
	    anew=B_EMPTY; vnew=B_EMPTY; b_uni(vseen,vout);
	    BIT_DO(vno,vout)
	      if(set= *f_ins(vno,otoa)) b_uni(anew,set);
	    BIT_END
	    b_sub(b_int(anew,atoms),aseen); b_uni(aseen,anew);
	    BIT_DO(ano,anew)
	      if(b_subseteq(F_ELEM(2l,arec=F_ELEM(ano,atoio)),vseen)) {
	        Y_ELEM(ano,outlook)=(unsigned long int)depth;
	        b_uni(vnew,F_ELEM(1l,arec));
	      }
	      else b_rem(ano,aseen);
	    BIT_END
	    b_sub(vnew,vseen); b_uni(vseen,vnew);
	    i_swap(vout,vnew);
	    i_deletes(anew,vnew,(ITEM)I_TERM);
	    depth++;
	  }
	  b_sub(atoms,aseen);
	  if(depth) depth--;
	  BIT_DO(ano,atoms)	/* Assign max_depth+1 to unassigned atoms */
	      Y_ELEM(ano,outlook)=(unsigned long int)depth;
	  BIT_END
	}
	l_push(i_dec(head),clause);
	i_deletes(vout,atoms,aseen,vseen,(ITEM)I_TERM);
	return(outlook);
}

/* r_osplit - suppose v appears n=g(v,l') times as an output variable in l'
	in bot and s=f(v,theta). The number of ways these n variable
	occurences can be partitioned is

	O(s,0) = 1
	O(s,n) = s.O(s,n-1) + O(s+1,n-1)
		   No-split-v    Split-v

	A tabulation of the function is lazily evaluated using the table osplit.
 */

LONG
r_osplit(s,n)
	LONG s,n;
	{
	ITEM *entry;
	LONG *result;
	if(!n) return(1l);
	if(!(*(entry=f_ins(s,osplit)))) *entry=Y_EMPTY; /* Look-up */
	if(!(*(result=(LONG *)y_ins(n,*entry))))  	/* Calculate */
		*result=s*r_osplit(s,n-1l)+r_osplit(s+1l,n-1l);
	return(*result);
}

/* r_vdomains - produces the closure of input to output variables.
 *	The domain of output variable o is taken to
 *	be the set of all variables from which o can be derived
 *	by I/O relations within literals in the bottom clause.
 */

ITEM
r_vdomains(otoa,atoio)
	ITEM otoa,atoio;
	{
	LONG vno=0l,ano,vno1;
	ITEM *atoms,atoms1,result=F_EMPTY,allv,newv,alla,newa;
	FUNC_DO(atoms,otoa)
	  if(*atoms) {
	    newv=b_copy(allv= *f_ins(vno,result)=B_EMPTY);
	    newa=b_copy(alla=b_copy(*atoms));
	    while(!b_emptyq(newa)) {
	      BIT_DO(ano,newa)
		b_uni(newv,F_ELEM(1l,F_ELEM(ano,atoio)));
	      BIT_END
	      b_sub(newv,allv); b_uni(allv,newv);
	      BIT_DO(vno1,newv)
		if(atoms1= *f_ins(vno1,otoa))
		  b_uni(newa,atoms1);
	      BIT_END
	      b_sub(newa,alla); b_uni(alla,newa);
	    }
	    i_deletes(newv,newa,alla,(ITEM)I_TERM);
	  }
	  vno++;
	FUNC_END
	return(result);
}

/* r_minh - calculates lower bound on the number of atoms required
 *	to complete the I/O relations in a clause. This is done by
 *	mini-maxing over the and-or graph of I/O in individual
 *	atoms in the bottom clause.
 */

LONG
r_minh(vins,vouts,clause,atoio,otoa,vdomains)
	ITEM vins,vouts,clause,atoio,otoa,vdomains;
	{
	LONG result;
	register ITEM head,atom,vvis,vins1,vknown,mins;
	if(b_emptyq(vouts)) return(0l);
	vins1=b_copy(vins);
	if(clause) {
	  if(L_EMPTYQ(clause)) d_error("r_minh - handed empty clause");
	  else head=l_pop(clause);
	  LIST_DO(atom,clause)
	    if(atom->extra!=UNASS)
	      b_uni(vins1,F_ELEM(2l,F_ELEM(atom->extra,atoio)));
	  LIST_END
	  l_push(i_dec(head),clause);
	}
	result=r_maxminh(vouts,vins1,atoio,otoa,vdomains,mins=Y_EMPTY,
		vvis=B_EMPTY,vknown=B_EMPTY);
	i_deletes(mins,vins1,vvis,vknown,(ITEM)I_TERM);
	return(result);
}

LONG
r_maxminh(vouts,vins,atoio,otoa,vdomains,mins,vvis,vknown)
	ITEM vouts,vins,atoio,otoa,vdomains,mins,vvis,vknown;
	{
	LONG sum=0l,max=0l,*min,o,a,maxmin,n=0l;
	ITEM doms=B_EMPTY,vouts1=b_sub(b_sub(b_copy(vouts),vins),vvis),as,
		domso,domso1;
	BIT_DO(o,vouts1)	/* Maximise over AND */
	  b_add(o,vvis);
	  min=(LONG *)y_ins(o,mins); /* Min already computed? */
	  if(!b_memq(o,vknown)) {
	    *(min)=INF;
	    if(as= *f_ins(o,otoa)) {
	      BIT_DO(a,as)	/* Minimise over OR */
		maxmin=r_maxminh(F_ELEM(1l,F_ELEM(a,atoio)),
			vins,atoio,otoa,vdomains,mins,vvis,vknown);
		min=(LONG *)y_ins(o,mins);
		MIN(*min,maxmin);
	      BIT_END
	    }
	    (*min)++;		/* Min of proofs = min subproof + 1 */
	    b_add(o,vknown);
	  }
	  sum+=(LONG)*min;	/* Sum is an overestimate */
	  if(domso= *f_ins(o,vdomains)) {
	    if(!b_memq(o,doms)) n++;
	    sum-=b_size(b_sub(b_sub(b_int(domso1=b_copy(domso),doms),vins),
			vvis));
	    b_uni(doms,domso); /* Subtracting domain int is underestimate */
	    i_delete(domso1);
	  }
	  else n++;
	  MAX(max,(LONG)*min);	/* Max proof in conjunction of proofs */
	  b_rem(o,vvis);
	BIT_END
	max+=n-1;		/* At least 1 literal per subproof */
	MAX(max,sum);		/* Max of 2 underestimates */
	i_deletes(doms,vouts1,(ITEM)I_TERM);
	return(max);
}

PREDICATE
r_posonly()
	{
	return(posonly);
}
