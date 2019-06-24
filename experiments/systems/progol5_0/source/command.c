#include <stdio.h>
#include "progol.h"

/*
 * #######################################################################
 *
 *			Command Routines
 *                      ----------------
 *
 * #######################################################################
 */
extern char ttychline();
extern PREDICATE interp();
PREDICATE c_genc();


/* interp_quest - interprets a question. Succeeds at most once.
 */


PREDICATE
interp_quest(cclause)
	ITEM cclause;
	{
	PREDICATE result;
	if(result=interp(cclause,TRUE,FALSE))
	  interp((ITEM)NULL,TRUE,FALSE);	/* Reset interpreter */
	return(result);
}

extern ITEM ct_sat(),cl_vrenum(),r_outlook(),r_vdomains(),r_posonly();

/* c_updsamp - conditions stochastic labels on positive examples and
 *	updates sample ground atoms of psym for learning
 *	from positive examples.
 */

c_updsamp(psym)
	LONG psym;
	{
	ITEM *entry,*entry1,atom;
	LONG psym1,cost=0l,cno,cnt=0l;
	if(!(*(entry=f_ins(psym,psamps)))) {
	  if(!(psym1=(LONG)*y_ins(psym,ptog)))
	    d_error("c_updsamp - predicate has no generator predicate");
	  if(condition) c_condition(psym,psym1);
	  if(!(*(entry1=f_ins(psym,bptab))))
	    *entry1=B_EMPTY;
	  BIT_DO(cno,*entry1)
	    cost+= *y_ins(cno,costs);
	  BIT_END
	  *entry=d_sample(psym1,4l*cost);
	  LIST_DO(atom,*entry)
	    if(atom->item_type=='f') I_GET(F_ELEM(0l,atom))=(POINTER)psym;
	    else I_GET(atom)=(POINTER)psym;
	    atom->extra=cnt++;
	  LIST_END
	}
	if(!cover) c_updbsamp(psym,(ITEM)NULL);
	else if(!(*(entry=f_ins(psym,bpsamps)))) *entry=B_EMPTY;
	/* c_updbsamp(psym); */
}

/* c_updbsamp - compute subsample which is not yet provable from
 *	theory so far. This makes calculation of clause generality more
 *	efficient. Cover testing is done if call arg is NULL.
 */

c_updbsamp(psym,call)
	LONG psym;
	ITEM call;
	{
	ITEM *entry,*entry1,atom;
	ITEM c1,call1,*atom1;
	if(call) {
	  call1=d_gcpush(c1=i_copy(F_ELEM(0l,call)));
	  atom1=ATMP(call1);
	}
	entry=f_ins(psym,psamps);
	if(!(*(entry1=f_ins(psym,bpsamps)))) *entry1=B_EMPTY;
	LIST_DO(atom,*entry)
	  if(!b_memq(atom->extra,*entry1)) {
	    if(call) {
	      *atom1=atom;
	      if(interp_quest(call1)) b_add(atom->extra,*entry1);
	    }
	    else if(d_groundcall(atom)) b_add(atom->extra,*entry1);
	  }
	LIST_END
	if(call) {
	  *atom1=(ITEM)NULL;
	  i_deletes(c1,call1,(ITEM)I_TERM);
	}
}

/* c_condition - uses examples to condition stochastic logic program
 *	associated with a particular predicate.
 */

c_condition(psym,psym1)
	LONG psym,psym1;
	{
	ITEM ccl,cl,*entry,head,pred;
	if(*(entry=f_ins(psym,ptab))) {
	  if(!(psym1=(LONG)*y_ins(psym,ptog))) {
	    d_error("c_condition - predicate has no generator predicate\n");
	  }
	  conditioning=TRUE;
	  LIST_DO(ccl,*entry)
	    if(l_length(cl=F_ELEM(0l,ccl))==1l) {
	      if((head=HOF((LIST)I_GET(cl)))->item_type== 'h') pred=head;
	      else pred=F_ELEM(0l,head);
	      I_GET(pred)=(POINTER)psym1;
	      l_push((ITEM)NULL,cl);
	      interp_quest(ccl);
	      l_pop(cl);
	      I_GET(pred)=(POINTER)psym;
	    }
	  LIST_END
	  conditioning=FALSE;
	}
}

PREDICATE
c_doall(froot)
	ITEM froot;
	{
	STRING name;
	char mess[MAXMESS];
	LONG psym,cno;
	ITEM user,*entry,bcl0,bcl1,changed,ccl;
	double start,start0,total;
	PREDICATE succeeded;
	SET(searching);
	if(froot->item_type== 'h')
		name=QP_ntos((LONG)I_GET(froot));
	else return(FALSE);
	if(STRSUFQ(name,".pl")) sprintf(mess,"%s",name);
	else sprintf(mess,"%s.pl",name);
	reconsult=FALSE; mseen=FALSE;
	if(cl_readrls(mess)) {
		/* Generalise each predicate in hmodes */
	  if(b_emptyq(observables)) {
	    g_message(1l,"No observables found");
	  }
	  else {
	    user=b_sub(b_sub(b_copy(allpsyms),lib1),aux);
	    b_int(user,observables);
	    bcl0=b_copy(bclauses);
	    start0=cputime();
	    BIT_DO(psym,user)
	      if(*(entry=f_ins(psym,bptab))&& !b_emptyq(b_int(*entry,bclauses))) {
	        start=cputime();
	        if(c_gen1(psym)) {
	          sprintf(mess,"Time taken %.3lfs",fabs(cputime()-start));
	            g_message(1l,mess);
		}
	      }
	    BIT_END
	    total=cputime()-start0;
	    b_sub(bcl1=b_copy(bclauses),bcl0);
	    b_uni(bcl1,b_sub(bcl0,bclauses));
	    changed=B_EMPTY;
	    BIT_DO(cno,bcl1)
	      if(ccl=b_elem(cno,spcls)) {
	        b_add(PSYM(HOF((LIST)I_GET(F_ELEM(0l,ccl)))),changed);
	        i_dec(ccl);
	      }
	    BIT_END
	    i_delete(user);
	    user=b_sub(b_sub(b_copy(allpsyms),lib1),aux);
	    b_int(changed,user);
	    if(!b_emptyq(changed)) {
	      printf("\n"); printf("\n");
	      g_message(2l,"The following definitions changed");
	      printf("\n");
	      BIT_DO(psym,changed)
	        c_list1(psym,FALSE);
	      BIT_END
	    }
	    i_deletes(user,bcl0,bcl1,changed,(ITEM)I_TERM);
	    sprintf(mess,"Total time taken %.3fs",total);
	    g_message(1l,mess);
	  }
	}
	return(TRUE);
}

/* c_genc - generalise a clause C, by adding a clause to a predicate
	which is a descendent of the predicate in the head of C.
 */

ITEM c_mdesc(),c_assneg(),c_gencps();

PREDICATE
c_genc(cclause,ps)
	ITEM cclause,ps;
	{
	LONG psym,cno,psymc;
	ITEM besth;
	PREDICATE result=FALSE;
	/* if(!GRNDQ(cclause)) return(FALSE); */
	BIT_DO(psym,ps)	/* Stickelise all ancestors of descendents */
          i_delete(c_stickelise(psym));
	BIT_END
	besth=c_gencps(cclause,ps); /* Generalise by adding clause to ps */
	if(besth) {
	  result=TRUE;
	  c_assred(besth,psymc); /* Assert clause and rem redundancy */
	}
	i_deletes(besth,(ITEM)I_TERM);
	return(result);
}

/* c_gencps - generalise the clause described in catom1 by
 *	adding a clause to one of the background predicates
 *	described in ps.
 */

ITEM
c_gencps(cclause,ps)
	ITEM cclause,ps;
	{
	ITEM besth=(ITEM)NULL,max=(ITEM)NULL;
	LONG psym;
	BIT_DO(psym,ps) /* Find most compressive clause for any of ps */
	  c_gencp(cclause,psym,&besth,&max);
	BIT_END
	if(besth && (L_EMPTYQ(besth) || F_VAL(max)<=0.0)) {
	  i_delete(besth);
	  besth=(ITEM)NULL;
	}
	i_delete(max);
	return(besth);
}

/* c_assred - assert clause and remove redundancy
 */

c_assred(cclause,psymc)
	ITEM cclause;
	LONG psymc;
	{
	LONG varno,psym,psym1;
	ITEM clause=i_inc(F_ELEM(0l,cclause)),cclause1,ancestors;
	psym=PSYM(HOF((LIST)I_GET(clause)));
	ancestors=ANC(psym);
	cl_unflatten(&clause);
	if(verbose>=2) {
	  g_message(2l,"Result of search is");
	  printf("\n");
	  cl_print(clause);
	  printf("\n");
	}
	cl_vrenum(clause,&varno);
	cclause1=i_tup4(i_dec(clause),i_dec(I_INT(varno)),
		  idot0,i_dec(F_EMPTY));
	if(posonly&&cover)
		c_updbsamp(psymc,cclause1);
	cl_assert(cclause1,TRUE,TRUE,TRUE,FALSE,TRUE,(ITEM)NULL);
	BIT_DO(psym1,ancestors)
	  d_treduce(psym1);
	BIT_END
	i_deletes(ancestors,cclause1,(ITEM)I_TERM);
}

/* c_assneg - asserts the negation of the given clause. Extra
 *	information for virtual assertion and retraction of new clauses
 *	stored in catoms1, cle1, clt1. 
 */

ITEM
c_assneg(cclause)
	ITEM cclause;
	{
	ITEM clause=F_ELEM(0l,cclause),retract=Y_EMPTY,pseen=B_EMPTY,
		nums=L_EMPTY,*entry;
	LONG nvs=(LONG)I_GET(F_ELEM(1l,cclause)),cno=cclause->extra;
	if(nvs) cl_skolem(clause);
	c_snegate(HOF((LIST)I_GET(clause)));
	cl_assatoms(clause,retract,pseen,cno,nums);
		/* Assert Skolemised body */
	cl_leprop(cno,i_sort(nums));
	cl_psfirstarg(pseen);
	catoms->extra=cno;
	if(*(entry=f_ins(cno,catoms))) b_uni(bclauses,*entry);
	catoms1=catoms; clt1=clt; cle1=cle;
	catoms=clt=cle=(ITEM)NULL;
	i_deletes(pseen,nums,(ITEM)I_TERM);
	return(retract);
}

/* c_retneg - retracts the atoms asserted by c_assneg and restores
 *	global data structures.
 */

c_retneg(retract)
	ITEM retract;
	{
	catoms=catoms1; clt=clt1; cle=cle1;
	catoms1=clt1=cle1=(ITEM)NULL;
	cl_retatoms(retract);	/* Retract Skolemised body */
}

/* c_mdesc - returns the descendents of predicate psym which have
 *	modeh declarations.
 */

ITEM
c_mdesc(clause)
	ITEM clause;
	{
	ITEM des=B_EMPTY,des1,atom,head=l_pop(clause);
	LIST_DO(atom,clause)
	  des1=c_mdesp(PSYM(atom));
	  b_uni(des,des1);
	  i_delete(des1);
	LIST_END
	l_push(i_dec(head),clause);
	return(des);
}

/* c_mdesp - returns the descendents of predicate psym which have
 *	modeh declarations.
 */

ITEM
c_mdesp(psym)
	LONG psym;
	{
	ITEM des=DES(psym),result=b_copy(des),*entry;
	LONG psym1;
	BIT_DO(psym1,des)
	  if(!(*(entry=f_ins(psym1,hmodes)))||L_EMPTYQ(*entry))
	    b_rem(psym1,result);
	BIT_END
	i_delete(des);
	return(result);
}

/* c_gencp - generalise clause in catoms1 by adding a clause to predicate
 *	psym. This is done by generating a start set of atoms using all the
 *	modeh atoms of predicate psym, and then maximising over the
 *	resultant clauses.
 */

#define		RECALLN		20l

ITEM c_sat();

c_gencp(ccl,psym,besth,max)
	LONG psym;
	ITEM ccl,*besth,*max;
	{
	char mess[MAXMESS];
	ITEM *modes,atoms=L_EMPTY,mode,call,atom,atom1,clause,cclause,
		retract,cclause1;
	LONG n,varno;
	PREDICATE succeeded;
	if(!(*(modes=f_ins(psym,hmodes)))) {
	  sprintf(mess,"c_gencp - predicate %s/%d has no modeh declaration",
		QP_ntos(psym),QP_ntoa(psym));
	  d_error(mess);
	}
	retract=c_assneg(cclause1=i_copy(ccl)); /* Assert ~C */
	LIST_DO(mode,*modes)	/* Generate start-set (abducibles) */
	  call=F_ELEM(0l,mode);
	  atom=HOF(TOF((LIST)I_GET(F_ELEM(0l,call))));
	  c_snegate(HOF(TOF((LIST)I_GET(F_ELEM(0l,call)))));
	  n=(LONG)I_GET(F_ELEM(2l,mode));
	  if(n<RECALLN) n=RECALLN;
	  succeeded=interp(call,TRUE,TRUE);		/* Call */
	  while(succeeded && n-- >0l) {
	    atom1=ct_skolem(p_copy(atom,(BIND)term_stack0,FALSE));
	    c_snegate(atom1);
	    l_suf(i_dec(atom1),atoms);
	    succeeded=interp((ITEM)I_TERM,TRUE,TRUE);	/* Retry */
	  }
	  if(succeeded) interp((ITEM)NULL,TRUE,TRUE);	/* Reset */
	  c_snegate(HOF(TOF((LIST)I_GET(F_ELEM(0l,call)))));
	LIST_END
	c_retneg(retract); /* Retract ~C */
	i_deletes(retract,cclause1,(ITEM)I_TERM);
	i_sort(atoms);		/* Remove duplicates */
	LIST_DO(atom,atoms)	/* Generalise start set */
	  cclause=i_copy(ccl);
	  clause=F_ELEM(0l,cclause);
	  i_delete(l_pop(clause));	/* Replace clause head */
	  l_push(atom,clause);
	  cl_swrite(mess,clause);
	  g_message(2l,"Generalising <%s>",mess);
	  cclause1=c_sat(cclause,bcl_costs(psym),max);
	  if(cclause1) {
	    i_delete(*besth);
	    *besth=i_inc(cclause1);
	  }
	  i_deletes(cclause,cclause1,(ITEM)I_TERM);
	LIST_END
	i_delete(atoms);
}

ITEM
c_sat(cclause,nex,best)
	ITEM cclause,*best;
	LONG nex;
	{
	ITEM atoio=F_EMPTY,otoa=F_EMPTY,head=(ITEM)NULL,hypothesis,*entry,
		retract=Y_EMPTY,clause=F_ELEM(0l,cclause),outlook,cclause1,
		vdomains,ancestors,result=(ITEM)NULL;
	LONG psym,psym1,varno;
	double oldnoise,fnex=nex;
	if(hypothesis=ct_sat(cclause,atoio,otoa,&head)) {
	  cl_symreduce(&hypothesis,atoio,head);
	  outlook=r_outlook(hypothesis,head,otoa,atoio);
	  vdomains=r_vdomains(otoa,atoio);
	  if(verbose>=2) {
	    g_message(2l,"Most specific clause is");
	    printf("\n");
	    cl_print(hypothesis);
	    printf("\n");
	  }
	  if(SETQ(searching)) {
	    ancestors=ANC(psym=PSYM(HOF((LIST)I_GET(clause))));
	    if(b_size(ancestors)>1l) cover=FALSE;
	    BIT_DO(psym1,ancestors) /* Prepare examples for testing */
	      d_pushfores(psym1,retract);
	    BIT_END
	    if(posonly) {
	      g_message(2l,"Learning from positive examples of %s/%d",
		QP_ntos(psym),QP_ntoa(psym));
	      oldnoise=noiselim;
	      noiselim=100.0;
	      c_updsamp(psym);
	    }
	    fnex=fnex*inflate/100.0;
	    r_search(&hypothesis,atoio,otoa,outlook,vdomains,fnex,best);
	    if(posonly) noiselim=oldnoise;
	    if(hypothesis&& !L_EMPTYQ(hypothesis)) {
	      cl_unflatten(&hypothesis);
	      if(verbose>=2) {
	        g_message(2l,"Result of search is");
		printf("\n");
	        cl_print(hypothesis);
		printf("\n");
	      }
	    }
	    else {
	      printf("[No compression]\n\n");
	    }
	    d_popfores(retract);
	    if(hypothesis&& !L_EMPTYQ(hypothesis)) {
	      cl_vrenum(hypothesis,&varno);
	      result=i_tup4(hypothesis,i_dec(I_INT(varno)),
			  idot0,i_dec(F_EMPTY));
	    }
	    else result=(ITEM)NULL;
	    i_delete(ancestors);
	  }
	  i_deletes(hypothesis,outlook,vdomains,(ITEM)I_TERM);
	}
	i_deletes(atoio,otoa,head,retract,(ITEM)I_TERM);
	return(result);
}


void c_interp(cclause)
	ITEM cclause;
	{
	ITEM vtable;
	ITEM *fptr,*entry,cclause1,best,ps;
	FUNC f;
	PREDICATE succeeded,first;
	LONG type,psym;
	if(cclause==(ITEM)I_ERROR)
	  return;
	if((type=(LONG)CTYPE(cclause))==pdot0) {	/* Assertion */
	  if(cl_assert(cclause,FALSE,TRUE,TRUE,FALSE,TRUE,(ITEM)NULL)) {
	    printf("[<"); ccl_fwrite(ttyout,cclause,NULL);
	    printf("> added to clauses]\n");
	    CONTRA
	  }
	  return;
	}
	else if((type)==pcut) {			/* Example */
	  CTYPE(cclause)=(POINTER)pdot0;
	  cl_assert(cclause,FALSE,TRUE,TRUE,FALSE,TRUE,(ITEM)NULL);
	  CONTRA
	  ps=c_mdesp(psym=PSYM(HOF((LIST)I_GET(F_ELEM(0l,cclause)))));
		/* Descendent predicates with modeh declarations */
	  if(!b_emptyq(ps)) c_genc(cclause,ps);
	  else g_message(2l,"No moded descendents of %s/%d",QP_ntos(psym),
		QP_ntoa(psym));
	  i_delete(ps);
	  return;
	}
	vtable=F_ELEM(3l,cclause);		/* Query */
	f=(FUNC)I_GET(HOF((LIST)I_GET(F_ELEM(0l,cclause))));
	succeeded=interp(cclause,TRUE,TRUE);
	while (succeeded) {
		if(!(f->arr_size-1l)) {
		  interp((ITEM)NULL,TRUE,TRUE);
		  break;
		}
		first=TRUE;
		ARG_LOOP(fptr,f) {
		  if(!(*(entry=f_ins((LONG)I_GET(*fptr),vtable))))
		    d_error("c_interp - bad variable table");
		  if(STREQ((STRING)I_GET(*entry),"_")) continue;
		  if(!first) printf("\n");
		  else first=FALSE;
		  printf("%s = ",(STRING)I_GET(*entry));
		  p_fwritesub(ttyout,*fptr,(BIND)term_stack0);
		}
		printf(" ");
		if(ttychline()!= ';') {
		  interp((ITEM)NULL,TRUE,TRUE);
		  break;
		}
		succeeded=interp((ITEM)I_TERM,TRUE,TRUE);
	}
	if(succeeded) printf("yes\n");
	else printf("no\n");
}


main_prompt()
	{
	ITEM c;
	char mess[MAXMESS],mess1[MAXMESS];
	double start;
	for (;;) {
		printf("|- ");
		if((c=ccl_ttyread())==(ITEM)I_TERM) break;
		else if(c==(ITEM)I_ERROR) continue;
		ccl_swrite(mess,c);
		start=cputime();
		c_interp(c);
		sprintf(mess1,"%s - Time taken %.2lfs",mess,
			fabs(cputime()-start));
		g_message(1l,mess1);
		i_delete(c);
	}
}
