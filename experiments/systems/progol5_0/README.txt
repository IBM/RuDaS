Progol5.0 source code and examples (16.3.09)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Contained in this directory is the source code, examples and man page
for the Inductive Logic Programming system Progol (Version 5.0)
described in the paper ``Logical backpropagation'' by Stephen
Muggleton and Chris Bryant (submitted to ILP00).

Progol is freely available for academic research and teaching. Progol is
also available under license for commercial research. To apply for such a
license, please write to s.muggleton@imperial.ac.uk.

Progol5.0 extends Progol4.2 in the following ways.

1. Multiple predicate learning is implemented. Examples given
	for predicate p can be used to generalise predicate q,
	where q is below p in the calling diagram. This is closely related
	to "abduction" in logic programming, though the mechanism
	is that of inverse entailment (see ``Inverse entailment and
	Progol'' by Stephen Muggleton which appears in the New Generation
	Computing Journal, Vol. 13, pp. 245-286 1995). Abductive instances for
	q are constructed from instances of p by use of contra-positive
	clauses (in a similar way to that used by Stickel's Prolog Technology
	Theorem Prover). Thus for q, an additional predicate definition
	is constructed for '~q' from clauses having q atoms in their bodies.
	The control strategy repeatedly takes examples of p and
	maximises generalisations of the instances of all choices for q.
	The choice for p can be limited by the user specifying that
	p is an oservable predicate by adding something like the following
	into the Progol input file.

	:- observable(p/1)?

	Without such statements, all predicates are assumed observable.
2. A resolution proof length bound (r) has been implemented in addition to the
	existing proof depth bound (h). This makes learning of recursive
	predicates much more efficient. r has a default setting of 400.
3. The default setting for the limit on the number of search nodes has been
	reduced to 200.
4. It is possible to switch off full pruning in Progol5.0.
	When full pruning is switched off Progol5.0 will consider all
	hypotheses with maximal compression. Full pruning (the default)
	is deactivated as follows.

		  :- unset(full_pruning)?

	As usual you can view the state of settings using the following
	command.

		  :- settings?

Progol5.0 is not described in any written paper yet, since it is still
being developed and experimented on.

This directory contains the following.

	progol5_0.tar.gz
	expand.sh

Now do the following shell command:

$ expand.sh

This should produce the subdirectories, `examples5.0' and
`source'. progol and qsample will be automatically compiled in
subdirectory `source'.

The examples5.0 directory contains two theories, grammar.pl
and numbers.pl. You can test these out by removing some of the
clauses in the theory and getting Progol5.0 to reconstruct them.
The file numbers.eps graphs the results of an experiment in
which randomly chosen subsets of the complete theory for
numbers.pl were left out. Accuracy is plotted against the
percentage of the initial theory left out both before and
after reconstruction by Progol5.0. The graph shows that
leaving 10% of the theory out dramatically reduces accuracy,
though recovery by Progol5.0 is almost complete. However,
if 50% of the theory is discarded, the theory is irrecoverable
by Progol5.0.

Include progol's source directory in your .login file path entry to allow
you to use progol as a command.  Remember to source your .login file before
continuing.

Try running Progol on all of the examples and see what happens.



*****************************************************************************
Professor Stephen H Muggleton            Tel: +44 20 7594 8307 (direct)
Computational Bioinformatics Laboratory  Bridget Gundry: +44 20 7594 1245
Department of Computing                  Fax: +44 20 7581 8024
Imperial College                         Email: shm@doc.ic.ac.uk
180 Queens Gate                          URL: http://www.doc.ic.ac.uk/~shm/
London SW7 2BZ
United Kingdom
*****************************************************************************
