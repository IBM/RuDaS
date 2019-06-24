'''
Python 3.
'''

from src.logic import *
import copy
from utils import *



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

    def __init__(self, original_facts_file, original_rule_file, original_test_file, to_evaluate_facts_file, to_evaluate_rules_file):
        self.original_facts_file_name = original_facts_file
        self.original_rules_file_name = None
        self.original_test_file_name= original_test_file
        self.original_rules_file_name = original_rule_file
        self.to_evaluate_facts_file_name = to_evaluate_facts_file
        self.to_evaluate_rules_file_name = to_evaluate_rules_file
        self.num_possible_ground_atoms=0
        self.herbrand_normalization=0
        self.name = to_evaluate_facts_file
        [to_evaluate_facts, to_evaluate_rules, to_evaluate_pred, to_evaluate_const] = parseFiles_general(to_evaluate_facts_file, to_evaluate_rules_file)
        [original_facts, original_rules, original_pred, original_const] = parseFiles_general(original_facts_file, original_rule_file)
        self.original_logic_program = LogicProgram(original_facts, original_rules, original_pred, len(original_const))
        if to_evaluate_facts == {}:
            to_evaluate_facts = copy.deepcopy(original_facts)
        self.to_evaluate_logic_program = LogicProgram(to_evaluate_facts, to_evaluate_rules, to_evaluate_pred,
                                                          len(to_evaluate_const))
        if original_test_file:
            test_facts = parseFacts_general(original_test_file)[0]
            self.original_logic_program.add_inferred_facts(test_facts)
        self.herbrand_distance = None
        self.herbrand_accuracy_classic = 1
        self.herbrand_accuracy_normalized = 1
        self.standard_accuracy = 0


    def print_results_terminal(self):
        print("#########")
        print("Comparing:\n-", self.original_facts_file_name, "+", self.original_rules_file_name, "\n-", self.to_evaluate_facts_file_name, "+", self.to_evaluate_rules_file_name)
        print("---------")
        print("Distances computed:")
        if self.herbrand_distance != -1:
            print("- Herbrant distance: ", self.herbrand_distance)
            print("- Herbrant accuracy (classic normalization): ", self.herbrand_distance_classic)
            print("- Herbrant accuracy (new normalization): ", self.herbrand_distance_normalized)
        print("#########")


    def save_log_on_file(self, file_name):
        file = open(file_name, "w")
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
        delta_facts = delta_dict(self.original_logic_program.inferred_facts, self.to_evaluate_logic_program.inferred_facts)
        delta_facts= remove_aux_pred_NLP(delta_facts)
        for fact in atoms_tostr(delta_facts):
            file.write(fact+"\n")
        file.close()


    def save_results_on_file(self, file_name):
        file = open(file_name, "w")
        file.write("Herbrant distance: "+str(self.herbrand_distance)+"\n")
        # '''
        file.write("size union: "+str(count_facts(union_dict(self.original_logic_program.inferred_facts, self.to_evaluate_logic_program.inferred_facts)))+"\n")
        file.write("size inf_origin: "+str(count_facts(self.original_logic_program.inferred_facts))+"\n")
        file.write("size inf_toeval: "+str(count_facts(self.to_evaluate_logic_program.inferred_facts))+"\n")
        # '''
        file.write("Herbrant accuracy (classic normalization): "+str(self.herbrand_distance_classic)+"\n")
        file.write("Herbrant accuracy (new normalization): "+str(self.herbrand_distance_normalized)+"\n")
        file.write("Standard accuracy : "+str(self.standard_accuracy)+"\n")
        file.close()

    def compute_Herbrand_weighted(self):
        # TODO
        pass

    def compute_Herbrand(self):
        if len(self.to_evaluate_logic_program.rules)==0:
            self.herbrand_distance=count_facts(self.original_logic_program.inferred_facts)
            pass
        self.to_evaluate_logic_program.ground_program()
        self.original_logic_program.ground_program()
        predicates_list=[]
        #predicates_list=list(self.original_logic_program.original_facts.keys())+list(self.original_logic_program.inferred_facts.keys())
        for fact_list in self.original_logic_program.original_facts:
            predicates_list.append(self.original_logic_program.original_facts[fact_list][0].predicate)
        for fact_list in self.original_logic_program.inferred_facts:
            predicates_list.append(self.original_logic_program.inferred_facts[fact_list][0].predicate)

        #num_different_facts = 0
        delta_facts = delta_dict(self.original_logic_program.inferred_facts, self.to_evaluate_logic_program.inferred_facts)
        delta_facts= remove_aux_pred_NLP(delta_facts)
        union_facts = union_dict(self.original_logic_program.inferred_facts, self.to_evaluate_logic_program.inferred_facts)
        union_facts = remove_aux_pred_NLP(union_facts)
        num_delta = count_facts(delta_facts)
        num_union = count_facts(union_facts)
        assert self.to_evaluate_logic_program.num_constants >= self.original_logic_program.num_constants, "ERROR message: the two programs are defined on different constants"
        num_constants = self.to_evaluate_logic_program.num_constants
        print(num_constants, len(predicates_list))
        for predicate in predicates_list:
            self.num_possible_ground_atoms += num_constants**predicate.arity
        self.herbrand_distance = float(num_delta)
        self.herbrand_normalization = float(num_union)
        print(self.num_possible_ground_atoms)
        if self.num_possible_ground_atoms != 0:
            self.herbrand_distance_classic = self.herbrand_distance / self.num_possible_ground_atoms
        if self.herbrand_normalization != 0:
            self.herbrand_distance_normalized = self.herbrand_distance / self.herbrand_normalization
        self.herbrand_distance_classic = 1-self.herbrand_distance_classic
        self.herbrand_distance_normalized = 1-self.herbrand_distance_normalized


    def compute_Standard(self):
        if len(self.to_evaluate_logic_program.rules)==0:
            pass
        self.to_evaluate_logic_program.ground_program()
        self.original_logic_program.ground_program()
        predicates_list=[]
        for fact_list in self.original_logic_program.original_facts:
            predicates_list.append(self.original_logic_program.original_facts[fact_list][0].predicate)
        for fact_list in self.original_logic_program.inferred_facts:
            predicates_list.append(self.original_logic_program.inferred_facts[fact_list][0].predicate)
        delta_facts = delta_dict(self.original_logic_program.inferred_facts, self.to_evaluate_logic_program.inferred_facts)
        delta_facts = remove_aux_pred_NLP(delta_facts)

        union_facts = union_dict(self.original_logic_program.inferred_facts, self.to_evaluate_logic_program.inferred_facts)
        union_facts = remove_aux_pred_NLP(union_facts)
        num_delta = count_facts(delta_facts)
        num_union = count_facts(union_facts)
        num_intersection = num_union-num_delta
        if num_intersection!=0 and count_facts(self.to_evaluate_logic_program.inferred_facts)!=0:
            num_inferred = count_facts(remove_aux_pred_NLP(self.to_evaluate_logic_program.inferred_facts))
            self.standard_accuracy = num_intersection / num_inferred


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


if __name__ == '__main__':
    testing()