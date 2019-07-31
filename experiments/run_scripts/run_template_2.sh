#!/bin/bash

NAMES=(VAR-NAMES)
TESTS=VAR-TESTS

DIR=`dirname $0`
SYSDIR=$DIR/../systems
DATA=$DIR/../data/$TESTS/

for NAME in ${NAMES[@]}
do


SYSTEM=Neural-LP
echo "Running Neural-LP..."
source activate $SYSTEM
python $SYSDIR/$SYSTEM/src/main.py --datadir=$DATA/$SYSTEM/$NAME --exps_dir=$DIR/../output/$TESTS/$SYSTEM --exp_name=$NAME > $DIR/../output/$TESTS/$SYSTEM/$NAME/log.txt 2> $DIR/../output/$TESTS/$SYSTEM/$NAME/err.txt
echo "done."

SYSTEM=ntp
echo "Running ntp..."
source activate $SYSTEM
export PYTHONPATH=$PYTHONPATH:$SYSDIR/$SYSTEM
python $SYSDIR/$SYSTEM/ntp/experiments/learn.py $DATA/$SYSTEM/$NAME/run.conf > $DIR/../output/$TESTS/$SYSTEM/$NAME/log.txt 2> $DIR/../output/$TESTS/$SYSTEM/$NAME/err.txt
mv -v out/$NAME/* $DIR/../output/$TESTS/$SYSTEM/$NAME/
rm -r out/$NAME
echo "done."

SYSTEM=amiep
echo "Running amiep..."
java -jar $SYSDIR/$SYSTEM/amie_plus.jar -mins 3 -minis 3 -minpca 0.25 -oute  $DATA/$SYSTEM/$NAME/train.txt > $DIR/../output/$TESTS/$SYSTEM/$NAME/results.txt 2> $DIR/../output/$TESTS/$SYSTEM/$NAME/err.txt
echo "done."

SYSTEM=FOIL
echo "Running FOIL..."
# ADD "-m 200000" if the max tuples are exceeded
../systems/FOIL/./foil6 -v0 -n <$DATA/$SYSTEM/$NAME/train_FOIL.d >$DIR/../output/$TESTS/$SYSTEM/$NAME/FOIL_out.txt 2> $DIR/../output/$TESTS/$SYSTEM/$NAME/err.txt
echo "done."

done
exit 0

