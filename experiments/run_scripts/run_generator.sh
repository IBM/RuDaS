#!/bin/bash

# bash should halt the script in case any of them returns a non-true exit code. = error
set -e

#conda activate rudas
#cd /~/RuDaS/src/
cd /Users/veronika.thost/Desktop/git/RuDaS/src/

SIZES=(0 1 2)
CATEGORIES=(2 4 6)
MAXDEPTHS=(1 2 3)
OWAS=(0 0.1 0.2 0.3 0.4)
NOISES=(0 0.2 0.4)
MISSINGS=(0 0.15 0.3)

for SIZE in ${SIZES[*]}; do
    for CATEGORY in ${CATEGORIES[*]}; do
        for MAXDEPTH in ${MAXDEPTHS[*]}; do
            for OWA in ${OWAS[*]}; do
                for NOISE in ${NOISES[*]}; do
                    for MISSING in ${MISSINGS[*]}; do
                        python generator.py --path='../datasets/new/' --size=$SIZE --category=$CATEGORY \
                        --maxdepth=$MAXDEPTH --owa=$OWA --noise=$NOISE --missing=$MISSING --test=$OWA
                    done
                done
            done
        done
    done
done

exit 0