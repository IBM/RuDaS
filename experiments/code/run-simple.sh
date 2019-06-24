#!/bin/bash

DIR=`dirname $0`
SYSDIR=$DIR/../systems
DATA=$DIR/../data/simple/

#CHAIN-XS CHAIN-S RDG-S DRDG-S RDG-XS DRDG-XS DRDG-S-2
#CHAIN-S-2 CHAIN-S-3 RDG-XS-2 DRDG-XS-3 CHAIN-XS-3 RDG-XS-3 CHAIN-XS-2 DRDG-XS-2 DRDG-S-2 DRDG-S-3 RDG-S-3 RDG-S-2
for NAME in CHAIN-XS-1 CHAIN-S-2-1 CHAIN-S-2-2
do
SYSTEM=Neural-LP

source activate $SYSTEM
python $SYSDIR/$SYSTEM/src/main.py --datadir=$DATA/$SYSTEM/$NAME --exps_dir=$DIR/../output/simple/$SYSTEM --exp_name=$NAME > $DIR/../output/simple/$SYSTEM/$NAME/log.txt

SYSTEM=ntp

source activate $SYSTEM

export PYTHONPATH=$PYTHONPATH:$SYSDIR/$SYSTEM
python $SYSDIR/$SYSTEM/ntp/experiments/learn.py $DATA/$SYSTEM/$NAME/run.conf > $DIR/../output/simple/$SYSTEM/$NAME/log.txt

#SYSTEM=amiep

#java -jar $SYSDIR/$SYSTEM/amie_plus.jar -mins 3 -minis 3 -minpca 0.25 -oute  $DATA/$SYSTEM/$NAME/train.txt > $DIR/../output/simple/$SYSTEM/$NAME/results.txt

done

exit 0

