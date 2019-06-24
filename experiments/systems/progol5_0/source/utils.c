/* ####################################################################### */

/*                      PROGOL Utility Routines                		   */
/*                   	--------------------- 		                   */

/* ####################################################################### */

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include "progol.h"

/*  d_error(msg) - error messages for PROGOL, prints message
 *	and forces a core dump.
 */

int
d_error(mesg)
        char *mesg;  {

        fprintf(stderr, "\n*** PROGOL Error\n");
        fprintf(stderr, "*** %s\n", mesg);
	exit(1);
}

/*  strsave(s) - allocate space, save char *s, and return a pointer to s
 *   return NULL if no space is found
 */

STRING
strsave(s)
        char *s; {
        char *p;
	LONG length;

	length=strlen(s)+1;
        if ((p = (char *)PROGOL_CALLOC(length,sizeof(char))) != NULL)
                strcpy(p, s);
        else
                d_error("strsave: PROGOL_CALLOC error (no space?)\n");

        return(p);
}

/* ualphanum(name) - checks whether name contains
 *	only alphanumeric characters starting with upper case letter
 */

PREDICATE
ualphanum(name)
	STRING name;
	{
	register STRING sp;
	
	if (*name == '\0' || !(UPCASE(*name)||*name=='_')) return(FALSE);
	STR_LOOP(sp,name+1) if (!ALPHANUM(*sp)) return(FALSE);
	return(TRUE);
}

/* lalphanum(name) - checks whether name contains
 *	only alphanumeric characters starting with lower case letter
 */

PREDICATE
lalphanum(name)
	STRING name;
	{
	register STRING sp;
	
	if (*name == '\0' || !LOWCASE(*name)) return(FALSE);
	STR_LOOP(sp,name+1) if (!ALPHANUM(*sp)) return(FALSE);
	return(TRUE);
}

/* number(name) - checks whether "name" is a number. Uses simple
 *	finite state parser.
 */

#define	SIGN	0l
#define	WHOL	1l
#define	FRAC	2l
#define	EXSN	3l
#define	EXNM	4l

PREDICATE
number(name)
	STRING name;
	{
	LONG state=SIGN;
	STRING strp;
	register char c;
	STR_LOOP(strp,name) {
	  c= *strp;
	  switch(state) {
	    case SIGN:
	      if(DIGIT(c)) state=WHOL;
	      else if((c=='+'||c=='-')&& *(strp+1)!='\0') state=WHOL;
	      else return(FALSE);
	      break;
	    case WHOL:
	      if(c=='.') state=FRAC;
	      else if(!DIGIT(c)) return(FALSE);
	      break;
	    case FRAC:
	      if((c=='e'||c=='E')&& *(strp+1)!='\0') state=EXSN;
	      else if(!DIGIT(c)) return(FALSE);
	      break;
	    case EXSN:
	      if(DIGIT(c)) state=EXNM;
	      else if((c=='+'||c=='-')&& *(strp+1)!='\0') state=EXNM;
	      else return(FALSE);
	      break;
	    case EXNM:
	      if(!DIGIT(c)) return(FALSE);
	      break;
	    default:
	      return(FALSE);
	  }
	}
	if(state==SIGN) return(FALSE);
	else return(TRUE);
}

/* lsymbol(name) - checks whether name contains
 *	only Prolog non-alphanumeric printable characters
 */

PREDICATE
lsymbol(name)
	STRING name;
	{
	register STRING sp;
	if (*name == '\0') return(FALSE);
	else if(*name== '-' && number(name+1l)) return(TRUE);
	STR_LOOP(sp,name)
		switch(*sp) {
		    case '+': case '-': case '*': case '/': case '\\': case '^':
		    case '<': case '>': case '=': case '`': case '~': case ':':
		    case '.': case '?': case '@': case '#': case '$': case '&': 
		    case '!': case ',': case ';':
			break;
		    default:
			return(FALSE);
		}
	return(TRUE);
}

/* fsym(pair,name,arity) - pair should be term of type F/Arity.
 *	Assigns name and arity and returns hashed symbol number.
 */

LONG
fsym(pair,name,arity,sub)
	ITEM pair;
	STRING *name;
	LONG *arity;
	BIND sub;
	{
	PREDICATE succeeds=TRUE;
	ITEM p,a;
	BIND sub1=sub,sub2=sub;
	if(pair->item_type == 'f') {
	    if((LONG)I_GET(F_ELEM(0l,pair))==pdiv2) {
	      p=F_ELEM(1l,pair); a=F_ELEM(2l,pair);
	      if(sub) {SKIPVARS(p,sub1); SKIPVARS(a,sub2);}
	      if(p->item_type!='h'||a->item_type!='i')
		succeeds=FALSE;
	    }
	    else succeeds=FALSE;
	    if(!succeeds) return((LONG)I_ERROR);
	    *name=QP_ntos((LONG)I_GET(p));
	    *arity=(LONG)I_GET(a);
	}
	else if(pair->item_type == 'h') {
	    *name=QP_ntos((LONG)I_GET(pair));
	    *arity=0l;
	}
	else return((LONG)I_ERROR);
	return(QP_ston(*name,*arity));
}

/* predn - string formatting for Pred/Arity
 */

predn(mess,name,arity)
	char mess[];
	STRING name;
	LONG arity;
	{
	PREDICATE unquoted,symbolic=lsymbol(name),num=number(name);
	char mess1[MAXMESS];
	unquoted=(symbolic||num||lalphanum(name));
	if(unquoted) sprintf(mess1,"%s",name);
	else sprintf(mess1,"'%s'",name);
	if(!(arity))sprintf(mess,"%s",mess1);
	else if(lsymbol(name))sprintf(mess,"%s /%d",mess1,arity);
	else sprintf(mess,"%s/%d",mess1,arity);
}

/* cputime() - returns cputime since start of process in seconds.
 */

#ifdef SUNCHECK
#include <sys/types.h>
#include <sys/times.h>
#include <sys/time.h>
#else
#include <time.h>
#endif

double
cputime()
	{
#ifdef SUNCHECK
	int sec,usec;
	double res,dsec,dusec;
	struct rusage buffer;
	getrusage(RUSAGE_SELF,&buffer);
	sec=buffer.ru_utime.tv_sec;
	usec=buffer.ru_utime.tv_usec;
	dsec=sec; dusec=usec;
	res=dsec+(dusec/1000000.0);
	return(res);
#else
	time_t res;
	time(&t2);
	res=t2;
	return(res);
#endif
}

/* datetime() - returns no. of seconds since 1.1.1970
 */

LONG
datetime()
	{
#ifdef SUNCHECK
	struct timeval tp;
	struct timezone tzp;
	gettimeofday(&tp,&tzp);
	return(tp.tv_sec);
#else
	return(0);
#endif
}

/* g_message(v,s,a0,..,a9) - same argument pattern as printf
 *	except for first argument (verbosity level).
 *	Only prints s if the verbose level is greater than or eq to 1st arg.
 */

int
g_message(v,s,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9)
	LONG v;
	STRING s;
	POINTER a0,a1,a2,a3,a4,a5,a6,a7,a8,a9;
	{
	char mess[MAXMESS];
	sprintf(mess,"[%s]\n",s);
	if(verbose>=v) printf(mess,a0,a1,a2,a3,a4,a5,a6,a7,a8,a9);
}

/* charline(in) - returns the first printable char on the tty line
 *	and consumes the rest of the line. If the line is
 *	empty then returns '\0'.
 */

char
ttychline()
	{
	char c,result= '\0';
	c=getc(stdin);	/* Clear last character from buffer */
	while((c=getc(stdin)) != '\n' && c!=(char)EOF)
	    if(!result && !PADDING(c)) result=c;
	ungetc(c,stdin);
	return(result);
}
