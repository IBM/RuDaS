#include <stdio.h>
#include "progol.h"

/*
 * #######################################################################
 *
 *                      List Processing Routines
 *                      ------------------------
 *
 * #######################################################################
 */

/* ####################################################################### 
 *
 * Add element to end of list. No search
 *	to the end of list necessary.
 */

LIST *
l_end(val, last)
        register ITEM val;
        register LIST *last;
        {
	register LIST new_mem;
	if (!(new_mem = (LIST)PROGOL_CALLOC(1l, sizeof(struct lmem)))) {
                d_error("l_end - PROGOL_CALLOC failed");
        }
	new_mem->pres=val;
	new_mem->next=(LIST)NULL;
	*last=new_mem;
	return(&(new_mem->next));
}

/* ####################################################################### 
 *
 * l_push/2 - push object onto list-based stack.
 *	Usage counter of object unchanged.
 */

ITEM
l_push(val,i)
        ITEM val; 
        ITEM i;
        {
	LIST list;        
	LIST new_mem;
	if (i->item_type != 'l')
		d_error("l_push - item not a list");
	else if (!(new_mem = (LIST)PROGOL_CALLOC(1l,sizeof(struct lmem))))
                d_error("l-suffix - PROGOL_CALLOC failed");
	else {
		new_mem->pres=i_inc(val);
		if(!(list=(LIST)I_GET(i))) i->extra=(LONG)&(new_mem->next);
		new_mem->next=list;
		I_GET(i)=(POINTER)new_mem;
	}
	return(i);
}

/* ####################################################################### 
 *
 * l_pop/ - pops the first object off a list-based stack. Usage counter of
 *		object unchanged.
 */

ITEM
l_pop(l)
	ITEM l;
	{
	LIST list=(LIST)I_GET(l);
	ITEM result;
	if (l->item_type != 'l')
		d_error("l_pop - not handed a list");
	if (!list) d_error("l_pop - handed empty list");
	else {
		result=HOF(list);
		if(!(I_GET(l)=(POINTER)TOF(list)))
			l->extra=(LONG)&I_GET(l);
		L_DEL(list);
	}
	return(result);
}

/* ####################################################################### 
 *
 * l_append/2 - destructively appends 2 lists to each other. The first list
 *	is overwritten and returned. The second list is unaffected.
 */

ITEM
l_append(l1,l2)
	ITEM l1,l2;
        {
	register LIST e,*ep;

	if (!l1 || !l2) d_error("l_append - handed NULL pointer");
	else if (l1->item_type != 'l' || l2->item_type != 'l')
		d_error("l_append - not handed lists");
	else {
		LISTP_LOOP(ep,l1);
		LIST_LOOP(e,(LIST)I_GET(l2))
			L_TERM(e,ep);
		l1->extra=(ULONG)ep;
	}
	return(l1);
}

/*
 * l_length(list) - returns the length of the list
 */

LONG
l_length(list)
	ITEM list;
	{
	register LIST elem;
	register LONG result=0l;
	LIST_LOOP(elem,(LIST)I_GET(list)) result++;
	return(result);
}

/* ####################################################################### 
 *
 * l_reverse/1 - destructive list reverse
 */

ITEM
l_reverse(list)
	ITEM list;
	{
	register LIST rev=(LIST)NULL,firstl=(LIST)I_GET(list),firstr;
	while(firstr=firstl) {
	  firstl= TOF(firstl);	/* Pop list */
	  TOF(firstr)=rev;	/* Push onto rev */
	  rev=firstr;
	}
	I_GET(list)=(POINTER)rev;
	return(list);
}

/*
 * l_copy - makes a surface copy of a list. The underlying
 *	structure is not copied.
 */

ITEM
l_copy(l)
	ITEM l;
	{
	ITEM result;
	register LIST elem,*last=L_LAST(result=L_EMPTY);
	LIST_LOOP(elem,(LIST)I_GET(l)) last=l_end(i_inc(L_GET(elem)),last);
	result->extra=(LONG)last;
	return(result);
}


/*
 * l_ins/2 - Inserts a new item into a list
 *	before the given element. Returns LIST pointer to the
 *	element given as input. If elemp points to NULL, the
 *	effect is the same as l_end, except that the item
 *	pointer is incremented.
 */

LIST *
l_ins(i,elemp)
	ITEM i;
	LIST *elemp;
	{
	register LIST elem= *elemp;
	elemp=l_end(i_inc(i),elemp);
	*elemp=elem;
	return(elemp);
}


/* l_suf(e,l) - suffixes the list l with e. e is placed at the end
 *	of l using the end of list pointer in the extra field of l.
 */

ITEM
l_suf(e,l)
	ITEM e,l;
	{
	register LIST new_mem,*last=(LIST *)l->extra;
	if (!(new_mem=(LIST)PROGOL_CALLOC(1l, sizeof(struct lmem)))) {
                d_error("l_suf - PROGOL_CALLOC failed");
        }
	new_mem->pres=e;
	if(e) I_INC(e);
	new_mem->next=(LIST)NULL;
	*last=new_mem;
	l->extra=(LONG)(&(new_mem->next));
	return(l);
}

/* l_empty() - returns a new empty list. The extra field is assigned
 *	as a LIST * pointing to the end of the list. This is used
 *	by l_suf to add elements to the end of the list.
 */

ITEM
l_empty()
	{
	register ITEM result;
	register LIST *last=L_LAST(result=i_create('l',(POINTER)NULL));
	result->extra=(LONG)last;
	return(result);
}

/* l_ltop - converts a list to a Prolog list
 */

ITEM
l_ltop(l)
	ITEM l;
	{
	ITEM result=(ITEM)NULL,*end= &result,e;
	LIST_DO(e,l)
	  *end=i_tup3(i_dec(i_create('h',pdot2)),e,NULL);
	  end=&F_ELEM(2l,*end);
	LIST_END
	*end=i_create('h',pempty);
	return(result);
}

/* l_ltopc - converts a list to a comma separated Prolog list
 */

ITEM
l_ltopc(l)
	ITEM l;
	{
	ITEM result=(ITEM)NULL,*end= &result;
	LIST elem,ll=(LIST)I_GET(l);
	PENL_LOOP(elem,ll) {
	  *end=i_tup3(i_dec(i_create('h',pcomma2)),L_GET(elem),NULL);
	  end=&F_ELEM(2l,*end);
	}
	*end=i_inc(L_GET(elem));
	return(result);
}

/* l_pctol1 - converts a list to a comma separated Prolog list
 */

ITEM
l_pctol1(l)
	ITEM l;
	{
	ITEM e,end,result=L_EMPTY;
	PCLIST_DO(e,end,l)
	  l_suf(e,result);
	PCLIST_END
	l_suf(end,result);
	return(result);
}

/* l_pctol - converts a Prolog clause represented as a :- b,c,..
 *	to the list (a b c ..)
 */

ITEM
l_pctol(c)
	ITEM c;
	{
	ITEM result;
	if(PSYM(c)==pimplies2) {
	  result=l_pctol1(F_ELEM(2l,c));
	  l_push(F_ELEM(1l,c),result);
	}
	else l_push(c,result=L_EMPTY);
	return(result);
}

/* l_ptol - converts a Prolog list to a list
 */

ITEM
l_ptol(l)
	ITEM l;
	{
	ITEM e,end,result=L_EMPTY;
	PLIST_DO(e,end,l)
	  l_suf(e,result);
	PLIST_END
	if(PSYM(end)!=pempty) {i_delete(result); result=(ITEM)NULL;}
	return(result);
}

/* l_elem(n,l) returns the nth element of l. The first element is element 0.
 *	If list not long enough then returns NULL;
 */

ITEM
l_elem(n,l)
	LONG n;
	ITEM l;
	{
	ITEM e;
	LIST_DO(e,l)
	  if((n--)<=0) return(e);
	LIST_END
	return((ITEM)NULL);
}
