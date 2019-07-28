#!/bin/bash

# 1. BEFORE RUNNING
# ACTIVATE conda env
# SET your path below, also datasets path
# 2. CALL with ./run_generator.sh
# (you might have to call before: chmod +x run_generator.sh)


# bash should halt the script in case any of them returns a non-true exit code. = error
set -e

#conda activate rudas
cd /home/veronika/RuDaS/src/

DATASETSDIR='../datasets/new3/'
rm -rf $DATASETSDIR

SIZES=(4)
CATEGORIES=(0)
MAXDEPTHS=(3)
OWAS=(0.3)
NOISES=(0.2)
MISSINGS=(0.15)

for SIZE in ${SIZES[*]}; do
    for CATEGORY in ${CATEGORIES[*]}; do
        for MAXDEPTH in ${MAXDEPTHS[*]}; do
            for OWA in ${OWAS[*]}; do
                for NOISE in ${NOISES[*]}; do
                    for MISSING in ${MISSINGS[*]}; do
                        python generator.py --path=$DATASETSDIR --size=$SIZE --category=$CATEGORY \
                        --maxdepth=$MAXDEPTH --owa=$OWA --noise=$NOISE --missing=$MISSING --test=$OWA
                    done
                done
            done
        done
    done
done

exit 0