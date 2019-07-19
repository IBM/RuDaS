import os
import shutil
from exp_utils import *
from evaluator import *
from constants import *



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
    # testing()
    # USE this for grid search of optimal parameters
    # grid_search()
    experiments_evaluation()

