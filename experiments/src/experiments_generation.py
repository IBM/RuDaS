import os
import shutil
from constants import *
from utils import *
from exp_utils import *
# TODO
# write sh file var dynamically - use var for list



def write_run_script(test_name, datasets_string):
    p = os.getcwd() + "/../run_scripts/run_template_1.sh"
    print(p)
    with open(p, 'r') as f:#./run_scripts/run_template_1.sh '/Users/veronika.thost/Desktop/git/RuDaS/experiments/run_scripts/run_template_1.sh'
        ls = f.readlines()
    datasets_complete = ""
    for dataset in datasets_string.split(" "):
        datasets_complete += dataset+"/COMPLETE "
    datasets_incomplete = ""
    for dataset in datasets_string.split(" "):
        datasets_incomplete += dataset + "/INCOMPLETE "
    datasets_incomplete_noise = ""
    for dataset in datasets_string.split(" "):
        datasets_incomplete_noise += dataset + "/INCOMPLETE_NOISE "
    # COMPLETE
    with open(os.getcwd() +'/../run_scripts/run_'+test_name+'_complete.sh', 'w') as f:
        f.writelines(ls[0:2])
        f.write(ls[2].replace("VAR-NAMES", datasets_complete))
        f.write(ls[3].replace("VAR-TESTS", test_name))
        f.writelines(ls[4:])
    # INCOMPLETE
    with open(os.getcwd() + '/../run_scripts/run_' + test_name +'_incomplete.sh', 'w') as f:
        f.writelines(ls[0:2])
        f.write(ls[2].replace("VAR-NAMES", datasets_incomplete))
        f.write(ls[3].replace("VAR-TESTS", test_name))
        f.writelines(ls[4:])
    # INCOMPLETE + NOISE
    with open(os.getcwd() + '/../run_scripts/run_' + test_name + '_incomplete_noise.sh', 'w') as f:
        f.writelines(ls[0:2])
        f.write(ls[2].replace("VAR-NAMES", datasets_incomplete_noise))
        f.write(ls[3].replace("VAR-TESTS", test_name))
        f.writelines(ls[4:])

def write_run_script2(test_name, datasets_string):
    p = os.getcwd() + "/../run_scripts/run_template_2.sh"
    print(p)
    datasets_incomplete_noise = ""
    for dataset in datasets_string.split(" "):
        datasets_incomplete_noise += dataset + "/INCOMPLETE_NOISE "
    with open(p, 'r') as f:#./run_scripts/run_template_2.sh '/Users/veronika.thost/Desktop/git/RuDaS/experiments/run_scripts/run_template_2.sh'
        ls = f.readlines()
    with open(os.getcwd() +'/../run_scripts/run_'+test_name+'_2.sh', 'w') as f:
        f.writelines(ls[0:2])
        f.write(ls[2].replace("VAR-NAMES", datasets_incomplete_noise))
        f.write(ls[3].replace("VAR-TESTS", test_name))
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
                path1 = dir + s + '/' + ds + '/COMPLETE'
                path2 = dir + s + '/' + ds + '/INCOMPLETE'
                path3 = dir + s + '/' + ds + '/INCOMPLETE_NOISE'
                if os.path.exists(path1):
                    shutil.rmtree(path1, ignore_errors=True)
                if os.path.exists(path2):
                    shutil.rmtree(path2, ignore_errors=True)
                if os.path.exists(path3):
                    shutil.rmtree(path3, ignore_errors=True)
                os.makedirs(path1,)
                os.makedirs(path2,)
                os.makedirs(path3,)
    #COMPLETE ------------------------------------------------------------------------

    for ds in DATASETS:
        srcpath = DATASETS_DIR + ds + '/'
        _, rules, _, _ = parseFiles_general(None, srcpath + RULE_FILE)

        ## ntp
        dstpath = DATA_DIR + NTP + '/' + ds + '/COMPLETE' + '/'

        ntpconf = dstpath + 'run.conf'
        src_dst = [('../../other/ntp.conf', ntpconf),
                   (srcpath + TRAIN_FILE_COMPLETE, dstpath + TRAIN + '.nl'),
                   (srcpath + TEST_FILE, dstpath + TEST + '.nl'),
                   # (srcpath + VALID_FILE, dstpath + 'dev.nl'),
                   (srcpath + ENTITIES_FILE, dstpath + ENTITIES + '.txt')]
        for t in src_dst:
            shutil.copyfile(t[0], t[1])

        extract_validation_data(dstpath, '.nl', .15, valid='dev')  # TODO discuss this percentage? is of train!

        write_ntp_rule_template(rules, dstpath + 'rules.nlt')

        replace_in_file(ntpconf, [('$DATAPATH', os.path.abspath(dstpath)),
                                  ('$OUTPUTPATH', os.path.abspath(OUTPUT_DIR + NTP + '/' + ds + '/COMPLETE/')),
                                  ('$SYSTEMSPATH', os.path.abspath(SYSTEMS_DIR + NTP + '/')),
                                  ('$TRAIN', TRAIN), ('$TEST', TEST), ('$NAME', ds + '/COMPLETE')])
        #replace_in_file(SYSTEMS_DIR + NTP + '/ntp/experiments/learn.py',
        #                [('./out/', os.path.abspath(OUTPUT_DIR + NTP + '/COMPLETE') + '/')])  # we have to change the code here :(

        ## neural lp
        dstpath = DATA_DIR + NEURAL_LP + '/' + ds + '/COMPLETE' + '/'

        trpath = dstpath + TRAIN + '.txt'
        tpath = dstpath + TEST + '.txt'
        vpath = dstpath + VALID + '.txt'
        fpath = dstpath + 'facts' + '.txt'
        src_dst = [(srcpath + TRAIN_FILE_COMPLETE, trpath),
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

        dstpath = DATA_DIR + AMIE_PLUS + '/' + ds + '/COMPLETE' + '/'

        trpath = dstpath + TRAIN + '.txt'
        shutil.copyfile(srcpath + TRAIN_FILE_COMPLETE, trpath)
        prolog_to_tsv(trpath)

        # setup_experiments_FOIL()

        ##FOIL
        srcpath_FOIL = DATASETS_DIR + ds + '/'
        _, rules_FOIL, _, _ = parseFiles_general(None, srcpath_FOIL + RULE_FILE)


        ## FOIL
        dstpath_FOIL = DATA_DIR + FOIL + '/' + ds + '/COMPLETE' + '/'
        srpath_FOIL = srcpath_FOIL + TRAIN_FILE_COMPLETE
        # vpath_FOIL = srcpath_FOIL + VALID + '.txt'
        vpath_FOIL = None
        preprocess_FOIL(srpath_FOIL, vpath_FOIL, dstpath_FOIL)

    # INCOMPLETE ------------------------------------------------------------------------
    for ds in DATASETS:
        srcpath = DATASETS_DIR + ds + '/'
        _, rules, _, _ = parseFiles_general(None, srcpath + RULE_FILE)

        ## ntp
        dstpath = DATA_DIR + NTP + '/' + ds + '/INCOMPLETE' + '/'

        ntpconf = dstpath + 'run.conf'
        src_dst = [('../../other/ntp.conf', ntpconf),
                   (srcpath + TRAIN_FILE_INCOMPLETE, dstpath + TRAIN + '.nl'),
                   (srcpath + TEST_FILE, dstpath + TEST + '.nl'),
                   # (srcpath + VALID_FILE, dstpath + 'dev.nl'),
                   (srcpath + ENTITIES_FILE, dstpath + ENTITIES + '.txt')]
        for t in src_dst:
            shutil.copyfile(t[0], t[1])

        extract_validation_data(dstpath, '.nl', .15, valid='dev')  # TODO discuss this percentage? is of train!

        write_ntp_rule_template(rules, dstpath + 'rules.nlt')

        replace_in_file(ntpconf, [('$DATAPATH', os.path.abspath(dstpath)),
                                  ('$OUTPUTPATH', os.path.abspath(OUTPUT_DIR + NTP + '/' + ds + '/INCOMPLETE/')),
                                  ('$SYSTEMSPATH', os.path.abspath(SYSTEMS_DIR + NTP + '/')),
                                  ('$TRAIN', TRAIN), ('$TEST', TEST), ('$NAME', ds)])
        # replace_in_file(SYSTEMS_DIR + NTP + '/ntp/experiments/learn.py',
        #                 [('./out/', os.path.abspath(OUTPUT_DIR + NTP + '/')+ '/')]) # we have to change the code here :(

        ## neural lp
        dstpath = DATA_DIR + NEURAL_LP + '/' + ds + '/INCOMPLETE' + '/'

        trpath = dstpath + TRAIN + '.txt'
        tpath = dstpath + TEST + '.txt'
        vpath = dstpath + VALID + '.txt'
        fpath = dstpath + 'facts' + '.txt'
        src_dst = [(srcpath + TRAIN_FILE_INCOMPLETE, trpath),
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
            while i < c // 4:
                f.write(ls[i])
                i += 1

        with open(fpath, 'w') as f:
            for l in ls[c // 4:]:
                f.write(l)

        # tsv format
        prolog_to_tsv(trpath)
        prolog_to_tsv(fpath)
        prolog_to_tsv(tpath)
        prolog_to_tsv(vpath)

        ## amiep

        dstpath = DATA_DIR + AMIE_PLUS + '/' + ds + '/INCOMPLETE' + '/'

        trpath = dstpath + TRAIN + '.txt'
        shutil.copyfile(srcpath + TRAIN_FILE_INCOMPLETE, trpath)
        prolog_to_tsv(trpath)

        # setup_experiments_FOIL()

        ##FOIL
        srcpath_FOIL = DATASETS_DIR + ds + '/'
        _, rules_FOIL, _, _ = parseFiles_general(None, srcpath_FOIL + RULE_FILE)

        ## FOIL
        dstpath_FOIL = DATA_DIR + FOIL + '/' + ds + '/INCOMPLETE' + '/'
        srpath_FOIL = srcpath_FOIL + TRAIN_FILE_INCOMPLETE
        # vpath_FOIL = srcpath_FOIL + VALID + '.txt'
        vpath_FOIL = None
        preprocess_FOIL(srpath_FOIL, vpath_FOIL, dstpath_FOIL)


    # INCOMPLETE_NOISE ------------------------------------------------------------------------
    for ds in DATASETS:
        srcpath = DATASETS_DIR + ds + '/'
        _, rules, _, _ = parseFiles_general(None, srcpath + RULE_FILE)

        ## ntp
        dstpath = DATA_DIR + NTP + '/' + ds + '/INCOMPLETE_NOISE' + '/'

        ntpconf = dstpath + 'run.conf'
        src_dst = [('../../other/ntp.conf', ntpconf),
                   (srcpath + TRAIN_FILE_INCOMPLETE_NOISE, dstpath + TRAIN + '.nl'),
                   (srcpath + TEST_FILE, dstpath + TEST + '.nl'),
                   # (srcpath + VALID_FILE, dstpath + 'dev.nl'),
                   (srcpath + ENTITIES_FILE, dstpath + ENTITIES + '.txt')]
        for t in src_dst:
            shutil.copyfile(t[0], t[1])

        extract_validation_data(dstpath, '.nl', .15, valid='dev')  # TODO discuss this percentage? is of train!

        write_ntp_rule_template(rules, dstpath + 'rules.nlt')

        replace_in_file(ntpconf, [('$DATAPATH', os.path.abspath(dstpath)),
                                  ('$OUTPUTPATH', os.path.abspath(OUTPUT_DIR + NTP + '/' + ds + '/INCOMPLETE_NOISE/')),
                                  ('$SYSTEMSPATH', os.path.abspath(SYSTEMS_DIR + NTP + '/')),
                                  ('$TRAIN', TRAIN), ('$TEST', TEST), ('$NAME', ds)])
        # replace_in_file(SYSTEMS_DIR + NTP + '/ntp/experiments/learn.py',
        #                 [('./out/', os.path.abspath(OUTPUT_DIR + NTP + '/')+ '/')]) # we have to change the code here :(

        ## neural lp
        dstpath = DATA_DIR + NEURAL_LP + '/' + ds + '/INCOMPLETE_NOISE' + '/'

        trpath = dstpath + TRAIN + '.txt'
        tpath = dstpath + TEST + '.txt'
        vpath = dstpath + VALID + '.txt'
        fpath = dstpath + 'facts' + '.txt'
        src_dst = [(srcpath + TRAIN_FILE_INCOMPLETE_NOISE, trpath),
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
            while i < c // 4:
                f.write(ls[i])
                i += 1

        with open(fpath, 'w') as f:
            for l in ls[c // 4:]:
                f.write(l)

        # tsv format
        prolog_to_tsv(trpath)
        prolog_to_tsv(fpath)
        prolog_to_tsv(tpath)
        prolog_to_tsv(vpath)

        ## amiep

        dstpath = DATA_DIR + AMIE_PLUS + '/' + ds + '/INCOMPLETE_NOISE' + '/'

        trpath = dstpath + TRAIN + '.txt'
        shutil.copyfile(srcpath + TRAIN_FILE_INCOMPLETE_NOISE, trpath)
        prolog_to_tsv(trpath)

        # setup_experiments_FOIL()

        ##FOIL
        srcpath_FOIL = DATASETS_DIR + ds + '/'
        _, rules_FOIL, _, _ = parseFiles_general(None, srcpath_FOIL + RULE_FILE)

        ## FOIL
        dstpath_FOIL = DATA_DIR + FOIL + '/' + ds + '/INCOMPLETE_NOISE' + '/'
        srpath_FOIL = srcpath_FOIL + TRAIN_FILE_INCOMPLETE_NOISE
        # vpath_FOIL = srcpath_FOIL + VALID + '.txt'
        vpath_FOIL = None
        preprocess_FOIL(srpath_FOIL, vpath_FOIL, dstpath_FOIL)


def experiment_1():
    tests = [EXP1]
    for test in tests:
        global DATASETS_DIR
        DATASETS_DIR = DATASETS_BASE_DIR + test + "/"
        global DATA_DIR
        DATA_DIR = DATA_BASE_DIR + test + "/"
        global OUTPUT_DIR
        OUTPUT_DIR = OUTPUT_BASE_DIR + test + "/"
        global DATASETS
        DATASETS = [str(f) for f in os.listdir(DATASETS_DIR) if
                    not str(f).startswith('datasets') and not str(f).startswith('.') and not str(f).startswith(
                        'test') and not str(f).startswith('README')]
        write_run_script(test, " ".join(DATASETS))
        setup_experiments()
        pass

def experiment_2():
    tests = [EXP2]
    for test in tests:
        global DATASETS_DIR
        DATASETS_DIR = DATASETS_BASE_DIR + test + "/"
        global DATA_DIR
        DATA_DIR = DATA_BASE_DIR + test + "/"
        global OUTPUT_DIR
        OUTPUT_DIR = OUTPUT_BASE_DIR + test + "/"
        global DATASETS
        DATASETS = [str(f) for f in os.listdir(DATASETS_DIR) if
                    not str(f).startswith('datasets') and not str(f).startswith('.') and not str(f).startswith(
                        'test') and not str(f).startswith('README')]
        write_run_script2(test, " ".join(DATASETS))
        setup_experiments()
        pass


def experiment_old():
    print(os.getcwd())
    tests = [SIMPLE, SIMPLE_OWA, SIMPLE_CWA, BINARY, GENERAL]
    # also: EXISTING, TEST
    for test in tests:
        global DATASETS_DIR
        DATASETS_DIR = DATASETS_BASE_DIR + test + "/"
        global DATA_DIR
        DATA_DIR = DATA_BASE_DIR + test + "/"
        global OUTPUT_DIR
        OUTPUT_DIR = OUTPUT_BASE_DIR + test + "/"
        global DATASETS
        DATASETS = [str(f) for f in os.listdir(DATASETS_DIR) if
                    not str(f).startswith('datasets') and not str(f).startswith('.') and not str(f).startswith(
                        'test') and not str(f).startswith('README')]
        write_run_script(test, " ".join(DATASETS))
        setup_experiments()
        pass



if __name__ == '__main__':
    # experiment_old()
    experiment_1()
    experiment_2()
