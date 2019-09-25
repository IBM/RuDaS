'''
 Copyright IBM Corp. 1992, 1993 All Rights Reserved
SPDX-License-Identifier: Apache-2.0
'''

import os
from generator import *
from utils import *
from itertools import chain

FILES = [
    "log", "rules",
    F_EVAL_CW, F_EVAL_SUPPORT, F_EVAL_CONSEQS,
    F_TRAIN_CW, F_TRAIN_SUPPORT, F_TRAIN_CONSEQS,
    F_TRAIN_CW_N,
    F_TRAIN_OW, F_TRAIN_OW_N,
    F_TEST,
    F_RELATIONS, F_ENTITIES
]


def test_files_present(path):
    for f in FILES:
        if not os.path.exists(path + f +F_EXT):
            print(f, "*" * 50)
            return False
    return True

def test_files_lengths(path):
    f2l_test = extract_file_lengths(path)
    f2l = extract_log_lengths(path)

    for f, l in f2l.items():
        if l != f2l_test[f]:
            print(f, f2l_test[f], l,"*"*50)
            return False

    rels = set()
    es = set()
    for f in [F_TRAIN_CW, F_TRAIN_CW_N]:
        with open(path + f + F_EXT) as file:
            for l in file.readlines():
                rels.add(l[:l.index("(")])
                es.add(l[l.index("(")+1:l.index(",")])
                es.add(l[l.index(",")+1:l.index(")")])

    rest = [
        (f2l_test[F_EVAL_CW], f2l_test[F_EVAL_CONSEQS]+f2l_test[F_EVAL_SUPPORT]),
        (f2l_test[F_TRAIN_CW], f2l_test[F_TRAIN_CONSEQS]+f2l_test[F_TRAIN_SUPPORT]),
        (f2l_test[F_RELATIONS], len(rels)),
        (f2l_test[F_ENTITIES], len(es))]
    for t in rest:
        if t[0] != t[1]:
            print(t, "*" * 50)
            return False

    return True

def test_parameters(path):
    pass

def extract_file_lengths(path):
    f2l = {}
    for f in FILES:
        lines = []
        with open(path + f+F_EXT, "r") as file:
            lines = file.readlines()
        f2l[f] = len(lines)
    return f2l

def extract_log_lengths(path):
    lines = []
    with open(path + "log" + F_EXT, "r") as file:
        lines = file.readlines()

    f2l = {}
    owa_removed = 0
    n_removed = -1
    n_added = 0
    # need to update these two iteratively since the log may contain several failed generation starts (until both(!) fact sets are generated)
    g1 = -1
    g2 = -1
    for l in lines:
        if "generated" in l and "facts" in l:
            i1 = l.index(" ") + 1
            i2 = i1 + l[i1:].index(" ")
            n = int(l[i1:i2])

            if g1 == -1:
                g1 = n
            elif g2 == -1:
                g2 = n
            else:
                g1=g2
                g2=n

        elif "owa: removed " in l:
            i = l[5:].index(" ") + 6
            owa_removed += int(l[i:i+l[i:].index(" ")])
        elif "noise:" in l:
            i = l.index(" ") + 1
            n = int(l[i:i+l[i:].index(" ")])
            if n_removed < 0:
                n_removed = n
            else:
                n_added += n
        elif "noise (target):" in l:
            i = l.index(": ") + 2
            n_added += int(l[i:i+l[i:].index(" ")])

    f2l[F_TRAIN_CW] = g1
    f2l[F_EVAL_SUPPORT] = g2

    # print(n_added, n_removed)
    f2l[F_TRAIN_CW_N] = f2l[F_TRAIN_CW] + n_added - n_removed
    f2l[F_TRAIN_OW] = f2l[F_TRAIN_CW] - owa_removed
    f2l[F_TRAIN_OW_N] = f2l[F_TRAIN_OW] + n_added - n_removed
    f2l[F_TEST] = owa_removed
    # for f,l in f2l.items():
    #     print(f,l)
    return f2l

def extract_log_params(path):
    lines = []
    with open(path + "log" + F_EXT, "r") as file:
        lines = file.readlines()

    p2n = {}
    for l in lines:
        l = l.strip()

        if "size" in l:
            # TODO
            pass#p2n["size"] = ...
        if "owafactor" in l or "noisefactor"  in l or "missfactor" in l or "test" in l:
            p2n[l:l.index(":")] = float(l[l.index(" ")+1:])

# assuming predicate fits
def check_assignment(fact, atom, assignment):
    assignment1 = copy.deepcopy(assignment)
    for i, t in enumerate(atom.arguments):
        if t.name in assignment1:
            if fact.arguments[i] != assignment1[t.name]:
                # print(fact.arguments[i],assignment1[t.name])
                return {}
        elif isinstance(t, Constant):
            # if isinstance(fact.arguments[i], Constant) and
            if fact.arguments[i] == t:
                assignment1[t.name] = fact.arguments[i]
            else:
                return {}
        else:
            assignment1[t.name] = fact.arguments[i]
    return assignment1


def get_assignments(fact_dict, atom, assignments): # = [([],{})]):

    # if not assignments:
    result = []
    for f in fact_dict[atom.predicate.name]:

        for fs, ass in assignments:
            # r = []
            # for ass0 in ass:
            ass1 = check_assignment(f, atom, ass)
            if ass1:
                result.append((fs+[f], ass1))
    return result


def extend_by_head(atom, fs, ass):
    fs.append(Atom(atom.predicate,[ass[t.name] if t.name in ass else t for t in atom.arguments])) #else case is const
    return fs


# looks for rules instantiations and graph instantiations in data
# but thus ignores missfactor (leading to partial insts of ..., so counts the partial instances as noise)
# thus
# underestimates insts
# overestimates noisefactor
def test_graph_insts(path):

    rf = path + "rules.txt"
    tf = path + "train.txt"
    fs, rs, _, _ = parseFiles_general(tf, rf)

    rs.reverse()

    rd = {}
    for r in rs:
        if r.head.predicate.name in rd:
            rd[r.head.predicate.name].append(r)
        else:
            rd[r.head.predicate.name] = [r]

    count_r_insts = [0] * len(rs)
    count_rb_insts = [0] * len(rs)
    rinst_assignments = []
    for i, r in enumerate(rs):
        # print()
        # print(r)
        bs = r.body

        ass = [([],{})]
        for i1, a in enumerate(bs):
            ass1 = get_assignments(fs, a, ass)

            ass = ass1
            # if i1 == len(bs)-1:
            #     print(ass1)
            #     print(len(ass1))
        count_rb_insts[i] = len(ass1)
        ass1 = get_assignments(fs, r.head, ass)
        # print(ass1)
        # print(len(ass1))
        count_r_insts[i] = len(ass1)
        # if count_r_insts[i] == 0 and len(rd[r.head.predicate.name]) == 1:
        #     print("narder since rule has no full insts")
        rinst_assignments.extend(ass1)

    print("num rule body insts:",count_rb_insts)
    print("num rule insts:",count_r_insts)
    print("overestimated owa:",sum(count_r_insts)/sum(count_rb_insts)) # overestimated since not all partial rule insts must be real insts




    assignments = []
    for pn, rl in rd.items():
        tmpassignments = []

        for r in rl:
            # print("*"*20)
            # print(r)
            ass1 = [([], {})] if not assignments else copy.deepcopy(assignments)
            for i1, a in enumerate(r.body):
                ass1 = get_assignments(fs, a, ass1) # could be critical in later iterations if new atoms have vars from old but diff ass bec replaced in ass. so better would be to renam a;; nes...

                # if i1 == len(r.body) - 1:
                    # print(ass1)
                    # print(len(ass1))

            for fs1,ass in ass1:
                extend_by_head(r.head, fs1, ass)
            # print(ass1)
            tmpassignments.extend(ass1)
        assignments.extend(tmpassignments)
    # print(len(assignments))

    rest = chain.from_iterable(fs1 for fs1 in fs.values())
    # print("1",len([f for f in rest]))

    all=len([f for f in rest])
    for fs1, ass1 in assignments:
        for f in fs1:
            if f in fs[f.predicate.name]:
                fs[f.predicate.name].remove(f) # derived heads might not be in there

    for fs1, ass1 in rinst_assignments:
        for f in fs1:
            if f in fs[f.predicate.name]:
                fs[f.predicate.name].remove(f) # derived heads might not be in there
    noise = chain.from_iterable(fs1 for fs1 in fs.values())
    l = len([f for f in noise])
    #print(l,"noise, of",all,"ie (overestimated)",l/all)
    print("overestimated noise", l / all,"(",l,"/",all,")")



def print_statistics(paths):
    TYPE = "category:"
    SIZE = "size:"
    DEPTH = "maxdepth:"
    keys = [
        # "size", #DatasetSize.S
        # "category", #DatasetCategory.DISJUNCTIVE_ROOTED_DAG_RECURSIVE
        # mindags", 1
        # maxdags", 1
        # "maxdepth", #2
        # dagsupport", 3
        # skipnode", 5
        # "owafactor:", #0.2
        # "noisefactor:",# 0.2
        # "missfactor:", #0.15
        "r",
        "f",
        "p",
        "c",

       # train: size, 633, predicates, 11, constants, 416""
        # targetsextra", True
        # maxorchild", 2
        # maxatoms", 2
        # minarity", 2
        # maxarity", 2
        # test", 0.2
        # eval", True
        # sizefactor1", 1.1111111111111112
        # sizefactor2", 1.1333333333333333
        # fixedsize", 612
        # generatablesize", 1224
        # mingeneratablesize", 127
        #"numpredicates", #11
       # "numconstants", #612
    ]

    cats = [DatasetCategory.CHAIN_RECURSIVE,DatasetCategory.ROOTED_DAG_RECURSIVE,DatasetCategory.DISJUNCTIVE_ROOTED_DAG_RECURSIVE]
    sizes = [DatasetSize.XS,DatasetSize.S]
    depths = ["2","3"]
    dic = {}
    for cat in cats:
        dic2 = {}
        dic[cat.name] = dic2
        for s in sizes:
            dic3 = {}
            dic2[s.name] = dic3
            for d in depths:
                dic4 = {}
                dic3[d] = dic4
                for k in keys:
                    dic4[k] = []

    stats = {}
    for path in paths:
        for d in os.listdir(path):
            if d.startswith("."): continue

            lines = []
            with open(path + d + "/rules.txt", "r") as file:
                lines = file.readlines()
            r = len(lines)

            with open(path + d + "/log.txt", "r") as file:
                lines = file.readlines()

            for l in lines:
                l = l.strip()

                if TYPE in l:
                    t = l.split(":")[1].split(".")[1].strip()
                if SIZE in l and l[0] == "s":
                    s = l.split(":")[1].split(".")[1].strip()
                if DEPTH in l:
                    d = l.split(":")[1].strip()


                if "train:" in l:
                    tl = l.split(",")
                    tl = [i.strip() for i in tl]

                    tmp = dic[t][s][d]
                    tmp["r"].append(int(r))
                    tmp["f"].append(int(tl[1]))
                    tmp["p"].append(int(tl[3]))
                    tmp["c"].append(int(tl[5]))
    for k1 in dic:
        for k2 in dic[k1]:
            for k3 in dic[k1][k2]:

                s = k1 + " "+ k2+ " " + k3 + " "
                for k4 in dic[k1][k2][k3]:
                    s += " & "
                    d = dic[k1][k2][k3][k4]
                    s += str(min(d))+"/"+str(int(round(np.average(d))))+"/"+str(max(d))
                    # print(k1,k2,k3,k4, str(min(d))+"/"+str(int(round(np.average(d))))+"/"+str(max(d))) # ,d)

                print(s)




#
#
#
#     for f, l extract_log_lengths()

if __name__ == '__main__':
    print("testing generator")

    dpath = "../datasets/exp1/"
    for d in os.listdir(dpath):
        if d.startswith("."): continue
    #

        if not "XS" in d:
            continue
        print(#"\n",
             d)
        path = dpath + d + "/"
        test_graph_insts(path)
        # break
    #
    #     b = test_files_present(path)
    #     if not b:
    #         print(d)
    #     b = test_files_lengths(path)
    #     if not b:
    #         print("P",d)
    # print_statistics(["../datasets/exp1/","../datasets/exp2/"])
    # test_graph_insts(["../datasets/exp1/","../datasets/exp2/"])
