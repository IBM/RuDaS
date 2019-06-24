#include <stdio.h>
#include "progol.h"

/*
 * #######################################################################
 *
 *			Routines related to subsumption testing
 *                      ---------------------------------------
 *
 * #######################################################################
 */


/* s_ugrecq - tests whether given clause has an unguarded recursion.
 *	This is decided by testing whether the head unifies with
 *	a skolemisation of the first atom in the body.
 */

PREDICATE
s_ugrecq(ccl)
	ITEM ccl;
	{
	PREDICATE result=FALSE;
	ITEM head,first,term,u;
	LIST l,t;
	if((l=(LIST)I_GET(F_ELEM(0l,ccl)))&&(t=TOF(l))&&(head=HOF(l))&&
		(first=HOF(t))&&(PSYM(head)==PSYM(first))) {
	  TERM_DO(term,first)	/* Skolemise first atom in body */
	    if(term->item_type== 'v') term->item_type= 'k';
	  TERM_END
	  u=i_tup3(i_dec(i_create('h',(POINTER)peq2)),head,first);
	  I_GET(F_ELEM(1l,gcall))=(POINTER)ct_vmax(head)+1l;
	  result=d_groundcall(u);
	  I_GET(F_ELEM(1l,gcall))=(POINTER)0l;
	  TERM_DO(term,first)	/* Unskolemise first atom in body */
	    if(term->item_type== 'k') term->item_type= 'v';
	  TERM_END
	  i_delete(u);
	}
	return(result);
}

/* s_guard - given an unguarded recursion this function ensures
 *	finite failure by forcing the recursion to match only unit
 *	clauses.  For instance, the clause
 *		lt(U,V) :- lt(U,W), lt(W,V)	is transformed to
 *
 *	lt(U,V) :- clause(lt(U,W),true), W=<V.
 *
 */

s_guard(ccl,libq)
	ITEM ccl;
	PREDICATE libq;
	{
	char name1[MAXMESS];
	ITEM *first,atom;
	if(!libq) {
	  ccl_swrite(name1,ccl);
	  printf("[<%s> - non-termination being guarded]\n",name1);
	}
	atom= *(first= &(HOF(TOF((LIST)I_GET(F_ELEM(0l,ccl))))));
	*first=i_tup3(i_dec(i_create('h',QP_ston("clause",2l))),
		i_dec(atom),i_dec(i_create('h',ptrue)));
}

ITEM s_gcopy();

ITEM
s_gcopies(psym,ccl)
	LONG psym;
	ITEM ccl;
	{
	ITEM atom,result=L_EMPTY,cl=F_ELEM(0l,ccl);
	LONG ano=0l;
	LIST_DO(atom,cl)
	  if(ano&&PSYM(atom)==psym) {
	    l_suf(i_dec(s_gcopy(ccl,ano)),result);
	  }
	  ano++;
	LIST_END
	return(result);
}

ITEM
s_gcopy(ccl,ano)
	ITEM ccl;
	LONG ano;
	{
	ITEM result=i_copy(ccl),cl=F_ELEM(0l,result);
	LIST *lp,elem,elem0;
	LISTP_LOOP(lp,cl) {
	  if(!(ano--)) {
	    elem= *lp;
	    *lp = (*lp)->next;
	    elem->next=TOF(elem0=(LIST)I_GET(cl));
	    elem0->next=elem;
	    break;
	  }
	}
	return(result);
}
