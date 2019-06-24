/* ####################################################################### */

/*                      PROGOL random sampling				   */
/*                      ---------------------				   */

/* ####################################################################### */

#include        <stdio.h>
#include	"progol.h"


/*
 * hibit/1 - finds the highest bit set in an integer by binary chop.
 *	Fails when given 0.
 */

LONG
hibit(n)
	register unsigned long int n;
	{
	register long int hi=INT_SZ-1,lo=0,mid;
	if (!n) d_error("hibit - given 0 as input");
	while (hi != lo) {
		mid=((hi-lo)>>1)+lo;
		if (!(n>>(mid+1))) hi=mid;
		else lo=mid+1;
	}
	return(hi);
}

/*
 * b_smpair/3 - efficient method for picking a random pair <x,y>
 *	where both x and y are in the range 0 to sz-1 and
 *	x<y.
 */

b_smpair(sz,x,y)
	LONG sz,*x,*y;
	{
	LONG val,swap;
	if(sz<2) d_error("b_smpair - set size < 2");
	val=MYRAND(0,sz*(sz-1));
	if((*x=val/sz)>=(*y=val%sz)) {
	  swap= *y;
	  *y= *x+1;
	  *x= swap;
	}
}

/* isqrt(x) - returns the largest integer less than the sqrt of x.
 *	Successive approximation calculated for y^2=x as follows
 *		yi+1 = yi+((x/yi)-yi)/2 = (yi+(x/yi))/2
 *	  where y0 = 2^(log2(x)/2)
 *	Converges in time O(log x)
 */

LONG
isqrt(sq)
	LONG sq;
	{
	LONG oldi,newi;
	double oldf,newf,sqf=sq;
	if(sq==0) return(0);
	else if(sq<0) d_error("isqrt - negative argument");
	newi=1<<(LOG2(sq)>>1); newf=newi;
	do {
	  oldf=newf;
	  newf=(oldf+sqf/oldf)/2;
	  oldi=oldf; newi=newf;
	} while(oldi!=newi);
	return(newi);
}

/*
 * prencode/3 - given 0<=x<y<n this function encodes the
 *	pair <x,y> to a unique number in the range [1,n*(n-1)/2]
 *	The result k= x((2n-3)-x)/2 + y
 */

LONG
prencode(x,y,n)
	LONG x,y,n;
	{
	if(n>30000) d_error("prencode - set too large");
	else if(x<0||y<=x||n<=y) d_error("prencode - bad values");
	return(((x*((n<<1)-3-x))>>1)+y);
}

/*
 * prdecode/4 - given k=prencode(x,y,n) this function decodes
 *	x and y from k and n. Decoding is done by binary
 *	chop. Upper and lower bounds are computed using
 *	(2n-3-sqrt(abs((2n-3)^2-8k+8(n-1))))/2 <=x<=
 *		(2n-3-sqrt(abs((2n-3)^2-8k+8)))/2. y is
 *	computed using y=(x(x-(2n-3))+2k)/2
 */

prdecode(k,n,x,y)
	LONG k,n,*x,*y;
	{
	LONG lo,hi,mid,n2=(n<<1)-3,k2=k<<1,sq=(n2*n2)-(k<<3);
	if(n>30000) d_error("prdecode - set too large");
	else if(k<=0||k>(n*(n-1))>>1) d_error("prdecode - bad values");
	lo=(n2-isqrt(sq+((n-1)<<3)))>>1;
	hi=sq+8>0?(n2-isqrt(sq+8))>>1:n2>>1;
	do {
		mid=((hi-lo)>>1)+lo;
		*y=(mid*(mid-n2)+k2)>>1;
		if(*y<=mid) hi=mid;
		else if(*y<n) {lo=mid;break;}
		else lo=mid+1;
	} while (hi>lo);
	*x=lo;
	*y=(lo*(lo-n2)+k2)>>1;
}

struct otree *
ot_create(v,nl)
	LONG v,nl;
	{
	struct otree *result;
	if (!(result = (struct otree *)PROGOL_CALLOC(1, sizeof(struct otree))))
		d_error("ot_create - calloc failure");
	result->val=v;
	result->nleft=nl;
	result->left=(struct otree *)NULL;
	result->right=(struct otree *)NULL;
	return(result);
}

void ot_delete(o)
	struct otree *o;
	{
	if(!o) return;
	ot_delete(o->left);
	ot_delete(o->right);
	PROGOL_CFREE(o,sizeof(struct otree));
}

struct otree *
ot_ins(v,lo,tree)
	LONG v,lo;
	struct otree *tree;
	{
	struct otree **ptree= &tree;
	while(*ptree)
	    if(v<= (*ptree)->nleft) {(*ptree)->nleft--;ptree= &((*ptree)->left);}
	    else {v-=(*ptree)->nleft;lo=(*ptree)->val;ptree= &((*ptree)->right);}
	*ptree=ot_create(v+lo,v-1);
	return(tree);
}

ot_print(indent,tree)
	LONG indent;
	struct otree *tree;
	{
	LONG cnt;
	if(tree) {
		ot_print(indent+1,tree->left);
		for(cnt=indent;cnt>0;cnt--) printf("    ");
		printf("<%d,%d>\n",tree->val,tree->nleft);
		ot_print(indent+1,tree->right);
	}
	else {
		for(cnt=indent;cnt>0;cnt--) printf("    ");
		printf("_\n");
	}
}

/*
 * ot_sample(m,n,tree) - randomly choose m different pairs from
 *	a set size n and insert into tree. Return tree.
 */

struct otree *
ot_sample(m,n)
	LONG m,n;
	{
	struct otree *tree=(struct otree *)NULL;
	if(m>n) m=n;
	for(;m>0;m--,n--) tree=ot_ins(MYRAND(1,n),0,tree);
	return(tree);
}

ot_prprint(tree,n)
	struct otree *tree;
	LONG n;
	{
	LONG x,y;
	if(tree) {
		ot_prprint(tree->left,n);
		prdecode(tree->val,n,&x,&y);
		printf("<%d,%d>\n",x,y);
		ot_prprint(tree->right,n);
	}
}

/*
 * b_bitsum/2 - returns an array giving for each word in the bitset the sum of the
 *	number of bits up to that word.
 */

BLOCK
b_bitsum(b1,total)
	BLOCK b1;
	LONG *total;
	{
	register LONG cnt,sum=0l;
	register unsigned long int val;
	register BLOCK bp1,be1,b2=b_create(B_SIZE(b1)),bp2,be2;
	BLOCK_LOOP2(bp1,bp2,be1,be2,b1,b2) {
	  *bp2=sum;
	  if (val= *bp1)
	    for (cnt=0l;cnt<INT_SZ;cnt+=BYTE_SZ,val>>=BYTE_SZ)
	      sum += byte_sz[val&0xff];
	}
	*total=sum;
	return(b2);
}

/*
 * b_samprem/4 - carries out a binary split search for the
 *	integer containing the given element and returns
 *	the bit number.
 */

LONG w_findnth();

LONG
b_findnth(elem,b,bitsum)
	LONG elem;
	BLOCK b,bitsum;
	{
	register LONG lo,hi,mid;
	hi=B_SIZE(b)-1; b++; bitsum++;
	for(lo=0l;lo<hi;) {
	  mid=lo+(((hi-lo)+1)>>1);
	  if (elem<=bitsum[mid]) hi=mid-1;
	  else lo=mid;
	}
	return((lo<<LOG_INT_SZ)+w_findnth(elem-bitsum[lo],b[lo]));
}

/*
 * bitsinc is a table indicating the position of bits that are set
 *	in a byte. The nth bit which is set in i is bit number
 *	bitsinc[i][n-1].
 */

LONG bitsinc[BYTE_RNG] [BYTE_SZ] =
{{8,8,8,8,8,8,8,8},{0,8,8,8,8,8,8,8},{1,8,8,8,8,8,8,8},{0,1,8,8,8,8,8,8},
 {2,8,8,8,8,8,8,8},{0,2,8,8,8,8,8,8},{1,2,8,8,8,8,8,8},{0,1,2,8,8,8,8,8},
 {3,8,8,8,8,8,8,8},{0,3,8,8,8,8,8,8},{1,3,8,8,8,8,8,8},{0,1,3,8,8,8,8,8},
 {2,3,8,8,8,8,8,8},{0,2,3,8,8,8,8,8},{1,2,3,8,8,8,8,8},{0,1,2,3,8,8,8,8},
 {4,8,8,8,8,8,8,8},{0,4,8,8,8,8,8,8},{1,4,8,8,8,8,8,8},{0,1,4,8,8,8,8,8},
 {2,4,8,8,8,8,8,8},{0,2,4,8,8,8,8,8},{1,2,4,8,8,8,8,8},{0,1,2,4,8,8,8,8},
 {3,4,8,8,8,8,8,8},{0,3,4,8,8,8,8,8},{1,3,4,8,8,8,8,8},{0,1,3,4,8,8,8,8},
 {2,3,4,8,8,8,8,8},{0,2,3,4,8,8,8,8},{1,2,3,4,8,8,8,8},{0,1,2,3,4,8,8,8},
 {5,8,8,8,8,8,8,8},{0,5,8,8,8,8,8,8},{1,5,8,8,8,8,8,8},{0,1,5,8,8,8,8,8},
 {2,5,8,8,8,8,8,8},{0,2,5,8,8,8,8,8},{1,2,5,8,8,8,8,8},{0,1,2,5,8,8,8,8},
 {3,5,8,8,8,8,8,8},{0,3,5,8,8,8,8,8},{1,3,5,8,8,8,8,8},{0,1,3,5,8,8,8,8},
 {2,3,5,8,8,8,8,8},{0,2,3,5,8,8,8,8},{1,2,3,5,8,8,8,8},{0,1,2,3,5,8,8,8},
 {4,5,8,8,8,8,8,8},{0,4,5,8,8,8,8,8},{1,4,5,8,8,8,8,8},{0,1,4,5,8,8,8,8},
 {2,4,5,8,8,8,8,8},{0,2,4,5,8,8,8,8},{1,2,4,5,8,8,8,8},{0,1,2,4,5,8,8,8},
 {3,4,5,8,8,8,8,8},{0,3,4,5,8,8,8,8},{1,3,4,5,8,8,8,8},{0,1,3,4,5,8,8,8},
 {2,3,4,5,8,8,8,8},{0,2,3,4,5,8,8,8},{1,2,3,4,5,8,8,8},{0,1,2,3,4,5,8,8},
 {6,8,8,8,8,8,8,8},{0,6,8,8,8,8,8,8},{1,6,8,8,8,8,8,8},{0,1,6,8,8,8,8,8},
 {2,6,8,8,8,8,8,8},{0,2,6,8,8,8,8,8},{1,2,6,8,8,8,8,8},{0,1,2,6,8,8,8,8},
 {3,6,8,8,8,8,8,8},{0,3,6,8,8,8,8,8},{1,3,6,8,8,8,8,8},{0,1,3,6,8,8,8,8},
 {2,3,6,8,8,8,8,8},{0,2,3,6,8,8,8,8},{1,2,3,6,8,8,8,8},{0,1,2,3,6,8,8,8},
 {4,6,8,8,8,8,8,8},{0,4,6,8,8,8,8,8},{1,4,6,8,8,8,8,8},{0,1,4,6,8,8,8,8},
 {2,4,6,8,8,8,8,8},{0,2,4,6,8,8,8,8},{1,2,4,6,8,8,8,8},{0,1,2,4,6,8,8,8},
 {3,4,6,8,8,8,8,8},{0,3,4,6,8,8,8,8},{1,3,4,6,8,8,8,8},{0,1,3,4,6,8,8,8},
 {2,3,4,6,8,8,8,8},{0,2,3,4,6,8,8,8},{1,2,3,4,6,8,8,8},{0,1,2,3,4,6,8,8},
 {5,6,8,8,8,8,8,8},{0,5,6,8,8,8,8,8},{1,5,6,8,8,8,8,8},{0,1,5,6,8,8,8,8},
 {2,5,6,8,8,8,8,8},{0,2,5,6,8,8,8,8},{1,2,5,6,8,8,8,8},{0,1,2,5,6,8,8,8},
 {3,5,6,8,8,8,8,8},{0,3,5,6,8,8,8,8},{1,3,5,6,8,8,8,8},{0,1,3,5,6,8,8,8},
 {2,3,5,6,8,8,8,8},{0,2,3,5,6,8,8,8},{1,2,3,5,6,8,8,8},{0,1,2,3,5,6,8,8},
 {4,5,6,8,8,8,8,8},{0,4,5,6,8,8,8,8},{1,4,5,6,8,8,8,8},{0,1,4,5,6,8,8,8},
 {2,4,5,6,8,8,8,8},{0,2,4,5,6,8,8,8},{1,2,4,5,6,8,8,8},{0,1,2,4,5,6,8,8},
 {3,4,5,6,8,8,8,8},{0,3,4,5,6,8,8,8},{1,3,4,5,6,8,8,8},{0,1,3,4,5,6,8,8},
 {2,3,4,5,6,8,8,8},{0,2,3,4,5,6,8,8},{1,2,3,4,5,6,8,8},{0,1,2,3,4,5,6,8},
 {7,8,8,8,8,8,8,8},{0,7,8,8,8,8,8,8},{1,7,8,8,8,8,8,8},{0,1,7,8,8,8,8,8},
 {2,7,8,8,8,8,8,8},{0,2,7,8,8,8,8,8},{1,2,7,8,8,8,8,8},{0,1,2,7,8,8,8,8},
 {3,7,8,8,8,8,8,8},{0,3,7,8,8,8,8,8},{1,3,7,8,8,8,8,8},{0,1,3,7,8,8,8,8},
 {2,3,7,8,8,8,8,8},{0,2,3,7,8,8,8,8},{1,2,3,7,8,8,8,8},{0,1,2,3,7,8,8,8},
 {4,7,8,8,8,8,8,8},{0,4,7,8,8,8,8,8},{1,4,7,8,8,8,8,8},{0,1,4,7,8,8,8,8},
 {2,4,7,8,8,8,8,8},{0,2,4,7,8,8,8,8},{1,2,4,7,8,8,8,8},{0,1,2,4,7,8,8,8},
 {3,4,7,8,8,8,8,8},{0,3,4,7,8,8,8,8},{1,3,4,7,8,8,8,8},{0,1,3,4,7,8,8,8},
 {2,3,4,7,8,8,8,8},{0,2,3,4,7,8,8,8},{1,2,3,4,7,8,8,8},{0,1,2,3,4,7,8,8},
 {5,7,8,8,8,8,8,8},{0,5,7,8,8,8,8,8},{1,5,7,8,8,8,8,8},{0,1,5,7,8,8,8,8},
 {2,5,7,8,8,8,8,8},{0,2,5,7,8,8,8,8},{1,2,5,7,8,8,8,8},{0,1,2,5,7,8,8,8},
 {3,5,7,8,8,8,8,8},{0,3,5,7,8,8,8,8},{1,3,5,7,8,8,8,8},{0,1,3,5,7,8,8,8},
 {2,3,5,7,8,8,8,8},{0,2,3,5,7,8,8,8},{1,2,3,5,7,8,8,8},{0,1,2,3,5,7,8,8},
 {4,5,7,8,8,8,8,8},{0,4,5,7,8,8,8,8},{1,4,5,7,8,8,8,8},{0,1,4,5,7,8,8,8},
 {2,4,5,7,8,8,8,8},{0,2,4,5,7,8,8,8},{1,2,4,5,7,8,8,8},{0,1,2,4,5,7,8,8},
 {3,4,5,7,8,8,8,8},{0,3,4,5,7,8,8,8},{1,3,4,5,7,8,8,8},{0,1,3,4,5,7,8,8},
 {2,3,4,5,7,8,8,8},{0,2,3,4,5,7,8,8},{1,2,3,4,5,7,8,8},{0,1,2,3,4,5,7,8},
 {6,7,8,8,8,8,8,8},{0,6,7,8,8,8,8,8},{1,6,7,8,8,8,8,8},{0,1,6,7,8,8,8,8},
 {2,6,7,8,8,8,8,8},{0,2,6,7,8,8,8,8},{1,2,6,7,8,8,8,8},{0,1,2,6,7,8,8,8},
 {3,6,7,8,8,8,8,8},{0,3,6,7,8,8,8,8},{1,3,6,7,8,8,8,8},{0,1,3,6,7,8,8,8},
 {2,3,6,7,8,8,8,8},{0,2,3,6,7,8,8,8},{1,2,3,6,7,8,8,8},{0,1,2,3,6,7,8,8},
 {4,6,7,8,8,8,8,8},{0,4,6,7,8,8,8,8},{1,4,6,7,8,8,8,8},{0,1,4,6,7,8,8,8},
 {2,4,6,7,8,8,8,8},{0,2,4,6,7,8,8,8},{1,2,4,6,7,8,8,8},{0,1,2,4,6,7,8,8},
 {3,4,6,7,8,8,8,8},{0,3,4,6,7,8,8,8},{1,3,4,6,7,8,8,8},{0,1,3,4,6,7,8,8},
 {2,3,4,6,7,8,8,8},{0,2,3,4,6,7,8,8},{1,2,3,4,6,7,8,8},{0,1,2,3,4,6,7,8},
 {5,6,7,8,8,8,8,8},{0,5,6,7,8,8,8,8},{1,5,6,7,8,8,8,8},{0,1,5,6,7,8,8,8},
 {2,5,6,7,8,8,8,8},{0,2,5,6,7,8,8,8},{1,2,5,6,7,8,8,8},{0,1,2,5,6,7,8,8},
 {3,5,6,7,8,8,8,8},{0,3,5,6,7,8,8,8},{1,3,5,6,7,8,8,8},{0,1,3,5,6,7,8,8},
 {2,3,5,6,7,8,8,8},{0,2,3,5,6,7,8,8},{1,2,3,5,6,7,8,8},{0,1,2,3,5,6,7,8},
 {4,5,6,7,8,8,8,8},{0,4,5,6,7,8,8,8},{1,4,5,6,7,8,8,8},{0,1,4,5,6,7,8,8},
 {2,4,5,6,7,8,8,8},{0,2,4,5,6,7,8,8},{1,2,4,5,6,7,8,8},{0,1,2,4,5,6,7,8},
 {3,4,5,6,7,8,8,8},{0,3,4,5,6,7,8,8},{1,3,4,5,6,7,8,8},{0,1,3,4,5,6,7,8},
 {2,3,4,5,6,7,8,8},{0,2,3,4,5,6,7,8},{1,2,3,4,5,6,7,8},{0,1,2,3,4,5,6,7}};

/*
 * w_findnth/2 - find the nth bit set in the word and return its bit number.
 */

LONG
w_findnth(n,w)
	LONG n,w;
	{
	LONG val= w,bno,nset,index;
	for (bno=0l;bno<INT_SZ;bno+=BYTE_SZ,val>>=BYTE_SZ)
	  if((nset=byte_sz[index=val&0xff])<n) n-=nset;
	  else {
	    bno+=bitsinc[index][n-1];
	    break;
	  }
	return(bno);
}

/*
 * b_sample(m,bs) - randomly choose m elements of the bitset
 *	bs and return sampled set.
 */

ITEM
b_sample(m,bs)
	LONG m;
	ITEM bs;
	{
	LONG size;
	ITEM result=B_EMPTY;
	BLOCK b=(BLOCK)I_GET(bs),bsum=b_bitsum(b,&size);
	struct otree *tree=ot_sample(m,size);
	b_sample1(tree,b,bsum,result);
	ot_delete(tree); b_delete(bsum);
	return(result);
}

b_sample1(tree,b,bsum,result)
	struct otree *tree;
	BLOCK b,bsum;
	ITEM result;
	{
	if(tree) {
		b_sample1(tree->left,b,bsum,result);
		b_add(b_findnth(tree->val,b,bsum),result);
		b_sample1(tree->right,b,bsum,result);
	}
}

/* normal(x,mu,sg) - gives the tail of the normal distribution
 *	with mean mu and std. dvn. sg. Calculation is done using
 *	the following Taylor series for mu=0, sg=1.
 *	
 *	Integral_0^x p(x)dx = Sum_(i=0)^(inf)((-1^i)x^(2i+1)/((2i+1)i!(2^i)))
 *						sqrt(2 PI)
 */

#define PI5	3.1416
#define DELTA	0.001

double
s_normal(z)
	double z;
	{
	PREDICATE neg;
	double fy;
	register double i,sign,y,dy,ti,ifact;
	y=0.0; dy=RINF; sign=1.0; ifact=1.0;
	if(neg=(z<0.0)) z= -z;
	if(z>5.0) y=0.5;
	else for(i=0.0;dy>=DELTA;) {
	      ti=2.0*i+1.0;
	      dy=(pow(z,ti))/(sqrt(2.0*PI5)*ti*ifact*pow(2.0,i));
	      y+=sign*dy;
	      sign= -sign; ifact*=(i+=1.0);
	}
	fy=(neg?0.5-y:y+0.5);
	return(fy);
}

/* s_nmsample - randomly samples according to the density function
 *	of the normal distribution. This is done by transforming
 *	the choice to the [0-1] interval, and carrying out a binary
 *	chop procedure.
 */

#define		GN(z,mu,sg)	((z)*(sg)+(mu))

double
s_nmsample(mu,sg)
	double mu,sg;
	{
	double r=RAND,lo= -5.0,hi=5.0,mid;
	double result;
	while(lo+DELTA<hi) {
	  mid=(hi+lo)/2.0;
	  if(r<s_normal(mid)) hi=mid;
	  else lo=mid;
	}
	result=GN(mid,mu,sg);
	return(result);
}

/* s_permute - algorithm for randomly permuting N clauses in a file.
 *	It is aimed at avoiding loading file into main memory. This is a
 *	randomised version of quick-sort, and thus has time complexity
 *	O(NlgN). For small files it switches to Knuth's algorithm and
 *	does the permuting in main memory. Knuth's algorithm is O(N).
 */



/* s_kpermute - Knuth's algorithm for in memory random permutation
 *	of an array.
 */

void s_kpermute(file,len,fileO,depth)
	STRING file,fileO;
	LONG len,depth;
	{
	ITEM perms=F_EMPTY,i,swap;
	LONG cnt,r;
	FILEREC *f,*fout;
	DOUBLE dlen,dcnt,dr;
	if (!(f=frecopen(file,"r"))) {
		printf("[Cannot find %s]\n",file);
		return;
	}
	/* Read clauses into array */
	for(cnt=depth;cnt>0l;cnt--) printf(" ");
	g_message(1l,"IN MEMORY permute %s (%d)",file,len);
	cnt=0l;
	while((i=ccl_fread(f))!=(ITEM)I_TERM) {
	    if(i==(ITEM)I_ERROR) continue;
	    else *f_ins(cnt++,perms)=i;
	}
	if(cnt!=len) {
		printf("cnt=%d; len=%d\n",cnt,len);
		d_error("Miscount of clauses in file");
	}
	freclose(f);
	fout=frecopen(fileO,"w");
	dlen=len;
	for(cnt=0l;cnt<len;cnt++) {	/* Permutation */
	  dcnt=cnt;
	  dr=dcnt+(dlen-dcnt)*drand48();
	  r=dr;
	  swap= *f_ins(r,perms);
	  *f_ins(r,perms)= *f_ins(cnt,perms);
	  *f_ins(cnt,perms)=swap;
	  ccl_fwrite(fout,swap,(BIND)NULL);
	  i_fnl(fout);
	}
	i_delete(perms);
	freclose(fout);
}

void s_randsplit(file,file0,file1,len0,len)
	STRING file,file0,file1;
	LONG len0,len;
	{
	FILEREC *f,*f0,*f1,*fout;
	ITEM i;
	DOUBLE dlen0=len0,dlen=len;
	char mess[MAXMESS],mess1[MAXMESS];
	if (!(f=frecopen(file,"r"))) {
		printf("[Cannot find %s]\n",file);
		return;
	}
	sprintf(file0,"%s0",file); sprintf(file1,"%s1",file);
	f0=frecopen(file0,"w"); f1=frecopen(file1,"w");
	while((i=ccl_fread(f))!=(ITEM)I_TERM) {
	    if(i==(ITEM)I_ERROR) continue;
	    else {
	      if(drand48()<(dlen0/dlen)) {fout=f0; (dlen0)--;}
	      else fout=f1;
	      dlen--;
	      ccl_fwrite(fout,i,(BIND)NULL);
	      i_fnl(fout);
	    }
	    i_delete(i);
	}
	freclose(f); freclose(f0); freclose(f1);
}


#define	INMEM	20000l

void s_permute(fileI,len,fileO,depth)
	STRING fileI,fileO;
	LONG len,depth;
	{
	char file0I[MAXMESS],file1I[MAXMESS],
		file0O[MAXMESS],file1O[MAXMESS],command[MAXMESS];
	LONG len0=len>>1l,cnt;
	sprintf(fileO,"%sO",fileI);
	if(len<=1l) {
	  sprintf(command,"cp %s %s",fileI,fileO);
	  system(command);
	  return;
	}
	if(len<=INMEM) {s_kpermute(fileI,len,fileO,depth+1l); return;}
	for(cnt=depth;cnt>0l;cnt--) printf(" ");
	g_message(1l,"Permute %s (%d)",fileI,len);
	s_randsplit(fileI,file0I,file1I,len0,len);
	s_permute(file0I,len0,file0O,depth+1l); s_delfile(file0I);
	s_permute(file1I,len-len0,file1O,depth+1l); s_delfile(file1I);
	s_appfile(file0O,file1O,fileO);
	s_delfile(file0O); s_delfile(file1O);
}

s_delfile(f)
	STRING f;
	{
	char command[MAXMESS];
	sprintf(command,"rm %s",f);
	system(command);
}

s_appfile(f0,f1,f)
	STRING f0,f1,f;
	{
	char command[MAXMESS];
	sprintf(command,"cat %s %s > %s",f0,f1,f);
	system(command);
}

LONG
s_length(file)
	STRING file;
	{
	LONG res=0l;
	FILEREC *f;
	ITEM i;
	char mess[MAXMESS],mess1[MAXMESS];
	if (!(f=frecopen(file,"r"))) {
		printf("[Cannot find %s]\n",file);
		return(0);
	}
	g_message(1l,"Counting clauses in <%s>",file);
	while((i=ccl_fread(f))!=(ITEM)I_TERM) {
	    if(i==(ITEM)I_ERROR) continue;
	    else res++;
	    i_delete(i);
	}
	freclose(f);
	return(res);
}

