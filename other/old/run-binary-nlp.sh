#!/bin/bash

DIR=`dirname $0`
SYSDIR=$DIR/../systems
DATA=$DIR/../data/binary/

for NAME in RDG-XS-2 CHAIN-XS-2 DRDG-XS-2 CHAIN-S-2-0 CHAIN-S-2 RDG-S-2 DRDG-S-2 CHAIN-S-3 RDG-S-3 DRDG-S-3

do
SYSTEM=Neural-LP

source activate $SYSTEM
python $SYSDIR/$SYSTEM/src/main.py --datadir=$DATA/$SYSTEM/$NAME --exps_dir=$DIR/../output/binary/$SYSTEM --exp_name=$NAME > $DIR/../output/binary/$SYSTEM/$NAME/log.txt

#SYSTEM=ntp

#source activate $SYSTEM

#export PYTHONPATH=$PYTHONPATH:$SYSDIR/$SYSTEM
#python $SYSDIR/$SYSTEM/ntp/experiments/learn.py $DATA/$SYSTEM/$NAME/run.conf > $DIR/../output/binary/$SYSTEM/$NAME/log.txt

#SYSTEM=amiep

#ava -jar $SYSDIR/$SYSTEM/amie_plus.jar -mins 3 -minis 3 -minpca 0.25 -oute  $DATA/$SYSTEM/$NAME/train.txt > $DIR/../output/binary/$SYSTEM/$NAME/results.txt

done

exit 0

