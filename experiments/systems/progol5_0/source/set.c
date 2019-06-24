
/* ####################################################################### */

/*                      PROGOL Set Routines                                  */
/*                      -----------------                                  */

/* ####################################################################### */

#include        <stdio.h>
#include        "progol.h"

LIST m_sort(),merge();

/* #######################################################################
 *
 * i_sort/1 - destructive merge sort. Uses comparison function given as
 *	argument.
 */

ITEM
i_sortc(list,cmp)
	ITEM list;
	LONG (*cmp) ();
	{
	if (!list) d_error("i_sort - handed null pointer");
	else if (list->item_type != 'l') d_error("i_sort - not handed a list");
	else I_GET(list)=(POINTER)m_sort((LIST)I_GET(list),cmp);
	SUFLIST(list);
	return(list);
}

/* #######################################################################
 *
 * q_sort/1 - destructive quick sort
 */

LIST
q_sort(in_list)
        register LIST in_list; 
        {
	register LIST elem,head=in_list,tail,list3,list4;
	LIST list1,list2;

        if (!in_list) return(in_list);
	else tail=in_list->next;
        partition(head,tail,&list1,&list2);
        list3=q_sort(list1);
        list4=q_sort(list2);
        
        /* Stick together as list3|head|list4 */

	if (list3) {
		PENL_LOOP(elem,list3);
		elem->next=head;
	}
	else list3=head;
        head->next=list4;
        return(list3);
}

/* ####################################################################### */

int
partition(comp,in_list,out1,out2)
                                        /* partition in_list into       */
                                        /*      out1 & out2             */
        register LIST comp,in_list;
        register LIST *out1;
	register LIST *out2; 
        {
        register LIST elem=in_list,rest;

	*out1=(LIST)NULL; *out2=(LIST)NULL;
	while(elem) {
		rest=elem->next;
		elem->next=(LIST)NULL;
		switch(i_cmp(L_GET(elem),L_GET(comp))) {
			case EQ: i_delete(L_GET(elem));
				L_DEL(elem);
				break;
			case LT: *out1=elem;
				out1 = &(elem->next);
				break;
			case GT: *out2=elem;
				out2 = &(elem->next);
				break;
			default: d_error("partition - bad switch value");
		}
		elem=rest;
	}
}

/* #######################################################################
 *
 * m_sort/1 - destructive merge sort
 */

LIST
m_sort(in_list,cmp)
        register LIST in_list; 
	LONG (*cmp) ();
        {
	LIST list1,list2,list3,list4;

        if (!in_list || !in_list->next) return(in_list);
        split(in_list,&list1,&list2);
        list3=m_sort(list1,cmp);
        list4=m_sort(list2,cmp);
        
        return(merge(list3,list4,cmp));
}

/* ####################################################################### */

int
split(in_list,out1,out2)
                                        /* split in_list into       */
                                        /*      out1 & out2         */
        register LIST in_list,*out1,*out2; 
        {
        register LIST elem=in_list,rest;
	PREDICATE first=TRUE;

	*out1=(LIST)NULL; *out2=(LIST)NULL;
	while(elem) {
		rest=elem->next;
		elem->next=(LIST)NULL;
		if (first) {
			*out1=elem;
			out1= &(elem->next);
			first=FALSE;
		}
		else {
			*out2=elem;
			out2= &(elem->next);
			first=TRUE;
		}
		elem=rest;
	}
}

/* #######################################################################
 *
 * merge/2 - creates a new list which is the union of the args
 *		the argument lists should be in sorted order
 */

LIST
merge(list1,list2,cmp)
	LIST list1,list2;
	LONG (*cmp) ();
	{
	LIST elem1=list1,elem2=list2,result,*last= &result,
		del;

	while (elem1 && elem2) {
		switch((*cmp) (L_GET(elem1),L_GET(elem2))) {
			case EQ: del=elem1;
				*last=elem2;
				elem1=elem1->next;
				elem2=elem2->next;
				i_delete(L_GET(del));
				L_DEL(del);
				break;
			case LT: *last=elem1;
				elem1=elem1->next;
				break;
			case GT: *last=elem2;
				elem2=elem2->next;
				break;
			default: d_error("partition - bad switch value");
		}
		last= &((*last)->next);
	}
	*last= (elem1 ? elem1 : elem2);
	return(result);
}

/* #######################################################################
 *
 * set_subset/2 - set subset test. Tests set1 improper subset of set2.
 *
 */

PREDICATE
set_subset(set1,set2)
	ITEM set1,set2;
	{
	register LIST elem1,elem2;

	if (!set1 || !set2) d_error("set_subset - handed NULL pointer");
	else if (set1->item_type != 'l' || set2->item_type != 'l')
		d_error("set_subset - not handed lists");
	elem1=(LIST)I_GET(set1);
	elem2=(LIST)I_GET(set2);
	while (elem1) {
		if (!elem2)
			return(FALSE);
		else {
		    switch(i_cmp(L_GET(elem1),L_GET(elem2))) {
			case EQ:
				elem1 = elem1->next;
				elem2 = elem2->next;
				break;
			case LT:
				return(FALSE);
			case GT:
				elem2 = elem2->next;
				break;
			default: d_error("set_subset - bad switch value");
		    }
		}
	}
	return(TRUE);
}

/* #######################################################################
 *
 * set_uni/2 - destructive set union. The set union overwrites
 *	the set in the first argument. The first argument is returned.
 *	Both arguments should be sorted sets. For efficiency, this
 *	is not checked.
 */

ITEM
set_uni(set1,set2)
	ITEM set1,set2;
	{
	register LIST *elemp1,elem2;

	if(!set1 || !set2) d_error("set_uni - handed NULL pointer");
	else if (set1->item_type != 'l' || set2->item_type != 'l')
		d_error("set_uni - not handed lists");
	elemp1=L_LAST(set1);
	elem2=(LIST)I_GET(set2);
	while (*elemp1 || elem2) {
		if (!(*elemp1)) {
			L_TERM(elem2,elemp1);
			elem2 = elem2->next;
		}
		else if (!elem2) break;
		else switch(i_cmp(L_GET(*elemp1),L_GET(elem2))) {
			case EQ: elemp1 = &((*elemp1)->next);
				elem2 = elem2->next;
				break;
			case LT: elemp1 = &((*elemp1)->next);
				break;
			case GT: elemp1=l_ins(L_GET(elem2),elemp1);
				elem2= elem2->next;
				break;
			default: d_error("set_uni - bad switch value");
		}
	}
	return(set1);
}

/* #######################################################################
 *
 * set_sub/2 - destructive set subtraction. The set difference overwrites
 *	the set in the first argument. The first argument is returned.
 *	Both arguments should be sorted sets. For efficiency, this
 *	is not checked.
 */

ITEM
set_sub(set1,set2)
	ITEM set1,set2;
	{
	register LIST *elemp1,elem2;

	if(!set1 || !set2) d_error("set_sub - handed NULL pointer");
	else if (set1->item_type != 'l' || set2->item_type != 'l')
		d_error("set_sub - not handed lists");
	elemp1=L_LAST(set1);
	elem2=(LIST)I_GET(set2);
	while (*elemp1) {
		if (!elem2) break;
		else {
		    switch(i_cmp(L_GET(*elemp1),L_GET(elem2))) {
			case EQ: L_REM(elemp1)
				elem2 = elem2->next;
				break;
			case LT: elemp1 = &((*elemp1)->next);
				break;
			case GT: elem2 = elem2->next;
				break;
			default: d_error("set_sub - bad switch value");
		    }
		}
	}
	return(set1);
}

/* #######################################################################
 *
 * set_int/2 - destructive set intersection. The set intersection overwrites
 *	the set in the first argument. The first argument is returned.
 *	Both arguments should be sorted sets. For efficiency, this
 *	is not checked.
 */

ITEM
set_int(set1,set2)
	ITEM set1,set2;
	{
	register LIST *elemp1,elem2;

	if(!set1 || !set2) d_error("set_int - handed NULL pointer");
	else if (set1->item_type != 'l' || set2->item_type != 'l')
		d_error("set_int - not handed lists");
	elemp1=L_LAST(set1);
	elem2=(LIST)I_GET(set2);
	while (*elemp1 || elem2) {
		if (!(*elemp1)) break;
		else if (!elem2) L_REM(elemp1)
		else switch(i_cmp(L_GET(*elemp1),L_GET(elem2))) {
			case EQ: elemp1 = &((*elemp1)->next);
				elem2 = elem2->next;
				break;
			case LT: L_REM(elemp1)
				break;
			case GT: elem2 = elem2->next;
				break;
			default: d_error("set_int - bad switch value");
		}
	}
	return(set1);
}

