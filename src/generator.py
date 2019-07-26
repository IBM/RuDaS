'''Python 3
Dataset Generator
'''
from enum import Enum
import numpy as np
import shutil
import itertools
import os
import sys
import random
import copy
import math
import logic as log
import graph
import operator
import functools
import inspect
import argparse
import time
import signal

class DatasetCategory(Enum):
    '''
    Description
    ----------
        0   = mixed dags (default, expecially useful for training)
        1   = chain (h:-b1,b2. b1:-a1,a2. a1:-c1,c2)
        3   = tree-dags (h:-b1,b2. b1:-a1,a2. b2:-c1,c2. a1:-d1,d2. a2:-d3,d4. ... )
        5   = tree-dags+or_nodes (different rule with the same head h:-b1,b2. h:-a1,a2. h:-c1,c2.)
        recursive versions (h(X):-h(Y),b1(X,Y). b1(X,Y):-b1(Z,Y),b2(X,Z))
    '''
    MIXED = 0 #is DISJUNCTIVE_ROOTED_DAG with RECURSION
    CHAIN = 1
    CHAIN_RECURSIVE = 2 #is CHAIN with recursion(self loops)
    ROOTED_DAG = 3
    ROOTED_DAG_RECURSIVE = 4
    DISJUNCTIVE_ROOTED_DAG = 5
    DISJUNCTIVE_ROOTED_DAG_RECURSIVE = 6


class DatasetSize(Enum):
    XS = (50, 100, 3)
    S = (101, 1000, 5)
    M = (1001,10000, 10)
    L = (10001, 100000, 20)
    XL = (100001, 500000, 50)
    # XXL = ()

    def __init__(self, min, max, nsupport):
        self.min = min
        self.max = max
        self.nsupport = nsupport   #count


SIZE_EVAL_SUPPORT = 100

TRAIN = "train"
# TRAIN1 = "train"
# TRAIN2 = "valid"
EVAL = "eval"

CW = "-cw"
OW = "-ow"
NOISE = "-n"
SUPPORT = "-support"
CONSEQS = "-conseqs"

F_EXT = ".txt"

## EVAL
# CWA = support + consequences
F_EVAL_CW = EVAL + CW
F_EVAL_SUPPORT = EVAL + SUPPORT
F_EVAL_CONSEQS = EVAL + CONSEQS
## FULL TRAIN (= train+valid in one file)
F_TRAIN_CW = TRAIN + CW
# closed-world data split into
F_TRAIN_SUPPORT = TRAIN + SUPPORT
F_TRAIN_CONSEQS = TRAIN + CONSEQS
# OWA: SOME MISSING CONSEQUENCES, ACCORDING TO owa_factor PARAMETER
F_TRAIN_OW = TRAIN + OW
# NOISE: ACCORDING TO noise_factor PARAMETERS
F_TRAIN_CW_N = TRAIN + CW + NOISE
F_TRAIN_OW_N = TRAIN
## files where above train is split into train and valid
# F_TRAIN1_CW = TRAIN + CW + F_EXT
# F_TRAIN1_OW = TRAIN + OW + F_EXT
# F_TRAIN1_CW_N = TRAIN + CW + NOISE + F_EXT
# F_TRAIN1_OW_N = TRAIN + F_EXT
# F_TRAIN2_CW = TRAIN + CW + F_EXT
# F_TRAIN2_OW = TRAIN + OW + F_EXT
# F_TRAIN2_CW_N = TRAIN + CW + NOISE + F_EXT
# F_TRAIN2_OW_N = TRAIN + F_EXT
## the consequences that are not in F_TRAIN are provided as test data
F_TEST = "test"
## below refer to F_TRAIN_CWA (so that every entity etc. in test is also in entities)
F_RELATIONS = "relations"
F_ENTITIES = "entities"

STDOUT = sys.stdout

def handler(signum, frame):
    # print("Forever is over!")
    raise Exception("end of time")

# NOTE
# with several dags we vary size. first is of maxdepth and others varying randomly within max bound
# overlap true might "destroy" chains or individual structs
# for given constant and predicate number the test if they are sufficient is not very exact
def generate_dataset(name=None, path="../datasets/", size=DatasetSize.S, category=DatasetCategory.MIXED, overlap=False, \
                     singletarget=False, mindags=1, maxdags=1, maxdepth=3, nodesupport=None, dagsupport=3, skipnode=5, \
                     owafactor=.3, noisefactor=.2, missfactor=.15, targetsextra=True, maxorchild=2, maxatoms=2, minarity=2, maxarity=2, \
                     numpreds=None, numconstants=None, test=.3): #, valid=.2):

    '''TODO in the end: update these descriptions
    :param name: the name of the dataset directory that is created
    :param path: the system path where to create the dataset directory
    :param size: value of enum DatasetSize; requested size of test dataset for owa, train dataset and cwa versions will be larger.
    :param category: value of enum DatasetCategory
    :param overlap: boolean value; if overlap = False, then generated DAGs do not share predicates
    :param singletarget: boolean value; if singletarget = True, then all DAGs will share the same target
    :param mindags: minimum number of generated DAGs; must be > 0
    :param maxdags: maximum number of generated DAGs; must be >= mindags
    :param maxdepth: maximum depth of a single DAG
    :param nodesupport: number of complete instantiations of a rule; must be >= 0 and does not count towards size of dataset;
                        if left None, then it is initialized according to size's nsupport attribute
    :param dagsupport: share of facts that are part of a complete instantiations of the DAG;
                       1 means every support fact is part of some full dag instantiation, 2 means about half are, etc.
    :param skipnode: number expressing probability that a node is skipped during DAG instantiation when generating support;
                     n means node is skipped with probability is (100/(n+1))%
    :param owafactor: value in [0,1] describing the percentage of consequences missing in the owa version of the dataset
    :param noisefactor: value in [0,1] describing the percentage of noise in the datasets
    :param targetsextra: boolean value; if true owafactor and noisefactor are reflected specifically in target atoms too
    :param maxorchild: maximum in degree of OR nodes (minimum is 2)
    :param maxatoms: maximum number of body atoms in a rule
    :param minarity: minimum arity of predicates
    :param maxarity: maximum arity of predicates
    :param numpreds: number of predicates to be used; if left None, then it is initialized in accordance with size
    :param numconstants: number of constants to be used; if left None, then it is initialized in accordance with size
    :return: nothing; generates different versions of a dataset in the directory requested
    '''
    ## initialize basic parameters
    # TODO in the end: test all params eg >=0 ...

    signal.signal(signal.SIGALRM, handler)

    if maxdags < mindags:
        raise ValueError("Parameter maxdags must not be less than parameter mindags.")

    if maxarity < minarity:
        raise ValueError("Parameter maxarity must not be less than parameter minarity.")

    # TODO not sure about this
    if owafactor < test:
        raise ValueError("Parameter owafactor must not be less than parameter test.")

    if name is None or name == "":
        if category == DatasetCategory.CHAIN or category == DatasetCategory.CHAIN_RECURSIVE:
            name = "CHAIN"
        elif category == DatasetCategory.ROOTED_DAG or category == DatasetCategory.ROOTED_DAG_RECURSIVE:
            name = "RDG"
        elif category == DatasetCategory.DISJUNCTIVE_ROOTED_DAG or category == DatasetCategory.DISJUNCTIVE_ROOTED_DAG_RECURSIVE:
            name = "DRDG"
        elif category == DatasetCategory.MIXED:
            name = "MIXED"

        name += '-' + str(size.name) + '-' + str(maxdepth)
        if os.path.exists(path + name + '/'):
            i = 0
            while os.path.exists(path + name + '-' + str(i) + '/'):
                i += 1
            name += '-' + str(i)

    path = path + name + '/'
    if os.path.exists(path):
        shutil.rmtree(path, ignore_errors=True)
    os.makedirs(path, )
    sys.stdout = STDOUT
    print("generating dataset", name)

    sys.stdout = open(path+"log.txt", 'a')
    time.sleep(2) # wait two seconds to get switch
    # print("dataset:", name) is in parameters so printed below


    f = inspect.currentframe()
    v = inspect.getargvalues(f)

    for arg in v.args:
        if arg == "path": continue
        var_value = v.locals[arg]
        if var_value:
            print( arg+':', var_value)

    # ignore that parameter if no disjunctive DAG
    if category not in [DatasetCategory.DISJUNCTIVE_ROOTED_DAG, DatasetCategory.DISJUNCTIVE_ROOTED_DAG_RECURSIVE, DatasetCategory.MIXED]:
        maxorchild = 1
    elif maxorchild < 2:
        raise ValueError("Parameter maxorchild must not be less than two.")

    maxchild = max(maxatoms, maxorchild)

    if nodesupport is None:
        nodesupport = size.nsupport
    elif nodesupport < 0:
        raise ValueError("Parameter nodesupport must not be less than zero.")


    numdags = random.randint(mindags, maxdags)
    # NOTE the below numbers are approximations because maxdepth varies for all dags
    # maxdepth/2 represents "average maxdepth"
    # first 1 is chain; does not really depend on maxdepth
    minnumdagleafatoms = 1 ** (maxdepth // 2) * 1
    # we do not multiply by 2 (head + body atom) because head is same as body in node before
    minnumdagatoms = sum([1 ** i for i in range(maxdepth // 2 + 1)]) if numpreds is None else \
                     min(sum([1 ** i for i in range(maxdepth // 2 + 1)]), numpreds) if overlap else \
                     min(sum([1 ** i for i in range(maxdepth // 2 + 1)]), numpreds // numdags)
    maxnumdagatoms = sum([maxchild ** i for i in range(maxdepth // 2 + 1)]) * maxatoms

    # include one check when numpreds > 0. otherwise we might generate a strange dataset containing only samples
    if numpreds is not None:
        # we require more than one predicate for at least one DAG
        if numpreds <= 2:
            raise ValueError("Parameter numpreds must be greater than two.")
        if not overlap and numpreds//numdags <= 1:
            raise ValueError("Parameter numdags too large for given number of predicates.")
    # 5 is just an offset to have some more, loops are not considered here (i.e., just look at maxatoms)
    numpreds = numpreds if numpreds is not None and numpreds > 0 else numdags * maxnumdagatoms + 5

    # arity 1 less often for predicates (only in m1% of cases, with m1 <= 10 for first target and m1<=25 for others)
    m1 = max(maxarity - minarity, 9)
    m2 = max(maxarity - minarity, 3)
    m3 = 2 if minarity == 1 and maxarity > 1 else minarity
    # TODO think about extending this to other targets
    target0arity = random.randint(m3, maxarity) if minarity > 1 or random.randint(0, m1) > 0 else 1
    # other predicates
    predicates = [log.Predicate(i, target0arity if i == 0 else
                                                   random.randint(m3, maxarity) if minarity > 1
                                                                                   or random.randint(0, m2) > 0 else 1)
                  for i in range(numpreds)]
    random.shuffle(predicates)

    # we use /2 because we apply the factor to entire size but it actually only refers to consequences
    sizefactor1 = 1/(1-owafactor/2)
    # consider node skipping in instantiation/generation with factor +1/skipnode
    sizefactor2 = (1+((dagsupport-1)/dagsupport) * 1/skipnode if skipnode else 1)
    # fixe size of dataset
    fixedsize = int((size.min + int((size.max - size.min) * random.random())) * sizefactor1)
    # ideally assume dataset contains 60% of all possible facts; ie we have constants and predicates to actually generate more facts than fixedsize
    generatablesize = int(fixedsize * (1+1/skipnode if skipnode else 1) * 10//6)
    mingeneratablesize = int(size.min * sizefactor1 * sizefactor2)

    if numconstants is not None and \
            size_dag_grounding(numdags, minnumdagatoms, minnumdagleafatoms, numconstants, overlap) < mingeneratablesize:
        raise ValueError("Parameter numconstants is most likely too small to generate dataset of requested size.")

    if numconstants is None:
        # minimum
        numconstants = math.ceil((generatablesize/(numdags * maxnumdagatoms))**(1/maxarity))
        while size_dag_grounding(numdags, minnumdagatoms, minnumdagleafatoms, numconstants, overlap) < generatablesize:
            numconstants += 1
    else: # adapt fixedsize
        while size_dag_grounding(numdags, minnumdagatoms, minnumdagleafatoms, numconstants, overlap) < fixedsize \
                and fixedsize > mingeneratablesize:
            fixedsize -= 1

    constants = [log.Constant(i) for i in range(numconstants)]

    print("sizefactor1: ", sizefactor1)
    print("sizefactor2: ", sizefactor2)
    print("fixedsize: ", fixedsize)
    print("generatablesize: ", generatablesize)
    print("mingeneratablesize: ", mingeneratablesize)
    print("numpredicates: ",len(predicates))
    print("numconstants: ",len(constants))
    #######################################

    generated_initial = False
    while not generated_initial:
        try:
            signal.alarm(180)

            dags = []
            var_doms = []
            if singletarget:
                target = predicates[0]
            preds = np.array_split(predicates, numdags) if not overlap else predicates #oif sinfletar get add at first everywheer
            for i in range(numdags):
                steps = maxdepth if i == 0 else random.randint(1, maxdepth)
                category1 = category if category != DatasetCategory.MIXED else list(DatasetCategory)[random.randint(1,len(DatasetCategory)-1)]
                dags.append(generate_inference_structure(category1, target if singletarget else (preds[i] if overlap else preds[i][0]), \
                                                         preds if overlap else preds[i], constants, maxatoms, steps, maxorchild, 1))

                var_doms.append({v.name: (get_var_domain(constants) if v.name not in dags[i].const_variables \
                              else [dags[i].const_variables[v.name]]) for v in dags[i].variables})

            ## generate minimal set of facts to learn individual rules
            nsupport = {p.name: [] for p in predicates}
            if nodesupport:
                for i in range(numdags):
                    for n in dags[i].nodes:
                        for _ in range(nodesupport):
                            for f in generate_nsupport(n, get_assignment(var_doms[i])):
                                nsupport[f.predicate.name].append(f)
                print("nsupport size:",len(list(itertools.chain(*nsupport.values()))))

            ## generate additional facts by going through tree several times
            assignments = [[] for _ in dags]
            targets = [predicates[i].name for i in range(numdags)] if overlap else [p[0].name for p in preds]
            train_cwa, train_support = generate_facts(dags, predicates, var_doms, assignments, dagsupport, skipnode, fixedsize, 0, support=nsupport)
            eval_cwa, eval_support = generate_facts(dags, predicates, var_doms, assignments, dagsupport, skipnode, 0, SIZE_EVAL_SUPPORT)

            signal.alarm(0)
            generated_initial = True

        except Exception:
            signal.alarm(0)

    train_conseqs1 = extract_consequences(train_cwa, train_support, targets) # w/o target facts
    train_conseqs2 = extract_consequences({p:train_cwa[p] for p in train_cwa if p in targets}, train_support)
    eval_conseqs = extract_consequences(eval_cwa, eval_support)

    rules = list(itertools.chain.from_iterable([dag.get_rules() for dag in dags]))

    ## write first files
    write_rule_file(rules, path)
    write_fact_file(train_support, path, F_TRAIN_SUPPORT)
    write_fact_file(train_conseqs1+train_conseqs2, path, F_TRAIN_CONSEQS)
    write_fact_file(train_cwa, path, F_TRAIN_CW)
    write_fact_file(eval_support, path, F_EVAL_SUPPORT)
    write_fact_file(eval_conseqs, path, F_EVAL_CONSEQS)
    write_fact_file(eval_cwa, path, F_EVAL_CW)

    ## owa: remove some consequences
    train = copy.deepcopy(train_cwa)
    # valid = []
    test = []
    # train_conseqs = train_conseqs1 + train_conseqs2
    # train_conseqs.shuffle()
    rem = [] # must exist also if owa is 0
    if owafactor:
        if targetsextra:
            rem1 = remove_consequences(train, train_conseqs1, owafactor)
            rem2 = remove_consequences(train, train_conseqs2, owafactor)
            rem = rem1 + rem2
        else:
            rem = remove_consequences(train, train_conseqs1 + train_conseqs2, owafactor)

        # m1 = min(len(train)*valid, len(train_conseqs))
        # valid = train_conseqs[:m1]
        # m1 = min(len(train) * valid, len(train_conseqs))
        test = rem #[:len(rem)*(len(test)//owafactor)]

        write_fact_file(train, path, F_TRAIN_OW)
        write_fact_file(test, path, F_TEST)

    # TODO how to get a test set if we have no owa factor? or zero? or just assume we need to have it?
    # I opt for the latter since we anyway have the eval set in addition
    # ie if you had owa=0, you'd have to use that as evaluation

    ## noise
    if noisefactor:

        if targetsextra:
            add1, rem1 = add_noise(train, train_support, rem, targets, predicates, constants, noisefactor, missfactor)
            add2 = add_target_noise(train, rem, targets, predicates, constants, noisefactor)
        else:
            add1, rem1 = add_noise(train, train_support, rem, [], predicates, constants, noisefactor, missfactor)
            add2 = []

        write_fact_file(train, path, F_TRAIN_OW_N)

        for f in add1:
            train_cwa[f.predicate.name].append(f)
        for f in rem1:
            train_cwa[f.predicate.name].remove(f)
        for f in add2:
            train_cwa[f.predicate.name].append(f)

        write_fact_file(train_cwa, path, F_TRAIN_CW_N)

    # entities and relations refer to those in cwa w/o noise
    # s.t. we cannot end up with a test/valid file that contains entities not in the entities file
    cs = []
    for p in train_cwa:
        for f in train_cwa[p]:
            cs.extend(f.arguments)
    for p in train:
        for f in train[p]:
            cs.extend(f.arguments)
            # print(log.strlist(f.arguments))
    cs = [c for c in constants if c in cs]
    write_list_file([p for p in predicates if train_cwa[p.name] or train[p.name]], path, F_RELATIONS)
    write_list_file(cs, path, F_ENTITIES)

    # TODO in the end: maybe adapt this
    # TODO do we need statistics like noise etc for later evaluation tables if we have many datasets? then
    print_stats(size, category, dags, targets, train_support, train_conseqs1+train_conseqs2, train, train_cwa, eval_support, eval_conseqs, eval_cwa, test, path)


# TODO CHECK that! got confused totally
# the computed value is probably not exact (in some cases we can have duplicate atoms leading to an even smaller size)
# but should be close to the minimum
# conservative, consider chain, 1 atom in body, assume arity 1 (* (numconstants**minnumdagleafatoms^1)
# if DAGs may share predicates, then we must be even more conservative an can only consider one DAG
def size_dag_grounding(numdags, minnumdagatoms, minnumdagleafatoms, numconstants, overlap):
    # return minnumdagleafatoms * numconstants + (minnumdagatoms-minnumdagleafatoms) * minnumdagleafatoms * numconstants ... is same as
    # return (1 + (minnumdagatoms-minnumdagleafatoms)) * minnumdagleafatoms * numconstants
    # new
    size = (1 + (minnumdagatoms - minnumdagleafatoms)) * numconstants**minnumdagleafatoms
    if overlap:
        return size
    return numdags * size


# strict means that the generated dag has to be of specified category
# however, we currently ignore recursion ie a CHAIN_RECURSIVE etc. dataset does not have to contain a recursion
def generate_inference_structure(category, target, predicates, constants, max_atoms, steps, max_or_degree, strict):

    if category == DatasetCategory.CHAIN:
        dag = graph.ChainDAG(target, predicates, constants, max_atoms, steps, False)
    elif category == DatasetCategory.CHAIN_RECURSIVE:
        dag = graph.ChainDAG(target, predicates, constants, max_atoms, steps, True)
    elif category == DatasetCategory.ROOTED_DAG:
        dag = graph.RootedDAG(target, predicates, constants, max_atoms, steps, False, 1)
    elif category == DatasetCategory.ROOTED_DAG_RECURSIVE:
        dag = graph.RootedDAG(target, predicates, constants, max_atoms, steps, True, 1)
    elif category == DatasetCategory.DISJUNCTIVE_ROOTED_DAG:
        dag = graph.RootedDAG(target, predicates, constants, max_atoms, steps, False, max_or_degree)
    elif category == DatasetCategory.DISJUNCTIVE_ROOTED_DAG_RECURSIVE:
        dag = graph.RootedDAG(target, predicates, constants, max_atoms, steps, True, max_or_degree)

    if not strict or check_category(dag, category):
        print("generated DAG after " + str(strict) + " steps, maxdepth ",steps,":\n", str(dag))
        old = sys.stdout
        sys.stdout = STDOUT
        print("generated DAG after " + str(strict) + " steps, maxdepth ", steps, ":\n", str(dag))
        sys.stdout = old
        return dag

    return generate_inference_structure(category, target, predicates, constants, max_atoms, steps, max_or_degree, strict+1)


#we currently ignore recursion ie a CHAIN_RECURSIVE etc. dataset does not have to contain a recursion
def check_category(dag, category):
    if category == DatasetCategory.CHAIN or category == DatasetCategory.CHAIN_RECURSIVE:
        return True
    if category == DatasetCategory.ROOTED_DAG or category == DatasetCategory.ROOTED_DAG_RECURSIVE:
        for n in dag.nodes:
            if len(dag.get_relevant_children(n)) > 1:
                return True
    if category == DatasetCategory.DISJUNCTIVE_ROOTED_DAG or category == DatasetCategory.DISJUNCTIVE_ROOTED_DAG_RECURSIVE:
        if dag.contains_ornodes():
            return True
    return False


def get_var_domain(constants):
    cs = copy.deepcopy(constants)
    random.shuffle(cs)
    return cs #[0:random.randint(1, len(cs))]


def get_assignment(var_doms, assignments=[]):
    a = {v: var_doms[v][random.randint(0, len(var_doms[v]) - 1)] for v in var_doms}
    while a in assignments:
        a = {v: var_doms[v][random.randint(0, len(var_doms[v]) - 1)] for v in var_doms}

    assignments.append(a)
    return a


# TODO not sure if loop works with all assignments in memory, ie test for size L etc
def generate_facts(dags, predicates, var_doms, assignments, dagsupport, skipnode, minfacts, minsupport, support=None):
    # list of lists of leaves where we can start
    # each inner list contains alternative branches leading to same or node
    leaves = [dag.get_alternative_leaves(dag.root) for dag in dags]
    old = sys.stdout
    sys.stdout = STDOUT
    print("started")
    sys.stdout = old

    # a minimal set of body facts necessary to derive targets (w/o inferred consequences)
    support = {p.name: [] for p in predicates} if support is None else support
    # contains also consequences of support
    facts = {p.name: [] for p in predicates} if support is None else copy.deepcopy(support)

    i0 = 0
    # do not use numpy! it cannot handle too large numbers and then makes them negative what would mess up the below comparisons
    maxi0 = max([functools.reduce(operator.mul, [len(vs) for vs in d.values()],1) for d in var_doms])
    s = 0
    mins = minfacts if minfacts else minsupport
    while s < mins and i0 < maxi0:
        olds = s
        for i in range(len(dags)):
            # the variable assignment considered
            a = get_assignment(var_doms[i], assignments[i])
            # print(log.dict_tostr(a))
            # processed tree nodes (for which we generated support)
            processed = []
            l1 = [l1[random.randint(0, len(l1) - 1)] for l1 in leaves[i]]
            skip = 0 if dagsupport and i0 % dagsupport == 0 else skipnode
            i1 = 0
            while l1 and s < mins:
                # generate support for full tree for every assignment
                # support is in the end also part of facts!
                l1 = [n2 for n2 in (generate_support(n1, a, facts, support, processed, skip) \
                                    for n1 in l1) if n2 is not None]
                log.forward_inference(facts, dags[i].get_rules())

                s = len(list(itertools.chain(*facts.values()))) if minfacts else len(list(itertools.chain(*support.values())))
                # print(i0,i1, ":", str(s))
                i1 += 1
        i0 += 1
        if s > olds:
            print(str(i0), ":", str(s))
            old = sys.stdout
            sys.stdout = STDOUT
            print(str(i0), ":", str(s))
            sys.stdout = old

    # THE COMMENTED BELOW IS A NICER BUT LESS EFFICIENT VERSION WITH WHICH I GOT TROUBLE FOR SIZES LARGER THAN 1000
    # assignments = [product(list(doms.values())) for doms in var_doms]
    # print("numassignments:",",".join([str(len(a)) for a in assignments]))
    # # TODO not sure if loop works with all assignments in memory, ie test for size L etc
    #
    # i0 = 0
    # maxi0 = max([len(a) for a in assignments])
    # s = len(list(itertools.chain(*testfacts_cwa.values())))
    # while s < fixedsize and i0 < maxi0:
    #     olds = s
    #     for i in range(numdags):
    #         # the variable assignment considered
    #         a = dict(zip(var_doms[i].keys(), assignments[i][i0%len(assignments[i])]))
    #         # print(log.dict_tostr(a))
    #         # processed tree nodes (for which we generated support)
    #         processed = []
    #         l1 = [l1[random.randint(0, len(l1)-1)] for l1 in leaves[i]]
    #         skip = 0 if dagsupport and i0 % dagsupport == 0 else skipnode
    #         i1 = 0
    #         while l1 and s < fixedsize:
    #             # generate support for full tree for every assignment
    #             # support is in the end also part of facts!
    #             l1 = [n2 for n2 in (generate_support(n1, a, testfacts_cwa, support, processed, skip) \
    #                                 for n1 in l1) if n2 is not None]
    #             log.forward_inference(testfacts_cwa, dags[i].get_rules())
    #
    #             s = len(list(itertools.chain(*testfacts_cwa.values())))
    #             # print(i0,i1, ":", str(s))
    #             i1 += 1
    #     i0 += 1
    #     if s > olds:
    #         print(str(i0),":",str(s))

    print("stopped support generation after", i0, "steps")
    if i0 == maxi0:
        print(str(i0), ":", str(s))
        print("because maximal number of assignments reached.")
    print("generated", s, "facts")#, size.min is", size.min)
    # print(len(list(itertools.chain(*testfacts_cwa.values()))))
    return facts, support


# for all nodes in tree successively, randomly
# randomly, skip every skip-th node
# if skip=0, every node is considered
def generate_support(node, assignment, facts, support, processed, skip):
#    if isinstance(node, OrNode):
#        print("!! ornode in generate support "+str(node.nid))
#        return node.parent

    if node.nid in processed:  # processed through path of other child already
        return None

    cs = [c for c in node.children if c.parent.nid == node.nid]
    cs = [c for c in cs if isinstance(c, graph.RuleNode) and c.nid not in processed]
    if cs:  # not yet processable
        return node

    # generate support only sometimes!
    if not skip or random.randint(0, skip) > 0:
        for f in generate_nsupport(node, assignment):
           if f not in facts[f.predicate.name]:  # TODO CHECK not sure if this is required, but think so since we have lists (set would not work at other points because the atoms are not hashabl;e)
                facts[f.predicate.name].append(f)
                support[f.predicate.name].append(f)
    processed.append(node.nid)
    return node.parent if isinstance(node.parent, graph.RuleNode) else \
        (None if node.parent is None else node.parent.parent)

# for one node
def generate_nsupport(node, assignment):
    support = []
    for a in node.rule.body:
        f = copy.deepcopy(a)
        f.arguments = [assignment[t.name] if isinstance(t, log.Variable) else t for t in f.arguments]
        support.append(f)
    return support


def add_facts(fact_lst, fact_dict):
    for f in fact_lst:
        if f not in fact_dict[f.predicate.name]:
            fact_dict[f.predicate.name].append(f)


def extract_consequences(facts, support, targets=[]):
    consequences = [f for p in facts if p not in targets for f in facts[p] if f not in support[p]]
    random.shuffle(consequences)
    return consequences


def remove_consequences(facts, consequences, owa_factor):
    s = int(len(consequences) * owa_factor)
    print("owa: removed", s, "from", len(consequences), " consequences", )
    for f in consequences[0:s]:
        facts[f.predicate.name].remove(f)
    return consequences[0:s]


# NOTE that we only REMOVE support atoms (i.e., no consequences), but the share is computed based on all atoms
# we may ADD atoms whose predicate occurs in rule heads, if it is no target predicate
def add_noise(facts, support, removed, targets, predicates, constants, noise_factor, miss_factor):
    # remove n1 "correct" facts
    n0 = sum([len(support[p]) for p in support if p not in targets]) #support in one dag may contain target of other if overlap=True
    n1 = int(n0 * miss_factor)
    print("noise: ", n1, "of", n0, "support facts removed")#, len(consequences), "consequences)")

    rem = []
    ps = [p for p in support if len(support[p]) and p not in targets]
    for _ in range(n1):
        p = ps[random.randint(0,len(ps)-1)]
        f = support[p][random.randint(0,len(support[p])-1)]
        while f in rem:
            p = ps[random.randint(0, len(ps) - 1)]
            f = support[p][random.randint(0, len(support[p]) - 1)]
        facts[p].remove(f)
        rem.append(f)
        if not len(support[p]):
            ps.remove(p)

    # add n1 "irrelevant" facts
    n0 = sum([len(facts[p]) for p in facts if p not in targets])
    n1 = int(n0 * noise_factor // (1 - noise_factor))
    print("noise: ", n1, "facts added")#, len(consequences)+len(support), "facts")

    add = []
    ps = [p for p in predicates if p.name not in targets]
    for _ in range(n1):
        p = ps[random.randint(0,len(ps)-1)]
        f = log.Atom(p, [constants[random.randint(0,len(constants)-1)] for _ in range(p.arity)])
        while f in rem or f in removed or f in facts[p.name]: #TODO can we ensure that this terminates?
            p = ps[random.randint(0, len(ps) - 1)]
            f = log.Atom(p, [constants[random.randint(0, len(constants) - 1)] for _ in range(p.arity)])
        facts[p.name].append(f)
    return add, rem


# NOTE that we do NOT remove targets (= consequences)
# ie we only add atoms
def add_target_noise(facts, removed, targets, predicates, constants, noise_factor):
    n0 = sum([len(facts[p]) for p in facts if p in targets])
    n1 = int(n0 * noise_factor // (1 - noise_factor))
    print("noise (target):",n1,"facts added to",n0,"original target facts")
    # rem = []
    # ps = [p for p in facts if len(facts[p]) and p in targets]
    # for _ in range(n1):
    #     p = ps[random.randint(0,len(ps)-1)]
    #     f = facts[p][random.randint(0,len(facts[p])-1)]
    #     facts[p].remove(f)
    #     rem.append(f)
    #     if not len(facts[p]):
    #         ps.remove(p)

    add = []
    ps = [p for p in predicates if p.name in targets]
    for _ in range(n1):
        p = ps[random.randint(0,len(ps)-1)]
        f = log.Atom(p, [constants[random.randint(0,len(constants)-1)] for _ in range(p.arity)])
        while f in facts[p.name] or f in removed:
            p = ps[random.randint(0, len(ps) - 1)]
            f = log.Atom(p, [constants[random.randint(0, len(constants) - 1)] for _ in range(p.arity)])
        facts[p.name].append(f)
        add.append(f)
    return add


def write_list_file(list, path, name):
    with open(path + name + F_EXT, "w") as f:
        f.write("\n".join([str(o) for o in list]))

def write_rule_file(rules, path):
    rules1 = [ str(r) for r in rules]
    with open(path + "rules" + F_EXT, "w") as f:
        f.write("\n".join(rules1))


def write_fact_file(facts, path, name):
    facts1 = [ str(f) for f in list(itertools.chain(*facts.values()))] if isinstance(facts,dict) else [ str(f) for f in facts]
    random.shuffle(facts1)
    with open(path + name + F_EXT, "w") as f: #+ name + "facts" + ext , "w") as f:
        f.write(".\n".join(facts1) + '.')


def print_stats(size, category, dags, targets, train_support, train_conseqs, train, train_cwa, eval_support, eval_conseqs, eval_cwa, test, path):

    # np = sum([0 if not facts[p] else 1 for p in facts])
    # nc = len(set(c.name for p in facts for f in facts[p] for c in f.arguments))
    # ns = [(root,1)]
    # ds = []
    # while ns:
    #     for (n,d) in ns:
    #         ns.remove((n,d))
    #         if n.children:
    #             for c in n.children:
    #                 ns.append((c,d+1))
    #         else:
    #             ds.append(d)

    #with open(path + 'stats.txt', "w") as f:
    print('size,',size.name)
    print('category,',category.name)
    print('numdags,',str(len(dags)))
    print('targets,', ', '.join([str(p) for p in targets]))
    # print('numrules,'+str(len(rules)))
    # print('\n')
    facts = train_support
    print('trainsupport: size,',len(list(itertools.chain(*facts.values()))),', predicates,',sum([0 if not facts[p] else 1 for p in facts]),', constants,',\
          len(set(c.name for p in facts for f in facts[p] for c in f.arguments)))
    facts = train_conseqs
    print('trainconseqs: size,',len(facts),)#', predicates,',sum([0 if not facts[p] else 1 for p in facts]),', constants,',\
          # len(set(c.name for p in facts for f in facts[p] for c in f.arguments)))
    facts = train
    print('train: size,',len(list(itertools.chain(*facts.values()))),', predicates,',sum([0 if not facts[p] else 1 for p in facts]),', constants,',\
          len(set(c.name for p in facts for f in facts[p] for c in f.arguments)))
    facts = train_cwa
    print('train-cwa: size,',len(list(itertools.chain(*facts.values()))),', predicates,',sum([0 if not facts[p] else 1 for p in facts]),', constants,',\
          len(set(c.name for p in facts for f in facts[p] for c in f.arguments)))
    facts = eval_support
    print('evalsupport: size,',len(list(itertools.chain(*facts.values()))),', predicates,',sum([0 if not facts[p] else 1 for p in facts]),', constants,',\
          len(set(c.name for p in facts for f in facts[p] for c in f.arguments)))
    facts = eval_conseqs
    print('evalconseqs: size,',len(facts)) #', predicates,',sum([0 if not facts[p] else 1 for p in facts]),', constants,',\
          # len(set(c.name for p in facts for f in facts[p] for c in f.arguments)))
    facts = eval_cwa
    print('eval-cwa: size,',len(list(itertools.chain(*facts.values()))),', predicates,',sum([0 if not facts[p] else 1 for p in facts]),', constants,',\
          len(set(c.name for p in facts for f in facts[p] for c in f.arguments)))
    facts = test
    print('test: size,',len(facts))#list(itertools.chain(*facts.values()))),', predicates,',sum([0 if not facts[p] else 1 for p in facts]),', constants,',\
          # len(set(c.name for p in facts for f in facts[p] for c in f.arguments)))
    # facts = valid
    # print('valid: size,',len(facts))#list(itertools.chain(*facts.values()))),', predicates,',sum([0 if not facts[p] else 1 for p in facts]),', constants,',\
          # len(set(c.name for p in facts for f in facts[p] for c in f.arguments)))


# def product(*args, repeat=1):
#     # product('ABCD', 'xy') --> Ax Ay Bx By Cx Cy Dx Dy
#     # product(range(2), repeat=3) --> 000 001 010 011 100 101 110 111
#     pools = [tuple(pool) for pool in args] * repeat
#     p1 = [pool for pool in args]
#     result = [[]]
#     for a in args:
#         print(a)
#     for pool in pools:
#         result = [x+[y] for x in result for y in pool]
#     random.shuffle(result)
#     result = [x[0] for x in result]
#     # for prod in result:
#     #     #     yield tuple(prod)
#     return result

def product(args):
    result = itertools.product(*args)
    result = list(result)
    random.shuffle(result)
    # result = [x[0] for x in result]
    # for prod in result:
    #     #     yield tuple(prod)
    return result

if __name__ == '__main__':
    SIZES = [DatasetSize.XS, DatasetSize.S, DatasetSize.M, DatasetSize.L, DatasetSize.XL]

    parser = argparse.ArgumentParser(description='RuDaS Dataset Generator.')

    parser.add_argument('--name', default=None, type=str, help="...")
    parser.add_argument('--path', default="../datasets/", type=str, help="...")
    parser.add_argument('--size', default=1, type=int, choices=[0,1,2,3,4], help="...")
    parser.add_argument('--category', default=0, type=int, choices=[0,1,2,3,4,5,6], help="...")
    parser.add_argument('--overlap', type=int, default=0, choices=[0,1], help="...")
    parser.add_argument('--singletarget', type=int, default=0, choices=[0,1], help="...")
    parser.add_argument('--mindags', default=1, type=int, help="...")
    parser.add_argument('--maxdags', default=1, type=int, help="...")
    parser.add_argument('--maxdepth', default=3, type=int, help="...")
    parser.add_argument('--nodesupport', type=int, default=None, help="...")
    parser.add_argument('--dagsupport', default=3, type=int, help="...")
    parser.add_argument('--skipnode', default=5, type=int, help="...")
    parser.add_argument('--owa', default=.3, type=float, help="...")
    parser.add_argument('--noise', default=.2, type=float, help="...")
    parser.add_argument('--missing', default=.15, type=float, help="...")
    parser.add_argument('--targetsextra', default=1, type=int, choices=[0,1], help="...")
    parser.add_argument('--maxorchild', default=2, type=int, help="...")
    parser.add_argument('--maxatoms', default=2, type=int, help="...")
    parser.add_argument('--minarity', default=2, type=int, help="...")
    parser.add_argument('--maxarity', default=2, type=int, help="...")
    parser.add_argument('--numpreds', default=None, type=int, help="...")
    parser.add_argument('--numconstants', default=None, type=int, help="...")
    parser.add_argument('--test', default=.3, type=float, help="...")
    # TODO explanations


    args = parser.parse_args()
    generate_dataset(name=args.name,
                     path=args.path,
                     size=SIZES[args.size],
                     category=DatasetCategory(args.category),
                     overlap=bool(args.overlap),
                     singletarget=bool(args.singletarget),
                     mindags=args.mindags,
                     maxdags=args.maxdags,
                     maxdepth=args.maxdepth,
                     nodesupport=args.nodesupport,
                     dagsupport=args.dagsupport,
                     skipnode=args.skipnode,
                     owafactor=args.owa,
                     noisefactor=args.noise,
                     missfactor=args.missing,
                     targetsextra=bool(args.targetsextra),
                     maxorchild=args.maxorchild,
                     maxatoms=args.maxatoms,
                     minarity=args.minarity,
                     maxarity=args.maxarity,
                     numpreds=args.numpreds,
                     numconstants=args.numconstants,
                     test=args.test)

#
#     print("testing dataset generation")
# #
#     # cs = [DatasetCategory.CHAIN_RECURSIVE]#,DatasetCategory.ROOTED_DAG_RECURSIVE]#
#     #
#     cs = [#DatasetCategory.CHAIN, #.DISJUNCTIVE_ROOTED_DAG_RECURSIVE]#,
#     DatasetCategory.ROOTED_DAG_RECURSIVE]
#        # DatasetCategory.DISJUNCTIVE_ROOTED_DAG_RECURSIVE] #
#     ss = [DatasetSize.XS]#,DatasetCategory.CHAIN_RECURSIVE]#DatasetSize.XS,
#     for i in range(len(cs)):
#         for j in range(len(ss)):
#             # generate_dataset(size=ss[j], category=cs[i], nodesupport=0, maxdepth=2)
#             # generate_dataset(size=ss[j], category=cs[i], nodesupport=0)#, mindags=3, maxdags=3)
#             # generate_dataset(size=ss[j], category=cs[i], nodesupport=0, maxdepth=2, numpreds=3, path='../datasets/simple/')#DatasetSize.XS,
#             generate_dataset(size=ss[j], category=cs[i], nodesupport=0, maxdepth=3, #numpreds=3,
#                              path='../datasets/new/')  # DatasetSize.XS,

    # cs = [DatasetCategory.MIXED]
    # ss = [DatasetSize.M, DatasetSize.L]
    # for i in range(len(cs)):
    #     for j in range(len(ss)):
    #         generate_dataset(size=ss[j], category=cs[i], mindags=3, maxdags=3)

    # cs = [DatasetCategory.CHAIN_RECURSIVE]
    # ss = [DatasetSize.XS]
    # for i in range(len(cs)):
    #     for j in range(len(ss)):
    #         generate_dataset(size=ss[j], category=cs[i], nodesupport=0)