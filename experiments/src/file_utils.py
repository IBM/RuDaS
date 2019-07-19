
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
            l = l.strip().replace(').', '')
            i = l.index('(')
            if ',' in l:
                f.write(l[i+1:].replace(',', '\t'+l[:i]+'\t')+'\n')
            else:  # DUMMY! (for unary atoms)
                f.write(l[i + 1:]+'\t' + l[:i] + '\t' + l[i + 1:] + '\n')


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


def parseFiles_comma_separated(factsFile):
    facts = []
    rules = []
    # parse facts
    predicates = []
    constants = []
    if factsFile is not None:
        with open(factsFile, "r") as f:
            for line in f:
                if len(line) > 2:
                    [current_fact, current_predicate, current_constants] = parse_atom_comma_separated(line)
                    facts.append(current_fact)
                    if current_predicate not in predicates:
                        predicates.append(current_predicate)
                    for current_constant in current_constants:
                        if current_constant not in constants:
                            constants.append(current_constant)
    facts_dict = order_facts(facts)
    return [facts_dict, rules, predicates, constants]


def parse_atom_comma_separated(atom_string):
    atom_string=atom_string.replace('\n', '')
    atom_string=atom_string.replace(' ', '')
    atom_string_tokens = atom_string.split(',')
    predicate_string = atom_string_tokens[0]  # predicate name
    arguments_string = atom_string_tokens[1:]  # arguments
    predicate = Predicate(predicate_string, len(arguments_string))
    arguments = []
    constants = []
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


def convert_comma_separated(format, facts_file, rules_file=None):
    '''
        Parameters
        ----------
        factsFile   :   string
            name of the file that stores the original facts
        rulesFile   :   string
            name of the file that stores the original rules
        format      :   string
            'FOIL', 'ProGol'
        Description
        ----------
        convert comma_separated file into FOIL or ProGol format
        '''
    # filename_w_ext = os.path.basename(facts_file)
    # filename, file_extension = os.path.splitext(filename_w_ext)
    path, filename = os.path.split(facts_file)
    [facts, rules, predicates, constants] = parseFiles_comma_separated(facts_file)
    if format == 'FOIL':
        convert_input_FOIL(facts, predicates, constants, filename)
    elif format == 'ProGol':
        convert_input_ProGol(facts, rules, predicates, constants, path, filename)
    else:
        print("Unknown format: choose between FOIL and ProGol")
