
DATASETS_BASE_DIR = '../../datasets/'
DATA_BASE_DIR = '../data/'
OUTPUT_BASE_DIR = '../output/'
RESULTS_BASE_DIR = '../results/'
SYSTEMS_DIR = '../systems/'

# DATASETS_DIR_SIMPLE ='../../datasets/simple/'
# DATASETS_DIR_SIMPLE_OWA ='../../datasets/simple-owa/'
# DATASETS_DIR_SIMPLE_CWA ='../../datasets/simple-cwa/'
# DATASETS_DIR_EXISTING = '../../datasets/existing/'
# DATASETS_DIR_BINARY = '../../datasets/binary/'
# DATASETS_DIR_GENERAL = '../../datasets/general/'
#
# DATA_DIR_SIMPLE = '../data/simple/'
# DATA_DIR_SIMPLE_OWA = '../data/simple-owa/'
# DATA_DIR_SIMPLE_CWA = '../data/simple-cwa/'
# DATA_DIR_EXISTING = '../data/existing/'
# DATA_DIR_BINARY = '../data/binary/'
# DATA_DIR_GENERAL = '../data/general/'
#
# OUTPUT_DIR_SIMPLE = '../output/simple/'
# OUTPUT_DIR_SIMPLE_OWA = '../output/simple-owa/'
# OUTPUT_DIR_SIMPLE_CWA = '../output/simple-cwa/'
# OUTPUT_DIR_EXISTING = '../output/existing/'
# OUTPUT_DIR_BINARY = '../output/binary/'
# OUTPUT_DIR_GENERAL = '../output/general/'
#

# RESULTS_DIR_SIMPLE = '../results/simple/'
# RESULTS_DIR_SIMPLE_OWA = '../results/simple-owa/'
# RESULTS_DIR_SIMPLE_CWA = '../results/simple-cwa/'
# RESULTS_DIR_EXISTING = '../results/existing/'
# RESULTS_DIR_BINARY = '../results/binary/'
# RESULTS_DIR_GENERAL = '../results/general/'


TEST = 'test'
TRAIN = 'train'
VALID = 'valid'
RULES = 'rules'
RELATIONS = 'relations'
ENTITIES = 'entities'
EVAL_SUPPORT = 'eval-support'
EVAL_CONSEQS = 'eval-conseqs'
EXP1 = 'exp1'
EXP2 = 'exp2'
SIMPLE = 'simple'
SIMPLE_OWA ='simple-owa'
SIMPLE_CWA ='simple-cwa'
EXISTING = 'existing'
BINARY = 'binary'
GENERAL = 'general'

COMPL = '-cw'
INCOMPL = '-ow'
INCOMPL_N = ''


EXT = '.txt'
RULE_FILE = RULES+EXT
TEST_FILE = TEST+EXT

TRAIN_FILE = TRAIN+EXT
TRAIN_FILE_COMPLETE = TRAIN+COMPL+EXT
TRAIN_FILE_INCOMPLETE = TRAIN+INCOMPL+EXT
TRAIN_FILE_INCOMPLETE_NOISE = TRAIN+INCOMPL_N+EXT

VALID_FILE = VALID+EXT
EVAL_SUPPORT_FILE = EVAL_SUPPORT+EXT
EVAL_CONSEQS_FILE = EVAL_CONSEQS+EXT
RELATIONS_FILE = RELATIONS+EXT
ENTITIES_FILE = ENTITIES+EXT

NTP = 'ntp'
NEURAL_LP = 'Neural-LP'
AMIE_PLUS = 'amiep'
FOIL = 'FOIL'

SYSTEMS = [NEURAL_LP, AMIE_PLUS, FOIL, NTP]
SYSTEMS2 = [FOIL]



