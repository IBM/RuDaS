import os
from generator import *

FILES = [
    "log", "rules",
    F_EVAL_CW, F_EVAL_SUPPORT, F_EVAL_CONSEQS,
    F_TRAIN_CW, F_TRAIN_SUPPORT, F_TRAIN_CONSEQS,
    F_TRAIN_CW_N,
    F_TRAIN_OW, F_TRAIN_OW_N,
    F_TEST,
    F_RELATIONS, F_ENTITIES
]
# FILES = [f+F_EXT for f in FILES]

# TODO add tests testing if required parameters are satisfied: size, factors

def test_files_present(path):
    for f in FILES:
        if not os.path.exists(path + f +F_EXT):
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
    es =  set()
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
    for l in lines:
        if "generated" in l and "facts" in l:
            i1 = l.index(" ") + 1
            i2 = i1 + l[i1:].index(" ")
            n = int(l[i1:i2])
            if F_TRAIN_CW not in f2l:
                f2l[F_TRAIN_CW] = n
            else:
                f2l[F_EVAL_SUPPORT] = n
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

    # print(n_added, n_removed)
    f2l[F_TRAIN_CW_N] = f2l[F_TRAIN_CW] + n_added - n_removed
    f2l[F_TRAIN_OW] = f2l[F_TRAIN_CW] - owa_removed
    f2l[F_TRAIN_OW_N] = f2l[F_TRAIN_OW] + n_added - n_removed
    f2l[F_TEST] = owa_removed
    # for f,l in f2l.items():
    #     print(f,l)
    return f2l


if __name__ == '__main__':
    print("testing generator")

    dpath = "../datasets/new/"
    for d in os.listdir(dpath):
        if d.startswith("."): continue

        # print("\n",d)
        path = dpath + d + "/"
        b = test_files_present(path)
        if not b:
            print(d)
        b = test_files_lengths(path)
        if not b:
            print("P",d)

