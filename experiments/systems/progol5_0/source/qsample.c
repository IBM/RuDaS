/* ####################################################################### */

/*			Qsample program					   */
/*			---------------					   */

/* ####################################################################### */

/* qsample can be used to randomly generate training and test sets.
 * It is based on a sampling algorithm by Donald Knuth which can
 * be found in the following book.
 *
 *    Knuth, D.E., "The art of computer programming", 3rd ed., Vol.1,
 *		Addison-Wesley, 1997.
 *
 * It should be used in one of the following ways.
 *
 *	qsample N FromFile ToFile		OR
 *	qsample N FromFile Sample Complement
 *
 * where
 *
 *	N is the Sample size.
 *	Fromfile should contain examples line by line. 
 *	Sample will contain N sampled lines.
 *	Complement contains the lines of Fromfile not in Sample.
 *
 * Qsample will produce a different random sample every time it is
 * called, since the random number generator is seeded from gettimeofday,
 * which returns the number of seconds since 1.1.1970.
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/times.h>
#include <sys/time.h>
#include <stdlib.h>

extern double drand48();

#define		RAND		(drand48())
#define		SRAND(x)	(srand48(x))
#define 	MYRAND(lo,hi) ((lo)+((hi)-(lo))*RAND)
#define		FALSE	0
#define		TRUE	1

long int datetime();

main(argc,argv)
	int argc;
	char *argv[];
	{
	int samplesize,nlines=0,newline=TRUE,printing=FALSE,
		comment=FALSE,start=cputime(),complement;
	double dnlines,dsamplesize;
	FILE *in,*out1,*out2;
	char *inname,*outname1,*outname2;
	char c;
	if(argc==4) complement=FALSE;
	else if(argc==5) complement=TRUE;
	else {
	  printf("Command should be one of the following\n\t<qsample N FromFile ToFile>\n\t<qsample N FromFile Sample Complement>\n");
	  exit(0);
	}
	sscanf(argv[1],"%d",&samplesize);
	inname=argv[2]; outname1=argv[3];
	if (!(in=fopen(inname,"r"))) {
		printf("Cannot find %s\n",inname);
		exit(1);
	}
	out1=fopen(outname1,"w");
	if (complement) {
	  outname2=argv[4];
	  out2=fopen(outname2,"w");
	}
	printf("Counting number of lines\n");
	while((c=fgetc(in))!=(char)EOF) {
	    if(newline) {
		if(c=='%') comment=TRUE;
		newline=FALSE;
	    }
	    if(c=='\n') {
		if(!comment) nlines++;
		newline=TRUE;
		comment=FALSE;
	    }
	}
	printf("Counted %d lines\n",nlines);
	fclose(in); in=fopen(inname,"r");
	printf("Sampling %d from %d\n",samplesize,nlines);
	newline=TRUE; comment=FALSE; printing=FALSE;
	SRAND(datetime());
	while((c=fgetc(in))!=(char)EOF) {
	    if(newline) {
		dnlines=nlines; dsamplesize=samplesize;
		if(c=='%') {printing=FALSE; comment=TRUE;}
		else if(MYRAND(0.0,dnlines)<=dsamplesize) {
		    printing=TRUE;
		    samplesize--;
		}
		else printing=FALSE;
		newline=FALSE;
	    }
	    if(printing) fputc(c,out1);
	    else if(complement&& !comment) fputc(c,out2);
	    if(c=='\n') {
		if(!comment) nlines--;
		newline=TRUE;
		comment=FALSE;
	    }
	}
	fclose(in); fclose(out1);
	if(complement) fclose(out2);
	printf("Time taken = %dms\n",cputime()-start);
}

#include <sys/types.h>
#include <sys/times.h>
#include <sys/time.h>

int
cputime()
	{
	static long int t1=0l;
	long int t2,res;
	struct tms buffer;
	times(&buffer);
	res=(t2=buffer.tms_stime+buffer.tms_utime)-t1;
	t1=t2;
	return(res*16.6);
}

/* datetime() - returns no. of seconds since 1.1.1970
 */

long int
datetime()
	{
	struct timeval tp;
	struct timezone tzp;
	gettimeofday(&tp,&tzp);
	return(tp.tv_sec);
}
