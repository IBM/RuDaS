/* ####################################################################### */

/*			PROGOL hashing functions				   */
/*			----------------------				   */

/* ####################################################################### */

#include        <stdio.h>
#include        "progol.h"


/*
 * A hash table is a functor of lists
 */

PREDICATE h_add_eq();
LONG h_add_hfn();

/******************************************************************
 * superhash - a table of equality predicates and hash functions
 */

struct hashfns superhash[] = {
	h_keyeq, h_hash,	/* general key-pair */
	h_add_eq, h_add_hfn,	/* Hashing and equivalence */
				/*  based on address */
	0, 0
};

/* #######################################################################
 *
 * h_create/1 - hash table size is calculated as 2^size for the given size.
 *	Thus size should be in the range 4-10
 */

ITEM
h_create(size)
	LONG size;
	{
	FUNC f;
	register ITEM result=i_create('f',f=f_create(1l<<size)),*fptr;
	
	FUNC_LOOP(fptr,f) *fptr=(ITEM)NULL; 
	return(result);
}

/* ####################################################################### 
 *
 * h_hash/2 - generalised hashing function
 */

LONG
h_hash(i,htable)
	ITEM i,htable;
	{
	register char *strp;
	register ITEM *felem;
	register LIST elem;
	double *real;
	FUNC f;
	register LONG result,hash_sz;
	register BLOCK bp,be,b;
	if (!i) return(0l);
	else {
		result=0l;
		f=(FUNC)I_GET(htable);
		hash_sz=f->arr_size-1;
		switch (i->item_type) {
		    case 'i': case 'v': case 'h': case 'k':
			result =(LONG)I_GET(i);
			break;
		    case 'r':
			real=(double *)I_GET(i);
			result = *real;
			break;
		    case 'a':
		    case 's':
			STR_LOOP(strp,(STRING)I_GET(i))
				result += *strp;
			break;
		    case 'b': case 'I':
			b=(BLOCK)I_GET(i);
			BLOCK_LOOP(bp,be,b)
				result += *bp;
			break;
		    case 'R':
			R_DO(real,i)
				result += *real;
			R_END
			break;
		    case 'l':
			LIST_LOOP(elem,(LIST)I_GET(i))
				result += h_hash(L_GET(elem),htable);
			break;
		    case 'f':
			f=(FUNC)I_GET(i);
			FUNC_LOOP(felem,f)
				result += h_hash(*felem,htable);
			break;
		    default:
			d_error("h_hash - bad item type");
		}
	}
	return(result & hash_sz);
}

/*
 * h_gen - generalised hashing function. Uses hashing function
 *	and equality testing function from superhash table, using
 *	hashi to tell it which to apply. List pointer returned
 *	is either pointing to NULL pointer at end of list
 *	if not there (i.e. list can be extended), or to the list
 *	cell containing the record.
 */

LIST *
h_gen(i,htable,hashi)
	ITEM i,htable;
	LONG hashi;
	{
	register LIST *last;
	struct hashfns *hptr=superhash + hashi;
	ITEM *hplace= &(((FUNC)I_GET(htable))->arr[hptr->hfn(i,htable)]);
	if (!(*hplace)) return(L_LAST(*hplace=L_EMPTY));
	LISTP_LOOP(last,*hplace)
		if (hptr->eq(i,L_GET(*last))) break;
	return(last);
}

/*
 * h_keyeq - standard equality predicate for hash records which
 *	are a key/item pair
 */

PREDICATE
h_keyeq(i1,irec2)
	ITEM i1,irec2;
	{
	FUNC f=(FUNC)I_GET(irec2);
	register ITEM i2= *(f->arr);
	PREDICATE result;

	result=(ITMEQ(i1,i2) ? TRUE : FALSE);
	return(result);
}

/********************************************************************
 * Hashing functions for address based hashing and equality.
 *	Used with h_gen.
 */

LONG
h_add_hfn(i,htable)
	ITEM i,htable;
	{
	FUNC f=(FUNC)I_GET(htable);
	LONG num= (LONG)i,hash_sz=f->arr_size - 1;

	return((num >> 2) & hash_sz);
}

PREDICATE
h_add_eq(i1,i2)
	ITEM i1,i2;
	{
	return(i1 == i2 ? TRUE : FALSE);
}

/*
 * h_ins - assumes hash records are stored as pairs (HGEN). Returns
 *	Pointer to hash entry if one is found. Otherwise adds in new
 *	entry with given key and returns pointer to null entry.
 */

ITEM *
h_ins(key,htable)
	ITEM key,htable;
	{
	LIST *hlast;
	register ITEM *result;
	FUNC f;
	if (*(hlast=h_gen(key,htable,HGEN))) /* Already in */
		result= ((FUNC)I_GET(L_GET(*hlast)))->arr + 1;
	else {		/* New one */
		l_end(i_create('f',f=f_create(2l)),hlast);
		result= f->arr;
		*result++ = i_inc(key);
		*result= (ITEM)NULL;
	}
	return(result);
}

/*
 * h_del - assumes hash records are stored as pairs (HGEN). Deletes
 *	pairwise record. Error given if record not found.
 */

int
h_del(key,htable)
	ITEM key,htable;
	{
	LIST *hlast,elem;
	if (elem=(*(hlast=h_gen(key,htable,HGEN)))) {	/* Found */
	  *hlast=elem->next;
	  i_delete(L_GET(elem));
	  L_DEL(elem);
	}
}
