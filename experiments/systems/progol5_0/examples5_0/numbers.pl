% Translate numbers written in English into integers
% eg. wordnum([three,hundred,and,twenty,five],[],3*100+(20+5))

:- set(h,100)?
:- set(r,1000)?
:- set(inflate,99)?
:- set(nodes,100)?

:- observable(wordnum/3)?

:- modeh(1,wordnum(+wlist,-wlist,-expr))?
:- modeb(1,tenu(+wlist,-wlist,-expr))?
:- modeb(1,word100(+wlist,-wlist,-expr))?
:- modeb(1,word1000(+wlist,-wlist,-expr))?
:- modeb(1,thou(+wlist,-wlist,-expr))?
:- modeb(1,hun(+wlist,-wlist,-expr))?
:- modeb(1,(+wlist = [-constant|-wlist]))?
:- modeb(1,(+expr = (-expr) + (-expr)))?
:- modeb(1,(+expr = (-expr) * (-expr)))?
:- modeb(1,(+any = #any))?
:- modeb(1,digit(+constant,-nat))?
:- modeb(1,tenmult(+constant,-nat))?
:- modeh(1,tenu(+wlist,-wlist,(-expr)+(-expr)))?
:- modeh(1,tenu(+wlist,-wlist,-expr))?
:- modeh(1,word100(+wlist,-wlist,(-expr)+(-expr)))?
:- modeh(1,word100(+wlist,-wlist,-expr))?
:- modeh(1,word1000(+wlist,-wlist,(-expr)+(-expr)))?
:- modeh(1,word1000(+wlist,-wlist,-expr))?
:- modeh(1,thou(+wlist,-wlist,(-expr)*(#expr)))?
:- modeh(1,hun(+wlist,-wlist,(-expr)*(#expr)))?
:- modeh(1,digit(#constant,#nat))?
:- modeh(1,tenmult(#constant,#nat))?

:- set(i,5), set(c,5)?

%%%%%%%%%%%%%%%%%%%%
% Types
 
wlist([]).
wlist([W|Rest]) :- constant(W), wlist(Rest).

expr(C) :- nat(C).
expr(X+Y) :- expr(X), expr(Y).
expr(X*Y) :- expr(X), expr(Y).

%%%%%%%%%%%%%%%%%%%%
% Positive examples (randomly sampled from allwords)

% :- consult(ex100)?

%%%%%%%%%%%%%%%%%%%%%%%
% Background knowledge

% :- consult(t051)?

%%%%%%%%%%%%%%%%%%%%%%%
% Integrity constraints

:- hypothesis(Head,Body,_), nongenerative((Head,Body)).
			% hypothesised clauses must be strongly generative

nongenerative(C) :-	% C is a comma-separated list of atoms
	in(A1,C), var(U,A1),
	not((in(A2,C), A1\==A2, var(V,A2), U==V)).

var(V,V) :- var(V).
var(V,T) :- arg(N,T,T1), N>0, var(V,T1).

% Syntactic constraints on clause form

prune(digit(_,_),Body) :- Body\=true.     % Terminals must be unit clauses
prune(tenmult(_,_),Body) :- Body\=true.
prune(Head,Body) :- in(Head,Body).    % Self-recursion banned
prune(Head,Body) :-                     % Mutual recursion depth 1
        in(Atom,Body), clause(Atom,Body1),
	in(Head,Body1).
prune(Head,Body) :-                     % Mutual recursion depth 2
	in(Atom1,Body), clause(Atom1,Body1),
	in(Atom2,Body1), clause(Atom2,Body2),
	in(Head,Body2).

% wordnums/1 - writes to File all instances of wordnums/3.

wordnums(File) :-
	tell(File),
	wordnum(X,Y,Z),
	write(wordnum(X,Y,Z)), write('.'), nl,
	fail.
wordnums(_) :-
	told.

'*wordnum'(A,B,C) :- suffix(A,B), expr(C).
'*tenu'(A,B,C) :- suffix(A,B), expr(C).
'*word100'(A,B,C) :- suffix(A,B), expr(C).
'*word1000'(A,B,C) :- suffix(A,B), expr(C).
'*thou'(A,B,C) :- suffix(A,B), expr(C).
'*hun'(A,B,C) :- suffix(A,B), expr(C).

suffix(L,L).
suffix([H|T],L) :- suffix(T,L).

% Complete grammar

wordnum(A,[],T) :- tenu(A,[],T).
wordnum(A,[],T) :- word100(A,[],T).
wordnum(A,[],T) :- word1000(A,[],T).
word1000(A,[],T) :- thou(A,[],T).
word1000(A,B,T+N) :- thou(A,[and|C],T), tenu(C,B,N).
word1000(A,B,T+H) :- thou(A,C,T), word100(C,B,H).
thou([D,thousand|R],R,T*1000) :- digit(D,T).
word100(A,[],H) :- hun(A,[],H).
word100(A,B,H+T) :- hun(A,[and|C],H), tenu(C,B,T).
hun([D,hundred|R],R,H*100) :- digit(D,H).
tenu([D],[],N) :- digit(D,N).
tenu([ten],[],10).
tenu([eleven],[],11).
tenu([twelve],[],12).
tenu([thirteen],[],13).
tenu([fourteen],[],14).
tenu([fifteen],[],15).
tenu([sixteen],[],16).
tenu([seventeen],[],17).
tenu([eighteen],[],18).
tenu([nineteen],[],19).
tenu([T],[],N) :- tenmult(T,N).
tenu([T,D],[],N+M) :- tenmult(T,N), digit(D,M).
digit(one,1).
digit(two,2).
digit(three,3).
% digit(four,4).
% digit(five,5).
digit(six,6).
digit(seven,7).
digit(eight,8).
digit(nine,9).
tenmult(twenty,20).
tenmult(thirty,30).
tenmult(forty,40).
tenmult(fifty,50).
tenmult(sixty,60).
tenmult(seventy,70).
tenmult(eighty,80).
tenmult(ninety,90).

% 100 randomly sampled examples

wordnum([one,hundred,and,twenty,two],[],1*100+(20+2)).
wordnum([two,hundred,and,thirty,two],[],2*100+(30+2)).
wordnum([three,hundred,and,seventy,one],[],3*100+(70+1)).
wordnum([five,hundred,and,eleven],[],5*100+11).
wordnum([five,hundred,and,forty],[],5*100+40).
wordnum([five,hundred,and,eighty,four],[],5*100+(80+4)).
wordnum([six,hundred,and,seventy,eight],[],6*100+(70+8)).
wordnum([seven,hundred,and,forty,two],[],7*100+(40+2)).
wordnum([eight,hundred,and,eighty,two],[],8*100+(80+2)).
wordnum([nine,hundred,and,sixteen],[],9*100+16).
wordnum([nine,hundred,and,thirty,nine],[],9*100+(30+9)).
wordnum([nine,hundred,and,seventy,eight],[],9*100+(70+8)).
wordnum([one,thousand,and,eighty,five],[],1*1000+(80+5)).
wordnum([two,thousand,and,ninety,four],[],2*1000+(90+4)).
wordnum([three,thousand,and,fifty,four],[],3*1000+(50+4)).
wordnum([three,thousand,and,eighty,seven],[],3*1000+(80+7)).
wordnum([four,thousand,and,twenty,seven],[],4*1000+(20+7)).
wordnum([five,thousand,and,twenty,six],[],5*1000+(20+6)).
wordnum([five,thousand,and,forty,two],[],5*1000+(40+2)).
wordnum([five,thousand,and,eighty,nine],[],5*1000+(80+9)).
wordnum([six,thousand,and,thirty,nine],[],6*1000+(30+9)).
wordnum([eight,thousand,and,seventy,three],[],8*1000+(70+3)).
wordnum([nine,thousand,and,eighteen],[],9*1000+18).
wordnum([nine,thousand,and,sixty,five],[],9*1000+(60+5)).
wordnum([one,thousand,one,hundred,and,twelve],[],1*1000+(1*100+12)).
wordnum([one,thousand,one,hundred,and,thirty,eight],[],1*1000+(1*100+(30+8))).
wordnum([one,thousand,one,hundred,and,forty,three],[],1*1000+(1*100+(40+3))).
wordnum([one,thousand,one,hundred,and,seventy,seven],[],1*1000+(1*100+(70+7))).
wordnum([one,thousand,four,hundred,and,eighty,three],[],1*1000+(4*100+(80+3))).
wordnum([one,thousand,five,hundred,and,eighty,five],[],1*1000+(5*100+(80+5))).
wordnum([one,thousand,six,hundred,and,ninety,nine],[],1*1000+(6*100+(90+9))).
wordnum([one,thousand,seven,hundred,and,fifty,seven],[],1*1000+(7*100+(50+7))).
wordnum([one,thousand,eight,hundred,and,sixteen],[],1*1000+(8*100+16)).
wordnum([two,thousand,three,hundred,and,forty,two],[],2*1000+(3*100+(40+2))).
wordnum([two,thousand,four,hundred,and,forty,two],[],2*1000+(4*100+(40+2))).
wordnum([two,thousand,six,hundred,and,ten],[],2*1000+(6*100+10)).
wordnum([two,thousand,six,hundred,and,sixty,two],[],2*1000+(6*100+(60+2))).
wordnum([three,thousand,one,hundred,and,nine],[],3*1000+(1*100+9)).
wordnum([three,thousand,one,hundred,and,ninety,five],[],3*1000+(1*100+(90+5))).
wordnum([three,thousand,two,hundred,and,eleven],[],3*1000+(2*100+11)).
wordnum([three,thousand,two,hundred,and,eighty,eight],[],3*1000+(2*100+(80+8))).
wordnum([three,thousand,four,hundred,and,ninety,seven],[],3*1000+(4*100+(90+7))).
wordnum([three,thousand,five,hundred,and,ninety,six],[],3*1000+(5*100+(90+6))).
wordnum([three,thousand,six,hundred,and,fifty,one],[],3*1000+(6*100+(50+1))).
wordnum([three,thousand,nine,hundred,and,fifty,two],[],3*1000+(9*100+(50+2))).
wordnum([three,thousand,nine,hundred,and,fifty,three],[],3*1000+(9*100+(50+3))).
wordnum([four,thousand,two,hundred,and,ninety],[],4*1000+(2*100+90)).
wordnum([four,thousand,two,hundred,and,twenty,one],[],4*1000+(2*100+(20+1))).
wordnum([four,thousand,two,hundred,and,forty,six],[],4*1000+(2*100+(40+6))).
wordnum([four,thousand,two,hundred,and,fifty,one],[],4*1000+(2*100+(50+1))).
wordnum([four,thousand,two,hundred,and,fifty,six],[],4*1000+(2*100+(50+6))).
wordnum([four,thousand,three,hundred,and,three],[],4*1000+(3*100+3)).
wordnum([four,thousand,three,hundred,and,seventy,seven],[],4*1000+(3*100+(70+7))).
wordnum([four,thousand,four,hundred,and,fifty,six],[],4*1000+(4*100+(50+6))).
wordnum([four,thousand,five,hundred,and,thirteen],[],4*1000+(5*100+13)).
wordnum([four,thousand,six,hundred,and,nine],[],4*1000+(6*100+9)).
wordnum([four,thousand,six,hundred,and,sixty,seven],[],4*1000+(6*100+(60+7))).
wordnum([four,thousand,nine,hundred,and,twenty,two],[],4*1000+(9*100+(20+2))).
wordnum([five,thousand,one,hundred,and,sixty],[],5*1000+(1*100+60)).
wordnum([five,thousand,six,hundred,and,fifty,three],[],5*1000+(6*100+(50+3))).
wordnum([five,thousand,seven,hundred,and,three],[],5*1000+(7*100+3)).
wordnum([five,thousand,nine,hundred,and,sixty,seven],[],5*1000+(9*100+(60+7))).
wordnum([five,thousand,nine,hundred,and,eighty,four],[],5*1000+(9*100+(80+4))).
wordnum([six,thousand,one,hundred,and,ninety,nine],[],6*1000+(1*100+(90+9))).
wordnum([six,thousand,two,hundred,and,forty],[],6*1000+(2*100+40)).
wordnum([six,thousand,two,hundred,and,seventy,two],[],6*1000+(2*100+(70+2))).
wordnum([six,thousand,three,hundred,and,sixty,one],[],6*1000+(3*100+(60+1))).
wordnum([six,thousand,five,hundred,and,seventy,nine],[],6*1000+(5*100+(70+9))).
wordnum([six,thousand,six,hundred,and,forty,nine],[],6*1000+(6*100+(40+9))).
wordnum([six,thousand,seven,hundred,and,twenty],[],6*1000+(7*100+20)).
wordnum([six,thousand,nine,hundred,and,twenty,two],[],6*1000+(9*100+(20+2))).
wordnum([six,thousand,nine,hundred,and,fifty,seven],[],6*1000+(9*100+(50+7))).
wordnum([seven,thousand,two,hundred,and,ninety,six],[],7*1000+(2*100+(90+6))).
wordnum([seven,thousand,four,hundred,and,sixty,one],[],7*1000+(4*100+(60+1))).
wordnum([seven,thousand,four,hundred,and,ninety,seven],[],7*1000+(4*100+(90+7))).
wordnum([seven,thousand,five,hundred,and,forty],[],7*1000+(5*100+40)).
wordnum([seven,thousand,five,hundred,and,eighty,one],[],7*1000+(5*100+(80+1))).
wordnum([eight,thousand,one,hundred,and,seventy],[],8*1000+(1*100+70)).
wordnum([eight,thousand,one,hundred,and,thirty,six],[],8*1000+(1*100+(30+6))).
wordnum([eight,thousand,two,hundred,and,seventy],[],8*1000+(2*100+70)).
wordnum([eight,thousand,five,hundred,and,twenty,nine],[],8*1000+(5*100+(20+9))).
wordnum([eight,thousand,five,hundred,and,thirty,one],[],8*1000+(5*100+(30+1))).
wordnum([eight,thousand,five,hundred,and,forty,seven],[],8*1000+(5*100+(40+7))).
wordnum([eight,thousand,six,hundred,and,eighteen],[],8*1000+(6*100+18)).
wordnum([eight,thousand,six,hundred,and,eighty,four],[],8*1000+(6*100+(80+4))).
wordnum([eight,thousand,eight,hundred,and,thirty],[],8*1000+(8*100+30)).
wordnum([eight,thousand,eight,hundred,and,twenty,one],[],8*1000+(8*100+(20+1))).
wordnum([eight,thousand,eight,hundred,and,fifty,two],[],8*1000+(8*100+(50+2))).
wordnum([eight,thousand,eight,hundred,and,fifty,nine],[],8*1000+(8*100+(50+9))).
wordnum([eight,thousand,nine,hundred,and,thirty,three],[],8*1000+(9*100+(30+3))).
wordnum([nine,thousand,one,hundred,and,fifteen],[],9*1000+(1*100+15)).
wordnum([nine,thousand,two,hundred,and,eight],[],9*1000+(2*100+8)).
wordnum([nine,thousand,four,hundred,and,forty,seven],[],9*1000+(4*100+(40+7))).
wordnum([nine,thousand,four,hundred,and,fifty,five],[],9*1000+(4*100+(50+5))).
wordnum([nine,thousand,seven,hundred,and,thirty,six],[],9*1000+(7*100+(30+6))).
wordnum([nine,thousand,seven,hundred,and,fifty,eight],[],9*1000+(7*100+(50+8))).
wordnum([nine,thousand,eight,hundred,and,forty,seven],[],9*1000+(8*100+(40+7))).
wordnum([nine,thousand,eight,hundred,and,fifty,six],[],9*1000+(8*100+(50+6))).
wordnum([nine,thousand,nine,hundred,and,fifty,nine],[],9*1000+(9*100+(50+9))).
wordnum([nine,thousand,nine,hundred,and,ninety,nine],[],9*1000+(9*100+(90+9))).

