#include <stdio.h>
#include "progol.h"

/*
 * #######################################################################
 *
 *			Functor Processing Routines
 *			---------------------------
 *
 * #######################################################################
 */

FUNC
f_create(size)
	LONG size;
	{
	FUNC result;
	char mess[MAXWORD];
	if (!(result = (FUNC)PROGOL_CALLOC(1l, sizeof(struct functor)))) {
		sprintf(mess,"f_create - calloc failure 1 (size=%d,mem=%d)",
			size,memout);
		d_error(mess);
	}
	else if (!(result->arr = (ITEM *)PROGOL_CALLOC(size,sizeof(ITEM)))) {
		sprintf(mess,"f_create - calloc failure 2 (size=%d,mem=%d)",
			size,memout);
		d_error(mess);
	}
	else
		result->arr_size=size;
	return(result);
}

FUNC
f_zero(f)
	FUNC f;
	{
	register ITEM *farr,*fptr=(farr=f->arr)+(f->arr_size);
	while(--fptr>=farr)
		*fptr=(ITEM)NULL;
	return(f);
}

int
f_delete(f)
	FUNC f;
	{
	PROGOL_CFREE(f->arr,f->arr_size * sizeof(ITEM));
	PROGOL_CFREE(f,sizeof(struct functor));
}

/* ####################################################################### 
 *
 * f_ltof/1 - places all the items in a list into a functor
 */

ITEM
f_ltof(i)
	ITEM i;
	{
	FUNC f=f_create(l_length(i));
	ITEM result=i_create('f',(POINTER)f);
	register ITEM *arrp=f->arr;
	register LIST elem;
	
	LIST_LOOP(elem,(LIST)I_GET(i)) *arrp++ = i_inc(L_GET(elem));
	return(result);
}

/* ####################################################################### 
 *
 * f_ftol/1 - places all the items in a functor into a list
 */

ITEM
f_ftol(i)
	ITEM i;
	{
	ITEM result;
	register LIST *last=L_LAST(result=L_EMPTY);
	register FUNC f=(FUNC)I_GET(i);
	register ITEM *fptr;

	FUNC_LOOP(fptr,f) last=l_end(i_inc(*fptr),last);
	return(result);
}

/*
 * f_enlarge/1 - doubles the length of the given array, copies
 *	the old elements to the new array, swaps the
 *	new for the old array and deletes the old one.
 */

int
f_enlarge(i)
	ITEM i;
	{
	register FUNC f1=(FUNC)I_GET(i),
		f2=f_zero(f_create(f1->arr_size << 1));
	register ITEM *fptr1,*fptr2;
	for(fptr1=f1->arr+f1->arr_size,fptr2=f2->arr+f1->arr_size;
		fptr1>f1->arr;*(--fptr2)= *(--fptr1));
	I_GET(i)=(POINTER)f2;
	f_delete(f1);
}

/*
 * f_ins/3 - returns item pointer to given place in array, enlarging
 *	array if necessary.
 */

ITEM *
f_ins(n,array)
	LONG n;
	ITEM array;
	{
	register FUNC f;
	while((f=(FUNC)I_GET(array))->arr_size<=n)
	  f_enlarge(array);
	return(&(f->arr[n]));
}

/*
 * f_copy/1 - does a shallow copy of an array
 */

ITEM
f_copy(i)
	ITEM i;
	{
	register ITEM *fptr1,*fptr2,result;
	register FUNC f1,f2=(FUNC)I_GET(i);
	result=i_create('f',(POINTER)(f1=f_create(f2->arr_size)));
	FUNC_LOOP2(fptr1,fptr2,f1,f2)
		*fptr1=i_inc(*fptr2);
	return(result);
}
