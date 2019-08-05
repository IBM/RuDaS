import os
import shutil
from evaluator import *
from constants import *
import copy
from logic import *
from utils import *
from exp_utils import *
from constants import *

AMIEP_PARAMETER_list = [0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9]
NEURAL_LP_PARAMETER_list = [0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9]
NTP_PARAMETER_list = [0.0, 0.02, 0.5, 0.1, 0.15, 0.2, 0.25, 0.3, 0.35, 0.4]

def remove_aux_pred_NLP(facts):
    origin = copy.deepcopy(facts)
    key2pop = []
    for key in facts:
        if "inv" in key:
            key2pop.append(key)
    for key in set(key2pop):
        origin.pop(key, None)
    return origin




def convert(format, facts_file, rules_file=None):
    '''
    Parameters
    ----------
    facts_file   :   string
        name of the file that stores the original facts
    rules_file   :   string
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
    if format == 'FOIL':
        convert_input_FOIL(facts, predicates, constants, filename)
    else:
        print("Unknown format: choose between FOIL and ProGol")


def convert_out_FOIL(results_file):
    rules = []
    path, filename = os.path.split(results_file)
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


def convert_out_AMIE_plus(results_file, parameter=AMIEP_PARAMETER):
    rules = []
    path, filename = os.path.split(results_file)
    output_file = path + "/" + "results4eval.txt"
    with open(results_file, 'r') as file:
        file_lines = file.readlines()
    file.close()
    for line in file_lines:
        if "=>" in line:
            rules.append(line)
    file_lines = copy.deepcopy(rules)
    rules = []
    confidence = []
    for line in file_lines:
        line = line.split('\t')
        if float(line[3]) >= parameter:
            rules.append(line[0])
            confidence.append(float(line[3]))
    final_rules = []
    for rule in rules:
        rule = rule.split('   => ')
        head = rule[1]
        body = rule[0]
        rule_string = ""
        head = head.split('  ')
        body = body.split('  ')
        rule_string += head[1]
        rule_string += "("
        rule_string += ((head[0].replace(" ","")).replace("?","")).capitalize()
        rule_string += ","
        rule_string += (head[2].replace("?","")).capitalize()
        rule_string += ") :- "
        num_bodies = int(len(body)/3)
        for i in range(num_bodies):
            rule_string += body[3*i+1]
            rule_string += "("
            rule_string += ((body[3*i].replace(" ", "")).replace("?", "")).capitalize()
            rule_string += ","
            rule_string += (body[3*i+2].replace("?", "")).capitalize()
            rule_string += ")"
            if i < num_bodies-1:
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
        line = line.replace('\n', '')
        line = line.split('\t')
        '''
        the first number is the weight of the rule
        the second number is the weight of the rule normalized
        we use the normalized weight
        to use the non-normalized weight replace the following line with
        confidence_value = float(line[0].split(' ')[0])
        '''
        confidence_value = float(line[0].split(' ')[1].replace(")", "").replace("(", ""))
        rules.append(line[1])
        confidence.append(confidence_value)
    final_rules = []
    final_confidence = []
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
    # ri-normalize
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
    for r in rules_to_print:
        file.write(r + "\n")
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
            results_file = line.split(' ')[-1].replace("\n", "")
            results_file = results_file.split("out/")[1]
            if 'exp1' in path:
                results_file = '../output/exp1/ntp/' + results_file
            elif 'exp2' in path:
                results_file = '../output/exp2/ntp/' + results_file
    rules = []
    if results_file:
        with open(results_file, 'r') as file:
            file_lines = file.readlines()
        file.close()
        confidence = []
        for line in file_lines:
            if len(line) > 1 and line[1] == ".":
                line = line.replace('\n', '')
                line = line.split('\t')
                confidence_value = float(line[0])
                if confidence_value >= parameter:
                    rules.append(line[1])
                    confidence.append(confidence_value)
    file = open(output_file, "w")
    for r in rules:
        file.write(r + "\n")
    file.close()


def find_parameters(sys):
    '''
    optimal parameters are:
    AMIEP_PARAMETER = 0.5
    NEURAL_LP_PARAMETER = 0.5
    NTP_PARAMETER = 0.2
    '''
    parameter_list = None
    if sys == "AMIE_PLUS":
        parameter_list = AMIEP_PARAMETER_list
    elif sys == "NEURAL_LP":
        parameter_list = NEURAL_LP_PARAMETER_list
    elif sys == "NTP":
        parameter_list = NTP_PARAMETER_list
    else:
        print("invalid system")
        pass
    for value in parameter_list:
        for ds in DATASETS:
            if sys == "AMIE_PLUS":
                output_directory = OUTPUT_DIR + AMIE_PLUS + '/' + ds + "/"
                convert_out_AMIE_plus(output_directory + "results.txt", value)
            elif sys == "NEURAL_LP":
                output_directory = OUTPUT_DIR + NEURAL_LP + '/' + ds + "/"
                convert_out_neural_lp(output_directory + "rules.txt", value)
            else: #sys == "NTP"
                output_directory = OUTPUT_DIR + NTP + '/' + ds + "/"
                convert_out_ntp(output_directory + "log.txt", value)
            print("starting", sys, ds)
            outputFile = RESULTS_BASE_DIR + sys + '_' + ds + '_' + str(value) + ".txt"
            original_factsFile = DATASETS_DIR + ds + '/' + EVAL_SUPPORT_FILE
            original_rulesFile = DATASETS_DIR + ds + '/' + RULE_FILE
            original_testFile = DATASETS_DIR + ds + '/' + EVAL_CONSEQS_FILE
            toEvaluate_factsFile = original_factsFile
            toEvaluate_rulesFile = OUTPUT_DIR + sys + '/' + ds + "/results4eval.txt"
            evaluator = Evaluator(original_factsFile, original_rulesFile, original_testFile, toEvaluate_factsFile, toEvaluate_rulesFile)
            evaluator.compute_Herbrand()
            evaluator.save_results_on_file(outputFile)
            print("done", sys, ds)
        print("done", ds)


def tune_parameters(sys):
    '''grid search for tuning parameters specific for the evaluated systems'''
    # sys must be in [AMIE_PLUS, NEURAL_LP, NTP]
    parameter_list = None

    if sys == "AMIE_PLUS":
        parameter_list = AMIEP_PARAMETER_list
    elif sys == "NEURAL_LP":
        parameter_list = NEURAL_LP_PARAMETER_list
    elif sys == "NTP":
        parameter_list = NTP_PARAMETER_list
    else:
        print("invalid system")
        pass
    avarege_values = [0.0]*len(parameter_list)
    for index in range(len(parameter_list)):
        for ds in DATASETS:
            val = parameter_list[index]
            file_name = RESULTS_BASE_DIR + sys +'/'+sys + '_' + ds + '_' + str(val) + ".txt"
            with open(file_name, "r") as f:
                for line in f:
                    if 'Herbrant accuracy (new normalization):' in line:
                        val = float(line.split(':')[1])
                        avarege_values[index] += val
                        break
        avarege_values[index] = avarege_values[index]/len(DATASETS)
    max_accuracy = max(avarege_values)
    max_val_index = avarege_values.index(max_accuracy)
    max_parameter = parameter_list[max_val_index] # in case of ties, we use the lowest index
    print("optimal value for ", sys, "=", max_parameter)



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
    else:
        print("Unknown format: choose between FOIL and ProGol")



def evaluate_systems():
    for ds in DATASETS:
        dsData = ds.split("/")[0]
        for sys in SYSTEMS:
            try:
                print("starting evaluation", sys, dsData)
                outputFile = OUTPUT_DIR + sys + '/' + ds + "/evaluation.txt"
                logFile = OUTPUT_DIR + sys + '/' + ds + "/evaluation_log"
                original_factsFile = DATASETS_DIR + dsData + '/' + EVAL_SUPPORT_FILE
                original_rulesFile = DATASETS_DIR + dsData + '/' + RULE_FILE
                original_testFile = DATASETS_DIR + dsData + '/' + EVAL_CONSEQS_FILE
                toEvaluate_factsFile = original_factsFile
                toEvaluate_rulesFile = OUTPUT_DIR + sys + '/' + ds + "/results4eval.txt"
                evaluator = Evaluator(original_factsFile, original_rulesFile, original_testFile, toEvaluate_factsFile, toEvaluate_rulesFile)
                evaluator.compute_Herbrand()
                evaluator.compute_accuracy()
                evaluator.compute_precision()
                evaluator.compute_recall()
                evaluator.compute_f1score()
                evaluator.save_results_on_file(outputFile)
                #evaluator.save_log_on_file(logFile)
                print("done", sys, ds)
            except:
                print(sys+'not evaluated on'+ds)
        print("----------done", ds)


def grid_search():
    for sys in [AMIE_PLUS, NEURAL_LP, NTP]:
        find_parameters(sys)
        tune_parameters(sys)


def experiments_evaluation_old():
    tests = [SIMPLE, SIMPLE_OWA, SIMPLE_CWA, EXISTING, BINARY, GENERAL, NEW]
    for test in tests:
        DATASETS_DIR = DATASETS_BASE_DIR + test
        DATA_DIR = DATA_BASE_DIR + test
        OUTPUT_DIR = OUTPUT_BASE_DIR + test
        DATASETS = [str(f) for f in os.listdir(DATASETS_DIR) if
                    not str(f).startswith('datasets') and not str(f).startswith('.') and not str(f).startswith(
                        'test') and not str(f).startswith('README')]
        convert_systems_output()
        evaluate_systems()

def convert_systems_output():
    print("converting systems outputs...")
    for ds in DATASETS:
        #AMIE PLUS
        convert_output_amiep(ds)
        #NEURAL LP
        convert_output_neural_lp(ds)
        #NTP
        try:
            convert_output_ntp(ds)
        except:
            print('ntp not evaluated on ', ds)
        #FOIL
        convert_output_FOIL(ds)
    print("done converting systems outputs.")



def convert_output_amiep(ds):
    output_directory = OUTPUT_DIR + AMIE_PLUS + '/' + ds + "/"
    convert_out_AMIE_plus(output_directory + "results.txt")


def convert_output_neural_lp(ds):
    output_directory = OUTPUT_DIR + NEURAL_LP + '/' + ds + "/"
    convert_out_neural_lp(output_directory + "rules.txt")


def convert_output_ntp(ds):
    output_directory = OUTPUT_DIR + NTP + '/' + ds + "/"
    convert_out_ntp(output_directory + "log.txt")


def convert_output_FOIL(ds):
    output_directory = OUTPUT_DIR + FOIL + '/' + ds + "/"
    convert_out_FOIL(output_directory + "FOIL_out.txt")


def converter_test():
    print("Testing FOIL Converter")
    convert_out_FOIL("../output/existing/FOIL/EVEN/rules.txt")

    print("Testing AMIEP Converter")
    convert_out_AMIE_plus("../output/amiep/CHAIN-S-2/results.txt")

    print("Testing Neural-LP Converter")
    convert_out_neural_lp("../output/existing/Neural-LP/EVEN/rules.txt")

    print("Testing NTP Converter")
    convert_out_ntp("../output/simple/ntp/CHAIN-S-2-2/log.txt")



def test():
    tests = [EXP1]
    for test in tests:
        global DATASETS_DIR
        DATASETS_DIR = DATASETS_BASE_DIR + test +"/"
        global DATA_DIR
        DATA_DIR = DATA_BASE_DIR + test +"/"
        global OUTPUT_DIR
        OUTPUT_DIR = OUTPUT_BASE_DIR + test +"/"
        global DATASETS
        DATASETS = ['RDG-XS-2-0/COMPLETE1']
        #DATASETS = []
        #datasets = [str(f) for f in os.listdir(DATASETS_DIR) if
         #           not str(f).startswith('datasets') and not str(f).startswith('.') and not str(f).startswith(
         #               'test') and not str(f).startswith('README') and str(f) == "CHAIN-XS-2-0"]
        #for dd in datasets:
        #    DATASETS.append(dd + "/INCOMPLETE_NOISE")
        convert_systems_output()
        evaluate_systems()

def experiments_1_evaluation():
    tests = [EXP1]
    for test in tests:
        global DATASETS_DIR
        DATASETS_DIR = DATASETS_BASE_DIR + test+"/"
        global DATA_DIR
        DATA_DIR = DATA_BASE_DIR + test+"/"
        global OUTPUT_DIR
        OUTPUT_DIR = OUTPUT_BASE_DIR + test+"/"
        global DATASETS
        DATASETS = []
        datasets = [str(f) for f in os.listdir(DATASETS_DIR) if
                    not str(f).startswith('datasets') and not str(f).startswith('.') and not str(f).startswith(
                        'test') and not str(f).startswith('README')]
        for dd in datasets:
            DATASETS.append(dd+"/COMPLETE1")
            DATASETS.append(dd+"/INCOMPLETE1")
            DATASETS.append(dd+"/INCOMPLETE_NOISE1")
        convert_systems_output()
        evaluate_systems()

def experiments_2_evaluation():
    tests = [EXP2]
    for test in tests:
        global DATASETS_DIR
        DATASETS_DIR = DATASETS_BASE_DIR + test+"/"
        global DATA_DIR
        DATA_DIR = DATA_BASE_DIR + test+"/"
        global OUTPUT_DIR
        OUTPUT_DIR = OUTPUT_BASE_DIR + test+"/"
        global DATASETS
        DATASETS = []
        datasets = [str(f) for f in os.listdir(DATASETS_DIR) if
                    not str(f).startswith('datasets') and not str(f).startswith('.') and not str(f).startswith(
                        'test') and not str(f).startswith('README') ]
        for dd in datasets:
            DATASETS.append(dd + "/INCOMPLETE_NOISE2")
        convert_systems_output()
        evaluate_systems()

def evaluate_specific(experiment, datasets):
    if experiment == 1:
        test = EXP1
    elif experiment == 2:
        test = EXP2
    global DATASETS_DIR
    DATASETS_DIR = DATASETS_BASE_DIR + test+"/"
    global DATA_DIR
    DATA_DIR = DATA_BASE_DIR + test+"/"
    global OUTPUT_DIR
    OUTPUT_DIR = OUTPUT_BASE_DIR + test+"/"
    global DATASETS
    DATASETS = []
    for dd in datasets:
        if experiment == 1:
            DATASETS.append(dd + "/COMPLETE1")
            DATASETS.append(dd + "/INCOMPLETE1")
            DATASETS.append(dd + "/INCOMPLETE_NOISE1")
        elif experiment == 2:
            DATASETS.append(dd + "/INCOMPLETE_NOISE2")
    convert_systems_output()
    evaluate_systems()

if __name__ == '__main__'():
    #test()
    # converter_test
    # USE this for grid search of optimal parameters
    # grid_search()
    #experiments_evaluation_old()
    experiments_1_evaluation()
    experiments_2_evaluation()
    #data = ["CHAIN-XS-2"]
    #evaluate_specific(2, data)

