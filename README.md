# RuDaS: Synthetic dataset generation code and evaluation tools for ILP

RuDaS (*S*ynthetic *Da*tasets for *Ru*le Learning), is a tool for generating synthetic datasets containing both facts and rules, and for evaluating rule learning systems, that overcomes the above mentioned shortcomings of existing datasets and proper evaluation methods. 

RuDaS is highly parameterizable; for instance, number of constants, predicates, facts, consequences of rules (i.e., completeness) amount of noise (e.g., wrong or missing facts) and kinds of dependencies between rules can be selected. 

Moreover, RuDaS allows for assessing the performance of rule learning systems by computing classical and more recent metrics, including a new one that we introduce.

In this repository there is also the code (see experiments/README) we used to evaluate representatives of different types of rule learning systems on our datasets demonstrating the necessity of having a diversified portfolio of datasets to help revealing the variety in the capabilities of the systems and thus also to support and help researchers in developing and optimizing new/existing approaches. 

*Paper:* <!-- add link to ArXiv wgenb we have it-->

Requirements: Python 3 
<!--TODO any others check. then create file requirements.txt)-->
See experiments/README for additional requirements for running the experiments


## Dataset generation code

* rules can be arbitrary long and with n-ary predicates
* anonimous constants and predicates = constants c1 c2 .. , predicates p1 p2 ..
* format: prolog standard, 2 different files type: one for facts one for rules
* parameters:
   * number of constants
   * number of predicates
   * min/max arity of predicates
   * number ofrules
   * maximal length of rules
   * number of reasoning steps (depht of the tree or number of total steps)
   * connected components rules category
   * min/max number of connected components
   * maximal depth of rule graphs 
   * dataset size: XS, S, M, L, XL
   * open-world degree n_OW in [0, 1]
   * amount of noise in the data nNoise+ , nNoise- in [0, 1]
* Noise:
   * adding fact that are not necessary to prove the goal: nNoise+
   * removing support facts: nNoise-
   * removing consequences facts: n_OW
* categories to show capabilities of the ILP system:
   * Chain -> h:-b1,b2. b1:-a1,a2. a1:-c1,c2
   * Rooted Directed Graph (DG) -> h:-b1,b2. b1:-a1,a2. b2:-c1,c2. a1:-d1,d2. a2:-d3,d4. ...
   * Disjunctive Rooted DG -> different rules same head: h:-b1,b2. h:-a1,a2. h:-c1,c2.
   * Mixed -> mix of the above.
   * All of them can have recursion -> h(X):-h(Y),b1(X,Y). b1(X,Y):-b1(Z,Y),b2(X,Z)

<!--TODO: Add figure for categories-->

## Evaluation Tools for ILP code

Evaluation tool to compute distances between logic programs:
* *Herbrand distance:* the traditional distance between Herbrand mod- els; two normalized versions of the Herbrand distance
* *Herbrand accuracy:* (H-accuracy), Herbrand distance normalized on the Herbrand universe
* *Herbrand score:* (H- score), a new metric we propose in this paper;
* *Accuracy*
* *Precision* (or standard confidence)
* *Recall*
* *F1-score*


Predicate invention: we don't penalize them in the evaluation


## Future extensions
* Probabilistic dataset: generate probabilistic facts and/or rules
* More expressive logic: for example full first order or higher order
<!--ADD MORE-->

## Available Datasets Description

<!--TODO TABLE-->

|dataset| n. | type | Size | Depth | n.Rules | n.Facts n.Pred n.Const
| --- | --- |--- | --- | --- | --- | --- | --- | --- |
| name | | | | | | | | |
| ... | | | | | | | | |
| name | | | | | | | | |

