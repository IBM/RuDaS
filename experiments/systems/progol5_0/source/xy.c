/* ####################################################################### */

/*                      PROGOL Fast-Hash Routines                          */
/*                      -------------------------                          */

/* ####################################################################### */

#include        <stdio.h>
#include        "progol.h"

/* pf_ins/2 - this does a fast mapping <x,y>->integer. The domain
 *	is arbitrary and the range is the series 0,1,2,..
 *	All hashing is done using the global table hxy.
 */

#define	XYINIT	16l
LONG *xy_acreate();
LONG *xy_enlarge();

LONG
pf_ins(p,f,add)
	LONG p,f;
	PREDICATE add;
	{
	register LONG *ip,**ipp,pf;
	register ITEM *entry;
	if (!(ip= *(ipp=hxy+HXY(p,f)))) {
		if(!add) return(XYTERM);
		else ip=(*ipp=xy_acreate(XYINIT));
	}
	for(ip++;*ip!= XYTERM && !(*ip==p&& *(ip+1l)==f);ip+=3l);
	if(((ip- *ipp)+4l)> **ipp) ip=xy_enlarge(ipp);
	if(*ip==XYTERM) {	/* New pair */
	  if(!add) return(XYTERM);
	  *ip=p; *(ip+1l)=f; *(ip+2l)=pf=hxyn++; *(ip+3l)=XYTERM;
	  if(!(*(entry=f_ins(p,ptopf)))) *entry=Y_EMPTY;
	  Y_PUSH(pf,*entry);
	}
	return(*(ip+2l));
}

/*
 * xy_acreate/0 - create and initialise a new list of hash cells
 */

LONG *
xy_acreate(size)
	LONG size;
	{
	LONG *result;
	if(!(result=(LONG*)PROGOL_CALLOC(size,sizeof(LONG))))
		d_error("xy_acreate");
	*result=size;
	*(result+1l)=XYTERM;
	return(result);
}

int
xy_adelete(arr)
	LONG *arr;
	{
	PROGOL_CFREE(arr,*arr * sizeof(LONG *));
}

/*
 * xy_enlarge/1 - doubles the length of a list of hash cells terminated by
 *	a XYTERM. Return pointer to the terminator.
 */

LONG *
xy_enlarge(ipp)
	LONG **ipp;
	{
	LONG *newlist=xy_acreate((**ipp)<<1),*ip1,*ip2;
	for(ip1= newlist+1,ip2= *ipp+1;*ip2 != XYTERM;*ip1++ = *ip2++);
	PROGOL_CFREE(*ipp,**ipp * sizeof(LONG *));
	*ipp=newlist;
	*ip1=XYTERM;
	return(ip1);
}

int
xy_write()
	{
	LONG **ipp,*ip,cnt=0l;
	for(ipp=hxy;ipp < hxy+HASH10+1;ipp++,cnt++) {
	  if(ip= *ipp) {
	    printf("%ld(%ld): ",cnt,*ip++);
	    while(*ip != XYTERM) {
		printf("%ld/%ld->%ld ",*ip,*(ip+1l),*(ip+2l));
		ip+=3l;
	    }
	    printf(";\n");
	  }
	}
}
