#include <stdio.h>
#include "progol.h"

/*
 * #######################################################################
 *
 *                      Item Processing Functions
 *                      ------------------------
 *
 * #######################################################################
 */

#ifdef MEMCHECK
ITEM all_items;
#endif

/* ####################################################################### 
 *
 * i_copy/1 - recursively produces a copy of an item.
 */
void put1tfile1(c)
	register char c;
	{
	*(glob_file->bufp)++ = c;
	if (glob_file->bufp < ((glob_file->buf+LBUF) + BUFSIZE)) return;
	else frecflush(glob_file);
}

PREDICATE instring,reset=FALSE;

void
put1tfile(c)
	register char c;
	{
	if(!(reset && c==' ')) put1tfile1(c);
	switch(c) {
	    case '\'':
		instring=(instring ? FALSE : TRUE);
		line_cnt++;
		break;
	    case '\n':
		line_cnt=0l;
		break;
	    case '\t':
		line_cnt += 8;
		break;
	    case ' ': case ',': case '|':
		if ((line_cnt++ >= 60) && !instring) {
			put1tfile1('\n');
			put1tfile1('\t');
			line_cnt=8;
			reset=TRUE;
			return;
		}
		break;
	    default:
		line_cnt++;
	}
	reset=FALSE;
}


void i_delete(i)
	ITEM i;
	{
	register LIST elem;
	register ITEM *felem;
	FUNC f;
#ifdef MEMCHECK
	LIST *hlast;
#endif
	if (i == (ITEM)NULL)	return;
	else if (!(i->usage)) d_error("i_delete - deleting item with no usage");
	else if (--(i->usage) != 0)	return;
	else {
#ifdef MEMCHECK
	    if (*(hlast=h_gen(i,all_items,ADDR))) {
		L_DEL(*hlast);
		*hlast= (*hlast)->next;
	    }
	    else d_error("i_delete - item not found");
#endif
	    switch(i->item_type) {
		case 'i': 	/* Integer */
		case 'v': 	/* Prolog variable */
		case 'h': 	/* Prolog name */
		case 'k': 	/* Skolem constant */
			break;
		case 'b':	/* Block */
			b_delete((BLOCK)I_GET(i));
			break;
		case 'I': /* Integer Array */
			y_delete((unsigned long int *)I_GET(i));
			break;
		case 'R': /* Real Array */
			R_delete((double *)I_GET(i));
			break;
		case 'r':	/* Real number */
			PROGOL_CFREE((POINTER)I_GET(i), sizeof(double));
			break;
		case 'a':	/* Atom */
		case 's':	/* String */
			S_DEL((POINTER)I_GET(i));
			break;
		case 'f':	/* Functor */
			f=(FUNC)I_GET(i);
			FUNC_LOOP(felem,f) {
				i_delete(*felem);
			}
			f_delete(f);
			break;
		case 'l':	/* List */
			LIST_LOOP(elem,(LIST)I_GET(i)) {
				i_delete(L_GET(elem));
				L_DEL(elem);
			}
			break;
		default:
			d_error("i_delete - invalid item type");
	    }
	    PROGOL_CFREE((POINTER)i,sizeof(struct item));
	}
}

/* ####################################################################### 
 *
 * i_write/3 - writes any item using the generic function put1ch for
 *	writing individual characters. used both for writing to strings
 *	and for writing to files. extra flag decides whether to print
 *	field 'extra'. this is printed in hex between <>s when not unassigned.
 */

void i_write(i,put1ch,qextra)
	ITEM i;
	LONG (*put1ch) ();
	PREDICATE qextra;
	{
	char c;
	LONG nib_cnt,val;
	double *real;
	char mess[MAXWORD];
	STRING strp;
	LIST elem;
	ITEM *felem;
	FUNC f;
	BLOCK b,bp,bf;

	if (!i) {(*put1ch) ('_'); return;}
	else if (i->usage == 0) d_error("i_fwrite - item with zero usage");
	else {
	    switch(i->item_type) {
		case 'i':	/* Integer */
			sprintf(mess,"%d",(LONG)I_GET(i));
			STR_LOOP(strp,mess)
				(*put1ch) (*strp);
			break;
		case 'v':	/* Prolog variable */
			sprintf(mess,"@%d",(LONG)I_GET(i));
			STR_LOOP(strp,mess) (*put1ch) (*strp);
			break;
		case 'k':	/* Skolem constant */
			sprintf(mess,"$%d",(LONG)I_GET(i));
			STR_LOOP(strp,mess) (*put1ch) (*strp);
			break;
		case 'b':	/* Block */
			(*put1ch) ('#');
			b=(BLOCK)I_GET(i);
			BLOCK_LOOP(bp,bf,b) {
			  val= *bp;
			  for(nib_cnt=INT_SZ-NBBL_SZ;nib_cnt>=0l;nib_cnt-=NBBL_SZ) {
			    c=val&0xf;
			    (*put1ch) ((c <= 9) ? (c+'0') : (c-10+'A'));
			    val >>= NBBL_SZ;
			  }
			}
			break;
		case 'I':		/* Integer Array */
			(*put1ch) ('%');
			Y_DO(bp,i)
			  sprintf(mess,"%d",*bp);
			  STR_LOOP(strp,mess)
				(*put1ch) (*strp);
			  (*put1ch) ('|');
			Y_END
			break;
		case 'R':	/* Real Array */
			(*put1ch) ('%');
			R_DO(real,i)
			  sprintf(mess,"%.3f",*real);
			  STR_LOOP(strp,mess)
				(*put1ch) (*strp);
			  (*put1ch) ('|');
			R_END
			break;
		case 'r':	/* Float */
			real = (double *)I_GET(i);
			sprintf(mess,"%.3f",*real);
			STR_LOOP(strp,mess) (*put1ch) (*strp);
			break;
		case 'a':	/* Atom */
			STR_LOOP(strp,(STRING)I_GET(i)) (*put1ch) (*strp);
			break;
		case 's':	/* String */
			(*put1ch) ('\'');
			STR_LOOP(strp,(STRING)I_GET(i)) {
				if (*strp == '\'') (*put1ch) ('\\');
				(*put1ch) (*strp);
			}
			(*put1ch) ('\'');
			break;
		case 'h':	/* Prolog name */
			(*put1ch) ('`');
			STR_LOOP(strp,QP_ntos((LONG)I_GET(i))) {
				if (*strp == '`') (*put1ch) ('\\');
				(*put1ch) (*strp);
			}
			(*put1ch) ('`');
			break;
		case 'f':	/* Functor */
			(*put1ch) ('[');
			f=(FUNC)I_GET(i);
			if (f->arr_size) {
				for(felem=f->arr;felem < f->arr+f->arr_size-1;
						felem++) {
					i_write(*felem,put1ch,qextra);
					(*put1ch) (' ');
				}
				i_write(*felem,put1ch,qextra);
			}
			(*put1ch) (']');
			break;
		case 'l':	/* List */
			(*put1ch) ('(');
			if (I_GET(i)) {
				PENL_LOOP(elem,(LIST)I_GET(i)) {
					i_write(L_GET(elem),put1ch,qextra);
					(*put1ch) (' ');
				}
				i_write(L_GET(elem),put1ch,qextra);
			}
			(*put1ch) (')');
			break;
		default:
			d_error("i_write - invalid item type");
	    }
	}
	if(qextra) {
		sprintf(mess,"<%x>",i->extra);
		STR_LOOP(strp,mess) (*put1ch) (*strp);
	}
}





ITEM
i_copy(i)
	ITEM i;
	{
	ITEM result;
	register ITEM *felem1,*felem2;
	register LIST elem,*last;
	FUNC f1,f2;
	if (!i) return((ITEM)NULL);
	else {
		switch (i->item_type) {
		    case 'i': case 'a':
		    case 'h': case 'v': case 'k':
			result=i_create(i->item_type,(POINTER)I_GET(i));
			break;
		    case 'r':
			{double *r=(double *)I_GET(i);
			result=i_create('r',(POINTER)r_create(*r));
			}
			break;
		    case 'b':
			result=b_copy(i);
			break;
		    case 'I':
			result=b_copy(i);
			result->item_type='I';
			break;
		    case 'R':
			result=R_copy(i);
			break;
		    case 'l':
			last=L_LAST(result=L_EMPTY);
			LIST_LOOP(elem,(LIST)I_GET(i))
				last=l_end(i_copy(L_GET(elem)),last);
			result->extra=(LONG)last;
			break;
		    case 'f':
			result=i_create('f',
			    (POINTER)f_create(((FUNC)I_GET(i))->arr_size));
			f1=(FUNC)I_GET(result); f2=(FUNC)I_GET(i);
			FUNC_LOOP2(felem1,felem2,f1,f2)
				*felem1= i_copy(*felem2);
			break;
		    case 's':
			result=i_create(i->item_type,
				strsave((POINTER)I_GET(i)));
			break;
		    default:
			d_error("i_copy - bad item type");
		}
		if(i->item_type!='l') result->extra=i->extra;
	}
	return(result);
}

/* ####################################################################### 
 *
 * i_cmp/2 - recursively compares two items and says whether the first
 *	is greater than (GT) equal to (EQ) or less than (LT) the second
 */

LONG
i_cmp(i1,i2)
	register ITEM i1,i2;
	{
	LONG result,n1,n2;
	LONG sz1,sz2;
	double *rp1,*rp2,r1,r2;
	FUNC f1,f2;
	register LIST e1,e2;
	register ITEM *fe1,*fe2;
	register BLOCK bp1,bp2,be1,be2,b1,b2;
	register char t1,t2;

	if (i1 == i2) return(EQ);
	else if (!i1) return(LT);
	else if (!i2) return(GT);
	t1=i1->item_type; t2=i2->item_type;
	if(t1 != t2) {
	    if(t1== 'i' &&t2== 'r') {	/* Integer/Float comparison */
	      n1=(LONG)I_GET(i1); r1=n1; rp2=(double *)I_GET(i2);
	      if(r1 == *rp2) result=EQ;
	      else result=((r1 > *rp2)?GT:LT);
	    }
	    else if(t1== 'r' &&t2== 'i') {
	      n2=(LONG)I_GET(i2); r2=n2; rp1=(double *)I_GET(i1);
	      if(*rp1 == r2) result=EQ;
	      else result=((*rp1 > r2)?GT:LT);
	    }
	    else return((t1 > t2) ? GT : LT);
	}
	else {
	    switch(t1) {
		case 'i':	/* Integer */
		case 'v':	/* Prolog variable */
		case 'k':	/* Skolem constant */
		    if ((LONG)I_GET(i1) == (LONG)I_GET(i2)) result=EQ;
		    else
			result=(((LONG)I_GET(i1)>(LONG)I_GET(i2))?GT:LT);
		    break;
		case 'r':	/* Float */
		    rp1 = (double *)(I_GET(i1)); rp2 = (double *)(I_GET(i2));
		    if (*rp1 == *rp2) result=EQ;
		    else result=((*rp1 > *rp2)?GT:LT);
		    break;
		case 'a':	/* Atom */
		case 's':	/* String */
		    result=strcmp((STRING)I_GET(i1),(STRING)I_GET(i2));
		    if (result) result=(result>0 ? GT : LT);
		    break;
		case 'f':	/* Functor */
		    f1=(FUNC)I_GET(i1); f2=(FUNC)I_GET(i2);
		    FUNC_LOOP2(fe1,fe2,f1,f2) {
			switch(result=i_cmp(*fe1,*fe2)) {
			    case EQ:
				break;
			    case LT:
			    case GT:
				return(result);
			    default:
				d_error("i_cmp - invaid inequality value");
			}
		    }
		    if (fe1 >= (f1->arr + f1->arr_size)) {
			if (fe2 >= (f2->arr + f2->arr_size)) result=EQ;
			else result=LT;
		    }
		    else result=GT;
		    break;
		case 'b':	/* Block */
		case 'I':	/* Integer array */
		    b1=(BLOCK)I_GET(i1); b2=(BLOCK)I_GET(i2);
		    BLOCK_LOOP2(bp1,bp2,be1,be2,b1,b2) {
			LONG v1=*bp1,v2=*bp2;
			if (v1 > v2) return(GT);
			else if (v1 < v2) return(LT);
		    }
		    if ((sz1=b_size(i1))==(sz2=b_size(i2))) result=EQ;
		    else result=((sz1>sz2)?GT:LT);
		    break;
		case 'R':	/* Float array */
		    {double *ar1=I_FP(i1),*ar2=I_FP(i2),
			*e1=ar1+1l,*e2=ar2+1l,*f1,*f2;
		     sz1= *ar1; sz2=*ar2;
		     f1=ar1+sz1; f2=ar2+sz2;
		     for(;e1<=f1&&e2<=f2;e1++,e2++) {
		       /* Check whether comparison masked in extra field */
		       if(!((i1->extra)>>(e1-(ar1+1)))&&
		       		!((i2->extra)>>(e2-(ar2+1)))) {
		         if(*e1 > *e2) return(GT);
		         else if (*e1 < *e2) return(LT);
		       }
		     }
		    }
		    if ((sz1=R_SIZE(i1))==(sz2=R_SIZE(i2))) result=EQ;
		    else result=((sz1>sz2)?GT:LT);
		    break;
		case 'l':	/* List */
		   { ITEM h1,h2,f1,f2; LONG l1,l2;
		    /* Test for correct clause ordering */
		    if(!L_EMPTYQ(i1)&& !L_EMPTYQ(i2)&&
			(h1=HOF((LIST)I_GET(i1))) &&
			(h2=HOF((LIST)I_GET(i2))) &&
			h1->item_type== 'f' &&
			h2->item_type== 'f' &&
			(f1=F_ELEM(0l,h1))->item_type== 'h' &&
			(f2=F_ELEM(0l,h2))->item_type== 'h' &&
			(LONG)I_GET(f1)==(LONG)I_GET(f2) &&
			(l1=l_length(i1))!=(l2=l_length(i2)))
				result=(l1>l2?GT:LT);
		    else {
		      LIST_LOOP2(e1,e2,(LIST)I_GET(i1),(LIST)I_GET(i2)) {
			switch (result=i_cmp((ITEM)L_GET(e1),(ITEM)L_GET(e2))) {
			    case EQ:
				break;
			    case LT:
			    case GT:
				return(result);
			    default:
				d_error("i_cmp - invalid inequality value");
			}
		      }
		      if (!e1) {
			if (!e2) result=EQ;
			else result=LT;
		      }
		      else result=GT;
		    }
		    break;
		   }
		case 'h':	/* Prolog name */
		    if(namecmp) {
		      if ((LONG)I_GET(i1) == (LONG)I_GET(i2))
			result= EQ;
		      else {
			result=strcmp(QP_ntos((LONG)I_GET(i1)),
			    QP_ntos((LONG)I_GET(i2)));
			result=(result>0 ? GT : LT);
		      }
		    }
		    else {
		      if ((LONG)I_GET(i1) == (LONG)I_GET(i2)) result=EQ;
		      else result=(((LONG)I_GET(i1)>(LONG)I_GET(i2))?GT:LT);
		    }
		    break;
		default:
		    d_error("i_cmp - invalid item type");
	    }
	}
	return(result);
}

/* ####################################################################### 
 *
 * i_inc/1 - increments the usage counter of the item and returns the item
 */

ITEM
i_inc(item)
	register ITEM item;
	{
	if (!item) return((ITEM)NULL);
	else if (item->usage >= 65535)
		d_error("i_inc - usage counter overflow");
	else item->usage++;
	return(item);
}
/* ####################################################################### 
 *
 * i_dec/1 - decrements the usage counter of the item and returns the item
 */

ITEM
i_dec(item)
	register ITEM item;
	{
	if (!item) return((ITEM)NULL);
	else if (item->usage == 0)
		d_error("i_dec - usage counter underflow");
	else item->usage--;
	return(item);
}

#define ADDR	1

ITEM
i_create(itype,val) 
	char itype;
	POINTER val;
	{
	ITEM item;
	LIST *hlast;
#ifdef MEMCHECK
	ITEM *hplace;
	LIST lptr;
#endif
        if (!(item = (ITEM)PROGOL_CALLOC(1l, sizeof(struct item))))
		d_error("i_create - calloc failure");
	else {
		item->item_type=itype;
		item->usage=1;
		item->obj=val;
		item->extra=UNASS;
#ifdef MEMCHECK
		hplace= &(((FUNC)I_GET(all_items))->arr[h_add_hfn(item,all_items)]);
		if (!(*hplace)) {
		  if (!(*hplace = (ITEM)PROGOL_CALLOC(1l, sizeof(struct item))))
				d_error("i_create - calloc failure");
		  (*hplace)->item_type= 'l';
		  (*hplace)->usage=1;
		  (*hplace)->obj=(POINTER)NULL;
		  hlast=L_LAST(*hplace);
		}
		else
		    LISTP_LOOP(hlast,*hplace)
			if (item == L_GET(*hlast)) break;
		if (!(lptr = (LIST)PROGOL_CALLOC(1l, sizeof(struct lmem))))
			d_error("i_create - calloc failure 2");
		lptr->pres= item;
		lptr->next= *hlast;
		*hlast= lptr;
#endif
	}
	return(item);
}

#ifdef MEMCHECK
int
i_print_all()
	{
	register ITEM *felem;
	FUNC f= (FUNC)I_GET(all_items);
	LIST elem;
	
	printf("All items:\n");
	FUNC_LOOP(felem,f) {
	    if (*felem) {
		LIST_LOOP(elem,(LIST)I_GET(*felem)) {
		    printf("\t");
		    printf("0x%x ",L_GET(elem));
		    i_fwrite(ttyout,L_GET(elem));
		    printf("\n");
		}
	    }
	}
}
#endif

#define		NARGS	10

/*******************************************************************************
 * i_deletes - deletes a list of up to 9 items. Care should be taken
 *	not to give it more than 9 items at once (error given). The last item must
 *	always be terminated by (ITEM)I_TERM as the last item.
 */

int
i_deletes(f0,f1,f2,f3,f4,f5,f6,f7,f8,f9)
	ITEM f0,f1,f2,f3,f4,f5,f6,f7,f8,f9;
	{
	ITEM fargs[NARGS],*fptr=fargs;

	*fptr++ = f0; *fptr++ = f1; *fptr++ = f2; *fptr++ = f3; *fptr++ = f4;
	*fptr++ = f5; *fptr++ = f6; *fptr++ = f7; *fptr++ = f8; *fptr = f9;
	for(fptr=fargs;*fptr != (ITEM)I_TERM && ((fptr-fargs) < NARGS);fptr++)
		i_delete(*fptr);
	if ((fptr-fargs) == NARGS)
		d_error("i_deletes - no terminator or too many arguments");
}

/********************************************************************************
 * i_delete - recursively deletes an item of any type.
 */


/* ####################################################################### 
 *
 * put1tstring/1 prints the next character to the global string pointer 
 *	it is used by i_write when printing to strings (as opposed to a file).
 */

STRING glob_str;

int
put1tstring(c)
	{
	*glob_str++ = c;
}

int
i_swrite(s,i)
	STRING s;
	ITEM i;
	{
	glob_str = s;
	i_write(i,put1tstring,FALSE);
	put1tstring('\0');
}

FILEREC *glob_file;

/* ####################################################################### 
 *
 * put1tfile/1 prints the next character to the global output file pointer 
 *	it is used by i_write when printing file output (as opposed to strings).
 */

LONG line_cnt;



int
frecflush(f)
	FILEREC *f;
	{
	register LONG n = (f->bufp - (f->buf+LBUF));
	if (fwrite((f->buf+LBUF),n,1,f->file) != 1)
		printf("frecflush - bad fwrite");
	f->bufp = (f->buf+LBUF);
}



int
i_fwrite(out,i)
	FILEREC *out;
	ITEM i;
	{
	line_cnt=0l;
	instring=FALSE;
	glob_file=out;
	i_write(i,put1tfile,FALSE);
	frecflush(glob_file);
}

int
i_fewrite(out,i)
	FILEREC *out;
	ITEM i;
	{
	line_cnt=0l;
	instring=FALSE;
	glob_file=out;
	i_write(i,put1tfile,TRUE);
	frecflush(glob_file);
}

int
i_print(i)
	ITEM i;
	{
	i_fwrite(ttyout,i); printf("\n");
}

int
i_eprint(i)
	ITEM i;
	{
	i_fewrite(ttyout,i); printf("\n");
}

int
i_fnl(out)
	FILEREC *out;
	{
	glob_file=out;
	put1tfile('\n');
	frecflush(glob_file);
}

int
i_ftab(out)
	FILEREC *out;
	{
	glob_file=out;
	put1tfile('\t');
	frecflush(glob_file);
}

int
i_fpr(out,s)
	FILEREC *out;
	STRING s;
	{
	register char *c;
	glob_file=out;
	STR_LOOP(c,s) put1tfile(*c);
	frecflush(glob_file);
}

/* ####################################################################### 
 *
 * get1fstring/0 gets the next character from the global string buffer 
 *	it is used by i_read when parsing strings (as opposed to a file).
 */

char
get1fstring()
	{
	return(*glob_str++);
}

int
unget1fstring(c)
	register char c;		/* Argument not actually used */
	{
	glob_str--;
}

/* ####################################################################### 
 *
 * get1ffile/0 gets the next character from the global input file pointer 
 *	it is used by i_read when parsing file input (as opposed to strings).
 */

char
get1ffile1()
	{
	register LONG n;
	if (glob_file->bufp < glob_file->buflim)
		return(*(glob_file->bufp)++);
	else if ((n=fread(glob_file->bufp=glob_file->buf+LBUF,1,
				BUFSIZE,glob_file->file)) > 0) {
		glob_file->buflim=glob_file->bufp+n;
		return(*(glob_file->bufp)++);
	}
	else if (!n) return('\0');
	else d_error("get1ffile - negative value returned by read");
}

char
get1ffile()
	{
	char c;
	if ((c=get1ffile1()) == '\n') (glob_file->line_no)++;
	return(c);
}

LONG
unget1ffile(c)
	register char c;
	{
	if ((*(--(glob_file->bufp))=c) == '\n') (glob_file->line_no)--;
}

char
get1ftty()
	{
	register char c;
	return(((c=getc(stdin)) == (char)EOF) ? '\0' : c);
}

LONG
unget1ftty(c)
	register char c;
	{
	ungetc(c,stdin);
}

FILEREC *
frecopen(name,mode)
	STRING name,mode;
	{
	FILEREC *result;
	FILE *f=fopen(name,mode);
	if (!f) return((FILEREC *)NULL);
	result=frecreate(name);
	result->file=f;
	return(result);
}

int
freclose(f)
	FILEREC *f;
	{
	fclose(f->file);
	frecdelete(f);
}

FILEREC *
frecreate(name)
	STRING name;
	{
	FILEREC *result;
        if (!(result = (FILEREC *)PROGOL_CALLOC(1l, sizeof(FILEREC)))) {
		d_error("frecopen - calloc failure");
	}
	result->bufp=(result->buflim=result->buf+LBUF);
	result->line_no=1;
	result->filename=strsave(name);
	return(result);
}

int
frecdelete(f)
	FILEREC *f;
	{
	S_DEL(f->filename);
	PROGOL_CFREE((POINTER)f,sizeof(FILEREC));
}

/*******************************************************************************
 * i_swap/2 - swaps the complete contents of i1 and i2.
 *	Returns first argument.
 */

ITEM
i_swap(i1,i2)
	ITEM i1,i2;
	{
	struct item swapper;
	swapper.item_type=i1->item_type;
	swapper.usage=i1->usage;
	swapper.obj=i1->obj;
	swapper.extra=i1->extra;
	i1->item_type=i2->item_type;
	i1->usage=i2->usage;
	i1->obj=i2->obj;
	i1->extra=i2->extra;
	i2->item_type=swapper.item_type;
	i2->usage=swapper.usage;
	i2->obj=swapper.obj;
	i2->extra=swapper.extra;
	return(i1);
}

/**************************************************************************
 * r_create/1 - returns a pointer to enough space for a floating point
 *	number.
 */

double *
r_create(real)
	double real;
	{
	double *result;
        if (!(result = (double *)PROGOL_CALLOC(1l, sizeof(double)))) {
		d_error("r_create - calloc failure");
	}
	*result= real;
	return(result);
}

/**************************************************************************
 * r_get/1 - dereferences real numbered item and returns as integer
 */

LONG
r_get(i)
	ITEM i;
	{
	register double *rp=(double *)I_GET(i),r= *rp;
	return((LONG)r);
}

/**************************************************************************
 * r_intof/1 - returns integer part of double
 */

LONG
r_intof(r)
	double r;
	{
	register LONG result=r;
	return(r);
}


/*
 * i_tup2/2 - puts pair of items in a FUNC
 */

ITEM
i_tup2(i1,i2)
	ITEM i1,i2;
	{
	FUNC f;
	ITEM result=i_create('f',f=f_create(2l));
	f->arr[0l]=i_inc(i1);
	f->arr[1l]=i_inc(i2);
	return(result);
}

/*
 * i_tup3/3 - puts triple of items in a FUNC
 */

ITEM
i_tup3(i1,i2,i3)
	ITEM i1,i2,i3;
	{
	FUNC f;
	ITEM result=i_create('f',f=f_create(3l));
	f->arr[0l]=i_inc(i1);
	f->arr[1l]=i_inc(i2);
	f->arr[2l]=i_inc(i3);
	return(result);
}

/*
 * i_tup4/4 - puts quadruple of items in a FUNC
 */

ITEM
i_tup4(i1,i2,i3,i4)
	ITEM i1,i2,i3,i4;
	{
	FUNC f;
	ITEM result=i_create('f',f=f_create(4l));
	f->arr[0l]=i_inc(i1);
	f->arr[1l]=i_inc(i2);
	f->arr[2l]=i_inc(i3);
	f->arr[3l]=i_inc(i4);
	return(result);
}
