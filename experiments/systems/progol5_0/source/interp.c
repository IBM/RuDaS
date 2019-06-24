/* ####################################################################### */

/*                      PROGOL Prolog interpreter                           */
/*                      ------------------------                           */

/* ####################################################################### */

#include        <stdio.h>
#include        "progol.h"

#define CALL	0l
#define DONE	1l
#define FAIL	2l
#define RETRY	3l

#define		dbound	hlim
char *stack;
LONG stack_size=STACK_SIZE;
LONG breaks=0l;		/* Break depth */
CALL_ENV env_stack0;
char *term_stack0;
PREDICATE generate=FALSE;

LONG rchoose();

/*
 *	PREDICATE interp(clause_in,clearstack,generate)
 *	if `clause_in'==clause, calls Prolog interp. to prove clause
 *	subsequent calls look for additional solutions
 *	quits with FALSE when no more solutions exist, then ready for new query
 *	if `clause_in'==NULL, then it resets the interpreter and quits
 *	
 */

PREDICATE
interp(clause_in,clearstack,gen)
	ITEM clause_in;
	PREDICATE clearstack,gen;
	{
	register CALL_ENV  env_stack;
	register char *term_stack;
	static char *next_term_stack;
	static ITEM clause=(ITEM)NULL;
	static PREDICATE reinterp=FALSE,retrying,builtin,dfailed;
	static CALL_ENV parent,root_env,save_env;
	static ITEM built_terms,matches;
	register LONG rchoice= -INF;
	register ITEM goal;
	register BIND sub,sub1;
	register PREDICATE unif,collecting=FALSE;
	ITEM *entry;
	LONG psym;
	BREAK break1;
#define BREAK_PUSH	break1.clause=clause; break1.reinterp=reinterp; \
			break1.retrying=retrying; break1.parent=parent; \
			break1.next_term_stack=next_term_stack; \
			break1.root_env=root_env; break1.save_env=save_env; \
			break1.built_terms=built_terms; \
			break1.env_stack0=env_stack0; \
			break1.env_stack=env_stack; \
			break1.term_stack0=term_stack0; breaks++; \
			env_stack0=env_stack-1l; term_stack0=next_term_stack; \
			reinterp=FALSE
#define BREAK_POP 	clause=break1.clause; reinterp=break1.reinterp; \
			retrying=break1.retrying; parent=break1.parent; \
			next_term_stack=break1.next_term_stack; \
			root_env=break1.root_env; save_env=break1.save_env; \
			built_terms=break1.built_terms; \
			env_stack0=break1.env_stack0; \
			env_stack=break1.env_stack; \
			term_stack0=break1.term_stack0; breaks--
#define SUCCEEDS	{/* showstack(env_stack,env_stack0); */ \
			save_env=env_stack; reinterp=TRUE; \
			return(TRUE); }
#define FAILS		{i_deletes(clause,built_terms,(ITEM)I_TERM); \
			reinterp=FALSE; save_env=(CALL_ENV)NULL; \
			pdebug=FALSE; return(FALSE);}
#define CHECK_I_STACKS  if((char*)env_stack <= (char*)term_stack) { \
			    printf("[Environment stack overflow]\n"); FAILS;}
#define CLEAR_STACK(c)	for(sub=((BIND)term_stack)+(VAR_NO(c))-1l; \
			    sub>=(BIND)term_stack;sub--) sub->term=(ITEM)NULL;
#define I_DEBUG(p,e)	if(SETQ(tracing)||pdebug||b_memq(PSYM(HOF(e->atom)),spies)) \
			    if(!plg_debug(p,e)) FAILS
#define NEXT_CLAUSE(e,l) {register LIST ze;register LONG cno;LIST_LOOP(ze,l){\
			    cno=L_GET(ze)->extra; if(SETQ(sampling)&& !collecting)\
			    {if(cno==rchoice) break;}\
			    else if(b_memq(cno,bclauses)) break;}e=ze;}
	if (!clause_in) FAILS;
	generate=gen;
	if (reinterp) {env_stack=save_env; goto restart_interp;}
	/*
	 *   initialise stacks, first environment to goal clause
	 */
	built_terms=L_EMPTY;
	clause=i_inc(clause_in);
	env_stack=root_env=env_stack0;
	term_stack=term_stack0;
	parent=env_stack;
	env_stack->parent=(CALL_ENV)NULL;
	env_stack->clause=clause;
	env_stack->next_call=TOF(CLAUSE(env_stack->clause));
	if(!env_stack->next_call) SUCCEEDS;
	env_stack->next_clause=(LIST)NULL;
	env_stack->term_stack=term_stack;
	if(clearstack) CLEAR_STACK(clause);
	env_stack->subst=((BIND)term_stack);
	term_stack=(char*)(((BIND)term_stack)+VAR_NO(clause));
	CHECK_I_STACKS;
	env_stack->reset=(BIND *)NULL;
	dfailed=FALSE;
	for(;;) {
	    /*
	     *	parent contains details of next goal, setup new environment
	     */
	    env_stack--;
	    env_stack->parent=parent;
	    env_stack->reset=(BIND *)NULL;
	    env_stack->atom=parent->next_call;
	    parent->next_call=TOF(parent->next_call);
	    env_stack->term_stack=term_stack;
	    env_stack->callno=0l;
	    I_DEBUG(CALL,env_stack);
	    retrying=FALSE;
	    /*
	     *   fetch list of matching clauses 
	     */
	    sub=env_stack->parent->subst;
	    if ((goal=HOF(env_stack->atom))->item_type== 'v') { /* Var call */
		SKIPVARS(goal,sub);
		if(goal->item_type== 'v') {
		    env_stack->next_clause=(LIST)NULL;
		    goto restart_interp;
		}
	    }
	    PDEF(entry,goal,sub); /* First argument indexing */
	    if(SETQ(sampling)) {collecting=TRUE; matches=B_EMPTY;}
	    else collecting=FALSE;
	    if((root_env-env_stack) <= dbound && *entry)
		NEXT_CLAUSE(env_stack->next_clause,(LIST)I_GET(*entry))
	    else {
	      if((root_env-env_stack) > dbound && !dfailed) {
		dfailed=TRUE;
	        g_message(1l,"WARNING: depth-bound failure - use set(h,..)");
	      }
	      env_stack->next_clause=(LIST)NULL; retrying=TRUE;
	    }
restart_interp:
#define RESET	if(reset=env_stack->reset) \
			while(*reset) (*(reset++))->term=(ITEM)NULL
	    for(;;) {
	        /*
	         *      backtrack to goal that has further clauses to match
	         */
	        for(;;) {
	            register BIND *reset;
		    RESET;
	            if(env_stack->next_clause) {
			if(retrying) I_DEBUG(RETRY,env_stack);
			break;
		    }
		    if(SETQ(sampling)&&collecting) {
		      collecting=retrying=FALSE;
		      rchoice=rchoose(matches); i_delete(matches);
		      if(rchoice!=I_ERROR) {
		        NEXT_CLAUSE(env_stack->next_clause,(LIST)I_GET(*entry));
		        goto restart_interp;
		      }
		    }
		    if(SETQ(sampling)&& !collecting)
			{/*UNSET(sampling); */ FAILS;}
		    /* Test for cut and jump to env_stack's parent's parent */
		    if((goal=HOF(env_stack->atom))->item_type=='h' &&
				(LONG)I_GET(goal)==pcut) {
			register CALL_ENV cut_point=env_stack->parent;
			do {env_stack++; RESET;} while(env_stack!=cut_point);
			if(env_stack>=root_env) FAILS;
		    }
		    I_DEBUG(FAIL,env_stack);
		    env_stack->parent->next_call=env_stack->atom;
	            if(++env_stack>=root_env) FAILS;
	        }
	        /*
	         *      try another matching
	         */
		sub1=env_stack->parent->subst;
		if ((goal=HOF(env_stack->atom))->item_type== 'v')
			SKIPVARS(goal,sub1);
		psym=PSYM(goal);
	        env_stack->clause=HOF(env_stack->next_clause);
		env_stack->subst=((BIND)(term_stack=env_stack->term_stack));
		CLEAR_STACK(HOF(env_stack->next_clause));
	        next_term_stack=(char*)(env_stack->reset=(BIND*)
		    (((BIND)term_stack)+VAR_NO(HOF(env_stack->next_clause))));
		retrying=TRUE;
		sub=sub1;
		/* Built-in predicate */
	        if(b_memq(psym,lib)&& !(TOF(env_stack->next_clause))) {
		    builtin=TRUE;
		    BREAK_PUSH;
		    if(env_stack->callno>dbound ||
				!l_interp(goal,sub,env_stack->subst,
				env_stack->callno++,built_terms,&break1)) {
			BREAK_POP;
			NEXT_CLAUSE(env_stack->next_clause,
				TOF(env_stack->next_clause));
			env_stack->reset=(BIND*)NULL;
			continue;
		    }
		    BREAK_POP;
		}
		else {builtin=FALSE; NEXT_CLAUSE(env_stack->next_clause,
			TOF(env_stack->next_clause));}
	        unif=unify(env_stack->subst,
			HOF(CLAUSE(env_stack->clause)),sub,goal,
		       (BIND**)&next_term_stack,(UNIFY_STACK)(env_stack-1));
		if(!unif) continue;
	        else if(unif==(PREDICATE)I_ERROR) FAILS
	        else if(!collecting) {
		  if(conditioning) (*y_ins(env_stack->clause->extra,labels))++;
		  break;
		}
		else if(builtin) {i_delete(matches); break;}
		else b_add(env_stack->clause->extra,matches);
	    }
	    /*
	     *	unification was successful for "env_stack", so find next
	     *  parent with unsatisfied subgoal
	     */
	    term_stack=next_term_stack;
	    env_stack->next_call=TOF(CLAUSE(env_stack->clause));
	    for(parent=env_stack;!parent->next_call;parent=parent->parent) {
		if(!parent->parent) SUCCEEDS;
		I_DEBUG(DONE,parent);
	    }
	}
}

/*
 *	unify(s1,t1,s2,t2,reset,u_stack)
 *
 *	Try to unify two bindings,
 *	Always leaves trail of vars bound in "*reset",
 *		whether successful or not.
 *	Trail is 0 terminated, fills upward.
 *	Return 1 if successful, 0 otherwise.
 */

PREDICATE
unify(s1,t1,s2,t2,reset,u_stack)
	BIND s1,s2;
	ITEM t1,t2;
	BIND **reset;		/*  stack "*reset" fills up  */
	UNIFY_STACK u_stack;	/*  "u_stack" fills down to "*reset" */
	{
	register BIND temp_subst;
	register char vt1,vt2;
	register FUNC f1,f2;
	UNIFY_STACK u_stack_start=u_stack;
	BIND *r_stack= *reset;
	LONG next;
#define CHECK_U_STACKS   if ((char*)u_stack <= (char*)r_stack ) { \
			    printf("[Unification stack overflow]\n"); \
			    return((PREDICATE)I_ERROR); }
#define VASSIGN(x,y) temp_subst = BOUND(u_stack->x); temp_subst->term = \
		  u_stack->y.term; temp_subst->subst = u_stack->y.subst; \
		  *(r_stack++) = temp_subst; CHECK_U_STACKS
	(--u_stack)->b1.term = t1;
	u_stack->b1.subst = s1;
	u_stack->b2.term = t2;
	u_stack->b2.subst = s2;
	u_stack->next = 0;
	CHECK_U_STACKS;
	/*
	 *	each pass of loop unifies two terms at the current place
	 *	exit loop when unification has failed
	 */
	for(;;) {
	    /*
	     *	skip through variables with bindings
	     */
	    SKIPVARS(u_stack->b1.term,u_stack->b1.subst);
	    SKIPVARS(u_stack->b2.term,u_stack->b2.subst);
	    vt1=u_stack->b1.term->item_type;
	    vt2=u_stack->b2.term->item_type;
	    /*
 	     *	if either term is a variable, then have success at current place
	     */
	    if(vt1=='v'&&(vt2!='v'||(BOUND(u_stack->b1)>BOUND(u_stack->b2))))
		{VASSIGN(b1,b2);}
	    else if(vt2 == 'v') {
	      if(vt1!='v'||(BOUND(u_stack->b1)<BOUND(u_stack->b2)))
		{VASSIGN(b2,b1);}
	    }
	    else {
		double *r1,*r2;
		/*
		 * otherwise, check for matching function symbol
		 */
		if(vt1 != vt2)
		    break;
		if(vt1 == 'f')
		    if ((LONG)I_GET(FNAME(f1=(FUNC)I_GET(u_stack->b1.term))) ==
			(LONG)I_GET(FNAME(f2=(FUNC)I_GET(u_stack->b2.term)))) {
			/*
			 *	matching functors, so recurse
			 */
			(u_stack--)->next = f1->arr_size-1;
			CHECK_U_STACKS;
			u_stack->b1.subst = (u_stack+1)->b1.subst;
			u_stack->b2.subst = (u_stack+1)->b2.subst;
			u_stack->b1.term = f1->arr[f1->arr_size-1];
			u_stack->b2.term = f2->arr[f1->arr_size-1];
			continue;
		    } else break;
		if(vt1=='r') {
		    r1=(double *)(I_GET(u_stack->b1.term));
		    r2=(double *)(I_GET(u_stack->b2.term));
		    if(*r1 != *r2) break;
		}
		else if(I_GET(u_stack->b1.term) != I_GET(u_stack->b2.term))
		    break;
		else
		/*
		 *	matching non-functors (constant, int, ...)
		 */
		;
	    }
	    /*
	     *	success at current place, so find next place to match
	     */
	    for(;;) {
	        if (++u_stack >= u_stack_start) {
		    /*
		     *	no parent, so unification complete 
		     */
		    *r_stack = (BIND)NULL;
		    *reset=r_stack+2l;
		    /* {char *rechar=(char *)*reset;
		     ALIGN8(rechar);
		    } */
		    return(TRUE);
	        }
	        if ( --(u_stack->next) )
		    break;
	    }
	    /*
	     *	arguments remain to be unified in parent
	     */
	    f1=(FUNC)I_GET(u_stack->b1.term);
	    f2=(FUNC)I_GET(u_stack->b2.term);
	    next = u_stack->next;
	    (--u_stack)->b1.term = f1->arr[next];
	    u_stack->b2.term = f2->arr[next];
	    u_stack->b1.subst = (u_stack+1)->b1.subst;
	    u_stack->b2.subst = (u_stack+1)->b2.subst;
	}
	/*
	 *	ununifiable
	 */
	*r_stack=(BIND)NULL;
	return(FALSE);
}


/* i_debug(point,env) - Prolog debugging
 */

STRING points[] = {
	"Call",
	"Done",
	"Fail",
	"Retry"
};

PREDICATE
plg_debug(point,env)
	LONG point;
	CALL_ENV env;
	{
	static PREDICATE skipping=FALSE;
	static CALL_ENV env1;
	ITEM goal=HOF(env->atom);
	PREDICATE result=TRUE;
	pdebug=TRUE;
	if(goal->item_type=='h' &&(LONG)I_GET(goal)==pcut)
	    return(result);
	if(skipping && (point==DONE||point==FAIL) && env==env1)
	    skipping=FALSE;
	if(!skipping) {
	  if(breaks) printf("Break%d>",breaks);
	  printf("(%ld) %s: ",plg_depth(env),points[point]);
	  p_fwritesub(ttyout,goal,env->parent->subst);
	  printf(" ");
	  switch(ttychline()) {
	    case 'a':
	      result=FALSE;
	      break;
	    case 'l':
	      pdebug=FALSE;
	      break;
	    case 's':
	      if(point==CALL) {
	        skipping=TRUE;
	        env1=env;
	      }
	      break;
	    case 'n':
	      UNSET(tracing);
	      pdebug=FALSE;
	      break;
	    default:
	      break;
	  }
	}
	return(result);
}

/* plg_depth(env) - returns the depth of the given calling environment
 *	by climbing back through its ancestors.
 */

LONG
plg_depth(env)
	CALL_ENV env;
	{
	register LONG result;
	register CALL_ENV env1;
	for(result= -2l,env1=env;env1;result++,env1=env1->parent);
	return(result);
}

showstack(es,es0)
	CALL_ENV es,es0;
	{
	register CALL_ENV gstack;
	for(gstack=es;gstack<=es0;gstack++) {
	  cl_fwrite(ttyout,F_ELEM(0l,gstack->clause),gstack->subst);
	  i_fnl(ttyout);
	}
}

LONG
rchoose(matches)
	ITEM matches;
	{
	register LONG sum=0l,sofar=0l,cno;
	double rnum,rsum,rsofar;
	BIT_DO(cno,matches)
	  sum+=LABEL(cno);
	BIT_END
	rsum=sum; rnum=RAND;
	BIT_DO(cno,matches)
	  sofar+=LABEL(cno);
	  rsofar=sofar;
	  if((rsofar/rsum)>=rnum) return(cno);
	BIT_END
	return(I_ERROR);
}
