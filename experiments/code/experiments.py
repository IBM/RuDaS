import os
import shutil
from file_utils import *
import eval_utils
import evaluator
from constants import *


# TODO
# write sh file var dynamically - use var for list



def write_run_script(test_name, datasets_string):
    p = os.getcwd() + "/../run_scripts/run_template.sh"
    print(p)
    with open(p, 'r') as f:#./run_scripts/run_template.sh '/Users/veronika.thost/Desktop/git/RuDaS/experiments/run_scripts/run_template.sh'
        ls = f.readlines()

    with open(os.getcwd() +'/../run_scripts/run_'+test_name[:-1]+'.sh', 'w') as f:
        f.writelines(ls[0:2])
        f.write(ls[2].replace("VAR-NAMES", datasets_string))
        f.write(ls[3].replace("VAR-TESTS", test_name[:-1]))
        f.writelines(ls[4:])


# gets path of a train file
def extract_validation_data(path, ext, split, valid=VALID):
    ls = []
    with open(path+TRAIN+ext, 'r') as f:
        ls = f.readlines()

    ls1 = ls[:int((1-split)*len(ls))]
    ls2 = ls[int((1 - split) * len(ls)):]

    with open(path+TRAIN+ext, 'w') as f:
        for l in ls1:
            f.write(l)

    with open(path+valid+ext, 'w') as f:
        for l in ls2:
            f.write(l)

def setup_experiments():
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
        _, rules, _, _ = eval_utils.parseFiles_general(None, srcpath + RULE_FILE)

        ## ntp
        dstpath = DATA_DIR + NTP + '/' + ds + '/'

        ntpconf = dstpath + 'run.conf'
        src_dst = [('../../other/ntp.conf', ntpconf),
                   (srcpath + TRAIN_FILE, dstpath + TRAIN + '.nl'),
                   (srcpath + TEST_FILE, dstpath + TEST + '.nl'),
                   # (srcpath + VALID_FILE, dstpath + 'dev.nl'),
                   (srcpath + ENTITIES_FILE, dstpath + ENTITIES + '.txt')]
        for t in src_dst:
            shutil.copyfile(t[0], t[1])

        extract_validation_data(dstpath, '.nl', .15, valid='dev') # TODO discuss this percentage? is of train!

        write_ntp_rule_template(rules, dstpath + 'rules.nlt')

        replace_in_file(ntpconf, [('$DATAPATH',os.path.abspath(dstpath )),
                                  ('$OUTPUTPATH',os.path.abspath(OUTPUT_DIR + NTP + '/' + ds + '/')),
                                  ('$SYSTEMSPATH', os.path.abspath(SYSTEMS_DIR + NTP + '/')),
                                  ('$TRAIN',TRAIN),('$TEST',TEST),('$NAME',ds)])
        # replace_in_file(SYSTEMS_DIR + NTP + '/ntp/experiments/learn.py',
        #                 [('./out/', os.path.abspath(OUTPUT_DIR + NTP + '/')+ '/')]) # we have to change the code here :(

        ## neural lp
        dstpath = DATA_DIR + NEURAL_LP + '/' + ds + '/'

        trpath = dstpath + TRAIN + '.txt'
        tpath = dstpath + TEST + '.txt'
        vpath = dstpath + VALID + '.txt'
        fpath = dstpath + 'facts' + '.txt'
        src_dst = [(srcpath + TRAIN_FILE, trpath),
                   (srcpath + TEST_FILE, tpath),
                   # (srcpath + VALID_FILE, vpath),
                   (srcpath + RELATIONS_FILE, dstpath + RELATIONS + '.txt'),
                   (srcpath + ENTITIES_FILE, dstpath + ENTITIES + '.txt')]
        for t in src_dst:
            shutil.copyfile(t[0], t[1])

        extract_validation_data(dstpath, '.txt', .15)
        # split original train into neural lp's special facts and train 3:1
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

        # setup_experiments_FOIL()

def setup_experiments_FOIL():
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
        _, rules_FOIL, _, _ = eval_utils.parseFiles_general(None, srcpath_FOIL + RULE_FILE)

        ## FOIL
        dstpath_FOIL = DATA_DIR + FOIL + '/' + ds + '/'
        trpath_FOIL = srcpath_FOIL + TRAIN + '.txt'
        # vpath_FOIL = srcpath_FOIL + VALID + '.txt'
        vpath_FOIL = None # TODO check this is from other file
        outFile_FOIL = OUTPUT_DIR + FOIL + '/' + ds + "/results4eval.txt"
        eval_utils.run_FOIL(trpath_FOIL, vpath_FOIL, dstpath_FOIL, outFile_FOIL)
        print("-----------done---FOIL", ds)

def convert_systems_output():
    print("converting systems outputs...")
    for ds in DATASETS:
        #AMIE PLUS
        convert_output_amiep(ds)
        #NEURAL LP
        convert_output_neural_lp(ds)
        #NTP
        convert_output_ntp(ds)
    print("done converting systems outputs.")

def convert_output_amiep(ds):
    output_directory = OUTPUT_DIR + AMIE_PLUS + '/' + ds + "/"
    eval_utils.convert_out_AMIE_plus(output_directory + "results.txt")

def convert_output_neural_lp(ds):
    output_directory = OUTPUT_DIR + NEURAL_LP + '/' + ds + "/"
    eval_utils.convert_out_neural_lp(output_directory + "rules.txt")

def convert_output_ntp(ds):
    output_directory = OUTPUT_DIR + NTP + '/' + ds + "/"
    eval_utils.convert_out_ntp(output_directory + "log.txt")

def evaluate_systems():
    for ds in DATASETS:
        for sys in SYSTEMS:
            print("starting evaluation", sys, ds)
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
            eval_utils.convert_out_neural_lp(output_directory + "rules.txt", value)
            #NTP
            #output_directory = OUTPUT_DIR + NTP + '/' + ds + "/"
            #utils.convert_out_ntp(output_directory + "log.txt", value)

            print("starting", sys, ds)
            outputFile = RESULTS_BASE_DIR  + sys + '_' + ds + '_' + str(value) + ".txt"
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
            file_name = RESULTS_BASE_DIR + sys +'/'+sys + '_' + ds + '_' + str(val) + ".txt"
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



SIMPLE = 'simple/'
SIMPLE_OWA ='simple-owa/'
SIMPLE_CWA ='simple-cwa/'
EXISTING = 'existing/'
BINARY = 'binary/'
GENERAL = 'general/'
NEW = 'new/'

if __name__ == '__main__':
    print(os.getcwd())
    tests = [#SIMPLE, SIMPLE_OWA, SIMPLE_CWA, EXISTING, BINARY, GENERAL,
         NEW]

    for test in tests:

        DATASETS_DIR = DATASETS_BASE_DIR + test
        DATA_DIR = DATA_BASE_DIR + test
        OUTPUT_DIR = OUTPUT_BASE_DIR + test

        DATASETS = [str(f) for f in os.listdir(DATASETS_DIR) if
                    not str(f).startswith('datasets') and not str(f).startswith('.') and not str(f).startswith(
                        'test') and not str(f).startswith('README')]

        write_run_script(test," ".join(DATASETS))

        setup_experiments()
        # setup_experiments_FOIL()
        # convert_systems_output()
        # evaluate_systems()
        # find_parameters()
        # tune_parameters()
        pass
