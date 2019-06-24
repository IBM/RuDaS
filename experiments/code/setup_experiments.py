import os
import shutil
import futils
import utils
import evaluator
import plotly.plotly as py
import plotly.graph_objs as go

import matplotlib.pyplot as plt



# TODO put this in extra file
# write sh file var dynamically - use var for list
# make struct for # SET THIS BEFORE RUNNING below
#  TODO comment below for extracting to new file
DATASETS_DIR_SIMPLE ='../../datasets/simple/'
DATASETS_DIR_SIMPLE_OWA ='../../datasets/simple-owa/'
DATASETS_DIR_SIMPLE_CWA ='../../datasets/simple-cwa/'
DATASETS_DIR_EXISTING = '../../datasets/existing/'
DATASETS_DIR_BINARY = '../../datasets/binary/'
DATASETS_DIR_GENERAL = '../../datasets/general/'

DATA_DIR_SIMPLE = '../data/simple/'
DATA_DIR_SIMPLE_OWA = '../data/simple-owa/'
DATA_DIR_SIMPLE_CWA = '../data/simple-cwa/'
DATA_DIR_EXISTING = '../data/existing/'
DATA_DIR_BINARY = '../data/binary/'
DATA_DIR_GENERAL = '../data/general/'

OUTPUT_DIR_SIMPLE = '../output/simple/'
OUTPUT_DIR_SIMPLE_OWA = '../output/simple-owa/'
OUTPUT_DIR_SIMPLE_CWA = '../output/simple-cwa/'
OUTPUT_DIR_EXISTING = '../output/existing/'
OUTPUT_DIR_BINARY = '../output/binary/'
OUTPUT_DIR_GENERAL = '../output/general/'

RESULTS_DIR = '../results/'
RESULTS_DIR_SIMPLE = '../results/simple/'
RESULTS_DIR_SIMPLE_OWA = '../results/simple-owa/'
RESULTS_DIR_SIMPLE_CWA = '../results/simple-cwa/'
RESULTS_DIR_EXISTING = '../results/existing/'
RESULTS_DIR_BINARY = '../results/binary/'
RESULTS_DIR_GENERAL = '../results/general/'


SYSTEMS_DIR = '../systems/'
TEST='test'
TRAIN='train'
VALID='valid'
RULES='rules'
RELATIONS='relations'
ENTITIES='entities'
EVAL_SUPPORT='eval-support'
EVAL_CONSEQS='eval-conseqs'

EXT='.txt'
RULE_FILE=RULES+EXT
TEST_FILE=TEST+EXT
TRAIN_FILE=TRAIN+EXT
VALID_FILE=VALID+EXT
EVAL_SUPPORT_FILE=EVAL_SUPPORT+EXT
EVAL_CONSEQS_FILE=EVAL_CONSEQS+EXT
RELATIONS_FILE=RELATIONS+EXT
ENTITIES_FILE=ENTITIES+EXT

# TODO fix this
DATASETS_SIMPLE = [str(f) for f in os.listdir(DATASETS_DIR_SIMPLE) if not str(f).startswith('datasets') and not str(f).startswith('.') and not str(f).startswith('test') and not str(f).startswith('README')]
DATASETS_SIMPLE_OWA = [str(f) for f in os.listdir(DATASETS_DIR_SIMPLE_OWA) if not str(f).startswith('datasets') and not str(f).startswith('.') and not str(f).startswith('test') and not str(f).startswith('README')]
DATASETS_SIMPLE_CWA = [str(f) for f in os.listdir(DATASETS_DIR_SIMPLE_CWA) if not str(f).startswith('datasets') and not str(f).startswith('.') and not str(f).startswith('test') and not str(f).startswith('README')]
DATASETS_EXISTING = [str(f) for f in os.listdir(DATASETS_DIR_EXISTING) if not str(f).startswith('datasets') and not str(f).startswith('.') and not str(f).startswith('test') and not str(f).startswith('README')]
DATASETS_BINARY = [str(f) for f in os.listdir(DATASETS_DIR_BINARY) if not str(f).startswith('datasets') and not str(f).startswith('.') and not str(f).startswith('test') and not str(f).startswith('README')]
DATASETS_GENERAL = [str(f) for f in os.listdir(DATASETS_DIR_GENERAL) if not str(f).startswith('datasets') and not str(f).startswith('.') and not str(f).startswith('test') and not str(f).startswith('README')]




NTP = 'ntp'
NEURAL_LP = 'Neural-LP'
AMIE_PLUS = 'amiep'
FOIL = 'FOIL'
#MIT_IBM = 'neural-theorem-proving'
# TODO add others
SYSTEMS = [NTP, NEURAL_LP, AMIE_PLUS, FOIL]
SYSTEMS2 = [FOIL]

# class Systems(Enum):
#     NEURALLP = (50, 100, 3, .2)
#     MITIBM = (101, 1000, 5, .2)
#     FOIL = (1001,10000, 10, .2)
#     NTP = (10001, 100000, 20, .2)
#     AMIE = (100001, 500000, 50, .2)





def replace_in_file(path, replacements):
    with open(path) as f:
        s = f.read()
    with open(path, 'w') as f:
        for r in replacements:
            s = s.replace(r[0], r[1])
        f.write(s)

# TODO put with parse stuff from evaluator in extra file
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


# def parse_rules(path):
#     rules = []
#     # parse rules
#     if rulesFile != None:
#         with open(rulesFile, "r") as f:
#             for line in f:
#                 if (len(line) > 4):
#                     [current_rule, current_predicates, current_constants] = parse_rule_general(line.split('.')[0])
#                     rules.append(current_rule)
#                     for current_predicate in current_predicates:
#                         if current_predicate not in predicates:
#                             predicates.append(current_predicate)
#                     for current_constant in current_constants:
#                         if current_constant not in constants:
#                             constants.append(current_constant)
#     facts_dict = order_facts(facts)
#     return [facts_dict, rules, predicates, constants]




def run_experiments():
    print("setting up experiments")
    print('datasets (copy and paste into run.sh): ', " ".join(DATASETS))

    for s in SYSTEMS:
        for dir in [DATA_DIR, OUTPUT_DIR]:
            path = dir + s
            if os.path.exists(path):
                shutil.rmtree(path, ignore_errors=True)
            for ds in DATASETS:
                path = dir + s + '/' + ds
                if os.path.exists(path):
                    shutil.rmtree(path, ignore_errors=True)
                os.makedirs(path,)

    for ds in DATASETS:
        srcpath = DATASETS_DIR + ds + '/'
        _, rules, _, _ = utils.parseFiles_general(None, srcpath + RULE_FILE)

        ## ntp
        dstpath = DATA_DIR + NTP + '/' + ds + '/'

        ntpconf = dstpath + 'run.conf'
        src_dst = [(DATA_DIR + 'other/ntp.conf', ntpconf),
                   (srcpath + TRAIN_FILE, dstpath + TRAIN + '.nl'),
                   (srcpath + TEST_FILE, dstpath + TEST + '.nl'),
                   (srcpath + VALID_FILE, dstpath + 'dev.nl'),
                   (srcpath + ENTITIES_FILE, dstpath + ENTITIES + '.txt')]

        for t in src_dst:
            shutil.copyfile(t[0], t[1])

        futils.write_ntp_rule_template(rules, dstpath+'rules.nlt')

        replace_in_file(ntpconf, [('$DATAPATH',os.path.abspath(dstpath )),
                                  ('$OUTPUTPATH',os.path.abspath(OUTPUT_DIR + NTP + '/' + ds + '/')),
                                  ('$SYSTEMSPATH', os.path.abspath(SYSTEMS_DIR + NTP + '/')),
                                  ('$TRAIN',TRAIN),('$TEST',TEST),('$NAME',ds)])
        replace_in_file(SYSTEMS_DIR + NTP + '/ntp/experiments/learn.py', [('./out/', os.path.abspath(OUTPUT_DIR + NTP + '/')+ '/')]) # we have to change the code here :(

        ## neural lp
        dstpath = DATA_DIR + NEURAL_LP + '/' + ds + '/'

        trpath = dstpath + TRAIN + '.txt'
        tpath = dstpath + TEST + '.txt'
        vpath = dstpath + VALID + '.txt'
        fpath = dstpath + 'facts' + '.txt'
        src_dst = [(srcpath + TRAIN_FILE, trpath),
                   (srcpath + TEST_FILE, tpath),
                   (srcpath + VALID_FILE, vpath),
                   (srcpath + RELATIONS_FILE, dstpath + RELATIONS + '.txt'),
                   (srcpath + ENTITIES_FILE, dstpath + ENTITIES + '.txt')]

        for t in src_dst:
            shutil.copyfile(t[0], t[1])
        #split original train into facts and train 3:1
        c = 0
        with open(trpath, 'r') as f:
            ls = f.readlines()
            c = len(ls)

        with open(trpath, 'w') as f:
            i = 0
            while i < c//4:
                f.write(ls[i])
                i += 1

        with open(fpath, 'w') as f:
            for l in ls[c//4:]:
                f.write(l)

        # tsv format
        prolog_to_tsv(trpath)
        prolog_to_tsv(fpath)
        prolog_to_tsv(tpath)
        prolog_to_tsv(vpath)

        ## amiep

        dstpath = DATA_DIR + AMIE_PLUS + '/' + ds + '/'

        trpath = dstpath + TRAIN + '.txt'
        shutil.copyfile(srcpath + TRAIN_FILE, trpath)
        prolog_to_tsv(trpath)
        '''
        ## FOIL
        srcpath_FOIL = DATASETS_DIR + ds + '/'
        _, rules_FOIL, _, _ = utils.parseFiles_general(None, srcpath_FOIL + RULE_FILE)
        ## FOIL
        dstpath_FOIL = DATA_DIR + FOIL + '/' + ds + '/'
        trpath_FOIL = srcpath_FOIL + TRAIN + '.txt'
        vpath_FOIL = srcpath_FOIL + VALID + '.txt'
        outFile_FOIL = OUTPUT_DIR + FOIL + '/' + ds + "/results4eval.txt"
        utils.run_FOIL(trpath_FOIL, vpath_FOIL, dstpath_FOIL, outFile_FOIL)
        print("done---FOIL", ds)
        '''

def run_experiments_FOIL():
    print("running FOIL")
    for s in SYSTEMS2:
        for dir in [DATA_DIR, OUTPUT_DIR]:
            path = dir + s
            if os.path.exists(path):
                shutil.rmtree(path, ignore_errors=True)
            for ds in DATASETS:
                path = dir + s + '/' + ds
                if os.path.exists(path):
                    shutil.rmtree(path, ignore_errors=True)
                os.makedirs(path, )

    for ds in DATASETS:
        print("starting---FOIL", ds)
        srcpath_FOIL = DATASETS_DIR + ds + '/'
        _, rules_FOIL, _, _ = utils.parseFiles_general(None, srcpath_FOIL + RULE_FILE)

        ## FOIL
        dstpath_FOIL = DATA_DIR + FOIL + '/' + ds + '/'
        trpath_FOIL = srcpath_FOIL + TRAIN + '.txt'
        vpath_FOIL = srcpath_FOIL + VALID + '.txt'
        outFile_FOIL = OUTPUT_DIR + FOIL + '/' + ds + "/results4eval.txt"
        utils.run_FOIL(trpath_FOIL, vpath_FOIL, dstpath_FOIL, outFile_FOIL)
        print("-----------done---FOIL", ds)

def convert_systems_output():
    print("converting output systems...")
    for ds in DATASETS:
        #AMIE PLUS
        convert_systems_amiep(ds)
        #NEURAL LP
        convert_systems_neural_lp(ds)
        #NTP
        convert_systems_ntp(ds)
    print("done converting output systems.")


def convert_systems_amiep(ds):
    output_directory = OUTPUT_DIR + AMIE_PLUS + '/' + ds + "/"
    utils.convert_out_AMIE_plus(output_directory+"results.txt")



def convert_systems_neural_lp(ds):
    output_directory = OUTPUT_DIR + NEURAL_LP + '/' + ds + "/"
    utils.convert_out_neural_lp(output_directory+"rules.txt")


def convert_systems_ntp(ds):
    output_directory = OUTPUT_DIR + NTP + '/' + ds + "/"
    utils.convert_out_ntp(output_directory+"log.txt")


def evaluate_systems():
    for ds in DATASETS:
        for sys in SYSTEMS:
            print("starting", sys, ds)
            outputFile = OUTPUT_DIR + sys + '/' + ds + "/evaluation.txt"
            logFile = OUTPUT_DIR + sys + '/' + ds + "/evaluation_log.txt"
            original_factsFile = DATASETS_DIR + ds + '/' + EVAL_SUPPORT_FILE
            original_rulesFile = DATASETS_DIR + ds + '/' + RULE_FILE
            original_testFile = DATASETS_DIR + ds + '/' + EVAL_CONSEQS_FILE
            toEvaluate_factsFile = original_factsFile
            toEvaluate_rulesFile = OUTPUT_DIR + sys + '/' + ds + "/results4eval.txt"
            eval = evaluator.Evaluator(original_factsFile, original_rulesFile, original_testFile, toEvaluate_factsFile, toEvaluate_rulesFile)
            eval.compute_Herbrand()
            eval.compute_Standard()
            eval.save_results_on_file(outputFile)
            eval.save_log_on_file(logFile)
            print("done",sys,ds)
        print("----------done",ds)


def find_parameters():
    '''
    AMIEP_PARAMETER = 0.5
    NEURAL_LP_PARAMETER = 0.5
    NTP_PARAMETER = 0.2
    '''
    # [NTP, NEURAL_LP, AMIE_PLUS]
    sys = NEURAL_LP
    AMIEP_PARAMETER_list = [0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9]
    NEURAL_LP_PARAMETER_list = [0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9]
    NTP_PARAMETER_list = [0.0, 0.02, 0.5, 0.1, 0.15, 0.2, 0.25, 0.3, 0.35, 0.4]
    parameter_list = NEURAL_LP_PARAMETER_list
    for value in parameter_list:
        for ds in DATASETS:
            #AMIE PLUS
            #output_directory = OUTPUT_DIR + AMIE_PLUS + '/' + ds + "/"
            #utils.convert_out_AMIE_plus(output_directory + "results.txt", value)
            #NEURAL LP
            output_directory = OUTPUT_DIR + NEURAL_LP + '/' + ds + "/"
            utils.convert_out_neural_lp(output_directory + "rules.txt", value)
            #NTP
            #output_directory = OUTPUT_DIR + NTP + '/' + ds + "/"
            #utils.convert_out_ntp(output_directory + "log.txt", value)

            print("starting", sys, ds)
            outputFile = RESULTS_DIR  + sys + '_' + ds + '_' + str(value) + ".txt"
            original_factsFile = DATASETS_DIR + ds + '/' + EVAL_SUPPORT_FILE
            original_rulesFile = DATASETS_DIR + ds + '/' + RULE_FILE
            original_testFile = DATASETS_DIR + ds + '/' + EVAL_CONSEQS_FILE
            toEvaluate_factsFile = original_factsFile
            toEvaluate_rulesFile = OUTPUT_DIR + sys + '/' + ds + "/results4eval.txt"
            eval = evaluator.Evaluator(original_factsFile, original_rulesFile, original_testFile, toEvaluate_factsFile, toEvaluate_rulesFile)
            eval.compute_Herbrand()
            eval.save_results_on_file(outputFile)
            print("done", sys, ds)
        print("done", ds)


def tune_parameters():
    # [AMIE_PLUS, NEURAL_LP, NTP]
    sys = NEURAL_LP
    AMIEP_PARAMETER_list = [0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9]
    NEURAL_LP_PARAMETER_list = [0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9]
    NTP_PARAMETER_list = [0.0, 0.02, 0.5, 0.1, 0.15, 0.2, 0.25, 0.3, 0.35, 0.4]
    parameter_list = NEURAL_LP_PARAMETER_list
    avarege_values=[0.0]*len(parameter_list)
    for index in range(len(parameter_list)):
        for ds in DATASETS:
            val = parameter_list[index]
            file_name = RESULTS_DIR + sys +'/'+sys + '_' + ds + '_' + str(val) + ".txt"
            with open(file_name, "r") as f:
                for line in f:
                    if 'Herbrant accuracy (new normalization):' in line:
                        val=float(line.split(':')[1])
                        avarege_values[index] += val
                        break
        avarege_values[index]=avarege_values[index]/len(DATASETS)
    max_accuracy=max(avarege_values)
    max_val_index=avarege_values.index(max_accuracy)
    max_parameter = parameter_list[max_val_index] # in case of ties, we use the lowest index
    print("optimal value for ", sys, "=", max_parameter)

def print_evaluation():
    #TODO
    pass

# SET THIS BEFORE RUNNING
DATASETS = DATASETS_SIMPLE_CWA
DATASETS_DIR = DATASETS_DIR_SIMPLE_CWA
DATA_DIR = DATA_DIR_SIMPLE_CWA
OUTPUT_DIR = OUTPUT_DIR_SIMPLE_CWA


if __name__ == '__main__':
    # run_experiments()
    # run_experiments_FOIL()
    # convert_systems_output()
    # evaluate_systems()
    # find_parameters()
    # tune_parameters()
    pass
