/* ####################################################################### */

/*              PROGOL include File for Structures and #defines             */
/*              --------------------------------------------               */

/* ####################################################################### */

typedef	int BOOL, INT;
typedef long int LONG;
typedef	double DOUBLE;
typedef char *STRING;
typedef char *POINTER;
typedef struct item *ITEM;
typedef struct lmem *LIST;
typedef unsigned long int ULONG;
typedef unsigned long int *BLOCK;
typedef struct functor *FUNC;
typedef struct filerec FILEREC;
typedef long int PREDICATE;

#include        "ext_fn.h"

/*
#define		MEMCHECK
#define		UNBUFFERED
#define		CHECK_SECURITY
 */
#define		SUNCHECK

extern FILEREC *glob_file;	/* I/O declarations */
extern STRING glob_str;
extern PREDICATE instring;
extern LONG line_cnt;
extern char get1ffile();
extern LONG unget1ffile();
extern char get1ftty();
extern LONG unget1ftty();
extern char get1fstring();
extern unget1fstring();
extern void put1tfile();
extern int put1tstring();

/*
extern long rand();
extern void srand();
 */
extern double drand48();

#define		RAND		(drand48())
#define		SRAND(x)	(srand48(x))
#define		MYRAND(lo,hi) ((rand()%(((hi)-(lo))+1l))+(lo))

#define		PROGOL_CALLOC(a,b)	a_dalloc(a,b)
#define		PROGOL_CFREE(a,b)	a_dfree(a,b)

#define         TRUE            1
#define         FALSE           0
#define		BOOLEAN		0
#define		NATURAL		1
#define		REAL		2
#define		GT		1
#define		EQ		0
#define		LT		-1
#define		MAXN		500l
#define         MAXMESS         10000l
#define         MAXWORD         200l
#define		BUFSIZE		512l
#define		LBUF		4l
#define		XYTERM		0xffffffff
#define		I_TERM		-1l
#define		I_ERROR		-2l
#define		HGEN		0l
#define		HASH10		1023l
#define		INF		9999l
#define		RINF		9999.9
#define		SEP		0l
#define		ALPHN		1l
#define		SYM		2l
#define		OPTR		2l
#define		OPND		3l
#define		ABS		1l
#define		INTRA		2l
#define		IN		-1
#define		UNASS		0xffffffffl
#define		NGRND		0xffffl
#define		SKOL		1l
#define		OUT		2l
#define		CONST		3l
#define		SAMPLE		100l
/* Progol learning stages */
#define		NOLEARN		0l
#define		SATURATE	1l
#define		REFINE		2l

#define		H_VAL(s)	((double)R_ELEM(0l,s))
#define		F_VAL(s)	(-((double)R_ELEM(1l,s)))
#define		P_VAL(s)	(-((double)R_ELEM(2l,s)))
#define		N_VAL(s)	((double)R_ELEM(3l,s))
#define		C_VAL(s)	((double)R_ELEM(4l,s))
#define		R_CMP0(s)	(P_VAL(s)-(C_VAL(s)+H_VAL(s)))
#define		R_VAL(s)	(F_VAL(s)/(R_CMP0(s)-N_VAL(s)))
#define		F_VAL0(s)	(F_VAL(s)/R_VAL(s))
#define		A_CODE(s)	R_ELEM(5l,s)
#define		A_ICODE(s)	r_intof(A_CODE(s))
#define		R_PARENT(s)	R_ELEM(6l,s)
#define		R_IPARENT(s)	r_intof(R_PARENT(s))
#define		R_CMP(s)	(R_VAL(s)*R_CMP0(s))
#define		R_PASS(s)	(100.0*N_VAL(s)<=noiselim*(P_VAL(s)+N_VAL(s))&&F_VAL(s)>=0.0&&H_VAL(s)==0.0)
#define R_PRUNE(s)	(R_IPARENT(s)&&((N_VAL(s)==0.0&&H_VAL(s)==0.0)||R_CMP(s)<0.0||(C_VAL(s)-1.0+H_VAL(s))>clim))

/* Skolemised terms have an extra field with the highest bit unset */

#define		IS(x,y)		((1 << (INT_SZ-(x))) & ~(y))
#define		ON(x,y)		((y) &= ~(1 << (INT_SZ-(x))))
#define		OFF(x,y)	((y) |= (1 << (INT_SZ-(x))))

#define		PADDING(c)	(c == ' ' || c == '\n' || c == '\t')
#define		DIGIT(c)	(c >= '0' && c <= '9')
#define		LOWCASE(c)	((c) >= 'a' && (c) <= 'z')
#define		UPCASE(c)	((c) >= 'A' && (c) <= 'Z')
#define		ALPHA(c)	(LOWCASE(c) || UPCASE(c))
#define		ALPHANUM(c)	(ALPHA(c) || DIGIT(c) || \
					(c == '_'))
#define		GROUNDQ(ccl)	((LONG)I_GET(F_ELEM(1l,(ccl)))==0l)
#define		MAX(x,y)	if((y)>(x)) (x)=(y)
#define		MIN(x,y)	if((y)<(x)) (x)=(y)
#define		HEX(c)		(DIGIT(c) || (c >= 'A' && c <= 'Z'))
#define		LOG2(n)		((n)&0xffff0000l?((n)&0xff000000l?llog2[(n)>>24l]+24l:llog2[(n)>>16l]+16l):((n)&0xff00l?llog2[(n)>>8l]+8l:llog2[n]))
#define		B_SIZE(b)	(*(b))
#define		F_ELEM(n,i)	(((FUNC)I_GET(i))->arr[n])
#define		F_SIZE(i)	(((FUNC)I_GET(i))->arr_size)
#define		FNAME(f)	((f)->arr[0])
#define		S_DEL(s)	PROGOL_CFREE((s),strlen(s)+1)
#define		L_DEL(l)	PROGOL_CFREE((POINTER)(l),sizeof(struct lmem))
#define		L_REM(lp)	{register LIST l= *(lp); *(lp)=l->next; \
				i_delete(L_GET(l)); L_DEL(l);}
#define         STREQ(s1,s2)    (!strcmp(s1,s2))
#define		STR_LOOP(sp,s)	for((sp) = (s) ; *(sp) ; (sp)++)
#define		STRSUFQ(s1,s2) (STREQ((s1)+strlen(s1)-strlen(s2),(s2)))
#define		NUM_LOOP(n,s,i,f)	for((n) = (s); (n) != ((f) + (i)); (n) += (i))
#define		TERM_LOOP(e,n,p)	for((e)=(p),(n)=1;(n)>0;(n)+=((*(e)>0)?*(e)-1:-1),(e)+=((*(e)>= -2)?2:1))
#define		TERM_LOOP2(e1,n1,e2,n2,p1,p2)	for(e1=(p1),e2=(p2),n1=1,n2=1;(n1)>0 && (n2)>0;(n1)+=((*(e1)>0)? *(e1)-1:-1),(n2)+=((*(e2)>0)? *(e2)-1:-1),(e1)+=((*(e1)>= -2)?2:1),(e2)+=((*(e2)>= -2)?2:1))
#define		ARG_LOOP(ip,f)	for(ip=((f)->arr)+1;ip<(f)->arr+(f)->arr_size;ip++)
#define		ARG_LOOP2(e1,e2,f1,f2)	for(e1=((f1)->arr)+1,e2=((f2)->arr)+1;(e1<((f1)->arr + (f1)->arr_size)) && (e2 < ((f2)->arr + (f2)->arr_size));e1++,e2++)
#define		ARG_LOOP3(e1,e2,e3,f1,f2,f3)	for(e1=((f1)->arr)+1,e2=((f2)->arr)+1,e3=((f3)->arr)+1;(e1<((f1)->arr+(f1)->arr_size))&&(e2<((f2)->arr+(f2)->arr_size))&&(e3<((f3)->arr+(f3)->arr_size));e1++,e2++,e3++)
#define		ARG_LOOP4(e1,e2,e3,e4,f1,f2,f3,f4)	for(e1=((f1)->arr)+1,e2=((f2)->arr)+1,e3=((f3)->arr)+1,e4=((f4)->arr)+1;(e1<((f1)->arr+(f1)->arr_size))&&(e2<((f2)->arr+(f2)->arr_size))&&(e3<((f3)->arr+(f3)->arr_size))&&(e4<((f4)->arr+(f4)->arr_size));e1++,e2++,e3++,e4++)
#define		Y_DO(e,y) {register ULONG zf; for((e)=((ULONG *)I_GET(y))+1l,zf= *((e)++);zf--;(e)++){
#define		Y_END }}
#define		R_DO(e,y) {register ULONG zf; for((e)=I_FP(y)+1l,zf= *((e)++);zf--;(e)++){
#define		R_END }}
#define		BLOCK_LOOP(e,f,b) for((e)=(b)+1,(f)=(b)+B_SIZE(b);(e) <= (f);(e)++)
#define		BLOCK_LOOP2(e1,e2,f1,f2,b1,b2) for((e1)=(b1)+1,(e2)=(b2)+1,(f1)=(b1)+B_SIZE(b1),(f2)=(b2)+B_SIZE(b2);(e1)<=(f1)&&(e2)<=(f2);(e1)++,(e2)++)
#define		BIT_LOOP(n,e,f,b,v,c,p) BLOCK_LOOP(e,f,b)if(!((v)= *(e))){(n)+=INT_SZ;continue;}else for((c)=0;(c)<INT_SZ;(c)+=BYTE_SZ,(v)>>=BYTE_SZ,(n)+= *(++(p)))for((p)=bitspos[(v)&0xff];*(p)!= -1 &&(((n)+= *(p)++)+1);)
#define		BIT_DO(n,bs) {register BLOCK zb=(BLOCK)I_GET(bs),zbp,zbe;register LONG zv,zc,*zp;n=0l;BIT_LOOP(n,zbp,zbe,zb,zv,zc,zp){
#define		BIT_END }}
#define		BIT_NEXT(n,bs) {register BLOCK zb=(BLOCK)I_GET(bs),ze,zf; \
		  register ULONG zw;register LONG zc,*zp;register PREDICATE \
		  zfound=FALSE;if((n)==UNASS){(n)=0l;zw= *(ze=zb+1l);} else \
		  if(!(~(n)&0x1f)){zw=(*(ze=zb+((n)>>LOG_INT_SZ)+2l));n++;} \
		  else {zw=(*(ze=zb+((n)>>LOG_INT_SZ)+1l))&(~((1l<<(((n)&0x1f) \
		  +1l))-1l));n&=0xffffffe0;}for(zf=zb+B_SIZE(zb);ze<=zf;zw= \
		  *(++ze)) {if(!zw){(n)+=INT_SZ;continue;}else for((zc)=0l; \
		  (zc)<INT_SZ;(zc)+=BYTE_SZ,(zw)>>=BYTE_SZ,(n)+= *(++(zp))){ \
	          for((zp)=bitspos[(zw)&0xff];*(zp)!= -1l&&(((n)+= *(zp)++)+ \
		  1l);){zfound=TRUE;break;}if(zfound)break;}if(zfound)break; \
		  }if(!zfound)(n)=UNASS;}
#define		FUNC_LOOP(ip,f)	for(ip=(f)->arr;ip<(f)->arr+(f)->arr_size;ip++)
#define		FUNC_LOOP2(e1,e2,f1,f2)	for(e1=(f1)->arr,e2=(f2)->arr;(e1<((f1)->arr + (f1)->arr_size)) && (e2 < ((f2)->arr + (f2)->arr_size));e1++,e2++)
#define         LIST_LOOP(e,l) for(e=l;e;e=e->next)
#define         LIST_LOOP2(e1,e2,l1,l2)  for(e1=l1,e2=l2; e1 && e2; e1=e1->next,e2=e2->next)
#define         LIST_LOOP3(e1,e2,e3,l1,l2,l3)  for(e1=l1,e2=l2,e3=l3; e1 && e2 && e3; e1=e1->next,e2=e2->next,e3=e3->next)
#define		PENL_LOOP(e, l) for(e = l ; e->next ; e = e->next)
#define		ARG_DO(e,i) {register FUNC zf=(FUNC)I_GET(i);ARG_LOOP(e,zf) {
#define		ARG_DO2(e1,e2,i1,i2) {register FUNC zf1=(FUNC)I_GET(i1),zf2=(FUNC)I_GET(i2);ARG_LOOP2(e1,e2,zf1,zf2) {
#define		ARG_DO3(e1,e2,e3,i1,i2,i3) {register FUNC zf1=(FUNC)I_GET(i1),zf2=(FUNC)I_GET(i2),zf3=(FUNC)I_GET(i3);ARG_LOOP3(e1,e2,e3,zf1,zf2,zf3) {
#define		ARG_END }}
#define		FUNC_DO(e,i) {register FUNC zf=(FUNC)I_GET(i);FUNC_LOOP(e,zf) {
#define		FUNC_END }}
#define		LIST_DO(i,l) {register LIST ze;LIST_LOOP(ze,(LIST)I_GET(l)){i=L_GET(ze);
#define		LIST_END }}
#define		AR_LOOP(p,e,a)	for((p)=(a)+1,(e)=(a)+ *(a);(p)<=(e);(p)++)
#define		L_GET(l)       ((l)->pres)
#define		I_GET(i)       ((i)->obj)
#define		I_FP(i)		(double *)(I_GET(i))
#define		I_FASS(i,f)	{double *fp=I_FP(i); *fp=(f);}
#define         I_ASSIGN(i,val)       (((i)->obj)=(POINTER)val)
#define		LISTP_LOOP(lp,l)	for(lp= ((LIST *)&((l)->obj));*(lp);lp= &((*(lp))->next))
#define		SUFLIST(l)	{LIST *lp; LISTP_LOOP(lp,l); (l)->extra=(LONG)lp;}
#define		ITMEQ(i1,i2)	(!i_cmp(i1,i2))
#define		L_TERM(lp,lpp)	(lpp=l_end(i_inc(L_GET(lp)),lpp))
#define		HOF(l)		((l)->pres)
#define		TOF(l)		((l)->next)
#define		I_NINC(i)	(((LONG)I_GET(i))++)
#define		L_LAST(l)	((LIST *)&(I_GET(l)))
#define		L_EMPTY		(l_empty())
#define		F_EMPTY		(h_create(3l))
#define		B_EMPTY		(i_create('b',(POINTER)b_create(1l)))
#define		Y_EMPTY		(i_create('I',(POINTER)y_create(2l)))
#define		Y_EMPTYQ(y)	(!(*(((ULONG *)I_GET(y))+1l)))
#define		Y_SIZE(y)	(*(((ULONG *)I_GET(y))+1l))
#define		Y_PUSH(v,y)	(y_push(v,(ULONG **)&I_GET(y)))
#define		Y_POP(y)	(y_pop((ULONG *)I_GET(y)))
#define		Y_TOP(y)	(*(((ULONG *)I_GET(y))+(*(((ULONG *)I_GET(y))+1l))+1l))
#define		Y_ELEM(e,y)	(*(((ULONG *)I_GET(y))+(e)+2l))
#define		R_EMPTY		(i_create('R',(POINTER)R_create(2l)))
#define		R_EMPTYQ(y)	(!(*(((ULONG *)I_GET(y))+1l)))
#define		R_SIZE(y)	(*(((ULONG *)I_GET(y))+1l))
#define		R_PUSH(v,y)	(R_push(v,(double **)&I_GET(y)))
#define		R_POP(y)	(R_pop((double *)I_GET(y)))
#define		R_TOP(y)	R_top((double *)I_GET(y))
#define		R_ELEM(e,y)	(*(((double *)I_GET(y))+(e)+2l))
#define		I_INT(n)	(i_create('i',(POINTER)(n)))
#define		I_INC(i)	((i)->usage)++
#define		I_DEC(i)	((i)->usage)--
#define		L_EMPTYQ(l)	((LIST)I_GET(l) == (LIST)NULL)
#define		FSYM(a)		((LONG)I_GET(((FUNC)I_GET(a))->arr[0]))
#define		i_sort(l)	i_sortc((l),i_cmp)
#define		i_psort(l)	{namecmp=TRUE;i_sortc((l),i_cmp);namecmp=FALSE;}
#define		ct_vars(t)	ct_terms((t),TRUE,FALSE)
#define		HXY(i,j)	((((i)&0x1f)<<5)|((j)&0x1f))
#define		GETXY(ip)	(*(++(ip)))
#define		FNDXY(x,y,xy)	for((xy)= *(hxy+HXY(x,y))+1;*(xy)!=XY(x,y);(xy)+=2)
#define		PSYM(a)		((a)->item_type== 'f'?((LONG)I_GET(F_ELEM(0l,a))):(((a)->item_type== 'r')?(r_get(a)):((LONG)I_GET(a))))
#define		TERM_DO(e,t)	{register ITEM zts=Y_EMPTY,zas=Y_EMPTY;\
				 PREDICATE znew=TRUE;Y_PUSH((LONG)(t),zts);\
				 while(!Y_EMPTYQ(zts)){if(znew){\
				 (e)=(ITEM)Y_TOP(zts);
#define		TERM_END	 znew=FALSE;if(((ITEM)Y_TOP(zts))->item_type\
				 == 'f')Y_PUSH(0l,zas);else Y_POP(zts);}\
			         if(!Y_EMPTYQ(zas)){\
				 if(++Y_TOP(zas)<F_SIZE((ITEM)Y_TOP(zts))){ \
				 Y_PUSH(F_ELEM(Y_TOP(zas),((ITEM)Y_TOP(zts))),zts); \
				 znew=TRUE;}else{Y_POP(zts);Y_POP(zas);}}}\
				 i_deletes(zts,zas,(ITEM)I_TERM);}
#define		PLIST_DO(e,zl,l) {for(zl=(l);\
			zl->item_type=='f'&&PSYM(zl)==pdot2;\
			zl=F_ELEM(2l,zl)){(e)=F_ELEM(1l,zl);
#define		PCLIST_DO(e,zl,l) {for(zl=(l);\
			zl->item_type=='f'&&PSYM(zl)==pcomma2;\
			zl=F_ELEM(2l,zl)){(e)=F_ELEM(1l,zl);
#define		PLIST_END }}
#define		PCLIST_END }}
#define		CTYPE(c)	(I_GET(F_ELEM(2l,(c))))
#define		ALIGN8(p)	(p=(char *)(((ULONG)p+4l)&0xfffffff8))
#define		CCHEAD(ccl)	(HOF((LIST)I_GET(F_ELEM(0l,(ccl)))))
#define		FIRSTARG(a)	((a)->item_type== 'f'?F_ELEM(1l,a):(ITEM)NULL)
#define		LABEL(c)	(*y_ins((c),labels)+1l)
#define		SET(f)		((f)=TRUE)
#define		UNSET(f)	((f)=FALSE)
#define		SETQ(f)		(f)
#define		ATMP(call)	(&(F_ELEM(1l,HOF(TOF((LIST)I_GET(F_ELEM(0l,call)))))))
#define		ANC(p)		b_add((p),c_cpclosure((p),bppar))
#define		DES(p)		b_add((p),c_cpclosure((p),bpchd))
#define		GRNDQ(x)        (!((LONG)I_GET(F_ELEM(1l,(x)))))

/*
 * #######################################################################
 */

struct item {
	char item_type;
					/*		atom ('a') */
					/*		bitset ('b') */
					/*		functor ('f') */
					/*		hashed string ('h') */
					/*		  (Prolog symbol) */
					/*		integer ('i') */
					/*		integer array ('I') */
					/*		skolem constant ('k') */
					/*		list ('l') */
					/*		real ('r') */
					/*		real array ('R') */
					/* 		string ('s') */
					/*		prolog variable ('v') */
	unsigned short int usage;	/* Usage count */
	POINTER obj;
	ULONG extra;	/* Used for storing path/term info */
};

struct lmem {			/* List member */
        ITEM pres;		/* Present item */
        LIST next;		/* Next in the list */
};

#define		BLCK_SZ 4l	/* 4 integers in a bit array block */
#define		INT_SZ 32l	/* VAX has 32 bits in an unsigned long int */
#define		LOG_INT_SZ 5l	/* VAX has 32 bits in an unsigned long int */
#define		NBBL_SZ	4l	/* 4 bits in a nibble */
#define		BYTE_SZ	8l	/* 4 bits in a nibble */
#define		BYTE_RNG 256l

struct functor {
	LONG arr_size;
	ITEM *arr;
};

struct hashfns {
	PREDICATE (*eq) ();
	LONG (*hfn) ();
};

struct filerec {
	STRING filename;
	FILE *file;
	char buf[BUFSIZE+LBUF];
	STRING bufp;
	STRING buflim;
	LONG line_no;
};

struct otree {
	LONG val;
	LONG nleft;
	struct otree *left;
	struct otree *right;
};

struct setrec {
	STRING setname;
	ULONG settype;
	POINTER setptr;
};

#include	"interp.h"
#include        "ext_var.h"
#include	<math.h>
#include        <sys/time.h>
#include        <sys/resource.h>
#include	<unistd.h>
#include	<time.h>
