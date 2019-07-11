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




def parseFiles_comma_separated(factsFile):
    facts = []
    facts_dict = {}
    rules = []
    # parse facts
    predicates = []
    constants = []
    if factsFile != None:
        with open(factsFile, "r") as f:
            for line in f:
                if (len(line) > 2):
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



def parseRules_general(rulesFile):
    # parse rules
    rules = []
    predicates = []
    constants = []
    if rulesFile != None:
        with open(rulesFile, "r") as f:
            for line in f:
                if (len(line) > 4):
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
    facts_dict = {}
    # parse facts
    predicates = []
    constants = []
    if factsFile != None:
        with open(factsFile, "r") as f:
            for line in f:
                if (len(line) > 3):
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
    [facts_dict, predicates_f, constants_f]= parseFacts_general(factsFile)
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
    fact_dict={}
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
    rule_string=rule_string.split(':-')
    head_string = rule_string[0]
    bodies_string = (rule_string[1]).split('),')
    [head, pred, constants_tmp] = parse_atom_general(head_string)
    predicates.append(pred)
    for const in constants_tmp:
        if const not in constants:
            constants.append(const)
    bodies = []
    for body in bodies_string:
        [body_atom, pred, constants_tmp]=parse_atom_general(body)
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

def post_process_FOIL(factsFile, validationFile, preprocessingFolder):
    pass


def convert(format, facts_file, rules_file=None):
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
    #filename_w_ext = os.path.basename(facts_file)
    #filename, file_extension = os.path.splitext(filename_w_ext)
    path, filename = os.path.split(facts_file)
    [facts, rules, predicates, constants] = parseFiles_general(facts_file, rules_file)
    if format=='FOIL':
        convert_input_FOIL(facts, predicates, constants, filename)
    elif format=='ProGol':
        convert_input_ProGol(facts, rules, predicates, constants, path, filename)
    else:
        print("Unknown format: choose between FOIL and ProGol")



def convert_input_FOIL(facts, predicates, constants, filename, preprocessingFolder):

    file = open(preprocessingFolder+filename+"_FOIL.d", "w")
    #writing constants
    const_string = ""
    for i in range(len(constants)):
        if i>0:
            const_string+=", "
        const_string+=constants[i]
        if i == len(constants)-1:
            const_string+="."
    file.write("E: "+const_string)
    file.write("\n\n")
    #writing facts
    for predicate in predicates:
        predicate_string=predicate+"("
        predicate_arity=facts[predicate][0].predicate.arity
        for i in range(predicate_arity):
            if i != 0:
                predicate_string += ","
            predicate_string += "E"
        predicate_string += ")"
        file.write(predicate_string+"\n")
        for fact in facts[predicate]:
            fact_string=""
            for j in range(predicate_arity):
                if j != 0:
                    fact_string += ","
                fact_string += fact.arguments[j].name
            file.write(fact_string+"\n")
        file.write(".\n")
    file.close()

def convert_input_ProGol(facts, rules, predicates, constants, path, filename):
    #TODO
    pass

def convert_out_FOIL(results_file):
    rules = []
    path, filename = os.path.split(results_file)

    output_file = path + "/FOIL_out.txt"
    with open(results_file, 'r') as file:
        lines = file.readlines()
    file.close()
    for line in lines:
        if ":-" in line:
            rules.append(line)
    output_file = path+"/results4eval.txt"
    file = open(output_file, "w")
    for rule in rules:
        file.write(rule+"\n")
    file.close()

def convert_out_AMIE_plus(results_file,parameter=AMIEP_PARAMETER):
    rules=[]
    path, filename = os.path.split(results_file)
    output_file = path + "/" +"results4eval.txt"
    with open(results_file, 'r') as file:
        file_lines = file.readlines()
    file.close()
    # TODO
    for line in file_lines:
        if "=>" in line:
            rules.append(line)
    file_lines = copy.deepcopy(rules)
    rules=[]
    confidence=[]
    for line in file_lines:
        line=line.split('\t')
        if float(line[3]) >= parameter:
            rules.append(line[0])
            confidence.append(float(line[3]))
    final_rules=[]
    for rule in rules:
        rule = rule.split('   => ')
        head = rule[1]
        body = rule[0]
        rule_string=""
        head=head.split('  ')
        body=body.split('  ')
        rule_string+=head[1]
        rule_string+="("
        rule_string+=((head[0].replace(" ","")).replace("?","")).capitalize()
        rule_string+=","
        rule_string+=(head[2].replace("?","")).capitalize()
        rule_string+=") :- "
        num_bodies=int(len(body)/3)
        for i in range(num_bodies):
            rule_string += body[3*i+1]
            rule_string += "("
            rule_string += ((body[3*i].replace(" ", "")).replace("?", "")).capitalize()
            rule_string += ","
            rule_string += (body[3*i+2].replace("?", "")).capitalize()
            rule_string+=")"
            if i<num_bodies-1:
                rule_string += ","
        rule_string += "."
        final_rules.append(rule_string)
    file = open(output_file, "w")
    for rule in final_rules:
        file.write(rule + "\n")
    file.close()


def convert_out_neural_lp(results_file, parameter=NEURAL_LP_PARAMETER):
    path, filename = os.path.split(results_file)
    output_file = path + "/" + "results4eval.txt"
    print(path)
    with open(results_file, 'r') as file:
        file_lines = file.readlines()
    file.close()
    rules = []
    confidence = []
    for line in file_lines:
        line = line.replace('\n','')
        line = line.split('\t')
        '''
        the first number is the weight of the rule
        the second number is the weight of the rule normalized
        we use the normalized weight
        to use the non-normalized weight replace the following line with
        confidence_value = float(line[0].split(' ')[0])
        '''
        confidence_value = float(line[0].split(' ')[1].replace(")","").replace("(",""))
        rules.append(line[1])
        confidence.append(confidence_value)
    final_rules = []
    final_confidence=[]
    prev_head_pred = []
    index = -1
    for i in range(len(rules)):
        rule = rules[i]
        rule = rule.split(' <-- ')
        head = rule[0]
        head_pred = head.split('(')[0]
        body = rule[1]
        rule_string = ""
        rule_string += head
        rule_string += " :- "
        rule_string += body
        rule_string += "."
        if head_pred != prev_head_pred:
            index += 1
            if "equal" in body:
                final_rules.append([])
                final_confidence.append([])
            else:
                final_rules.append([rule_string])
                final_confidence.append([confidence[i]])
        else:
            final_rules[index].append(rule_string)
            final_confidence[index].append(confidence[i])
        prev_head_pred = head_pred
    #ri-normalize
    asd=1
    for index in range(len(final_rules)):
        if final_confidence[index][0] < 1.0:
            norm_factor = final_confidence[index][0]
            for i in range(len(final_confidence[index])):
                final_confidence[index][i]=final_confidence[index][i] / norm_factor
    rules_to_print = []
    for index in range(len(final_rules)):
        for i in range(len(final_rules[index])):
            if final_confidence[index][i]>=parameter:
                rules_to_print.append(final_rules[index][i])
    file = open(output_file, "w")
    for rule in rules_to_print:
        file.write(rule + "\n")
    file.close()

def convert_out_ntp(log_file, parameter=NTP_PARAMETER):
    path, filename = os.path.split(log_file)
    output_file = path + "/" + "results4eval.txt"
    with open(log_file, 'r') as file:
        log_lines = file.readlines()
    file.close()
    results_file = None
    for line in log_lines:
        if "Writing induced logic program to" in line:
            results_file=line.split(' ')[-1].replace("\n","")
            results_file=".."+results_file.split("experiments")[1]
    rules=[]
    if results_file:
        with open(results_file, 'r') as file:
            file_lines = file.readlines()
        file.close()
        confidence = []
        for line in file_lines:
            if len(line)>1 and line[1]==".":
                line = line.replace('\n', '')
                line = line.split('\t')
                confidence_value = float(line[0])
                if confidence_value >= parameter:
                    rules.append(line[1])
                    confidence.append(confidence_value)
    file = open(output_file, "w")
    for rule in rules:
        file.write(rule + "\n")
    file.close()
'''
# REFORMATTED
def run_FOIL_test(facts_file):
    path, filename = os.path.split(facts_file)
    #convert_comma_separated("FOIL",facts_file)
    #convert("FOIL",facts_file)
    rules=[]
    foil_path="experiments/systems/FOIL/"
    command = foil_path+"./foil6 -v0 -n <"+foil_path+"/FOILdata/"+filename+"_FOIL.d"
    output = subprocess.check_output(command, shell=True).decode("utf-8")
    output=output.split("\n")
    for line in output:
        if ":-" in line:
            rules.append(line)
    file = open(path + "/" + filename + "_FOIL_result", "w")
    for rule in rules:
        file.write(rule+"\n")
    file.close()


def run_FOIL(facts_file, validationFile, preprocessingFolder, output_file):
    preprocess_FOIL(facts_file, validationFile, preprocessingFolder)

    path, filename = os.path.split(facts_file)
    filename= "train"
    rules = []
    foil_path=FOIL_PATH
    foil_path = "../systems/FOIL"
    # ADD "-m 200000" if the max tuples are exceeded
    command = foil_path+"/./foil6 -v0 -n <"+preprocessingFolder+filename+"_FOIL.d"
    output = subprocess.check_output(command, shell=True).decode("utf-8")
    output_lines = output.split("\n")
    path_out, _ = os.path.split(output_file)
    file = open(path_out+"/FOIL_out.txt", "w")
    for line in output_lines:
        file.write(line + "\n")
    file.close()
    for line in output_lines:
        if ":-" in line:
            rules.append(line)
    file = open(output_file, "w")
    for rule in rules:
        file.write(rule+"\n")
    file.close()

'''

def run_ProGol(facts_file, rules_file=None):
    #TODO
    pass

if __name__ == '__main__':

    #print("Testing Converter")
    #factsFile = "../../datasets/test/even-facts-ext"
    #validationFile = "../../datasets/test/even-facts-ext-validation"
    #output_file = "../../datasets/test/even-facts-ext_FOIL_RESULT"
    #preprocessingFolder = "../systems/FOIL/FOILdata/"
    ##run_FOIL_test(factsFile)
    #run_FOIL(factsFile, validationFile, preprocessingFolder, output_file)
    
    #print("Testing AMIEP Converter")
    #convert_out_AMIE_plus("../output/amiep/CHAIN-S-2/results.txt")

    #print("Testing Neural-LP Converter")
    #convert_out_neural_lp("../output/existing/Neural-LP/EVEN/rules.txt")

    #print("Testing NTP Converter")
    #convert_out_ntp("../output/simple/ntp/CHAIN-S-2-2/log.txt")
    pass