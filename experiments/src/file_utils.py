
import copy
import os
import src.logic as log

def replace_in_file(path, replacements):
    with open(path) as f:
        s = f.read()
    with open(path, 'w') as f:
        for r in replacements:
            s = s.replace(r[0], r[1])
        f.write(s)

# for fact file
# NOTE makes only sense for binary atoms
def prolog_to_tsv(path):
    ls = []
    with open(path, 'r') as f:
        ls = f.readlines()

    with open(path, 'w') as f:
        for l in ls:
            l = l.strip().replace(').','')
            i = l.index('(')
            if ',' in l:
                f.write(l[i+1:].replace(',','\t'+l[:i]+'\t')+'\n')
            else: # DUMMY! (for unary atoms)
                f.write(l[i + 1:]+'\t' + l[:i] + '\t' + l[i + 1:] + '\n')

def write_ntp_rule_template(rules, dstpath):
    terms = ['A','B','C','D','E','F'] #'G','H'
    with open(dstpath , "w") as f:
        for r in rules:  # TODO predicate printing, constants by same vars? - but make Y
            dict = {}
            i = 0
            r1 = copy.deepcopy(r)
            r1.head.predicate.name = "#"+r1.head.predicate.name.replace('p','')
            for t in r1.head.arguments:
                if t.name not in dict:
                    dict[t.name] = terms[i]
                    i += 1
                t.name = dict[t.name]

            for a in r1.body:
                a.predicate.name = "#"+a.predicate.name.replace('p','')
                for t in a.arguments:
                    if t.name not in dict:
                        dict[t.name] = terms[i]
                        i += 1
                    t.name = dict[t.name]
            f.write("20 " + str(r1) + "\n")#[0:len(str(r1)) - 1]


