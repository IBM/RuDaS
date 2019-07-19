'''
Python 3.
'''
import os
import sys
import subprocess
from src.logic import *

FOIL_PATH = "experiments/systems/FOIL/"
AMIEP_PARAMETER = 0.7
NEURAL_LP_PARAMETER = 0.6
NTP_PARAMETER = 0.0



def count_facts_dict(dict):
    num_facts = 0
    for key, value in dict.items():
        num_facts += len([item for item in value if item])
    return num_facts


def remove_aux_pred(facts, predicates_list):
    # origin = copy.deepcopy(facts)
    pred = [str(p) for p in predicates_list]
    key2pop = []
    for key in facts:
        if key not in pred:
            key2pop.append(key)
    for key in set(key2pop):
        facts.pop(key, None)
    return facts




def union_dict(dict1, dict2):
    keys = set(dict1).union(dict2)
    union = {}
    for key in keys:
        list_d = []
        list1 = []
        list2 = []
        if key in dict1:
            list1 = dict1[key]
        if key in dict2:
            list2 = dict2[key]
        for element in list1:
            list_d.append(element)
        for element in list2:
            if element not in list_d:
                list_d.append(element)
        union[key] = list_d
    return union


def delta_dict(dict1, dict2):
    keys = set(dict1).union(dict2)
    delta = {}
    for key in keys:
        list_d = []
        list1 = []
        list2 = []
        if key in dict1:
            list1 = dict1[key]
        if key in dict2:
            list2 = dict2[key]
        for element in list1:
            if element not in list2:
                list_d.append(element)
        for element in list2:
            if element not in list1:
                list_d.append(element)
        delta[key] = list_d
    return delta


def parseRules_general(rulesFile):
    # parse rules
    rules = []
    predicates = []
    constants = []
    if rulesFile is not None:
        with open(rulesFile, "r") as f:
            for line in f:
                if len(line) > 4:
                    [current_rule, current_predicates, current_constants] = parse_rule_general(line.split('.')[0])
                    rules.append(current_rule)
                    for current_predicate in current_predicates:
                        if current_predicate not in predicates:
                            predicates.append(current_predicate)
                    for current_constant in current_constants:
                        if current_constant not in constants:
                            constants.append(current_constant)
    return [rules, predicates, constants]


def parseFacts_general(factsFile):
    facts = []
    # parse facts
    predicates = []
    constants = []
    if factsFile is not None:
        with open(factsFile, "r") as f:
            for line in f:
                if len(line) > 3:
                    [current_fact, current_predicate, current_constants] = parse_atom_general(line.split('.')[0])
                    facts.append(current_fact)
                    if current_predicate not in predicates:
                        predicates.append(current_predicate)
                    for current_constant in current_constants:
                        if current_constant not in constants:
                            constants.append(current_constant)
    facts_dict = order_facts(facts)
    return [facts_dict, predicates, constants]


def parseFiles_general(factsFile, rulesFile):
    [facts_dict, predicates_f, constants_f] = parseFacts_general(factsFile)
    predicates = predicates_f
    constants = constants_f
    rules = None
    if rulesFile:
        [rules, predicates_r, constants_r] = parseRules_general(rulesFile)
        for predicate in predicates_r:
            if predicate not in predicates:
                predicates.append(predicate)
        for constant in constants_r:
            if constant not in constants:
                constants.append(constant)
    return [facts_dict, rules, predicates, constants]


def parse_atom_general(atom_string):
    atom_string = atom_string.replace(" ", "")
    atom_string = atom_string.replace(")", "")
    predicate_string = (atom_string.split('(')[0])  # predicate name
    arguments_string = (atom_string.split('(')[1]).split(',')  # arguments
    predicate = Predicate(predicate_string, len(arguments_string))
    arguments = []
    constants=[]
    for arg in arguments_string:
        if arg.isupper():  # if is capital letter is variable
            arguments.append(Variable(arg))
        else:  # is constant
            arguments.append(Constant(arg))
    for const in arguments:
        if type(const) == Constant and const.name not in constants:
            constants.append(const.name)
    atom = Atom(predicate, arguments)
    return [atom, predicate_string, constants]


def order_facts(facts):
    fact_dict = {}
    for fact in facts:
        if fact.predicate.name in fact_dict:
            if fact not in fact_dict[fact.predicate.name]:  # avoiding facts duplicates
                fact_dict[fact.predicate.name].append(fact)
        else:
            fact_dict[fact.predicate.name] = [fact]
    return fact_dict


def parse_rule_general(rule_string):
    predicates = []
    constants = []
    rule_string = rule_string.split(':-')
    head_string = rule_string[0]
    bodies_string = (rule_string[1]).split('),')
    [head, pred, constants_tmp] = parse_atom_general(head_string)
    predicates.append(pred)
    for const in constants_tmp:
        if const not in constants:
            constants.append(const)
    bodies = []
    for body in bodies_string:
        [body_atom, pred, constants_tmp] = parse_atom_general(body)
        for conts in constants_tmp:
            if conts not in constants:
                constants.append(conts)
        bodies.append(body_atom)
        if pred not in predicates:
            predicates.append(pred)
    rule = Rule(bodies, head)
    return [rule, predicates, constants]
