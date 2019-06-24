# Synthetic dataset generation code and evaluation tools for ILP

Requirements: Python 3 (TODO any others? check. then create file requirements.txt)

See experiments/README for additional requirements for running the experiments


## Dataset generation code

* rules can be arbitrary long and with n-ary predicates
* predicate invention: we don?t want them in the dataset and rules:
   * we don?t want to have auxiliary predicates in the dataset
   * having them would imply that the theory can be expressed using binary rules and so we will not need predicate invention in our system
   * we could just eliminate them from the facts but since the dataset is automatically generated, we can not distinguish them from the name compared to normal predicate
* parameter: number of constants, predicates, rules
* parameter: number of steps (depht of the tree or number of total steps)
* categories to show capabilities of the ILP system:
   * recursion -> h(X):-h(Y),b1(X,Y). b1(X,Y):-b1(Z,Y),b2(X,Z)
   * trees -> h:-b1,b2. b1:-a1,a2. b2:-c1,c2. a1:-d1,d2. a2:-d3,d4. ...
   * forest -> different rules same head: h:-b1,b2. h:-a1,a2. h:-c1,c2.
   * chains -> h:-b1,b2. b1:-a1,a2. a1:-c1,c2
   * mix -> expecially for training
* anonimous constants and predicates = constants c1 c2 .. , predicates p1 p2 ..
* format: prolog standard, 2 different files, one for facts one for rules
* Noise:
   * adding fact that are not necessary to prove the goal [close world assumption]
   * removing fact [close world assumption]
   * adding negative facts [open world assumption]

## Evaluation Tools for ILP code

Evaluation tool to compute distances between logic programs:

* *Distance1*: distance between Herbrand interpretations (fuzzy distance)
* *Distance2*: directly on rules (logic programs) (we don?t count auxiliary pred) <add citation of the paper>

## Future extensions
* Probabilistic dataset: generate probabilistic facts and/or rules
* More expressive logic: for example full first order or higher order
