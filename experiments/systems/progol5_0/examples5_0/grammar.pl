%%%%%%%%%%%%%%%%%%%%
% Grammar learning problem. Learns s/2, a simple English language
%	phrase grammar.

:- modeh(1,s(+wlist,-wlist))?
:- modeb(1,s(+wlist,-wlist))?
:- modeb(1,det(+wlist,-wlist))?
:- modeb(1,conj(+wlist,-wlist))?
:- modeb(1,adj(+wlist,-wlist))?
:- modeh(*,np(+wlist,-wlist))?
:- modeb(*,np(+wlist,-wlist))?
:- modeh(*,vp(+wlist,-wlist))?
:- modeb(*,vp(+wlist,-wlist))?
:- modeb(1,prep(+wlist,-wlist))?
:- modeb(1,noun(+wlist,-wlist))?
:- modeb(1,verb(+wlist,-wlist))?
:- modeh(1,det([#constant|+wlist],-wlist))?
:- modeh(1,adj([#constant|+wlist],-wlist))?
:- modeh(1,noun([#constant|+wlist],-wlist))?
:- modeh(1,verb([#constant|+wlist],-wlist))?
:- modeh(1,prep([#constant|+wlist],-wlist))?
:- modeh(1,conj([#constant|+wlist],-wlist))?
:- set(i,5)?
:- set(c,5)?
:- obersevable(s/2)?

%%%%%%%%%%%%%%%%%%%%
% Types

wlist([]).
wlist([W|Rest]) :- constant(W), wlist(Rest).

%%%%%%%%%%%%%%%%%%%%%%%
% Background knowledge

s(S) :- s(S,[]).

s(S1,S2) :- np(S1,S3), vp(S3,S4), np(S4,S2).
s(S1,S2) :- np(S1,S3), vp(S3,S4), np(S4,S5), prep(S5,S6), np(S6,S2).

np(S1,S2) :- det(S1,S3), noun(S3,S2).
np(S1,S2) :- det(S1,S3), adj(S3,S4), noun(S4,S2).

det([a|S],S).
det([the|S],S).

adj([big|S],S).
adj([small|S],S).
adj([nasty|S],S).

vp(S1,S2) :- verb(S1,S2).
vp(S1,S2) :- verb(S1,S3), prep(S3,S2).

noun([man|S],S).
% noun([dog|S],S).
% noun([house|S],S).
noun([ball|S],S).

verb([takes|S],S).
verb([walks|S],S).
verb([hits|S],S).

prep([at|S],S).
prep([to|S],S).
prep([on|S],S).
prep([in|S],S).
prep([into|S],S).

conj([and|S],S).

%%%%%%%%%%%%%%%%%%%%
% Positive examples

s([the,dog,walks,to,the,man],[]).
s([the,man,walks,the,dog],[]).
s([a,dog,hits,a,ball],[]).
s([the,man,walks,in,the,house],[]).
s([the,man,walks,into,the,house],[]).
s([the,man,hits,the,dog],[]).
s([a,ball,hits,the,dog],[]).
s([the,dog,walks,on,the,house],[]).
s([the,man,hits,at,the,ball],[]).
s([the,big,man,hits,at,the,ball],[]).
s([the,small,dog,walks,on,the,house],[]).
s([the,small,dog,walks,in,the,house],[]).
s([the,small,dog,walks,into,the,house],[]).
s([the,small,man,hits,the,dog],[]).
s([the,big,man,hits,the,dog],[]).
s([a,ball,hits,the,small,dog],[]).
s([the,nasty,man,hits,the,dog],[]).
s([the,man,hits,the,nasty,dog],[]).

s([the,man,hits,the,nasty,men],[]).
s([the,small,dog,walks,on,the,men],[]).
s([the,men,hit,the,nasty,dog],[]).
s([the,men,walk,into,the,house],[]).
s([the,small,men,hit,the,dog],[]).
s([the,dogs,hit,the,man],[]).
s([the,dogs,walk,into,the,house],[]).

s([men,hit,the,nasty,dog],[]).
s([men,walk,into,the,house],[]).
s([men,hit,the,dog],[]).
s([dogs,hit,the,man],[]).
s([dogs,walk,into,the,house],[]).
s([dogs,like,the,man],[]).
s([dogs,like,the,house],[]).

% More complex positive examples.

s([a,man,hits,the,ball,at,the,dog],[]).
s([the,man,hits,the,ball,at,the,house],[]).
s([the,man,takes,the,dog,to,the,ball],[]).
s([a,man,takes,the,ball,to,the,house],[]).
s([the,dog,takes,the,ball,to,the,house],[]).
s([the,dog,takes,the,ball,to,the,man],[]).
s([the,man,hits,the,ball,to,the,dog],[]).
s([the,man,walks,the,dog,to,the,house],[]).

%%%%%%%%%%%%%%%%%%%%
% Negative examples

:- s([a,dog,walks,the],[]).
:- s([a,man,walks,the],[]).
:- s([a,man,walks,the,walks],[]).
:- s([a,man,walks,the,house,a],[]).
:- s([a,man,walks,the,dog,at],[]).
:- s([the,man,walks,the,dog,to,the],[]).
:- s([the,dog],[]).

% Syntactic constraints on clause form

prune(det(_,_),Body) :- Body\=true.	% Terminals must be unit clauses
prune(adj(_,_),Body) :- Body\=true.
prune(noun(_,_),Body) :- Body\=true.
prune(verb(_,_),Body) :- Body\=true.
prune(prep(_,_),Body) :- Body\=true.
% prune(Head,Body) :- in(Head,Body).	% Self-recursion banned
prune(Head,Body) :-			% Mutual recursion depth 1
	in(Atom,Body), clause(Atom,Body1),
	in(Head,Body1).
prune(Head,Body) :-			% Mutual recursion depth 2
	in(Atom1,Body), clause(Atom1,Body1),
	in(Atom2,Body1), clause(Atom2,Body2),
	in(Head,Body2).

% Domain predicates

'*s'(A,B) :- suffix(A,B).	% Term and non-terms give suffix of word-list
'*det'(A,B) :- suffix(A,B).
'*adj'(A,B) :- suffix(A,B).
'*np'(A,B) :- suffix(A,B).
'*vp'(A,B) :- suffix(A,B).
'*prep'(A,B) :- suffix(A,B).
'*noun'(A,B) :- suffix(A,B).
'*verb'(A,B) :- suffix(A,B).
'*conj'(A,B) :- suffix(A,B).

suffix(L,L).
suffix([H|T],L) :- suffix(T,L).
