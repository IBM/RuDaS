/* ####################################################################### */

/*			PROGOL main functions				   */
/*			--------------------				   */

/* ####################################################################### */

#include <stdio.h>
#include "progol.h"


PREDICATE interactive=TRUE;
ITEM fileroot=(ITEM)NULL;
PREDICATE c_doall();

#ifdef SUNCHECK
main(int argc, char ** argv,char ** envp)
	{
	printf("CProgol Version 5.0\n\n");
#ifdef	CHECK_SECURITY
	check_security();
#endif
#ifdef	UNBUFFERED
	setvbuf(stdout,(char *)NULL,_IONBF,0l);
#endif
	c_open();
	checkargs(argc,argv,envp);
	l_init();			/* Initialise built-in predicates */
	if(interactive) main_prompt();
	else c_doall(fileroot);
	c_close();
	return(1l);
}
#else
main()
	{
#ifdef	CHECK_SECURITY
	check_security();
#endif
	c_open();
	checkargs(argc,argv,envp);
	l_init();			/* Initialise built-in predicates */
	main_prompt();
	c_close();
}
#endif

extern CALL_ENV env_stack0;

/*
 * checkargs/2 - expects command line
 *	progol [-i] fileroot
 */

checkargs(argc,argv,envp)
	LONG argc;
	STRING argv[],envp[];
	{
	LONG argno,n;
	STRING sp;
	for(argno=1;argno<argc;argno++) {
	    if(*argv[argno] == '-') {
		STR_LOOP(sp,argv[argno]+1l)
		    switch(*sp) {
			case 's': /* Prolog stack size */
			    if(++argno<argc && sscanf(argv[argno],"%ld",&n))
			      stack_size=(n>=0l?n:0l);
			    else printf("[Bad stack size]\n");
			    break;
			case 'v': /* Verbose flag */
			    verbose=2l;
			    break;
			default:
			    printf("[Unrecognised flag option <%c>]\n",*sp);
		    }
	    }
	    else if (fileroot==(ITEM)NULL) {
		fileroot=i_create('h',(POINTER)QP_ston(argv[argno],0l));
		interactive=FALSE;
	    }
	    else printf("[Ignoring additional file <%s>]\n",argv[argno]);
	}
        if (!(stack=(char *)malloc(stack_size*sizeof(char))))
		d_error("checkargs - calloc failed");
        /* ALIGN8(stack); */
	term_stack0=stack;
	env_stack0=((CALL_ENV)(stack+stack_size))-1l;
	while(*envp != (STRING)NULL) {	/* Check environment */
	  int l;
	  if((l=pcmp("EDITOR",*envp))>=0l && *(*envp+l)== '=')
	    editor=(*envp+l+1l);
	  envp++;
	}
	if(STREQ(editor,"")) editor= "vi";
}

pcmp(s1,s2)
	STRING s1,s2;
	{
	LONG i=0l;
	while(*s1 != '\0') {
	  i++;
	  if(*s1++ != *s2++) return(-1l);
	}
	return(i);
}
