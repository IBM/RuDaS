/* ####################################################################### */

/*                      PROGOL Cterm Cost Functions			   */
/*                      --------------------------			   */

/* ####################################################################### */

#include        <stdio.h>
#include        "progol.h"

/***************************************************************************
 * ct_cost/1 - gives the cost of a term calculated as follows
 *		variable - 1
 *		term - 2 + sumi(cost(argi))
 *		constant - 2
 *		integer - 2
 *		real - 2
 */

LONG
ct_cost(term)
	ITEM term;
	{
	register LONG result=0l,v;
	register ITEM subt;
	TERM_DO(subt,term)
	  switch(subt->item_type) {
	    case 'v': result++; break;
	    case 'i':
		v=(LONG)I_GET(subt);
		result+=(v<0l? -v+2l:v+2l);
		break;
	    case 'r': {
		double *rp=(double *)I_GET(subt);
		v= *rp;
		result+=v;
	        }
		break;
	    default: result+=2l;
	  }
	TERM_END
	return(result);
}

/* cl_cost/1 - the cost of a clause is sumi(cost(literali))
 */

LONG
cl_cost(cl)
	ITEM cl;
	{
	register ITEM elem;
	register LONG result=0l;
	LIST_DO(elem,cl)
	    result += ct_cost(elem);
	LIST_END
	return(result);
}

/* cl_costs/1 - the cost of a set of clauses is sumi(cost(clausei))
 */

LONG
cl_costs(cls)
	ITEM cls;
	{
	register ITEM cl;
	register LONG result=0l;
	LIST_DO(cl,cls)
	    result += cl_cost(cl);
	LIST_END
	return(result);
}

/* bc_costs - cost of clauses in ancestor predicates of psym.
 */

LONG
bcl_costs(psym)
	LONG psym;
	{
	ITEM anc=ANC(psym),bcl;
	LONG cno,result=0l,psym1;
	BIT_DO(psym1,anc)
	  if(bcl= *f_ins(psym1,bptab)) {
	    BIT_DO(cno,bcl)
	      result+= *y_ins(cno,costs);
	    BIT_END
	  }
	BIT_END
	i_delete(anc);
	return(result);
}
