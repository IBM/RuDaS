'''
 Copyright IBM Corp. 1992, 1993 All Rights Reserved
SPDX-License-Identifier: Apache-2.0
'''

import copy
import os
from logic import *
from exp_utils import *
from utils import *
from constants import *


def prolog_to_tsv(path):
    ls = []
    with open(path, 'r') as f:
        ls = f.readlines()
    with open(path, 'w') as f:
        for l in ls:
            l = l.strip().replace(').', '')
            i = l.index('(')
            if ',' in l:
                f.write(l[i+1:].replace(',', '\t'+l[:i]+'\t')+'\n')
            else:  # DUMMY! (for unary atoms)
                f.write(l[i + 1:]+'\t' + l[:i] + '\t' + l[i + 1:] + '\n')

def preprocess_FOIL(factsFile, validationFile, preprocessingFolder):
    _, filename = os.path.split(factsFile)
    filename = "train"
    [facts_dict, predicates, constants] = parseFacts_general(factsFile)
    if validationFile:
        [facts_dict_val, predicates_val, constants_val] = parseFacts_general(validationFile)
        facts_dict = union_dict(facts_dict, facts_dict_val)
        for pred in predicates_val:
            if pred not in predicates:
                predicates.append(pred)
        for const in constants_val:
            if const not in constants:
                constants.append(const)
    convert_input_FOIL(facts_dict, predicates, constants, filename, preprocessingFolder)



def convert_input_FOIL(facts, predicates, constants, filename, preprocessingFolder):
    file = open(preprocessingFolder+filename+"_FOIL.d", "w")
    # writing constants
    const_string = ""
    for i in range(len(constants)):
        if i>0:
            const_string += ", "
        const_string += constants[i]
        if i == len(constants)-1:
            const_string += "."
    file.write("E: "+const_string)
    file.write("\n\n")
    # writing facts
    for predicate in predicates:
        predicate_string = predicate+"("
        predicate_arity = facts[predicate][0].predicate.arity
        for i in range(predicate_arity):
            if i != 0:
                predicate_string += ","
            predicate_string += "E"
        predicate_string += ")"
        file.write(predicate_string+"\n")
        for fact in facts[predicate]:
            fact_string = ""
            for j in range(predicate_arity):
                if j != 0:
                    fact_string += ","
                fact_string += fact.arguments[j].name
            file.write(fact_string+"\n")
        file.write(".\n")
    file.close()



def replace_in_file(path, replacements):
    with open(path) as f:
        s = f.read()
    with open(path, 'w') as f:
        for r in replacements:
            s = s.replace(r[0], r[1])
        f.write(s)


# for fact file
# NOTE makes only sense for binary atoms


def write_ntp_rule_template(rules, dstpath):
    terms = ['A', 'B', 'C', 'D', 'E', 'F']  #'G','H'
    with open(dstpath , "w") as f:
        for r in rules:  # TODO predicate printing, constants by same vars? - but make Y
            dict = {}
            i = 0
            r1 = copy.deepcopy(r)
            r1.head.predicate.name = "#"+r1.head.predicate.name.replace('p', '')
            for t in r1.head.arguments:
                if t.name not in dict:
                    dict[t.name] = terms[i]
                    i += 1
                t.name = dict[t.name]
            for a in r1.body:
                a.predicate.name = "#"+a.predicate.name.replace('p', '')
                for t in a.arguments:
                    if t.name not in dict:
                        dict[t.name] = terms[i]
                        i += 1
                    t.name = dict[t.name]
            f.write("20 " + str(r1) + "\n")  # [0:len(str(r1)) - 1]


