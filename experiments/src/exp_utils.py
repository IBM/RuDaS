import copy
import os
from logic import *
from exp_utils import *
from utils import *
from constants import *


def prolog_to_tsv(path):
    ls = []
    with open(path, 'r') as f:
        ls = f.readlines()
    with open(path, 'w') as f:
        for l in ls:
            l = l.strip().replace(').', '')
            i = l.index('(')
            if ',' in l:
                f.write(l[i+1:].replace(',', '\t'+l[:i]+'\t')+'\n')
            else:  # DUMMY! (for unary atoms)
                f.write(l[i + 1:]+'\t' + l[:i] + '\t' + l[i + 1:] + '\n')




