/* ####################################################################### */

/*                      PROGOL chi-square functions			   */
/*                      ---------------------------			   */

/* ####################################################################### */

#include        <stdio.h>
#include        "progol.h"

static DOUBLE gsum(), gprod();

#define	FUZZY	((DOUBLE)1e-30)

DOUBLE gamma_func();

/* Chi-square probability for a rxc contingency table */
DOUBLE
chisq(table,r,c)
LONG **table;
LONG r, c;
{
	DOUBLE expected, x2, prob;
	LONG *rtotals, *ctotals, sum, df;
	LONG nrows, ncols;
	register LONG i, j;

	rtotals = (LONG *)PROGOL_CALLOC(r,sizeof(LONG));
	ctotals = (LONG *)PROGOL_CALLOC(c,sizeof(LONG));
	sum  = 0l;
	nrows = r; ncols = c;
	for (i=0; i<r;i++) {
		rtotals[i] = (DOUBLE)0.0;
		for (j=0;j<c;j++)
			rtotals[i]+=table[i][j];
		sum+= rtotals[i];
		if (!rtotals[i])
			nrows--; 
	}
	for (j=0; j<c;j++) {
		ctotals[j] = (DOUBLE)0.0;
		for (i=0;i<r;i++)
			ctotals[i]+=table[i][j];
		if (!ctotals[j])
			ncols--; 
	}
	df = (nrows*ncols) - nrows - ncols  + 1;
	x2 = (DOUBLE)0.0;
	for (i=0;i<r;i++) 
		for (j=0;j<c;j++) {
			expected = (DOUBLE)rtotals[i]*(DOUBLE)ctotals[j]/sum;
			x2 += pow(((DOUBLE)table[i][j]-expected),(DOUBLE)2.0)/(expected+FUZZY);
		}
	PROGOL_CFREE(rtotals,r*sizeof(LONG));
	PROGOL_CFREE(ctotals,c*sizeof(LONG));
	prob = gamma_func(0.5*df,0.5*x2);
	return (DOUBLE)prob;

}
	
/* Chi-square probability for a 2xc contingency table */
DOUBLE
chisq2(table,c)
LONG **table; 
LONG c;
{
	DOUBLE expected, x2, prob;
	LONG rtotals[2], *ctotals, sum, df;
	LONG nrows, ncols;
	register LONG i, j;

	sum  = 0l;
	nrows = 2l; ncols = c;
	for (i=0; i<2;i++) {
		rtotals[i] = (DOUBLE)0.0;
		for (j=0;j<c;j++)
			rtotals[i]+=table[i][j];
		sum+= rtotals[i];
		if (!rtotals[i])
			nrows--; 
	}
	ctotals = (LONG *)PROGOL_CALLOC(c,sizeof(LONG));
	for (j=0; j<c;j++) {
		ctotals[j] = (DOUBLE)0.0;
		for (i=0;i<2;i++)
			ctotals[j]+=table[i][j];
		if (!ctotals[j])
			ncols--; 
	}
	df = (nrows*ncols) - nrows - ncols  + 1;
	x2 = (DOUBLE)0.0;
	for (i=0;i<2;i++) 
		for (j=0;j<c;j++) {
			expected = (DOUBLE)rtotals[i]*(DOUBLE)ctotals[j]/sum;
			x2 += pow(((DOUBLE)table[i][j]-expected),(DOUBLE)2.0)/(expected+FUZZY);
		}
	PROGOL_CFREE(ctotals,c*sizeof(LONG));
	prob = gamma_func(0.5*df,0.5*x2);
	return (DOUBLE)prob;

}
	
/* incomplete gamma function */
DOUBLE
gamma_func(a,x)
DOUBLE a,x;
{

	if ((x < (DOUBLE)0.0) || (a <= (DOUBLE)0.0)) {
		return (DOUBLE)0.0;
	}

	if (x < a + (DOUBLE)1.0)
		return ((DOUBLE)1.0 - gsum(a,x));
	else return (DOUBLE)gprod(a,x);
}

#define	SER_TERMS	100
#define	EPSILON		((DOUBLE)3.0e-7)

/* finite series expansion of the gamma function */
static DOUBLE
gsum(a,x)
DOUBLE a,x;
{
	register INT i;
	BOOL converged;
	DOUBLE sum, del, ap;

	if (x <= (DOUBLE)0.0)
		return (DOUBLE)0.0;

	ap = a;
	sum = (DOUBLE)1.0/a;
	del = sum;
	converged = FALSE;
	for (i=0; i<SER_TERMS;i++) {
		ap = ap + (DOUBLE)1.0;
		del = del*x/(ap);
		sum+= del;
		if (fabs(del)<(fabs(sum)*EPSILON)) {
			converged = TRUE;
			break;
		}
	}
	if (!converged)
		g_message(0,"df too large/no convergence in sum for gamma function");

	return (DOUBLE)(sum*exp(-x+a*log(x) - lgamma(a)));
}

/* continued fraction expansion of incomplete gamma function */
static DOUBLE
gprod(a,x)
DOUBLE a,x;
{
	register INT n;
	BOOL converged;
	DOUBLE gold, g, fac, b1, b0, anf, ana, a1, a0;

	b0 = gold = (DOUBLE)0.0;
	b1 = fac = a0 = (DOUBLE)1.0;
	a1 = x;
	converged = FALSE;
	for (n=1;n<= SER_TERMS;n++){
		ana = (DOUBLE)n - a;
		a0 = (a1+a0*ana)*fac;
		b0 = (b1+b0*ana)*fac;
		anf = (DOUBLE)n*fac;
		a1 = x*a0+anf*a1;
		b1 = x*b0+anf*b1;
		if (a1 != (DOUBLE)0.0) {
			fac = (DOUBLE)1/a1;
			g = b1*fac;
			if (fabs((g-gold)/g) < EPSILON) {
				converged = TRUE;
				break;
			}
			gold = g;
		}
	}
	if (!converged)
		g_message(0,"df too large/no convergence in product for gamma function");

	return (DOUBLE)(exp(-x+a*log(x) - lgamma(a))*g);
}



/* ln Gamma(x): for systems that do not have this in math.h */

/*
#define	STP	((DOUBLE)2.50662827465)

static DOUBLE
lgamma(xx)
DOUBLE xx;
{
	DOUBLE x, tmp, ser;

	x = (DOUBLE)xx - (DOUBLE)1.0;
	tmp = x+(DOUBLE)5.5;
	tmp = (x+(DOUBLE)0.5)*log(tmp) - tmp;
	ser = (DOUBLE)1.0 + (DOUBLE)76.18009173/(x+(DOUBLE)1.0) - (DOUBLE)86.50532033/(x+(DOUBLE)2.0) + (DOUBLE)24.01409822/(x+(DOUBLE)3.0) - (DOUBLE)1.231739516/(x+(DOUBLE)4.0) + (DOUBLE)0.120858003e-2/(x+(DOUBLE)5.0) - (DOUBLE)0.536382e-5/(x+(DOUBLE)6.0);

	return (tmp+log(STP*ser));
}
*/


