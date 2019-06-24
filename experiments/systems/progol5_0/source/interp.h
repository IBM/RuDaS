/* ####################################################################### */

/*              PROGOL Include File for PROLOG interpreter		   */
/*              -----------------------------------------                  */

/* ####################################################################### */

#define	STACK_SIZE 1000000	/*  assumed max. no. of vars to reset  */
typedef  struct	unify_stack  *UNIFY_STACK;
typedef  struct	call_env  *CALL_ENV;
typedef  struct	binding  BIND_OBJ;
typedef  struct	binding  *BIND;
typedef  struct	break_env  BREAK;

struct binding {
	struct binding  *subst;	/*   where to find subst. for this term */
	ITEM	term;		/*   where answer should go   */
};

struct call_env {
	struct call_env	*parent;
	ITEM	clause;		/*  procedure for this                   */
	LIST	atom;		/*  in clause of parent                  */
	LIST	next_call;	/*  unsatisfied subgoals                 */
	LIST	next_clause;	/*  where to find next clause on fail    */
	char	*term_stack;	/*  where to restart term_stack on fail  */
	BIND	subst;		/*  for vars. in clause                  */
	BIND	*reset;		/*  bindings to undo on fail             */
	LONG	callno;		/*  for recall no. of builtins		 */
	LONG	extra;		/*  extra 4 bytes related to call 	 */
};

struct unify_stack {
	struct	binding   b1,b2;	/*  two terms */
	int	next;			/*  next argument to match */
};

struct break_env {
	ITEM clause;
	PREDICATE reinterp,retrying;
	char *next_term_stack,*term_stack0;
	CALL_ENV parent,root_env,save_env,env_stack0,env_stack;
	ITEM built_terms;
	LONG nres;
};

#define	FUNC_ARG(it,arg) ((((FUNC)I_GET(it))->arr)[arg])	
#define	FUNC_ZERO(it)	 ((((FUNC)I_GET(it))->arr)[0])	
#define	FUNC_SIZE(it)	 (((FUNC)I_GET(it))->arr_size)	

#define	BOUND(b)	((b).subst + (LONG)I_GET((b).term))
#define HEAD(cl)	HOF((LIST)I_GET(FUNC_ZERO(cl)))
#define TAIL(cl)	TOF((LIST)I_GET(FUNC_ZERO(cl)))
#define CLAUSE(cl)	(LIST)I_GET(FUNC_ZERO(cl))
#define VAR_ONE(cl)	0l
#define VAR_NO(cl)	(LONG)I_GET(FUNC_ARG(cl,1))

#define SKIPVARS(t,s)	{register BIND s1; while((t)->item_type=='v'&&\
	(s1=(s)+(LONG)I_GET(t))->term){(t)=s1->term;(s)=s1->subst;}}
#define	PDEF(d,a,s) {register LONG psym=PSYM(a);register ITEM first;\
	 if(*(d=f_ins(psym,ptab))&&b_memq(psym,indexed)&&(first=FIRSTARG(a))){\
	 SKIPVARS(first,s);if(first->item_type!= 'r')PDEF1(d,psym,first)}}
#define PDEF1(d,p,f) {register LONG pf;if(*(d=f_ins(p,ptab))&& \
	b_memq(p,indexed)){if(f->item_type!= 'v' && \
	(((pf=pf_ins(p,PSYM(f),FALSE))==XYTERM)|| !(*(d=f_ins(pf,pftab))))) \
	d=f_ins(p,pvtab);}}

#define CONTRA {g_message(2l,"Testing for contradictions"); \
		if(d_groundcall(fhead)) \
		g_message(2l,"WARNING: contradiction - false is provable"); \
		else g_message(2l,"No contradictions found");}

