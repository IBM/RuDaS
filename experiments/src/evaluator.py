'''
Python 3.
'''

from src.logic import *
import copy
from eval_utils import *
from constants import *
import pickle


class Evaluator(object):
    '''
    Parameters
    ----------
    factsFile   :   string
        name of the file that stores the original facts
    rulesFile   :   string
        name of the file that stores the original rules
    herbrand :   boolean
        true if we want to  use this evaluation mathodology, false otherwise
    distance1 :   boolean
        true if we want to  use this evaluation mathodology, false otherwise
    distance2 :   boolean
        true if we want to  use this evaluation mathodology, false otherwise
    Description
    ----------
    provide a measure regarding how similar two logic programs are
    '''

    def __init__(self, original_facts_file, original_rule_file, original_inferred_file, to_evaluate_facts_file, to_evaluate_rules_file):
        #files
        self.original_facts_file_name = original_facts_file
        self.original_rules_file_name = None
        self.original_test_file_name = original_inferred_file
        self.original_rules_file_name = original_rule_file
        self.to_evaluate_facts_file_name = to_evaluate_facts_file
        self.to_evaluate_rules_file_name = to_evaluate_rules_file
        #parsing
        self.name = to_evaluate_facts_file
        [to_evaluate_facts, to_evaluate_rules, to_evaluate_pred, to_evaluate_const] = parseFiles_general(to_evaluate_facts_file, to_evaluate_rules_file)
        [original_facts, original_rules, original_pred, original_const] = parseFiles_general(original_facts_file, original_rule_file)
        self.original_logic_program = LogicProgram(original_facts, original_rules, original_pred, len(original_const))
        if to_evaluate_facts == {}:
            to_evaluate_facts = copy.deepcopy(original_facts)
        self.to_evaluate_logic_program = LogicProgram(to_evaluate_facts, to_evaluate_rules, to_evaluate_pred, len(to_evaluate_const))
        if original_inferred_file:
            test_facts = parseFacts_general(original_inferred_file)[0]
            self.original_logic_program.add_inferred_facts(test_facts)
        #distances
        self.num_possible_ground_atoms = None
        self.herbrand_distance = None
        self.herbrand_accuracy = None
        self.herbrand_score = None
        self.accuracy = None
        self.precision = None #=standard accuracy
        self.recall = None
        self.f1score = None
        # aux_distances
        self.fp = None
        self.fn = None
        self.tp = None
        self.num_fp = None
        self.num_fn = None
        self.num_fp = None
        self.num_tp = None
        self.num_tn = None
        self.num_support = None
        self.fpfntptn = False

    def print_results_terminal(self):
        print("#########")
        print("Comparing:\n-", self.original_facts_file_name, "+", self.original_rules_file_name, "\n-", self.to_evaluate_facts_file_name, "+", self.to_evaluate_rules_file_name)
        print("---------")
        print("Distances computed:")
        if self.herbrand_distance:
            print("- Herbrant distance: ", self.herbrand_distance)
            print("- Herbrant accuracy (classic normalization): ", self.herbrand_accuracy)
            print("- Herbrant score: ", self.herbrand_score)
        if self.accuracy:
            print("- Accuracy: ", self.accuracy)
        if self.precision: #=standard accuracy
            print("- Precision (or Standard confidence): ", self.precision)
        if self.recall:
            print("- Recall: ", self.recall)
        if self.f1score:
            print("- F1-score: ", self.f1score)
        print("#########")


    def save_log_on_file(self, file_name):
        file = open(file_name + ".pickle", 'wb')
        pickle.dump(self, file)
        file.close()
        file = open(file_name+".txt", "w")
        file.write("++++++++++++++++++++++++++++++++++++++++++++\n")
        file.write("Inferred facts: \n")
        file.write("++++++++++++++++++++++++++++++++++++++++++++\n")
        for fact in atoms_tostr(self.to_evaluate_logic_program.inferred_facts):
            file.write(fact+"\n")
        file.write("++++++++++++++++++++++++++++++++++++++++++++\n")
        file.write("Facts to infer: \n")
        file.write("++++++++++++++++++++++++++++++++++++++++++++\n")
        for fact in atoms_tostr(self.original_logic_program.inferred_facts):
            file.write(fact+"\n")
        file.write("++++++++++++++++++++++++++++++++++++++++++++\n")
        file.write("Facts difference: " + str(self.herbrand_distance) + "\n")
        file.write("++++++++++++++++++++++++++++++++++++++++++++\n")
        file.write("Generated pickle file with evaluator instance\n")
        file.write("size union:\t" + str(count_facts_dict(union_dict(self.original_logic_program.inferred_facts, self.to_evaluate_logic_program.inferred_facts))) + "\n")
        file.write("size inf_origin: " + str(count_facts_dict(self.original_logic_program.inferred_facts)) + "\n")
        file.write("size inf_toeval: " + str(count_facts_dict(self.to_evaluate_logic_program.inferred_facts)) + "\n")
        file.close()

    def save_results_on_file(self, file_name):
        file = open(file_name, "w")
        if self.herbrand_distance:
            file.write("Herbrant distance:\t"+str(self.herbrand_distance)+"\n")
            file.write("Herbrant accuracy (classic normalization):\t"+str(self.herbrand_accuracy)+"\n")
            file.write("Herbrant score (new normalization):\t"+str(self.herbrand_score)+"\n")
        if self.accuracy:
            file.write("Accuracy:\t"+str(self.standard_accuracy)+"\n")
        if self.precision: #=standard accuracy
            file.write("Precision:\t"+str(self.precision)+"\n")
        if self.recall:
            file.write("Recall:\t"+str(self.recall)+"\n")
        if self.f1score:
            file.write("F1score:\t"+str(self.f1score)+"\n")
        file.close()

    def compute_fpfntptn(self, predicates):
        if not self.fpfntptn:
            self.fpfntptn = True
            self.to_evaluate_logic_program.ground_program()
            self.original_logic_program.ground_program()
            #buildind fn fp and tp
            self.fn = {}
            self.fp = {}
            self.tp = {}
            original_dict = self.original_logic_program.inferred_facts
            toevaluate_dict = self.to_evaluate_logic_program.inferred_facts
            keys = set(original_dict).union(toevaluate_dict)
            for key in keys:
                list_fn = []
                list_fp = []
                list_tp = []
                list_orig = []
                list_2eval = []
                if key in original_dict:
                    list_orig = original_dict[key]
                if key in toevaluate_dict:
                    list_2eval = toevaluate_dict[key]

                for element in list_orig:
                    if element not in list_2eval:
                        list_fn.append(element)
                    else:
                        list_tp.append(element)
                for element in list_2eval:
                    if element not in list_orig:
                        list_fp.append(element)
                self.fn[key] = list_fn
                self.tp[key] = list_tp
                self.fp[key] = list_fp
            self.fp = remove_aux_pred(self.fp, predicates)
            self.num_fp = float(count_facts_dict(self.fp))
            self.num_fn = float(count_facts_dict(self.fn))
            self.num_tp = float(count_facts_dict(self.tp))
            self.num_support = float(self.original_logic_program.num_original_facts)
            # this can be commented out in the case of auxiliary constants
            assert self.to_evaluate_logic_program.num_constants != self.original_logic_program.num_constants, "ERROR message: the two programs are defined on different constants. this assertion can be commented out in the case of auxiliary constants"
            num_constants = self.original_logic_program.num_constants
            #print(num_constants, len(predicates))
            for predicate in predicates:
                self.num_possible_ground_atoms += num_constants ** predicate.arity
            self.num_tn = float(self.num_possible_ground_atoms - self.num_support - self.num_fn - self.num_fp - self.num_tp)


    def compute_Herbrand(self):
        if len(self.to_evaluate_logic_program.rules) == 0:
            self.herbrand_distance = count_facts_dict(self.original_logic_program.inferred_facts)
            pass
        self.to_evaluate_logic_program.ground_program()
        self.original_logic_program.ground_program()
        predicates_list = []
        for fact_list in self.original_logic_program.original_facts:
            predicates_list.append(self.original_logic_program.original_facts[fact_list][0].predicate)
        for fact_list in self.original_logic_program.inferred_facts:
            predicates_list.append(self.original_logic_program.inferred_facts[fact_list][0].predicate)
        self.compute_fpfntptn(predicates_list)
        print(self.num_possible_ground_atoms)
        self.herbrand_distance = float(self.num_fn + self.num_fp)
        herbrand_normalization = float(self.num_fn + self.num_fp + self.num_tp)
        if self.num_possible_ground_atoms != 0:
            self.herbrand_accuracy = self.herbrand_distance / self.num_possible_ground_atoms
        if herbrand_normalization != 0:
            self.herbrand_score = self.herbrand_distance / herbrand_normalization
        self.herbrand_accuracy = 1 - self.herbrand_accuracy
        self.herbrand_score = 1 - self.herbrand_score

    def compute_precision(self):
        self.compute_fpfntptn()
        self.precision = self.num_tp / (self.num_tp + self.num_fp)

    def compute_accuracy(self):
        self.compute_fpfntptn()
        self.accuracy = (self.num_tp + self.num_tn) / (self.num_tp + self.num_fp + self.num_fp + self.num_fn)

    def compute_recall(self):
        self.compute_fpfntptn()
        self.recall = self.num_tp / (self.num_tp + self.num_fn)

    def compute_f1score(self):
        self.compute_fpfntptn()
        if not self.precision:
            self.compute_precision()
        if not self.recall:
            self.compute_recall()
        self.f1score = (2 * self.recall * self.precision) / (self.recall + self.precision)


def testing():
    print("Testing Evaluator")
    '''
    some test files
    "datasets/examples/c4-ex2-facts"
    "datasets/test/even-facts"
    "datasets/test/countries-facts"
    ###
    IF I have the test.txt file, configuring as follows:
    original_factsFile = "test.txt"
    original_rulesFile = None
    ELIF I have the rules.txt the test.txt could be avoided configuring as follow
    original_factsFile = "train.txt"
    original_rulesFile = "rules.txt"
    '''
    # TEST1
    # comparison without test set
    '''
    print("\n")
    print("+++++++++++++++++++++++")
    print("++TEST 1 - without test set+")
    print("+++++++++++++++++++++++")
    #bad program
    original_factsFile = "../../datasets/test/even-facts"
    original_rulesFile = "../../datasets/test/even-rules"
    toEvaluate_factsFile = "../../datasets/test/even-facts"
    toEvaluate_rulesFile = "../../datasets/test/even-rules_FOIL_result"
    eval = Evaluator(original_factsFile, original_rulesFile, None, toEvaluate_factsFile, toEvaluate_rulesFile)
    eval.compute_Herbrand()
    eval.print_results_terminal()
    ## good program
    original_factsFile = "../../datasets/test/even-facts-ext"
    original_rulesFile = "../../datasets/test/even-rules"
    toEvaluate_factsFile = "../../datasets/test/even-facts-ext"
    toEvaluate_rulesFile = "../../datasets/test/even-rules-ext_FOIL_result"
    eval = Evaluator(original_factsFile, original_rulesFile, None, toEvaluate_factsFile, toEvaluate_rulesFile)
    eval.compute_Herbrand()
    eval.print_results_terminal()
    '''
    # TEST2
    # comparison with test set
    '''
    print("\n")
    print("+++++++++++++++++++++++")
    print("++TEST 2 with test set+")
    print("+++++++++++++++++++++++")
    # bad program
    original_factsFile = "../../datasets/test/even-facts"
    original_rulesFile = "../../datasets/test/even-rules"
    original_testFile = "../../datasets/test/even-test"
    toEvaluate_factsFile = "../../datasets/test/even-facts"
    toEvaluate_rulesFile = "../../datasets/test/even-rules_FOIL_result"
    eval = Evaluator(original_factsFile, original_rulesFile, original_testFile, toEvaluate_factsFile, toEvaluate_rulesFile)
    eval.compute_Herbrand()
    eval.print_results_terminal()
    ## good program
    original_factsFile = "../../datasets/test/even-facts-ext"
    original_rulesFile = "../../datasets/test/even-rules"
    original_testFile = "../../datasets/test/even-test-ext"
    toEvaluate_factsFile = "../../datasets/test/even-facts-ext"
    toEvaluate_rulesFile = "../../datasets/test/even-rules-ext_FOIL_result"
    eval = Evaluator(original_factsFile, original_rulesFile, original_testFile, toEvaluate_factsFile, toEvaluate_rulesFile)
    eval.compute_Herbrand()
    eval.print_results_terminal()
    '''
    # TEST3
    # comparison with example from AMIE PLUS (with test set)
    print("\n")
    print("+++++++++++++++++++++++")
    print("++TEST 3 with AMIEplus+")
    print("+++++++++++++++++++++++")
    original_factsFile = "../../datasets/CHAIN-XS-3/train.txt"
    original_rulesFile = "../../datasets/CHAIN-XS-3/rules.txt"
    original_testFile = "../../datasets/CHAIN-XS-3/test.txt"
    toEvaluate_factsFile = "../../datasets/CHAIN-XS-3/train.txt"
    #convert
    #convert_out_AMIE_plus("experiments/output/test/amiep/CHAIN-XS-3/results.txt")
    #evaluate
    #toEvaluate_rulesFile = "../../datasets/CHAIN-XS-3/rules.txt"
    toEvaluate_rulesFile = "../output/ntp/CHAIN-XS-3/results4eval.txt"
    eval = Evaluator(original_factsFile, original_rulesFile, original_testFile, toEvaluate_factsFile, toEvaluate_rulesFile)
    eval.compute_Herbrand()
    eval.print_results_terminal()


def evaluate_systems():
    for ds in DATASETS:
        for sys in SYSTEMS:
            print("starting evaluation", sys, ds)
            outputFile = OUTPUT_DIR + sys + '/' + ds + "/evaluation.txt"
            logFile = OUTPUT_DIR + sys + '/' + ds + "/evaluation_log"
            original_factsFile = DATASETS_DIR + ds + '/' + EVAL_SUPPORT_FILE
            original_rulesFile = DATASETS_DIR + ds + '/' + RULE_FILE
            original_testFile = DATASETS_DIR + ds + '/' + EVAL_CONSEQS_FILE
            toEvaluate_factsFile = original_factsFile
            toEvaluate_rulesFile = OUTPUT_DIR + sys + '/' + ds + "/results4eval.txt"
            evaluator = Evaluator(original_factsFile, original_rulesFile, original_testFile, toEvaluate_factsFile, toEvaluate_rulesFile)
            evaluator.compute_Herbrand()
            evaluator.compute_accuracy()
            evaluator.compute_precision()
            evaluator.compute_recall()
            evaluator.compute_f1score()
            evaluator.save_results_on_file(outputFile)
            evaluator.save_log_on_file(logFile)
            print("done", sys, ds)
        print("----------done", ds)


def grid_search():
    for sys in [AMIE_PLUS, NEURAL_LP, NTP]:
        find_parameters(sys)
        tune_parameters(sys)


def experiments_evaluation():
    tests = [SIMPLE_CWA]
    # [SIMPLE, SIMPLE_OWA, SIMPLE_CWA, EXISTING, BINARY, GENERAL, NEW]
    for test in tests:
        DATASETS_DIR = DATASETS_BASE_DIR + test
        DATA_DIR = DATA_BASE_DIR + test
        OUTPUT_DIR = OUTPUT_BASE_DIR + test
        DATASETS = [str(f) for f in os.listdir(DATASETS_DIR) if
                    not str(f).startswith('datasets') and not str(f).startswith('.') and not str(f).startswith(
                        'test') and not str(f).startswith('README')]
        convert_systems_output()
        evaluate_systems()


if __name__ == '__main__':
    #testing()
    # USE this for grid search of optimal parameters
    # grid_search()
    experiments_evaluation()

