/* ####################################################################### */

/*                      PROGOL predefined predicates			   */
/*                      ---------------------------			   */

/* ####################################################################### */

#include        <stdio.h>
#include        "progol.h"

struct libstruc {
	STRING pattern;
	LONG nargs;
	PREDICATE repeat;
	PREDICATE (*func) ();
	STRING helpmsg;
};

extern struct libstruc clib[];
extern STRING plib[];
extern PREDICATE generate;

/* l_init - initialise the Progol built-in predicate library.
 *	Initialises 'psymtlib', 'lib' and 'ptab'.
 */

l_init()
	{
	struct libstruc *cptr;
	LONG cnt1,cnt2,psym,argn;
	ITEM atom,cclause,*fptr,*entry,clause,lib2=B_EMPTY,pseen=B_EMPTY;
	FUNC f;
	STRING *scl;
	/* Load built-in C-library */
	for(cptr=clib,cnt1=0l;cptr->pattern;cptr++,cnt1++) {
	  psym=QP_ston(cptr->pattern,argn=cptr->nargs);
	  b_add(psym,allpsyms);
	  *y_ins(psym,psymtlib)=cnt1;
	  b_add(psym,lib2);
	  if(!(cptr->func)) continue;
	  b_add(psym,lib);
	  if(cptr->repeat) b_add(psym,repeats);
	  if(argn) {
	    atom=i_create('f',f=f_create(argn+1l));
	    FNAME(f)=i_create('h',(POINTER)psym); cnt2=0l;
	    ARG_LOOP(fptr,f) *fptr=i_create('v',(POINTER)cnt2++);
	  }
	  else atom=i_create('h',(POINTER)psym);
	  cclause=i_tup4(i_dec(l_push(i_dec(atom),L_EMPTY)),
		i_dec(I_INT(argn)),i_dec(i_create('h',pdot0)),NULL);
	  if(!(*(entry=f_ins(psym,ptab)))) *entry=L_EMPTY;
	  l_push(i_dec(cclause),*entry);
	  b_add(b_num(cclause,spcls),bclauses);
	  if(!(*(entry=f_ins(psym,bptab)))) *entry=B_EMPTY;
	  b_add(cclause->extra,*entry);
	}
	/* Load built-in Prolog library */
	for(scl=plib;**scl;scl++) {
	  clause=ccl_sread(*scl);
	  cl_assert(clause,FALSE,TRUE,FALSE,TRUE,FALSE,pseen);
	  i_delete(clause);
	}
	cl_psfirstarg(pseen);
	b_uni(lib1,b_uni(lib2,lib));
	i_deletes(lib2,pseen,(ITEM)I_TERM);
}

#define MAXARGS	10l

PREDICATE
l_interp(goal,subin,subout,callno,built,break1)
	ITEM goal,built;
	BIND subin,subout;
	BREAK *break1;
	LONG callno;
	{
	register LONG psym,libno;
	register FUNC f;
	ITEM args[MAXARGS],*arg,*fptr;
	BIND subs[MAXARGS],*sub;
	psym=PSYM(goal);
	if(callno && !b_memq(psym,repeats)) return(FALSE);
	if(goal->item_type=='f') {
	    f=(FUNC)I_GET(goal);
	    arg=args; sub=subs;
	    ARG_LOOP(fptr,f) {
		*arg = *fptr; *sub = subin;
		SKIPVARS(*arg,*sub);
		arg++; sub++;
	    }
	}
	libno= *y_ins(psym,psymtlib);
	return((*((clib+libno)->func))(args,subs,subout,
					callno,built,break1));
}

#define LIB(x)		PREDICATE x(args,subsin,subout,callno,built,break1) \
			ITEM args[],built; BIND subsin[],subout; \
			LONG callno; BREAK *break1;

/* c_newframe - expands the stack with new frame to accomodate variables
 *	in given term. Returns new stack frame.
 */

BIND
c_newframe(term,break1)
	ITEM term;
	BREAK* break1;
	{
	BIND sub;
	LONG cnt,arity;
	if(!(arity=ct_vmax(term)+1l)) sub=(BIND)NULL;
	else if((char *)(break1->env_stack)<=
	  (char *)((sub=(BIND)(break1->next_term_stack))+arity))
		d_error("c_newframe - stack overflow");
	else {
	  sub=(BIND)(break1->next_term_stack);
	  for(cnt=0l;cnt<arity;cnt++) (sub+cnt)->term=(ITEM)NULL;
	  break1->next_term_stack= /* Set up new variable frame */
	    (char*)(break1->env_stack->reset=(BIND*)(sub+arity));
	}
	return(sub);
}

LIB(l_asserta) {
	PREDICATE result;
	ITEM arg0=p_copy(args[0l],subsin[0l],TRUE),cl,ccl;
	LONG vno;
	cl=l_pctol(arg0); cl_vrenum(cl,&vno);
	ccl=i_tup4(cl,i_dec(I_INT(cl_vmax(cl))),idot0,i_dec(F_EMPTY));
	result=cl_assert(ccl,FALSE,FALSE,TRUE,FALSE,TRUE,NULL);
	i_deletes(arg0,cl,ccl,(ITEM)I_TERM);
	return(result);
}

LIST
l_cln(atomic,lccls)
	PREDICATE atomic;
	LIST lccls;
	{
	register ITEM ccl;
	register LIST elem;
	LIST_LOOP(elem,lccls) {
	  ccl=L_GET(elem);
	  if(b_memq(ccl->extra,bclauses)&&(!atomic||
		(l_length(F_ELEM(0l,ccl))==1l))) return(elem);
	}
	return((LIST)NULL);
}

LIB(l_check1) {
	PREDICATE result=FALSE;
	result=(reducing&& !b_memq(PSYM(args[0l]),lib1));
	return(result);
}

LIB(l_clause) {
	PREDICATE result=FALSE,atomic;
	ITEM arg0,arg1,*def,ccl,head,cl,first;
	char itype;
	LONG cno,cnt,arity;
	BIND s=subsin[0l],sub;
	LIST lccls,elem;
	if((itype=(arg0=args[0l])->item_type)=='f'||itype=='h') {
	  PDEF(def,arg0,s);
	  if(!b_memq(PSYM(arg0),lib1)&&*def){
	    atomic=(((arg1=args[1l])->item_type)== 'h' &&PSYM(arg1)==ptrue);
	    if(!callno) lccls=(LIST)I_GET(*def);
	    else lccls=(LIST)break1->env_stack->extra;
	    if(elem=l_cln(atomic,lccls)) {
	      break1->env_stack->extra=(LONG)(elem->next);
	      ccl=L_GET(elem);
	      sub=(BIND)(break1->next_term_stack);
	      arity=(LONG)I_GET(F_ELEM(1l,ccl));
	      break1->next_term_stack= /* Set up new variable frame */
		(char*)(break1->env_stack->reset=(BIND*)(sub+arity));
	      for(cnt=0l;cnt<arity;cnt++) {
		(sub+cnt)->term=(ITEM)NULL;
		(sub+cnt)->subst=(BIND)NULL;
	      }
	      head=l_pop(cl=F_ELEM(0l,ccl));
	      subout->term=head; subout->subst=sub;
	      if(L_EMPTYQ(cl))
		  (subout+1l)->term=i_create('h',(POINTER)ptrue);
	      else (subout+1l)->term=l_ltopc(cl);
	      (subout+1l)->subst=sub;
	      (subout+2l)->term=I_INT(ccl->extra);
	      (subout+2l)->subst=(BIND)NULL;
	      l_push(i_dec(head),cl);
	      l_push(i_dec((subout+1l)->term),built);
	      l_push(i_dec((subout+2l)->term),built);
	      result=TRUE;
	    }
	  }
	}
	return(result);
}

LIB(l_constant) {
	register PREDICATE result=FALSE;
	char itype;
	if((itype=(args[0l]->item_type))=='h'||itype=='i') result=TRUE;
	return(result);
}

LIB(l_determination) {
	STRING name;
	LONG arity,p,p1;
	if((p=fsym(args[0l],&name,&arity,subsin[0l]))==(LONG)I_ERROR ||
		(p1=fsym(args[1l],&name,&arity,subsin[1l]))==(LONG)I_ERROR) {
	    printf("[Command should have form determination(P1/A1,P2/A2)]\n");
	    return(TRUE);
	}
	cl_ddeclare(p,p1);
	return(TRUE);
}

LIB(l_edit) {
	LONG arity,psym;
	STRING name;
	ITEM command=args[0l],clause,*clauses;
	FILEREC *out;
	char mess[MAXMESS],fname[MAXMESS];
	PREDICATE recon,ms;
	if(fsym(command,&name,&arity,subsin[0l])==(LONG)I_ERROR) {
	    printf("[Command should have form edit(Pred/Arity)]\n");
	    return(FALSE);
	}
	psym=QP_ston(name,arity);
	if(b_memq(psym,lib1)) {	/* Library predicate */
	    printf("[Cannot edit library predicate %s/%d]\n",name,arity);
	    return(FALSE);
	}
	sprintf(fname,"/tmp/pgltmpXXXXXX");
	mkstemp(fname);
	if(!(out=frecopen(fname,"w"))) {
	  printf("[Cannot open file for editing %s/%d]\n",name,arity);
	  return(FALSE);
	}
	clauses=f_ins(psym,ptab);
	if(*clauses) {
	  if(*clauses)
	    LIST_DO(clause,*clauses)
		ccl_fwrite(out,clause,NULL);
		i_fnl(out);
	    LIST_END
	}
	freclose(out);
	sprintf(mess,"%s %s",editor,fname);
	system(mess);
	recon=reconsult; reconsult=TRUE;
	ms=mseen; mseen=FALSE;
	cl_readrls(fname);
	reconsult=recon; mseen=ms;
	sprintf(mess,"rm -f %s",fname);
	system(mess);
	return(TRUE);
}

LIB(l_permute) {
        ITEM fname=args[0];
        STRING fileI;
        char fileO[MAXMESS];
        if(fname->item_type== 'h')
                fileI=QP_ntos((STRING)I_GET(fname));
        else return(FALSE);
        s_permute(fileI,s_length(fileI),fileO,0l);
        g_message(1l,"Results in <%s>",fileO);
        return(TRUE);
}

LIB(l_plt) {
	PREDICATE result;
	ITEM arg0=p_copy(args[0l],subsin[0l],TRUE),
		arg1=p_copy(args[1l],subsin[1l],TRUE);
	namecmp=TRUE;
	result=(i_cmp(arg0,arg1)==LT);
	namecmp=FALSE;
	i_deletes(arg0,arg1,(ITEM)I_TERM);
	return(result);
}

LIB(l_equiv) {
	PREDICATE result;
	ITEM arg0=p_copy(args[0l],subsin[0l],TRUE),
		arg1=p_copy(args[1l],subsin[1l],TRUE);
	namecmp=TRUE;
	result=(i_cmp(arg0,arg1)==EQ);
	namecmp=FALSE;
	i_deletes(arg0,arg1,(ITEM)I_TERM);
	return(result);
}

LIB(l_hypothesis) {
	PREDICATE result=FALSE;
	BIND sub;
	LONG arity,cnt;
	ITEM cl,head;
	if(hyp1) {
	  sub=(BIND)(break1->next_term_stack);
	  arity=(LONG)I_GET(F_ELEM(1l,hyp1));
	  break1->next_term_stack= /* Set up new variable frame */
		(char*)(break1->env_stack->reset=(BIND*)(sub+arity));
	  for(cnt=0l;cnt<arity;cnt++) {
	    (sub+cnt)->term=(ITEM)NULL;
	    (sub+cnt)->subst=(BIND)NULL;
	  }
	  head=l_pop(cl=F_ELEM(0l,hyp1));
	  subout->term=head; subout->subst=sub;
	  if(L_EMPTYQ(cl))
	    (subout+1l)->term=i_create('h',(POINTER)ptrue);
	  else (subout+1l)->term=l_ltopc(cl);
	  (subout+1l)->subst=sub;
	  (subout+2l)->term=I_INT(hyp1->extra);
	  (subout+2l)->subst=(BIND)NULL;
	  l_push(i_dec(head),cl);
	  l_push(i_dec((subout+1l)->term),built);
	  l_push(i_dec((subout+2l)->term),built);
	  result=TRUE;
	}
	return(result);
}

/*
LIB(l_localtime) {
	PREDICATE result=FALSE;
	struct tm *time;
	LONG tsec,m;
	ITEM trep;
	char mess[MAXMESS];
	if(args[0l]->item_type == 'i') {
	  tsec=(LONG)I_GET(args[0l]);
	  time=localtime(&tsec);
	  sprintf(mess,"%d-%d-%d-%d-%d-%d",
		time->tm_year+1900l,
		time->tm_mon+1l,
		time->tm_mday,
		time->tm_hour,
		time->tm_min,
		time->tm_sec);
	  trep=exp_sread(mess);
	  (subout+1l)->term=trep;
	  l_push(i_dec((subout+1l)->term),built);
	  result=TRUE;
	}
	else if(args[1l]->item_type == 'f' && PSYM(args[1l])==pminus2) {
	  struct timeval t;
	  struct timezone tz;
	  gettimeofday(&t,&tz);
	  time=localtime((time_t)&t);
	  p_swritesub(mess,args[1l],subsin[1l]);
	  if((m=sscanf(mess,"%u-%u-%u-%u-%u-%u",
		&(time->tm_year),&(time->tm_mon),&(time->tm_mday),
		&(time->tm_hour),&(time->tm_min),&(time->tm_sec)))==6l) {
	    time->tm_year-=1900l;
	    time->tm_mon-=1l;
	    tsec=timelocal(time);
	    subout->term=I_INT(tsec);
	    l_push(i_dec(subout->term),built);
	    result=TRUE;
	  }
	  else g_message(0l,"error at field %d of second argument",m+1l);
	}
	return(result);
}
*/

LIB(l_lt) {
	register PREDICATE result=FALSE;
	register char arg0,arg1;
	register LONG s,c2,i0,i1,cno;
	register ITEM a0=args[0l],a1=args[1l],*e1,*e2;
	register double f,*fp;
	arg0=a0->item_type; arg1=a1->item_type;
	if(callno&& !generate) result=FALSE;
	else if ((arg0== 'i' ||arg0== 'r')&&(arg1== 'i' ||arg1== 'r')) {
	  if(!callno) result=(i_cmp(a0,a1)<EQ);
	}
	else if ((arg0== 'k'&&(arg1== 'i' ||arg1== 'r' ||arg1== 'k'))||
		 (arg1== 'k'&&(arg0== 'i' ||arg0== 'r'))) {
	  if((cno=catoms->extra)&&(*(e1=f_ins(cno,clt)))&&
		(*(e2=f_ins(b_num(a0,sple),*e1))))
	  result=b_memq(b_num(a1,sple),*e2);
	}
	else switch(arg0) {
	  case 'i':
	    switch(arg1) {
	      case 'v':
	        i1=(LONG)I_GET(a0)+callno+1l;
	        (subout+1l)->term=I_INT(i1);
	        l_push(i_dec((subout+1l)->term),built);
	        result=TRUE;
	        break;
	      default: break;
	    }
	    break;
	  case 'r':
	    switch(arg1) {
	      case 'v':
	        fp=(double *)I_GET(a0);
	        f=(*fp)+(1.0/c_fraction(callno+1l)-1.0);
	        (subout+1l)->term=i_create('r',(POINTER)r_create(f));
	        l_push(i_dec((subout+1l)->term),built);
	        result=TRUE;
	        break;
	      default: break;
	    }
	    break;
	  case 'v':
	    switch(arg1) {
	      case 'i':
	        i1=(LONG)I_GET(a1)-callno-1l;
	        subout->term=I_INT(i1);
	        l_push(i_dec(subout->term),built);
	        result=TRUE;
	        break;
	      case 'r':
	        fp=(double *)I_GET(a1);
	        f=(*fp)-(1.0/c_fraction(callno+1l)-1.0);
	        subout->term=i_create('r',(POINTER)r_create(f));
	        l_push(i_dec(subout->term),built);
	        result=TRUE;
	        break;
	      case 'v':
	        s=isqrt(c2=callno<<1l); i0=callno-((s*(s+1))>>1l);
	        if(i0<0) {i1=s; i0+=s;} else i1=s+1l;
	        subout->term=I_INT(i0);
	        (subout+1l)->term=I_INT(i1);
	        l_push(i_dec(subout->term),built);
	        l_push(i_dec((subout+1l)->term),built);
	        result=TRUE;
	        break;
	      default: break;
	    }
	    break;
	  case 'k':
	    break;
	  default: break;
	}
	return(result);
}

LIB(l_le) {
	register PREDICATE result=FALSE;
	register char arg0,arg1;
	register LONG s,c2,i0,i1,cno;
	register ITEM a0=args[0l],a1=args[1l],*e1,*e2;
	register double f,*fp;
	arg0=a0->item_type; arg1=a1->item_type;
	if(callno&& !generate) result=FALSE;
	else if ((arg0== 'i' ||arg0== 'r')&&(arg1== 'i' ||arg1== 'r')) {
	  if(!callno) result=(i_cmp(a0,a1)<=EQ);
	}
	else if ((arg0== 'k'&&(arg1== 'i' ||arg1== 'r' ||arg1== 'k'))||
		 (arg1== 'k'&&(arg0== 'i' ||arg0== 'r'))) {
	  if((cno=catoms->extra)&&(*(e1=f_ins(cno,cle)))&&
		(*(e2=f_ins(b_num(a0,sple),*e1))))
	  result=b_memq(b_num(a1,sple),*e2);
	}
	else switch(arg0) {
	  case 'i':
	    switch(arg1) {
	      case 'v':
	        i1=(LONG)I_GET(a0)+callno;
	        (subout+1l)->term=I_INT(i1);
	        l_push(i_dec((subout+1l)->term),built);
	        result=TRUE;
	        break;
	      default: break;
	    }
	    break;
	  case 'r':
	    switch(arg1) {
	      case 'v':
	        fp=(double *)I_GET(a0);
		if(!callno) f= *fp;
	        else f=(*fp)+(1.0/c_fraction(callno)-1.0);
	        (subout+1l)->term=i_create('r',(POINTER)r_create(f));
	        l_push(i_dec((subout+1l)->term),built);
	        result=TRUE;
	        break;
	      default: break;
	    }
	    break;
	  case 'v':
	    switch(arg1) {
	      case 'i':
	        i1=(LONG)I_GET(a1)-callno;
	        subout->term=I_INT(i1);
	        l_push(i_dec(subout->term),built);
	        result=TRUE;
	        break;
	      case 'r':
	        fp=(double *)I_GET(a1);
		if(!callno) f= *fp;
	        else f=(*fp)-(1.0/c_fraction(callno)-1.0);
	        subout->term=i_create('r',(POINTER)r_create(f));
	        l_push(i_dec(subout->term),built);
	        result=TRUE;
	        break;
	      case 'v':
	        s=isqrt(c2=callno<<1l); i0=callno-((s*(s+1))>>1l);
	        if(i0<0) {i1=s-1l; i0+=s;} else i1=s;
	        subout->term=I_INT(i0);
	        (subout+1l)->term=I_INT(i1);
	        l_push(i_dec(subout->term),built);
	        l_push(i_dec((subout+1l)->term),built);
	        result=TRUE;
	        break;
	      default: break;
	    }
	    break;
	  case 'k':
	    break;
	  default: break;
	}
	return(result);
}

LIB(l_name) {
	char mess[MAXMESS];
	PREDICATE result=FALSE;
	register char *c,type0; register LONG n;
	ITEM arg0,arg1,l,num;
	double *fp;
	if((type0=(arg0=args[0l])->item_type)== 'h'||
		type0=='i'||type0=='r') {
	  if(type0=='h') strcpy(mess,QP_ntos((LONG)I_GET(arg0)));
	  else if(type0=='i') sprintf(mess,"%d",(LONG)I_GET(arg0));
	  else {fp=(double *)I_GET(arg0); sprintf(mess,"%.3f",*fp);}
	  l=L_EMPTY;
	  STR_LOOP(c,mess) l_suf(i_dec(I_INT(n= *c)),l);
	  (subout+1l)->term=l_ltop(l);
	  l_push(i_dec((subout+1l)->term),built);
	  i_delete(l);
	  result=TRUE;
	}
	else if((arg1=args[1l])->item_type== 'f') {
	  if(l=l_ptol(arg1=p_copy(args[1l],subsin[1l],FALSE))) {
	    c=mess; result=TRUE;
	    LIST_DO(num,l)
	      if(num->item_type=='i'&&
		0<=(n=(LONG)I_GET(num))&&n<=255l) *c++ =n;
	      else result=FALSE;
	    LIST_END
	    *c= '\0';
	    if(number(mess)) {
	      *c++ = ' '; *c= '\0';
	      if(!(subout->term=exp_sread(mess))) result=FALSE;
	    }
	    else {
		subout->term=i_create('h',QP_ston(mess,0l));
		subout->subst=(BIND)NULL;
	    }
	    l_push(i_dec(subout->term),built);
	  }
	  i_deletes(l,arg1,(ITEM)I_TERM);
	}
	return(result);
}

LIB(l_int) {
	register PREDICATE result=FALSE;
	register ITEM arg,ans;
	LONG sign,p1;
	if((arg=args[0])->item_type=='i')
	  if(callno) FALSE;
	  else result=TRUE;
	else if (arg->item_type=='v') {
	  sign=(++callno&1l?-1l:1l);
	  ans=subout->term=I_INT(sign*(callno>>1l));
	  subout->subst=(BIND)NULL;
	  l_push(i_dec(ans),built);
	  result=TRUE;
	}
	return(result);
}

LIB(l_modeb) {
	ITEM det=args[0l],pred=args[1l];
	PREDICATE result=cl_mdeclare(det,pred,bmodes);
	return(result);
}

LIB(l_modeh) {
	ITEM det=args[0l],pred=args[1l];
	PREDICATE result=cl_mdeclare(det,pred,hmodes);
	return(result);
}

LIB(l_modes) {
	printf("Head modes\n");
	l_modes1(hmodes);
	printf("Body modes\n");
	l_modes1(bmodes);
	return(TRUE);
}

l_modes1(modes)
	ITEM modes;
	{
	ITEM *rec,mdec,t,io,atom;
	BIND sub;
	FUNC_DO(rec,modes)
	  if(*rec) LIST_DO(mdec,*rec)
	    atom=HOF(TOF((LIST)I_GET(F_ELEM(0l,F_ELEM(0l,mdec)))));
	    io=F_ELEM(1l,mdec); sub=(BIND)stack;
	    LIST_DO(t,io)
	      sub->subst=(BIND)NULL;
	      (sub++)->term=t;
	    LIST_END
	    printf("  mode(%d,",(LONG)I_GET(F_ELEM(2l,mdec)));
	    p_fwritesub(ttyout,atom,(BIND)stack);
	    printf(")\n");
	  LIST_END
	FUNC_END
}

LIB(l_nat) {
	register PREDICATE result=FALSE;
	register ITEM arg;
	if(callno&& !generate) result=FALSE;
	else if((arg=args[0])->item_type=='i' && (LONG)I_GET(arg)>=0l)
	  if(callno) result=FALSE;
	  else result=TRUE;
	else if (arg->item_type=='v') {
	  if(callno<MAXN) subout->term=F_ELEM(callno+MAXN,nums);
	  else {
	  	subout->term=I_INT(callno);
		l_push(i_dec(subout->term),built);
	  }
	  subout->subst=(BIND)NULL;
	  result=TRUE;
	}
	return(result);
}

double s_nmsample();

LIB(l_normal) {
	PREDICATE result=TRUE;
	double x,mu,sg,*xp,*mup,*sgp;
	switch(args[0l]->item_type) {
	  case 'r': case 'i': case 'v':
	    break;
	  default:
	    result=FALSE;
	    break;
	}
	switch(args[1l]->item_type) {
	  case 'r':
            mup=I_FP(args[1l]);
	    break;
	  case 'i':
	    mu=(LONG)I_GET(args[1l]);
	    mup= &mu;
	    break;
	  default:
	    result=FALSE;
	    break;
	}
	switch(args[2l]->item_type) {
	  case 'r':
	    sgp=I_FP(args[2l]);
	    break;
	  case 'i':
	    sg=(LONG)I_GET(args[2l]);
	    sgp= &sg;
	    break;
	  default:
	    result=FALSE;
	    break;
	}
	if(result&&args[0l]->item_type=='v') {
	    x=s_nmsample(*mup,*sgp);
	    subout->term=i_create('r',r_create(x));
	    subout->subst=(BIND)NULL;
	    l_push(i_dec(subout->term),built);
	}
	return(result);
}

LIB(l_set1) {
	ITEM flag;
	STRING fname,fname1;
	struct setrec *fptr;
	PREDICATE result=FALSE;
	if((flag=args[0l])->item_type=='h') {
	  fname=QP_ntos((LONG)I_GET(flag));
	  if(STREQ(fname,"posonly")) {
	    noiselim=100.0;
	    inflate=400l;
	    g_message(2l,"Noise has been set to 100%%");
	    g_message(2l,"Example inflation has been set to 400%%");
	  }
	  for(fptr=settings;fname1=fptr->setname;fptr++)
	    if(fptr->settype==BOOLEAN && STREQ(fname,fname1)) {
	      PREDICATE *bptr=(PREDICATE *)fptr->setptr;
	      *bptr=TRUE;
	      result=TRUE;
	      break;
	    }
	}
	if(!result) {
	  printf("[Command should have form set(Flag) where Flag is one of:]\n");
	  for(fptr=settings;fname1=fptr->setname;fptr++) {
	    if(fptr->settype==BOOLEAN) printf("\t%s\n",fname1);
	  }
	}
	else printf("[The %s flag has been turned ON]\n", fname);
	return(result);
}

LIB(l_stage) {
	LONG pstage;
	switch(stage) {
	  case NOLEARN:
	    pstage=pnolearn0;
	    break;
	  case SATURATE:
	    pstage=psaturate0;
	    break;
	  case REFINE:
	    pstage=prefine0;
	    break;
	  default:
	    d_error("l_stage - bad learning stage");
	}
	subout->term=i_create('h',(POINTER)pstage);
	subout->subst=(BIND)NULL;
	l_push(i_dec(subout->term),built);
	return(TRUE);
}

LIB(l_system) {
	PREDICATE result=FALSE;
	if(args[0l]->item_type=='h') {
	  system(QP_ntos((LONG)I_GET(args[0l])));
	  result=TRUE;
	}
	return(result);
}

LIB(l_uniform) {
	PREDICATE result=TRUE,intq=FALSE;
	double x,lo,hi,*xp,*lop,*hip,r;
	LONG xint;
	switch(args[1l]->item_type) {
	  case 'r':
            lop=I_FP(args[1l]);
	    break;
	  case 'i':
	    lo=(LONG)I_GET(args[1l]);
	    lop= &lo;
	    intq=TRUE;
	    break;
	  default:
	    result=FALSE;
	    break;
	}
	switch(args[2l]->item_type) {
	  case 'r':
	    hip=I_FP(args[2l]);
	    intq&=FALSE;
	    break;
	  case 'i':
	    hi=(LONG)I_GET(args[2l]);
	    hip= &hi;
	    intq&=TRUE;
	    break;
	  default:
	    result=FALSE;
	    break;
	}
	if(result) {
	  switch(args[0l]->item_type) {
	    case 'r':
	      xp=I_FP(args[0l]);
	      result=(*lop<= *xp&& *xp<= *hip);
	      break;
	    case 'i':
	      x=(LONG)I_GET(args[0l]);
	      result=(*lop<=x&& x<= *hip);
	      break;
	    case 'v':
	      r=RAND;
	      if(intq) {
	        x= *lop+r*((*hip+1.0)- *lop);
		xint=x;
	        subout->term=I_INT(xint);
	      }
	      else {
	        x= *lop+r*(*hip- *lop);
	        subout->term=i_create('r',r_create(x));
	      }
	      l_push(i_dec(subout->term),built);
	      subout->subst=(BIND)NULL;
	      break;
	    default:
	      result=FALSE;
	      break;
	  }
	}
	return(result);
}


LIB(l_unset) {
	ITEM flag;
	STRING fname,fname1;
	struct setrec *fptr;
	PREDICATE result=FALSE;
	if((flag=args[0l])->item_type=='h') {
	  fname=QP_ntos((LONG)I_GET(flag));
	  for(fptr=settings;fname1=fptr->setname;fptr++)
	    if(fptr->settype==BOOLEAN && STREQ(fname,fname1)) {
	      PREDICATE *bptr=(PREDICATE *)fptr->setptr;
	      *bptr=FALSE;
	      result=TRUE;
	      break;
	    }
	}
	if(!result) {
	  printf("Command should have form set(Flag) where Flag is one of:\n");
	  for(fptr=settings;fname1=fptr->setname;fptr++) {
	    if(fptr->settype==BOOLEAN) printf("\t%s\n",fname1);
	  }
	}
	else printf("[The %s flag has been turned OFF]\n", fname);
	return(result);
}

LIB(l_set2) {
	LONG psym,ival;
	ITEM limit=args[0l],value=args[1l];
	struct setrec *fptr;
	PREDICATE result=FALSE;
	STRING fname,fname1;
	if(!(limit->item_type=='h' && value->item_type=='i' &&
	    (ival=(LONG)I_GET(value))>=0l))
		return(FALSE);
	psym=(LONG)I_GET(limit);
	fname1=QP_ntos(psym);
	for(fptr=settings;fname=fptr->setname;fptr++) {
	  if(STREQ(fname,fname1)) {
	    switch(fptr->settype) {
	      case BOOLEAN: {
	        PREDICATE *bptr=(PREDICATE *)fptr->setptr;
	        if(ival<=1l) *bptr=(PREDICATE)ival;
		result=TRUE;
	        }
	        break;
	      case NATURAL: {
	        LONG *iptr=(LONG *)fptr->setptr;
	        *iptr=(LONG)ival;
		result=TRUE;
	        }
	        break;
	      case REAL: {
	        double *rptr=(double *)fptr->setptr;
	        *rptr=(double)ival;
		result=TRUE;
	        }
	        break;
	      default:
		break;
	    }
	  }
	  if(result) break;
	}
	return(result);
}

LIB(l_settings) {
	struct setrec *sptr;
	STRING sname;
	PREDICATE *bptr;
	LONG *nptr,n,cwidth=0l;
	double *rptr;
	ITEM names=L_EMPTY;
	char mess[MAXMESS];
	for(sptr=settings;sname=sptr->setname;sptr++) {
	  switch(sptr->settype) {
	      case BOOLEAN:
		bptr=(PREDICATE *)sptr->setptr;
	        if(*bptr)
		  sprintf(mess,"%s=ON",sname);
	        else sprintf(mess,"%s=OFF",sname);
		break;
	      case NATURAL:
		nptr=(LONG *)sptr->setptr;
		sprintf(mess,"%s=%ld",sname,*nptr);
		break;
	      case REAL:
		rptr=(double *)sptr->setptr;
		sprintf(mess,"%s=%.0f",sname,*rptr);
		break;
	      default:
		d_error("l_settings - bad type");
		break;
	  }
	  if((n=strlen(mess))>cwidth) cwidth=n;
	  l_suf(i_dec(i_create('s',strsave(mess))),names);
	}
	l_showw(i_sort(names),cwidth);
	i_delete(names);
	return(TRUE);
}

extern ITEM store;

LIB(l_record) {
	ITEM arg0=args[0l],arg1=args[1l];
	PREDICATE result=TRUE;
	LONG a0;
	if(arg0->item_type!='i') result=FALSE;
	else {
	  a0=(LONG)I_GET(arg0);
	  if(a0==0l) {
	    if(!store) store=L_EMPTY;
	    l_suf(i_dec(p_copy(arg1,subsin[1l],TRUE)),store);
	  }
	  else if(a0==1l&&store) {
	    unsigned long int *vp;
	    LONG vmax=0l,vno,cnt;
	    ITEM term,plist=l_ltop(store),vnums=Y_EMPTY;
	    BIND sub=(BIND)(break1->next_term_stack);
	    TERM_DO(term,plist) /* Renumber variables */
	      if(term->item_type=='v') {
		if(!(*(vp=y_ins(vno=(LONG)I_GET(term),vnums)))) *vp= ++vmax;
		I_GET(term)= (POINTER)(*vp-1);
	      }
	    TERM_END
	    break1->next_term_stack= /* Set up new variable frame */
		(char*)(break1->env_stack->reset=(BIND*)(sub+vmax));
	    for(cnt=0l;cnt<vmax;cnt++) (sub+cnt)->term=(ITEM)NULL;
	    l_push(i_dec(plist),built);
	    i_deletes(store,vnums,(ITEM)I_TERM); store=(ITEM)NULL;
	    (subout+1l)->term=plist;
	    (subout+1l)->subst=sub;
	  }
	  else result=FALSE;
	}
	return(result);
}

LIB(l_sort) {
	ITEM arg0=args[0l],pl0,pl1,l;
	PREDICATE result=FALSE;
	if((arg0->item_type=='f'&&PSYM(arg0)==pdot2)||
		arg0->item_type=='h'&&PSYM(arg0)==pempty) {
	  if(l=l_ptol(pl0=p_copy(arg0,subsin[0l],TRUE))) {
	    i_psort(l);
	    (subout+1l)->term=pl1=l_ltop(l);
	    (subout+1l)->subst=subsin[0l];
	    l_push(i_dec(pl1),built);
	    result=TRUE;
	  }
	  i_deletes(pl0,l,(ITEM)I_TERM);
	}
	return(result);
}

LIB(l_stats) {
	a_pr_block_stats();
	return(TRUE);
}

LIB(l_tell) {
	register result=FALSE;
	if(plgout!=ttyout) frecdelete(plgout);
	if(args[0]->item_type=='h')
	  if(plgout=frecopen(QP_ntos((LONG)I_GET(args[0])),"w")) result=TRUE;
	  else plgout=ttyout;
	return(result);
}

LIB(l_sample3) {
	LONG arity;
	STRING name;
	ITEM command=args[0l],atoms,pl1;
	if(fsym(command,&name,&arity,subsin[0l])==(LONG)I_ERROR||
		args[1l]->item_type!='i') {
	    printf("[Command should have form sample(Pred/Arity,N,S)]\n");
	    return(FALSE);
	}
	else atoms=d_sample(QP_ston(name,arity),(LONG)I_GET(args[1l]));
	(subout+2l)->term=pl1=l_ltop(atoms);
	(subout+2l)->subst=NULL;
	l_push(i_dec(pl1),built);
	i_delete(atoms);
	return(TRUE);
}

LIB(l_reduce) {
	ITEM command=args[0l];
	LONG arity,psym;
	STRING name;
	if(fsym(command,&name,&arity,subsin[0l])==(LONG)I_ERROR) {
	    printf("[Command should have form reduce(Pred/Arity)]\n");
	    return(FALSE);
	}
	d_treduce(psym=QP_ston(name,arity));
	return(TRUE);
}

LIB(l_retract) {
	PREDICATE result=FALSE;
	ITEM arg0=args[0l],arg1=args[1l],def;
	register LIST elem,*elemp;
	char type0=arg0->item_type;
	LONG cno,psym;
	if((type0=='f'||type0=='h')&&arg1->item_type=='i'&&
		(def=F_ELEM(psym=PSYM(arg0),ptab))!=(ITEM)NULL) {
	  b_rem(cno=(LONG)I_GET(arg1),bclauses);
	  b_rem(cno,F_ELEM(psym,bptab));
	  elemp=L_LAST(def);
	  while(*elemp)
	    if(L_GET(*elemp)->extra==cno) {
	      elem= *elemp; *elemp=(*elemp)->next;
	      l_push(i_dec(L_GET(elem)),built); L_DEL(elem);
	    }
	    else elemp=&((*elemp)->next);
	  if(L_EMPTYQ(def)) {
	    i_delete(def);
	    def=F_ELEM(PSYM(arg0),ptab)=(ITEM)NULL;
	  }
	  else SUFLIST(def);
	  if(def) cl_pfirstarg(psym);
	  result=TRUE;
	}
	return(result);
}

LIB(l_nospy) {
	i_delete(spies);
	spies=B_EMPTY;
	return(TRUE);
}

PREDICATE seen_obs=FALSE;

LIB(l_observable) {
	LONG arity;
	STRING name;
	ITEM command=args[0l],bemp;
	if(fsym(command,&name,&arity,subsin[0l])==(LONG)I_ERROR) {
	    printf("[Command should have form obervable(Pred/Arity)]\n");
	    return(FALSE);
	}
	else {
	  if(!seen_obs) { /* modeh declared predicates are observables
	  			unless observables declared */
	    b_int(observables,bemp=B_EMPTY);
	    i_delete(bemp);
	    seen_obs=TRUE;
	  }
	  b_add(QP_ston(name,arity),observables);
	}
	return(TRUE);
}

LIB(l_op) {
	ITEM astring,*entry,sym,assoc,prec;
	char mess[MAXWORD];
	LONG psym,passoc;
	prec=args[0l]; assoc=args[1l]; sym=args[2l];
	if(sym->item_type!='h') return(FALSE);
	else if(assoc->item_type!='h') return(FALSE);
	else if(prec->item_type!='i') return(FALSE);
	b_add(psym=(LONG)I_GET(sym),op);
	passoc=(LONG)I_GET(assoc);
	astring=i_create('s',strsave(QP_ntos(passoc)));
	if(strlen((STRING)I_GET(astring))==2l) psym=QP_ston(QP_ntos(psym),1l);
	if(*(entry=f_ins(psym,ops))) {
	    printf("[Redeclaration of operator %s ignored]\n",QP_ntos(psym));
	    i_delete(astring);
	    return(TRUE);
	}
	*entry=i_tup2(i_dec(astring),prec);
	return(TRUE);
}

LIB(l_ops) {
	FUNC f=(FUNC)I_GET(ops);
	ITEM *fptr;
	LONG operator=0l;
	FUNC_LOOP(fptr,f) {
	  if(*fptr) {
	    printf("  op(%d,%s,'%s')\n",(LONG)I_GET(F_ELEM(1l,*fptr)),
		(STRING)I_GET(F_ELEM(0l,*fptr)),QP_ntos(operator));
	  }
	  operator++;
	}
	return(TRUE);
}

LIB(l_quit) {
	exit(1);
}

LIB(l_repeat) {
	return(TRUE);
}

/*
LIB(l_right) {
	register LONG n,p,p1,x,y,z,z2;
	ITEM primes;
	if(args[0]->item_type!='i') return(FALSE);
	primes=B_EMPTY;
	n=(LONG)I_GET(args[0]);
	for(p=3l;p<=n;p+=2l) {
	  BIT_DO(p1,primes)
	    if(p1*p1>p) goto prime;
	    if(!(p%p1)) goto notprime;
	  BIT_END
	  prime:
	  b_add(p,primes);
	  notprime:
	}
	printf("There are %d primes less than or equal to %d\n",
		b_size(primes)+2l,n);
	for(x=3l;x<=n;x++)
	  for(y=x;y<=n;y++) {
	    z=isqrt(z2=x*x+y*y);
	    if(z*z==z2&&(b_memq(x,primes)||
			b_memq(y,primes)||b_memq(z,primes)))
		printf("%d %d %d\n",x,y,z);
	  }
	i_delete(primes);
	return(TRUE);
}
*/

LIB(l_see) {
	register result=FALSE;
	if(plgin!=ttyin) {freclose(plgin); plgin=ttyin;}
	if(args[0]->item_type=='h')
	  if(plgin=frecopen(QP_ntos((LONG)I_GET(args[0])),"r")) result=TRUE;
	  else plgin=ttyin;
	return(result);
}

LIB(l_spies) {
	printf("Spypoints are:\n");
	l_showp(spies);
	return(TRUE);
}

LIB(l_size) {
	printf("Size of int: %d bytes\n",sizeof(int));
	printf("Size of LONG: %d bytes\n",sizeof(LONG));
	printf("Size of float: %d bytes\n",sizeof(float));
	printf("Size of double: %d bytes\n",sizeof(double));
	return(TRUE);
}

LIB(l_spy) {
	LONG arity;
	STRING name;
	ITEM command=args[0l];
	if(fsym(command,&name,&arity,subsin[0l])==(LONG)I_ERROR) {
	    printf("[Command should have form spy(Pred/Arity)]\n");
	    return(FALSE);
	}
	else b_add(QP_ston(name,arity),spies);
	return(TRUE);
}

LIB(l_trace) {
	SET(tracing);
	return(TRUE);
}

LIB(l_notrace) {
	UNSET(tracing);
	return(TRUE);
}

LIB(l_told) {
	register result=FALSE;
	if(plgout!=ttyout) {
	  freclose(plgout);
	  plgout=ttyout;
	  result=TRUE;
	}
	return(result);
}

LIB(l_commutative) {
	ITEM command=args[0l];
	LONG arity;
	STRING name;
	if(fsym(command,&name,&arity,subsin[0l])==(LONG)I_ERROR) {
	    printf("[Command should have form commutative(Pred/Arity)]\n");
	    return(FALSE);
	}
	else b_add(QP_ston(name,arity),commutes);
	return(TRUE);
}

LIB(l_commutatives) {
	printf("Commutative predicates are:\n");
	l_showp(commutes);
	return(TRUE);
}

LIB(l_consult) {
	ITEM fname=args[0];
	STRING name;
	char file[MAXMESS];
	PREDICATE recon,result;
	if(fname->item_type== 'h')
		name=QP_ntos((STRING)I_GET(fname));
	else return(FALSE);
	if(STRSUFQ(name,".pl")) sprintf(file,"%s",name);
	else sprintf(file,"%s.pl",name);
	recon=reconsult; reconsult=FALSE;
	result=cl_readrls(file);
	reconsult=recon;
	return(result);
}

LIB(l_reconsult) {
	ITEM fname=args[0];
	STRING name;
	char file[MAXMESS];
	PREDICATE recon,ms,result;
	if(fname->item_type== 'h')
		name=QP_ntos((STRING)I_GET(fname));
	else return(FALSE);
	if(STRSUFQ(name,".pl")) sprintf(file,"%s",name);
	else sprintf(file,"%s.pl",name);
	recon=reconsult; reconsult=TRUE;
	ms=mseen; mseen=FALSE;
	result=cl_readrls(file);
	reconsult=recon; mseen=ms;
	return(result);
}

LIB(l_advise) {
	ITEM fname=args[0];
	STRING name;
	char file[MAXMESS];
	if(fname->item_type== 'h')
		name=QP_ntos((STRING)I_GET(fname));
	else return(FALSE);
	sprintf(file,"%s.pl",name);
	return(cl_writerls(file));
}

#define CWIDTH 35

LIB(l_help0) {
	printf("The following system predicates are available:\n");
	l_showp(lib1);
	printf("Help for system predicates using help(Predicate/Arity)\n");
	return(TRUE);
}

LIB(l_help1) {
	ITEM command=args[0l];
	struct libstruc *cptr;
	STRING name;
	LONG arity,psym;
	char mess[MAXMESS];
	if(fsym(command,&name,&arity,subsin[0l])==(LONG)I_ERROR) {
	    printf("[Command should have form help(Pred/Arity)]\n");
	    return(FALSE);
	}
	if (!b_memq(psym=QP_ston(name,arity),lib1)) {
	    predn(mess,name,arity);
	    printf("[Unknown system predicate - %s]\n",mess);
	    return(FALSE);
	}
	cptr=clib+((LONG)Y_ELEM(psym,psymtlib));
	predn(mess,cptr->pattern,cptr->nargs);
	printf("[%s - %s]\n",mess,cptr->helpmsg);
	return(TRUE);
}

LIB(l_label1) {
	ITEM clausen=args[0l];
	LONG cno;
	if(clausen->item_type!='i'||(cno=(LONG)I_GET(clausen))<0l)
	  return(FALSE);
	(*y_ins(cno,labels))++;
	return(TRUE);
}

LIB(l_label2) {
	ITEM clausen=args[0l],i_ans;
	LONG cno,ans;
	if(clausen->item_type!='i'||(cno=(LONG)I_GET(clausen))<0l)
	  return(FALSE);
	ans= LABEL(cno);
	i_ans=(subout+1l)->term=I_INT(ans);
	l_push(i_dec(i_ans),built);
	return(TRUE);
}

LIB(l_list0) {
	ITEM user=b_rem(pfalse0,b_sub(b_sub(b_copy(allpsyms),lib1),aux));
	LONG pno;
	BIT_DO(pno,user)
	  if(!F_ELEM(pno,ptab)) b_rem(pno,user);
	BIT_END
	printf("The following user predicates are defined:\n");
	l_showp(user);
	i_delete(user);
	return(TRUE);
}


LIB(l_list1) {
	ITEM command=args[0l];
	LONG arity,psym;
	STRING name;
	if(fsym(command,&name,&arity,subsin[0l])==(LONG)I_ERROR) {
	    printf("[Command should have form listing(Pred/Arity)]\n");
	    return(FALSE);
	}
	c_list1(psym=QP_ston(name,arity),TRUE);
	return(TRUE);
}

c_list1(psym,numq)
	LONG psym;
	PREDICATE numq;
	{
	ITEM *clauses,clause;
	LONG cnt=0l;
	if(b_memq(psym,lib))
	  printf("[No Prolog definition for library predicate %s/%d]\n",
		QP_ntos(psym),QP_ntoa(psym));
	else {
	  clauses=f_ins(psym,ptab);
	  if(!(*clauses))
	    printf("[Predicate %s/%d not defined]\n",
		QP_ntos(psym),QP_ntoa(psym));
	  else {
	    if(*clauses) {
	      LIST_DO(clause,*clauses)
	        if(b_memq(clause->extra,bclauses)) {
		  ccl_print(clause);
		  cnt++;
	        }
	      LIST_END
	      printf("\n");
	      if(numq)
		printf("[Total number of clauses = %d]\n",cnt);
	    }
	 }
	}
}

PREDICATE c_gen1();

LIB(l_gen1) {
	ITEM command=args[0l];
	STRING name;
	LONG arity,psym;
	PREDICATE oldsearchq=SETQ(searching),result;
	if(fsym(command,&name,&arity,subsin[0l])==(LONG)I_ERROR) {
	    printf("[Command should have form generalise(Pred/Arity)]\n");
	    return(FALSE);
	}
	SET(searching);
	result=c_gen1(psym=QP_ston(name,arity));
	if(oldsearchq) SET(searching);
	else UNSET(searching);
	return(result);
}


PREDICATE
c_gen1(psym)
	LONG psym;
	{
	char mess1[MAXMESS];
	PREDICATE result=FALSE;
	ITEM *clauses,bcl,bcl1,clause,ps;
	LONG cno,nex;
	ps=c_mdesp(psym);
		/* Descendent predicates with modeh declarations */
	if(b_memq(psym,lib1))
	  g_message(2l,"Cannot generalise library predicate %s/%d",
		QP_ntos(psym),QP_ntoa(psym));
	else if(!b_emptyq(ps) && (bcl= *f_ins(psym,bptab))) {
	  g_message(2l,"Generalising %s/%d",QP_ntos(psym),QP_ntoa(psym));
	  d_treduce(psym);	/* Remove redundancy first */
	  bcl1=b_copy(bcl= *f_ins(psym,bptab));
	  nex=bcl_costs(psym);
	  BIT_DO(cno,bcl1)
	    if(b_memq(cno,bclauses)) {
		ccl_swrite(mess1,clause=i_copy(i_dec(b_elem(cno,spcls))));
		if(!GRNDQ(clause)) {
		  i_delete(clause);
		  continue;
		}
		printf("[Generalising %s]\n",mess1);
		if(c_genc(clause,ps)) {
		  i_delete(ps);
		  ps=c_mdesp(psym);	/* Descendents may have changed */
		  result=TRUE;
		}
		i_deletes(clause,(ITEM)I_TERM);
	    }
	  BIT_END
	  i_delete(bcl1);
	  if(result) c_list1(psym);
	  printf("\n");
	}
	i_delete(ps);
	return(result);
}

#define MINLAYER	5l
#define	ERROR(p)	((*y_ins((p),fp))+(*y_ins((p),fn)))
#define	TOTAL(p)	(ERROR(p)+((*y_ins((p),tp))+(*y_ins((p),tn))))
#define	TP		0
#define	TN		1
#define	FP		2
#define	FN		3


LIB(l_layer) {
	ITEM fname=args[0l],cl,atom,olddef,oldbdef,exs,tp,tn,fp,fn,*e1,*e2,
		layer,ccl1,costs1,bcl1,pseen,pseen1,ccl,train,test;
	STRING name;
	char file[MAXMESS],mess[MAXMESS],mess1[MAXMESS];
	LONG psym,psym1,status,oldinf,oldcond,trn,tst;
	FILEREC *in;
	PREDICATE groundq,oldsearchq=SETQ(searching),recon,ms;
	double AP,aP,Ap,ap,start,start0;
	if(fname->item_type== 'h')
		name=QP_ntos((STRING)I_GET(fname));
	else return(FALSE);
	if(STRSUFQ(name,".pl")) sprintf(file,"%s",name);
	else sprintf(file,"%s.pl",name);
	if (!(in=frecopen(file,"r"))) {
		printf("[Cannot find %s]\n",file);
		return(FALSE);
	}
	tp=Y_EMPTY; tn=Y_EMPTY; fp=Y_EMPTY; fn=Y_EMPTY; layer=Y_EMPTY;
	train=Y_EMPTY; test=Y_EMPTY;
	olddef=F_EMPTY; oldbdef=F_EMPTY; exs=F_EMPTY;
	costs1=y_copy(costs);
	pseen=B_EMPTY; pseen1=B_EMPTY;
	SET(searching); recon=reconsult; reconsult=TRUE;
	ms=mseen; mseen=FALSE;
	start0=cputime();
	while((ccl=ccl_fread(in))!=(ITEM)I_TERM) {
	    if(ccl==(ITEM)I_ERROR) continue;
	    else if(CTYPE(ccl)==(POINTER)pdot0) {	/* Statement */
	      cl=F_ELEM(0l,ccl);
	      atom=HOF((LIST)I_GET(cl));
	      psym=PSYM(atom);
	      psym1=((psym==pfalse0 && l_length(cl)>1)?
			PSYM(HOF(TOF((LIST)I_GET(cl)))):psym);
	      if(!(*f_ins(psym1,hmodes))) {
	        cl_assert(ccl,FALSE,TRUE,FALSE,FALSE,TRUE,pseen);
		i_delete(ccl);
		continue;
	      }
	      if((groundq=((LONG)I_GET(F_ELEM(1l,ccl))==0l)) &&
			l_length(cl)==1l)
		/* Ground positive */
		status=(d_groundcall(atom)?TP:FP);
	      else if(groundq && psym==pfalse0 && l_length(cl)==2l) {
		/* Ground negative */
		atom=HOF(TOF((LIST)I_GET(cl)));
		status=(d_groundcall(atom)?FN:TN);
	      }
	      else {	/* Other types of clause */
		if(d_cimplied(ccl)) status=((psym==pfalse0)?FN:TP);
		else status=((psym==pfalse0)?TN:FP);
	      }
	      psym=psym1;
	      if(!b_memq(psym,pseen1)) {
		if(!(*(e1=f_ins(psym,ptab)))) *e1=L_EMPTY;
		*f_ins(psym,olddef)=l_copy(*e1);
		if(!(*(e1=f_ins(psym,bptab)))) *e1=B_EMPTY;
		*f_ins(psym,oldbdef)=b_copy(*e1);
		b_add(psym,pseen1);
	      }
	      (*y_ins(psym,test))++;
	      switch(status) {
		case TP:	/* True positive */
		  (*y_ins(psym,tp))++;
		  break;
		case TN:	/* True negative */
		  (*y_ins(psym,tn))++;
		  break;
		case FP:	/* False positive (omission error) */
		  (*y_ins(psym,fp))++;
		  break;
		case FN:	/* False negative (commission error) */
		  (*y_ins(psym,fn))++;
		  break;
		default: d_error("l_test - unknown status value");
	      }
	      if((status==FP)||(status==FN)) {	/* Store error */
		if(!(*(e1=f_ins(psym,exs)))) *e1=L_EMPTY;
		l_push(ccl,*e1);
		(*y_ins(psym,train))++;
	      }
	      if((posonly || ERROR(psym)>0l) && (TOTAL(psym)>=(MINLAYER<<
			*y_ins(psym,layer)))) { /* New layer */
		(*y_ins(psym,layer))++;
		g_message(1l,"LAYER %d, PREDICATE %s/%d",Y_ELEM(psym,layer),
			    QP_ntos(psym),QP_ntoa(psym));
	  	AP= *y_ins(psym,tp); aP= *y_ins(psym,fn);
	  	Ap= *y_ins(psym,fp); ap= *y_ins(psym,tn);
	  	c_chisq(AP,aP,Ap,ap);
		/* Reset to start state and generalise exceptions */
		Y_ELEM(psym,tp)=Y_ELEM(psym,tn)=
			Y_ELEM(psym,fp)=Y_ELEM(psym,fn)=0l;
		b_sub(bclauses,*f_ins(psym,bptab));
		i_deletes(*f_ins(psym,ptab),*f_ins(psym,bptab),
			costs,(ITEM)I_TERM);
	        *f_ins(psym,ptab)= l_copy(*f_ins(psym,olddef));
	        *f_ins(psym,bptab)= b_copy(*f_ins(psym,oldbdef));
		costs=y_copy(costs1); b_uni(bclauses,*f_ins(psym,oldbdef));
		if(!(*(e1=f_ins(psym,exs)))) *e1=L_EMPTY;
		LIST_DO(ccl1,*e1)
		      cl_assert(ccl1,FALSE,FALSE,TRUE,FALSE,TRUE,NULL);
		LIST_END
		start=cputime();
		if(posonly) {
		  oldinf=inflate; oldcond=condition;
		  UNSET(condition);
		  trn= *y_ins(psym,train); tst= *y_ins(psym,test);
		  inflate=oldinf*tst/(trn+1l);
		  i_delete(*(e1=f_ins(psym,psamps))); *e1=(ITEM)NULL;
		  i_delete(*(e1=f_ins(psym,bpsamps))); *e1=(ITEM)NULL;
		  c_gen1(psym);
		  inflate=oldinf; condition=oldcond;
		}
		else c_gen1(psym);
		sprintf(mess1,"%s - Time taken %.2lfs, Cum. time %.2fs",mess,
			fabs(cputime()-start), fabs(cputime()-start0));
		g_message(1l,mess1);
	      }
	    }
	    else if(CTYPE(ccl)==(POINTER)pquest) {	/* Headless clause */
		start=cputime();
		interp_quest(ccl);
		ccl_swrite(mess,ccl);
		sprintf(mess1,"%s - Time taken %.2lfs",mess,
			fabs(cputime()-start));
		g_message(1l,mess1);
		l_push(ccl,comms);
	    }
	    else {
	      printf("[Ignored statement:]");
	      ccl_print(ccl);
	    }
	    i_delete(ccl);
	}
	BIT_DO(psym,pseen1)
	  printf("\n[FINAL PREDICATE %s/%d]\n\n",QP_ntos(psym),QP_ntoa(psym));
	  AP= *y_ins(psym,tp); aP= *y_ins(psym,fn);
	  Ap= *y_ins(psym,fp); ap= *y_ins(psym,tn);
	  c_chisq(AP,aP,Ap,ap);
	  printf("TRAIN/TEST = %d/%d\n",*y_ins(psym,train),*y_ins(psym,test));
	BIT_END
	i_deletes(tp,tn,fp,fn,olddef,oldbdef,exs,(ITEM)I_TERM);
	i_deletes(costs1,layer,pseen,pseen1,train,test,(ITEM)I_TERM);
	freclose(in);
	reconsult=recon; mseen=ms;
	if(oldsearchq) SET(searching);
	else UNSET(searching);
	return(TRUE);
}

PREDICATE yates=TRUE;

extern DOUBLE gamma_func();

#define RIGHT	(AP+ap)
#define WRONG	(aP+Ap)
#define N	(RIGHT+WRONG)
#define P	(RIGHT/N)
#define Q	(1-P)
#define PACC	(P*100.0)
#define ERR	pow((P*Q)/N,0.5)
#define PERR	(ERR*100.0)
#define N1	(AP+aP)
#define N2	(Ap+ap)
#define N3	(AP+Ap)
#define N4	(aP+ap)
#define E1	((N1*N3)/N)
#define E2	((N1*N4)/N)
#define E3	((N2*N3)/N)
#define E4	((N2*N4)/N)
#define O1	(yates?(AP>E1?AP-0.5:(AP<E1?AP+0.5:AP)):AP)
#define O2	(yates?(aP>E2?aP-0.5:(aP<E2?aP+0.5:aP)):aP)
#define O3	(yates?(Ap>E3?Ap-0.5:(Ap<E3?Ap+0.5:Ap)):Ap)
#define O4	(yates?(ap>E4?ap-0.5:(ap<E4?ap+0.5:ap)):ap)
#define OE1	((O1-E1)*(O1-E1)/E1)
#define OE2	((O2-E2)*(O2-E2)/E2)
#define OE3	((O3-E3)*(O3-E3)/E3)
#define OE4	((O4-E4)*(O4-E4)/E4)
#define CHISQ	(OE1+OE2+OE3+OE4)
#define CHISQP	(gamma_func(0.5*((N3>0.0&&N4>0.0)?1.0:0.0),0.5*CHISQ))

LIB(l_leave) {
	ITEM command=args[0l],bpcls,bcls,cls,cls1,bred,one,bnegs,ccl;
	STRING name;
	LONG arity,psym,cno;
	double AP,aP,Ap,ap;
	char mess1[MAXMESS];
	PREDICATE oldsearchq=SETQ(searching),result=FALSE;
	AP=aP=Ap=ap=0.0;
	if((psym=fsym(command,&name,&arity,subsin[0l]))==(LONG)I_ERROR) {
	    printf("[Command should have form leave(Pred/Arity)]\n");
	    return(FALSE);
	}
	SET(searching);
	if(cls1= *f_ins(psym,ptab)) {
	  printf("[TESTING POSITIVES]\n");
	  cls=l_copy(cls1);
	  bpcls=b_copy(F_ELEM(psym,bptab));
	  bcls=b_copy(bclauses);
	  BIT_DO(cno,bpcls)
	      b_rem(cno,F_ELEM(psym,bptab)); b_rem(cno,bclauses);
	      b_lsub(F_ELEM(psym,ptab),one=b_add(cno,B_EMPTY));
c_list1(psym,TRUE);
	      c_gen1(psym);	/* Generalise w/o clause */
	      i_delete(F_ELEM(psym,bptab));
	      F_ELEM(psym,bptab)=one; /* Test clause implied */
	      ccl_swrite(mess1,i_dec(b_elem(cno,spcls)));
	      if(b_emptyq(bred=d_tredundant(psym))) {
		Ap+=1.0;
		printf("[Wrong: %s]\n",mess1);
	  	  printf("[Partial accuracy= %.f/%.f]\n",RIGHT,N);
	      }
	      else {
	        AP+=1.0;		/* Add up and reset state */
		printf("[Right: %s]\n",mess1);
	  	  printf("[Partial accuracy= %.f/%.f]\n",RIGHT,N);
	      }
	      i_deletes(bred,F_ELEM(psym,ptab),F_ELEM(psym,bptab),
		bclauses,(ITEM)I_TERM);
	      F_ELEM(psym,ptab)=cls1=l_copy(cls);
	      F_ELEM(psym,bptab)=b_copy(bpcls);
	      bclauses=b_copy(bcls);
	      cl_pfirstarg(psym);
	  BIT_END
	  i_deletes(bpcls,cls,bcls,(ITEM)I_TERM);

	  printf("[TESTING NEGATIVES]\n");
	  if((psym!=pfalse0) && (bnegs= *f_ins(pfalse0,bptab))) {
	    cls=l_copy(*f_ins(psym,ptab));
	    bpcls=b_copy(F_ELEM(psym,bptab));
	    bcls=b_copy(bclauses);
	    BIT_DO(cno,bnegs)
	        b_rem(cno,bclauses);
	        c_gen1(psym);	/* Generalise w/o clause */
	        ccl_swrite(mess1,ccl=i_dec(b_elem(cno,spcls)));
	        if(interp_quest(ccl)) {
		  aP+=1.0;
		  printf("[Wrong: %s]\n",mess1);
	  	  printf("[Partial accuracy= %.f/%.f]\n",RIGHT,N);
	        }
	        else {
	          ap+=1.0;			/* Add up and reset state */
		  printf("[Right: %s]\n",mess1);
	  	  printf("[Partial accuracy= %.f/%.f]\n",RIGHT,N);
	        }
	        i_deletes(F_ELEM(psym,ptab),F_ELEM(psym,bptab),
			bclauses,(ITEM)I_TERM);
	        F_ELEM(psym,ptab)=cls1=l_copy(cls);
	        F_ELEM(psym,bptab)=b_copy(bpcls);
	        bclauses=b_copy(bcls);
	        cl_pfirstarg(psym);
	    BIT_END
	    i_deletes(bpcls,cls,bcls,(ITEM)I_TERM);
	  }
	  c_chisq(AP,aP,Ap,ap);
	  result=TRUE;
	}
	else printf("[Predicate %s/%d not defined]\n",QP_ntos(psym),
		QP_ntoa(psym));
	/* Count up and return result */
	if(oldsearchq) SET(searching);
	else UNSET(searching);
	return(result);
}

/* Chi-square test on 2/2 contingency table
		 A ~A
	      P	AP aP
	     ~P Ap ap
 */

c_chisq(AP,aP,Ap,ap)
	DOUBLE AP,aP,Ap,ap;
	{
	printf("Contingency table=    ________A________~A\n");
	printf("                    P|%9.f|%9.f| %9.f\n",AP,aP,N1);
	printf("                     |(%7.1f)|(%7.1f)| \n",E1,E2);
	printf("                   ~P|%9.f|%9.f| %9.f\n",Ap,ap,N2);
	printf("                     |(%7.1f)|(%7.1f)|\n",E3,E4);
	printf("                      ~~~~~~~~~~~~~~~~~~~\n",Ap,ap);
	printf("                      %9.f %9.f  %9.f\n",N3,N4,N);
	printf("[Overall accuracy= %.2f%% +/- %.2f%%]\n",PACC,PERR);
	yates=TRUE;
	printf("[Chi-square = %.2f]\n",CHISQ);
	yates=FALSE;
	printf(" [Without Yates correction = %.2f]\n",CHISQ);
	yates=TRUE;
	printf("[Chi-square probability = %.4f]\n",CHISQP);
}

LIB(l_chisq) {
	double AP=(LONG)I_GET(args[0l]),aP=(LONG)I_GET(args[1l]),
		Ap=(LONG)I_GET(args[2l]),ap=(LONG)I_GET(args[3l]);
	c_chisq(AP,aP,Ap,ap);
	return(TRUE);
}

LIB(l_seen) {
	register result=FALSE;
	if(plgin!=ttyin) {
	  freclose(plgin);
	  plgin=ttyin;
	  result=TRUE;
	}
	return(result);
}

LIB(l_write) {
	noquotes=TRUE;
	p_fwritesub(plgout,args[0l],subsin[0l]);
	noquotes=FALSE;
	return(TRUE);
}

LIB(l_read) {
	register ITEM term;
	PREDICATE result=FALSE;
	term=exp_fread(plgin);
	if(term==(ITEM)I_TERM) term=i_inc(eof);
	if(term!=(ITEM)I_ERROR) {
	  subout->term=term;
	  subout->subst=(BIND)NULL;
	  l_push(i_dec(term),built);
	  result=TRUE;
	}
	return(result);
}

LIB(l_read1) {
	register ITEM term;
	PREDICATE result=FALSE;
	while((term=exp_fread(plgin))==(ITEM)I_ERROR);
	if(term!=(ITEM)I_TERM) {
	  subout->term=term;
	  subout->subst=(BIND)NULL;
	  l_push(i_dec(term),built);
	  result=TRUE;
	}
	return(result);
}


LIB(l_nl) {
	i_fnl(plgout);
	return(TRUE);
}

LIB(l_tab) {
	register PREDICATE result=FALSE;
	register LONG cnt;
	if(args[0l]->item_type=='i' && (cnt=(LONG)I_GET(args[0l]))>=0l) {
		for(;cnt;cnt--) i_fpr(plgout," ");
		result=TRUE;
	}
	return(result);
}

LIB(l_stickel) {
	LONG arity;
	STRING name;
	ITEM command=args[0l],anc;
	if(fsym(command,&name,&arity,subsin[0l])==(LONG)I_ERROR) {
	    printf("[Command should have form ancestor(Pred/Arity)]\n");
	    return(FALSE);
	}
	else {
	  anc=c_stickelise(QP_ston(name,arity));
	  i_delete(anc);
	}
	return(TRUE);
}

LIB(l_arg) {
	PREDICATE result=FALSE,numok=FALSE,numset=FALSE;
	ITEM arg0,arg1,arg2,sym;
	LONG n;
	BIND sub2;
	if((arg0=args[0])->item_type=='i') {
	    if((n=(LONG)I_GET(arg0)) >= 0l) numok=TRUE;
	}
	else if((arg0=args[0])->item_type=='v') {
	    subout->term=I_INT(n=callno);
	    subout->subst=(BIND)NULL;
	    l_push(i_dec(subout->term),built);
	    numok=TRUE;
	    numset=TRUE;
	}
	if(numok && (numset || !callno)) {
	    if ((arg1=args[1])->item_type== 'f' && n < FUNC_SIZE(arg1)) {
	      if(!n) {
		l_push(i_dec(sym=i_copy(F_ELEM(0l,arg1))),built);
		I_GET(sym)=(POINTER)QP_ston(QP_ntos((LONG)I_GET(sym)),0l);
		(sub2=subout+2l)->term=sym;
		(subout+2l)->subst=(BIND)NULL;
	      }
	      else (sub2=subout+2l)->term=F_ELEM(n,arg1);
	      sub2->subst=subsin[1l];
	      result=TRUE;
	    }
	    else if(arg1->item_type == 'h' && !n && !callno) {
		(sub2=subout+2l)->term=arg1;
		sub2->subst=subsin[1l];
		result=TRUE;
	    }
	}
	return(result);
}

LIB(l_float) {
	register PREDICATE result=FALSE;
	register ITEM arg,ans;
	double frac; 
	if(callno&& !generate) result=FALSE;
	else if((arg=args[0])->item_type=='r')
	  if(callno) result=FALSE;
	  else result=TRUE;
	else if (arg->item_type=='v') {
	  frac=c_fraction(callno);
	  ans=subout->term=i_create('r',(POINTER)r_create(frac));
	  l_push(i_dec(ans),built);
	  result=TRUE;
	}
	return(result);
}

LIB(l_functor) {
	PREDICATE result=FALSE;
	ITEM arg0,arg1,arg2;
	char type0;
	LONG arity,cnt;
	BIND sub;
	if((type0=(arg0=args[0l])->item_type)=='v') {
	  if((arg1=args[1l])->item_type=='h' &&
		(arg2=args[2l])->item_type=='i') {
	    if(!(arity=(LONG)I_GET(arg2))) subout->term=arg1;
	    else if((char *)(break1->env_stack)<=
		(char *)((sub=(BIND)(break1->next_term_stack))+arity))
	      return(FALSE);
	    else {
	      BIND sub=(BIND)(break1->next_term_stack);
	      arg0=i_create('f',f_create((LONG)I_GET(arg2)+1l));
	      F_ELEM(0l,arg0)=i_create('h',
		QP_ston(QP_ntos((LONG)I_GET(arg1)),arity));
	      for(cnt=0l;cnt<arity;cnt++) {
		F_ELEM(cnt+1l,arg0)=i_create('v',(POINTER)cnt);
		(sub+cnt)->term=(ITEM)NULL;
	      }
	      break1->next_term_stack= /* Set up new variable frame */
		(char*)(break1->env_stack->reset=(BIND*)(sub+arity));
	      l_push(i_dec(arg0),built);
	      subout->term=arg0;
	      subout->subst=sub;
	    }
	    result=TRUE;
	  }
	}
	else if(type0=='f') {
	  (subout+1l)->term=i_create('h',(POINTER)QP_ston(
			QP_ntos((LONG)I_GET(F_ELEM(0l,arg0))),0l));
	  (subout+2l)->term=i_create('i',(POINTER)(F_SIZE(arg0)-1l));
	  l_push(i_dec((subout+1l)->term),built);
	  l_push(i_dec((subout+2l)->term),built);
	  result=TRUE;
	}
	else if(type0=='h') {
	  (subout+1l)->term=arg0;
	  (subout+1l)->subst=(BIND)NULL;
	  (subout+2l)->term=i_create('i',(POINTER)0l);
	  (subout+2l)->subst=(BIND)NULL;
	  l_push(i_dec((subout+2l)->term),built);
	  result=TRUE;
	}
	return(result);
}

LIB(l_randseed)
	{
	LONG dt=datetime();
	SRAND(dt);
	printf("[The random number generator seed has been reset using the time of day]\n");
	return(TRUE);
}

LIB(l_fixedseed)
	{
	SRAND(1);
	printf("[The random number generator seed has been reset to 1]\n");
	return(TRUE);
}

LIB(l_univ) {
	PREDICATE result=FALSE;
	ITEM arg0,arg1,l,term0,first,pl,new0,sym,first1;
	BIND sub1;
	LONG len;
	char type0;
	if((type0=(arg0=args[0l])->item_type)=='v') {
	  if((arg1=args[1l])->item_type=='f' &&
		(LONG)I_GET(F_ELEM(0l,arg1))==pdot2) {
	    if(l=l_ptol(ct_renumv(arg1=p_copy(args[1l],subsin[1l],TRUE)))) {
	      first=HOF((LIST)I_GET(l));
	      if((first=HOF((LIST)I_GET(l)))->item_type=='h') {
	        if((len=l_length(l))==1l) term0=i_inc(first);
	        else {
		  first1=i_create('h',QP_ston(
			QP_ntos((LONG)I_GET(first)),len-1l));
		  i_delete(l_pop(l));
		  l_push(i_dec(first1),l);
	          term0=f_ltof(l);
	        }
	        l_push(i_dec(term0),built);
	        l_push(i_dec(arg1),built);
	        subout->term=term0;
		subout->subst=sub1=c_newframe(term0,break1);
	        (subout+1l)->term=arg1;
	        (subout+1l)->subst=sub1;
	        result=TRUE;
	      }
	    }
	    i_deletes(l,(ITEM)I_TERM);
	  }
	}
	else if(type0=='f') {
	  l_push(i_dec(new0=i_copy(arg0)),built);
	  sym=F_ELEM(0l,new0);
	  I_GET(sym)=(POINTER)QP_ston(QP_ntos((LONG)I_GET(sym)),0l);
	  (sub1=subout+1l)->term=pl=l_ltop(l=f_ftol(new0));
	  sub1->subst=subsin[0l];
	  l_push(i_dec(pl),built);
	  i_delete(l);
	  result=TRUE;
	}
	else if(type0=='h'||type0=='i'||type0=='r') {
	  (sub1=subout+1l)->term=pl=l_ltop(l=l_push(arg0,L_EMPTY));
	  sub1->subst=subsin[0l];
	  l_push(i_dec(pl),built);
	  i_delete(l);
	  result=TRUE;
	}
	return(result);
}


/* l_test1 - ground unit positive and negative examples tested
 *	efficiently using d_groundcall. Other types of Horn clauses
 *	tested using d_cimplied. Roughly twice as efficient as
 *	testing ground unit positives using d_cimplied.
 */

PREDICATE c_test();

LIB(l_test1) {
	ITEM fname=args[0];
	ITEM pseen,tp,tn,fp,fn;
	double AP,aP,Ap,ap;
	LONG psym;
	pseen=B_EMPTY;
	tp=Y_EMPTY; tn=Y_EMPTY; fp=Y_EMPTY; fn=Y_EMPTY;
	if(c_test(fname,pseen,tp,tn,fp,fn)) {
	  BIT_DO(psym,pseen)
	    printf("\n[PREDICATE %s/%d]\n\n",QP_ntos(psym),QP_ntoa(psym));
	    AP= *y_ins(psym,tp); aP= *y_ins(psym,fn);
	    Ap= *y_ins(psym,fp); ap= *y_ins(psym,tn);
	    c_chisq(AP,aP,Ap,ap);
	  BIT_END
	}
	i_deletes(pseen,tp,tn,fp,fn,(ITEM)I_TERM);
	return(TRUE);
}

/* l_test2 - same as test1 but returns answers as Prolog list of lists
 *	of the form [[p/n,AP,aP,Ap,ap],..] rather than printing
 *	out the results using c_chisq.
 */

LIB(l_test2) {
	ITEM fname=args[0];
	ITEM pseen,tp,tn,fp,fn,pa,clist0,clist1;
	LONG AP,aP,Ap,ap;
	LONG psym;
	pseen=B_EMPTY;
	tp=Y_EMPTY; tn=Y_EMPTY; fp=Y_EMPTY; fn=Y_EMPTY;
	if(c_test(fname,pseen,tp,tn,fp,fn)) {
	  clist1=L_EMPTY;
	  BIT_DO(psym,pseen)
	    AP= *y_ins(psym,tp); aP= *y_ins(psym,fn);
	    Ap= *y_ins(psym,fp); ap= *y_ins(psym,tn);
	    pa=i_tup3(i_dec(i_create('h',(POINTER)pdiv2)),
			i_dec(i_create('h',QP_ston(QP_ntos(psym),0l))),
			i_dec(I_INT(QP_ntoa(psym))));
	    clist0=l_push(pa,
		  l_push(i_dec(I_INT(AP)),
		  l_push(i_dec(I_INT(aP)),
		  l_push(i_dec(I_INT(Ap)),
		  l_push(i_dec(I_INT(ap)),L_EMPTY)))));
	    l_suf(i_dec(l_ltop(clist0)),clist1);
	    i_deletes(pa,clist0,(ITEM)I_TERM);
	  BIT_END
	  (subout+1l)->term=l_ltop(clist1);
	  (subout+1l)->subst=(BIND)NULL;
	  l_push(i_dec((subout+1l)->term),built);
	  i_delete(clist1);
	}
	i_deletes(pseen,tp,tn,fp,fn,(ITEM)I_TERM);
	return(TRUE);
}

PREDICATE
c_test(fname,pseen,tp,tn,fp,fn)
	ITEM fname,pseen,tp,tn,fp,fn;
	{
	STRING name;
	PREDICATE groundq;
	ITEM ccl,cl,atom;
	LONG psym,status=TP;
	FILEREC *in;
	char file[MAXMESS];
	if(fname->item_type== 'h')
		name=QP_ntos((STRING)I_GET(fname));
	else return(FALSE);
	if(STRSUFQ(name,".pl")) sprintf(file,"%s",name);
	else sprintf(file,"%s.pl",name);
	if (!(in=frecopen(file,"r"))) {
		printf("[Cannot find %s]\n",file);
		return(FALSE);
	}
	while((ccl=ccl_fread(in))!=(ITEM)I_TERM) {
	    if(ccl==(ITEM)I_ERROR) continue;
	    else if(CTYPE(ccl)==(POINTER)pdot0) {	/* Statement */
	      cl=F_ELEM(0l,ccl);
	      atom=HOF((LIST)I_GET(cl));
	      psym=PSYM(atom);
	      if((groundq=GROUNDQ(ccl)) && l_length(cl)==1l)
		/* Ground positive */
		status=(d_groundcall(atom)?TP:FP);
	      else if(groundq && psym==pfalse0 && l_length(cl)==2l) {
		/* Ground negative */
		psym=PSYM(atom=HOF(TOF((LIST)I_GET(cl))));
		status=(d_groundcall(atom)?FN:TN);
	      }
	      else {	/* Other types of clause */
		if(d_cimplied(ccl)) status=((psym==pfalse0)?FN:TP);
		else status=((psym==pfalse0)?TN:FP);
	        if(psym==pfalse0 && l_length(cl)>=2l)
			psym=PSYM(HOF(TOF((LIST)I_GET(cl))));
	      }
	      b_add(psym,pseen);
	      switch(status) {
		case TP:	/* True positive */
		  (*y_ins(psym,tp))++;
		  break;
		case TN:	/* True negative */
		  (*y_ins(psym,tn))++;
		  break;
		case FP:	/* False negative (omission error) */
		  (*y_ins(psym,fp))++;
		  if(verbose>1l) {
		    printf("[False negative:]");
		    ccl_print(ccl);
		  }
		  break;
		case FN:	/* False positive (commission error) */
		  (*y_ins(psym,fn))++;
		  if(verbose>1l) {
		    printf("[False positive:]");
		    ccl_print(ccl);
		  }
		  break;
		default: d_error("c_test - unknown status value");
	      }
	    }
	    else {
	      printf("[Ignored statement:]");
	      ccl_print(ccl);
	    }
	    i_delete(ccl);
	}
	freclose(in);
	return(TRUE);
}

PREDICATE l_eval();

LIB(l_is) {
	double ans;
	ITEM i_ans;
	char itype;
	if(l_eval(args[1l],subsin[1l],&ans,&itype)) {
	  if(itype== 'i') {
	    LONG i=(LONG)ans;
	    if(-MAXN<=i&&i<MAXN) subout->term=F_ELEM(i+MAXN,nums);
	    else {
	      i_ans=subout->term= I_INT((LONG)ans);
	      l_push(i_dec(i_ans),built);
	    }
	  }
	  else {
	    i_ans=subout->term=i_create('r',r_create(ans));
	    l_push(i_dec(i_ans),built);
	  }
	  subout->subst=(BIND)NULL;
	  return(TRUE);
	}
	else return(FALSE);
}

PREDICATE
l_eval(t,s,ans,atype)
	ITEM t;
	BIND s;
	double *ans;
	char *atype;
	{
	LONG result=FALSE;
	LONG p,ix,iy;
	double fx,fy,*fp;
	double dx,dy;
	char xtype,ytype;
	PREDICATE i;
	SKIPVARS(t,s);
	switch(t->item_type) {
	  case 'f':
	    p=PSYM(t);
	    switch(F_SIZE(t)) {
	      case 2l:
	        if(l_eval(F_ELEM(1l,t),s,&fx,atype)) {
		  if(*atype=='i') ix=(LONG)fx;
		  if(p==pminus1) *ans=(double)(*atype=='i'?-ix:-fx);
		  else if(p==pplus1) *ans=fx;
		  else if(p==plog1) {
		    if(*atype== 'i'&&ix>0l) {*ans=log(dx=ix); *atype='r';}
		    else if(fx>0.0) *ans=log(dx=fx);
		    else return(FALSE);
		  }
		  else if(p==pexp1) {
		    if(*atype== 'i') {*ans=exp(dx=ix); *atype='r';}
		    else *ans=exp(dx=fx);
		  }
		  else if(p==psin1) {
		    if(*atype== 'i') {*ans=sin(dx=ix); *atype='r';}
		    else *ans=sin(dx=fx);
		  }
		  else if(p==pcos1) {
		    if(*atype== 'i') {*ans=cos(dx=ix); *atype='r';}
		    else *ans=cos(dx=fx);
		  }
		  else if(p==ptan1) {
		    if(*atype== 'i') {*ans=tan(dx=ix); *atype='r';}
		    else *ans=tan(dx=fx);
		  }
		  else if(p==pasin1) {
		    if(*atype== 'i') {*ans=asin(dx=ix); *atype='r';}
		    else *ans=asin(dx=fx);
		  }
		  else if(p==pacos1) {
		    if(*atype== 'i') {*ans=acos(dx=ix); *atype='r';}
		    else *ans=acos(dx=fx);
		  }
		  else if(p==patan1) {
		    if(*atype== 'i') {*ans=atan(dx=ix); *atype='r';}
		    else *ans=atan(dx=fx);
		  }
		  else if(p==pceil1) {
		    if(*atype== 'i') *ans=(double)ix;
		    else *ans=(double)(ix=ceil(dx=fx));
		    *atype='i';
		  }
		  else if(p==pfloor1) {
		    if(*atype== 'i') *ans=(double)ix;
		    else *ans=(double)(ix=floor(dx=fx));
		    *atype='i';
		  }
		  else return(FALSE);
		  result=TRUE;
		}
		break;
	      case 3l:
	        if(l_eval(F_ELEM(1l,t),s,&fx,&xtype)&&
			l_eval(F_ELEM(2l,t),s,&fy,&ytype)) {
		  if(xtype=='i'&&ytype=='r') { /* Type conversion */
		    ix=(LONG)fx; fx=ix; xtype='r'; *atype='r';
		  }
		  else if(xtype=='r'&&ytype=='i') {
		    iy=(LONG)fy; fy=iy; ytype='r'; *atype='r';
		  }
		  else if(xtype=='i'&&ytype=='i') {
		    ix=(LONG)fx; iy=(LONG)fy;
		    if(p==pdiv2) {fx=ix; fy=iy; *atype='r';}
		    else *atype='i';
		  }
		  else *atype='r';
		  i=(xtype=='i');
	          if(p==pplus2) *ans=(double)(i?ix+iy:fx+fy);
		  else if(p==pminus2) *ans=(double)(i?ix-iy:fx-fy);
		  else if(p==ptimes2) *ans=(double)(i?ix*iy:fx*fy);
		  else if(p==pdiv2&&fy!=0) *ans=fx/fy;
		  else if(p==phat2&&((i&&ix>=0)||(!i&&fx>=0.0))) {
		    if(i) {dx=ix;dy=iy;}
		    else {dx=fx;dy=fy;}
		    *ans=pow(dx,dy);
		    *atype='r';
		    /* if(i) {ix= *ans;*ans=(double)ix;} */
		  }
		  else if(p==pmod2&&i&&iy) *ans=(double)(ix%iy);
		  else if(p==por2&&i) *ans=(double)(ix|iy);
		  else if(p==pand2&&i) *ans=(double)(ix&iy);
		  else return(FALSE);
		  result=TRUE;
	        }
	        break;
	      default:
		break;
	    }
	    break;
	  case 'i':
	    ix=(LONG)I_GET(t);
	    *ans=(double)ix;
	    *atype='i';
	    result=TRUE;
	    break;
	  case 'r':
	    fp=(double *)I_GET(t);
	    *ans= *fp;
	    *atype='r';
	    result=TRUE;
	    break;
	  case 'h':
	    result=TRUE;
	    p=PSYM(t);
	    if(p==prand0) {
	      *ans=RAND;
	      *atype='r';
	    }
	    else if(p==pcputime0) {
	      *ans=(double)cputime();
	      *atype='r';
	    }
	    else result=FALSE;
	    break;
	  default:
	    break;
	}
	return(result);
}

LIB(l_var) {
	return(args[0]->item_type=='v');
}

LIB(l_vassert) {
	PREDICATE result=FALSE;
	LONG cno;
	if(args[0]->item_type=='i' && (cno=(LONG)I_GET(args[0]))>=0
		&& cno<(LONG)(I_GET(F_ELEM(2l,spcls)))) {
	  b_add(cno,bclauses);
	  result=TRUE;
	}
	return(result);
}

LIB(l_vretract) {
	PREDICATE result=FALSE;
	LONG cno;
	if(args[0]->item_type=='i' && (cno=(LONG)I_GET(args[0]))>=0) {
	  b_rem(cno,bclauses);
	  result=TRUE;
	}
	return(result);
}

ITEM
l_namesp(preds,max,nc)
	ITEM preds;
	LONG *max,*nc;
	{
	char mess[MAXMESS];
	STRING name;
	LONG arity,psym,n;
	ITEM result=L_EMPTY,rec;
	BIT_DO(psym,preds)
	  rec=F_ELEM(psym,F_ELEM(0l,spsyms));
	  name=((STRING)I_GET(F_ELEM(0l,rec)));
	  arity=((LONG)I_GET(F_ELEM(1l,rec)));
	  predn(mess,name,arity);
	  if((n=strlen(mess))>*max) *max=n;
	  l_suf(i_dec(i_create('s',strsave(mess))),result);
	  if(rec= *f_ins(psym,bptab)) *nc+=b_size(rec);
	BIT_END
	return(i_sort(result));
}


l_showp(preds)
	ITEM preds;
	{
	LONG cwidth=0l,nc=0l;
	ITEM names=l_namesp(preds,&cwidth,&nc);
	l_showw(names,cwidth);
	i_delete(names);
	if(preds!=lib1) printf("[Total number of clauses = %d]\n",nc);
}

l_showw(names,cwidth)
	ITEM names;
	LONG cwidth;
	{
	ITEM name;
	LONG cnt1=0l,cnt2,columns=(78)/(cwidth+1l);
	STRING mess;
	LIST_DO(name,names)
	  if(!cnt1) printf("  ");
	  printf(mess=I_GET(name));
	  for(cnt2=(cwidth+1l)-strlen(mess);cnt2>0;cnt2--)
		printf(" ");
	  if(++cnt1 >= columns) {
		cnt1=0l;
		printf("\n");
	  }
	LIST_END
	if(cnt1) printf("\n");
}

ITEM c_choose(),c_kpart();

/* Below is an expression for generating random natural numbers
	using distribution 2^{x-1} */

/*

#define RNAT	(-log2(RAND))

LIB(l_rand) {
	LONG y,nsyms=(LONG)I_GET(F_ELEM(2l,spsyms)),cnt1,cnt2;
	ITEM terms=F_ELEM(0l,spsyms),bterms,term;
	bterms=b_allones(nsyms);
	for(cnt1=20l;cnt1;cnt1--) {
	 for(cnt2=5l;cnt2;cnt2--) {
	  y= RNAT+1.0;
	  term=c_choose(y,terms,bterms);
	  at_fwrite(ttyout,term); printf("  ");
	  i_deletes(term,(ITEM)I_TERM);
	 }
	 i_fnl(ttyout);
	}
	i_delete(bterms);
	return(TRUE);
}
*/

/* c_fraction - returns the double constructed by reflecting (in binary)
 *	the given integer around the decimal point.
 *	eg. c_fraction(10)=0.01
 */

double
c_fraction(i)
	LONG i;
	{
	LONG val;
	double frac,res; 
	for(res=0.0,frac=0.5,val=i;val;frac/=2.0,val>>=1l)
	    if(val&1l) res+=frac;
	return(res);
}

ITEM
c_kpart(k,n)
	LONG k,n;
	{
	LONG cnt;
	ITEM result=Y_EMPTY;
	for(cnt=k;cnt>0l;cnt--) Y_PUSH(1l,result);
	for(cnt=n-k;cnt>0l;cnt--) Y_ELEM(MYRAND(0l,k-1l),result)++;
	return(result);
}

ITEM
c_range(lo,hi,terms,bterms)
	LONG lo,hi;
	ITEM terms,bterms;
	{
	LONG arity,psym;
	ITEM result=B_EMPTY;
	BIT_DO(psym,bterms)
	  arity=((LONG)I_GET(F_ELEM(1l,F_ELEM(psym,terms))));
	  if(lo<=arity && arity<=hi) b_add(psym,result);
	BIT_END
	return(result);
}

ITEM
c_choose(n,terms,bterms)
	LONG n;
	ITEM terms,bterms;
	{
	LONG m,r,psym,arity,i;
	ITEM bterms1,result,fsym,kpart,*subt;
	if(n<=1l) {
	  if(b_emptyq(bterms1=c_range(0l,0l,terms,bterms))) {
	    result=i_create('i',n);
	  }
	  else {
	    r=MYRAND(1,b_size(bterms1));
	    psym=b_ith(r,bterms1);
	    result=i_create('h',psym);
	  }
	}
	else {
	  if(b_emptyq(bterms1=c_range(1l,n-1l,terms,bterms))) {
	    result=i_create('i',n);
	  }
	  else {
	    r=MYRAND(1,b_size(bterms1));
	    psym=b_ith(r,bterms1);
	    fsym=i_create('h',psym);
	    arity=(LONG)I_GET(F_ELEM(1l,F_ELEM(psym,terms)));
	    result=i_create('f',f_create(arity+1l));
	    F_ELEM(0l,result)=fsym;
	    kpart=c_kpart(arity,n-1l);
	    for(i=1l;i<=arity;i++)
		F_ELEM(i,result)=c_choose(Y_ELEM(i-1l,kpart),terms,bterms);
	    i_delete(kpart);
	  }
	}
	i_delete(bterms1);
	return(result);
}

/* Table of library predicates
 */

struct libstruc clib[] = {
	"!", 0l, FALSE, NULL, "Prolog cut",
	",", 2l, TRUE, NULL, "(A, B) succeeds if A and B succeed",
	";", 2l, TRUE, NULL, "(A; B) succeeds if A or B succeed",
	"->", 2l, TRUE, NULL, "(A->B) if A then B",
	"=<", 2l, TRUE, l_le, "A =< B where A,B instantiated",
	">=", 2l, TRUE, NULL, "A >= B where A,B instantiated",
	"<", 2l, TRUE, l_lt, "A < B where A,B instantiated",
	">", 2l, TRUE, NULL, "A > B where A,B instantiated",
	"\\=", 2l, TRUE, NULL, "A \\= B where A,B instantiated",
	"\\==", 2l, TRUE, NULL, "A \\== B where A,B instantiated",
	"\\+", 1l, TRUE, NULL, "negation by failure",
	"permute", 1l, FALSE, l_permute, "permute(File) randomly permutes order of clauses in File",
	"@<", 2l, FALSE, l_plt, "A @< B term A lexicographically less than B",
	"@>", 2l, FALSE, NULL, "A @> B term A lexicographically greater than B",
	"@>=", 2l, FALSE, NULL, "A @>= B term A lexicographically greater or equal to B",
	"@=<", 2l, FALSE, NULL, "A @=< B term A lexicographically less or equal to B",
	"==", 2l, FALSE, l_equiv, "A == B if terms A and B identical",
	"=", 2l, FALSE, NULL, "A = B unifies A and B",
	"=..", 2l, FALSE, l_univ, "f(T1,..,Tn) =.. [f,T1,..,Tn]",
	".", 2l, FALSE, NULL, "[File|Files] reconsults File and Files",
	"advise", 1l, FALSE, l_advise, "writes user predicates to file",
	"any", 1l, FALSE, NULL, "any(X) succeeds on any term",
	"arg", 3l, TRUE, l_arg, "arg(N,T,T1) where T1 is the Nth argument of term T",
	"asserta", 1l, FALSE, l_asserta, "asserta(Clause) asserts the clause at beginning of definition",
	"bagof", 3l, FALSE, NULL, "bagof(X,P,B) bagof X such that P is B",
	"check1", 1l, FALSE, l_check1, "check1(Atom) used in not/1 to check if Atom is modeh during theory reduction",
	"chisq", 4l, FALSE, l_chisq, "chisq(AP,aP,Ap,ap) prints chi-square statistics",
	"clause", 2l, TRUE, NULL, "clause(Head,Body) where Head must be instantiated",
	"clause", 3l, TRUE, l_clause, "clause(Head,Body,N) where N is the number of the clause",
	"commutative", 1l, FALSE, l_commutative, "commutative(Pred/Arity) means input arguments unordered",
	"commutatives", 0l, FALSE, l_commutatives, "show commutative predicates",
	"constant", 1l, FALSE, l_constant, "constant(X) true if X is a constant",
	"consult", 1l, FALSE, l_consult, "consults file",
	"determination", 2l, FALSE, l_determination, "determination for predicate, eg. determination(mult/3,plus/3)",
	"edit", 1l, FALSE, l_edit, "edit(Pred/Arity) allows editing of predicate",
	"element", 2l, FALSE, NULL, "element(X,L) if X is a member of list L",
	"fixedseed", 0l, FALSE, l_fixedseed, "random seed set to 1",
	"float", 1l, TRUE, l_float, "float(F) is true if F is a floating point number",
	"functor", 3l, FALSE, l_functor, "functor(Term,Fsym,Arity) where Term has function symbol Fsym/Arity",
	"generalise", 1l, FALSE, l_gen1, "generalise(Pred/Arity) generalises all clauses with head Pred/Arity",
	"help", 0l, FALSE, l_help0, "lists the system predicates, help available using help/1",
	"help", 1l, FALSE, l_help1, "gives help information on particular command, eg. help(tell/1)",
	"hypothesis", 3l, FALSE, l_hypothesis, "hypothesis(Head,Body,N) where N is number of hypothesis being tested",
	"in", 2l, FALSE, NULL, "in(A,As) if A is in comma separated list As",
	"int", 1l, TRUE, l_int, "int(X) succeeds when X is an integer",
	"is", 2l, FALSE, l_is, "X is Y where X is the evaluation of expression Y",
	"layer", 1l, FALSE, l_layer, "layer(File) layered learning from File",
	"leave", 1l, FALSE, l_leave, "leave(Pred/Arity) gives predictive accuracy using leave-one-out",
	"label", 1l, FALSE, l_label1, "label(ClauseN) increments the label of the clause",
	"label", 2l, FALSE, l_label2, "label(ClauseN,N) N is the label of the given clause",
	"length", 2l, FALSE, NULL, "length(List,Length)",
	"listing", 0l, FALSE, l_list0, "list user-defined predicates",
	"listing", 1l, FALSE, l_list1, "listing(Pred/Arity) prints the clauses of Pred/Arity",
	/*
	"localtime", 2l, FALSE, l_localtime, "localtime(S,D) S is secs since zero time and D is Year-Month-Mday-Wday-Hour-Min-Sec",
	 */
	"modeb", 2l, FALSE, l_modeb, "determinacy and body mode, eg. modeb(1,mult(+int,+int,-int))",
	"modeh", 2l, FALSE, l_modeh, "determinacy and head mode, eg. modeh(1,qsort([+int|+list],-list))",
	"modes", 0l, FALSE, l_modes, "show head and body modes of predicates",
	"name", 2l, FALSE, l_name, "name(X,L) where X is a constant and L is a list of asciis",
	"nat", 1l, TRUE, l_nat, "nat(X) succeeds when X is a natural number",
	"nl", 0l, FALSE, l_nl, "print new line",
	"normal", 3l, FALSE, l_normal, "normal(X,Mu,Sigma) - random X from normal dstn with mean Mu and std dev. Sigma",
	"nospy", 0l, FALSE, l_nospy, "turn OFF Prolog spy points",
	"not", 1l, FALSE, NULL, "negation by failure",
	"notrace", 0l, FALSE, l_notrace, "set interpreter trace flag OFF",
	"number", 1l, TRUE, NULL, "number(X) if X is integer or real",
	"observable", 1l, FALSE, l_observable, "observable(Pred/Arity) states Pred is an observable",
	"op", 3l, FALSE, l_op, "op(Precedence,Associativity,Symbol) same as Prolog",
	"ops", 0l, FALSE, l_ops, "show present operator precedences and associativities",
	"otherwise", 0l, FALSE, NULL, "otherwise always succeeds",
	/* "right", 1l, FALSE, l_right, "right(X) - print right triangles to X",*/
	/* "rand", 0l, FALSE, l_rand, "rand generates random terms with log distribution", */
	"prune", 0l, FALSE, NULL, "prune/0 calls user defined prune(Head,Body) which defines when clauses can be pruned from search",
	"prune1", 2l, FALSE, NULL, "prune1/2 contains system-defined prunes",
	"quit", 0l, FALSE, l_quit, "exits from Progol",
	"randomseed", 0l, FALSE, l_randseed, "random seed set to time of day",
	"read", 1l, FALSE, l_read, "read(X) reads X from input",
	"read1", 1l, TRUE, l_read1, "repeat read on recall until end_of_file",
	"read", 2l, TRUE, NULL, "read(File,X) - uses read1 to find X in File",
	"reconsult", 1l, FALSE, l_reconsult, "reconsults file",
	"record", 2l, FALSE, l_record, "used by bagof to record instances",
	"reduce", 1l, FALSE, l_reduce, "reduce the given predicate definition",
	"repeat", 0l, TRUE, l_repeat, "succeeds indefinitely on recall",
	"retract", 1l, FALSE, NULL, "retract(Clause) removes clause from definitions",
	"retract", 2l, FALSE, l_retract, "retract(Head,N) retracts clause number n",
	"sample", 3l, FALSE, l_sample3, "sample(Pred/Arity,N,S) - S is a list of N randomly sampled atoms of the predicate",
	"see", 1l, FALSE, l_see, "see(X) opens file X for reading",
	"seen", 0l, FALSE, l_seen, "seen closes file open for reading",
	"set", 1l, FALSE, l_set1, "set(Flag) where Flag is one of memoing, reductive, \n\tsampling, searching, splitting, tracing, full_pruning",
	"set", 2l, FALSE, l_set2, "set(Limit,Natural) where Limit is one of c, h, i, noise, nodes or verbose",
	"settings", 0l, FALSE, l_settings, "show present parameter settings",
	"setof", 3l, FALSE, NULL, "setof(X,P,S) S is the setof X such that P(X)",
	"size", 0l, FALSE, l_size, "show sizes of objects",
	"sort", 2l, FALSE, l_sort, "sort(L1,L2) sorts list L1 to list L2",
	"spies", 0l, FALSE, l_spies, "show present spy points",
	"spy", 1l, FALSE, l_spy, "spy(Pred/Arity) place spy point on Pred",
	"stage", 1l, FALSE, l_stage, "stage(S) where S is learning stage nolearn, saturate or refine",
	"stats", 0l, FALSE, l_stats, "table of blocks being used in dynamic memory",
	"stickel", 1l, FALSE, l_stickel, "stickel(Pred/Arity) - Stickelise ancestors of given predicate",
	"system", 1l, FALSE, l_system, "system(C) - executes system command C where C is a constant",
	"tab", 1l, FALSE, l_tab, "print n spaces",
	"tell", 1l, FALSE, l_tell, "tell(X) opens file X for writing",
	"test", 1l, FALSE, l_test1, "test(File) computes predictive accuracy on File.pl",
	"test", 2l, FALSE, l_test2, "test(File,Res) Res is list of lists of the form [Pred/Arity,AP,aP,Ap,ap]",
	"told", 0l, FALSE, l_told, "told closes file open for writing",
	"trace", 0l, FALSE, l_trace, "set interpreter trace flag ON",
	"true", 0l, FALSE, NULL, "true always succeeds",
	"uniform", 3l, FALSE, l_uniform, "uniform(X,Lo,Hi) - random X from uniform dstn in [Lo,Hi]",
	"unset", 1l, FALSE, l_unset, "set interpreter trace flag OFF",
	"var", 1l, FALSE, l_var, "test if argument is a variable",
	"vassert", 1l, FALSE, l_vassert, "vassert(N) virtual assertion of clause N",
	"vretract", 1l, FALSE, l_vretract, "vretract(N) virtual retraction of clause N",
	"write", 1l, FALSE, l_write, "write(X) writes X to output",

        0, 0, 0, 0
};

STRING plib[] = {
	"Term=Term.",
	"X>=Y :- Y=<X.",
	"X>Y :- Y<X.",
	"X\\=Y :- not(X=Y).",
	"X\\==Y :- not(X==Y).",
	"\\+(Atom) :- not(Atom).",
	"X@>Y :- Y@<X.",
	"X@=<Y :- X@<Y;X==Y.",
	"X@>=Y :- X@>Y;X==Y.",
	"[-File] :- !, reconsult(File), !.",
	"[File] :- !, consult(File), !.",
	"[-File|Files] :- !, reconsult(File), Files, !.",
	"[File|Files] :- !, consult(File), Files, !.",
	"not(Atom) :- (check1(Atom); Atom), !, fail.",
	"not(_).",
	"'!'.",
	"(Atom1, Atom2) :- Atom1, Atom2.",
	"(Atom;_) :- Atom.",
	"(_;Atom) :- Atom.",
	"(Atom1->Atom2) :- Atom1, !, Atom2.",
	"any(Term).",
	"bagof(X,P,_) :- P, record(0,X), fail.",
	"bagof(_,_,Bag) :- record(1,Bag), !.",
	"bagof(_,_,[]).",
	"clause(Head,Body) :- clause(Head,Body,_).",
	"element(X,[X|_]).",
	"element(X,[_|T]) :- element(X,T).",
	"in(Atom,Atom) :- Atom\\=(_,_).",
	"in(Atom,(Atom,_)).",
	"in(Atom,(_,Atoms)) :- in(Atom,Atoms).",
	"length([],0).",
	"length([H|T],N) :- length(T,N1), N is N1+1.",
	"number(X) :- int(X).",
	"number(X) :- float(X).",
	"otherwise.",
	"prune :- hypothesis(H,B,_), (prune1(H,B); prune(H,B)).",
	"prune1(_,Body) :- in(Atom1,Body), Atom1=.. [Cmp,A,C], var(A),\
		element(Cmp,['=<','<','>=','>']), in(Atom2,Body),\
		Atom2=.. [Cmp,B,D], A==B, C\\==D.",
        "prune1(_,Body) :- in(Atom1,Body), Atom1=.. [Cmp,C,A], var(A),\
                element(Cmp,['=<','<','>=','>']), in(Atom2,Body),\
                Atom2=.. [Cmp,D,B], A==B, C\\==D.",
	"read(File,X) :- see(File), read1(X).",
	"retract((Head :- Body)) :- !, bagof([Head,Body,N],clause(Head,Body,N),L), element([Head,Body,N],L), retract(Head,N).",
	"retract(Atom) :- bagof([Atom,N],clause(Atom,true,N),L), element([Atom,N],L), retract(Atom,N).",
	"setof(X,P,Set) :- bagof(X,P,Bag), sort(Bag,Set).",
	"true.",
	""
};
