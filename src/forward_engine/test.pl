p(X):-q(X).
r(X,Y):-p(Y),p(X).
r(c,d).
q(a).
q(b).
:- initialization forall(r(X,Y), writeln(r(X,Y))).
:- initialization forall(p(X), writeln(p(X))).
:- initialization forall(q(X), writeln(q(X))).
:- initialization halt.