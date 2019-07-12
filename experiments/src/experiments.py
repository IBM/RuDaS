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
    # print('datasets (copy and paste into run.sh): ', " ".join(DATASETS))

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

        ##FOIL
        srcpath_FOIL = DATASETS_DIR + ds + '/'
        _, rules_FOIL, _, _ = eval_utils.parseFiles_general(None, srcpath_FOIL + RULE_FILE)


        ## FOIL
        dstpath_FOIL = DATA_DIR + FOIL + '/' + ds + '/'
        trpath_FOIL = srcpath_FOIL + TRAIN + '.txt'
        # vpath_FOIL = srcpath_FOIL + VALID + '.txt'
        vpath_FOIL = None
        outFile_FOIL = OUTPUT_DIR + FOIL + '/' + ds + "/results4eval.txt"
        eval_utils.preprocess_FOIL(trpath_FOIL, vpath_FOIL, dstpath_FOIL)


''' reformatted
def run_experiments_FOIL():
    print("running FOIL")
    #done in setup_experiments
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
'''




if __name__ == '__main__':
    print(os.getcwd())
    tests = [SIMPLE_CWA]
        #[SIMPLE, SIMPLE_OWA, SIMPLE_CWA, EXISTING, BINARY, GENERAL, NEW]

    for test in tests:

        DATASETS_DIR = DATASETS_BASE_DIR + test
        DATA_DIR = DATA_BASE_DIR + test
        OUTPUT_DIR = OUTPUT_BASE_DIR + test

        DATASETS = [str(f) for f in os.listdir(DATASETS_DIR) if
                    not str(f).startswith('datasets') and not str(f).startswith('.') and not str(f).startswith(
                        'test') and not str(f).startswith('README')]

        write_run_script(test," ".join(DATASETS))

        setup_experiments()
        pass
